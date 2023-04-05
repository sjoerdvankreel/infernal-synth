#ifndef INF_BASE_PLUGIN_PLUGIN_CONTROLLER_HPP
#define INF_BASE_PLUGIN_PLUGIN_CONTROLLER_HPP

#include <inf.base/topology/topology_info.hpp>

#include <vector>
#include <string>
#include <cstdint>

namespace inf::base {

// For factory presets (just every .vstpreset in the resources folder).
struct factory_preset
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
  std::unique_ptr<inf::base::topology_info> _topology;
  std::vector<inf::base::factory_preset> _factory_presets;

  plugin_controller(std::unique_ptr<inf::base::topology_info>&& topology) :
  _state(topology->params.size()), _topology(std::move(topology)), _factory_presets()
  { _topology->init_factory_preset(_state.data()); }

public:
  inf::base::param_value const* state() const { return _state.data(); }
  inf::base::topology_info const* topology() const { return _topology.get(); }
  std::vector<inf::base::factory_preset>& factory_presets() { return _factory_presets; }

  virtual void restart() = 0;
  virtual void save_preset(std::string const& path) = 0;
  virtual void load_preset(std::string const& path, bool factory) = 0;
  virtual void load_component_state(param_value* state, bool perform_edit) = 0;
  virtual void copy_param(std::int32_t source_tag, std::int32_t target_tag) = 0;
  virtual void swap_param(std::int32_t source_tag, std::int32_t target_tag) = 0;

  virtual double get_plugin_param(std::int32_t tag) = 0;
  virtual double base_to_plugin_param(std::int32_t index, param_value val) const = 0;
  virtual param_value plugin_to_base_param(std::int32_t index, double val) const = 0;
};

} // namespace inf::base
#endif // INF_BASE_PLUGIN_PLUGIN_CONTROLLER_HPP