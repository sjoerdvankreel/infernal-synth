#ifndef INF_VST_TOOL_DIFF_CHECKER_HPP
#define INF_VST_TOOL_DIFF_CHECKER_HPP

#include <cstdint>

namespace inf::vst::tool::diff {

std::int32_t
diff_plugin(char const* library1_path, char const* library2_path);

} // namespace inf::vst::tool::diff
#endif // INF_VST_TOOL_DIFF_CHECKER_HPP