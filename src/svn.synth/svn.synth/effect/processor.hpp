#ifndef SVN_SYNTH_DSP_EFFECT_PROCESSOR_HPP
#define SVN_SYNTH_DSP_EFFECT_PROCESSOR_HPP

#include <svn.synth/shared/state.hpp>
#include <svn.synth/effect/state.hpp>
#include <svn.synth/cv_bank/topology.hpp>
#include <svn.synth/shared/audio_part_processor.hpp>

#include <cmath>
#include <array>
#include <cassert>

namespace svn::synth {

struct scratch_space;
class cv_bank_processor;

class effect_processor:
public audio_part_processor
{
  std::int32_t _midi;
  effect_state* _state;

  std::int32_t _on;
  std::int32_t _type;
  std::int32_t _delay_type;
  std::int32_t _filter_type;
  std::int32_t _shaper_type;
  std::int32_t _flt_stvar_type;
  std::int32_t _shp_over_order;
  std::int32_t _shp_cheby_terms;
  std::int32_t _dly_multi_taps;
  std::int32_t _dly_multi_length;

  void update_flt_stvar_kbd_track(std::int32_t midi);
  static std::int32_t modulation_type(std::int32_t part_type);
  void update_block_params(base::automation_view const& automation, std::int32_t midi, float bpm);

  void process_dly(effect_process_input const& input, float* const* out);
  void process_dly_multi(effect_process_input const& input, float* const* out);
  void process_dly_feedback(effect_process_input const& input, float* const* out);

  void process_shp(effect_process_input const& input, float* const* out);
  void process_shp_cheby_one(effect_process_input const& input, float* const* out);
  void process_shp_cheby_sum(effect_process_input const& input, float* const* out);
  template <class shaper_type>
  void process_shp_basic(effect_process_input const& input, float* const* out, shaper_type shaper);

  void process_reverb(effect_process_input const& input, float* const* out, scratch_space& scratch);

  void process_flt_comb(effect_process_input const& input, float* const* out);
  void process_flt(effect_process_input const& input, float* const* out, scratch_space& scratch);
  void process_flt_stvar(effect_process_input const& input, float* const* out, scratch_space& scratch);

public:
  // Voice and global.
  effect_processor() = default;
  effect_processor(
    base::topology_info const* topology, base::part_id id, float sample_rate, float bpm,
    std::int32_t midi, effect_state* state, base::automation_view const& automation);

  std::int32_t graph_min_delay_samples() const;
  audio_part_output process_any(effect_input const& input, float* const* out, cv_bank_processor& cv, scratch_space& scratch);
  audio_part_output process_global(effect_input const& input, float* const* out, cv_bank_processor& cv, scratch_space& scratch);
};

inline std::int32_t
effect_processor::modulation_type(std::int32_t part_type)
{
  if (part_type == part_type::veffect)
    return vcv_route_output::veffect;
  return gcv_route_output::geffect;
}

} // namespace svn::synth
#endif // SVN_SYNTH_DSP_EFFECT_PROCESSOR_HPP