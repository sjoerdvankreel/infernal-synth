#ifndef INF_VST_TOOL_UI_GENERATOR_GENERATOR_HPP
#define INF_VST_TOOL_UI_GENERATOR_GENERATOR_HPP

#include <cstdint>

namespace inf::vst::tool::ui {

std::int32_t
write_vstgui_json(char const* library_path, char const* output_path);

} // namespace inf::vst::ui
#endif // INF_VST_TOOL_UI_GENERATOR_GENERATOR_HPP