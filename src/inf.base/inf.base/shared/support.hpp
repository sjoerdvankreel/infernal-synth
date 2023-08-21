#ifndef INF_BASE_SHARED_SUPPORT_HPP
#define INF_BASE_SHARED_SUPPORT_HPP

#include <inf.base/plugin/state.hpp>
#include <inf.base/topology/topology_info.hpp>

#include <cmath>
#include <chrono>
#include <cassert>
#include <cstdint>
#include <algorithm>

namespace inf::base {

inline std::int32_t constexpr midi_note_c4 = 60;
inline std::int32_t constexpr stereo_channels = 2;

inline float constexpr sanity_epsilon = 1.0e-4f;
inline float constexpr pi32 = 3.14159265358979323846264338327950288f;
inline double constexpr pi64 = 3.14159265358979323846264338327950288f;

// Toggle daz/ftz.
std::uint64_t disable_denormals();
void restore_denormals(std::uint64_t state);

// Midi -> frequency lookup.
struct note_to_frequency_table_init
{
  static inline constexpr std::int32_t note_count = 12;
  static inline constexpr std::int32_t cent_count = 100;
  static inline constexpr std::int32_t octave_count = 12;
  static inline constexpr std::int32_t frequency_count = octave_count * note_count * cent_count;

  float table[frequency_count];
  note_to_frequency_table_init();
  static note_to_frequency_table_init const init;
};

inline std::uint64_t
next_pow2(std::uint64_t x)
{
  uint64_t result = 0;
  if (x == 0) return 0;
  if (x && !(x & (x - 1))) return x;
  while (x != 0) x >>= 1, result++;
  return 1ULL << result;
}

template <class T> inline T
sanity(T val)
{
  assert(!std::isnan(val));
  assert(!std::isinf(val));
  assert(std::fpclassify(val) != FP_SUBNORMAL);
  return val;
}

template <class T> inline T
sanity_unipolar(T val)
{
  sanity(val);
  assert(val <= 1.0f + sanity_epsilon);
  assert(val >= 0.0f - sanity_epsilon);
  return val;
}

template <class T> inline T
sanity_bipolar(T val)
{
  sanity(val);
  assert(val <= 1.0f + sanity_epsilon);
  assert(val >= -1.0f - sanity_epsilon);
  return val;
}

inline std::int64_t
performance_counter()
{
  auto now = std::chrono::high_resolution_clock::now();
  return now.time_since_epoch().count();
}

// E.g. "3/4" -> hz.
inline float
timesig_to_frequency(float bpm, float timesig_val)
{ return bpm / (60.0f * 4.0f * timesig_val) ; }
// E.g. "3/4" -> sample count.
inline float
timesig_to_samples(float sample_rate, float bpm, float timesig_val)
{ return sample_rate * 60.0f * 4.0f / bpm * timesig_val; }

// Don't actually clip, host may still attenuate.
inline bool
audio_may_clip(float const* const* audio, 
  std::int32_t channel_count, std::int32_t sample_count)
{
  for (std::int32_t c = 0; c < channel_count; c++)
    for (std::int32_t s = 0; s < sample_count; s++)
      if(audio[c][s] < -1.0f || audio[c][s] > 1.0f) return true;
  return false;
}

// Audio mix.
inline void
add_audio(float* const* x, float const* const* y, 
  std::int32_t channel_count, std::int32_t sample_count, std::int32_t x_offset)
{
  for (std::int32_t c = 0; c < channel_count; c++)
    for (std::int32_t s = 0; s < sample_count; s++)
      x[c][x_offset + s] += y[c][s];
}

// Note including cents.
inline float
note_to_frequency(float midi)
{ return 440.0f * std::pow(2.0f, (midi - 69.0f) / 12.0f); }
inline float
note_to_frequency(std::int32_t midi)
{ return note_to_frequency(static_cast<float>(midi)); }
inline float
note_to_frequency_table(float midi)
{
  float midi_cent = midi * note_to_frequency_table_init::cent_count;
  std::int32_t cent_low = std::max(0, static_cast<std::int32_t>(midi_cent));
  std::int32_t cent_high = std::min(note_to_frequency_table_init::frequency_count - 1, cent_low + 1);
  float freq_low = note_to_frequency_table_init::init.table[cent_low];
  float freq_high = note_to_frequency_table_init::init.table[cent_high];
  float mix = midi_cent - cent_low;
  return (1.0f - mix) * freq_low + mix * freq_high;
}

// These are helper functions to go from base (which is either discrete or 0..1)
// to both VST3 and CLAP normalized values (always 0..1 for vst3, 0..1 for clap real or
// min-max for clap discrete) to display values (e.g. -6..+6) to text values (e.g. "-6dB""). 
// CLAP doesnt really need this 0-1 normalization but its easier to keep parity with vst3.

inline double 
discrete_to_format_normalized(
  inf::base::param_info const& info, bool discrete_is_normalized, std::int32_t val)
{ 
  assert(info.descriptor->data.type != inf::base::param_type::real);
  if(!discrete_is_normalized) return val;
  std::int32_t min = info.descriptor->data.discrete.min;
  std::int32_t max = info.descriptor->data.discrete.effective_max(info.part_index);
  return static_cast<double>(val - min) / (max - min); 
}

inline std::int32_t 
format_normalized_to_discrete(
  inf::base::param_info const& info, bool discrete_is_normalized, double val)
{ 
  assert(info.descriptor->data.type != inf::base::param_type::real);
  if (!discrete_is_normalized) return static_cast<std::int32_t>(val);
  std::int32_t min = info.descriptor->data.discrete.min;
  std::int32_t max = info.descriptor->data.discrete.effective_max(info.part_index);
  return min + std::clamp(static_cast<std::int32_t>(val * (max - min + 1)), 0, max - min);
}

inline double
format_normalized_to_display(
  inf::base::param_info const& info, bool discrete_is_normalized, double val)
{
  switch (info.descriptor->data.type)
  {
  case param_type::real: return info.descriptor->data.real.display.to_range(static_cast<float>(val));
  default: return format_normalized_to_discrete(info, discrete_is_normalized, val);
  }
}

inline double
display_to_format_normalized(
  inf::base::param_info const& info, bool discrete_is_normalized, double val)
{
  switch (info.descriptor->data.type)
  {
  case param_type::real: return info.descriptor->data.real.display.from_range(static_cast<float>(val));
  default: return discrete_to_format_normalized(info, discrete_is_normalized, static_cast<std::int32_t>(val));
  }
}

inline double 
base_to_format_normalized(
  inf::base::topology_info const* topology, bool discrete_is_normalized, std::int32_t param, inf::base::param_value val)
{
  auto const& info = topology->params[param];
  if (info.descriptor->data.type == inf::base::param_type::real) return val.real;
  return discrete_to_format_normalized(info, discrete_is_normalized, val.discrete);
}

inline inf::base::param_value
format_normalized_to_base(
  inf::base::topology_info const* topology, bool discrete_is_normalized, std::int32_t param, double val)
{ 
  auto const& info = topology->params[param];
  if(info.descriptor->data.type == inf::base::param_type::real) return inf::base::param_value(static_cast<float>(val));
  return inf::base::param_value(format_normalized_to_discrete(info, discrete_is_normalized, val));
}

inline std::string
format_normalized_to_text(
  inf::base::param_info const& info, bool discrete_is_normalized, double val)
{
  param_value value;
  switch (info.descriptor->data.type)
  {
  case param_type::real: value.real = static_cast<float>(format_normalized_to_display(info, discrete_is_normalized, val)); break;
  default: value.discrete = format_normalized_to_discrete(info, discrete_is_normalized, val); break;
  }
  return info.descriptor->data.format(false, value);
}

inline bool
text_to_format_normalized(
  inf::base::param_info const& info, bool discrete_is_normalized, char const* text, double& result)
{
  param_value value;
  if (!info.descriptor->data.parse(false, info.part_index, text, value)) return false;
  switch (info.descriptor->data.type)
  {
  case param_type::real: result = display_to_format_normalized(info, discrete_is_normalized, value.real); break;
  default: result = discrete_to_format_normalized(info, discrete_is_normalized, value.discrete); break;
  }
  return true;
}

inline double
param_default_to_format_normalized(param_info const& info, bool discrete_is_normalized)
{
  switch (info.descriptor->data.type)
  {
  case param_type::real: return info.descriptor->data.real.default_;
  default: return discrete_to_format_normalized(info, discrete_is_normalized, info.descriptor->data.discrete.default_);
  }
}

} // namespace inf::base
#endif // INF_BASE_SHARED_SUPPORT_HPP