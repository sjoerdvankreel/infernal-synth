#ifndef INF_PLUGIN_INFERNAL_SYNTH_SHARED_SCRATCH_SPACE_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_SHARED_SCRATCH_SPACE_HPP

#include <inf.base/shared/cont_storage.hpp>
#include <cstdint>

namespace inf::plugin::infernal_synth {

// Scratch space for anyone who wants it.
// Storage maintained by synth.
struct scratch_space
{
  static inline std::int32_t constexpr count_f32 = 8;
  static inline std::int32_t constexpr count_f64 = 6;

  base::cont_storage<float> storage_f32;
  base::cont_storage<double> storage_f64;

  // Interior pointers.
  scratch_space() = default;
  scratch_space(scratch_space const&) = delete;
  scratch_space& operator=(scratch_space const&) = delete;
  scratch_space(std::int32_t max_sample_count);
};

inline scratch_space::
scratch_space(std::int32_t max_sample_count) :
storage_f32(count_f32, max_sample_count),
storage_f64(count_f64, max_sample_count) {}

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_SHARED_SCRATCH_SPACE_HPP