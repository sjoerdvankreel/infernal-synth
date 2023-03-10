#ifndef INF_VST_TOOL_DIFF_CHECKER_HPP
#define INF_VST_TOOL_DIFF_CHECKER_HPP

#include <cstdint>

namespace inf::vst::tool::diff {

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

} // namespace inf::vst::tool::diff
#endif // INF_VST_TOOL_DIFF_CHECKER_HPP