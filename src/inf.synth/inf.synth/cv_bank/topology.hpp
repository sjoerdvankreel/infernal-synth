#ifndef INF_SYNTH_CV_BANK_TOPOLOGY_HPP
#define INF_SYNTH_CV_BANK_TOPOLOGY_HPP

#include <inf.synth/synth/config.hpp>
#include <inf.synth/amp/topology.hpp>
#include <inf.synth/synth/topology.hpp>
#include <inf.synth/effect/topology.hpp>
#include <inf.synth/audio_bank/topology.hpp>
#include <inf.synth/oscillator/topology.hpp>

#include <inf.base/shared/support.hpp>
#include <inf.base/topology/param_descriptor.hpp>

#include <vector>
#include <cstdint>
#include <numeric>

namespace inf::synth {

// ---- shared ----

extern base::param_descriptor const vcv_bank_params[];
extern base::param_descriptor const vcv_plot_params[];
extern base::param_descriptor const gcv_bank_params[];
extern base::param_descriptor const gcv_plot_params[];

// input, output, modifiers
inline std::int32_t constexpr cv_bank_vgcv_inout_off = 0; // On/off must be 0.
struct cv_route_input_op_t { enum value { add, sub, mul, count }; };
struct cv_bank_param_type_t { enum value { in, out, op, amt, off, scale, count }; };
typedef cv_route_input_op_t::value cv_route_input_op;
typedef cv_bank_param_type_t::value cv_bank_param_type;

inline std::int32_t
cv_bank_param_index(std::int32_t route, cv_bank_param_type type)
{ return route * cv_bank_param_type::count + type; }

struct cv_bank_param_t { enum value { 
  in1, out1, op1, amt1, off1, scale1,
  in2, out2, op2, amt2, off2, scale2,
  in3, out3, op3, amt3, off3, scale3,
  in4, out4, op4, amt4, off4, scale4,
  in5, out5, op5, amt5, off5, scale5,
  in6, out6, op6, amt6, off6, scale6,
  in7, out7, op7, amt7, off7, scale7,
  in8, out8, op8, amt8, off8, scale8,
  in9, out9, op9, amt9, off9, scale9,
  in10, out10, op10, amt10, off10, scale10,
  in11, out11, op11, amt11, off11, scale11,
  in12, out12, op12, amt12, off12, scale12,
  in13, out13, op13, amt13, off13, scale13,
  in14, out14, op14, amt14, off14, scale14,
  in15, out15, op15, amt15, off15, scale15, count }; };
typedef cv_bank_param_t::value cv_bank_param;

inline std::int32_t constexpr cv_bank_route_count =
cv_bank_param::count / static_cast<std::int32_t>(cv_bank_param_type::count);

// plot
struct cv_plot_param_t { enum value { target, length, count }; };
typedef cv_plot_param_t::value cv_plot_param;

inline char const* cv_plot_graph_name_selector(
base::topology_info const*, base::param_value const*, base::part_id, std::int32_t)
{ return "CV"; }

// vamp/gamp output routing and mapping
struct vgcv_route_amp_target_t { enum value { gain, bal, count }; };
typedef vgcv_route_amp_target_t::value vgcv_route_amp_target;
inline std::int32_t constexpr cv_route_amp_mapping[vgcv_route_amp_target::count] = 
{ amp_param::gain, amp_param::bal };

// effect output routing and mapping. vfx_count: stuff after this is global only.
struct vgcv_route_effect_target_t { enum value { 
  flt_stvar_freq, flt_stvar_res, flt_stvar_kbd, flt_stvar_shlf_gain,
  flt_comb_dly_plus, flt_comb_gain_plus, flt_comb_dly_min, flt_comb_gain_min,
  shp_mix, shp_gain, shp_cheby_sum_decay, vfx_count, dly_mix = vfx_count, dly_amt, dly_fdbk_sprd, dly_multi_sprd,
  reverb_mix, reverb_size, reverb_spread, reverb_damp, reverb_apf, count, gfx_count = count }; };
typedef vgcv_route_effect_target_t::value vgcv_route_effect_target;
inline std::int32_t constexpr vgcv_route_effect_mapping[vgcv_route_effect_target::count] = {
  effect_param::flt_stvar_freq, effect_param::flt_stvar_res, 
  effect_param::flt_stvar_kbd, effect_param::flt_stvar_shlf_gain,
  effect_param::flt_comb_dly_plus, effect_param::flt_comb_gain_plus, 
  effect_param::flt_comb_dly_min, effect_param::flt_comb_gain_min,
  effect_param::shp_mix, effect_param::shp_gain, effect_param::shp_cheby_sum_decay, 
  effect_param::dly_mix, effect_param::dly_amt, effect_param::dly_fdbk_sprd, effect_param::dly_multi_sprd,
  effect_param::reverb_mix, effect_param::reverb_size, effect_param::reverb_spread, 
  effect_param::reverb_damp, effect_param::reverb_apf };

// audio bank output routing and mapping
struct vgcv_route_audio_bank_target_t { enum value { 
  amt1, bal1, amt2, bal2, amt3, bal3, amt4, bal4, amt5, bal5, 
  amt6, bal6, amt7, bal7, amt8, bal8, amt9, bal9, amt10, bal10, 
  amt11, bal11, amt12, bal12, amt13, bal13, amt14, bal14, amt15, bal15, count }; };
typedef vgcv_route_audio_bank_target_t::value vgcv_route_audio_bank_target;
inline std::int32_t constexpr vgcv_route_audio_bank_mapping[vgcv_route_audio_bank_target::count] = {
  audio_bank_param::amt1, audio_bank_param::bal1, audio_bank_param::amt2, audio_bank_param::bal2,
  audio_bank_param::amt3, audio_bank_param::bal3, audio_bank_param::amt4, audio_bank_param::bal4,
  audio_bank_param::amt5, audio_bank_param::bal5, audio_bank_param::amt6, audio_bank_param::bal6,
  audio_bank_param::amt7, audio_bank_param::bal7, audio_bank_param::amt8, audio_bank_param::bal8,
  audio_bank_param::amt9, audio_bank_param::bal9, audio_bank_param::amt10, audio_bank_param::bal10,
  audio_bank_param::amt11, audio_bank_param::bal11, audio_bank_param::amt12, audio_bank_param::bal12,
  audio_bank_param::amt13, audio_bank_param::bal13, audio_bank_param::amt14, audio_bank_param::bal14,
  audio_bank_param::amt15, audio_bank_param::bal15 };

// ---- voice ----

// input routing and mapping, off must be 0
struct vcv_route_input_t { enum value { 
  off, velo, key, key_inv, midi_pb, venv, gcv_uni, gcv_uni_hold, 
  gcv_bi, gcv_bi_hold, vlfo, glfo, glfo_hold, count }; };
typedef vcv_route_input_t::value vcv_route_input;
inline std::int32_t constexpr vcv_route_input_counts[vcv_route_input::count] = {
  1 /* off */, 1 /* velocity */, 1 /* key */, 1 /* inv key */, 1 /* midi pb */, venv_count,
  master_gcv_count /* uni */, master_gcv_count /* uni hold */, 
  master_gcv_count /* bi */, master_gcv_count /* bi hold */,
  vlfo_count, glfo_count, glfo_count /* hold */};

// output routing and mapping, off must be 0
struct vcv_route_output_t { enum value { off, vosc, veffect, vaudio_bank, vamp, count }; };  
typedef vcv_route_output_t::value vcv_route_output;
inline std::int32_t constexpr vcv_route_output_mapping[vcv_route_output_t::count] = {
  -1, part_type::vosc, part_type::veffect, part_type::vaudio_bank, part_type::vamp };
inline std::int32_t constexpr vcv_route_output_counts[vcv_route_output::count] = {
  1 /* off */, vosc_count, veffect_count, 1 /* audio bank */, 1 /* voice */};
inline std::int32_t constexpr vcv_route_output_total_count = std::accumulate(
  vcv_route_output_counts, vcv_route_output_counts + vcv_route_output::count, 0);

// oscillator output routing
struct vcv_route_vosc_target_t { enum value {
  pm, fm, cent, uni_detune, uni_spread, ram_bal, ram_mix,
  basic_pw, mix_sine, mix_saw, mix_triangle, mix_pulse, mix_pw, dsf_dist, dsf_decay, 
  kps_filter, kps_feedback, kps_stretch, noise_color, noise_x, noise_y, count }; };
typedef vcv_route_vosc_target_t::value vcv_route_vosc_target;

// oscillator output mapping
inline std::int32_t constexpr vcv_route_vosc_mapping[vcv_route_vosc_target::count] = {
  osc_param::pm, osc_param::fm, osc_param::cent, osc_param::uni_dtn, osc_param::uni_sprd,
  osc_param::am_ring, osc_param::am_mix, osc_param::basic_pw, osc_param::mix_sine, 
  osc_param::mix_saw, osc_param::mix_triangle, osc_param::mix_pulse, osc_param::mix_pw, 
  osc_param::dsf_dist, osc_param::dsf_decay, osc_param::kps_filter, osc_param::kps_feedback, 
  osc_param::kps_stretch, osc_param::noise_color, osc_param::noise_x, osc_param::noise_y };

// output target mapping
inline std::int32_t constexpr vcv_route_output_target_counts[vcv_route_output::count] = {
  0 /* off */, vcv_route_vosc_target::count, vgcv_route_effect_target::vfx_count,
  vgcv_route_audio_bank_target::count, vgcv_route_amp_target::count };
inline std::int32_t const* const vcv_route_output_target_mapping[vcv_route_output::count] = {
  nullptr, vcv_route_vosc_mapping, vgcv_route_effect_mapping, 
  vgcv_route_audio_bank_mapping, cv_route_amp_mapping };

// ---- global ----

// input routing and mapping, off must be 0
struct gcv_route_input_t { enum value { off, midi_pb, gcv_uni, gcv_bi, glfo, count }; };
typedef gcv_route_input_t::value gcv_route_input;
inline std::int32_t constexpr gcv_route_input_counts[gcv_route_input::count] = {
  1 /* off */, 1 /* midi_pb */, master_gcv_count /* cvu */, master_gcv_count /* cvb */, glfo_count};

// output routing, off must be 0
struct gcv_route_output_t { enum value { off, geffect, gaudio_bank, gamp, count }; };
typedef gcv_route_output_t::value gcv_route_output;
inline std::int32_t constexpr gcv_route_output_mapping[gcv_route_output::count] = {
  -1, part_type::geffect, part_type::gaudio_bank, part_type::gamp };
inline std::int32_t constexpr gcv_route_output_counts[gcv_route_output::count] = {
  1 /* off */, geffect_count, 1 /* audio bank */, 1 /* master */};
inline std::int32_t constexpr gcv_route_output_total_count = std::accumulate(
  gcv_route_output_counts, gcv_route_output_counts + gcv_route_output::count, 0);

// output target mapping
inline std::int32_t const* const gcv_route_output_target_mapping[gcv_route_output::count] = {
  nullptr, vgcv_route_effect_mapping, vgcv_route_audio_bank_mapping, cv_route_amp_mapping };
inline std::int32_t constexpr gcv_route_output_target_counts[gcv_route_output::count] = {
  0 /* off */, vgcv_route_effect_target::gfx_count, vgcv_route_audio_bank_target::count, vgcv_route_amp_target::count };

} // namespace inf::synth
#endif // INF_SYNTH_CV_BANK_TOPOLOGY_HPP
