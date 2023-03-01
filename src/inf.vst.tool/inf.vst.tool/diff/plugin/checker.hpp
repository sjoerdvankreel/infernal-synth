#ifndef INF_VST_TOOL_DIFF_PLUGIN_CHECKER_HPP
#define INF_VST_TOOL_DIFF_PLUGIN_CHECKER_HPP

#include <cstdint>

namespace inf::vst::tool::diff::plugin {

std::int32_t
check(char const* library1_path, char const* library2_path);

} // namespace inf::vst::tool::diff
#endif // INF_VST_TOOL_DIFF_PLUGIN_CHECKER_HPP