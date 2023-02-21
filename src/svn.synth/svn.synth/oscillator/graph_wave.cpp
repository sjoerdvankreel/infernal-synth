#include <svn.synth/shared/config.hpp>
#include <svn.synth/shared/support.hpp>
#include <svn.synth/cv_bank/processor.hpp>
#include <svn.synth/oscillator/processor.hpp>
#include <svn.synth/shared/scratch_space.hpp>
#include <svn.synth/oscillator/graph_wave.hpp>

using namespace svn::base;

namespace svn::synth {

bool 
oscillator_wave_graph::needs_repaint(
  std::int32_t runtime_param) const
{
  std::int32_t begin = topology()->param_start(id());
  return begin <= runtime_param && runtime_param < begin + osc_param::count;
}

bool 
oscillator_wave_graph::dsp_to_plot(graph_plot_input const& input, std::vector<float>& plot)
{
  plot.resize(input.dsp_output->size());
  std::copy(input.dsp_output->begin(), input.dsp_output->end(), plot.begin());
  unipolar_untransform(plot.data(), false, true, static_cast<std::int32_t>(plot.size())); 
  return true;
}

std::int32_t 
oscillator_wave_graph::sample_count(param_value const* state, float sample_rate) const
{
  automation_view automation({ topology(), state, id() });
  float cent = automation.block_real_transform(osc_param::cent);
  std::int32_t oct = automation.block_discrete(osc_param::oct);
  std::int32_t note = automation.block_discrete(osc_param::note);
  float frequency = note_to_frequency(12 * (oct + 1) + note + cent);
  return static_cast<std::int32_t>(std::ceil(sample_rate / frequency * cv_plot_cycles));
}  
 
void 
oscillator_wave_graph::process_dsp_core(block_input const& input, float* output, float sample_rate)
{
  cv_hold_sample glfo_hold[glfo_count] = { };
  cv_hold_sample gcv_hold[master_gcv_count] = { };
  scratch_space scratch(input.data.sample_count);
  cv_bank_state cv_state_(topology(), input.data.sample_count);
  cv_bank_processor cv_bank(topology(), &cv_state_, gcv_hold, glfo_hold, 0.0f, input.data);

  _port.clear();
  _port.resize(input.data.sample_count, 1.0f);
  _audio_out[0].resize(input.data.sample_count);
  _audio_out[1].resize(input.data.sample_count);
  for(std::int32_t i = 0; i < vosc_count; i++)
  {
    _vosc_out[i].resize(stereo_channels, input.data.sample_count);
    _vosc_reset[i].resize(osc_max_voices, input.data.sample_count);
    _vosc_reset_pos[i].resize(osc_max_voices, input.data.sample_count);
    _vosc_reset[i].clear(input.data.sample_count);
    for(std::int32_t c = 0; c < stereo_channels; c++)
      for(std::int32_t s = 0; s < input.data.sample_count; s++)
        _vosc_out[i].buffer(c)[s] = 1.0f; // Neutral for RM/AM.
  }
  
  // Allow access to previous (and self) osc output for RM/AM/Sync.
  float* const* vosc_buffers[vosc_count];
  float* const* vosc_reset_pos_buffers[vosc_count];
  std::int32_t* const* vosc_reset_buffers[vosc_count];
  for (std::int32_t i = 0; i < vosc_count; i++)
  {
    vosc_buffers[i] = _vosc_out[i].buffers();
    vosc_reset_buffers[i] = _vosc_reset[i].buffers();
    vosc_reset_pos_buffers[i] = _vosc_reset_pos[i].buffers();
  }

  oscillator_input osc_in;
  osc_in.block = &input.data;
  osc_in.new_midi = midi_note_c4;
  osc_in.new_midi_start_pos = 0;
  osc_in.portamento = _port.data();
  osc_in.ram_in = vosc_buffers;
  osc_in.sync_reset = vosc_reset_buffers;
  osc_in.sync_reset_pos = vosc_reset_pos_buffers;

  auto oscillator_state_ = std::make_unique<oscillator_state>(sample_rate);
  float* audio_out[stereo_channels] = { _audio_out[0].data(), _audio_out[1].data() };
  oscillator_processor processor(topology(), id().index, sample_rate, input.data, midi_note_c4, 0, oscillator_state_.get());
  processor.process(osc_in, audio_out, cv_bank, scratch);
  if (_right) std::copy(_audio_out[1].data(), _audio_out[1].data() + input.data.sample_count, output);
  else std::copy(_audio_out[0].data(), _audio_out[0].data() + input.data.sample_count, output);
}

} // namespace svn::synth