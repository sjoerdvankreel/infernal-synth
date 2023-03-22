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
protected:
  // Separate copy of the parameter state used for graphs.
  std::vector<inf::base::param_value> _state;
  std::vector<inf::base::preset_item> _preset_items;
  std::unique_ptr<inf::base::topology_info> _topology;

  plugin_controller(std::unique_ptr<inf::base::topology_info>&& topology) :
  _state(topology->params.size()), _preset_items(), _topology(std::move(topology))
  { _topology->init_factory_preset(_state.data()); }

public:
  inf::base::param_value const* state() const { return _state.data(); }
  inf::base::topology_info const* topology() const { return _topology.get(); }
  std::vector<inf::base::preset_item>& preset_items() { return _preset_items; }

  virtual void restart() = 0;
  virtual void load_preset(std::size_t index) = 0;
  virtual void load_component_state(param_value* state, bool perform_edit) = 0;
  virtual void copy_param(std::int32_t source_tag, std::int32_t target_tag) = 0;
  virtual void swap_param(std::int32_t source_tag, std::int32_t target_tag) = 0;

  virtual double get_plugin_param(std::int32_t tag) const = 0;
  virtual double base_to_plugin_param(std::int32_t index, param_value val) const = 0;
  virtual param_value plugin_to_base_param(std::int32_t index, double val) const = 0;
};

} // namespace inf::base
#endif // INF_BASE_PLUGIN_PLUGIN_CONTROLLER_HPP