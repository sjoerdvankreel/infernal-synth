#ifndef INF_BASE_FORMAT_CLAP_CLAP_SUPPORT_HPP
#define INF_BASE_FORMAT_CLAP_CLAP_SUPPORT_HPP

#include <cstdint>

namespace inf::base::format::clap
{

// This is enough to push all of infernal synths parameters
// around about 10 times for now. But prefer some way to make
// this dynamic dependent on topology_info.
inline std::int32_t constexpr queue_size = 16384;

inline std::uint32_t constexpr midi_cc_mod_wheel = 1;
inline std::uint32_t constexpr midi_cc_channel_volume = 7;
inline std::uint32_t constexpr midi_cc_channel_pressure = 0xD0;
inline std::uint32_t constexpr midi_cc_pitch_bend = 0xE0;

struct audio_to_main_msg
{
  std::int32_t index;
  double value;
};

struct main_to_audio_msg
{
  std::int32_t index;
  enum { begin_edit, end_edit, adjust_value } type;
  double value;
};

} // inf::base::format::clap
#endif // INF_BASE_FORMAT_CLAP_CLAP_SUPPORT_HPP