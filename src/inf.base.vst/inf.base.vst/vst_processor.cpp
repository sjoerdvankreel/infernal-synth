#include <inf.base/shared/support.hpp>
#include <inf.base.vst/vst_processor.hpp>
#include <inf.base.vst/vst_parameter.hpp>
#include <inf.base.vst/vst_support.hpp>
#include <inf.base.vst/vst_io_stream.hpp>
#include <inf.base/topology/topology_info.hpp>

#include <base/source/fstreamer.h>
#include <pluginterfaces/base/ibstream.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/ivstparameterchanges.h>
#include <public.sdk/source/vst/utility/processdataslicer.h>

#include <map>
#include <string>
#include <cassert>
#include <cstdint>

using namespace inf::base;
using namespace Steinberg;
using namespace Steinberg::Vst; 
  
namespace inf::base::vst {

// Don't update output too often.
std::int32_t const 
output_param_update_msec = 200;
 
vst_processor::
vst_processor(std::unique_ptr<inf::base::topology_info>&& topology, FUID controller_id):
_state(topology->params.size()), _changed(topology->params.size()),
_accurate_parameters(static_cast<std::size_t>(topology->input_param_count)),
_output_updated(), _topology(std::move(topology)), _processor()
{
	setControllerClass(controller_id);
  _topology->init_factory_preset(_state.data());
  for(std::int32_t i = 0; i < _topology->input_param_count; i++)
    _accurate_parameters[i].setParamID(_topology->param_index_to_id[i]);

  // For bpm and stream position members.
  processContextRequirements.needTempo();
  processContextRequirements.needContinousTimeSamples();
}

// No 64-bit support.
tresult PLUGIN_API
vst_processor::canProcessSampleSize(int32 symbolic_size)
{ 
  if(symbolic_size == kSample32) return kResultTrue;
  return kResultFalse;
}

tresult PLUGIN_API 
vst_processor::setProcessing(TBool state)
{
  if(_processor) _processor->set_processing(state);
  return AudioEffect::setProcessing(state);
}

// Save parameter values to stream.
tresult PLUGIN_API
vst_processor::getState(IBStream* state)
{
  if(state == nullptr) return kResultFalse;
  IBStreamer streamer(state, kLittleEndian);
  vst_io_stream stream(&streamer);
  if(!stream.save_processor(*_topology, _state.data())) return kResultFalse;
  return kResultOk;
}

// Load parameter values from stream.
tresult PLUGIN_API 
vst_processor::setState(IBStream* state)
{
  if (state == nullptr) return kResultFalse;
  IBStreamer streamer(state, kLittleEndian);
  vst_io_stream stream(&streamer);
  if(!stream.load_processor(*_topology, _state.data())) return kResultFalse;
  return kResultOk;
}

// This gives us the working (max) block size and sample rate.
// Set up our audio_processor.
tresult PLUGIN_API
vst_processor::setupProcessing(ProcessSetup& setup)
{
  float sample_rate = static_cast<float>(setup.sampleRate);
  std::int32_t max_sample_count = setup.maxSamplesPerBlock;
  _processor = _topology->create_audio_processor(_state.data(), _changed.data(), sample_rate, max_sample_count);
  return AudioEffect::setupProcessing(setup);
}

// 1 stereo audio output, 1 event input, and 1 audio input if fx.
tresult PLUGIN_API
vst_processor::initialize(FUnknown* context)
{
  tresult result = AudioEffect::initialize(context);
  if (result != kResultTrue) return kResultFalse;
  addAudioOutput(STR16("Stereo Out"), SpeakerArr::kStereo);
  addEventInput(STR16("Event In"), _topology->max_note_events);
  if (!_topology->is_instrument()) addAudioInput(STR16("Stereo Out"), SpeakerArr::kStereo);
  return kResultTrue;
}

// 1 stereo audio output, 1 audio input if fx.
tresult PLUGIN_API 
vst_processor::setBusArrangements(
  SpeakerArrangement* inputs, int32 input_count,
  SpeakerArrangement* outputs, int32 output_count)
{
  if (_topology->is_instrument() && input_count != 0) return kResultFalse;
  if (output_count != 1 || outputs[0] != SpeakerArr::kStereo) return kResultFalse;
  if(!_topology->is_instrument() && (input_count != 1 || inputs[0] != SpeakerArr::kStereo)) return kResultFalse;
  return AudioEffect::setBusArrangements(inputs, input_count, outputs, output_count);
}

// Where the magic happens.
tresult PLUGIN_API
vst_processor::process(ProcessData& data)
{
  // For timing.
  std::int64_t new_start_perf_count = performance_counter();

  // Setup current block input.
  block_input& input = _processor->prepare_block(data.numSamples);  
  input.data.bpm = 0.0f;
  input.data.stream_position = -1L;
  input.data.sample_count = data.numSamples;
  if(data.processContext != nullptr)
  {
    input.data.bpm = static_cast<float>(data.processContext->tempo);
    input.data.stream_position = data.processContext->projectTimeSamples;
  }

  // Not running, just update state.
  // Assume no tempo means a test run.
  if (data.numSamples == 0 || data.numOutputs == 0 || input.data.bpm <= 0.0f)
  {
    process_input_parameters(data);
    return kResultOk;
  } 

  // Setup events.
  process_notes(input, data);
  process_automation(input, data);

  // Process current block.
  float const* const* audio_in = nullptr;
  if(!_topology->is_instrument()) audio_in = data.inputs[0].channelBuffers32;
  float* const* audio_out = data.outputs[0].channelBuffers32;
  input.channel_count = data.outputs[0].numChannels;
  block_output const& output = _processor->process(
    audio_in, audio_out, _prev_end_perf_count, new_start_perf_count);

  // Sync both in and output param values.
  process_input_parameters(data);
  process_output_parameters(output, data);

  // For timing.
  _prev_end_perf_count = performance_counter();
  return kResultOk;
}

// Translate from vst3 note events.
void
vst_processor::process_notes(block_input& input, ProcessData const& data)
{
  Event event;
  if (data.inputEvents == nullptr) return;
  int32 count = data.inputEvents->getEventCount();
  std::int32_t capacity = _processor->topology()->max_note_events;
  for (std::int32_t i = 0; i < count; i++)
  {
    if (data.inputEvents->getEvent(i, event) == kResultOk)
    {
      if (event.type == Event::kNoteOnEvent || event.type == Event::kNoteOffEvent)
      { 
        if (input.note_count == capacity) break;
        else
        {
          auto& note = input.notes[input.note_count++];
          switch (event.type)
          {
          case Event::kNoteOnEvent:
            note.note_on = true;
            note.midi = event.noteOn.pitch;
            note.velocity = event.noteOn.velocity;
            note.sample_index = event.sampleOffset;
            break;
          case Event::kNoteOffEvent:
            note.note_on = false;
            note.velocity = 0.0f;
            note.midi = event.noteOff.pitch;
            note.sample_index = event.sampleOffset;
            break;
          default:
            assert(false);
            break;
          }
        }
      }
    }
  }
}

// Translate from vst3 automation events using the last queued values.
// We use this to make sure values are synced even if no audio is running.
void
vst_processor::process_input_parameters(ProcessData const& data)
{
  int32 index;
  ParamValue value;
  IParamValueQueue* queue;
  auto changes = data.inputParameterChanges;
  if (changes == nullptr) return;
  int32 count = changes->getParameterCount();

  std::fill(_changed.begin(), _changed.end(), 0);
  _topology->state_check(_state.data());
  for (int32 i = 0; i < count; i++)
    if ((queue = changes->getParameterData(i)) != nullptr)
    {
      ParamID tag = queue->getParameterId();
      if (queue->getPoint(queue->getPointCount() - 1, index, value) == kResultTrue)
      {
        std::int32_t param_index = _topology->param_id_to_index[tag];
        assert(param_index >= 0 && param_index < _topology->params.size());
        _changed[param_index] = 1;
        _state[param_index] = vst_normalized_to_base(_topology.get(), param_index, value);
      }
    }
  _topology->state_check(_state.data());
}

// Translate from vst3 automation events.
void
vst_processor::process_automation(block_input& input, ProcessData const& data)
{
  int32 index;
  ParamValue value;
  IParamValueQueue* queue;
  auto changes = data.inputParameterChanges;  
  if (changes == nullptr) return;

  std::fill(_changed.begin(), _changed.end(), 0);
  for (std::int32_t i = 0; i < changes->getParameterCount(); i++)
    if ((queue = changes->getParameterData(i)) != nullptr)
    {
      ParamID tag = queue->getParameterId();
      std::int32_t param_index = _topology->param_id_to_index[tag];
      assert(param_index >= 0 && param_index < _topology->params.size());
      if (!_topology->params[param_index].descriptor->data.is_continuous())
      {
        // Per-block automation.
        if (queue->getPoint(queue->getPointCount() - 1, index, value) == kResultTrue)
        {
          _changed[param_index] = 1;
          _state[param_index] = vst_normalized_to_base(_topology.get(), param_index, value);
        }
      } 
      // Fixed should always be set at the default value (done by base).
      else if(_topology->params[param_index].descriptor->data.kind != param_kind::fixed)
      {
        // Per-sample automation.
        _changed[param_index] = 1;
        _accurate_parameters[param_index].setValue(_state[param_index].real);
        _accurate_parameters[param_index].beginChanges(queue);
        for (std::int32_t s = 0; s < data.numSamples; s++)
          input.continuous_automation_raw[param_index][s] = _accurate_parameters[param_index].advance(1);
        _accurate_parameters[param_index].endChanges();
      }
    }
}

// Translate to vst3 automation events.
void
vst_processor::process_output_parameters(
  block_output const& output, ProcessData& data)
{
  int32 index;
  if (!data.outputParameterChanges) return;
  if (data.processContext == nullptr) return;

  // Don't update too often.
  auto now = std::chrono::system_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - _output_updated);
  if(duration.count() < output_param_update_msec) return;
  _output_updated = now;

  std::int32_t input_count = _processor->topology()->input_param_count;
  for (std::int32_t p = 0; p < _processor->topology()->output_param_count; p++)
  { 
    ParamID tag = _topology->param_index_to_id[input_count + p];
    IParamValueQueue* queue = data.outputParameterChanges->addParameterData(tag, index);
    if (queue == nullptr) continue;
    double normalized = base_to_vst_normalized(_topology.get(), _topology->input_param_count + p, output.block_automation_raw[input_count + p]);
    queue->addPoint(0, normalized, index);
  }
}

} // namespace inf::base::vst