#include <inf.base/plugin/plugin_controller.hpp>

namespace inf::base {

void 
plugin_controller::controller_param_changed(std::int32_t tag, param_value base_value)
{
  auto iter = _param_listeners.find(topology()->param_id_to_index.at(tag));
  if(iter != _param_listeners.end())
  {
    param_value ui_value = base_value;
    auto desc = topology()->params[topology()->param_id_to_index.at(tag)];
    if(desc.descriptor->data.type == param_type::real)
      ui_value.real = desc.descriptor->data.real.display.to_range(base_value.real);
    for(auto& listener: iter->second)
      listener->controller_param_changed(ui_value);
  }
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