#ifndef INF_BASE_FORMAT_CLAP_CLAP_SUPPORT_HPP
#define INF_BASE_FORMAT_CLAP_CLAP_SUPPORT_HPP

#include <cstdint>

namespace inf::base::format::clap
{

// Will this be enough ?
inline std::int32_t constexpr queue_size = 4096;

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