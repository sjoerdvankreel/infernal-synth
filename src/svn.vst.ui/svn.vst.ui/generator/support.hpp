#ifndef SVN_VST_UI_SUPPORT_HPP
#define SVN_VST_UI_SUPPORT_HPP

#include <svn.base/topology/topology_info.hpp>
#include <svn.vst.ui/description/support.hpp>
#include <svn.vst.ui/description/ui_description.hpp>

#include <vstgui/uidescription/rapidjson/include/rapidjson/document.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/rapidjson.h>
#include <string>

namespace svn::vst::ui {

std::string size_to_string(std::int32_t w, std::int32_t h);
std::string print_rgba_hex(svn::base::ui_color const& color);
std::string get_control_tag(svn::base::part_info const& part, svn::base::param_info const& param);

inline std::string
get_color_name(svn::base::ui_color const& color)
{ return "color_" + print_rgba_hex(color); }
inline std::string
get_color_value(svn::base::ui_color const& color)
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

} // namespace svn::vst::ui
#endif // SVN_VST_UI_SUPPORT_HPP