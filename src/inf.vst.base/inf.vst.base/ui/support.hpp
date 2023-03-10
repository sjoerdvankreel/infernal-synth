#ifndef INF_VST_BASE_UI_SUPPORT_HPP
#define INF_VST_BASE_UI_SUPPORT_HPP

#include <inf.base/topology/part_descriptor.hpp>
#include <inf.base/topology/part_ui_descriptor.hpp>

#include <vstgui/lib/ccolor.h>
#include <vstgui/uidescription/uiviewcreator.h>

#include <string>
#include <cstdint>

namespace inf::vst::base {

inline VSTGUI::CColor
to_vst_color(inf::base::ui_color const& color)
{ return VSTGUI::CColor(color.r, color.g, color.b, color.a); }
inline inf::base::ui_color
from_vst_color(VSTGUI::CColor const& color)
{ return inf::base::ui_color(color.alpha, color.red, color.green, color.blue); }

inf::base::ui_color
from_vst_color_name(std::string const* name, VSTGUI::IUIDescription const* desc);

} // namespace inf::vst::base
#endif // INF_VST_BASE_UI_SUPPORT_HPP