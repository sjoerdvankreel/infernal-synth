#include <inf.synth.ui/ui.hpp>
#include <inf.synth/amp/topology.hpp>
#include <inf.synth/lfo/topology.hpp>
#include <inf.synth/voice/topology.hpp>
#include <inf.synth/synth/topology.hpp>
#include <inf.synth/effect/topology.hpp>
#include <inf.synth/cv_bank/topology.hpp>
#include <inf.synth/envelope/topology.hpp>
#include <inf.synth/oscillator/topology.hpp>
#include <inf.synth/audio_bank/topology.hpp>

using namespace juce;
using namespace inf::base;
using namespace inf::base::ui;

namespace inf::synth::ui {

static std::int32_t part_group_master = 0;
static std::int32_t part_group_audio = 1;
static std::int32_t part_group_cv = 2;

static icon_type
icon_for_osc_basic_type(std::int32_t value)
{
  switch (value)
  {
  case osc_basic_type::saw: return icon_type::saw;
  case osc_basic_type::sine: return icon_type::sine;
  case osc_basic_type::pulse: return icon_type::pulse;
  case osc_basic_type::triangle: return icon_type::tri;
  default: assert(false); return {};
  }
}

static icon_type
icon_for_lfo_basic_type(std::int32_t value)
{
  switch (value)
  {
  case lfo_basic_type::saw: return icon_type::saw;
  case lfo_basic_type::sine: return icon_type::sine;
  case lfo_basic_type::pulse: return icon_type::pulse;
  case lfo_basic_type::triangle: return icon_type::tri;
  default: assert(false); return {};
  }
}

static std::unique_ptr<ui_element>
create_osc_main_group(plugin_controller* controller, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 10, 1);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::on, edit_type::toggle, label_type::label, tooltip_type::off), 1, 0, 4, 1);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::kbd, edit_type::toggle, label_type::label, tooltip_type::off), 5, 0, 4, 1);
  return create_part_group_ui(controller, create_group_label_ui(controller, "Main", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_osc_group(plugin_controller* controller, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 3, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type::vosc, part_index, osc_param::type, edit_type::selector, tooltip_type::label), 0, 0, 2, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type::vosc, part_index, osc_param::type, edit_type::dropdown, tooltip_type::off), 2, 0, 1, 1);
  return create_part_group_ui(controller, create_group_label_ui(controller, "Osc", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_osc_main_grid(plugin_controller* controller, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 2, 1);
  grid->add_cell(create_part_group_container_ui(controller, part_group_audio, create_osc_main_group(controller, part_index)), 0, 0);
  grid->add_cell(create_part_group_container_ui(controller, part_group_audio, create_osc_osc_group(controller, part_index)), 1, 0);
  return grid;
}

static std::unique_ptr<ui_element>
create_osc_pitch_group(plugin_controller* controller, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 3, 1);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::note, edit_type::selector, label_type::value, tooltip_type::label), 0, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::oct, edit_type::selector, label_type::value, tooltip_type::label), 1, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::cent, edit_type::knob, label_type::value, tooltip_type::label), 2, 0);
  return create_part_group_ui(controller, create_group_label_ui(controller, "Pitch", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_am_group(plugin_controller* controller, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 3, 1);
  auto am = grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::am_src, edit_type::selector, label_type::value, tooltip_type::label), 0, 0);
  am->relevant_if(part_id(part_type::vosc, part_index), osc_param::on, false, [](std::int32_t part_index, std::int32_t val) { return part_index > 0; });
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::am_ring, edit_type::knob, label_type::label, tooltip_type::value), 1, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::am_mix, edit_type::knob, label_type::label, tooltip_type::value), 2, 0);
  return create_part_group_ui(controller, create_group_label_ui(controller, "AM", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_sync_group(plugin_controller* controller, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 1, 2);
  auto sync_src = grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::sync_src, edit_type::selector, label_type::value, tooltip_type::label), 0, 0);
  sync_src->relevant_if(part_id(part_type::vosc, part_index), osc_param::type, false, [](std::int32_t part_index, std::int32_t val) { return part_index > 0 && val != osc_type::kps && val != osc_type::noise; });
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::fm, edit_type::knob, label_type::label, tooltip_type::value), 0, 1);
  return create_part_group_ui(controller, create_group_label_ui(controller, "Sync", true), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_unison_group(plugin_controller* controller, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 2, 2);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::uni_voices, edit_type::selector, label_type::value, tooltip_type::label), 0, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::uni_dtn, edit_type::knob, label_type::label, tooltip_type::value), 0, 1);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::uni_sprd, edit_type::knob, label_type::label, tooltip_type::value), 1, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::uni_offset, edit_type::knob, label_type::label, tooltip_type::value), 1, 1);
  return create_part_group_ui(controller, create_group_label_ui(controller, "Unison", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_basic_group(plugin_controller* controller, std::int32_t part_index)
{
  auto outer_grid = create_grid_ui(controller, 1, 5);
  outer_grid->add_cell(create_iconed_param_ui(controller, part_type::vosc, part_index, osc_param::basic_type, edit_type::selector, icon_for_osc_basic_type, tooltip_type::label), 0, 0);
  auto inner_grid = create_grid_ui(controller, 3, 1);
  auto pw = inner_grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::basic_pw, edit_type::hslider, label_type::label, tooltip_type::value, false, 8), 1, 0, 1, 1);
  pw->relevant_if(part_id(part_type::vosc, part_index), osc_param::basic_type, false, [](std::int32_t part_index, std::int32_t val) { return val == osc_basic_type::pulse; });
  outer_grid->add_cell(std::move(inner_grid), 0, 1, 1, 4);
  return create_part_group_ui(controller, create_group_label_ui(controller, "Basic", true), std::move(outer_grid));
}

static std::unique_ptr<ui_element>
create_osc_mix_group(plugin_controller* controller, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 1, 5);
  grid->add_cell(create_iconed_param_ui(controller, part_type::vosc, part_index, osc_param::mix_sine, edit_type::knob, icon_type::sine, tooltip_type::value), 0, 0);
  grid->add_cell(create_iconed_param_ui(controller, part_type::vosc, part_index, osc_param::mix_saw, edit_type::knob, icon_type::saw, tooltip_type::value), 0, 1);
  grid->add_cell(create_iconed_param_ui(controller, part_type::vosc, part_index, osc_param::mix_triangle, edit_type::knob, icon_type::tri, tooltip_type::value), 0, 2);
  grid->add_cell(create_iconed_param_ui(controller, part_type::vosc, part_index, osc_param::mix_pulse, edit_type::knob, icon_type::pulse, tooltip_type::value), 0, 3);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::mix_pw, edit_type::knob, label_type::label, tooltip_type::value), 0, 4);
  return create_part_group_ui(controller, create_group_label_ui(controller, "Mix", true), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_dsf_group(plugin_controller* controller, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 3, 1);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::dsf_parts, edit_type::hslider, label_type::label, tooltip_type::value, false, 5), 0, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::dsf_dist, edit_type::hslider, label_type::label, tooltip_type::value, false, 5), 1, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::dsf_decay, edit_type::hslider, label_type::label, tooltip_type::value, false, 5), 2, 0);
  return create_part_group_ui(controller, create_group_label_ui(controller, "DSF", true), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_kps_group(plugin_controller* controller, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 3, 1);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::kps_filter, edit_type::hslider, label_type::label, tooltip_type::value, false, 5), 0, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::kps_feedback, edit_type::hslider, label_type::label, tooltip_type::value, false, 5), 1, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::kps_stretch, edit_type::hslider, label_type::label, tooltip_type::value, false, 5), 2, 0);
  return create_part_group_ui(controller, create_group_label_ui(controller, "K+S", true), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_noise_group(plugin_controller* controller, std::int32_t part_index)
{
  auto seed_grid = create_grid_ui(controller, 3, 1);
  seed_grid->add_cell(create_param_edit_ui(controller, part_type::vosc, part_index, osc_param::noise_seed, edit_type::knob, tooltip_type::value), 0, 0, 2, 1);
  seed_grid->add_cell(create_param_label_ui(controller, part_type::vosc, part_index, osc_param::noise_seed, label_type::label, Justification::centred), 2, 0, 1, 1);
  auto slider_grid = create_grid_ui(controller, 3, 1);
  slider_grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::noise_color, edit_type::hslider, label_type::label, tooltip_type::value, false, 7), 0, 0);
  slider_grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::noise_x, edit_type::hslider, label_type::label, tooltip_type::value, false, 7), 1, 0);
  slider_grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, part_index, osc_param::noise_y, edit_type::hslider, label_type::label, tooltip_type::value, false, 7), 2, 0);
  auto grid = create_grid_ui(controller, 1, 5);
  grid->add_cell(std::move(seed_grid), 0, 0, 1, 1);
  grid->add_cell(std::move(slider_grid), 0, 1, 1, 4);
  return create_part_group_ui(controller, create_group_label_ui(controller, "Noise", true), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_graph_group(plugin_controller* controller, std::int32_t part_index)
{
  auto result = create_grid_ui(controller, 3, 1);
  result->add_cell(create_part_graph_ui(controller, part_type::vosc, part_index, osc_graph::left, osc_param::on), 0, 0);
  result->add_cell(create_part_graph_ui(controller, part_type::vosc, part_index, osc_graph::spectrum, osc_param::on), 1, 0);
  result->add_cell(create_part_graph_ui(controller, part_type::vosc, part_index, osc_graph::right, osc_param::on), 2, 0);
  return result;
}

static std::unique_ptr<grid_element>
create_oscillator_grid(plugin_controller* controller, std::int32_t part_index)
{
  auto result = create_grid_ui(controller, 4, 8);
  result->add_cell(create_osc_osc_main_grid(controller, part_index), 0, 0, 3, 1);
  result->add_cell(create_part_group_container_ui(controller, part_group_audio, create_osc_pitch_group(controller, part_index)), 0, 1, 3, 1);
  result->add_cell(create_part_group_container_ui(controller, part_group_audio, create_osc_am_group(controller, part_index)), 0, 2, 3, 1);
  result->add_cell(create_part_group_container_ui(controller, part_group_audio, create_osc_sync_group(controller, part_index)), 0, 3, 1, 2);
  result->add_cell(create_part_group_container_ui(controller, part_group_audio, create_osc_unison_group(controller, part_index)), 1, 3, 2, 2);
  auto basic = result->add_cell(create_part_group_container_ui(controller, part_group_audio, create_osc_basic_group(controller, part_index)), 3, 0, 1, 5);
  basic->relevant_if(part_id(part_type::vosc, part_index), osc_param::type, true, [](std::int32_t part_index, std::int32_t val) { return val == osc_type::basic; });
  auto mix = result->add_cell(create_part_group_container_ui(controller, part_group_audio, create_osc_mix_group(controller, part_index)), 3, 0, 1, 5);
  mix->relevant_if(part_id(part_type::vosc, part_index), osc_param::type, true, [](std::int32_t part_index, std::int32_t val) { return val == osc_type::mix; });
  auto dsf = result->add_cell(create_part_group_container_ui(controller, part_group_audio, create_osc_dsf_group(controller, part_index)), 3, 0, 1, 5);
  dsf->relevant_if(part_id(part_type::vosc, part_index), osc_param::type, true, [](std::int32_t part_index, std::int32_t val) { return val == osc_type::dsf; });
  auto kps = result->add_cell(create_part_group_container_ui(controller, part_group_audio, create_osc_kps_group(controller, part_index)), 3, 0, 1, 5);
  kps->relevant_if(part_id(part_type::vosc, part_index), osc_param::type, true, [](std::int32_t part_index, std::int32_t val) { return val == osc_type::kps; });
  auto noise = result->add_cell(create_part_group_container_ui(controller, part_group_audio, create_osc_noise_group(controller, part_index)), 3, 0, 1, 5);
  noise->relevant_if(part_id(part_type::vosc, part_index), osc_param::type, true, [](std::int32_t part_index, std::int32_t val) { return val == osc_type::noise; });
  result->add_cell(create_osc_graph_group(controller, part_index), 0, 5, 4, 3);
  return result;
}

static std::unique_ptr<ui_element>
create_oscillator_selector(plugin_controller* controller)
{
  std::vector<std::unique_ptr<ui_element>> oscillators;
  for(std::int32_t i = 0; i < vosc_count; i++)
    oscillators.emplace_back(create_oscillator_grid(controller, i));
  return create_part_selector_ui(controller, "Osc", part_type::active, active_param::vosc, part_type::vosc, 3, 5, part_group_audio, std::move(oscillators));
}

static std::unique_ptr<ui_element>
create_fx_main_group(plugin_controller* controller, std::int32_t part_type, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 6, 6);
  grid->add_cell(create_labeled_param_ui(controller, part_type, part_index, effect_param::on, edit_type::toggle, label_type::label, tooltip_type::off), 1, 1, 4, 4);
  return create_part_group_ui(controller, create_group_label_ui(controller, "Main", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_fx_fx_group(plugin_controller* controller, std::int32_t part_type, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 6, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::type, edit_type::selector, tooltip_type::label), 0, 0, 2, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::type, edit_type::dropdown, tooltip_type::off), 2, 0, 1, 1);
  auto filter_knob = grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::filter_type, edit_type::selector, tooltip_type::label), 3, 0, 2, 1);
  filter_knob->relevant_if(part_id(part_type, part_index), effect_param::type, true, [](std::int32_t part_index, std::int32_t val) { return val == effect_type::filter; });
  auto filter_label = grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::filter_type, edit_type::dropdown, tooltip_type::off), 5, 0, 1, 1);
  filter_label->relevant_if(part_id(part_type, part_index), effect_param::type, true, [](std::int32_t part_index, std::int32_t val) { return val == effect_type::filter; });
  auto shape_knob = grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::shaper_type, edit_type::selector, tooltip_type::label), 3, 0, 2, 1);
  shape_knob->relevant_if(part_id(part_type, part_index), effect_param::type, true, [](std::int32_t part_index, std::int32_t val) { return val == effect_type::shaper; });
  auto shape_label = grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::shaper_type, edit_type::dropdown, tooltip_type::off), 5, 0, 1, 1);
  shape_label->relevant_if(part_id(part_type, part_index), effect_param::type, true, [](std::int32_t part_index, std::int32_t val) { return val == effect_type::shaper; });
  return create_part_group_ui(controller, create_group_label_ui(controller, "FX", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_fx_filter_stvar_group(plugin_controller* controller, std::int32_t part_type, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 3, 5);
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::flt_stvar_type, edit_type::selector, tooltip_type::label), 0, 0, 2, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::flt_stvar_type, edit_type::dropdown, tooltip_type::off), 2, 0, 1, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::flt_stvar_freq, edit_type::knob, tooltip_type::value), 0, 1, 2, 1);
  grid->add_cell(create_param_label_ui(controller, part_type, part_index, effect_param::flt_stvar_freq, label_type::label, Justification::centred), 2, 1, 1, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::flt_stvar_res, edit_type::knob, tooltip_type::value), 0, 2, 2, 1);
  grid->add_cell(create_param_label_ui(controller, part_type, part_index, effect_param::flt_stvar_res, label_type::label, Justification::centred), 2, 2, 1, 1);
  auto gain_knob = grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::flt_stvar_shlf_gain, edit_type::knob, tooltip_type::value), 0, 3, 2, 1);
  gain_knob->relevant_if(part_id(part_type, part_index), effect_param::flt_stvar_type, false, [](std::int32_t part_index, std::int32_t val) { return val == effect_flt_stvar_type::lsh || val == effect_flt_stvar_type::hsh || val == effect_flt_stvar_type::bll; });
  auto gain_label = grid->add_cell(create_param_label_ui(controller, part_type, part_index, effect_param::flt_stvar_shlf_gain, label_type::label, Justification::centred), 2, 3, 1, 1);
  gain_label->relevant_if(part_id(part_type, part_index), effect_param::flt_stvar_type, false, [](std::int32_t part_index, std::int32_t val) { return val == effect_flt_stvar_type::lsh || val == effect_flt_stvar_type::hsh || val == effect_flt_stvar_type::bll; });
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::flt_stvar_kbd, edit_type::knob, tooltip_type::value), 0, 4, 2, 1);
  grid->add_cell(create_param_label_ui(controller, part_type, part_index, effect_param::flt_stvar_kbd, label_type::label, Justification::centred), 2, 4, 1, 1);
  return create_part_group_ui(controller, create_group_label_ui(controller, "StVar", true), std::move(grid));
}

static std::unique_ptr<ui_element>
create_fx_filter_comb_group(plugin_controller* controller, std::int32_t part_type, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 3, 4);
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::flt_comb_gain_plus, edit_type::knob, tooltip_type::value), 0, 0, 2, 1);
  grid->add_cell(create_param_label_ui(controller, part_type, part_index, effect_param::flt_comb_gain_plus, label_type::label, Justification::centred), 2, 0, 1, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::flt_comb_dly_plus, edit_type::knob, tooltip_type::value), 0, 1, 2, 1);
  grid->add_cell(create_param_label_ui(controller, part_type, part_index, effect_param::flt_comb_dly_plus, label_type::label, Justification::centred), 2, 1, 1, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::flt_comb_gain_min, edit_type::knob, tooltip_type::value), 0, 2, 2, 1);
  grid->add_cell(create_param_label_ui(controller, part_type, part_index, effect_param::flt_comb_gain_min, label_type::label, Justification::centred), 2, 2, 1, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::flt_comb_dly_min, edit_type::knob, tooltip_type::value), 0, 3, 2, 1);
  grid->add_cell(create_param_label_ui(controller, part_type, part_index, effect_param::flt_comb_dly_min, label_type::label, Justification::centred), 2, 3, 1, 1);
  return create_part_group_ui(controller, create_group_label_ui(controller, "Comb", true), std::move(grid));
}

static std::unique_ptr<ui_element>
create_fx_filter_grid(plugin_controller* controller, std::int32_t part_type, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 1, 1);
  auto state_var = grid->add_cell(create_part_group_container_ui(controller, part_group_audio, create_fx_filter_stvar_group(controller, part_type, part_index)), 0, 0);
  state_var->relevant_if(part_id(part_type, part_index), effect_param::filter_type, true, [](std::int32_t part_index, std::int32_t val) { return val == effect_filter_type::state_var; });
  auto comb = grid->add_cell(create_part_group_container_ui(controller, part_group_audio, create_fx_filter_comb_group(controller, part_type, part_index)), 0, 0);
  comb->relevant_if(part_id(part_type, part_index), effect_param::filter_type, true, [](std::int32_t part_index, std::int32_t val) { return val == effect_filter_type::comb; });
  return grid;
}

static std::unique_ptr<ui_element>
create_fx_shaper_group(plugin_controller* controller, std::int32_t part_type, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 3, 5);
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::shp_over_order, edit_type::selector, tooltip_type::label), 0, 0, 2, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::shp_over_order, edit_type::dropdown, tooltip_type::off), 2, 0, 1, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::shp_gain, edit_type::knob, tooltip_type::value), 0, 1, 2, 1);
  grid->add_cell(create_param_label_ui(controller, part_type, part_index, effect_param::shp_gain, label_type::label, Justification::centred), 2, 1, 1, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::shp_mix, edit_type::knob, tooltip_type::value), 0, 2, 2, 1);
  grid->add_cell(create_param_label_ui(controller, part_type, part_index, effect_param::shp_mix, label_type::label, Justification::centred), 2, 2, 1, 1);
  auto sheb_grid = create_grid_ui(controller, 3, 2);
  sheb_grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::shp_cheby_terms, edit_type::selector, tooltip_type::value), 0, 0, 2, 1);
  sheb_grid->add_cell(create_param_label_ui(controller, part_type, part_index, effect_param::shp_cheby_terms, label_type::label, Justification::centred), 2, 0, 1, 1);
  auto sheb_sum_knob = sheb_grid->add_cell(create_param_edit_ui(controller, part_type, part_index, effect_param::shp_cheby_sum_decay, edit_type::knob, tooltip_type::value), 0, 1, 2, 1);
  sheb_sum_knob->relevant_if(part_id(part_type, part_index), effect_param::shaper_type, false, [](std::int32_t part_index, std::int32_t val) { return val == effect_shaper_type::cheby_sum; });
  auto sheb_sum_label = sheb_grid->add_cell(create_param_label_ui(controller, part_type, part_index, effect_param::shp_cheby_sum_decay, label_type::label, Justification::centred), 2, 1, 1, 1);
  sheb_sum_label->relevant_if(part_id(part_type, part_index), effect_param::shaper_type, false, [](std::int32_t part_index, std::int32_t val) { return val == effect_shaper_type::cheby_sum; });
  sheb_grid->relevant_if(part_id(part_type, part_index), effect_param::shaper_type, false, [](std::int32_t part_index, std::int32_t val) { return val == effect_shaper_type::cheby_one || val == effect_shaper_type::cheby_sum; });
  grid->add_cell(std::move(sheb_grid), 0, 3, 3, 2);
  return create_part_group_ui(controller, create_group_label_ui(controller, "Shape", true), std::move(grid));
}

static std::unique_ptr<ui_element>
create_fx_graph_group(plugin_controller* controller, std::int32_t part_type, std::int32_t part_index)
{
  auto result = create_grid_ui(controller, 1, 2);
  result->add_cell(create_part_graph_ui(controller, part_type, part_index, effect_graph::graph1, effect_param::type), 0, 0);
  result->add_cell(create_part_graph_ui(controller, part_type, part_index, effect_graph::graph2, effect_param::type), 0, 1);
  return result;
}

static std::unique_ptr<grid_element>
create_fx_grid(plugin_controller* controller, std::int32_t part_type, std::int32_t part_index)
{
  auto result = create_grid_ui(controller, 3, 8);
  result->add_cell(create_part_group_container_ui(controller, part_group_audio, create_fx_main_group(controller, part_type, part_index)), 0, 0, 1, 1);
  result->add_cell(create_part_group_container_ui(controller, part_group_audio, create_fx_fx_group(controller, part_type, part_index)), 1, 0, 2, 1);
  result->add_cell(create_fx_graph_group(controller, part_type, part_index), 0, 1, 2, 7);
  auto filter = result->add_cell(create_fx_filter_grid(controller, part_type, part_index), 2, 1, 1, 7);
  filter->relevant_if(part_id(part_type, part_index), effect_param::type, true, [](std::int32_t part_index, std::int32_t val) { return val == effect_type::filter; });
  auto shaper = result->add_cell(create_part_group_container_ui(controller, part_group_audio, create_fx_shaper_group(controller, part_type, part_index)), 2, 1, 1, 7);
  shaper->relevant_if(part_id(part_type, part_index), effect_param::type, true, [](std::int32_t part_index, std::int32_t val) { return val == effect_type::shaper; });
  return result;
}

static std::unique_ptr<ui_element>
create_fx_selector(plugin_controller* controller, std::int32_t part_type, std::int32_t part_count, std::int32_t selector_param_index)
{
  std::vector<std::unique_ptr<ui_element>> fxs;
  for (std::int32_t i = 0; i < part_count; i++)
    fxs.emplace_back(create_fx_grid(controller, part_type, i));
  return create_part_selector_ui(controller, "FX", part_type::active, selector_param_index, part_type, 1, 7, part_group_audio, std::move(fxs));
}

static std::unique_ptr<ui_element>
create_env_main_group(plugin_controller* controller, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 4, 1);
  auto on = grid->add_cell(create_labeled_param_ui(controller, part_type::venv, part_index, envelope_param::on, edit_type::toggle, label_type::label, tooltip_type::off, part_index == 0), 0, 0);
  on->relevant_if({ part_type::venv, part_index }, envelope_param::on, false, [](std::int32_t part_index, std::int32_t val) { return part_index > 0; });
  grid->add_cell(create_labeled_param_ui(controller, part_type::venv, part_index, envelope_param::synced, edit_type::toggle, label_type::label, tooltip_type::off), 1, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::venv, part_index, envelope_param::invert, edit_type::toggle, label_type::label, tooltip_type::off), 2, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::venv, part_index, envelope_param::bipolar, edit_type::toggle, label_type::label, tooltip_type::off), 3, 0);
  return create_part_group_ui(controller, create_group_label_ui(controller, "Main", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_env_env_group(plugin_controller* controller, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 3, 2);
  grid->add_cell(create_param_edit_ui(controller, part_type::venv, part_index, envelope_param::type, edit_type::selector, tooltip_type::label), 0, 0, 2, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type::venv, part_index, envelope_param::type, edit_type::dropdown, tooltip_type::off), 2, 0, 1, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type::venv, part_index, envelope_param::mode, edit_type::selector, tooltip_type::label), 0, 1, 2, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type::venv, part_index, envelope_param::mode, edit_type::dropdown, tooltip_type::off), 2, 1, 1, 1);
  return create_part_group_ui(controller, create_group_label_ui(controller, "Env", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_env_adr_time_grid(plugin_controller* controller, std::int32_t part_index, 
  std::int32_t prestage, std::int32_t stage1, std::int32_t stage2)
{
  auto grid = create_grid_ui(controller, 1, 24);
  grid->add_cell(create_labeled_param_ui(controller, part_type::venv, part_index, prestage, edit_type::vslider, label_type::label, tooltip_type::value), 0, 1, 1, 6);
  grid->add_cell(create_labeled_param_ui(controller, part_type::venv, part_index, stage1, edit_type::vslider, label_type::label, tooltip_type::value), 0, 9, 1, 6);
  grid->add_cell(create_labeled_param_ui(controller, part_type::venv, part_index, stage2, edit_type::vslider, label_type::label, tooltip_type::value), 0, 17, 1, 6);
  return grid;
}

static std::unique_ptr<ui_element>
create_env_adr_tempo_grid(plugin_controller* controller, std::int32_t part_index,
  std::int32_t prestage, bool prestage_is_synced, std::int32_t stage1, std::int32_t stage2)
{
  auto grid = create_grid_ui(controller, 8, 12);
  if(prestage_is_synced)
  { 
   grid->add_cell(create_param_label_ui(controller, part_type::venv, part_index, prestage, label_type::label, Justification::centred), 1, 2, 2, 2);
   grid->add_cell(create_param_edit_ui(controller, part_type::venv, part_index, prestage, edit_type::dropdown, tooltip_type::off), 1, 4, 2, 6);
  }
  else
    grid->add_cell(create_labeled_param_ui(controller, part_type::venv, part_index, prestage, edit_type::hslider, label_type::label, tooltip_type::value, false, 4), 1, 0, 2, 12);
  grid->add_cell(create_param_label_ui(controller, part_type::venv, part_index, stage1, label_type::label, Justification::centred), 3, 2, 2, 2);
  grid->add_cell(create_param_edit_ui(controller, part_type::venv, part_index, stage1, edit_type::dropdown, tooltip_type::off), 3, 4, 2, 6);
  grid->add_cell(create_param_label_ui(controller, part_type::venv, part_index, stage2, label_type::label, Justification::centred), 5, 2, 2, 2);
  grid->add_cell(create_param_edit_ui(controller, part_type::venv, part_index, stage2, edit_type::dropdown, tooltip_type::off), 5, 4, 2, 6);
  return grid;
}

static std::unique_ptr<ui_element>
create_env_adr_group(
  plugin_controller* controller, std::int32_t part_index, char const* group_name, 
  std::int32_t prestage_time, std::int32_t prestage_tempo, std::int32_t time1, std::int32_t time2, 
  std::int32_t tempo1, std::int32_t tempo2, std::int32_t slope1, std::int32_t slope2, std::int32_t split)
{
  auto upper_grid = create_grid_ui(controller, 4, 4);
  upper_grid->add_cell(create_labeled_param_ui(controller, part_type::venv, part_index, slope1, edit_type::knob, label_type::label, tooltip_type::value), 0, 0, 3, 2);
  upper_grid->add_cell(create_labeled_param_ui(controller, part_type::venv, part_index, slope2, edit_type::knob, label_type::label, tooltip_type::value), 0, 2, 3, 2);
  upper_grid->add_cell(create_labeled_param_ui(controller, part_type::venv, part_index, split, edit_type::hslider, label_type::label, tooltip_type::value, false, 4), 3, 0, 1, 4);
  auto grid = create_grid_ui(controller, 2, 1);
  grid->add_cell(std::move(upper_grid), 0, 0);
  auto time_grid = grid->add_cell(create_env_adr_time_grid(controller, part_index, prestage_time, time1, time2), 1, 0);
  time_grid->relevant_if({ part_type::venv, part_index }, envelope_param::synced, true, [](std::int32_t part_index, std::int32_t val) { return val == 0; });
  auto tempo_grid = grid->add_cell(create_env_adr_tempo_grid(controller, part_index, prestage_tempo == -1? prestage_time: prestage_tempo, prestage_tempo != -1, tempo1, tempo2), 1, 0);
  tempo_grid->relevant_if({ part_type::venv, part_index }, envelope_param::synced, true, [](std::int32_t part_index, std::int32_t val) { return val != 0; });
  return create_part_group_ui(controller, create_group_label_ui(controller, group_name, false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_envelope_grid(plugin_controller* controller, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 3, 7);
  grid->add_cell(create_part_group_container_ui(controller, part_group_cv, create_env_env_group(controller, part_index)), 0, 0, 1, 2);
  grid->add_cell(create_part_group_container_ui(controller, part_group_cv, create_env_main_group(controller, part_index)), 1, 0, 4, 1);
  grid->add_cell(create_part_group_container_ui(controller, part_group_cv, create_env_adr_group(controller, part_index, "DA",
    envelope_param::delay_time, envelope_param::delay_sync, envelope_param::attack1_time, envelope_param::attack2_time, 
    envelope_param::attack1_sync, envelope_param::attack2_sync, envelope_param::attack1_slope, 
    envelope_param::attack2_slope, envelope_param::attack_split_level)), 1, 1, 2, 2);
  grid->add_cell(create_part_group_container_ui(controller, part_group_cv, create_env_adr_group(controller, part_index, "HD",
    envelope_param::hold_time, envelope_param::hold_sync, envelope_param::decay1_time, envelope_param::decay2_time,
    envelope_param::decay1_sync, envelope_param::decay2_sync, envelope_param::decay1_slope, 
    envelope_param::decay2_slope, envelope_param::decay_split_level)), 1, 3, 2, 2);
  grid->add_cell(create_part_group_container_ui(controller, part_group_cv, create_env_adr_group(controller, part_index, "SR",
    envelope_param::sustain_level, -1, envelope_param::release1_time, envelope_param::release2_time,
    envelope_param::release1_sync, envelope_param::release2_sync, envelope_param::release1_slope, 
    envelope_param::release2_slope, envelope_param::release_split_level)), 1, 5, 2, 2);
  grid->add_cell(create_part_group_container_ui(controller, part_group_cv, create_part_graph_ui(controller, part_type::venv, part_index, 0, envelope_param::on)), 0, 2, 1, 5);
  return grid;
}

static std::unique_ptr<ui_element>
create_envelope_selector(plugin_controller* controller)
{
  std::vector<std::unique_ptr<ui_element>> envelopes;
  for (std::int32_t i = 0; i < venv_count; i++)
    envelopes.emplace_back(create_envelope_grid(controller, i));
  return create_part_selector_ui(controller, "Env", part_type::active, active_param::venv, part_type::venv, 2, 5, part_group_cv, std::move(envelopes));
}

static std::unique_ptr<ui_element>
create_lfo_main_group(plugin_controller* controller, std::int32_t part_type, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 5, 1); 
  grid->add_cell(create_labeled_param_ui(controller, part_type, part_index, lfo_param::on, edit_type::toggle, label_type::label, tooltip_type::off), 0, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type, part_index, lfo_param::synced, edit_type::toggle, label_type::label, tooltip_type::off), 1, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type, part_index, lfo_param::single, edit_type::toggle, label_type::label, tooltip_type::off), 2, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type, part_index, lfo_param::invert, edit_type::toggle, label_type::label, tooltip_type::off), 3, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type, part_index, lfo_param::bipolar, edit_type::toggle, label_type::label, tooltip_type::off), 4, 0);
  return create_part_group_ui(controller, create_group_label_ui(controller, "Main", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_lfo_lfo_group(plugin_controller* controller, std::int32_t part_type, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 3, 3);
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, lfo_param::type, edit_type::selector, tooltip_type::label), 0, 0, 2, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, lfo_param::type, edit_type::dropdown, tooltip_type::off), 2, 0, 1, 1);
  auto rate = grid->add_cell(create_param_edit_ui(controller, part_type, part_index, lfo_param::rate, edit_type::knob, tooltip_type::value), 0, 1, 2, 1);
  rate->relevant_if(part_id(part_type, part_index), lfo_param::synced, true, [](std::int32_t part_index, std::int32_t val) { return val == 0; });
  auto rate_label = grid->add_cell(create_param_label_ui(controller, part_type, part_index, lfo_param::rate, label_type::label, Justification::centred), 2, 1, 1, 1);
  rate_label->relevant_if(part_id(part_type, part_index), lfo_param::synced, true, [](std::int32_t part_index, std::int32_t val) { return val == 0; });
  auto tempo = grid->add_cell(create_param_edit_ui(controller, part_type, part_index, lfo_param::tempo, edit_type::knob, tooltip_type::label), 0, 1, 2, 1);
  tempo->relevant_if(part_id(part_type, part_index), lfo_param::synced, true, [](std::int32_t part_index, std::int32_t val) { return val != 0; });
  auto tempo_dropdown = grid->add_cell(create_param_edit_ui(controller, part_type, part_index, lfo_param::tempo, edit_type::dropdown, tooltip_type::off), 2, 1, 1, 1);
  tempo_dropdown->relevant_if(part_id(part_type, part_index), lfo_param::synced, true, [](std::int32_t part_index, std::int32_t val) { return val != 0; });
  grid->add_cell(create_param_edit_ui(controller, part_type, part_index, lfo_param::filter, edit_type::knob, tooltip_type::value), 0, 2, 2, 1);
  grid->add_cell(create_param_label_ui(controller, part_type, part_index, lfo_param::filter, label_type::label, Justification::centred), 2, 2, 1, 1);
  return create_part_group_ui(controller, create_group_label_ui(controller, "LFO", true), std::move(grid));
}

static std::unique_ptr<ui_element>
create_lfo_basic_group(plugin_controller* controller, std::int32_t part_type, std::int32_t part_index)
{
  auto knob_grid = create_grid_ui(controller, 2, 1);
  knob_grid->add_cell(create_param_edit_ui(controller, part_type, part_index, lfo_param::basic_type, edit_type::selector, tooltip_type::label), 0, 0);
  auto icon_grid = create_grid_ui(controller, 3, 3);
  icon_grid->add_cell(create_param_icon_ui(controller, part_type, part_index, lfo_param::basic_type, icon_for_lfo_basic_type), 1, 1, 1, 1);
  knob_grid->add_cell(std::move(icon_grid), 1, 0);
  auto slider_grid = create_grid_ui(controller, 12, 1);
  slider_grid->add_cell(create_labeled_param_ui(controller, part_type, part_index, lfo_param::basic_offset, edit_type::hslider, label_type::label, tooltip_type::value, false, 8), 1, 0, 4, 1);
  auto pw = slider_grid->add_cell(create_labeled_param_ui(controller, part_type, part_index, lfo_param::basic_pw, edit_type::hslider, label_type::label, tooltip_type::value, false, 8), 7, 0, 4, 1);
  pw->relevant_if(part_id(part_type, part_index), lfo_param::basic_type, false, [](std::int32_t part_index, std::int32_t val) { return val == lfo_basic_type::pulse; });
  auto grid = create_grid_ui(controller, 8, 4);
  grid->add_cell(std::move(knob_grid), 1, 0, 6, 1);
  grid->add_cell(std::move(slider_grid), 1, 1, 6, 3);
  return create_part_group_ui(controller, create_group_label_ui(controller, "Basic", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_lfo_random_group(plugin_controller* controller, std::int32_t part_type, std::int32_t part_index)
{
  auto knob_grid = create_grid_ui(controller, 8, 1);
  knob_grid->add_cell(create_param_edit_ui(controller, part_type, part_index, lfo_param::rand_type, edit_type::selector, tooltip_type::label), 0, 0, 3, 1);
  knob_grid->add_cell(create_param_edit_ui(controller, part_type, part_index, lfo_param::rand_type, edit_type::dropdown, tooltip_type::off), 3, 0, 1, 1);
  knob_grid->add_cell(create_param_edit_ui(controller, part_type, part_index, lfo_param::rand_steps, edit_type::selector, tooltip_type::value), 4, 0, 3, 1);
  knob_grid->add_cell(create_param_label_ui(controller, part_type, part_index, lfo_param::rand_steps, label_type::label, Justification::centred), 7, 0, 1, 1);
  auto other_grid = create_grid_ui(controller, 24, 1);
  auto rand_steps_grid = create_grid_ui(controller, 6, 8);
  rand_steps_grid->add_cell(create_param_label_ui(controller, part_type, part_index, lfo_param::rand_rand_steps, label_type::label, Justification::right), 0, 0, 6, 1);
  rand_steps_grid->add_cell(create_param_edit_ui(controller, part_type, part_index, lfo_param::rand_rand_steps, edit_type::toggle, tooltip_type::off), 1, 1, 4, 1);
  other_grid->add_cell(std::move(rand_steps_grid), 1, 0, 4, 1);
  auto seedx = other_grid->add_cell(create_labeled_param_ui(controller, part_type, part_index, lfo_param::rand_seedx, edit_type::hslider, label_type::label, tooltip_type::value, false, 8), 7, 0, 4, 1);
  seedx->relevant_if(part_id(part_type, part_index), lfo_param::rand_rand_steps, false, [](std::int32_t part_index, std::int32_t val) { return val != 0; });
  other_grid->add_cell(create_labeled_param_ui(controller, part_type, part_index, lfo_param::rand_seedy, edit_type::hslider, label_type::label, tooltip_type::value, false, 8), 13, 0, 4, 1);
  other_grid->add_cell(create_labeled_param_ui(controller, part_type, part_index, lfo_param::rand_amt, edit_type::hslider, label_type::label, tooltip_type::value, false, 8), 19, 0, 4, 1);
  auto grid = create_grid_ui(controller, 1, 4);
  grid->add_cell(std::move(knob_grid), 0, 0, 1, 1);
  grid->add_cell(std::move(other_grid), 0, 1, 1, 3);
  return create_part_group_ui(controller, create_group_label_ui(controller, "Random", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_lfo_free_group(
  plugin_controller* controller, std::int32_t part_type, std::int32_t part_index, 
  std::int32_t stage1, std::int32_t stage2, std::int32_t slope, char const* group_name)
{
  auto grid = create_grid_ui(controller, 6, 5);
  grid->add_cell(create_labeled_param_ui(controller, part_type, part_index, stage1, edit_type::hslider, label_type::label, tooltip_type::value, false, 8), 1, 0, 4, 2);
  grid->add_cell(create_labeled_param_ui(controller, part_type, part_index, stage2, edit_type::hslider, label_type::label, tooltip_type::value, false, 8), 1, 2, 4, 2);
  auto slope_grid = create_grid_ui(controller, 1, 4);
  slope_grid->add_cell(create_param_label_ui(controller, part_type, part_index, slope, label_type::label, Justification::centred), 0, 0);
  slope_grid->add_cell(create_param_edit_ui(controller, part_type, part_index, slope, edit_type::selector, tooltip_type::value), 0, 1, 1, 3);
  grid->add_cell(std::move(slope_grid), 0, 4, 6, 1);
  return create_part_group_ui(controller, create_group_label_ui(controller, group_name, true), std::move(grid));
}

static std::unique_ptr<ui_element>
create_lfo_free_groups(plugin_controller* controller, std::int32_t part_type, std::int32_t part_index)
{ 
  auto grid = create_grid_ui(controller, 4, 1);
  grid->add_cell(create_part_group_container_ui(controller, part_group_cv, create_lfo_free_group(
    controller, part_type, part_index, lfo_param::free_delay1, lfo_param::free_rise1, lfo_param::free_rise1_slope, "DA")), 0, 0);
  grid->add_cell(create_part_group_container_ui(controller, part_group_cv, create_lfo_free_group(
    controller, part_type, part_index, lfo_param::free_hold1, lfo_param::free_fall1, lfo_param::free_fall1_slope, "HD")), 1, 0);
  grid->add_cell(create_part_group_container_ui(controller, part_group_cv, create_lfo_free_group(
    controller, part_type, part_index, lfo_param::free_delay2, lfo_param::free_fall2, lfo_param::free_fall2_slope, "DD")), 2, 0);
  grid->add_cell(create_part_group_container_ui(controller, part_group_cv, create_lfo_free_group(
    controller, part_type, part_index, lfo_param::free_hold2, lfo_param::free_rise2, lfo_param::free_rise2_slope, "HA")), 3, 0);
  return grid;
}

static std::unique_ptr<ui_element>
create_lfo_grid(plugin_controller* controller, std::int32_t part_type, std::int32_t part_index)
{
  auto grid = create_grid_ui(controller, 3, 7);
  grid->add_cell(create_part_group_container_ui(controller, part_group_cv, create_lfo_lfo_group(controller, part_type, part_index)), 0, 1, 1, 3);
  grid->add_cell(create_part_group_container_ui(controller, part_group_cv, create_lfo_main_group(controller, part_type, part_index)), 0, 0, 3, 1);
  grid->add_cell(create_part_group_container_ui(controller, part_group_cv, create_part_graph_ui(controller, part_type, part_index, 0, lfo_param::on)), 0, 4, 1, 3);
  auto basic = grid->add_cell(create_part_group_container_ui(controller, part_group_cv, create_lfo_basic_group(controller, part_type, part_index)), 1, 1, 2, 6);
  basic->relevant_if(part_id(part_type, part_index), lfo_param::type, true, [](std::int32_t part_index, std::int32_t val) { return val == lfo_type::basic; });
  auto random = grid->add_cell(create_part_group_container_ui(controller, part_group_cv, create_lfo_random_group(controller, part_type, part_index)), 1, 1, 2, 6);
  random->relevant_if(part_id(part_type, part_index), lfo_param::type, true, [](std::int32_t part_index, std::int32_t val) { return val == lfo_type::random; });
  auto free = grid->add_cell(create_lfo_free_groups(controller, part_type, part_index), 1, 1, 2, 6);
  free->relevant_if(part_id(part_type, part_index), lfo_param::type, true, [](std::int32_t part_index, std::int32_t val) { return val == lfo_type::free; });
  return grid;
}

static std::unique_ptr<ui_element>
create_lfo_selector(plugin_controller* controller, std::int32_t part_type, std::int32_t part_count, std::int32_t selector_param_index)
{
  std::vector<std::unique_ptr<ui_element>> lfos;
  for (std::int32_t i = 0; i < part_count; i++)
    lfos.emplace_back(create_lfo_grid(controller, part_type, i));
  return create_part_selector_ui(controller, "LFO", part_type::active, selector_param_index, part_type, 3, 4, part_group_cv, std::move(lfos));
}

static std::unique_ptr<ui_element>
create_amp_group(plugin_controller* controller, part_type part_type)
{
  auto grid = create_grid_ui(controller, 1, 2);
  grid->add_cell(create_labeled_param_ui(controller, part_type, 0, amp_param::gain, edit_type::knob, label_type::label, tooltip_type::value), 0, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type, 0, amp_param::bal, edit_type::knob, label_type::label, tooltip_type::value), 0, 1);
  return create_part_single_ui(controller, "Amp", part_type::vamp, true, part_group_audio, create_part_group_container_ui(controller, part_group_audio, std::move(grid)));
}

static std::unique_ptr<ui_element>
create_voice_group(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 4, 4);
  grid->add_cell(create_param_edit_ui(controller, part_type::voice, 0, voice_param::mode, edit_type::selector, tooltip_type::label), 0, 0, 3, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type::voice, 0, voice_param::mode, edit_type::dropdown, tooltip_type::off), 3, 0, 1, 1);
  grid->add_cell(create_param_label_ui(controller, part_type::voice, 0, voice_param::note, label_type::label, Justification::centred), 0, 1, 1, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type::voice, 0, voice_param::note, edit_type::dropdown, tooltip_type::off), 1, 1, 1, 1);
  grid->add_cell(create_param_label_ui(controller, part_type::voice, 0, voice_param::oct, label_type::label, Justification::centred), 2, 1, 1, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type::voice, 0, voice_param::oct, edit_type::dropdown, tooltip_type::off), 3, 1, 1, 1);
  grid->add_cell(create_param_label_ui(controller, part_type::voice, 0, voice_param::port_mode, label_type::label, Justification::centred), 0, 2, 1, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type::voice, 0, voice_param::port_mode, edit_type::dropdown, tooltip_type::off), 1, 2, 1, 1);
  auto trig_label = grid->add_cell(create_param_label_ui(controller, part_type::voice, 0, voice_param::port_trig, label_type::label, Justification::centred), 2, 2, 1, 1);
  trig_label->relevant_if(part_id(part_type::voice, 0), voice_param::port_mode, false, [](std::int32_t part_index, std::int32_t val) { return val != voice_port_mode::off; });
  auto trig = grid->add_cell(create_param_edit_ui(controller, part_type::voice, 0, voice_param::port_trig, edit_type::dropdown, tooltip_type::off), 3, 2, 1, 1);

  auto time_grid = create_grid_ui(controller, 4, 1);
  trig->relevant_if(part_id(part_type::voice, 0), voice_param::port_mode, false, [](std::int32_t part_index, std::int32_t val) { return val != voice_port_mode::off; });
  time_grid->add_cell(create_param_label_ui(controller, part_type::voice, 0, voice_param::port_sync, label_type::label, Justification::centred), 0, 0, 1, 1);
  time_grid->add_cell(create_param_edit_ui(controller, part_type::voice, 0, voice_param::port_sync, edit_type::toggle, tooltip_type::off), 1, 0, 1, 1);
  auto time_label = time_grid->add_cell(create_param_label_ui(controller, part_type::voice, 0, voice_param::port_time, label_type::label, Justification::centred), 2, 0, 1, 1);
  time_label->relevant_if(part_id(part_type::voice, 0), voice_param::port_sync, true, [](std::int32_t part_index, std::int32_t val) { return val == 0; });
  auto time = time_grid->add_cell(create_param_edit_ui(controller, part_type::voice, 0, voice_param::port_time, edit_type::hslider, tooltip_type::value), 3, 0, 1, 1);
  time->relevant_if(part_id(part_type::voice, 0), voice_param::port_sync, true, [](std::int32_t part_index, std::int32_t val) { return val == 0; });
  auto tempo_label = time_grid->add_cell(create_param_label_ui(controller, part_type::voice, 0, voice_param::port_tempo, label_type::label, Justification::centred), 2, 0, 1, 1);
  tempo_label->relevant_if(part_id(part_type::voice, 0), voice_param::port_sync, true, [](std::int32_t part_index, std::int32_t val) { return val != 0; });
  auto tempo = time_grid->add_cell(create_param_edit_ui(controller, part_type::voice, 0, voice_param::port_tempo, edit_type::dropdown, tooltip_type::off), 3, 0, 1, 1);
  tempo->relevant_if(part_id(part_type::voice, 0), voice_param::port_sync, true, [](std::int32_t part_index, std::int32_t val) { return val != 0; });
  time_grid->relevant_if(part_id(part_type::voice, 0), voice_param::port_mode, false, [](std::int32_t part_index, std::int32_t val) { return val != voice_port_mode::off; });
  grid->add_cell(std::move(time_grid), 0, 3, 4, 1);

  return create_part_single_ui(controller, "Voice", part_type::voice, true, part_group_audio, create_part_group_container_ui(controller, part_group_audio, std::move(grid)));
}

static std::unique_ptr<ui_element>
create_voice_amp_grid(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 1, 3);
  grid->add_cell(create_amp_group(controller, part_type::vamp), 0, 0, 1, 1);
  grid->add_cell(create_voice_group(controller), 0, 1, 1, 2);
  return grid;
}

static std::unique_ptr<ui_element>
create_voice_part_fx_grid(plugin_controller* controller)
{
  auto result = create_grid_ui(controller, 4, 1);
  result->add_cell(create_fx_selector(controller, part_type::veffect, veffect_count, active_param::veffect), 0, 0, 3, 1);
  result->add_cell(create_voice_amp_grid(controller), 3, 0, 1, 1);
  return result;
}

static std::unique_ptr<ui_element>
create_cv_plot_controls(plugin_controller* controller, std::int32_t part_type)
{
  auto grid = create_grid_ui(controller, 4, 2);
  grid->add_cell(create_param_label_ui(controller, part_type, 0, cv_plot_param::length, label_type::label, Justification::centred), 1, 0, 1, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type, 0, cv_plot_param::length, edit_type::knob, tooltip_type::value), 0, 1, 3, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type, 0, cv_plot_param::target, edit_type::dropdown, tooltip_type::off), 3, 0, 1, 4);
  return create_part_group_container_ui(controller, part_group_cv, std::move(grid));
}

static std::unique_ptr<ui_element>
create_cv_plot_part(plugin_controller* controller, std::int32_t part_type)
{
  auto grid = create_grid_ui(controller, 1, 10);
  grid->add_cell(create_cv_plot_controls(controller, part_type), 0, 0, 1, 3);
  grid->add_cell(create_part_graph_ui(controller, part_type, 0, 0, cv_plot_param::target), 0, 3, 1, 7);
  return create_part_single_ui(controller, "CV Plot", part_type, true, part_group_cv, std::move(grid));
}

static std::unique_ptr<ui_element>
create_cv_graph_lfo_grid(plugin_controller* controller)
{
  auto result = create_grid_ui(controller, 4, 1);
  result->add_cell(create_lfo_selector(controller, part_type::vlfo, vlfo_count, active_param::vlfo), 0, 0, 3, 1);
  result->add_cell(create_cv_plot_part(controller, part_type::vcv_plot), 3, 0, 1, 1);
  return result;
}

static std::unique_ptr<ui_element>
create_audio_part(plugin_controller* controller, std::int32_t part_type)
{
  auto outer_grid = create_grid_ui(controller, audio_bank_route_count * 2 + 1, 1);
  auto header_grid = create_grid_ui(controller, 1, 32);
  header_grid->add_cell(create_label_ui(controller, "In/Out", Justification::centred), 0, 1, 1, 10);
  header_grid->add_cell(create_label_ui(controller, "Gain", Justification::centred), 0, 11, 1, 10);
  header_grid->add_cell(create_label_ui(controller, "Bal", Justification::centred), 0, 21, 1, 10);
  outer_grid->add_cell(create_part_group_container_ui(controller, part_group_audio, std::move(header_grid)), 0, 0, 1, 1);
  for (std::int32_t i = 0; i < audio_bank_route_count; i++)
  {
    auto inner_grid = create_grid_ui(controller, 2, 32);
    inner_grid->add_cell(create_param_edit_ui(controller, part_type, 0, audio_bank_param_index(i, audio_bank_param_type::in), edit_type::dropdown, tooltip_type::label, false, true), 0, 1, 1, 10);
    auto out = inner_grid->add_cell(create_param_edit_ui(controller, part_type, 0, audio_bank_param_index(i, audio_bank_param_type::out), edit_type::dropdown, tooltip_type::label, false, true), 1, 1, 1, 10);
    out->relevant_if(part_id(part_type, 0), audio_bank_param_index(i, audio_bank_param_type::in), false, [](std::int32_t part_index, std::int32_t val) { return val != 0; });
    auto gain = inner_grid->add_cell(create_param_edit_ui(controller, part_type, 0, audio_bank_param_index(i, audio_bank_param_type::amt), edit_type::knob, tooltip_type::value, false, true), 0, 11, 2, 10);
    gain->relevant_if(part_id(part_type, 0), audio_bank_param_index(i, audio_bank_param_type::in), false, [](std::int32_t part_index, std::int32_t val) { return val != 0; });
    auto bal = inner_grid->add_cell(create_param_edit_ui(controller, part_type, 0, audio_bank_param_index(i, audio_bank_param_type::bal), edit_type::knob, tooltip_type::value, false, true), 0, 21, 2, 10);
    bal->relevant_if(part_id(part_type, 0), audio_bank_param_index(i, audio_bank_param_type::in), false, [](std::int32_t part_index, std::int32_t val) { return val != 0; });
    outer_grid->add_cell(create_part_group_container_ui(controller, part_group_audio, std::move(inner_grid)), i * 2 + 1, 0, 2, 1);
  }
  return create_part_single_ui(controller, "Audio", part_type, false, part_group_audio, std::move(outer_grid));
}

static std::unique_ptr<ui_element>
create_cv_part(plugin_controller* controller, std::int32_t part_type)
{
  auto outer_grid = create_grid_ui(controller, cv_bank_route_count * 2 + 1, 1);
  auto header_grid = create_grid_ui(controller, 1, 42);
  header_grid->add_cell(create_label_ui(controller, "In/Op/Amt", Justification::centred), 0, 1, 1, 20);
  header_grid->add_cell(create_label_ui(controller, "Out/Scl/Offset", Justification::centred), 0, 21, 1, 20);
  outer_grid->add_cell(create_part_group_container_ui(controller, part_group_cv, std::move(header_grid)), 0, 0);
  for (std::int32_t i = 0; i < cv_bank_route_count; i++)
  {
    auto inner_grid = create_grid_ui(controller, 2, 42);
    inner_grid->add_cell(create_param_edit_ui(controller, part_type, 0, cv_bank_param_index(i, cv_bank_param_type::in), edit_type::dropdown, tooltip_type::label, false, true), 0, 1, 1, 20);
    auto op = inner_grid->add_cell(create_param_edit_ui(controller, part_type, 0, cv_bank_param_index(i, cv_bank_param_type::op), edit_type::dropdown, tooltip_type::label, false, true), 1, 1, 1, 10);
    op->relevant_if(part_id(part_type, 0), cv_bank_param_index(i, cv_bank_param_type::in), false, [](std::int32_t part_index, std::int32_t val) { return val != 0; });
    auto amt = inner_grid->add_cell(create_param_edit_ui(controller, part_type, 0, cv_bank_param_index(i, cv_bank_param_type::amt), edit_type::hslider, tooltip_type::value, false, true), 1, 11, 1, 10);
    amt->relevant_if(part_id(part_type, 0), cv_bank_param_index(i, cv_bank_param_type::in), false, [](std::int32_t part_index, std::int32_t val) { return val != 0; });
    auto out = inner_grid->add_cell(create_param_edit_ui(controller, part_type, 0, cv_bank_param_index(i, cv_bank_param_type::out), edit_type::dropdown, tooltip_type::label, false, true), 0, 21, 1, 20);
    out->relevant_if(part_id(part_type, 0), cv_bank_param_index(i, cv_bank_param_type::in), false, [](std::int32_t part_index, std::int32_t val) { return val != 0; });
    auto scl = inner_grid->add_cell(create_param_edit_ui(controller, part_type, 0, cv_bank_param_index(i, cv_bank_param_type::scale), edit_type::hslider, tooltip_type::value, false, true), 1, 21, 1, 10);
    scl->relevant_if(part_id(part_type, 0), cv_bank_param_index(i, cv_bank_param_type::in), false, [](std::int32_t part_index, std::int32_t val) { return val != 0; });
    auto off = inner_grid->add_cell(create_param_edit_ui(controller, part_type, 0, cv_bank_param_index(i, cv_bank_param_type::off), edit_type::hslider, tooltip_type::value, false, true), 1, 31, 1, 10);
    off->relevant_if(part_id(part_type, 0), cv_bank_param_index(i, cv_bank_param_type::in), false, [](std::int32_t part_index, std::int32_t val) { return val != 0; });
    outer_grid->add_cell(create_part_group_container_ui(controller, part_group_cv, std::move(inner_grid)), i * 2 + 1, 0, 2, 1);
  }
  return create_part_single_ui(controller, "CV", part_type, false, part_group_cv, std::move(outer_grid));
}

static std::unique_ptr<ui_element>
create_voice_grid(plugin_controller* controller)
{
  auto result = create_grid_ui(controller, 2, 17);
  result->add_cell(create_audio_part(controller, part_type::vaudio_bank), 0, 0, 2, 2);
  result->add_cell(create_oscillator_selector(controller), 0, 2, 1, 6);
  result->add_cell(create_envelope_selector(controller), 0, 8, 1, 6);
  result->add_cell(create_voice_part_fx_grid(controller), 1, 2, 1, 6);
  result->add_cell(create_cv_graph_lfo_grid(controller), 1, 8, 1, 6);
  result->add_cell(create_cv_part(controller, part_type::vcv_bank), 0, 14, 2, 3);
  return result;
}

std::unique_ptr<root_element>
create_synth_ui(plugin_controller* controller)
{
  auto result = create_root_ui(controller, create_voice_grid(controller), controller->editor_current_width(), juce::Colours::black);
  result->look_and_feel().setColour(PopupMenu::backgroundColourId, Colour(0x00000000));
  result->look_and_feel().setColour(TooltipWindow::backgroundColourId, Colour(0x00000000));

  result->look_and_feel().setColour(inf_look_and_feel::colors::param_label, Colour(0xFFA7BECB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_group_label, Colour(0xFFFD9A4D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::root_background, Colour(0xFF222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::icon_stroke_color, Colour(0xFFA7BECB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::icon_pw_stroke_color, Colour(0xFF2EB8D1));

  result->look_and_feel().setColour(inf_look_and_feel::colors::selector_label_text, Colour(0xFFFD9A4D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::selector_label_outline_low, Colour(0xFFDA6102));
  result->look_and_feel().setColour(inf_look_and_feel::colors::selector_label_outline_high, Colour(0xFFFD9A4D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::selector_label_background_low, Colour(0xFF444444));
  result->look_and_feel().setColour(inf_look_and_feel::colors::selector_label_background_high, Colour(0xFF777777));

  result->look_and_feel().setColour(inf_look_and_feel::colors::tooltip_text, Colour(0xFFA7BECB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::tooltip_outline_low, Colour(0xFF14525D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::tooltip_outline_high, Colour(0xFF2EB8D1));
  result->look_and_feel().setColour(inf_look_and_feel::colors::tooltip_background_low, Colour(0xFF444444));
  result->look_and_feel().setColour(inf_look_and_feel::colors::tooltip_background_high, Colour(0xFF777777));

  result->look_and_feel().setColour(inf_look_and_feel::colors::part_graph_grid, Colour(0xFF0D363E));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_graph_area, Colour(0xA0FD9A4D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_graph_line, Colour(0xFFFD9A4D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_graph_fill_low, Colour(0xFF111111));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_graph_fill_high, Colour(0xFF222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_graph_outline_low, Colour(0xFFDA6102));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_graph_outline_high, Colour(0xFFFD9A4D));

  result->look_and_feel().setColour(inf_look_and_feel::colors::part_group_container1_fill_low, Colour(0xFF333333));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_group_container1_fill_high, Colour(0xFF444444));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_group_container1_outline_low, Colour(0xFF444444));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_group_container1_outline_high, Colour(0xFF555555));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_group_container2_fill_low, Colour(0xFF3E1B0D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_group_container2_fill_high, Colour(0xFF5D2914));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_group_container2_outline_low, Colour(0xFF5D2914));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_group_container2_outline_high, Colour(0xFFAE4D25));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_group_container3_fill_low, Colour(0xFF0D363E));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_group_container3_fill_high, Colour(0xFF14525D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_group_container3_outline_low, Colour(0xFF14525D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::part_group_container3_outline_high, Colour(0xFF259AAE));

  result->look_and_feel().setColour(inf_look_and_feel::colors::tab_button_text, Colour(0xFFA7BECB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::tab_button_outline_low, Colour(0xFFDA6102));
  result->look_and_feel().setColour(inf_look_and_feel::colors::tab_button_outline_high, Colour(0xFFFD9A4D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::tab_button_background_low, Colour(0xFF444444));
  result->look_and_feel().setColour(inf_look_and_feel::colors::tab_button_background_high, Colour(0xFF777777));
  result->look_and_feel().setColour(inf_look_and_feel::colors::tab_button_highlight_background_low, Colour(0xFF0D363E));
  result->look_and_feel().setColour(inf_look_and_feel::colors::tab_button_highlight_background_high, Colour(0xFF14525D));

  result->look_and_feel().setColour(inf_look_and_feel::colors::dropdown_text, Colour(0xFFA7BECB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::dropdown_outline_low, Colour(0xFFDA6102));
  result->look_and_feel().setColour(inf_look_and_feel::colors::dropdown_outline_high, Colour(0xFFFD9A4D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::dropdown_background_low, Colour(0xFF444444));
  result->look_and_feel().setColour(inf_look_and_feel::colors::dropdown_background_high, Colour(0xFF777777));
  result->look_and_feel().setColour(inf_look_and_feel::colors::dropdown_highlight_background_low, Colour(0xFF0D363E));
  result->look_and_feel().setColour(inf_look_and_feel::colors::dropdown_highlight_background_high, Colour(0xFF14525D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::dropdown_tick_spot_fill_low, Colour(0x00222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::dropdown_tick_spot_fill_high, Colour(0x80BBBBBB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::dropdown_tick_gradient_fill_low, Colour(0xFF14525D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::dropdown_tick_gradient_fill_high, Colour(0xFF2EB8D1));

  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_highlight_low, Colour(0xFF111111));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_highlight_high, Colour(0xFFAAAAAA));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_outline_low, Colour(0xFF14525D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_outline_high, Colour(0xFF2EB8D1));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_outline_inactive, Colour(0xFF666666));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_cuts_inward_low, Colour(0xFFDA6102));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_cuts_outward_low, Colour(0x80DA6102));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_cuts_inward_high, Colour(0xFFFD9A4D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_cuts_outward_high, Colour(0x80FD9A4D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_thumb_inward_low, Colour(0x80228A9C));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_thumb_outward_low, Colour(0xFF228A9C));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_thumb_inward_high, Colour(0x802EB8D1));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_thumb_outward_high, Colour(0xFF2EB8D1));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_center_stroke_low, Colour(0xFFDA6102));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_center_stroke_high, Colour(0xFFFD9A4D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_spot_fill_low, Colour(0x00222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_spot_fill_high, Colour(0x80BBBBBB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_gradient_fill_low, Colour(0xFF222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_gradient_fill_high, Colour(0xFF999999));

  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_center_fill, Colour(0xFF444444));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_track_low, Colour(0xFF14525D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_track_high, Colour(0xFF2EB8D1));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_track_inactive, Colour(0xFF222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_highlight_low, Colour(0xFF444444));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_highlight_high, Colour(0xFFAAAAAA));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_spot_fill_low, Colour(0x00222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_spot_fill_high, Colour(0x80BBBBBB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_gradient_fill_low, Colour(0xE0DA6102));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_gradient_fill_high, Colour(0xE0FD9A4D));

  result->look_and_feel().setColour(inf_look_and_feel::colors::switch_outline_low, Colour(0xFF14525D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::switch_outline_high, Colour(0xFF2EB8D1));
  result->look_and_feel().setColour(inf_look_and_feel::colors::switch_spot_fill_low, Colour(0x00222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::switch_spot_fill_high, Colour(0x80BBBBBB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::switch_gradient_fill_low_on, Colour(0xA0DA6102));
  result->look_and_feel().setColour(inf_look_and_feel::colors::switch_gradient_fill_high_on, Colour(0xA0FD9A4D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::switch_gradient_fill_center_low, Colour(0xFF222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::switch_gradient_fill_center_high, Colour(0xFF999999));
  return result;
}

} // inf::synth::ui 