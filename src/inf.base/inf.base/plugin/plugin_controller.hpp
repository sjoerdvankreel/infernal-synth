#ifndef INF_BASE_PLUGIN_PLUGIN_CONTROLLER_HPP
#define INF_BASE_PLUGIN_PLUGIN_CONTROLLER_HPP

#include <inf.base/topology/topology_info.hpp>
#include <juce_data_structures/juce_data_structures.h>

#include <set>
#include <map>
#include <vector>
#include <string>
#include <cstdint>
#include <functional>

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

// On load/load preset.
class reload_listener
{
public:
  virtual void plugin_reloaded() = 0;
};
  
// For factory presets and themes.
struct external_resource
{
  std::string name;
  std::string path;
};

// VST3 context menu feature.
class host_context_menu
{
public:
  virtual ~host_context_menu() {};
  virtual std::int32_t item_count() const = 0;
  virtual void item_clicked(std::int32_t index) = 0;
  virtual void get_item(std::int32_t index, std::string& name, bool& enabled, bool& checked) const = 0;
};

struct editor_properties
{
  float aspect_ratio;
  char const* const* ui_size_names;
  std::int32_t min_width;
  std::int32_t max_width;
  std::int32_t font_scaling_min_width;
  std::int32_t font_scaling_max_width;
};

// Controller base interface.
class plugin_controller
{
  static inline std::string const global_meta_theme_key = "theme";
  static inline std::string const global_meta_ui_size_key = "ui_size";
  static inline std::string const global_meta_last_directory_key = "last_directory";
  static inline std::string const patch_meta_factory_preset_key = "factory_preset";

  std::int32_t _editor_width = 0;
  juce::InterProcessLock _global_meta_lock;
  juce::PropertiesFile::Options global_meta_options();
  std::string get_global_meta(std::string const& key);
  void set_global_meta(std::string const& key, std::string const& value);

protected:
  std::vector<inf::base::param_value> _state;
  std::map<std::string, std::string> _patch_meta_data;
  std::unique_ptr<inf::base::topology_info> _topology;
  std::set<reload_listener*> _reload_listeners = {};
  std::set<any_param_listener*> _any_param_listeners = {};
  std::map<std::int32_t, std::set<param_listener*>> _param_listeners = {};

  plugin_controller(std::unique_ptr<inf::base::topology_info>&& topology) :
  _global_meta_lock(juce::String(topology->plugin_name())),
  _state(topology->params.size()), _topology(std::move(topology))
  { _topology->init_factory_preset(_state.data()); }

  void reloaded();
  void controller_param_changed(std::int32_t tag, param_value base_value);

public:
  std::map<std::string, std::string>& patch_meta_data();
  inf::base::param_value const* state() const { return _state.data(); }
  inf::base::topology_info const* topology() const { return _topology.get(); }

  void init_patch();
  void clear_patch();
  void clear_part(part_id id);
  void copy_or_swap_part(part_id source, std::int32_t target, bool swap);

  virtual void restart() = 0;
  virtual void* current_editor_window() const = 0;
  virtual void reload_editor(std::int32_t width) = 0;
  virtual editor_properties get_editor_properties() const = 0;

  virtual std::string preset_file_extension() = 0;
  virtual void load_component_state(param_value* state) = 0;
  virtual void save_preset(std::string const& path) = 0;
  virtual bool load_preset(std::string const& path) = 0;

  virtual void copy_param(std::int32_t source_tag, std::int32_t target_tag) = 0;
  virtual void swap_param(std::int32_t source_tag, std::int32_t target_tag) = 0;
  virtual void editor_param_changed(std::int32_t index, param_value ui_value) = 0;

  virtual std::string default_theme_path(std::string const& plugin_file) const = 0;
  virtual std::vector<inf::base::external_resource> themes(std::string const& plugin_file) const = 0;
  virtual std::unique_ptr<host_context_menu> host_menu_for_param_index(std::int32_t param_index) const = 0;
  virtual std::vector<inf::base::external_resource> factory_presets(std::string const& plugin_file) const = 0;

  void add_reload_listener(reload_listener* listener)
  { _reload_listeners.insert(listener); }
  void remove_reload_listener(reload_listener* listener)
  { _reload_listeners.erase(listener); }
  void add_any_param_listener(any_param_listener* listener)
  { _any_param_listeners.insert(listener); }
  void remove_any_param_listener(any_param_listener* listener)
  { _any_param_listeners.erase(listener); }
  void add_param_listener(std::int32_t param_index, param_listener* listener);
  void remove_param_listener(std::int32_t param_index, param_listener* listener);

  std::int32_t editor_current_width() const { return _editor_width; }
  void editor_current_width(std::int32_t editor_width) { _editor_width = editor_width; }

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

  std::string get_theme() { return get_global_meta(global_meta_theme_key); }
  void set_theme(std::string const& theme) { set_global_meta(global_meta_theme_key, theme); }
  std::string get_ui_size() { return get_global_meta(global_meta_ui_size_key); }
  void set_ui_size(std::string const& ui_size) { set_global_meta(global_meta_ui_size_key, ui_size); }
  std::string get_last_directory() { return get_global_meta(global_meta_last_directory_key); }
  void set_last_directory(std::string const& last_directory) { set_global_meta(global_meta_last_directory_key, last_directory); }
  std::string get_factory_preset() { return _patch_meta_data[patch_meta_factory_preset_key]; }
  void set_factory_preset(std::string const& factory_preset) { _patch_meta_data[patch_meta_factory_preset_key] = factory_preset; }
};

} // namespace inf::base
#endif // INF_BASE_PLUGIN_PLUGIN_CONTROLLER_HPP