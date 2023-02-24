#include <inf.synth/synth/config.hpp>
#include <inf.synth/output/topology.hpp>

using namespace inf::base;

namespace inf::synth {
 
param_descriptor const  
output_params[output_param::count] =         
{ 
  { "{A6024C5F-CF37-48C8-BE5B-713191155AE5}", { { "Clip", "Clip" }, param_kind::output, false, 0, nullptr } },
  { "{6190497F-E08C-49DC-8C74-BAEF6D889BCB}", { { "Voices", "Voice count" }, "", param_kind::output, param_type::text, { 0, synth_polyphony, 0 }, 1, nullptr } },
  { "{76601D0B-6DF3-4EB7-81CD-2FF821C20BCB}", { { "Drain", "Drain" }, param_kind::output, false, 2, nullptr } },
  { "{485325E3-3E15-46C4-84FA-5D743BB75C3A}", { { "CPU", "Total CPU" }, "%", param_kind::output, param_type::text, { 0, 100, 0 }, 3, nullptr } }
}; 
  
} // namespace inf::synth 