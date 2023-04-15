#include <inf.synth.ui/ui.hpp>
#include <inf.synth/synth/topology.hpp>
#include <inf.synth/oscillator/topology.hpp>
#include <inf.synth/audio_bank/topology.hpp>

using namespace juce;
using namespace inf::base;
using namespace inf::base::ui;

namespace inf::synth::ui {

static std::unique_ptr<ui_element>
create_content(plugin_controller* controller)
{
  auto result = create_grid_ui();
  return create_param_ui(controller, part_type::vosc, 0, osc_param::gain, 0, 0);
}

std::unique_ptr<root_element>
create_synth_ui(plugin_controller* controller, std::int32_t width)
{
  auto result = create_root_ui(controller, width);
  result->content(create_content(controller));
  return result;
}

} // inf::synth::ui 