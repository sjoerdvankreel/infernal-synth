#include <inf.vst.ui/generator/support.hpp>
#include <inf.base/topology/param_descriptor.hpp>

#include <sstream>

using namespace rapidjson;
using namespace inf::base;

namespace inf::vst::ui {

std::string
print_rgba_hex(ui_color const& color)
{
  std::ostringstream oss;
  oss << std::hex << std::uppercase;
  oss << ((color.r >> 4) & 0xF);
  oss << (color.r & 0xF);
  oss << ((color.g >> 4) & 0xF);
  oss << (color.g & 0xF);
  oss << ((color.b >> 4) & 0xF);
  oss << (color.b & 0xF);
  oss << ((color.a >> 4) & 0xF);
  oss << (color.a & 0xF);
  return oss.str();
}

std::string
size_to_string(std::int32_t w, std::int32_t h)
{
  std::string result;
  result += std::to_string(w);
  result += ", ";
  result += std::to_string(h);
  return result;
}

Value&
add_member(Value& container, std::string const& key,
  Value const& value, Document::AllocatorType& allocator)
{
  Value key_copy(key.c_str(), allocator);
  Value value_copy(value, allocator);
  return container.AddMember(key_copy, value_copy, allocator);
}

Value&
add_member(Value& container, std::string const& key,
  std::string const& value, Document::AllocatorType& allocator)
{
  Value key_copy(key.c_str(), allocator);
  Value value_copy(value.c_str(), allocator);
  return container.AddMember(key_copy, value_copy, allocator);
}

void
add_child(
  Value& container, std::string const& key,
  Value const& value, Document::AllocatorType& allocator)
{
  if (!container.HasMember("children"))
    container.AddMember("children", Value(kObjectType), allocator);
  add_member(container["children"], key, value, allocator);
}

void
add_attribute(
  Value& container, std::string const& key,
  std::string const& value, Document::AllocatorType& allocator)
{
  if (!container.HasMember("attributes"))
    container.AddMember("attributes", Value(kObjectType), allocator);
  add_member(container["attributes"], key, value, allocator);
}
 
std::string
get_control_tag(part_info const& part, param_info const& param)
{
  std::string result = part.descriptor->static_name.detail;
  result += "_" + std::to_string(part.type_index);
  result += "_" + std::string(param.descriptor->data.static_name.detail);
  for (std::size_t c = 0; c < result.length(); c++)
  {
    if (std::isspace(result[c])) result[c] = '_';
    else result[c] = static_cast<char>(std::tolower(result[c]));
  }
  return result;
}

} // namespace inf::vst::ui