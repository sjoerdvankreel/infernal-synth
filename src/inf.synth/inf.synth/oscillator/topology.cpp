#include <inf.synth/synth/config.hpp>
#include <inf.synth/oscillator/config.hpp>
#include <inf.synth/oscillator/topology.hpp>

#include <inf.base/shared/support.hpp>
#include <inf.base/topology/part_descriptor.hpp>

using namespace inf::base;

namespace inf::synth {

static std::vector<list_item> const osc_types = { 
  { "{EB845D72-0468-4B7D-A0FA-ED8F2047FFA2}", "Basic" },  
  { "{D1298496-4238-4594-8EFC-28C4E9412267}", "Mix" },
  { "{F5D0D76E-1248-442B-9810-24BA98EF19EC}", "DSF" },
  { "{86E5E070-D167-4083-8C66-8BA89966DB14}", "K+S" },
  { "{919C4FE7-3067-4AC8-B050-818529188542}", "Noise" } }; 
static std::vector<list_item> const osc_basic_types = { 
  { "{9D115AC9-2960-4357-BAB1-2C4075B891AB}", "Sine" },  
  { "{F94E9891-2B5E-45EC-A122-4A368DC251DC}", "Saw" },
  { "{3B1D347E-C3ED-4D4A-BBF9-4441B35D353A}", "Pulse" },
  { "{172E9222-7DAD-4FC7-8748-8A1CBEA3317D}", "Tri" } }; 

static std::int32_t
osc_max_ram_index(std::int32_t part_index)
{ return part_index; }
static std::int32_t
osc_max_sync_index(std::int32_t part_index)
{ return part_index; }
static std::vector<std::string> const osc_ram_sources = 
generate_names(vosc_count, [](std::int32_t n) 
{ return std::string("Osc ") + std::to_string(n + 1); });
static std::vector<std::string> const osc_sync_sources =
generate_names(vosc_count + 1, [](std::int32_t n) 
{ return n == 0 ? std::string("Off") : std::string("Osc ") + std::to_string(n); }); 
     
param_descriptor const     
osc_params[osc_param::count] =               
{     
  { "{CA9274AE-9F57-4373-8C59-3786ACC1C1CD}", { { "On", "On" }, param_kind::voice, false } },    
  { "{84A7AEC8-25E9-4242-B32E-2E9E780F0E31}", { { "Type", "Type" }, "", param_kind::voice, param_type::list, { &osc_types, osc_type::basic } } },
  { "{10935D87-24A7-4994-BB9A-D46636933F6F}", { { "Sync", "Sync source" }, "", param_kind::voice, param_type::knob_list, { &osc_sync_sources, osc_sync_off, osc_max_sync_index } } },
  { "{60D81FF7-F9AB-4E19-B7BC-8D7D08CFEAD3}", { { "Kbd", "Keytrack" }, param_kind::voice, true } },
  { "{5E3DB4DC-B459-43C4-9BBD-0FF8F2232AFA}", { { "Oct", "Octave" }, "", param_kind::voice, param_type::list_knob, { 0, 9, 4 } } },
  { "{501C5217-5A5B-48D8-AEFE-CFE67417E8AD}", { { "Note", "Note" }, "", param_kind::voice, param_type::knob_list, { &note_names, "C" } } },
  { "{FD0A2D20-FCCD-4939-B1CC-BD2EBD998A25}", { { "Cent", "Cent" }, "", param_kind::continuous, linear_bounds(-0.5f, 0.5f, 0.0f, -50.0f, 50.0f, 1) } },
  { "{10D13CC1-EE5C-488A-8F48-F8BA70855B5A}", { { "PM", "Phasemod" }, "", param_kind::fixed, linear_bounds(-1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 2) } },
  { "{3D379D24-CE2D-4AE2-92EF-3951C7CA608C}", { { "FM", "Freqmod" }, "", param_kind::continuous, percentage_m11_bounds(0.0f) } },
  { "{AFA9850B-ACB0-4666-95C0-42D32D197EB2}", { { "Src", "R/AM source" }, "", param_kind::voice, param_type::knob_list, { &osc_ram_sources, 0, osc_max_ram_index } } },
  { "{4ED08060-260A-42EC-99E8-A2BEA628B652}", { { "Bal", "R/AM balance" }, "%", param_kind::continuous, percentage_m11_bounds(1.0f) } },
  { "{63C3C4F7-526A-4A1C-BD12-99548A765110}", { { "Mix", "R/AM mix" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{F5B30B84-5C3E-471F-9B27-D1FB6E06D0AF}", { { "Voices", "Uni voices" }, "", param_kind::voice, param_type::list_knob, { 1, osc_max_voices, 1 } } },
  { "{B96860E3-E2A9-4B6D-8BF9-9313C7705082}", { { "Offset", "Uni offset" }, "%", param_kind::voice, percentage_01_bounds(0.0f) } },
  { "{70C38B3D-E81C-42D9-A59A-9619DB318DFD}", { { "Dtn", "Uni detune" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } }, 
  { "{412B4D8D-4272-40ED-949F-479FB8407BF7}", { { "Sprd", "Uni spread" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{EAFF1105-BA8D-4C55-8BEC-6B73AECF95E2}", { { "Basic", "Basic type" }, "", param_kind::voice, param_type::list, { &osc_basic_types, osc_basic_type::sine } } },
  { "{35DAF80A-6EE0-4A3C-9E81-B225A466F4B2}", { { "PW", "Basic PW" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{2B45F0B5-7FED-4F8F-A550-ACC46EAF33DA}", { { "Sine", "Mix sine" }, "%", param_kind::continuous, percentage_m11_bounds(1.0f) } },  
  { "{2F34BB68-C67F-400F-9CC9-3D9BC90C7602}", { { "Saw", "Mix saw" }, "%", param_kind::continuous, percentage_m11_bounds(0.0f) } },
  { "{A064600E-6D6B-4ED8-BF45-645C00FEE914}", { { "Tri", "Mix triangle" }, "%", param_kind::continuous, percentage_m11_bounds(0.0f) } },   
  { "{D3A840F8-713E-4CA8-93C4-AF4DBA04C0CD}", { { "Pulse", "Mix pulse" }, "%", param_kind::continuous, percentage_m11_bounds(0.0f) } }, 
  { "{EC827166-06A1-4143-ACCB-F20783B009F4}", { { "PW", "Mix PW" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{8B45133B-38AA-4E23-9CB9-B05A4954A947}", { { "Partials", "DSF partials" }, "", param_kind::voice, param_type::knob, { 1, 9999, 2 } } },
  { "{083A6619-0309-48CA-8E9E-9A309EA61044}", { { "Distance", "DSF distance" }, "", param_kind::continuous, linear_bounds(0.05f, 20.0f, 1.0f, 2) } },
  { "{FFD6C9F3-B7D4-4819-A63A-40BC907F91AF}", { { "Decay", "DSF decay" }, "%", param_kind::continuous, percentage_01_bounds(0.5f) } }, 
  { "{3431E946-57D6-444B-B478-13230E7CD34E}", { { "Filter", "K+S filter" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },   
  { "{F0BD5863-3F06-492F-8540-F8F9C5B7060E}", { { "Feedback", "K+S feedback" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } }, 
  { "{F1E0D711-74A3-4238-A52B-59432CBE92D9}", { { "Stretch", "K+S stretch" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },  
  { "{30F3EFB0-F6E2-40E0-AE7B-D72A31E15B5C}", { { "Seed", "Noise seed" }, "", param_kind::voice, param_type::knob, { 1, 256, 1 } } },  
  { "{F0D8FF2E-31DC-4221-AFF2-CC79EAF00421}", { { "Color", "Noise color" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{1326E5CD-5544-4962-AE37-01307C306F18}", { { "X", "Noise X" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{4F1F7660-BE8B-421D-9646-DD9D6D827C38}", { { "Y", "Noise Y" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } } 
};      
  
} // namespace inf::synth  