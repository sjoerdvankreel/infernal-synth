#ifndef INF_TOOL_VST_UI_GENERATOR_GENERATOR_HPP
#define INF_TOOL_VST_UI_GENERATOR_GENERATOR_HPP

#include <cstdint>

namespace inf::tool::vst::ui {

std::int32_t
write_vstgui_json(char const* library_path, char const* output_path);

} // namespace inf::vst::ui
#endif // INF_TOOL_VST_UI_GENERATOR_GENERATOR_HPP