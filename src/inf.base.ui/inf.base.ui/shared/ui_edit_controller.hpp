#ifndef INF_BASE_UI_SHARED_UI_EDIT_CONTROLLER_HPP
#define INF_BASE_UI_SHARED_UI_EDIT_CONTROLLER_HPP

#include <inf.base/topology/topology_info.hpp>
#include <vstgui4/vstgui/lib/cframe.h>
#include <cstdint>

namespace inf::base::ui {

class ui_edit_controller :
public VSTGUI::VSTGUIEditorInterface
{
public:
  virtual param_value* state() const = 0;
  virtual topology_info const* topology() const = 0;
  virtual void update_dependent_visibility(std::int32_t param_id) const = 0;
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_SHARED_UI_EDIT_CONTROLLER_HPP