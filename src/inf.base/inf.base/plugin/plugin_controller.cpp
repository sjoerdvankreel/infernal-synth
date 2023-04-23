#include <inf.base/plugin/plugin_controller.hpp>

namespace inf::base {

void 
plugin_controller::controller_changed_param(std::int32_t tag, param_value value)
{
  auto iter = _param_listeners.find(topology()->param_id_to_index.at(tag));
  if(iter != _param_listeners.end())
    for(auto& listener: iter->second)
      listener->param_changed(value);
}

void
plugin_controller::remove_param_listener(std::int32_t param_index, param_listener* listener)
{
  auto iter = _param_listeners.find(param_index);
  if (iter != _param_listeners.end()) iter->second.erase(listener);
}

void
plugin_controller::add_param_listener(std::int32_t param_index, param_listener* listener)
{
  auto iter = _param_listeners.find(param_index);
  if(iter == _param_listeners.end())
    _param_listeners[param_index] = std::set<param_listener*>();
  _param_listeners.find(param_index)->second.insert(listener);
}

} // namespace inf::base