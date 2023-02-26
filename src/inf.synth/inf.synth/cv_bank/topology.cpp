#include <inf.synth/cv_bank/topology.hpp>
#include <inf.synth/envelope/topology.hpp>

#include <inf.base/shared/support.hpp>
#include <inf.base/topology/part_descriptor.hpp>
#include <inf.base/topology/param_ui_descriptor.hpp>

#include <ranges>

using namespace inf::base;

namespace inf::synth {

// borders
std::vector<box_descriptor> gcv_bank_borders()
{
  std::vector<box_descriptor> result;
  std::int32_t col_count = gcv_bank_table_col_count - 1;
  result.push_back(box_descriptor({ 0, 0, 2, 1 }));
  result.push_back(box_descriptor({ 2, 0, 2, 1 }));
  result.push_back(box_descriptor({ 4, 0, 3, 1 }));
  for(std::int32_t i = 1; i <= col_count; i++)
  {
    result.push_back(box_descriptor({ 0, i, 2, 1 }));
    result.push_back(box_descriptor({ 2, i, 2, 1 }));
    result.push_back(box_descriptor({ 4, i, 3, 1 }));
  }
  return result; 
}

std::vector<box_descriptor> vcv_bank_borders() 
{ 
  std::vector<box_descriptor> result;
  std::int32_t row_span = (vcv_bank_param::count - cv_bank_param_offset) / vcv_bank_table_col_count;
  result.push_back(box_descriptor({ 0, 0, 1, 2 }));
  result.push_back(box_descriptor({ 0, 2, 1, 2 })); 
  result.push_back(box_descriptor({ 0, 4, 1, 3 }));
  result.push_back(box_descriptor({ 1, 0, row_span, 2 }));
  result.push_back(box_descriptor({ 1, 2, row_span, 2 }));
  result.push_back(box_descriptor({ 1, 4, row_span, 3 }));
  return result;
} 

// operation
static std::vector<list_item> const cv_route_input_ops = {   
  { "{EFF13B0A-B9C3-4232-9EE3-38AB70434E0D}", "Add" },   
  { "{4FA2A827-DCE6-46FE-B78F-6D161982B538}", "Sub" },
  { "{C774C268-EF66-4482-9068-79AFCC60A090}", "Mul" } };
static std::vector<list_item> const cv_route_output_bases = {
  { "{49803D88-3801-444B-81B8-345BDA2C471D}", "Raw" },
  { "{93CB9C1C-8026-47D3-8548-D22DFFCA0204}", "Mod" },
  { "{70D6B78F-5D67-4A63-9131-59698685400B}", "Param" } }; 

// output routing
static list_item const vcv_route_outputs[vcv_route_output::count] = {
  { "{C318FB21-F79F-46DB-9DD9-C92DAA04430A}", "Off" },
  { "{0CD174A0-699B-401F-AE22-0B716C84A764}", "Osc" },
  { "{A04AB56A-6653-44B2-87CA-5AAA9E50249F}", "FX A" },
  { "{0A94D3C0-B00D-452D-A7F1-D1D30CC644C2}", "Audio A" },
  { "{73DD8E5A-F210-4E55-8FDC-4A1052E71D7E}", "Voice" } };
static list_item const gcv_route_outputs[gcv_route_output::count] = {
  { "{C80A4D13-6AE6-4D54-A477-E42E185F2C28}", "Off" },
  { "{F0CE73FF-7973-43A5-AD9C-75E67C689AE3}", "FX B" },
  { "{4BD18162-21CE-4B19-BAFA-05432C653EB0}", "Audio B" },
  { "{9E111E9D-8C9B-49B2-8D9D-A3A192E9DA4D}", "Master" } };

// input routing
static list_item const gcv_route_inputs[gcv_route_input::count] = {
  { "{E26F7364-965D-4CF3-AE95-178C1EEA8BCB}", "Off" },
  { "{E7FFCA1E-6777-4E10-85AA-F19B913AB87F}", "CV U" },
  { "{C8CF0423-29D7-4FB3-8B17-2B1B5AE735DC}", "CV B" },
  { "{862452FE-1786-48A8-9F60-33030BB8B3C9}", "LFO B" } };
static list_item const vcv_route_inputs[vcv_route_input::count] = {
  { "{76EDD4AA-E81B-4777-B46E-A0DE12B60541}", "Off" },
  { "{81C92941-AD92-47BD-B1DC-7A46AB6E088D}", "Velocity" },
  { "{6211FF30-9D1F-4011-B8AA-A3DF08D84B71}", "Key" },
  { "{9C81D502-DF00-4759-A2D0-B4FBCED9D9B0}", "Key Inv" },
  { "{F935E7CE-CC76-4548-B22F-4476A304BCEA}", "Env" },
  { "{E70042FB-3D4E-4822-98A5-B6CE5A58951B}", "CV U" },
  { "{9A13AB02-000C-4E3C-897C-0C1C9B7A603B}", "CV U" }, // Hold
  { "{C6F469EE-9A9A-4319-9825-ACDD75A14D6D}", "CV B" },
  { "{85D4AE4A-D83D-41AC-BB2B-81F7839126F5}", "CV B" }, // Hold
  { "{3498B234-D9A4-4A71-822B-C4304C3D2A63}", "LFO A" },
  { "{4D157622-02FA-4D9C-9B47-B999A7566D05}", "LFO B" },
  { "{7262E745-9116-4071-ACA7-0D11464E1892}", "LFO B" } }; // Hold

// target routing
static list_item const vgcv_route_amp_targets[vgcv_route_amp_target::count] = { 
  { "{4FD8B71A-3ACF-4D82-9070-4C1B5843C8A7}", "Gain" },
  { "{A3956376-EE40-45F1-BDA5-FE7A41F9BD60}", "Bal" } };
static list_item const vcv_route_audio_bank_targets[vcv_route_audio_bank_target::count] = {
  { "{327E458F-4686-44BA-9F32-68C58C53EBFB}", "Amt1" },
  { "{0925FBB7-3F1B-428C-8A00-F22D359CDFE3}", "Amt2" },
  { "{0A9F9198-192B-466F-911E-E3A83925670E}", "Amt3" },
  { "{A14EFB11-3FEF-4F0A-9398-E128E4C7F28D}", "Amt4" },
  { "{327E458F-4686-44BA-9F32-68C58C53EBFB}", "Bal1" },
  { "{0925FBB7-3F1B-428C-8A00-F22D359CDFE3}", "Bal2" },
  { "{0A9F9198-192B-466F-911E-E3A83925670E}", "Bal3" },
  { "{A14EFB11-3FEF-4F0A-9398-E128E4C7F28D}", "Bal4" } };
static list_item const gcv_route_audio_bank_targets[gcv_route_audio_bank_target::count] = {
  { "{AF20EA99-6827-4216-8CBA-5562F267F649}", "Amt1" },
  { "{44183897-5277-4A02-BED5-8D727E6C9C33}", "Amt2" },
  { "{2FA260E6-6384-4FE6-9D90-46AC3A4FCF28}", "Amt3" },
  { "{01B5D903-C453-4CB4-AEC2-39F88CC1A46B}", "Amt4" },
  { "{5B761DB0-688C-4396-96C5-2EE97A5513D0}", "Bal1" },
  { "{95126B9B-953F-4C21-870A-44C2D9396832}", "Bal2" },
  { "{CDB20D33-4427-4C70-BAFC-6B4A0A472757}", "Bal3" },
  { "{8308F1EE-2CB1-4F70-9644-118DC9798161}", "Bal4" } };
static list_item const vcv_route_vosc_targets[vcv_route_vosc_target::count] = {
  { "{8F07B23B-8F7E-4388-BBA7-968A30957F40}", "PM" },
  { "{987BE249-E672-4055-BFEA-BCDA828DB269}", "FM" }, 
  { "{132246C1-5D8D-45BD-ABA8-407BEC927FFD}", "Gain" },
  { "{7580316C-1C46-46B3-A230-E3F0BAA3634C}", "Cent" },
  { "{495D49A2-97F4-4B4D-A6B1-0BE194B09B68}", "Uni Dtn" }, 
  { "{0348A9C4-0414-418E-AC83-965C6F58487D}", "Uni Sprd" },
  { "{DBCDDC0D-8CA2-4DC1-A06C-A93AC8AF828A}", "RAM Bal" },
  { "{D1FB8DF5-9E17-493C-B564-324045FE18C1}", "RAM Mix" },
  { "{C0D18C5A-2BA6-4F1D-A1F0-C96A06F428E3}", "Bsc PW" },
  { "{6486FC45-83A2-4EAD-8782-84B6C31517C2}", "Mix Sine" },
  { "{EF423264-369A-417E-83A6-1CAEBEEC2A54}", "Mix Saw" },
  { "{EDDD2B9D-A472-4D58-A5B8-83984FE32F89}", "Mix Tri" },
  { "{D7D85D30-03A9-41AB-BB33-D7778491DB5A}", "Mix Pulse" },
  { "{54952EC5-1840-46D5-A73A-6E1BC0D09418}", "Mix PW" },
  { "{EF63EC0F-4F41-455A-9E89-B90A1F2365FA}", "DSF Dst" },
  { "{0B9DCACF-DD2F-4E05-8427-BE30E7914485}", "DSF Dcy" },
  { "{986660DA-E6EC-4B36-8AE0-D469B1BE0343}", "K+S Flt" },
  { "{58C3BC8F-8C44-4C88-BECA-4ADD765AF13A}", "K+S Fdbk" },
  { "{F4CC7E11-407F-4C05-834E-6DD0CDAE774B}", "K+S Str" },
  { "{8EBA9BAA-529F-445C-BEA1-D9CC17BC42A2}", "Nse Col" },
  { "{A3DF88E5-448D-47E1-B33E-AA0B932B4B38}", "Nse X" },
  { "{4C8CD2BD-54D3-466B-B733-68AE424724EF}", "Nse Y" } };
static list_item const vgcv_route_effect_targets[vgcv_route_effect_target::count] = {
  { "{682871AF-24F5-45FA-92E9-CDC0758CEFCF}", "SV Frq" },
  { "{A86529CD-46D7-412B-BCEE-C1C48D8A86A5}", "SV Res" },
  { "{2357A2F9-0189-4F01-9CA8-831FC5066A04}", "SV Kbd" },
  { "{B4370576-DE84-413E-96E2-3573C6C3709E}", "SV Shlf" },
  { "{1E877DD6-1CDE-4181-8301-7D3617225FC2}", "Cmb Dl+" },
  { "{2BC57564-B1DB-4CEA-8F88-A6621DC85AFE}", "Cmb Gn+" },
  { "{FF76F9FD-8DBD-4175-9C87-08C9A160A807}", "Cmb Dl-" },
  { "{CA428CA8-B31D-4E49-A99C-95CB3D35F1DD}", "Cmb Gn-" },
  { "{EC7157FC-13B0-4809-BEED-9ECD0123C0E1}", "Shp Mix" },
  { "{4475764D-B720-4A22-92F3-0BCFA511FB8B}", "Shp Gn" },
  { "{D9C994BE-7556-466D-AC84-F9BFA16C4E53}", "Shp Dcy" },
  { "{84333BE8-E252-4436-A1DD-77A389CB52D5}", "Dly Mix" },
  { "{FDF44D09-9E79-494B-8F79-7D120DDA3AEC}", "Dly Amt" },
  { "{5638F581-86B7-4F70-9D80-4BB21EAAAE6E}", "FbDly Sprd" },
  { "{3B1A6BFA-198E-4B35-8D3A-05F938E439A9}", "MtDly Sprd" },
  { "{87D0C08F-FC9F-4751-B00C-1A6FADF8F73C}", "Rev Mix" },
  { "{93352B10-5409-48DF-AAB2-E199CB5B7174}", "Rev Size" },
  { "{AA53FF25-0415-4890-A061-0B9563AC4C82}", "Rev Sprd" },
  { "{BCD6370B-D963-449F-B091-EE36C9893864}", "Rev Dmp" },
  { "{FD8D3763-B1C0-4F86-8985-8483D523815B}", "Rev APF" } };
   
// text spacing
static bool const gcv_route_input_spaces[gcv_route_input::count] = { true, false, false, false }; 
static bool const gcv_route_output_spaces[gcv_route_output::count] = { true, false, false, true };
static bool const vcv_route_output_spaces[vcv_route_output::count] = { true, true, false, false, true };
static bool const vcv_route_input_spaces[vcv_route_input::count] = { true, true, true, true, true, false, false, false, false, false, false, false };

// output targets  
static list_item const* const gcv_route_output_targets[gcv_route_output::count] = {
  nullptr, vgcv_route_effect_targets, gcv_route_audio_bank_targets, vgcv_route_amp_targets };
static list_item const* const vcv_route_output_targets[vcv_route_output::count] = {
  nullptr, vcv_route_vosc_targets, vgcv_route_effect_targets, vcv_route_audio_bank_targets, vgcv_route_amp_targets };
static std::vector<list_item> const vcv_route_output_target_list = zip_list_items( 
  vcv_route_outputs, vcv_route_output_spaces, vcv_route_output_counts,
  vcv_route_output_targets, vcv_route_output_target_counts, vcv_route_output::count);
static std::vector<list_item> const gcv_route_output_target_list = zip_list_items(
  gcv_route_outputs, gcv_route_output_spaces, gcv_route_output_counts,
  gcv_route_output_targets, gcv_route_output_target_counts, gcv_route_output::count);

// input sources
static char const* const vcv_route_input_suffixes[vcv_route_input::count] = {
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "Hold", nullptr, "Hold", nullptr, nullptr, "Hold" };
static list_item_info const vcv_route_input_infos[vcv_route_input::count] = {
  nullptr, nullptr, nullptr, nullptr, envelope_item_info, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
static std::vector<list_item> const vcv_route_input_sources_list = multi_list_items(
  vcv_route_inputs, vcv_route_input_infos, vcv_route_input_spaces, vcv_route_input_suffixes, vcv_route_input_counts, vcv_route_input::count, true);
static std::vector<list_item> const gcv_route_input_sources_list = multi_list_items(
  gcv_route_inputs, nullptr, gcv_route_input_spaces, nullptr, gcv_route_input_counts, gcv_route_input::count, false);
   
// plot params
static param_ui_descriptor const cv_plot_tgt_ui = { true, 0.0f, {} };

param_descriptor const
vcv_plot_params[cv_plot_param::count] =
{
  { "{CE1DC1C7-72C5-4811-8C35-8485FFAFFABC}", { {"Tgt", "Target"}, "", param_kind::ui, param_type::list, {&vcv_route_output_target_list, "Voice Gain" }, 0, &cv_plot_tgt_ui } },
  { "{41FB9033-220C-4DA7-836A-22808E17167F}", { { "Key", "Key hold" }, "Sec", param_kind::ui, quad_bounds(0.01f, 70.0f, "0.5", 2), 1, nullptr } }
};

param_descriptor const
gcv_plot_params[cv_plot_param::count] =
{
  { "{EBF618A3-D50D-439C-A2BA-95C06277276E}", { {"Tgt", "Target"}, "", param_kind::ui, param_type::list, {&gcv_route_output_target_list, "Master Gain" }, 0, &cv_plot_tgt_ui } },
  { "{60ED3115-0BD0-4870-9938-6D37759FE7D0}", { { "Length", "Length" }, "Sec", param_kind::ui, quad_bounds(0.01f, 70.0f, "2", 2), 1, nullptr } }
};

// voice params
param_descriptor const   
vcv_bank_params[vcv_bank_param::count] =            
{        
  { "{1F6DEE15-DEE7-443B-B9F8-E65BFF9C9C4A}", { { "On", "Enabled" }, param_kind::voice, false, -1, nullptr } },
  { "{3B025C6A-0230-491A-A51F-7CF1C81B69C9}", { { "In", "Input 1" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 }, 0, nullptr} },
  { "{2833E378-210B-404F-A4CB-0D6204A72CF0}", { { "In", "Input 2" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 }, 7, nullptr } },
  { "{25041AB5-2A06-4305-8009-C26D56311D26}", { { "In", "Input 3" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 }, 14, nullptr } },
  { "{5FDD8C86-8F2D-4613-BB98-BB673F502412}", { { "Out", "Output 1" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 }, 1, nullptr } },
  { "{D0B28D9E-8888-42EB-8D3C-177FB4585E42}", { { "Out", "Output 2" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 }, 8, nullptr } },
  { "{37420523-6A9D-4125-BAAB-24A28B9E0992}", { { "Out", "Output 3" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 }, 15, nullptr } },
  { "{8B64719E-7B90-4F8C-A3A5-1DB198723B91}", { { "Base", "Base 1" }, "", param_kind::voice, param_type::list, { &cv_route_output_bases, cv_route_output_base::mod }, 2, nullptr } },
  { "{FC8F74F9-4BC9-4B6E-9F95-4B02973F4487}", { { "Base", "Base 2" }, "", param_kind::voice, param_type::list, { &cv_route_output_bases, cv_route_output_base::mod  }, 9, nullptr } },
  { "{2FB8E7B3-D095-44AD-B044-38B3492750FD}", { { "Base", "Base 3" }, "", param_kind::voice, param_type::list, { &cv_route_output_bases, cv_route_output_base::mod  }, 16, nullptr } },
  { "{18EBC834-CF60-4A68-BAF2-C57BC0BAE55E}", { { "Op", "Op 1" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 }, 3, nullptr} },
  { "{CD10C60A-C25E-46A5-879E-C692E50AE36B}", { { "Op", "Op 2" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 }, 10, nullptr } },
  { "{C6D7FDD5-102A-4972-B0B2-77F3977C9046}", { { "Op", "Op 3" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 }, 17, nullptr } },
  { "{0C1E5C81-01EE-4AE6-A05C-199210B904CC}", { { "Off", "Offset 1" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 4, nullptr } },
  { "{20F28A5F-2956-412E-BD97-9220836C22A3}", { { "Off", "Offset 2" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 11, nullptr } },
  { "{BFFA2360-5A5F-4575-A0E4-32B8B74977BD}", { { "Off", "Offset 3" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 18, nullptr } },
  { "{20ACF437-2158-4900-8DC4-D36767442BF1}", { { "Scl", "Scale 1" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 5, nullptr } },
  { "{23EFCBDF-7744-4527-B82D-A69E56C7338B}", { { "Scl", "Scale 2" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 12, nullptr } },
  { "{87087F9C-62A2-4804-9292-CD9089F025B7}", { { "Scl", "Scale 3" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 19, nullptr } },
  { "{469D130F-2E4A-4960-871D-032B6F588313}", { { "Amt", "Amount 1" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 6, nullptr } },
  { "{58AFE21F-7945-4919-BB67-60CE8892A8AF}", { { "Amt", "Amount 2" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 13, nullptr } },
  { "{9C1F6472-6D48-42E6-B79E-3A00F33F70F5}", { { "Amt", "Amount 3" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 20, nullptr } }
};

// global params
param_descriptor const 
gcv_bank_params[gcv_bank_param::count] = 
{ 
  { "{C64A2AE2-E96D-487E-8373-3DA9DBB7B028}", { { "On", "Enabled" }, param_kind::block, false, -1, nullptr } },
  { "{84736ED7-FF72-4E69-AFFF-A8607B0F3041}", { { "In", "Input 1" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 }, 0, nullptr } },
  { "{36981D81-6FC1-42C1-A380-D0813C624D93}", { { "In", "Input 2" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 }, 1, nullptr } },
  { "{9517E1DC-8069-4F00-915C-A0686DD3FB26}", { { "In", "Input 3" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 }, 2, nullptr } },
  { "{EB0A819C-174B-41B8-B8E8-E91377441D66}", { { "In", "Input 4" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 }, 3, nullptr } },
  { "{4548DE68-1D70-4307-BBD2-09838CAC4701}", { { "Out", "Output 1" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 }, 4, nullptr } },
  { "{A5857037-CB45-4F83-AF2F-2C4083628E63}", { { "Out", "Output 2" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 }, 5, nullptr } },
  { "{DC95DEF0-DD36-493A-B97A-E580DEB8BEBD}", { { "Out", "Output 3" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 }, 6, nullptr } },
  { "{86D57462-7C08-4DD6-8FAC-EEC7BD0ABE34}", { { "Out", "Output 4" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 }, 7, nullptr } },
  { "{906B9EBE-3E68-4F9C-97E9-2B78EE563C38}", { { "Base", "Base 1" }, "", param_kind::block, param_type::list, { &cv_route_output_bases, cv_route_output_base::mod }, 8, nullptr } },
  { "{760FEBEF-14E3-4C14-95A6-3106438B66F4}", { { "Base", "Base 2" }, "", param_kind::block, param_type::list, { &cv_route_output_bases, cv_route_output_base::mod  }, 9, nullptr } },
  { "{9F32E255-F36D-4D97-84BF-49DF558862C0}", { { "Base", "Base 3" }, "", param_kind::block, param_type::list, { &cv_route_output_bases, cv_route_output_base::mod  }, 10, nullptr } },
  { "{71EE0E3E-3F93-4175-978B-EC80E6C97D4A}", { { "Base", "Base 4" }, "", param_kind::block, param_type::list, { &cv_route_output_bases, cv_route_output_base::mod  }, 11, nullptr } },
  { "{A526F21F-4C41-4925-90FD-D6BA442225E7}", { { "Op", "Op 1" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 }, 12, nullptr} },
  { "{FDFF29BB-E361-4350-9958-8266013A3124}", { { "Op", "Op 2" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 }, 13, nullptr } },
  { "{53EE242B-B967-499B-821B-4BF5419E08E5}", { { "Op", "Op 3" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 }, 14, nullptr } },
  { "{CD0E58CA-C183-4B59-9F45-2F0805379A1D}", { { "Op", "Op 4" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 }, 15, nullptr } },
  { "{41F96374-D208-4A29-911E-18CF7C27A6A0}", { { "Off", "Offset 1" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 16, nullptr } },
  { "{4CBAC51D-9597-48BF-9210-D9CA33E8DCD1}", { { "Off", "Offset 2" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 17, nullptr } },
  { "{D2EF64CE-1737-4776-AD8D-F28D5E359960}", { { "Off", "Offset 3" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 18, nullptr } },
  { "{44061640-FDE6-4E81-BDE0-6145A6EBD494}", { { "Off", "Offset 4" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 19, nullptr } },
  { "{5ACBDB05-BFCF-4A85-809B-D81DB8E835DE}", { { "Scl", "Scale 1" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 20, nullptr } },
  { "{2E03B188-2616-457B-ACC2-F1B735370420}", { { "Scl", "Scale 2" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 21, nullptr } },
  { "{45AE23A8-8EC6-4901-BECE-CB631FDFAA4D}", { { "Scl", "Scale 3" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 22, nullptr } },
  { "{D2937D43-3DEE-4657-A9E0-BD2B1323FA3A}", { { "Scl", "Scale 4" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 23, nullptr } },
  { "{A927E108-FB33-4AFC-A46A-726705004F78}", { { "Amt", "Amount 1" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 24, nullptr } },
  { "{98AF1BBD-D01A-41EF-82DC-FD852FD7154B}", { { "Amt", "Amount 2" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 25, nullptr } },
  { "{48F63E35-4D42-4EC1-AC9A-8E0CF2278095}", { { "Amt", "Amount 3" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 26, nullptr } },
  { "{2EC4D924-1912-4111-8AC1-B24D84384618}", { { "Amt", "Amount 4" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 27, nullptr } }
}; 

} // namespace inf::synth