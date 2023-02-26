#ifndef INF_SYNTH_CV_BANK_TOPOLOGY_HPP
#define INF_SYNTH_CV_BANK_TOPOLOGY_HPP

#include <inf.synth/synth/config.hpp>
#include <inf.synth/synth/topology.hpp>
#include <inf.synth/effect/topology.hpp>
#include <inf.synth/audio_bank/topology.hpp>
#include <inf.synth/oscillator/topology.hpp>
#include <inf.synth/voice_master/topology.hpp>

#include <inf.base/shared/support.hpp>
#include <inf.base/topology/param_descriptor.hpp>
#include <inf.base/topology/part_ui_descriptor.hpp>

#include <vector>
#include <cstdint>
#include <numeric>

namespace inf::synth {

// ---- shared ----

std::vector<base::box_descriptor> vcv_bank_borders();
std::vector<base::box_descriptor> gcv_bank_borders();
extern base::param_descriptor const vcv_bank_params[];
extern base::param_descriptor const vcv_plot_params[];
extern base::param_descriptor const gcv_bank_params[];
extern base::param_descriptor const gcv_plot_params[];

// for enabled
inline std::int32_t constexpr cv_bank_param_offset = 1;
inline std::int32_t constexpr cv_bank_vgcv_param_on = 0;
inline std::int32_t constexpr cv_bank_vgcv_inout_off = 0;

// plot
struct cv_plot_param_t { enum value { target, length, count }; };
typedef cv_plot_param_t::value cv_plot_param;

// input, output, modifiers
struct cv_route_input_op_t { enum value { add, sub, mul, count }; };
struct cv_route_output_base_t { enum value { raw, mod, param, count }; };
struct cv_bank_param_type_t { enum value { in, out, base, op, off, scale, amt, count }; };
typedef cv_route_input_op_t::value cv_route_input_op;
typedef cv_bank_param_type_t::value cv_bank_param_type;
typedef cv_route_output_base_t::value cv_route_output_base;

// voice/master output routing and mapping
struct vgcv_route_amp_target_t { enum value { gain, bal, count }; };
typedef vgcv_route_amp_target_t::value vgcv_route_amp_target;
inline std::int32_t constexpr cv_route_amp_mapping[vgcv_route_amp_target::count] = { 
  vgamp_param::gain, vgamp_param::bal };

// effect output routing
// vfx_count: stuff after this is global only.
struct vgcv_route_effect_target_t { enum value { 
  flt_stvar_freq, flt_stvar_res, flt_stvar_kbd, flt_stvar_shlf_gain,
  flt_comb_dly_plus, flt_comb_gain_plus, flt_comb_dly_min, flt_comb_gain_min,
  shp_mix, shp_gain, shp_cheby_sum_decay, vfx_count, dly_mix = vfx_count, dly_amt, dly_fdbk_sprd, dly_multi_sprd,
  reverb_mix, reverb_size, reverb_spread, reverb_damp, reverb_apf, count, gfx_count = count }; };
typedef vgcv_route_effect_target_t::value vgcv_route_effect_target;

// effect output mapping
inline std::int32_t constexpr vgcv_route_effect_mapping[vgcv_route_effect_target::count] = {
  effect_param::flt_stvar_freq, effect_param::flt_stvar_res, 
  effect_param::flt_stvar_kbd, effect_param::flt_stvar_shlf_gain,
  effect_param::flt_comb_dly_plus, effect_param::flt_comb_gain_plus, 
  effect_param::flt_comb_dly_min, effect_param::flt_comb_gain_min,
  effect_param::shp_mix, effect_param::shp_gain, effect_param::shp_cheby_sum_decay, 
  effect_param::dly_mix, effect_param::dly_amt, effect_param::dly_fdbk_sprd, effect_param::dly_multi_sprd,
  effect_param::reverb_mix, effect_param::reverb_size, effect_param::reverb_spread, 
  effect_param::reverb_damp, effect_param::reverb_apf };

// ui
inline std::vector<base::box_descriptor> const cv_plot_borders = { { 0, 0, 2, 1 } };
inline std::vector<base::graph_descriptor> const cv_plot_graph_descs = { { -1, "CV", { 0, 1, 2, 3 } } };

// ---- voice ----

// on/off must be 0
struct vcv_bank_param_t { enum value { on, 
  in1, in2, in3, out1, out2, out3, base1, base2, base3,
  op1, op2, op3, off1, off2, off3, scale1, scale2, scale3,
  amt1, amt2, amt3, count }; };
typedef vcv_bank_param_t::value vcv_bank_param;

// route count per bank
inline std::int32_t constexpr vcv_bank_route_count =
(vcv_bank_param::count - cv_bank_param_offset) / cv_bank_param_type::count;

// ui
inline std::int32_t const vcv_bank_table_col_count = cv_bank_param_type::count;
inline float const vcv_bank_table_col_widths[vcv_bank_table_col_count] = { 0.14f, 0.19f, 0.11f, 0.08f, 0.16f, 0.16f, 0.16f };
inline char const* const vcv_bank_table_col_headers[vcv_bank_table_col_count] = { "In", "Out", "Base", "Op", "Off", "Scl", "Amt" };
inline base::part_table_descriptor const vcv_bank_table = { false, vcv_bank_table_col_count, vcv_bank_table_col_widths, vcv_bank_table_col_headers };

// input routing and mapping, off must be 0
struct vcv_route_input_t { enum value { 
  off, velo, key, key_inv, venv, gcv_uni, gcv_uni_hold, 
  gcv_bi, gcv_bi_hold, vlfo, glfo, glfo_hold, count }; };
typedef vcv_route_input_t::value vcv_route_input;
inline std::int32_t constexpr vcv_route_input_counts[vcv_route_input::count] = {
  1 /* off */, 1 /* velocity */, 1 /* key */, 1 /* inv key */, venv_count, 
  master_gcv_count /* uni */, master_gcv_count /* uni hold */, 
  master_cv_count /* bi */, master_gcv_count /* bi hold */, 
  vlfo_count, glfo_count, glfo_count /* hold */};

// output routing and mapping, off must be 0
struct vcv_route_output_t { enum value { off, vosc, veffect, vaudio_bank, voice, count }; };  
typedef vcv_route_output_t::value vcv_route_output;
inline std::int32_t constexpr vcv_route_output_mapping[vcv_route_output_t::count] = {
  -1, part_type::vosc, part_type::veffect, part_type::vaudio_bank, part_type::voice };
inline std::int32_t constexpr vcv_route_output_counts[vcv_route_output::count] = {
  1 /* off */, vosc_count, veffect_count, vaudio_bank_count, 1 /* voice */ };
inline std::int32_t constexpr vcv_route_output_total_count = std::accumulate(
  vcv_route_output_counts, vcv_route_output_counts + vcv_route_output::count, 0);

// audio bank output routing and mapping
struct vcv_route_audio_bank_target_t { enum value { 
  amt1, amt2, amt3, amt4, 
  bal1, bal2, bal3, bal4, count }; };
typedef vcv_route_audio_bank_target_t::value vcv_route_audio_bank_target;
inline std::int32_t constexpr vcv_route_audio_bank_mapping[vcv_route_audio_bank_target::count] = {
vaudio_bank_param::amt1, vaudio_bank_param::amt2, vaudio_bank_param::amt3, vaudio_bank_param::amt4,
vaudio_bank_param::bal1, vaudio_bank_param::bal2, vaudio_bank_param::bal3, vaudio_bank_param::bal4 };

// oscillator output routing
struct vcv_route_vosc_target_t { enum value {
  pm, fm, gain, cent, uni_detune, uni_spread, ram_bal, ram_mix,
  basic_pw, mix_sine, mix_saw, mix_triangle, mix_pulse, mix_pw, dsf_dist, dsf_decay, 
  kps_filter, kps_feedback, kps_stretch, noise_color, noise_x, noise_y, count }; };
typedef vcv_route_vosc_target_t::value vcv_route_vosc_target;

// oscillator output mapping
inline std::int32_t constexpr vcv_route_vosc_mapping[vcv_route_vosc_target::count] = {
  osc_param::pm, osc_param::fm, osc_param::gain, osc_param::cent, osc_param::uni_dtn, osc_param::uni_sprd,
  osc_param::ram_bal, osc_param::ram_mix, osc_param::basic_pw, osc_param::mix_sine, 
  osc_param::mix_saw, osc_param::mix_triangle, osc_param::mix_pulse, osc_param::mix_pw, 
  osc_param::dsf_dist, osc_param::dsf_decay, osc_param::kps_filter, osc_param::kps_feedback, 
  osc_param::kps_stretch, osc_param::noise_color, osc_param::noise_x, osc_param::noise_y };

// output target mapping
inline std::int32_t constexpr vcv_route_output_target_counts[vcv_route_output::count] = {
  0 /* off */, vcv_route_vosc_target::count, vgcv_route_effect_target::vfx_count,
  vcv_route_audio_bank_target::count, vgcv_route_amp_target::count };
inline std::int32_t const* const vcv_route_output_target_mapping[vcv_route_output::count] = {
  nullptr, vcv_route_vosc_mapping, vgcv_route_effect_mapping, 
  vcv_route_audio_bank_mapping, cv_route_amp_mapping };

// ---- global ----

// on/off must be 0
struct gcv_bank_param_t { enum value { on, 
  in1, in2, in3, in4, out1, out2, out3, out4, base1, base2, base3, base4,
  op1, op2, op3, op4, off1, off2, off3, off4, scale1, scale2, scale3, scale4,
  amt1, amt2, amt3, amt4, count }; };
typedef gcv_bank_param_t::value gcv_bank_param;

// route count per bank
inline std::int32_t constexpr gcv_bank_route_count =
(gcv_bank_param::count - cv_bank_param_offset) / cv_bank_param_type::count;

// ui
inline std::int32_t const gcv_bank_table_col_count = gcv_bank_route_count + 1;
inline float const gcv_bank_table_col_widths[gcv_bank_table_col_count] = { 0.1f, 0.225f, 0.225f, 0.225f, 0.225f };
inline char const* const gcv_bank_table_row_headers[cv_bank_param_type::count] = { "In", "Out", "Base", "Op", "Off", "Scl", "Amt" };
inline base::part_table_descriptor const gcv_bank_table = { true, gcv_bank_table_col_count, gcv_bank_table_col_widths, gcv_bank_table_row_headers };

// input routing and mapping, off must be 0
struct gcv_route_input_t { enum value { off, gcv_uni, gcv_bi, glfo, count }; };
typedef gcv_route_input_t::value gcv_route_input;
inline std::int32_t constexpr gcv_route_input_counts[gcv_route_input::count] = {
  1 /* off */, master_gcv_count /* cvu */, master_gcv_count /* cvb */, glfo_count};

// output routing, off must be 0
struct gcv_route_output_t { enum value { off, geffect, gaudio_bank, master, count }; };
typedef gcv_route_output_t::value gcv_route_output;
inline std::int32_t constexpr gcv_route_output_mapping[gcv_route_output::count] = {
  -1, part_type::geffect, part_type::gaudio_bank, part_type::master };
inline std::int32_t constexpr gcv_route_output_counts[gcv_route_output::count] = {
  1 /* off */, geffect_count, gaudio_bank_count, 1 /* master */ };
inline std::int32_t constexpr gcv_route_output_total_count = std::accumulate(
  gcv_route_output_counts, gcv_route_output_counts + gcv_route_output::count, 0);

// audio bank output routing and mapping
struct gcv_route_audio_bank_target_t { enum value { 
  amt1, amt2, amt3, amt4,
  bal1, bal2, bal3, bal4, count }; };
typedef gcv_route_audio_bank_target_t::value gcv_route_audio_bank_target;
inline std::int32_t constexpr gcv_route_audio_bank_mapping[gcv_route_audio_bank_target::count] = {
  gaudio_bank_param::amt1, gaudio_bank_param::amt2, gaudio_bank_param::amt3, gaudio_bank_param::amt4, 
  gaudio_bank_param::bal1, gaudio_bank_param::bal2, gaudio_bank_param::bal3, gaudio_bank_param::bal4 };

// output target mapping
inline std::int32_t const* const gcv_route_output_target_mapping[gcv_route_output::count] = {
  nullptr, vgcv_route_effect_mapping, gcv_route_audio_bank_mapping, cv_route_amp_mapping };
inline std::int32_t constexpr gcv_route_output_target_counts[gcv_route_output::count] = {
  0 /* off */, vgcv_route_effect_target::gfx_count, gcv_route_audio_bank_target::count, vgcv_route_amp_target::count };

} // namespace inf::synth
#endif // INF_SYNTH_CV_BANK_TOPOLOGY_HPP
