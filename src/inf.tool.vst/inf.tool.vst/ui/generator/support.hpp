#ifndef INF_TOOL_VST_UI_GENERATOR_SUPPORT_HPP
#define INF_TOOL_VST_UI_GENERATOR_SUPPORT_HPP

#include <inf.base/topology/topology_info.hpp>
#include <inf.tool.vst/ui/description/support.hpp>
#include <inf.tool.vst/ui/description/description.hpp>

#include <vstgui/uidescription/rapidjson/include/rapidjson/document.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/rapidjson.h>
#include <string>

namespace inf::tool::vst::ui {

std::string to_machine_friendly(std::string const& val);
std::string size_to_string(std::int32_t w, std::int32_t h);
std::string print_rgba_hex(inf::base::ui_color const& color);
std::string get_control_tag(inf::base::part_info const& part, inf::base::param_info const& param);

inline std::string
get_color_name(std::string const& part_name, inf::base::named_ui_color const& color)
{ return to_machine_friendly("color_" + part_name + "_" + color.name); }
inline std::string
get_color_name(inf::base::part_info const& info, inf::base::named_ui_color const& color)
{ return get_color_name(info.descriptor->static_name.short_, color); }
inline std::string
get_color_name(part_type_ui_description const& desc, inf::base::named_ui_color const& color)
{ return get_color_name(desc.part_name, color); }

inline std::string
get_color_value(inf::base::named_ui_color const& color)
{ return "#" + print_rgba_hex(color.color); }

rapidjson::Value& add_member(
  rapidjson::Value& container, std::string const& key,
  std::string const& value, rapidjson::Document::AllocatorType& allocator);
rapidjson::Value& add_member(
  rapidjson::Value& container, std::string const& key,
  rapidjson::Value const& value, rapidjson::Document::AllocatorType& allocator);

void add_attribute(
  rapidjson::Value& container, std::string const& key,
  std::string const& value, rapidjson::Document::AllocatorType& allocator);
void add_child(
  rapidjson::Value& container, std::string const& key,
  rapidjson::Value const& value, rapidjson::Document::AllocatorType& allocator);

} // namespace inf::tool::vst::ui
#endif // INF_TOOL_VST_UI_GENERATOR_SUPPORT_HPP