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
  
// For factory presets and themes.
struct external_resource
{
  std::string name;
  std::string path;
};

// Controller base interface.
class plugin_controller
{
  static inline std::string const theme_key = "theme";
  static inline std::string const ui_size_key = "ui_size";
  static inline std::string const factory_preset_key = "factory_preset";
  static inline std::string const last_directory_key = "last_directory";

protected:
  std::vector<inf::base::param_value> _state;
  std::map<std::string, std::string> _meta_data;
  std::unique_ptr<inf::base::topology_info> _topology;
  std::set<any_param_listener*> _any_param_listeners = {};
  std::map<std::int32_t, std::set<param_listener*>> _param_listeners = {};

  plugin_controller(std::unique_ptr<inf::base::topology_info>&& topology) :
  _state(topology->params.size()), _topology(std::move(topology))
  { _topology->init_factory_preset(_state.data()); }

  void controller_param_changed(std::int32_t tag, param_value base_value);

public:
  inf::base::param_value const* state() const { return _state.data(); }
  inf::base::topology_info const* topology() const { return _topology.get(); }
  std::map<std::string, std::string>& meta_data() { return _meta_data; }
  std::map<std::string, std::string> const& meta_data() const { return _meta_data; }

  void init_patch();
  void clear_patch();
  void clear_part(part_id id);
  void copy_or_swap_part(part_id source, std::int32_t target, bool swap);

  virtual void restart() = 0;
  virtual std::string preset_file_extension() = 0;
  virtual void set_editor_width(std::int32_t width) = 0;
  virtual void load_component_state(param_value* state) = 0;
  virtual void save_preset(std::string const& path) = 0;
  virtual bool load_preset(std::string const& path, bool factory) = 0;

  virtual float editor_aspect_ratio() const = 0;
  virtual void* current_editor_window() const = 0;
  virtual std::int32_t editor_min_width() const = 0;
  virtual std::int32_t editor_max_width() const = 0;
  virtual std::int32_t editor_current_width() const = 0;
  virtual std::int32_t editor_font_scaling_min_width() const = 0;
  virtual std::int32_t editor_font_scaling_max_width() const = 0;
  virtual void copy_param(std::int32_t source_tag, std::int32_t target_tag) = 0;
  virtual void swap_param(std::int32_t source_tag, std::int32_t target_tag) = 0;
  virtual void editor_param_changed(std::int32_t index, param_value ui_value) = 0;

  virtual std::vector<char const*> ui_size_names() const = 0;
  virtual std::string default_theme_path(std::string const& plugin_file) const = 0;
  virtual std::vector<inf::base::external_resource> themes(std::string const& plugin_file) const = 0;
  virtual std::vector<inf::base::external_resource> factory_presets(std::string const& plugin_file) const = 0;

  std::string get_theme() { return _meta_data[theme_key]; }
  void set_theme(std::string const& theme) { _meta_data[theme_key] = theme; }
  std::string get_ui_size() { return _meta_data[ui_size_key]; }
  void set_ui_size(std::string const& ui_size) { _meta_data[ui_size_key] = ui_size; }
  std::string get_factory_preset() { return _meta_data[factory_preset_key]; }
  void set_factory_preset(std::string const& factory_preset) { _meta_data[factory_preset_key] = factory_preset; }
  std::string get_last_directory() { return _meta_data[last_directory_key]; }
  void set_last_directory(std::string const& last_directory) { _meta_data[last_directory_key] = last_directory; }

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