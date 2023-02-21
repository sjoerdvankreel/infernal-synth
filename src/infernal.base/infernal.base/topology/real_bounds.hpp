#ifndef SVN_BASE_TOPOLOGY_REAL_BOUNDS_HPP
#define SVN_BASE_TOPOLOGY_REAL_BOUNDS_HPP

#include <cmath>
#include <limits>
#include <cassert>
#include <cstdint>

namespace svn::base {

struct real_slope_t { enum value { linear, quadratic, decibel, count }; };
typedef real_slope_t::value real_slope;

// Dsp/display bounds for real valued params.
struct real_bounds
{
  float const min; // Minimum transformed value for real parameters.
  float const max; // Maximum transformed value for real parameters.
  float const linear_max; // For decibel scale.
  real_slope const slope; // Slope type.

  float to_linear(float val) const;
  float to_decibel(float val) const;
  float to_quadratic(float val) const;

  float from_linear(float val) const;
  float from_decibel(float val) const;
  float from_quadratic(float val) const;

  float to_range(float val) const;
  float from_range(float val) const;
  void to_range(float* vals, std::int32_t count) const;
  void from_range(float* vals, std::int32_t count) const;
  void to_range(float const* in, float* out, std::int32_t count) const;

  static inline real_bounds constexpr unit()
  { return { 0.0, 1.0, 1.0, real_slope::linear }; }
  static inline real_bounds constexpr linear(float min, float max)
  { return { min, max, max, real_slope::linear }; }
  static inline real_bounds constexpr quadratic(float min, float max)
  { return { min, max, max, real_slope::quadratic }; }
  static inline real_bounds decibel(float linear_max)
  { return { -std::numeric_limits<float>::infinity(), 20.0f * std::log10(linear_max), linear_max, real_slope::decibel }; }
};

inline float 
real_bounds::to_decibel(float val) const
{ return 20.0f * std::log10(val * linear_max); }
inline float real_bounds::to_linear(float val) const 
{ return min + (max - min) * val; }
inline float real_bounds::to_quadratic(float val) const
{ return min + (max - min) * val * val; }

inline float real_bounds::from_linear(float val) const
{ return (val - min) / (max - min); }
inline float real_bounds::from_quadratic(float val) const
{ return std::sqrt((val - min) / (max - min)); }
inline float real_bounds::from_decibel(float val) const
{ return std::exp(std::log(10.0f) * val / 20.0f) / linear_max; }

inline float
real_bounds::to_range(float val) const
{
  switch (slope)
  {
  case real_slope::linear: return to_linear(val);
  case real_slope::decibel: return to_decibel(val);
  case real_slope::quadratic: return to_quadratic(val);
  default: assert(false); return 0.0f;
  }
}

inline float
real_bounds::from_range(float val) const
{
  switch (slope)
  {
  case real_slope::linear: return from_linear(val);
  case real_slope::decibel: return from_decibel(val);
  case real_slope::quadratic: return from_quadratic(val);
  default: assert(false); return 0.0f;
  }
}

inline void
real_bounds::to_range(float* vals, std::int32_t count) const
{
  switch (slope)
  {
  case real_slope::linear: for (std::int32_t i = 0; i < count; i++) vals[i] = to_linear(vals[i]); break;
  case real_slope::decibel: for (std::int32_t i = 0; i < count; i++) vals[i] = to_decibel(vals[i]); break;
  case real_slope::quadratic: for (std::int32_t i = 0; i < count; i++) vals[i] = to_quadratic(vals[i]); break;
  default: assert(false); break;
  }
}

inline void
real_bounds::from_range(float* vals, std::int32_t count) const
{
  switch (slope)
  {
  case real_slope::linear: for (std::int32_t i = 0; i < count; i++) vals[i] = from_linear(vals[i]); break;
  case real_slope::decibel: for (std::int32_t i = 0; i < count; i++) vals[i] = from_decibel(vals[i]); break;
  case real_slope::quadratic: for (std::int32_t i = 0; i < count; i++) vals[i] = from_quadratic(vals[i]); break;
  default: assert(false); break;
  }
}

inline void
real_bounds::to_range(float const* in, float* out, std::int32_t count) const
{
  switch (slope)
  {
  case real_slope::linear: for (std::int32_t i = 0; i < count; i++) out[i] = to_linear(in[i]); break;
  case real_slope::decibel: for (std::int32_t i = 0; i < count; i++) out[i] = to_decibel(in[i]); break;
  case real_slope::quadratic: for (std::int32_t i = 0; i < count; i++) out[i] = to_quadratic(in[i]); break;
  default: assert(false); break;
  }
}

} // namespace svn::base
#endif // SVN_BASE_TOPOLOGY_REAL_BOUNDS_HPP
