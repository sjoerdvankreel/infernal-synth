#include <inf.base/plugin/plugin_controller.hpp>
#include <vector>

namespace inf::base {

std::map<std::string, std::string>& 
plugin_controller::meta_data()
{
  std::vector<std::string> unknown_keys;
  std::vector<std::string> known_keys;
  known_keys.push_back(theme_key);
  known_keys.push_back(ui_size_key);
  known_keys.push_back(factory_preset_key);
  known_keys.push_back(last_directory_key);
  for(auto iter = _meta_data.begin(); iter != _meta_data.end(); ++iter)
    if(std::find(known_keys.begin(), known_keys.end(), iter->first) == known_keys.end())
      unknown_keys.push_back(iter->first);
  for(std::size_t i = 0; i < unknown_keys.size(); i++)
    _meta_data.erase(unknown_keys[i]);
  return _meta_data;
}

void
plugin_controller::reloaded()
{
  for(auto iter = _reload_listeners.begin(); iter != _reload_listeners.end(); ++iter)
    (*iter)->plugin_reloaded();
}

void
plugin_controller::add_param_listener(std::int32_t param_index, param_listener* listener)
{
  auto iter = _param_listeners.find(param_index);
  if(iter == _param_listeners.end())
    _param_listeners[param_index] = std::set<param_listener*>();
  _param_listeners.find(param_index)->second.insert(listener);
}

void
plugin_controller::remove_param_listener(std::int32_t param_index, param_listener* listener)
{
  auto iter = _param_listeners.find(param_index);
  if (iter != _param_listeners.end()) iter->second.erase(listener);
}

void 
plugin_controller::controller_param_changed(std::int32_t tag, param_value base_value)
{
  std::int32_t index = topology()->param_id_to_index.at(tag);
  auto iter = _param_listeners.find(index);
  if(iter != _param_listeners.end())
  {
    param_value ui_value = topology()->base_to_ui_value(index, base_value);
    for(auto& listener: iter->second)
      listener->controller_param_changed(ui_value);
  }
  for(auto it = _any_param_listeners.begin(); it != _any_param_listeners.end(); ++it)
    (*it)->any_controller_param_changed(index);
}

void
plugin_controller::init_patch()
{
  std::vector<param_value> new_values(topology()->input_param_count, param_value());
  topology()->init_factory_preset(new_values.data());
  load_component_state(new_values.data());
}

void 
plugin_controller::clear_patch()
{
  std::vector<param_value> new_values(topology()->input_param_count, param_value());
  topology()->init_clear_patch(new_values.data());
  load_component_state(new_values.data());
}

void 
plugin_controller::clear_part(part_id id)
{
  std::int32_t param_start = topology()->param_bounds[id.type][id.index];
  std::int32_t param_count = topology()->static_parts[id.type].param_count;
  std::vector<param_value> new_values(state(), state() + topology()->params.size());
  topology()->init_param_defaults(new_values.data(), param_start, param_start + param_count);
  load_component_state(new_values.data());
}

void 
plugin_controller::copy_or_swap_part(part_id source, std::int32_t target, bool swap)
{
  std::int32_t target_start = topology()->param_bounds[source.type][target];
  std::int32_t source_start = topology()->param_bounds[source.type][source.index];
  std::int32_t param_count = topology()->static_parts[source.type].param_count;
  for (std::int32_t i = 0; i < param_count; i++)
  {
    std::int32_t source_tag = topology()->param_index_to_id[source_start + i];
    std::int32_t target_tag = topology()->param_index_to_id[target_start + i];
    if(swap) swap_param(source_tag, target_tag);
    else copy_param(source_tag, target_tag);
  }
  restart();
}

} // namespace inf::base