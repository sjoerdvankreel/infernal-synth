#ifndef SVN_VST_UI_GENERATOR_HPP
#define SVN_VST_UI_GENERATOR_HPP

#include <inf.base/topology/topology_info.hpp>
#include <inf.vst.ui/description/ui_description.hpp>
#include <vstgui/uidescription/rapidjson/include/rapidjson/document.h>

namespace inf::vst::ui {

rapidjson::Document
build_vstgui_json(
  inf::base::topology_info const& topology,
  controller_ui_description const& controller);

} // namespace inf::vst::ui
#endif // SVN_VST_UI_GENERATOR_HPP