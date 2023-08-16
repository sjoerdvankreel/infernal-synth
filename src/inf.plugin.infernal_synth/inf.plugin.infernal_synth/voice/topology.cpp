#include <inf.plugin.infernal_synth/voice/topology.hpp>
#include <inf.base/topology/part_descriptor.hpp>

using namespace inf::base;

namespace inf::plugin::infernal_synth {

static std::vector<list_item> const voice_port_trigs = {
  { "{506D3FCE-5A35-4209-9A0E-0931C6EDDC0B}", "Note" },
  { "{8D81E1D4-6063-470A-A5E3-0E65308EA182}", "Voice" } }; 
static std::vector<list_item> const voice_port_modes = {
  { "{8671F976-CB2C-4BB9-A79E-2BBA7A016967}", "Off" },
  { "{E3F6FCCB-B641-455D-BD0C-F52D7C65FF79}", "On" }, 
  { "{A270822E-E7E2-499F-8B24-62C4FEE3D912}", "Auto" } };
static std::vector<list_item> const voice_modes = {
  { "{CF05B74A-F96C-47DF-B908-39477BB6A14B}", "Poly" },
  { "{5EFD4121-52DC-43F2-A853-CC4DB9F8342F}", "Mono" }, 
  { "{49996567-D045-488B-85D0-97EC55FE8CDB}", "Release" } };

static std::vector<time_signature> const voice_port_timesig = synced_timesig(false, { 4, 1 }, {
  { timesig_type::one_over, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 16, 32, 64 } },
  { timesig_type::lower, { 2, 3, 4, 5, 6, 7, 8, 9, 12, 16 } },
  { timesig_type::upper, { 2, 3, 4, 5, 6, 7, 8 } },
  { timesig_type::over_one, { 1, 2, 3, 4 } } });
std::vector<float> const voice_port_timesig_values = synced_timesig_values(voice_port_timesig);
static std::vector<std::string> const voice_port_timesig_names = synced_timesig_names(voice_port_timesig);

param_descriptor const
voice_params[voice_param::count] =
{
  { "{5E6A8C53-AE49-4FDF-944D-57CF37FC2C0E}", { { "Oct", "Octave" }, "", param_kind::voice, param_type::list_knob, { 0, 9, 4 } } },
  { "{AE673674-1A16-4219-8EE5-048722BF52D1}", { { "Note", "Note" }, "", param_kind::voice, param_type::knob_list, { &note_names, "C" } } },
  { "{F3DFD0D7-652D-4F80-9C97-38037BCF58A7}", { { "Mode", "Mode" }, "", param_kind::block, param_type::list, { &voice_modes, voice_mode::poly } } },
  { "{511EE6C3-8798-4B7A-940D-100B8680517F}", { { "Port", "Portamento mode" }, "", param_kind::block, param_type::list, { &voice_port_modes, voice_port_mode::off } } },
  { "{921B5AE6-37FD-4953-94C3-16062C0D23ED}", { { "Trig", "Portamento trigger" }, "", param_kind::block, param_type::list, { &voice_port_trigs, voice_port_trig::note } } },
  { "{09243EDB-2DBE-450F-800F-C37BF8A3C44B}", { { "Sync", "Portamento tempo sync" }, param_kind::block, false } },
  { "{E27DEC2E-BA49-454C-8C28-F7532F6985DC}", { { "Time", "Portamento time" }, "Sec", param_kind::block, linear_bounds(0.01f, 10.0f, 1.0f, 2) } },
  { "{1DD53257-2104-4C66-BA00-2B73E9F6BA63}", { { "Tempo", "Portamento tempo" }, "", param_kind::block, param_type::knob_list, { &voice_port_timesig_names, "1/16" } } },
};
 
} // namespace inf::plugin::infernal_synth 