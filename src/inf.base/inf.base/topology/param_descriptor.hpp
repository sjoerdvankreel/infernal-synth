#ifndef INF_BASE_TOPOLOGY_PARAM_DESCRIPTOR_HPP
#define INF_BASE_TOPOLOGY_PARAM_DESCRIPTOR_HPP

#include <inf.base/shared/state.hpp>
#include <inf.base/topology/real_bounds.hpp>

#include <cmath>
#include <string>
#include <cstring>
#include <cstdint>

namespace inf::base {

// Defines how parameters are to be serialized.
struct param_io_t { enum value { real, discrete, text, count }; };
typedef param_io_t::value param_io;

// All but real are discrete param types.
struct param_type_t { enum value { real, toggle, knob, list_knob, text, list, knob_list, count }; };
typedef param_type_t::value param_type;

// Block: 
//   automated once at block start.
// Voice: 
//   automation takes effect at voice start for polyphonic synth.
//   Gets screwed up when voices start at multiple positions in a block. 
//   Still ok for chords. Degrades to block for non-voice level stuff.
//   Note: this needs cooperation from the audio processor implementation.
// Continuous: 
//   sample accurate automation.
//   Must be real-valued.
// Fixed:
//   Like continuous, but always transmitted using the default value.
//   Hidden in ui, not automatable.
//   Useful as modulation target.
// Output:
//   visible in ui, not editable, not automatable.
// Note: output params must go in output parts only..
struct param_kind_t { enum value { block, voice, continuous, fixed, output, count }; };
typedef param_kind_t::value param_kind;

// Real valued specific data.
struct real_descriptor
{
  float const default_; // Normalized default.
  std::int32_t const precision; // Value formatting precision.
  real_bounds const dsp; // Dsp range.
  real_bounds const display; // Display range.
  bool parse(char const* buffer, float& val) const;
  float parse_to_normalized(char const* buffer) const;
};

// Discrete valued specific data.
struct discrete_descriptor
{
  std::int32_t const min;
  std::int32_t const max;
  std::int32_t default_;
  std::vector<list_item> const* const items; // Items for a list parameter.
  std::vector<std::string> const* const names; // Names for a knob-list parameter.
  
  // IO: false parses for UI (display name), true parses for persistance (guids).
  std::int32_t parse_ui(param_type type, char const* buffer) const;
  bool parse(param_type type, bool io, char const* buffer, std::int32_t& val) const;

  // Regular discrete.
  discrete_descriptor(std::int32_t min, std::int32_t max, std::int32_t default_):
  min(min), max(max), default_(default_), items(nullptr), names(nullptr)
  { assert(min < max && min <= default_ && default_ <= max); }

  // For actual dropdown lists.
  discrete_descriptor(std::vector<list_item> const* items, std::int32_t default_) :
  min(0), max(static_cast<std::int32_t>(items->size() - 1)), 
  default_(default_), items(items), names(nullptr)
  { assert(items->size() > 0 && default_ >= 0 && default_ < static_cast<std::int32_t>(items->size())); }

  // For knob-lists with dynamically generated data.
  discrete_descriptor(std::vector<std::string> const* names, std::int32_t default_) :
  min(0), max(static_cast<std::int32_t>(names->size() - 1)),
  default_(default_), items(nullptr), names(names)
  { assert(names->size() > 0 && default_ >= 0 && default_ < static_cast<std::int32_t>(names->size())); }

  // For actual dropdown lists.
  discrete_descriptor(std::vector<list_item> const* items, char const* default_) :
  min(0), max(static_cast<std::int32_t>(items->size() - 1)), 
  default_(-1), items(items), names(nullptr)
  { 
    this->default_ = -1;
    assert(items->size() > 0 && default_ != nullptr);
    for(std::int32_t i = 0; i < static_cast<std::int32_t>(items->size()); i++)
      if ((*items)[i].name == std::string(default_))
        this->default_ = i;
    assert(this->default_ != -1);
  }

  // For knob-lists with dynamically generated data.
  discrete_descriptor(std::vector<std::string> const* names, char const* default_) :
  min(0), max(static_cast<std::int32_t>(names->size() - 1)),
  default_(-1), items(nullptr), names(names)
  { 
    this->default_ = -1;
    assert(names->size() > 0 && default_ != nullptr);
    for(std::int32_t i = 0; i < static_cast<std::int32_t>(names->size()); i++)
      if ((*names)[i] == std::string(default_))
        this->default_ = i;
    assert(this->default_ != -1);
  }
};

// Common bounds.

inline discrete_descriptor
discrete_2way_bounds(std::int32_t range, std::int32_t default_) {
  assert(range > 0 && -range <= default_ && default_ <= range);
  return { -range, range, default_ }; }

inline real_descriptor 
percentage_01_bounds(float default_) {
  assert(0.0f <= default_ && default_ <= 1.0f);
  return { default_, 1, real_bounds::linear(0.0f, 1.0f), real_bounds::linear(0.0f, 100.0f) }; }

inline real_descriptor 
decibel_bounds(float linear_max) { 
  assert(linear_max > 0.0f);
  return { 1.0f / linear_max, 1, real_bounds::linear(0.0f, linear_max), real_bounds::decibel(linear_max) }; }

inline real_descriptor 
percentage_m11_bounds(float default_) { 
  assert(-1.0f <= default_ && default_ <= 1.0f);
  return { (default_ + 1.0f) * 0.5f, 1, real_bounds::linear(-1.0f, 1.0f), real_bounds::linear(-100.0f, 100.0f) }; }

inline real_descriptor
quad_bounds(float min, float max, char const* default_, std::int32_t precision) {
  assert(min < max);
  real_descriptor for_parse = { min, precision, real_bounds::quadratic(min, max), real_bounds::quadratic(min, max) };
  return { for_parse.parse_to_normalized(default_), precision, for_parse.dsp, for_parse.display}; }

inline real_descriptor
linear_bounds(float min, float max, float default_, std::int32_t precision) {
  assert(min < max && min <= default_ && default_ <= max);  
  return { (default_ - min) / (max - min), precision, real_bounds::linear(min, max), real_bounds::linear(min, max) }; }

inline real_descriptor
millisecond_bounds(float min, float max, float default_, std::int32_t precision) {
  assert(min < max && min <= default_ && default_ <= max);
  return { (default_ - min) / (max - min), precision, real_bounds::linear(min * 0.001f, max * 0.001f), real_bounds::linear(min, max)}; }

inline real_descriptor 
linear_bounds(float dsp_min, float dsp_max, float default_, float display_min, float display_max, std::int32_t precision) {
  assert(dsp_min < dsp_max && display_min < display_max && dsp_min <= default_ && default_ <= dsp_max);
  return { (default_ - dsp_min) / (dsp_max - dsp_min), precision, real_bounds::linear(dsp_min, dsp_max), real_bounds::linear(display_min, display_max) }; }

// Describes automation input. Separated from param_descriptor so we can 
// re-use the same parameter info for different parts, e.g. voice/global lfo.
struct param_descriptor_data
{
  item_name const static_name; // Static name, e.g. "Frequency", "Resonance".
  param_type const type; // Parameter type.
  char const* const unit; // Parameter unit, e.g. "dB", "Hz".
  param_kind const kind; // Parameter has a fixed value. Don't display in UI and not automatable (useful as modulation target).
  union
  {
    real_descriptor const real; // Real valued specific data.
    discrete_descriptor const discrete; // Discrete valued specific data.
  };

  // IO: false parses/formats for UI (display name), true parses/formats for persistance (guids).
  param_value parse_ui(char const* buffer) const;
  std::string format(bool io, param_value val) const;
  bool parse(bool io, char const* buffer, param_value& val) const;
  std::size_t format(bool io, param_value val, char* buffer, std::size_t size) const;

  param_io io_type() const;
  bool is_continuous() const { return kind == param_kind::continuous || kind == param_kind::fixed; }
  param_value default_value() const { return type == param_type::real ? param_value(real.default_) : param_value(discrete.default_); }

  // Toggle.
  param_descriptor_data(
    item_name const& static_name, param_kind kind, bool default_):
    static_name(static_name), type(param_type::toggle), unit(""), 
    kind(kind), discrete(discrete_descriptor({ 0, 1, default_ })) {}

  // Knob/text.
  param_descriptor_data(
    item_name const& static_name, char const* unit, param_kind kind,
    param_type type, discrete_descriptor const& discrete) :
    static_name(static_name), type(type), unit(unit), kind(kind), discrete(discrete) {}

  // Real.
  param_descriptor_data(
    item_name const& static_name, char const* unit, param_kind kind, real_descriptor const& real) :
    static_name(static_name), type(param_type::real), unit(unit), kind(kind), real(real) {}
};

// Ties a parameter descriptor to an actual parameter id.
// E.g. global lfo 2 and voice lfo 2 may have the same data, but different id's.
struct param_descriptor
{
  std::string const guid; // Unique param id for serialization.
  param_descriptor_data const data; // Parameter metadata.
};

} // namespace inf::base
#endif // INF_BASE_TOPOLOGY_PARAM_DESCRIPTOR_HPP