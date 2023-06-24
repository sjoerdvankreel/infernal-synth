#ifndef INF_SYNTH_EFFECT_TOPOLOGY_HPP
#define INF_SYNTH_EFFECT_TOPOLOGY_HPP

#include <inf.base/topology/param_descriptor.hpp>
#include <inf.base/topology/topology_info.hpp>

#include <vector>
#include <cstdint>

namespace inf::synth {

extern base::param_descriptor const veffect_params[];
extern base::param_descriptor const geffect_params[];
extern std::vector<float> const effect_dly_timesig_values;
extern std::vector<float> const effect_dly_multi_hold_timesig_values;

char const* effect_graph_name_selector(
base::topology_info const* topology, base::param_value const* state,
base::part_id id, std::int32_t graph_type);

struct effect_param_t { enum value {
  on, type, filter_type, flt_stvar_type, flt_stvar_freq, flt_stvar_res, flt_stvar_kbd, 
  flt_stvar_shlf_gain, flt_comb_dly_plus, flt_comb_gain_plus, flt_comb_dly_min, flt_comb_gain_min,
  shaper_type, shp_over_order, shp_mix, shp_gain, shp_cheby_terms, shp_cheby_sum_decay, vfx_count,
  delay_type = vfx_count, dly_synced, dly_mix, dly_amt, dly_multi_hold_time, dly_multi_hold_tempo, 
  dly_fdbk_sprd, dly_multi_sprd, dly_fdbk_time_l, dly_fdbk_time_r, dly_fdbk_tempo_l, dly_fdbk_tempo_r, 
  dly_multi_time, dly_multi_tempo, dly_multi_taps, reverb_mix, reverb_size, reverb_spread, reverb_damp, 
  reverb_apf, gfx_count, count = gfx_count }; };
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

} // namespace inf::synth
#endif // INF_SYNTH_EFFECT_TOPOLOGY_HPP