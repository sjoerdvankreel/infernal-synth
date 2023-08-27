#ifndef INF_BASE_FORMAT_CLAP_CLAP_SUPPORT_HPP
#define INF_BASE_FORMAT_CLAP_CLAP_SUPPORT_HPP

#include <cassert>
#include <cstdint>

namespace inf::base::format::clap
{

// This is enough to push all of infernal synths parameters
// around about 10 times for now. But prefer some way to make
// this dynamic dependent on topology_info.
inline std::int32_t constexpr queue_size = 16384;

inline std::uint32_t constexpr midi_cc = 176;
inline std::uint32_t constexpr midi_cc_mod_wheel = 1;
inline std::uint32_t constexpr midi_cc_channel_volume = 7;
inline std::uint32_t constexpr midi_channel_pressure = 0xD0;
inline std::uint32_t constexpr midi_pitch_bend = 0xE0;

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

// https://fmslogo.sourceforge.io/manual/midi-table.html
inline double
midi_to_normalized(std::uint8_t msg, std::uint8_t data1, std::uint8_t data2)
{
  switch (msg)
  {
  case midi_cc: return data2 / 127.0;
  case midi_channel_pressure: return data1 / 127.0;
  case midi_pitch_bend:return ((data2 << 7) | data1) / static_cast<double>(1U << 14);
  default: assert(false); return 0.0;
  }
}

} // inf::base::format::clap
#endif // INF_BASE_FORMAT_CLAP_CLAP_SUPPORT_HPP