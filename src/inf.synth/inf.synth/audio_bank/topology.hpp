#ifndef INF_SYNTH_AUDIO_BANK_TOPOLOGY_HPP
#define INF_SYNTH_AUDIO_BANK_TOPOLOGY_HPP

#include <inf.synth/synth/config.hpp>
#include <inf.base/topology/param_descriptor.hpp>

#include <vector>
#include <cstdint>
#include <numeric>

namespace inf::synth {

// ---- shared ----
  
inline std::int32_t constexpr audio_bank_vgaudio_inout_off = 0; // On/off must be 0.
struct audio_bank_param_type_t { enum value { in, out, amt, bal, count }; };
typedef audio_bank_param_type_t::value audio_bank_param_type;

// ---- voice ----

struct vaudio_bank_param_t { enum value { 
  in1, out1, amt1, bal1,
  in2, out2, amt2, bal2,
  in3, out3, amt3, bal3,
  in4, out4, amt4, bal4,
  in5, out5, amt5, bal5,
  in6, out6, amt6, bal6,
  in7, out7, amt7, bal7,
  in8, out8, amt8, bal8,
  in9, out9, amt9, bal9,
  in10, out10, amt10, bal10,
  in11, out11, amt11, bal11,
  in12, out12, amt12, bal12,
  in13, out13, amt13, bal13,
  in14, out14, amt14, bal14,
  in15, out15, amt15, bal15,
  in16, out16, amt16, bal16,
  in17, out17, amt17, bal17,
  in18, out18, amt18, bal18,
  in19, out19, amt19, bal19,
  in20, out20, amt20, bal20,
  in21, out21, amt21, bal21,
  in22, out22, amt22, bal22,
  in23, out23, amt23, bal23,
  in24, out24, amt24, bal24, 
  count }; };
typedef vaudio_bank_param_t::value vaudio_bank_param;

extern base::param_descriptor const vaudio_bank_params[];
struct vaudio_route_output_t { enum value { off, veffect, voice, count }; }; // Off must be 0.
struct vaudio_route_input_t { enum value { off, vosc_all, vosc_any, veffect, count }; }; // Off must be 0.
typedef vaudio_route_input_t::value vaudio_route_input;
typedef vaudio_route_output_t::value vaudio_route_output;

inline std::int32_t constexpr vaudio_route_input_counts[vaudio_route_input::count] = { 1 /* off */, 1 /* vosc all */, vosc_count, veffect_count };
inline std::int32_t constexpr vaudio_route_output_counts[vaudio_route_output::count] = { 1 /* off */, veffect_count, 1 /* voice */ };
inline std::int32_t constexpr vaudio_route_output_total_count = std::accumulate(vaudio_route_output_counts, vaudio_route_output_counts + vaudio_route_output::count, 0);

// ---- global ----

// must be prefix of voice param list
struct gaudio_bank_param_t { enum value { 
  in1, out1, amt1, bal1,
  in2, out2, amt2, bal2,
  in3, out3, amt3, bal3,
  in4, out4, amt4, bal4,
  in5, out5, amt5, bal5,
  in6, out6, amt6, bal6,
  in7, out7, amt7, bal7,
  in8, out8, amt8, bal8,
  in9, out9, amt9, bal9,
  in10, out10, amt10, bal10,
  in11, out11, amt11, bal11,
  in12, out12, amt12, bal12,
  in13, out13, amt13, bal13,
  in14, out14, amt14, bal14,
  in15, out15, amt15, bal15,
  in16, out16, amt16, bal16,
  in17, out17, amt17, bal17,
  in18, out18, amt18, bal18,
  count }; };
typedef gaudio_bank_param_t::value gaudio_bank_param;

extern base::param_descriptor const gaudio_bank_params[];
struct gaudio_route_output_t { enum value { off, geffect, master, count }; }; // Off must be 0.
struct gaudio_route_input_t { enum value { off, voice, external, geffect, count }; }; // Off must be 0.
typedef gaudio_route_input_t::value gaudio_route_input;
typedef gaudio_route_output_t::value gaudio_route_output;

inline std::int32_t constexpr gaudio_route_output_counts[gaudio_route_output::count] = { 1 /* off */, geffect_count, 1 /* master */ };
inline std::int32_t constexpr gaudio_route_input_counts[gaudio_route_input::count] = { 1 /* off */, 1 /* voice */, 1 /* external */, geffect_count };
inline std::int32_t constexpr gaudio_route_output_total_count = std::accumulate(gaudio_route_output_counts, gaudio_route_output_counts + gaudio_route_output::count, 0);

} // namespace inf::synth
#endif // INF_SYNTH_AUDIO_BANK_TOPOLOGY_HPP
