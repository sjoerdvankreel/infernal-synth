#ifndef INF_PLUGIN_INFERNAL_SYNTH_SYNTH_CONFIG_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_SYNTH_CONFIG_HPP

#include <cstdint>

namespace inf::plugin::infernal_synth {

inline std::int32_t constexpr vosc_count = 4;
inline std::int32_t constexpr venv_count = 6;
inline std::int32_t constexpr vlfo_count = 6;
inline std::int32_t constexpr glfo_count = 6;
inline std::int32_t constexpr veffect_count = 10;
inline std::int32_t constexpr geffect_count = 10;
inline std::int32_t constexpr synth_polyphony = 32;

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_SYNTH_CONFIG_HPP
