#include <inf.synth.ui/ui.hpp>
#include <inf.synth/synth/topology.hpp>
#include <inf.synth/oscillator/topology.hpp>
#include <inf.synth/audio_bank/topology.hpp>

using namespace juce;
using namespace inf::base;
using namespace inf::base::ui;

namespace inf::synth::ui {

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

static std::unique_ptr<ui_element>
create_osc_main_group(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 6, 1);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::type, edit_type::selector, label_type::label, false), 0, 0, 2, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type::vosc, 0, osc_param::type, edit_type::dropdown, false), 2, 0, 1, 1);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::kbd, edit_type::toggle, label_type::label, false), 3, 0, 1, 1);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::gain, edit_type::knob, label_type::label, true), 4, 0, 2, 1);
  return create_group_ui(controller, create_group_label_ui(controller, "Main", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_pitch_group(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 3, 1);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::note, edit_type::selector, label_type::value, false), 0, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::oct, edit_type::selector, label_type::value, false), 1, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::cent, edit_type::knob, label_type::value, false), 2, 0);
  return create_group_ui(controller, create_group_label_ui(controller, "Pitch", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_ram_group(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 3, 1);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::ram_src, edit_type::selector, label_type::value, false), 0, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::ram_bal, edit_type::knob, label_type::label, true), 1, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::ram_mix, edit_type::knob, label_type::label, true), 2, 0);
  return create_group_ui(controller, create_group_label_ui(controller, "R/AM", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_sync_group(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 1, 2);
  auto sync_src = grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::sync_src, edit_type::selector, label_type::value, false), 0, 0);
  sync_src->relevant_if(part_id(part_type::vosc, 0), osc_param::type, false, [](std::int32_t val) { return val != osc_type::kps && val != osc_type::noise; });
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::fm, edit_type::knob, label_type::label, true), 0, 1);
  return create_group_ui(controller, create_group_label_ui(controller, "Sync", true), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_unison_group(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 2, 2);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::uni_voices, edit_type::knob, label_type::value, false), 0, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::uni_dtn, edit_type::knob, label_type::label, true), 0, 1);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::uni_sprd, edit_type::knob, label_type::label, true), 1, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::uni_offset, edit_type::knob, label_type::label, true), 1, 1);
  return create_group_ui(controller, create_group_label_ui(controller, "Unison", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_basic_group(plugin_controller* controller)
{
  auto outer_grid = create_grid_ui(controller, 1, 5);
  outer_grid->add_cell(create_iconed_param_ui(controller, part_type::vosc, 0, osc_param::basic_type, edit_type::selector, icon_for_osc_basic_type, false), 0, 0);
  auto inner_grid = create_grid_ui(controller, 3, 1);
  auto pw = inner_grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::basic_pw, edit_type::hslider, label_type::label, true), 1, 0, 1, 1);
  pw->relevant_if(part_id(part_type::vosc, 0), osc_param::basic_type, false, [](std::int32_t val) { return val == osc_basic_type::pulse; });
  outer_grid->add_cell(std::move(inner_grid), 0, 1, 1, 4);
  return create_group_ui(controller, create_group_label_ui(controller, "Basic", true), std::move(outer_grid));
}

static std::unique_ptr<ui_element>
create_osc_mix_group(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 1, 5);
  grid->add_cell(create_iconed_param_ui(controller, part_type::vosc, 0, osc_param::mix_sine, edit_type::knob, icon_type::sine, true), 0, 0);
  grid->add_cell(create_iconed_param_ui(controller, part_type::vosc, 0, osc_param::mix_saw, edit_type::knob, icon_type::saw, true), 0, 1);
  grid->add_cell(create_iconed_param_ui(controller, part_type::vosc, 0, osc_param::mix_triangle, edit_type::knob, icon_type::tri, true), 0, 2);
  grid->add_cell(create_iconed_param_ui(controller, part_type::vosc, 0, osc_param::mix_pulse, edit_type::knob, icon_type::pulse, true), 0, 3);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::mix_pw, edit_type::knob, label_type::label, true), 0, 4);
  return create_group_ui(controller, create_group_label_ui(controller, "Mix", true), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_dsf_group(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 3, 1);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::dsf_parts, edit_type::hslider, label_type::label, true), 0, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::dsf_dist, edit_type::hslider, label_type::label, true), 1, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::dsf_decay, edit_type::hslider, label_type::label, true), 2, 0);
  return create_group_ui(controller, create_group_label_ui(controller, "DSF", true), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_kps_group(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 3, 1);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::kps_filter, edit_type::hslider, label_type::label, true), 0, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::kps_feedback, edit_type::hslider, label_type::label, true), 1, 0);
  grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::kps_stretch, edit_type::hslider, label_type::label, true), 2, 0);
  return create_group_ui(controller, create_group_label_ui(controller, "K+S", true), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_noise_group(plugin_controller* controller)
{
  auto outer_grid = create_grid_ui(controller, 1, 5);
  outer_grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::noise_seed, edit_type::knob, label_type::label, true), 0, 0);
  auto inner_grid = create_grid_ui(controller, 3, 1);
  inner_grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::noise_color, edit_type::hslider, label_type::label, true), 0, 0);
  inner_grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::noise_x, edit_type::hslider, label_type::label, true), 1, 0);
  inner_grid->add_cell(create_labeled_param_ui(controller, part_type::vosc, 0, osc_param::noise_y, edit_type::hslider, label_type::label, true), 2, 0);
  outer_grid->add_cell(std::move(inner_grid), 0, 1, 1, 4);
  return create_group_ui(controller, create_group_label_ui(controller, "Noise", true), std::move(outer_grid));
}

static std::unique_ptr<grid_element>
create_oscillator_grid(plugin_controller* controller)
{
  // TODO move color to lnf
  auto result = create_grid_ui(controller, 4, 5);
  result->add_cell(create_container_fill_ui(controller, create_osc_main_group(controller), Colour(0xFF333333)), 0, 0, 3, 1);
  result->add_cell(create_container_fill_ui(controller, create_osc_pitch_group(controller), Colour(0xFF333333)), 0, 1, 3, 1);
  result->add_cell(create_container_fill_ui(controller, create_osc_ram_group(controller), Colour(0xFF333333)), 0, 2, 3, 1);
  result->add_cell(create_container_fill_ui(controller, create_osc_sync_group(controller), Colour(0xFF333333)), 0, 3, 1, 2);
  result->add_cell(create_container_fill_ui(controller, create_osc_unison_group(controller), Colour(0xFF333333)), 1, 3, 2, 2);
  auto basic = result->add_cell(create_container_fill_ui(controller, create_osc_basic_group(controller), Colour(0xFF333333)), 3, 0, 1, 5);
  basic->relevant_if(part_id(part_type::vosc, 0), osc_param::type, true, [](std::int32_t val) { return val == osc_type::basic; });
  auto mix = result->add_cell(create_container_fill_ui(controller, create_osc_mix_group(controller), Colour(0xFF333333)), 3, 0, 1, 5);
  mix->relevant_if(part_id(part_type::vosc, 0), osc_param::type, true, [](std::int32_t val) { return val == osc_type::mix; });
  auto dsf = result->add_cell(create_container_fill_ui(controller, create_osc_dsf_group(controller), Colour(0xFF333333)), 3, 0, 1, 5);
  dsf->relevant_if(part_id(part_type::vosc, 0), osc_param::type, true, [](std::int32_t val) { return val == osc_type::dsf; });
  auto kps = result->add_cell(create_container_fill_ui(controller, create_osc_kps_group(controller), Colour(0xFF333333)), 3, 0, 1, 5);
  kps->relevant_if(part_id(part_type::vosc, 0), osc_param::type, true, [](std::int32_t val) { return val == osc_type::kps; });
  auto noise = result->add_cell(create_container_fill_ui(controller, create_osc_noise_group(controller), Colour(0xFF333333)), 3, 0, 1, 5);
  noise->relevant_if(part_id(part_type::vosc, 0), osc_param::type, true, [](std::int32_t val) { return val == osc_type::noise; });
  return result;
}

std::unique_ptr<root_element>
create_synth_ui(plugin_controller* controller)
{
  auto result = create_root_ui(controller, create_oscillator_grid(controller), controller->editor_current_width(), juce::Colours::black);

  result->look_and_feel().setColour(Label::ColourIds::textColourId, Colour(0xFFA7BECB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::group_label_color, Colour(0xFFFD9A4D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::icon_stroke_color, Colour(0xFFA7BECB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::icon_pw_stroke_color, Colour(0xFF2EB8D1));

  result->look_and_feel().setColour(PopupMenu::backgroundColourId, Colour(0x00000000));
  result->look_and_feel().setColour(ComboBox::ColourIds::textColourId, Colour(0xFFA7BECB));
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