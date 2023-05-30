#include <inf.synth/lfo/config.hpp>
#include <inf.synth/lfo/topology.hpp>

#include <inf.base/shared/support.hpp>
#include <inf.base/topology/part_descriptor.hpp>

using namespace inf::base;

namespace inf::synth {
 
static std::vector<list_item> const lfo_types = {
  { "{BA47C8D9-458A-4FE7-B4EC-E0A5BF6F0073}", "Basic" },
  { "{86377625-0B20-45D0-8DB8-A92B5CD807D8}", "Rand" },
  { "{1B0F2B5B-FDC7-4F63-ADF1-621C18144739}", "Free" } };
static std::vector<list_item> const lfo_rand_types = {
  { "{188B114B-ACB1-495E-9811-E0B77D9EEE27}", "Slope" }, 
  { "{ADCF2659-BF48-4099-A552-084991CCE603}", "Level" },
  { "{9D6ED496-416D-44BE-BDE5-0CB4CEC709D3}", "Both" } };
static std::vector<list_item> const lfo_basic_types = { 
  { "{1C9D9313-8E02-415E-957C-0073359299DB}", "Sine" },
  { "{F5B224C4-CB9A-483C-A876-9F85E80DF4C0}", "Saw" },
  { "{03F4AF0D-4A6E-4A7B-BA93-87F61C5EEA8A}", "Pulse" },  
  { "{5793A503-33CC-42F2-A21F-37CAA7F20690}", "Tri" } };

static std::vector<time_signature> const lfo_timesig = synced_timesig(false, { 16, 1 }, {
  { timesig_type::one_over, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 16, 32, 64 } },
  { timesig_type::lower, { 2, 3, 4, 5, 6, 7, 8, 9, 12, 16 } },
  { timesig_type::upper, { 2, 3, 4, 5, 6, 7, 8 } },
  { timesig_type::over_one, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 } } });
std::vector<float> const lfo_timesig_values = synced_timesig_values(lfo_timesig); 
static std::vector<std::string> const lfo_timesig_names = synced_timesig_names(lfo_timesig);

static param_descriptor_data const lfo_on_data = { { "On", "On" }, param_kind::voice, false }; 
static param_descriptor_data const lfo_type_data = { { "Type", "Type" }, "", param_kind::voice, param_type::list, { &lfo_types, lfo_type::basic } };
static param_descriptor_data const lfo_bipolar_data = { { "Bipolar", "Bipolar" }, param_kind::voice, false };
static param_descriptor_data const lfo_sync_data = { { "Sync", "Tempo sync" }, param_kind::voice, false }; 
static param_descriptor_data const lfo_invert_data = { { "Invert", "Invert" }, param_kind::voice, false }; 
static param_descriptor_data const lfo_single_data = { { "Single", "Single cycle" }, param_kind::voice, false };
static param_descriptor_data const lfo_rate_data = { { "Rate", "Rate" }, "Hz", param_kind::continuous, quad_bounds(lfo_min_rate, lfo_max_rate, "1", 2) };
static param_descriptor_data const lfo_tempo_data = { { "Tempo", "Tempo" }, "", param_kind::voice, param_type::knob_list, { &lfo_timesig_names, "1/4" } };
static param_descriptor_data const lfo_filter_data = { { "Filter", "Filter" }, "%", param_kind::voice, percentage_01_bounds(1.0f) };
static param_descriptor_data const lfo_basic_type_data = { { "Basic", "Basic type" }, "", param_kind::voice, param_type::list, { &lfo_basic_types, lfo_basic_type::sine } };
static param_descriptor_data const lfo_basic_offset_data = { { "Offset", "Basic offset" }, "%", param_kind::voice, percentage_01_bounds(0.0f) };
static param_descriptor_data const lfo_basic_pw_data = { { "PW", "Basic PW" }, "%", param_kind::voice, percentage_01_bounds(1.0f) };
static param_descriptor_data const lfo_rand_type_data = { { "Rnd", "Random type" }, "", param_kind::voice, param_type::list, { &lfo_rand_types, lfo_rand_type::slope } };
static param_descriptor_data const lfo_rand_seedx_data = { { "SeedX", "Random seed X" }, "", param_kind::voice, param_type::knob, { 1, 256, 1 } };
static param_descriptor_data const lfo_rand_seedy_data = { { "SeedY", "Random seed Y" }, "", param_kind::voice, param_type::knob, { 1, 256, 1 } };
static param_descriptor_data const lfo_rand_rand_steps_data = { { "RandX", "Randomize X" }, param_kind::voice, false };
static param_descriptor_data const lfo_rand_steps_data = { { "Steps", "Random steps" }, "", param_kind::voice, param_type::list_knob, { lfo_rand_min_steps, lfo_rand_max_steps, lfo_rand_default_steps } };
static param_descriptor_data const lfo_rand_amt_data = { { "Amt", "Random amount" }, "%", param_kind::voice, percentage_01_bounds(1.0f) };
static param_descriptor_data const lfo_free_delay1_data = { { "Dly", "Free delay 1" }, "%", param_kind::voice, percentage_01_bounds(0.5f) };
static param_descriptor_data const lfo_free_rise1_data = { { "A1", "Free A1" }, "%", param_kind::voice, percentage_01_bounds(0.5f) };
static param_descriptor_data const lfo_free_rise1_slope_data = { { "S", "Free A1 slope" }, "", param_kind::voice, param_type::list_knob, discrete_2way_bounds(lfo_slope_range, 1) };
static param_descriptor_data const lfo_free_hold1_data = { { "Hld", "Free hold 1" }, "%", param_kind::voice, percentage_01_bounds(0.5f) };
static param_descriptor_data const lfo_free_fall1_data = { { "D1", "Free D1" }, "%", param_kind::voice, percentage_01_bounds(0.5f) };
static param_descriptor_data const lfo_free_fall1_slope_data = { { "S", "Free D1 slope" }, "", param_kind::voice, param_type::list_knob, discrete_2way_bounds(lfo_slope_range, 1) };
static param_descriptor_data const lfo_free_delay2_data = { { "Dly", "Free delay 2" }, "%", param_kind::voice, percentage_01_bounds(0.5f) };
static param_descriptor_data const lfo_free_fall2_data = { { "D2", "Free D2" }, "%", param_kind::voice, percentage_01_bounds(0.5f) };
static param_descriptor_data const lfo_free_fall2_slope_data = { { "S", "Free D2 slope" }, "", param_kind::voice, param_type::list_knob, discrete_2way_bounds(lfo_slope_range, 1) };
static param_descriptor_data const lfo_free_hold2_data = { { "Hld", "Free hold 2" }, "%", param_kind::voice, percentage_01_bounds(0.5f) };
static param_descriptor_data const lfo_free_rise2_data = { { "A2", "Free A2" }, "%", param_kind::voice, percentage_01_bounds(0.5f) };
static param_descriptor_data const lfo_free_rise2_slope_data = { { "S", "Free A2 slope" }, "", param_kind::voice, param_type::list_knob, discrete_2way_bounds(lfo_slope_range, 1) };
  
param_descriptor const    
vlfo_params[lfo_param::count] =  
{
  { "{42FB0553-788E-470F-906A-D95FED2ED980}", lfo_on_data },
  { "{F744C553-8CFA-4262-98A7-37E187BF27FF}", lfo_type_data },
  { "{0E9A5C22-34A2-4658-9DE6-718DF9028ECD}", lfo_bipolar_data },
  { "{83C1ED1B-095E-4F58-B091-39DA4F0125BF}", lfo_sync_data },
  { "{A9772BFB-ED32-42D9-A58A-A730A6634DAA}", lfo_invert_data },
  { "{C7092192-1E06-43BC-B3D0-BFA7A0492AE6}", lfo_single_data },
  { "{E320A1F0-2FCA-46F2-BBCB-0504D65503BC}", lfo_rate_data },
  { "{09618D35-EFAD-4E2E-8FD0-04B6F5AC14D5}", lfo_tempo_data }, 
  { "{A1C97D35-42E2-47F1-B9BA-11D8F286B67F}", lfo_filter_data },
  { "{071A82CE-AF06-4843-96D4-4D26CA66A1D9}", lfo_basic_type_data },
  { "{3522F726-2A14-4D50-A8D7-18472144328C}", lfo_basic_offset_data }, 
  { "{43BE423A-9F74-4041-B601-7CB0C4F3F0A3}", lfo_basic_pw_data },
  { "{86C7F58F-CD51-4E69-AD66-B2972B8D4B2E}", lfo_rand_type_data },
  { "{3234DC7B-4345-4DEA-8BDA-EEFF6439580A}", lfo_rand_seedx_data },
  { "{A4474F5D-8B86-4285-B0ED-33188A6DC01F}", lfo_rand_seedy_data },
  { "{821E8CA4-3EE8-45CF-B526-10D46FC390CA}", lfo_rand_rand_steps_data },
  { "{DDA30530-DBCD-4DBD-861C-8C2D9F282397}", lfo_rand_steps_data },
  { "{80FD8720-8423-477B-8EA8-08F989FA302E}", lfo_rand_amt_data },
  { "{A9DBEA7B-2FC2-4B15-A11C-719B9A1ABD38}", lfo_free_delay1_data },
  { "{D181F306-132F-4C1E-A7FD-72D1AD43E8B8}", lfo_free_rise1_data },
  { "{E9BFE976-4D66-4A9E-ACC6-869275AB0AB6}", lfo_free_rise1_slope_data },
  { "{8533C007-D714-415D-890F-C1CF66B181F6}", lfo_free_hold1_data },
  { "{FB0BF491-3AB4-4071-A8B3-88511992FA1C}", lfo_free_fall1_data },
  { "{42410A47-C6DC-472E-8C23-306D10456CEB}", lfo_free_fall1_slope_data },
  { "{CB4EA873-CFC1-45D6-B9EB-FF0B73F23ECF}", lfo_free_delay2_data },
  { "{D92339D4-0249-4CE8-B079-A0F65337FB6D}", lfo_free_fall2_data },
  { "{5A7CFDDF-EFD5-45BF-8599-5258A7450398}", lfo_free_fall2_slope_data },
  { "{EE4E2CBD-F6F2-407C-9F1C-C20778816F6C}", lfo_free_hold2_data },
  { "{95E2A5F2-D7EC-4077-98D6-D3AE00167BAE}", lfo_free_rise2_data },
  { "{FFC394AE-5BD5-40BD-BDF9-79DD9ED5D578}", lfo_free_rise2_slope_data }
};
 
param_descriptor const
glfo_params[lfo_param::count] = 
{  
  { "{EDA8B82B-D3FA-4B3A-8FB8-21A844971036}", lfo_on_data },
  { "{0852FEF9-4968-4351-A3DB-8325D943C898}", lfo_type_data },
  { "{B7F5C49B-4D92-43D9-B82B-715C6790FD7E}", lfo_bipolar_data },
  { "{08D47B00-6F72-4A77-B66E-82831A99B9E9}", lfo_sync_data },
  { "{CA68DCB1-A2D4-466D-A281-6972C4078EF8}", lfo_invert_data },
  { "{4F3974D2-BB30-4FE6-9247-73EC446CDC57}", lfo_single_data },
  { "{25BEB985-0465-46A4-AB1D-AB4CD4BDB6D3}", lfo_rate_data },
  { "{8D2394E4-D009-4286-8258-F98343BBD840}", lfo_tempo_data },
  { "{90055DA6-C7F6-41DC-BEC7-80B0153B0968}", lfo_filter_data },
  { "{69D618B6-9FFE-4C13-9BFD-899271EE4A00}", lfo_basic_type_data },
  { "{F98D6477-FDF4-4389-96DC-2E369DC0F6B9}", lfo_basic_offset_data },
  { "{B80D3669-EF18-468A-81F8-5DF1C750E009}", lfo_basic_pw_data },
  { "{BC3D8BC1-6596-4CA0-A51A-4128907225B7}", lfo_rand_type_data },
  { "{57DCF0B9-C6FF-4C1E-A673-90EF55DF718B}", lfo_rand_seedx_data }, 
  { "{1EA74D6F-0C8B-4C96-9E35-2C4145860A38}", lfo_rand_seedy_data },
  { "{CA48F606-4CDD-468A-B6BE-00AEB08686C5}", lfo_rand_rand_steps_data }, 
  { "{7EC1653A-CC1A-4817-B01B-94967103FE0F}", lfo_rand_steps_data },
  { "{FAFEA501-D172-4C86-8243-3E6F8CBB364C}", lfo_rand_amt_data },
  { "{61ED73C7-3330-46E5-9F61-22B409E9749E}", lfo_free_delay1_data },
  { "{47665BB3-9B18-46DE-94D7-2D45806F7631}", lfo_free_rise1_data },
  { "{B7F7000F-512D-4BBA-ADBE-BB6E4D61D96D}", lfo_free_rise1_slope_data },
  { "{EF442864-1EDA-4CB0-A79F-E924C076A1A7}", lfo_free_hold1_data },
  { "{91662BE5-C41E-4671-ABFA-19A2BD9B1997}", lfo_free_fall1_data },
  { "{564ADE6B-A3C9-41C8-86BC-21B29B00209B}", lfo_free_fall1_slope_data },
  { "{3A58B865-925D-48C9-881E-4E6C355C065C}", lfo_free_delay2_data },
  { "{C36BF7CA-AF6D-4E04-8007-6E794259D797}", lfo_free_fall2_data },
  { "{3D461FC4-647D-422F-810D-1D0AF155A930}", lfo_free_fall2_slope_data },
  { "{82C72D01-D46E-449C-A545-2096508CE9FB}", lfo_free_hold2_data },
  { "{A273D009-20D6-446E-A4AF-C52F69F547BC}", lfo_free_rise2_data },
  { "{75BE0782-4E29-4111-8BDD-A4E1208B12EE}", lfo_free_rise2_slope_data }
};

} // namespace inf::synth