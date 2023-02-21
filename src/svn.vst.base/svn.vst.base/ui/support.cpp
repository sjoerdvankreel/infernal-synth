#include <svn.vst.base/ui/support.hpp>

using namespace VSTGUI;

namespace svn::vst::base {

svn::base::ui_color
from_vst_color_name(std::string const* name, IUIDescription const* desc)
{
  CColor color;
  assert(name != nullptr);
  bool ok = UIViewCreator::stringToColor(name, color, desc);
  (void)ok;
  assert(ok);
  return from_vst_color(color);
}

} // namespace svn::vst::base