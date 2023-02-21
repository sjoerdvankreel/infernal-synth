#ifndef SVN_SYNTH_EFFECT_TOPOLOGY_HPP
#define SVN_SYNTH_EFFECT_TOPOLOGY_HPP

#include <svn.base/topology/param_descriptor.hpp>
#include <svn.base/topology/part_ui_descriptor.hpp>

#include <vector>
#include <cstdint>

namespace svn::synth {

extern base::param_descriptor const veffect_params[];
extern base::param_descriptor const geffect_params[];
extern std::vector<float> const effect_dly_timesig_values;

struct effect_param_t { enum value {
  on, type, filter_type, flt_stvar_type, flt_stvar_freq, flt_stvar_res, flt_stvar_kbd, 
  flt_stvar_shlf_gain, flt_comb_dly_plus, flt_comb_gain_plus, flt_comb_dly_min, flt_comb_gain_min,
  shaper_type, shp_over_order, shp_mix, shp_gain, shp_cheby_terms, shp_cheby_sum_decay, vfx_count,
  delay_type = vfx_count, dly_synced, dly_mix, dly_fdbk_time_l, dly_fdbk_time_r, dly_fdbk_tempo_l, 
  dly_fdbk_tempo_r, dly_fdbk_amt_l, dly_fdbk_amt_r, dly_multi_time, dly_multi_tempo, dly_multi_taps, 
  dly_multi_sprd, dly_multi_amt, reverb_mix, reverb_size, reverb_spread, reverb_damp, reverb_apf, 
  gfx_count, count = gfx_count }; };
typedef effect_param_t::value effect_param;

struct effect_graph_t { enum value { graph1, graph2, count }; };
struct effect_delay_type_t { enum value { feedback, multi, count }; };
struct effect_filter_type_t { enum value { state_var, comb, count }; };
struct effect_type_t { enum value { filter, shaper, delay, reverb, count }; };
struct effect_shaper_type_t { enum value { clip, tanh, fold, sine, cheby_one, cheby_sum, count }; };
struct effect_shp_over_order_t { enum value { over_none, over_2, over_4, over_8, over_16, count }; };
struct effect_flt_stvar_type_t { enum value { lpf, hpf, bpf, bsf, apf, peq, bll, lsh, hsh, count }; };
typedef effect_type_t::value effect_type;
typedef effect_graph_t::value effect_graph;
typedef effect_delay_type_t::value effect_delay_type;
typedef effect_filter_type_t::value effect_filter_type;
typedef effect_shaper_type_t::value effect_shaper_type;
typedef effect_shp_over_order_t::value effect_shp_over_order;
typedef effect_flt_stvar_type_t::value effect_flt_stvar_type;

inline std::vector<base::graph_descriptor> const& effect_graphs = {
{ effect_graph::graph1, "Filter IR/Shaper ramp/Delay left/Reverb left", { 0, 2, 4, 1 } },
{ effect_graph::graph2, "Filter FR/Shaper spectrum/Delay right/Reverb right", { 0, 3, 4, 1 } } };
inline std::vector<base::box_descriptor> const effect_borders = { { 0, 0, 1, 2 }, { 1, 0, 3, 2 } };

} // namespace svn::synth
#endif // SVN_SYNTH_EFFECT_TOPOLOGY_HPP