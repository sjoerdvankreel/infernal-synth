#include <inf.synth/shared/support.hpp>
#include <inf.synth/cv_bank/graph.hpp>
#include <inf.synth/cv_bank/topology.hpp>
#include <inf.synth/cv_bank/processor.hpp>
#include <inf.synth/lfo/processor.hpp>
#include <inf.synth/envelope/processor.hpp>
#include <inf.synth/shared/scratch_space.hpp>

using namespace inf::base;

namespace inf::synth {

static float constexpr vcv_plot_graph_velo = 0.5f;
static std::vector<std::tuple<std::int32_t, std::int32_t, std::int32_t>> const vtarget_table_out
= zip_list_table_init_out(vcv_route_output_counts, vcv_route_output_target_counts, vcv_route_output::count);
static std::vector<std::tuple<std::int32_t, std::int32_t, std::int32_t>> const gtarget_table_out
= zip_list_table_init_out(gcv_route_output_counts, gcv_route_output_target_counts, gcv_route_output::count);

bool
cv_bank_graph::dsp_to_plot(graph_plot_input const& input, std::vector<float>& plot)
{
  plot.resize(input.dsp_output->size());
  std::copy(input.dsp_output->begin(), input.dsp_output->end(), plot.begin());
  return false;
}

std::int32_t
cv_bank_graph::sample_count(param_value const* state, float sample_rate) const
{ 
  automation_view plot_automation(topology(), state, id());
  float length = plot_automation.block_real_transform(cv_plot_param::length);
  if (id().type == part_type::gcv_plot) return static_cast<std::int32_t>(std::ceil(length * cv_graph_rate));

  automation_view env_automation(topology(), state, { part_type::venv, 0 });
  envelope_processor amp_env(topology(), 0, cv_graph_rate, graph_bpm, env_automation);
  return static_cast<std::int32_t>(std::ceil(amp_env.total_key_samples(length)));
}
 
void
cv_bank_graph::process_dsp_core(block_input const& input, float* output, float sample_rate)
{
  cv_hold_sample glfo_hold[glfo_count] = { };
  cv_hold_sample gcv_bi_hold[master_gcv_count] = { };
  cv_hold_sample gcv_uni_hold[master_gcv_count] = { };
  scratch_space scratch(input.data.sample_count);
  cv_bank_state cv_state(topology(), input.data.sample_count);
  automation_view plot_automation = input.data.automation.rearrange_params(id());

  // Setup per-voice lfo's.
  for (std::int32_t i = 0; i < vlfo_count; i++)
  {
    part_id vlfo_id = { part_type::vlfo, i };
    lfo_processor voice_lfo(topology(), vlfo_id, cv_graph_rate, graph_bpm, input.data.automation);
    voice_lfo.process_any(input.data, cv_state.vlfo[i].buffer, scratch);
  }

  // Setup global lfo's.
  for (std::int32_t i = 0; i < glfo_count; i++)
  {
    part_id glfo_id = { part_type::glfo, i };
    lfo_processor global_lfo(topology(), glfo_id, cv_graph_rate, graph_bpm, input.data.automation);
    global_lfo.process_global(input.data, cv_state.glfo[i].buffer, scratch);
    glfo_hold[i].flags = cv_state.glfo[i].buffer.flags;
    glfo_hold[i].value = cv_state.glfo[i].buffer.values[0];
  }

  // Setup master cv.
  for (std::int32_t i = 0; i < master_gcv_count; i++)
  {
    part_id master_id = { part_type::master, 0 };
    automation_view master_automation = input.data.automation.rearrange_params(master_id);
    float value_uni = master_automation.block_real_transform(master_param::gcv1_uni + 2 * i);
    float value_bi = master_automation.block_real_transform(master_param::gcv1_uni + 2 * i + 1);
    std::fill(cv_state.gcv_uni[i].buffer.values, cv_state.gcv_uni[i].buffer.values + input.data.sample_count, value_uni);
    std::fill(cv_state.gcv_bi[i].buffer.values, cv_state.gcv_bi[i].buffer.values + input.data.sample_count, value_bi);
    cv_state.gcv_uni[i].buffer.flags.bipolar = false;
    cv_state.gcv_bi[i].buffer.flags.bipolar = true;
    gcv_uni_hold[i].flags = cv_state.gcv_uni[i].buffer.flags;
    gcv_uni_hold[i].value = cv_state.gcv_uni[i].buffer.values[0];
    gcv_bi_hold[i].flags = cv_state.gcv_bi[i].buffer.flags;
    gcv_bi_hold[i].value = cv_state.gcv_bi[i].buffer.values[0];
  }
  
  // Setup envelopes.
  float length = plot_automation.block_real_transform(cv_plot_param::length);
  std::int32_t release_sample = static_cast<std::int32_t>(std::ceil(length * cv_graph_rate));
  release_sample = std::min(release_sample, input.data.sample_count);
  for (std::int32_t i = 0; i < venv_count; i++)
  {
    envelope_input envelope_in;
    envelope_in.offset = 0;
    envelope_in.retrig_pos = -1;
    envelope_in.block = &input.data;
    envelope_in.release_sample = release_sample;
    envelope_processor envelope(topology(), i, cv_graph_rate, graph_bpm, input.data.automation);
    envelope.process(envelope_in, cv_state.venv[i].buffer);
  }

  // Find out modulation targets.
  float const* const* params;
  std::int32_t target_id = plot_automation.block_discrete(cv_plot_param::target);
  auto param_ids = id().type == part_type::vcv_plot ? vtarget_table_out[target_id] : gtarget_table_out[target_id];
  std::memset(output, 0, input.data.sample_count * sizeof(float));
  if (std::get<0>(param_ids) == -1 || std::get<2>(param_ids) == -1) return;
  std::int32_t rt_part_index = std::get<1>(param_ids);
  std::int32_t cv_route_output_id = std::get<0>(param_ids); 
  std::int32_t cv_route_target = std::get<2>(param_ids);

  auto const& data = id().type == part_type::vcv_plot? cv_bank_data::voice: cv_bank_data::global;
  std::int32_t that_part_type = data.route_output_mapping[cv_route_output_id];
  std::int32_t that_param_index = data.route_output_target_mapping[cv_route_output_id][cv_route_target];

  // Set up cv bank.
  cv_bank_processor cv_bank;
  if(id().type == part_type::vcv_plot)
    cv_bank = cv_bank_processor(topology(), &cv_state, gcv_uni_hold, gcv_bi_hold, glfo_hold, vcv_plot_graph_velo, midi_note_c4, input.data);
  else
  {
    cv_bank = cv_bank_processor(topology(), &cv_state);
    cv_bank.update_block_params(input.data);
  } 

  // Apply modulation.
  cv_bank_input cv_input;
  automation_view automated_view = input.data.automation.rearrange_params({ that_part_type, rt_part_index });
  cv_input.block = &input.data;
  cv_input.automated = &automated_view;
  cv_input.modulation_id = { cv_route_output_id, rt_part_index };
  cv_bank.modulate(cv_input, params);
  for (std::int32_t i = 0; i < input.data.sample_count; i++)
    output[i] = automated_view.from_dsp(that_param_index, params[that_param_index][i]);

  // Env 0 is hardwired to voice amp level (excluding invert and bipolar flags).
  if(id().type == part_type::gcv_plot) return;
  unipolar_untransform(cv_state.venv[0].buffer, input.data.sample_count);
  if(cv_route_output_id == vcv_route_output::voice && cv_route_target == vgcv_route_amp_target::gain)
    for(std::int32_t i = 0; i < input.data.sample_count; i++)
      output[i] *= cv_state.venv[0].buffer.values[i];
}

} // namespace inf::synth