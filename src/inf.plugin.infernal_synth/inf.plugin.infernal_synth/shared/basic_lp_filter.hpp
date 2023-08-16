#ifndef INF_PLUGIN_INFERNAL_SYNTH_SHARED_BASIC_LP_FILTER_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_SHARED_BASIC_LP_FILTER_HPP

#include <inf.base/shared/support.hpp>

namespace inf::plugin::infernal_synth {

// https://www.musicdsp.org/en/latest/Filters/117-one-pole-one-zero-lp-hp.html
struct basic_lp_filter
{
  float in1;
  float out1;
  float a, b;

  float next(float val);
  void init(float rate, float freq);
  basic_lp_filter(): in1(0.0f), out1(0.0f), a(0.0f), b(0.0f) {}
};

inline float 
basic_lp_filter::next(float val)
{
  float result = val * a + in1 * a + out1 * b;
  in1 = val;
  out1 = result;
  return result;
}

inline void 
basic_lp_filter::init(float rate, float freq)
{
  float w = 2.0f * rate;
  float angle = freq * 2.0f * base::pi32;
  float norm = 1.0f / (angle + w);
  b = (w - angle) * norm;
  a = angle * norm;
}

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_SHARED_BASIC_LP_FILTER_HPP