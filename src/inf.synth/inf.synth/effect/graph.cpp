#include <inf.synth/shared/config.hpp>
#include <inf.synth/effect/state.hpp>
#include <inf.synth/effect/graph.hpp>
#include <inf.synth/effect/topology.hpp>
#include <inf.synth/effect/processor.hpp>
#include <inf.synth/cv_bank/processor.hpp>
#include <inf.synth/shared/scratch_space.hpp>

#include <memory>

using namespace inf::base;

namespace inf::synth {

// Set up and process a single effect bank.
void
effect_graph_base::process_graph(block_input const& input, float sample_rate)
{
  cv_hold_sample glfo_hold[glfo_count] = { };
  cv_hold_sample gcv_hold[master_gcv_count] = { };
  scratch_space scratch(input.data.sample_count);
  cv_bank_state cv_state_(topology(), input.data.sample_count);
  float* audio_out[2] = { _audio_out[0].data(), _audio_out[1].data() };
  float const* audio_in[2] = { _audio_in[0].data(), _audio_in[1].data() };

  // Set up cv bank.
  cv_bank_processor cv_bank;
  bool global = id().type == part_type::geffect;
  if (global)
  {
    cv_bank = cv_bank_processor(topology(), &cv_state_);
    cv_bank.update_block_params(input.data);
  } else
    cv_bank = cv_bank_processor(topology(), &cv_state_, gcv_hold, glfo_hold, 0.0f, input.data);

  effect_input fx_input;
  fx_input.block = &input.data;
  fx_input.audio_in = audio_in;
  fx_input.new_midi_start_pos = 0;
  fx_input.new_midi = midi_note_c4;

  auto fx_state = std::make_unique<effect_state>(global, sample_rate, input.data.sample_count);
  auto processor = std::make_unique<effect_processor>(topology(), id(), 
    sample_rate, graph_bpm, midi_note_c4, fx_state.get(), input.data.automation);
  processor->process_any(fx_input, audio_out, cv_bank, scratch);
}

} // namespace inf::synth