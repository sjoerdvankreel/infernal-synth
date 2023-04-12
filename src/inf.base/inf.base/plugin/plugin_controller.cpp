#include <inf.base/plugin/plugin_controller.hpp>

namespace inf::base {

void 
plugin_controller::controller_changed_param(std::int32_t tag, param_value value)
{
  auto iter = _param_listeners.find(topology()->param_id_to_index.at(tag));
  if(iter != _param_listeners.end()) iter->second->param_changed(value);
}

} // namespace inf::base