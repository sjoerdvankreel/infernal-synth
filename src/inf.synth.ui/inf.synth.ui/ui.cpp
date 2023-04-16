#include <inf.synth.ui/ui.hpp>
#include <inf.synth/synth/topology.hpp>
#include <inf.synth/oscillator/topology.hpp>
#include <inf.synth/audio_bank/topology.hpp>

using namespace juce;
using namespace inf::base;
using namespace inf::base::ui;

namespace inf::synth::ui {

static std::unique_ptr<grid_element>
create_content(plugin_controller* controller)
{
  auto result = create_grid_ui(controller, 1, 4, 1.0);
  add_grid_param_cell(result.get(), part_type::vaudio_bank, 0, audio_bank_param::amt1, 0, 0, 1, 1)->color(Slider::ColourIds::rotarySliderFillColourId, Colours::red);
  add_grid_param_cell(result.get(), part_type::vaudio_bank, 0, audio_bank_param::bal1, 0, 1, 1, 1)->color(Slider::ColourIds::rotarySliderFillColourId, Colours::green);
  add_grid_param_cell(result.get(), part_type::vaudio_bank, 0, audio_bank_param::amt2, 0, 2, 1, 2)->color(Slider::ColourIds::rotarySliderFillColourId, Colours::blue);
  return result;
}

std::unique_ptr<root_element>
create_synth_ui(plugin_controller* controller, std::int32_t width)
{
  auto result = create_root_ui(controller, width, juce::Colours::aliceblue);
  result->content(create_content(controller));
  return result;
}

} // inf::synth::ui 