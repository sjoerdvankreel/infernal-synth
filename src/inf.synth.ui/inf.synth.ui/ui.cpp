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
  std::unique_ptr<param_element> result(std::make_unique<param_element>(controller, part_type::vosc, 0, osc_param::gain));
  result->bounds(Rectangle<int>(0, 0, 1, 1));
  return result;
}

std::unique_ptr<root_element>
create_synth_ui(plugin_controller* controller, Point<std::int32_t> const& size)
{
  std::unique_ptr<root_element> result(std::make_unique<root_element>());
  result->size(size);
  result->content(create_content(controller));
  return result;
}

} // inf::synth::ui 