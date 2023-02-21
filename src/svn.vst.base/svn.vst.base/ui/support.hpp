#ifndef SVN_VST_BASE_UI_SUPPORT_HPP
#define SVN_VST_BASE_UI_SUPPORT_HPP

#include <svn.base/topology/part_descriptor.hpp>
#include <svn.base/topology/part_ui_descriptor.hpp>

#include <vstgui/lib/ccolor.h>
#include <vstgui/uidescription/uiviewcreator.h>

#include <string>
#include <cstdint>

namespace svn::vst::base {

inline VSTGUI::CColor
to_vst_color(svn::base::ui_color const& color)
{ return VSTGUI::CColor(color.r, color.g, color.b, color.a); }
inline svn::base::ui_color
from_vst_color(VSTGUI::CColor const& color)
{ return svn::base::ui_color(color.alpha, color.red, color.green, color.blue); }

svn::base::ui_color
from_vst_color_name(std::string const* name, VSTGUI::IUIDescription const* desc);

} // namespace svn::vst::base
#endif // SVN_VST_BASE_UI_SUPPORT_HPP