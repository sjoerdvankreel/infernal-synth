#include <inf.synth.ui/ui.hpp>
#include <inf.synth/synth/topology.hpp>
#include <inf.synth/oscillator/topology.hpp>
#include <inf.synth/audio_bank/topology.hpp>

using namespace juce;
using namespace inf::base;
using namespace inf::base::ui;

namespace inf::synth::ui {

static std::unique_ptr<ui_element>
create_container()
{
  auto result = create_container_ui(Colours::orange);
  result->content(create_param_ui(part_type::vaudio_bank, 0, audio_bank_param::amt2));
  return result;
}

static std::unique_ptr<grid_element>
create_content()
{
  auto result = create_grid_ui(2, 4, 1.0);
  //result->add_cell(create_param_ui(part_type::vaudio_bank, 0, audio_bank_param::amt1), 0, 0, 1, 1);
  //result->add_cell(create_param_ui(part_type::vaudio_bank, 0, audio_bank_param::bal1), 0, 1, 1, 1);
  result->add_cell(create_container(), 0, 2, 2, 2);
  return result;
}

std::unique_ptr<root_element>
create_synth_ui(std::int32_t width)
{
  auto result = create_root_ui(width, juce::Colours::aliceblue);
  result->content(create_content());
  return result;
}

} // inf::synth::ui 