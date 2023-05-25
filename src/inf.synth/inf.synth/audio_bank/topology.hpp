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
  in1, in2, in3, in4, in5, in6, in7, in8, in9, in10, in11, in12, 
  in13, in14, in15, in16, in17, in18, in19, in20, in21, in22, in23, in24,
  out1, out2, out3, out4, out5, out6, out7, out8, out9, out10, out11, out12, 
  out13, out14, out15, out16, out17, out18, out19, out20, out21, out22, out23, out24,
  amt1, amt2, amt3, amt4, amt5, amt6, amt7, amt8, amt9, amt10, amt11, amt12,
  amt13, amt14, amt15, amt16, amt17, amt18, amt19, amt20, amt21, amt22, amt23, amt24,
  bal1, bal2, bal3, bal4, bal5, bal6, bal7, bal8, bal9, bal10, bal11, bal12,
  bal13, bal14, bal15, bal16, bal17, bal18, bal19, bal20, bal21, bal22, bal23, bal24, count }; };
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

struct gaudio_bank_param_t { enum value { 
  in1, in2, in3, in4, in5, in6, in7, in8, in9, in10, in11, in12, in13, in14, in15, in16, in17, in18,
  out1, out2, out3, out4, out5, out6, out7, out8, out9, out10, out11, out12, out13, out14, out15, out16, out17, out18,
  amt1, amt2, amt3, amt4, amt5, amt6, amt7, amt8, amt9, amt10, amt11, amt12, amt13, amt14, amt15, amt16, amt17, amt18,
  bal1, bal2, bal3, bal4, bal5, bal6, bal7, bal8, bal9, bal10, bal11, bal12, bal13, bal14, bal15, bal16, bal17, bal18, count }; };
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
