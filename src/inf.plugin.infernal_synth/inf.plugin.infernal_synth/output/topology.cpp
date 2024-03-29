#include <inf.plugin.infernal_synth/output/topology.hpp>

using namespace inf::base;

namespace inf::plugin::infernal_synth {

static std::vector<list_item> const usage_sources = {
  { "{EF08E89F-BA8D-4351-B884-28CED7C666DA}", "V.Out" },
  { "{6DA9D3AA-0AF1-46F3-A99C-46617B3283D9}", "M.Out" },
  { "{A9F10C03-1757-4759-B127-82CD86B61ED7}", "V.Audio" },
  { "{8C9A10BD-B98A-449F-B8D0-5F41E5BC81EE}", "G.Audio" },
  { "{7B4AE600-57E3-43DC-AED9-0FA987B96E80}", "V.CV" },
  { "{5A264DA4-D514-4BBD-9A88-46A2E099F319}", "G.CV" }, 
  { "{BAE7A913-2C2B-4C13-A293-B240F25A0F1B}", "Env" },
  { "{33B6122E-633E-4A3F-9FC9-697268D84BD0}", "Osc" },
  { "{739A0B4B-7111-4F30-8C09-94B474EB1C75}", "V.LFO" },
  { "{B7B01269-A8B2-4F34-A3F8-E66A5DC07B22}", "G.LFO" },
  { "{2579F57B-4E61-4727-8EC5-766F4711FA2D}", "V.FX" },
  { "{26C05CA1-6B5C-4B78-B2CB-662B43EF72AC}", "G.FX" } };
          
param_descriptor const  
output_params[output_param::count] =         
{ 
  { "{A6024C5F-CF37-48C8-BE5B-713191155AE5}", { { "Clip", "Clip" }, param_kind::output, false } },
  { "{6190497F-E08C-49DC-8C74-BAEF6D889BCB}", { { "Voices", "Voice count" }, "", param_kind::output, param_type::text, { 0, synth_polyphony, 0 } } },
  { "{76601D0B-6DF3-4EB7-81CD-2FF821C20BCB}", { { "Drain", "Drain" }, param_kind::output, false } },
  { "{485325E3-3E15-46C4-84FA-5D743BB75C3A}", { { "CPU", "Total CPU" }, "%", param_kind::output, param_type::text, { 0, 100, 0 } } },
  { "{C0AB0784-45BB-4C2E-AF4F-F236EFCF4C6D}", { { "High", "Highest usage module" }, "", param_kind::output, param_type::list, { &usage_sources, usage_source::vamp } } },
  { "{5E2D8A99-F779-4C12-97D3-66D8ADC02507}", { { "High CPU", "Highest usage CPU" }, "%", param_kind::output, param_type::text, { 0, 100, 0 } } }
}; 
  
} // namespace inf::plugin::infernal_synth 