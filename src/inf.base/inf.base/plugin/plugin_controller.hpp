#ifndef INF_BASE_PLUGIN_PLUGIN_CONTROLLER_HPP
#define INF_BASE_PLUGIN_PLUGIN_CONTROLLER_HPP

#include <inf.base/topology/topology_info.hpp>

#include <set>
#include <map>
#include <vector>
#include <string>
#include <cstdint>

namespace inf::base {

// Param changed by automation callback.
class param_listener
{
public:
  virtual void controller_param_changed(param_value ui_value) = 0;
};

// Any param changed by automation callback.
class any_param_listener
{
public:
  virtual void any_controller_param_changed(std::int32_t index) = 0;
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
  std::vector<inf::base::param_value> _state;
  std::unique_ptr<inf::base::topology_info> _topology;
  std::set<any_param_listener*> _any_param_listeners = {};
  std::vector<inf::base::factory_preset> _factory_presets = {};
  std::map<std::int32_t, std::set<param_listener*>> _param_listeners = {};

  plugin_controller(std::unique_ptr<inf::base::topology_info>&& topology) :
  _state(topology->params.size()), _topology(std::move(topology))
  { _topology->init_factory_preset(_state.data()); }

  void controller_param_changed(std::int32_t tag, param_value base_value);

public:
  inf::base::param_value const* state() const { return _state.data(); }
  inf::base::topology_info const* topology() const { return _topology.get(); }

  void init_patch();
  void clear_patch();
  void clear_part(part_id id);
  void copy_or_swap_part(part_id source, std::int32_t target, bool swap);

  virtual void restart() = 0;
  virtual std::string preset_file_extension() = 0;
  virtual void save_preset(std::string const& path) = 0;
  virtual void load_preset(std::string const& path, bool factory) = 0;
  virtual void load_component_state(param_value* state, bool perform_edit) = 0;

  virtual float editor_aspect_ratio() const = 0;
  virtual std::int32_t editor_min_width() const = 0;
  virtual std::int32_t editor_max_width() const = 0;
  virtual std::int32_t editor_default_width() const = 0;
  virtual std::int32_t editor_current_width() const = 0;
  virtual std::int32_t editor_font_scaling_min_width() const = 0;
  virtual std::int32_t editor_font_scaling_max_width() const = 0;
  virtual void copy_param(std::int32_t source_tag, std::int32_t target_tag) = 0;
  virtual void swap_param(std::int32_t source_tag, std::int32_t target_tag) = 0;
  virtual void editor_param_changed(std::int32_t index, param_value ui_value) = 0;

  virtual double get_plugin_param(std::int32_t tag) = 0;
  virtual double base_to_plugin_param(std::int32_t index, param_value val) const = 0;
  virtual param_value plugin_to_base_param(std::int32_t index, double val) const = 0;

  std::vector<inf::base::factory_preset>& factory_presets() 
  { return _factory_presets; }
  void add_any_param_listener(any_param_listener* listener)
  { _any_param_listeners.insert(listener); }
  void remove_any_param_listener(any_param_listener* listener)
  { _any_param_listeners.erase(listener); }
  void add_param_listener(std::int32_t param_index, param_listener* listener);
  void remove_param_listener(std::int32_t param_index, param_listener* listener);

  param_value base_value_at_index(std::int32_t param_index) const
  { return state()[param_index]; }
  param_value ui_value_at_index(std::int32_t param_index) const
  { return topology()->base_to_ui_value(param_index, base_value_at_index(param_index)); }
  param_value ui_value_at(part_id part, std::int32_t param) const
  { return ui_value_at_index(topology()->param_index(part, param)); }
  param_value base_value_at(part_id part, std::int32_t param) const
  { return base_value_at_index(topology()->param_index(part, param)); };
  param_value ui_value_at_id(std::int32_t param_id) const 
  { return ui_value_at_index(topology()->param_id_to_index.at(param_id)); }
  param_value base_value_at_id(std::int32_t param_id) const 
  { return base_value_at_index(topology()->param_id_to_index.at(param_id)); }
  param_value ui_value_at(std::int32_t part_type, std::int32_t part_index, std::int32_t param) const
  { return ui_value_at_index(topology()->param_index(part_id(part_type, part_index), param)); }
  param_value base_value_at(std::int32_t part_type, std::int32_t part_index, std::int32_t param) const
  { return base_value_at_index(topology()->param_index(part_id(part_type, part_index), param)); };
};

} // namespace inf::base
#endif // INF_BASE_PLUGIN_PLUGIN_CONTROLLER_HPP