#ifndef INF_BASE_UI_SHARED_REAL_BOUNDS_RANGE_HPP
#define INF_BASE_UI_SHARED_REAL_BOUNDS_RANGE_HPP

#include <inf.base/topology/real_bounds.hpp>
#include <juce_gui_basics/juce_gui_basics.h>
#include <algorithm>

namespace inf::base::ui {

// Mapping real_bounds onto juce.
class real_bounds_range:
public juce::NormalisableRange<double>
{
public:
  real_bounds_range(inf::base::real_bounds const& bounds):
  NormalisableRange(bounds.min, bounds.max,
    [bounds](double, double, double value_in_01) { 
      return bounds.to_range(static_cast<float>(value_in_01)); },
    [bounds](double, double, double value_in_range) { 
      return std::clamp(bounds.from_range(static_cast<float>(value_in_range)), 0.0f, 1.0f); }) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_SHARED_REAL_BOUNDS_RANGE_HPP