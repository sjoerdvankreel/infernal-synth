#ifndef INF_SYNTH_AUDIO_BANK_TOPOLOGY_HPP
#define INF_SYNTH_AUDIO_BANK_TOPOLOGY_HPP

#include <inf.synth/synth/config.hpp>
#include <inf.base/topology/param_descriptor.hpp>
#include <inf.base/topology/part_ui_descriptor.hpp>

#include <vector>
#include <cstdint>
#include <numeric>

namespace inf::synth {

// ---- shared ----

std::vector<base::box_descriptor> audio_bank_borders();
  
struct audio_bank_param_type_t { enum value { in, out, amt, bal, count }; };
typedef audio_bank_param_type_t::value audio_bank_param_type;

struct audio_bank_param_t { enum value { on, 
  in1, in2, in3, in4, in5, in6, out1, out2, out3, out4, out5, out6, 
  amt1, amt2, amt3, amt4, amt5, amt6, bal1, bal2, bal3, bal4, bal5, bal6, count }; };
typedef audio_bank_param_t::value audio_bank_param;

inline std::int32_t constexpr audio_bank_param_offset = 1; // for enabled
inline std::int32_t constexpr audio_bank_vgaudio_param_on = 0; // On/off must be 0.
inline std::int32_t constexpr audio_bank_vgaudio_inout_off = 0; // On/off must be 0.
inline std::int32_t constexpr audio_bank_route_count = (audio_bank_param::count - audio_bank_param_offset) / audio_bank_param_type::count;

inline std::int32_t const audio_bank_table_col_count = audio_bank_route_count + 1;
inline float const audio_bank_table_col_widths[audio_bank_table_col_count] = { 0.1f, 0.15f, 0.15f, 0.15f, 0.15f, 0.15f, 0.15f };
inline char const* const audio_bank_table_row_headers[audio_bank_param_type::count] = { "In", "Out", "Amt", "Bal" };
inline base::part_table_descriptor const audio_bank_table = { true, audio_bank_table_col_count, audio_bank_table_col_widths, audio_bank_table_row_headers };

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

} // namespace inf::synth
#endif // INF_SYNTH_AUDIO_BANK_TOPOLOGY_HPP
