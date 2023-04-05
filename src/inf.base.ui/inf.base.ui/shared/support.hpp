#ifndef INF_BASE_UI_SHARED_SUPPORT_HPP
#define INF_BASE_UI_SHARED_SUPPORT_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <inf.base/topology/part_descriptor.hpp>
#include <inf.base/topology/part_ui_descriptor.hpp>
#include <inf.base.ui/shared/plugin_ui_editor.hpp>

#include <vstgui/lib/ccolor.h>
#include <vstgui/lib/cframe.h>
#include <vstgui/uidescription/uiviewcreator.h>

#include <string>
#include <vector>
#include <cstdint>

namespace inf::base::ui {

inline VSTGUI::CColor
to_vst_color(inf::base::ui_color const& color)
{ return VSTGUI::CColor(color.r, color.g, color.b, color.a); }
inline inf::base::ui_color
from_vst_color(VSTGUI::CColor const& color)
{ return inf::base::ui_color(color.alpha, color.red, color.green, color.blue); }
inline plugin_ui_editor*
find_editor(VSTGUI::CView const* view)
{ return dynamic_cast<plugin_ui_editor*>(view->getFrame()->getEditor()); }

std::vector<VSTGUI::CMenuItem*>
create_context_menu(plugin_ui_editor* controller);
inf::base::ui_color
from_vst_color_name(std::string const* name, VSTGUI::IUIDescription const* desc);

} // namespace inf::base::ui
#endif // INF_BASE_UI_SHARED_SUPPORT_HPP