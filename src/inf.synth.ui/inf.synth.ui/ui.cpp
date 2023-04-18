#include <inf.synth.ui/ui.hpp>
#include <inf.synth/synth/topology.hpp>
#include <inf.synth/oscillator/topology.hpp>
#include <inf.synth/audio_bank/topology.hpp>

using namespace juce;
using namespace inf::base;
using namespace inf::base::ui;

namespace inf::synth::ui {

static std::unique_ptr<ui_element>
create_right_container()
{
  auto result = create_container_fill_ui(6, Colour(0xFF222222));
  result->content(create_param_ui(part_type::vaudio_bank, 0, audio_bank_param::amt2));
  return result;
}

static std::unique_ptr<grid_element>
create_container3()
{
  auto result = create_grid_ui(2, 3, 3, 1.0);
  result->add_cell(create_param_ui(part_type::gaudio_bank, 0, audio_bank_param::amt1), 0, 0, 1, 1);
  result->add_cell(create_param_ui(part_type::gaudio_bank, 0, audio_bank_param::bal1), 1, 0, 1, 1);
  result->add_cell(create_param_ui(part_type::gaudio_bank, 0, audio_bank_param::amt2), 0, 1, 1, 1);
  result->add_cell(create_param_ui(part_type::gaudio_bank, 0, audio_bank_param::bal2), 1, 1, 1, 1);
  result->add_cell(create_param_ui(part_type::gaudio_bank, 0, audio_bank_param::amt3), 0, 2, 1, 1);
  result->add_cell(create_param_ui(part_type::gaudio_bank, 0, audio_bank_param::bal3), 1, 2, 1, 1);
  return result;
}

static std::unique_ptr<ui_element>
create_bottom_left_container()
{
  auto result = create_container_fill_ui(6, Colour(0xFF222222));
  result->content(create_container3());
  result->color(GroupComponent::ColourIds::textColourId, juce::Colours::purple);
  return result;
}

static std::unique_ptr<ui_element>
create_top_left_container()
{
  auto result = create_container_fill_ui(6, Colour(0xFF222222));
  result->content(create_param_ui(part_type::vaudio_bank, 0, audio_bank_param::amt1));
  return result;
}

static std::unique_ptr<ui_element>
create_top_mid_container()
{
  auto result = create_container_fill_ui(6, Colour(0xFF222222));
  result->content(create_param_ui(part_type::vaudio_bank, 0, audio_bank_param::bal1));
  return result;
}

static std::unique_ptr<grid_element>
create_content()
{
  auto result = create_grid_ui(2, 4, 3, 1.0);
  result->add_cell(create_top_left_container(), 0, 0, 1, 1);
  result->add_cell(create_top_mid_container(), 0, 1, 1, 1);
  result->add_cell(create_right_container(), 0, 2, 2, 2);
  result->add_cell(create_bottom_left_container(), 1, 0, 1, 2);
  return result;
}

std::unique_ptr<root_element>
create_synth_ui(std::int32_t width)
{
  auto result = create_root_ui(width, juce::Colours::black);
  result->content(create_content());
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_thumb, Colour(0xFF990000));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_shadow, Colour(0xFF222222));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_outline, Colour(0xFF990000));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_highlight, Colour(0xFFEEEEEE));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_cuts_inward, Colour(0xFF660000));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_cuts_outward, Colour(0x40660000));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_gradient_fill_base, Colour(0xFF333333));
  result->look_and_feel().setColour(inf_look_and_feel::colors::knob_gradient_fill_highlight, Colour(0xFFBBBBBB));
  return result;
}

} // inf::synth::ui 