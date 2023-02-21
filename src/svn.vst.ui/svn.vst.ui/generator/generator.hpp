#ifndef SVN_VST_UI_GENERATOR_HPP
#define SVN_VST_UI_GENERATOR_HPP

#include <svn.base/topology/topology_info.hpp>
#include <svn.vst.ui/description/ui_description.hpp>
#include <vstgui/uidescription/rapidjson/include/rapidjson/document.h>

namespace svn::vst::ui {

rapidjson::Document
build_vstgui_json(
  svn::base::topology_info const& topology,
  controller_ui_description const& controller);

} // namespace svn::vst::ui
#endif // SVN_VST_UI_GENERATOR_HPP