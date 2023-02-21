#ifndef SVN_BASE_PROCESSOR_STATE_HPP
#define SVN_BASE_PROCESSOR_STATE_HPP

#include <inf.base/processor/automation_view.hpp>
#include <cstdint>

namespace inf::base {

// Just pick some speed for rendering.
inline float constexpr graph_bpm = 120.0f;

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

struct note_event
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
  // Midi note events.
  note_event* notes;
  std::int32_t note_count;

  // Generic block input.
  block_input_data data;
  std::int32_t channel_count;

  // Timing data.
  std::int64_t prev_perf_count;
  std::int32_t prev_sample_count;

  // Automation matrix for the entire processor.
  // Components may create views into this, limiting to specific parameters or sample ranges,
  // using automation_view. The default view provided to process() covers all samples and all parameters.
  // Continuous is per-sample, block covers block, voice, fixed, and output.
  // Continuous has gaps in it since block and continuous automation parameters
  // are declared interleaved but we want both block[p] and continuous[p] to refer to parameter number p.
  param_value* block_automation_raw;
  float* const* continuous_automation_raw;

  // Per-parameter indication (0/1) if the parameter of this index changed.
  std::int32_t* changed_automation_raw;
};

} // namespace inf::base
#endif // SVN_BASE_PROCESSOR_STATE_HPP