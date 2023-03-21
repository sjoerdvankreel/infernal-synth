#ifndef INF_BASE_PLUGIN_PLUGIN_CONTROLLER_HPP
#define INF_BASE_PLUGIN_PLUGIN_CONTROLLER_HPP

#include <inf.base/topology/topology_info.hpp>

#include <vector>
#include <cstdint>

namespace inf::base {

// For factory presets (just every .vstpreset in the resources folder).
struct preset_item
{
  std::string name;
  std::string path;
};

// Controller base interface.
class plugin_controller
{
public:
  virtual param_value* state() const = 0;
  virtual topology_info const* topology() const = 0;
  virtual void update_dependent_visibility(std::int32_t param_id) const = 0;

  virtual void restart() = 0;
  virtual void load_preset(std::size_t index) = 0;
  virtual std::vector<preset_item>& preset_items() = 0;
  virtual void load_component_state(param_value* state, bool perform_edit) = 0;
  virtual void copy_param(std::int32_t source_tag, std::int32_t target_tag) = 0;
  virtual void swap_param(std::int32_t source_tag, std::int32_t target_tag) = 0;
};

} // namespace inf::base
#endif // INF_BASE_PLUGIN_PLUGIN_CONTROLLER_HPP