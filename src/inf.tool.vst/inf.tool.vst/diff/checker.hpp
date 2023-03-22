#ifndef INF_TOOL_VST_DIFF_CHECKER_HPP
#define INF_TOOL_VST_DIFF_CHECKER_HPP

#include <cstdint>

namespace inf::tool::vst::diff {

std::int32_t
check_plugin(
  char const* library1_path, 
  char const* library2_path);

std::int32_t
check_preset_file(
  char const* library1_path, char const* preset1_path,
  char const* library2_path, char const* preset2_path);

std::int32_t
check_preset_folder(
  char const* library1_path, char const* folder1_path, 
  char const* library2_path, char const* folder2_path);

} // namespace inf::tool::vst::diff
#endif // INF_TOOL_VST_DIFF_CHECKER_HPP