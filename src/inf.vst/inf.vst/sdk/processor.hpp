#ifndef INF_VST_SDK_PROCESSOR_HPP
#define INF_VST_SDK_PROCESSOR_HPP

#include <inf.base/processor/state.hpp>
#include <inf.base/topology/topology_info.hpp>
#include <inf.base/processor/audio_processor.hpp>

#include <pluginterfaces/vst/ivstevents.h>
#include <public.sdk/source/vst/vstaudioeffect.h>
#include <public.sdk/source/vst/utility/sampleaccurate.h>

#include <chrono>
#include <memory>
#include <vector>

namespace inf::vst {

// Vst3 processor wrapping inf::base::audio_processor.
class vst_processor : 
public Steinberg::Vst::AudioEffect
{
private:
  using FUID = Steinberg::FUID;
  using TBool = Steinberg::TBool;
  using int32 = Steinberg::int32;
  using tresult = Steinberg::tresult;
  using Event = Steinberg::Vst::Event;
  using IBStream = Steinberg::IBStream;
  using ParamID = Steinberg::Vst::ParamID;
  using ProcessData = Steinberg::Vst::ProcessData;
  using ProcessSetup = Steinberg::Vst::ProcessSetup;
  using Parameter = Steinberg::Vst::SampleAccurate::Parameter;
  using SpeakerArrangement = Steinberg::Vst::SpeakerArrangement;

private:
  // Allow implementation to accurately measure total cpu.
  std::int64_t _prev_end_perf_count = 0;
  // State of all parameters. 
  // This fully defines the audio_processor and thus the plugin state.
  std::vector<inf::base::param_value> _state;
  // Indicates whether any param index changed this block (0/1, since no vector<bool>).
  std::vector<std::int32_t> _changed;
  // For sample accurate.
  std::vector<Parameter> _accurate_parameters;
  // Don't update output too often.
  std::chrono::system_clock::time_point _output_updated;
  // Need topology for parameter dsp bounds etc.
  std::unique_ptr<inf::base::topology_info> _topology;
  // Where it happens. We just translate automation and audio values from/to vst3 format.
  std::unique_ptr<inf::base::audio_processor> _processor;

public:
  tresult PLUGIN_API setState(IBStream* state) override;
	tresult PLUGIN_API getState(IBStream* state) override;
  tresult PLUGIN_API setProcessing(TBool state) override;
  tresult PLUGIN_API process(ProcessData& data) override;
  tresult PLUGIN_API initialize(FUnknown* context) override;
  tresult PLUGIN_API setupProcessing(ProcessSetup& setup) override;
  tresult PLUGIN_API canProcessSampleSize(int32 symbolic_size) override;
  tresult PLUGIN_API setBusArrangements(SpeakerArrangement* inputs, 
    int32 input_count, SpeakerArrangement* outputs, int32 output_count) override;

  vst_processor(std::unique_ptr<inf::base::topology_info>&& topology, FUID controller_id);

private:
  // Translating from/to vst3.
  void process_input_parameters(ProcessData const& data);
  void process_notes(inf::base::block_input& input, ProcessData const& data);
  void process_automation(inf::base::block_input& input, ProcessData const& data);
  void process_output_parameters(inf::base::block_output const& output, ProcessData& data);
};

} // namespace inf::vst
#endif // INF_VST_SDK_PROCESSOR_HPP