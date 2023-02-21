#include <inf.synth/synth/config.hpp>
#include <inf.synth/output/topology.hpp>

using namespace inf::base;

namespace inf::synth {

param_descriptor const  
output_params[output_param::count] =         
{ 
  { "{C39636DA-EBF5-4AA5-AFBB-BBCD5762D979}", { { "LFO A", "LFO A CPU" }, "%", param_kind::output, param_type::text, { 0, 100, 0 }, 2, nullptr } },
  { "{F6257C76-44B9-437B-8DE9-53FBB7DAC0A8}", { { "LFO B", "LFO B CPU" }, "%", param_kind::output, param_type::text, { 0, 100, 0 }, 7, nullptr } },
  { "{295F92F6-37CD-453E-B8C2-E03B6648A1E4}", { { "Aud A", "Audio bank A CPU" }, "%", param_kind::output, param_type::text, { 0, 100, 0 }, 0, nullptr } },
  { "{05411BF1-2D9A-4A3B-907A-FD26D8B70B1F}", { { "Aud B", "Audio bank B CPU" }, "%", param_kind::output, param_type::text, { 0, 100, 0 }, 5, nullptr } },
  { "{C3FBB5D7-E5EA-4EEB-9795-22298F7380B9}", { { "FX A", "FX A CPU" }, "%", param_kind::output, param_type::text, { 0, 100, 0 }, 3, nullptr } },
  { "{21242E0E-72F8-43CC-B946-019A66C24B7B}", { { "FX B", "FX B CPU" }, "%", param_kind::output, param_type::text, { 0, 100, 0 }, 8, nullptr } },
  { "{BCF00561-4D8E-4AB8-94CE-AF7EF65881FB}", { { "CV A", "CV bank A CPU" }, "%", param_kind::output, param_type::text, { 0, 100, 0 }, 1, nullptr } },
  { "{E9D5B2B9-532F-4136-91AB-310AB374A02C}", { { "CV B", "CV bank B CPU" }, "%", param_kind::output, param_type::text, { 0, 100, 0 }, 6, nullptr } },
  { "{1A5EA805-8678-4B9F-A03F-92E1A9D3C519}", { { "Osc", "Oscillator CPU" }, "%", param_kind::output, param_type::text, { 0, 100, 0 }, 10, nullptr } },
  { "{0DF43E23-9273-46BC-8BBE-52B2C0E612F7}", { { "Env", "Envelope CPU" }, "%", param_kind::output, param_type::text, { 0, 100, 0 }, 12, nullptr } },
  { "{DC142F66-E4B2-4BEA-8B2E-A9501B32B1FB}", { { "Amp", "Voice/Master CPU" }, "%", param_kind::output, param_type::text, { 0, 100, 0 }, 11, nullptr } },
  { "{9301C408-8B93-4165-A79B-F3630FD68F8B}", { { "Aux", "Auxiliary CPU" }, "%", param_kind::output, param_type::text, { 0, 100, 0 }, 13, nullptr } },
  { "{A6024C5F-CF37-48C8-BE5B-713191155AE5}", { { "Clip", "Clip" }, param_kind::output, false, 14, nullptr } },
  { "{6190497F-E08C-49DC-8C74-BAEF6D889BCB}", { { "Voices", "Voice count" }, "", param_kind::output, param_type::text, { 0, synth_polyphony, 0 }, 9, nullptr } },
  { "{485325E3-3E15-46C4-84FA-5D743BB75C3A}", { { "CPU", "Total CPU" }, "%", param_kind::output, param_type::text, { 0, 100, 0 }, 4, nullptr } },
}; 
  
} // namespace inf::synth 