#ifndef INF_BASE_PLUGIN_PLUGIN_CONTROLLER_HPP
#define INF_BASE_PLUGIN_PLUGIN_CONTROLLER_HPP

#include <inf.base/topology/topology_info.hpp>

#include <vector>
#include <string>
#include <cstdint>

namespace inf::base {

// Param changed by automation callback.
class param_listener
{
public:
  virtual void param_changed(param_value value) = 0;
};
  
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
  std::vector<inf::base::factory_preset> _factory_presets = {};
  std::map<std::int32_t, param_listener*> _param_listeners = {};

  plugin_controller(std::unique_ptr<inf::base::topology_info>&& topology) :
  _state(topology->params.size()), _topology(std::move(topology))
  { _topology->init_factory_preset(_state.data()); }

  void controller_changed_param(std::int32_t tag, param_value value);

public:
  inf::base::param_value const* state() const { return _state.data(); }
  inf::base::topology_info const* topology() const { return _topology.get(); }

  virtual void restart() = 0;
  virtual std::string preset_file_extension() = 0;
  virtual void save_preset(std::string const& path) = 0;
  virtual void load_preset(std::string const& path, bool factory) = 0;
  virtual void load_component_state(param_value* state, bool perform_edit) = 0;

  virtual std::int32_t editor_default_width() const = 0;
  virtual void editor_changed_param(std::int32_t index, param_value value) = 0;
  virtual void copy_param(std::int32_t source_tag, std::int32_t target_tag) = 0;
  virtual void swap_param(std::int32_t source_tag, std::int32_t target_tag) = 0;

  virtual double get_plugin_param(std::int32_t tag) = 0;
  virtual double base_to_plugin_param(std::int32_t index, param_value val) const = 0;
  virtual param_value plugin_to_base_param(std::int32_t index, double val) const = 0;

  std::vector<inf::base::factory_preset>& factory_presets() { return _factory_presets; }
  void clear_param_listener(std::int32_t param_index) { _param_listeners.erase(param_index); }
  void set_param_listener(std::int32_t param_index, param_listener* listener) { _param_listeners[param_index] = listener; }
};

} // namespace inf::base
#endif // INF_BASE_PLUGIN_PLUGIN_CONTROLLER_HPP