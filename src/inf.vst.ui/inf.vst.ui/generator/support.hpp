#ifndef INF_VST_UI_SUPPORT_HPP
#define INF_VST_UI_SUPPORT_HPP

#include <inf.base/topology/topology_info.hpp>
#include <inf.vst.ui/description/support.hpp>
#include <inf.vst.ui/description/ui_description.hpp>

#include <vstgui/uidescription/rapidjson/include/rapidjson/document.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/rapidjson.h>
#include <string>

namespace inf::vst::ui {

std::string size_to_string(std::int32_t w, std::int32_t h);
std::string print_rgba_hex(inf::base::ui_color const& color);
std::string get_control_tag(inf::base::part_info const& part, inf::base::param_info const& param);

inline std::string
get_color_name(inf::base::ui_color const& color)
{ return "color_" + print_rgba_hex(color); }
inline std::string
get_color_value(inf::base::ui_color const& color)
{ return "#" + print_rgba_hex(color); }

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

} // namespace inf::vst::ui
#endif // INF_VST_UI_SUPPORT_HPP