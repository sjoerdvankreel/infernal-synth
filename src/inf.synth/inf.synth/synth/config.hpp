#ifndef INF_SYNTH_SYNTH_CONFIG_HPP
#define INF_SYNTH_SYNTH_CONFIG_HPP

#include <cstdint>

namespace inf::synth {

inline std::int32_t constexpr vosc_count = 4;
inline std::int32_t constexpr venv_count = 6;
inline std::int32_t constexpr vlfo_count = 6;
inline std::int32_t constexpr glfo_count = 6;
inline std::int32_t constexpr veffect_count = 10;
inline std::int32_t constexpr geffect_count = 10;
inline std::int32_t constexpr vcv_bank_count = 7;
inline std::int32_t constexpr gcv_bank_count = 4;
inline std::int32_t constexpr vaudio_bank_count = 4;
inline std::int32_t constexpr gaudio_bank_count = 3;

inline std::int32_t constexpr synth_polyphony = 32;
inline std::int32_t constexpr synth_max_ui_height = 770;

} // namespace inf::synth
#endif // INF_SYNTH_SYNTH_CONFIG_HPP
