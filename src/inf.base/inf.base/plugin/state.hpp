#ifndef INF_BASE_PLUGIN_STATE_HPP
#define INF_BASE_PLUGIN_STATE_HPP

#include <inf.base/plugin/automation_view.hpp>

#include <array>
#include <cstdint>

namespace inf::base {

// Just pick some speed for rendering.
inline float constexpr graph_bpm = 120.0f;

// Hopefully this is enough?
inline std::int32_t constexpr max_input_event_count = 16384;

// Graph output.
struct graph_point 
{ 
  float x;
  float y; 
};

// Graph input.
struct graph_plot_input
{
  float sample_rate;
  param_value const* state;
  std::vector<float> const* dsp_output;
};

// Audio processor result.
struct block_output
{
  float* const* audio;
  param_value* block_automation_raw;
};

struct note_event_data
{
  // Note on/off.
  bool note_on = false;
  // Velocity in case of note on.
  float velocity = 0.0f;
  // Midi note index for both on and off events.
  std::int32_t midi = -1;
  // Index in current buffer.
  std::int32_t sample_index = -1;
};

struct automation_event_data
{
  param_value value;
  std::int32_t param_index;
};

struct input_event_type_t { enum value { note, automation }; };
typedef input_event_type_t::value input_event_type;

// Wrapper has to translate from format specific event stream.
struct input_event
{
  std::int32_t sample_index;
  input_event_type type;
  union { note_event_data note; automation_event_data automation; } data;
};

// Shared input data in case of polyphonic synth.
struct block_input_data
{
  // Block tempo. Vst3 doesnt handle this per-sample.
  float bpm;
  // Audio input for effect.
  float const* const* audio;

  // Sample based stream stuff.
  std::int32_t sample_count;
  std::int64_t stream_position;
  automation_view automation;
};

// Audio processor input.
struct block_input
{
  // Clear voices, delay lines etc.
  bool hard_reset = false;

  // Input events.
  std::array<input_event, max_input_event_count> input_events = {};
  std::int32_t input_event_count = 0;

  // Generic block input.
  block_input_data data = {};
  std::int32_t channel_count = 0;

  // Timing data.
  std::int64_t prev_perf_count = 0;
  std::int32_t prev_sample_count = 0;

  // Automation matrix for the entire processor.
  // Components may create views into this, limiting to specific parameters or sample ranges,
  // using automation_view. The default view provided to process() covers all samples and all parameters.
  // Continuous is per-sample, block covers block, voice, fixed, and output.
  // Continuous has gaps in it since block and continuous automation parameters
  // are declared interleaved but we want both block[p] and continuous[p] to refer to parameter number p.
  param_value* block_automation_raw = {};
  float* const* continuous_automation_raw = {};

  // Per-parameter indication (0/1) if the parameter of this index changed.
  std::int32_t* changed_automation_raw = {};
};

} // namespace inf::base
#endif // INF_BASE_PLUGIN_STATE_HPP