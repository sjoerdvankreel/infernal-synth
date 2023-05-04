#include <inf.synth.ui/ui.hpp>
#include <inf.synth/synth/topology.hpp>
#include <inf.synth/oscillator/topology.hpp>
#include <inf.synth/audio_bank/topology.hpp>

using namespace juce;
using namespace inf::base;
using namespace inf::base::ui;

namespace inf::synth::ui {

static std::unique_ptr<ui_element>
create_osc_main_group(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 6, 1);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::type, label_type::label, edit_type::selector), 0, 0, 2, 1);
  grid->add_cell(create_param_edit_ui(controller, part_type::vosc, 0, osc_param::type, edit_type::dropdown), 2, 0, 1, 1);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::kbd, label_type::label, edit_type::toggle), 3, 0, 1, 1);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::gain, label_type::label, edit_type::knob), 4, 0, 2, 1);
  return create_group_ui(controller, create_group_label_ui(controller, "Main", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_pitch_group(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 3, 1);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::note, label_type::value, edit_type::selector), 0, 0);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::oct, label_type::value, edit_type::selector), 1, 0);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::cent, label_type::value, edit_type::knob), 2, 0);
  return create_group_ui(controller, create_group_label_ui(controller, "Pitch", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_ram_group(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 3, 1);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::ram_src, label_type::value, edit_type::selector), 0, 0);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::ram_bal, label_type::label, edit_type::knob), 1, 0);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::ram_mix, label_type::label, edit_type::knob), 2, 0);
  return create_group_ui(controller, create_group_label_ui(controller, "R/AM", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_sync_group(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 1, 2);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::sync_src, label_type::value, edit_type::selector), 0, 0);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::fm, label_type::label, edit_type::knob), 0, 1);
  return create_group_ui(controller, create_group_label_ui(controller, "Sync", true), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_unison_group(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 2, 2);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::uni_voices, label_type::value, edit_type::knob), 0, 0);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::uni_dtn, label_type::label, edit_type::knob), 0, 1);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::uni_sprd, label_type::label, edit_type::knob), 1, 0);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::uni_offset, label_type::label, edit_type::knob), 1, 1);
  return create_group_ui(controller, create_group_label_ui(controller, "Unison", false), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_type_group(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 1, 5);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::mix_sine, label_type::label, edit_type::knob), 0, 0);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::mix_saw, label_type::label, edit_type::knob), 0, 1);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::mix_triangle, label_type::label, edit_type::knob), 0, 2);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::mix_pulse, label_type::label, edit_type::knob), 0, 3);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::mix_pw, label_type::label, edit_type::knob), 0, 4);
  return create_group_ui(controller, create_group_label_ui(controller, "Mix", true), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_hslider_group(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 3, 1);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::uni_dtn, label_type::label, edit_type::hslider), 0, 0);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::uni_sprd, label_type::label, edit_type::hslider), 1, 0);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::fm, label_type::label, edit_type::hslider), 2, 0);
  return create_group_ui(controller, create_group_label_ui(controller, "HSlide", true), std::move(grid));
}

static std::unique_ptr<ui_element>
create_osc_vslider_group(plugin_controller* controller)
{
  auto grid = create_grid_ui(controller, 1, 6);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::uni_dtn, label_type::label, edit_type::vslider), 0, 0);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::uni_sprd, label_type::label, edit_type::vslider), 0, 1);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::fm, label_type::label, edit_type::vslider), 0, 2);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::uni_dtn, label_type::label, edit_type::vslider), 0, 3);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::uni_sprd, label_type::label, edit_type::vslider), 0, 4);
  grid->add_cell(create_param_ui(controller, part_type::vosc, 0, osc_param::fm, label_type::label, edit_type::vslider), 0, 5);
  return create_group_ui(controller, create_group_label_ui(controller, "VSlide", false), std::move(grid));
}

static std::unique_ptr<grid_element>
create_oscillator_grid(plugin_controller* controller)
{
  auto result = create_grid_ui(controller, 4, 8);
  result->add_cell(create_container_fill_ui(controller, create_osc_main_group(controller), Colour(0xFF333333)), 0, 0, 3, 1);
  result->add_cell(create_container_fill_ui(controller, create_osc_pitch_group(controller), Colour(0xFF333333)), 0, 1, 3, 1);
  result->add_cell(create_container_fill_ui(controller, create_osc_ram_group(controller), Colour(0xFF333333)), 0, 2, 3, 1);
  result->add_cell(create_container_fill_ui(controller, create_osc_sync_group(controller), Colour(0xFF333333)), 0, 3, 1, 2);
  result->add_cell(create_container_fill_ui(controller, create_osc_unison_group(controller), Colour(0xFF333333)), 1, 3, 2, 2);
  result->add_cell(create_container_fill_ui(controller, create_osc_type_group(controller), Colour(0xFF333333)), 3, 0, 1, 5);
  result->add_cell(create_container_fill_ui(controller, create_osc_hslider_group(controller), Colour(0xFF333333)), 0, 5, 1, 3);
  result->add_cell(create_container_fill_ui(controller, create_osc_vslider_group(controller), Colour(0xFF333333)), 1, 5, 2, 3);
  return result;
}

std::unique_ptr<root_element>
create_synth_ui(plugin_controller* controller)
{
  auto result = create_root_ui(controller, create_oscillator_grid(controller), controller->editor_current_width(), juce::Colours::black);

  result->look_and_feel().setColour(Label::ColourIds::textColourId, Colour(0xFFA7BECB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::group_label_color, Colour(0xFFFD9A4D));

  result->look_and_feel().setColour(ComboBox::ColourIds::textColourId, Colour(0xFFA7BECB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::dropdown_outline_low, Colour(0xFFDA6102));
  result->look_and_feel().setColour(inf_look_and_feel::colors::dropdown_outline_high, Colour(0xFFFD9A4D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::dropdown_background_low, Colour(0xFF444444));
  result->look_and_feel().setColour(inf_look_and_feel::colors::dropdown_background_high, Colour(0xFF777777));
  result->look_and_feel().setColour(inf_look_and_feel::colors::dropdown_spot_fill_base, Colour(0x00222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::dropdown_spot_fill_highlight, Colour(0x80BBBBBB));

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
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_spot_fill_base, Colour(0x00222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_spot_fill_highlight, Colour(0x80BBBBBB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_gradient_fill_base, Colour(0xFF222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_gradient_fill_highlight, Colour(0xFF999999));

  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_center_fill, Colour(0xFF444444));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_track_low, Colour(0xFF14525D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_track_high, Colour(0xFF2EB8D1));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_track_inactive, Colour(0xFF222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_highlight_low, Colour(0xFF444444));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_highlight_high, Colour(0xFFAAAAAA));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_spot_fill_base, Colour(0x00222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_spot_fill_highlight, Colour(0x80BBBBBB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_gradient_fill_base, Colour(0xE0DA6102));
  result->look_and_feel().setColour(inf_look_and_feel::colors::slider_gradient_fill_highlight, Colour(0xE0FD9A4D));

  result->look_and_feel().setColour(inf_look_and_feel::colors::switch_outline_off, Colour(0xFF666666));
  result->look_and_feel().setColour(inf_look_and_feel::colors::switch_outline_on_low, Colour(0xFF14525D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::switch_outline_on_high, Colour(0xFF2EB8D1));
  result->look_and_feel().setColour(inf_look_and_feel::colors::switch_spot_fill_base, Colour(0x00222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::switch_spot_fill_highlight, Colour(0x80BBBBBB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::switch_gradient_fill_base_on, Colour(0xA0DA6102));
  result->look_and_feel().setColour(inf_look_and_feel::colors::switch_gradient_fill_highlight_on, Colour(0xA0FD9A4D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::switch_gradient_fill_center_base, Colour(0xFF222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::switch_gradient_fill_center_highlight, Colour(0xFF999999));
  return result;
}

} // inf::synth::ui 