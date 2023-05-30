#ifndef INF_SYNTH_SYNTH_CONFIG_HPP
#define INF_SYNTH_SYNTH_CONFIG_HPP

#include <cstdint>

namespace inf::synth {

inline std::int32_t constexpr vosc_count = 4;
inline std::int32_t constexpr venv_count = 6;
inline std::int32_t constexpr vlfo_count = 6;
inline std::int32_t constexpr glfo_count = 6;
inline std::int32_t constexpr veffect_count = 8;
inline std::int32_t constexpr geffect_count = 8;
inline std::int32_t constexpr synth_polyphony = 32;

} // namespace inf::synth
#endif // INF_SYNTH_SYNTH_CONFIG_HPP
