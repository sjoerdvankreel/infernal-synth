#include <inf.synth.ui/ui.hpp>
#include <inf.synth/synth/topology.hpp>
#include <inf.synth/oscillator/topology.hpp>
#include <inf.synth/audio_bank/topology.hpp>

using namespace juce;
using namespace inf::base;
using namespace inf::base::ui;

namespace inf::synth::ui {

static std::unique_ptr<ui_element>
create_ram_grid()
{
  auto result = create_grid_ui({ 1 }, {1, 1, 1});
  result->add_cell(create_param_ui(part_type::vosc, 0, osc_param::ram_src), 0, 0);
  result->add_cell(create_param_ui(part_type::vosc, 0, osc_param::ram_bal), 0, 1);
  result->add_cell(create_param_ui(part_type::vosc, 0, osc_param::ram_mix), 0, 2);
  return result;
}

static std::unique_ptr<ui_element>
create_unison_grid()
{
  auto result = create_grid_ui({ 1, 1 }, { 1, 1 });
  result->add_cell(create_param_ui(part_type::vosc, 0, osc_param::uni_voices), 0, 0);
  result->add_cell(create_param_ui(part_type::vosc, 0, osc_param::uni_dtn), 0, 1);
  result->add_cell(create_param_ui(part_type::vosc, 0, osc_param::uni_sprd), 1, 0);
  result->add_cell(create_param_ui(part_type::vosc, 0, osc_param::uni_offset), 1, 1);
  return result;
}

static std::unique_ptr<grid_element>
create_outer_grid()
{
  auto result = create_grid_ui({1, 1, 1}, {1, 1, 1});
  result->add_cell(create_container_fill_ui(create_ram_grid(), 15.0f, Colour(0xFF222222)), 0, 0, 1, 3);
  result->add_cell(create_container_fill_ui(create_unison_grid(), 15.0f, Colour(0xFF222222)), 1, 0, 2, 2);
  return result;
}

std::unique_ptr<root_element>
create_synth_ui(std::int32_t width)
{
  auto result = create_root_ui(create_outer_grid(), width, juce::Colours::black);
  juce::LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypefaceName("Verdana");
  result->look_and_feel().setColour(Label::ColourIds::textColourId, Colour(0xFFA7BECB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_highlight_low, Colour(0xFF111111));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_highlight_high, Colour(0xFFAAAAAA));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_outline_low, Colour(0xFF14525D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_outline_high, Colour(0xFF2EB8D1));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_outline_inactive, Colour(0xFF444444));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_cuts_inward_low, Colour(0xFFDA6102));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_cuts_outward_low, Colour(0x40DA6102));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_cuts_inward_high, Colour(0xFFFD9A4D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_cuts_outward_high, Colour(0x40FD9A4D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_thumb_inward_low, Colour(0x00228A9C));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_thumb_outward_low, Colour(0xFF228A9C));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_thumb_inward_high, Colour(0x002EB8D1));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_thumb_outward_high, Colour(0xFF2EB8D1));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_center_stroke_low, Colour(0xFFDA6102));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_center_stroke_high, Colour(0xFFFD9A4D));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_spot_fill_base, Colour(0x00222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_spot_fill_highlight, Colour(0x80BBBBBB));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_gradient_fill_base, Colour(0xFF222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_gradient_fill_highlight, Colour(0xFF999999));
  return result;
}

} // inf::synth::ui 