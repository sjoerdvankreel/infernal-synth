#include <inf.synth/audio_bank/topology.hpp>
#include <inf.base/shared/support.hpp>
#include <inf.base/topology/part_descriptor.hpp>
#include <inf.base/topology/param_ui_descriptor.hpp>

#include <ranges>

using namespace inf::base;

namespace inf::synth {

std::vector<box_descriptor> gaudio_bank_borders()
{
  return {};
} 

std::vector<box_descriptor> vaudio_bank_borders()
{
  return {};
}

static list_item const vaudio_route_inputs[vaudio_route_input::count] = {
  { "{49276AD4-960F-49D4-9B74-A8E4D5A86B6B}", "Off" },
  { "{A3D43958-5219-4D44-A51A-54853EF6F457}", "Osc" }, // any 
  { "{1E01C31B-3160-49B6-A88F-7A532AE5B407}", "FX A" } };
static list_item const vaudio_route_outputs[vaudio_route_output::count] = {
  { "{F91EF1BE-F439-44AA-9B6C-EAB5A186ECCB}", "Off" },
  { "{514ED3EB-9195-412D-8E50-9EB89D80CF1D}", "FX A" },
  { "{E05A9233-EF1F-4267-85DF-04F33DFC611B}", "Voice" } };
static list_item const gaudio_route_inputs[gaudio_route_input::count] = {
  { "{18620BA6-61E3-4AFE-88D0-37DBE443B67D}", "Off" }, 
  { "{973FADF8-945E-4196-9E9A-3B422B09ED21}", "Voice" },
  { "{C4FFD4E3-9BBC-4D9F-8083-FAD0268BD4C3}", "Ext" },
  { "{213616DC-90FD-4128-A517-E63C97457D15}", "FX B" } };
static list_item const gaudio_route_outputs[gaudio_route_output::count] = {
  { "{AE02B0B8-10DD-44F9-912C-C94E46D7615D}", "Off" },
  { "{5F5E1D56-311E-4558-9C90-B368E8BAF237}", "FX B" },
  { "{A781CA9A-B8AA-448A-924B-D16C31108CB5}", "Master" } };
   
static bool const vaudio_route_output_spaces[vaudio_route_output::count] = { true, false, true };
static bool const gaudio_route_output_spaces[gaudio_route_output::count] = { true, false, true };
static bool const vaudio_route_input_spaces[vaudio_route_input::count] = { true, true, false };
static bool const gaudio_route_input_spaces[gaudio_route_input::count] = { true, true, true, false };

static std::vector<list_item> const vaudio_route_input_items =  
multi_list_items(vaudio_route_inputs, nullptr, vaudio_route_input_spaces, nullptr, vaudio_route_input_counts, vaudio_route_input::count, false);
static std::vector<list_item> const gaudio_route_input_items =
multi_list_items(gaudio_route_inputs, nullptr, gaudio_route_input_spaces, nullptr, gaudio_route_input_counts, gaudio_route_input::count, false);
static std::vector<list_item> const vaudio_route_output_items =
multi_list_items(vaudio_route_outputs, nullptr, vaudio_route_output_spaces, nullptr, vaudio_route_output_counts, vaudio_route_output::count, false);
static std::vector<list_item> const gaudio_route_output_items = 
multi_list_items(gaudio_route_outputs, nullptr, gaudio_route_output_spaces, nullptr, gaudio_route_output_counts, gaudio_route_output::count, false);

param_descriptor const 
vaudio_bank_params[vaudio_bank_param::count] = 
{  
  { "{14096099-485D-4EB9-B055-E393DE2E993C}", { { "On", "Enabled" }, param_kind::voice, false, -1, nullptr } },
  { "{2E9F0478-B911-43DF-BB51-0C5836E4853F}", { { "In", "Input 1" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 }, 0, nullptr}},
  { "{A3A59082-CF73-4C28-A3FC-037729C9CB42}", { { "In", "Input 2" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 }, 4, nullptr } },
  { "{A8E6882A-8945-4F59-92B9-78004EAF5818}", { { "In", "Input 3" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 }, 8, nullptr } },
  { "{7DFD9AF0-7419-4B06-B875-F18D9C344D42}", { { "In", "Input 4" }, "", param_kind::voice, param_type::list, { &vaudio_route_input_items, 0 }, 12, nullptr } },
  { "{295DC5F0-FB32-4D43-8799-D79F23FD3AA9}", { { "Out", "Output 1" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 }, 1, nullptr } },
  { "{843EB41C-199F-4FAC-ACC4-841B3663248D}", { { "Out", "Output 2" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 }, 5, nullptr } },
  { "{D2E34E63-DE9E-4620-924D-1897614BF983}", { { "Out", "Output 3" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 }, 9, nullptr } },
  { "{C2C5B9BC-81B6-4D3E-947E-E0B900447BDF}", { { "Out", "Output 4" }, "", param_kind::voice, param_type::list, { &vaudio_route_output_items, 0 }, 13, nullptr } },
  { "{A3B15FE9-56DB-493B-A4E1-31A004F3C937}", { { "Amt", "Amount 1" }, "dB", param_kind::continuous, decibel_bounds(2.0f), 2, nullptr} },
  { "{810B55DF-6230-45C7-B478-7A3569DC9127}", { { "Amt", "Amount 2" }, "dB", param_kind::continuous, decibel_bounds(2.0f), 6, nullptr } },
  { "{6721B1EC-9688-48A3-B5B8-0ADD0A9CF16B}", { { "Amt", "Amount 3" }, "dB", param_kind::continuous, decibel_bounds(2.0f), 10, nullptr } },
  { "{51E69AB1-37F7-4361-84B9-2F1727F66C4A}", { { "Amt", "Amount 4" }, "dB", param_kind::continuous, decibel_bounds(2.0f), 14, nullptr } },
  { "{91996C1F-4510-4BC1-97BA-135C9881263F}", { { "Bal", "Stereo balance 1" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1), 3, nullptr} },
  { "{265269F4-F6DF-474E-A696-44EE01681C65}", { { "Bal", "Stereo balance 2" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1), 7, nullptr } },
  { "{64C90CE2-2AC0-4675-8F99-B88E807F712A}", { { "Bal", "Stereo balance 3" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1), 11, nullptr } },
  { "{01B6309E-B045-48A7-9BC1-8E828A528A3F}", { { "Bal", "Stereo balance 4" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1), 15, nullptr } }
};

param_descriptor const  
gaudio_bank_params[gaudio_bank_param::count] = 
{
  { "{85A0A7FB-8319-436E-9979-0A7267F1F636}", { { "On", "Enabled" }, param_kind::block, false, -1, nullptr } },
  { "{CD5CD403-259F-4B25-9C33-E246931E973B}", { { "In", "Input 1" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 }, 0, nullptr}},
  { "{E83D6E12-47C4-4738-8CFA-A18CC8B86C67}", { { "In", "Input 2" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 }, 4, nullptr } },
  { "{EF89DAE6-59F6-4B5E-BA7F-F9F9F4FA64C0}", { { "In", "Input 3" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 }, 8, nullptr } },
  { "{3F1E4A05-2C73-418D-B490-841106011784}", { { "In", "Input 4" }, "", param_kind::block, param_type::list, { &gaudio_route_input_items, 0 }, 12, nullptr } },
  { "{6AD76233-62A6-4F5A-ADCB-797786E00C54}", { { "Out", "Output 1" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 }, 1, nullptr } },
  { "{8BEAB138-D485-480C-B2BE-146354C5A2F9}", { { "Out", "Output 2" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 }, 5, nullptr } },
  { "{33E9FBE4-4654-4B92-AD03-5EF2EC3FEEF2}", { { "Out", "Output 3" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 }, 9, nullptr } },
  { "{9F28A209-5A25-4665-BB73-557BE1F3CC05}", { { "Out", "Output 4" }, "", param_kind::block, param_type::list, { &gaudio_route_output_items, 0 }, 13, nullptr } },
  { "{E444E539-65EF-449F-8407-EB128C6082B8}", { { "Amt", "Amount 1" }, "dB", param_kind::continuous, decibel_bounds(2.0f), 2, nullptr} },
  { "{FBCCD63D-1D9A-444C-A622-D9D3E8A771C7}", { { "Amt", "Amount 2" }, "dB", param_kind::continuous, decibel_bounds(2.0f), 6, nullptr } },
  { "{C147FB0B-25A9-44F9-88E2-77CA415F83BB}", { { "Amt", "Amount 3" }, "dB", param_kind::continuous, decibel_bounds(2.0f), 10, nullptr } },
  { "{E394ACC9-DA07-43DF-9BF7-02A57BF3F758}", { { "Amt", "Amount 4" }, "dB", param_kind::continuous, decibel_bounds(2.0f), 14, nullptr } },
  { "{3E7BA2FC-2984-4B2F-9936-754BEE44CFCE}", { { "Bal", "Stereo balance 1" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1), 3, nullptr} },
  { "{D2672F61-8811-420C-A4CA-1ED78D49AC55}", { { "Bal", "Stereo balance 2" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1), 7, nullptr } },
  { "{10073537-70C6-40FB-8F53-0ABB2C594944}", { { "Bal", "Stereo balance 3" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1), 11, nullptr } },
  { "{EBBBAC9E-3D8F-482A-A41B-32CB47324647}", { { "Bal", "Stereo balance 4" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1), 15, nullptr } },
};

} // namespace inf::synth