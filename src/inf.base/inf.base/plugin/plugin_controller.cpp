#include <inf.base/plugin/plugin_controller.hpp>

namespace inf::base {

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

} // namespace inf::base