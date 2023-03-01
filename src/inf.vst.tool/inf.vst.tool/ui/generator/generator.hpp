#ifndef INF_VST_TOOL_UI_GENERATOR_GENERATOR_HPP
#define INF_VST_TOOL_UI_GENERATOR_GENERATOR_HPP

#include <inf.base/topology/topology_info.hpp>
#include <inf.vst.tool/ui/description/description.hpp>
#include <vstgui/uidescription/rapidjson/include/rapidjson/document.h>

namespace inf::vst::tool::ui {

rapidjson::Document
build_vstgui_json(
  inf::base::topology_info const& topology,
  controller_ui_description const& controller);

} // namespace inf::vst::ui
#endif // INF_VST_TOOL_UI_GENERATOR_GENERATOR_HPP