#ifndef SVN_BASE_SHARED_SUPPORT_HPP
#define SVN_BASE_SHARED_SUPPORT_HPP

#include <svn.base/processor/state.hpp>
#include <svn.base/topology/topology_info.hpp>

#include <cmath>
#include <chrono>
#include <cassert>
#include <cstdint>
#include <algorithm>

namespace svn::base {

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

} // namespace svn::base
#endif // SVN_BASE_SHARED_SUPPORT_HPP