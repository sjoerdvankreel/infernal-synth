#ifndef INF_PLUGIN_INFERNAL_SYNTH_AUDIO_BANK_TOPOLOGY_HPP
#define INF_PLUGIN_INFERNAL_SYNTH_AUDIO_BANK_TOPOLOGY_HPP

#include <inf.plugin.infernal_synth/synth/config.hpp>
#include <inf.base/topology/param_descriptor.hpp>

#include <vector>
#include <cstdint>
#include <numeric>

namespace inf::plugin::infernal_synth {

// ---- shared ----

// On/off must be 0.
inline std::int32_t constexpr audio_bank_vgaudio_inout_off = 0; 
struct audio_bank_param_type_t { enum value { in, out, amt, bal, count }; };
typedef audio_bank_param_type_t::value audio_bank_param_type;

inline std::int32_t
audio_bank_param_index(std::int32_t route, audio_bank_param_type type)
{ return route * audio_bank_param_type::count + type; }

struct audio_bank_param_t { enum value { 
  in1, out1, amt1, bal1, in2, out2, amt2, bal2, in3, out3, amt3, bal3,
  in4, out4, amt4, bal4, in5, out5, amt5, bal5, in6, out6, amt6, bal6,
  in7, out7, amt7, bal7, in8, out8, amt8, bal8, in9, out9, amt9, bal9,
  in10, out10, amt10, bal10, in11, out11, amt11, bal11, in12, out12, amt12, bal12,
  in13, out13, amt13, bal13, in14, out14, amt14, bal14, in15, out15, amt15, bal15, count }; };
typedef audio_bank_param_t::value audio_bank_param;

inline std::int32_t constexpr audio_bank_route_count = 
audio_bank_param::count / static_cast<std::int32_t>(audio_bank_param_type::count);

// ---- voice ----

extern base::param_descriptor const vaudio_bank_params[];
struct vaudio_route_output_t { enum value { off, veffect, voice, count }; }; // Off must be 0.
struct vaudio_route_input_t { enum value { off, vosc_all, vosc_any, veffect, count }; }; // Off must be 0.
typedef vaudio_route_input_t::value vaudio_route_input;
typedef vaudio_route_output_t::value vaudio_route_output;

inline std::int32_t constexpr vaudio_route_input_counts[vaudio_route_input::count] = { 1 /* off */, 1 /* vosc all */, vosc_count, veffect_count };
inline std::int32_t constexpr vaudio_route_output_counts[vaudio_route_output::count] = { 1 /* off */, veffect_count, 1 /* voice */ };
inline std::int32_t constexpr vaudio_route_output_total_count = std::accumulate(vaudio_route_output_counts, vaudio_route_output_counts + vaudio_route_output::count, 0);

// ---- global ----

extern base::param_descriptor const gaudio_bank_params[];
struct gaudio_route_output_t { enum value { off, geffect, master, count }; }; // Off must be 0.
struct gaudio_route_input_t { enum value { off, voice, external, geffect, count }; }; // Off must be 0.
typedef gaudio_route_input_t::value gaudio_route_input;
typedef gaudio_route_output_t::value gaudio_route_output;

inline std::int32_t constexpr gaudio_route_output_counts[gaudio_route_output::count] = { 1 /* off */, geffect_count, 1 /* master */ };
inline std::int32_t constexpr gaudio_route_input_counts[gaudio_route_input::count] = { 1 /* off */, 1 /* voice */, 1 /* external */, geffect_count };
inline std::int32_t constexpr gaudio_route_output_total_count = std::accumulate(gaudio_route_output_counts, gaudio_route_output_counts + gaudio_route_output::count, 0);

} // namespace inf::plugin::infernal_synth
#endif // INF_PLUGIN_INFERNAL_SYNTH_AUDIO_BANK_TOPOLOGY_HPP
