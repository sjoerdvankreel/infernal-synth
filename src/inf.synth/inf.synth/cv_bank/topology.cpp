#include <inf.synth/cv_bank/topology.hpp>
#include <inf.synth/envelope/topology.hpp>

#include <inf.base/shared/support.hpp>
#include <inf.base/topology/part_descriptor.hpp>
#include <inf.base/topology/param_ui_descriptor.hpp>

#include <ranges>

using namespace inf::base;

namespace inf::synth {

std::vector<box_descriptor> cv_bank_borders(std::int32_t param_count) 
{ 
  std::vector<box_descriptor> result;
  std::int32_t row_span = (param_count - cv_bank_param_offset) / cv_bank_table_col_count;
  result.push_back(box_descriptor({ 0, 0, 1, 2 }));
  result.push_back(box_descriptor({ 0, 2, 1, 2 })); 
  result.push_back(box_descriptor({ 0, 4, 1, 2 }));
  result.push_back(box_descriptor({ 1, 0, row_span, 2 }));
  result.push_back(box_descriptor({ 1, 2, row_span, 2 })); 
  result.push_back(box_descriptor({ 1, 4, row_span, 2 }));
  return result;  
}  

// operation
static std::vector<list_item> const cv_route_input_ops = {   
  { "{EFF13B0A-B9C3-4232-9EE3-38AB70434E0D}", "Add" },   
  { "{4FA2A827-DCE6-46FE-B78F-6D161982B538}", "Sub" },
  { "{C774C268-EF66-4482-9068-79AFCC60A090}", "Mul" } };

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
  { "{33038A70-7256-46A8-AF33-6676157709C4}", "CV U" },
  { "{1209DAFC-6F0B-452E-B613-01097DB6249A}", "CV U" }, // Hold
  { "{C6F469EE-9A9A-4319-9825-ACDD75A14D6D}", "CV B" },
  { "{85D4AE4A-D83D-41AC-BB2B-81F7839126F5}", "CV B" }, // Hold
  { "{3498B234-D9A4-4A71-822B-C4304C3D2A63}", "LFO A" },
  { "{4D157622-02FA-4D9C-9B47-B999A7566D05}", "LFO B" },
  { "{7262E745-9116-4071-ACA7-0D11464E1892}", "LFO B" } }; // Hold

// target routing
static list_item const vgcv_route_amp_targets[vgcv_route_amp_target::count] = { 
  { "{4FD8B71A-3ACF-4D82-9070-4C1B5843C8A7}", "Gain" },
  { "{A3956376-EE40-45F1-BDA5-FE7A41F9BD60}", "Bal" } };
static list_item const vcv_route_audio_bank_targets[vgcv_route_audio_bank_target::count] = {
  { "{327E458F-4686-44BA-9F32-68C58C53EBFB}", "Amt1" },
  { "{0925FBB7-3F1B-428C-8A00-F22D359CDFE3}", "Amt2" },
  { "{0A9F9198-192B-466F-911E-E3A83925670E}", "Amt3" },
  { "{A14EFB11-3FEF-4F0A-9398-E128E4C7F28D}", "Amt4" },
  { "{EE4F0932-4046-4910-95DF-E452B143E94E}", "Amt5" },
  { "{D11DB66D-F4E6-4821-809E-A532A2A3AD79}", "Amt6" },
  { "{327E458F-4686-44BA-9F32-68C58C53EBFB}", "Bal1" },
  { "{0925FBB7-3F1B-428C-8A00-F22D359CDFE3}", "Bal2" },
  { "{0A9F9198-192B-466F-911E-E3A83925670E}", "Bal3" }, 
  { "{A14EFB11-3FEF-4F0A-9398-E128E4C7F28D}", "Bal4" },
  { "{A89056B2-4C8A-4027-96E8-4E0D90D7540F}", "Bal5" },
  { "{39FE9B13-131B-4347-86AC-D65CA377F4B5}", "Bal6" } };
static list_item const gcv_route_audio_bank_targets[vgcv_route_audio_bank_target::count] = {
  { "{AF20EA99-6827-4216-8CBA-5562F267F649}", "Amt1" },
  { "{44183897-5277-4A02-BED5-8D727E6C9C33}", "Amt2" },
  { "{2FA260E6-6384-4FE6-9D90-46AC3A4FCF28}", "Amt3" },
  { "{01B5D903-C453-4CB4-AEC2-39F88CC1A46B}", "Amt4" },
  { "{27F98374-E8E4-4954-9D9F-529A5EE3FDEC}", "Amt5" },
  { "{EC143428-C74F-43BE-96DD-288E37C15AB0}", "Amt6" },
  { "{5B761DB0-688C-4396-96C5-2EE97A5513D0}", "Bal1" },
  { "{95126B9B-953F-4C21-870A-44C2D9396832}", "Bal2" },
  { "{CDB20D33-4427-4C70-BAFC-6B4A0A472757}", "Bal3" },
  { "{8308F1EE-2CB1-4F70-9644-118DC9798161}", "Bal4" },
  { "{008C5F96-D8A5-4420-8C31-5987105B88ED}", "Bal5" },
  { "{45354B68-2389-4DB7-8472-4F44CF2B8A4F}", "Bal6" } };
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
  { "{2833E378-210B-404F-A4CB-0D6204A72CF0}", { { "In", "Input 2" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 }, 6, nullptr } },
  { "{25041AB5-2A06-4305-8009-C26D56311D26}", { { "In", "Input 3" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 }, 12, nullptr } },
  { "{B4C3E2E0-106D-4377-93F6-711561D5F1DA}", { { "In", "Input 4" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 }, 18, nullptr } },
  { "{DE53604C-CC85-40EA-B0AC-6CB00050EB4C}", { { "In", "Input 5" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 }, 24, nullptr } },
  { "{5FDD8C86-8F2D-4613-BB98-BB673F502412}", { { "Out", "Output 1" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 }, 1, nullptr } },
  { "{D0B28D9E-8888-42EB-8D3C-177FB4585E42}", { { "Out", "Output 2" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 }, 7, nullptr } },
  { "{37420523-6A9D-4125-BAAB-24A28B9E0992}", { { "Out", "Output 3" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 }, 13, nullptr } },
  { "{32C5980E-F41A-441C-85A8-3AC90A4AAD0A}", { { "Out", "Output 4" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 }, 19, nullptr } },
  { "{055B8D75-97ED-47E1-9A81-4FA5A3072E30}", { { "Out", "Output 5" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 }, 25, nullptr } },
  { "{18EBC834-CF60-4A68-BAF2-C57BC0BAE55E}", { { "Op", "Op 1" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 }, 2, nullptr} },
  { "{CD10C60A-C25E-46A5-879E-C692E50AE36B}", { { "Op", "Op 2" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 }, 8, nullptr } },
  { "{C6D7FDD5-102A-4972-B0B2-77F3977C9046}", { { "Op", "Op 3" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 }, 14, nullptr } },
  { "{927B6CF5-D576-42F9-80BA-4C83437F9041}", { { "Op", "Op 4" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 }, 20, nullptr } },
  { "{EDA84031-7061-448F-BCEB-6D941D772EFF}", { { "Op", "Op 5" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 }, 26, nullptr } },
  { "{469D130F-2E4A-4960-871D-032B6F588313}", { { "Amt", "Amount 1" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 3, nullptr } },
  { "{58AFE21F-7945-4919-BB67-60CE8892A8AF}", { { "Amt", "Amount 2" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 9, nullptr } },
  { "{9C1F6472-6D48-42E6-B79E-3A00F33F70F5}", { { "Amt", "Amount 3" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 15, nullptr } },
  { "{D04F0B81-2E96-42D7-895C-DFC47BA36B4C}", { { "Amt", "Amount 4" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 21, nullptr } },
  { "{C0E8DD3E-DCAD-48FA-9C4E-BB0FDDFC2554}", { { "Amt", "Amount 5" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 27, nullptr } },
  { "{0C1E5C81-01EE-4AE6-A05C-199210B904CC}", { { "Offset", "Offset 1" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 4, nullptr } },
  { "{20F28A5F-2956-412E-BD97-9220836C22A3}", { { "Offset", "Offset 2" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 10, nullptr } },
  { "{BFFA2360-5A5F-4575-A0E4-32B8B74977BD}", { { "Offset", "Offset 3" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 16, nullptr } },
  { "{B46AC7BD-982C-4A9D-B74B-AE27C25BE811}", { { "Offset", "Offset 4" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 22, nullptr } },
  { "{542280B8-942D-4FAA-AB26-5A39CE3FFF71}", { { "Offset", "Offset 5" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 28, nullptr } },
  { "{20ACF437-2158-4900-8DC4-D36767442BF1}", { { "Scale", "Scale 1" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 5, nullptr } },
  { "{23EFCBDF-7744-4527-B82D-A69E56C7338B}", { { "Scale", "Scale 2" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 11, nullptr } },
  { "{87087F9C-62A2-4804-9292-CD9089F025B7}", { { "Scale", "Scale 3" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 17, nullptr } },
  { "{D58E30EB-8F46-4EB7-84F2-37AA48F81721}", { { "Scale", "Scale 4" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 23, nullptr } },
  { "{62A7448C-3245-4B33-AF4E-42D98D3AD547}", { { "Scale", "Scale 5" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 29, nullptr } }
};

// global params
param_descriptor const 
gcv_bank_params[gcv_bank_param::count] = 
{ 
  { "{C64A2AE2-E96D-487E-8373-3DA9DBB7B028}", { { "On", "Enabled" }, param_kind::block, false, -1, nullptr } },
  { "{84736ED7-FF72-4E69-AFFF-A8607B0F3041}", { { "In", "Input 1" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 }, 0, nullptr } },
  { "{36981D81-6FC1-42C1-A380-D0813C624D93}", { { "In", "Input 2" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 }, 6, nullptr } },
  { "{9517E1DC-8069-4F00-915C-A0686DD3FB26}", { { "In", "Input 3" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 }, 12, nullptr } },
  { "{EB0A819C-174B-41B8-B8E8-E91377441D66}", { { "In", "Input 4" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 }, 18, nullptr } },
  { "{460C97B9-5DA2-40D9-B553-B59784A972B7}", { { "In", "Input 5" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 }, 24, nullptr } },
  { "{C26316CE-3855-44C4-8C0D-0AD3A06443CD}", { { "In", "Input 6" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 }, 30, nullptr } },
  { "{5D4EB303-0B5C-48A3-BC62-79571DD51387}", { { "In", "Input 7" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 }, 36, nullptr } },
  { "{FA7C27DD-3EFB-48A1-96FD-89BD7F195C62}", { { "In", "Input 8" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 }, 42, nullptr } },
  { "{4548DE68-1D70-4307-BBD2-09838CAC4701}", { { "Out", "Output 1" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 }, 1, nullptr } },
  { "{A5857037-CB45-4F83-AF2F-2C4083628E63}", { { "Out", "Output 2" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 }, 7, nullptr } },
  { "{DC95DEF0-DD36-493A-B97A-E580DEB8BEBD}", { { "Out", "Output 3" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 }, 13, nullptr } },
  { "{86D57462-7C08-4DD6-8FAC-EEC7BD0ABE34}", { { "Out", "Output 4" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 }, 19, nullptr } },
  { "{CF0921FB-E525-4217-A249-CBEDFDB58B72}", { { "Out", "Output 5" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 }, 25, nullptr } },
  { "{326AEE47-2AFC-4140-8AD3-A8B1AE1FD1DF}", { { "Out", "Output 6" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 }, 31, nullptr } },
  { "{C7A5A80A-5B69-4F42-BD92-57A0FA606DFE}", { { "Out", "Output 7" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 }, 37, nullptr } },
  { "{02FE5916-CD9C-4AB9-8D47-25198095090F}", { { "Out", "Output 8" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 }, 43, nullptr } },
  { "{A526F21F-4C41-4925-90FD-D6BA442225E7}", { { "Op", "Op 1" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 }, 2, nullptr} },
  { "{FDFF29BB-E361-4350-9958-8266013A3124}", { { "Op", "Op 2" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 }, 8, nullptr } },
  { "{53EE242B-B967-499B-821B-4BF5419E08E5}", { { "Op", "Op 3" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 }, 14, nullptr } },
  { "{CD0E58CA-C183-4B59-9F45-2F0805379A1D}", { { "Op", "Op 4" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 }, 20, nullptr } },
  { "{2D718791-CF63-4A17-A1E3-EBB1DD675F38}", { { "Op", "Op 5" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 }, 26, nullptr } },
  { "{9D3027D8-5146-45A6-BAF7-036DBF3D2345}", { { "Op", "Op 6" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 }, 32, nullptr } },
  { "{DF3A60E1-42E1-4E13-AA33-F8C56C877A92}", { { "Op", "Op 7" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 }, 38, nullptr } },
  { "{1E668318-1135-4325-A10A-7EC6F0A73D24}", { { "Op", "Op 8" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 }, 44, nullptr } },
  { "{A927E108-FB33-4AFC-A46A-726705004F78}", { { "Amt", "Amount 1" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 3, nullptr } },
  { "{98AF1BBD-D01A-41EF-82DC-FD852FD7154B}", { { "Amt", "Amount 2" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 9, nullptr } },
  { "{48F63E35-4D42-4EC1-AC9A-8E0CF2278095}", { { "Amt", "Amount 3" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 15, nullptr } },
  { "{2EC4D924-1912-4111-8AC1-B24D84384618}", { { "Amt", "Amount 4" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 21, nullptr } },
  { "{E8B33588-CF41-44B4-A2CB-A12E520A8A84}", { { "Amt", "Amount 5" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 27, nullptr } },
  { "{9806BBDC-23A5-4038-B4D0-1B541238E915}", { { "Amt", "Amount 6" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 33, nullptr } },
  { "{7BC87BF9-DBFC-4092-A03F-E93292DA7963}", { { "Amt", "Amount 7" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 39, nullptr } },
  { "{B59683B5-2031-46CC-8C95-B5B291320601}", { { "Amt", "Amount 8" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 45, nullptr } },
  { "{41F96374-D208-4A29-911E-18CF7C27A6A0}", { { "Offset", "Offset 1" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 4, nullptr } },
  { "{4CBAC51D-9597-48BF-9210-D9CA33E8DCD1}", { { "Offset", "Offset 2" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 10, nullptr } },
  { "{D2EF64CE-1737-4776-AD8D-F28D5E359960}", { { "Offset", "Offset 3" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 16, nullptr } },
  { "{44061640-FDE6-4E81-BDE0-6145A6EBD494}", { { "Offset", "Offset 4" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 22, nullptr } },
  { "{31E160CE-5424-42B6-858E-07FC00C5B502}", { { "Offset", "Offset 5" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 28, nullptr } },
  { "{72E153D2-4614-4F91-B348-DF7AE0453927}", { { "Offset", "Offset 6" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 34, nullptr } },
  { "{31D9A605-F16B-47AB-B5F4-C03B285D7EDA}", { { "Offset", "Offset 7" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 40, nullptr } },
  { "{9A949AC3-CFF8-49CC-81BC-78B59B2E4F8A}", { { "Offset", "Offset 8" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 46, nullptr } },
  { "{5ACBDB05-BFCF-4A85-809B-D81DB8E835DE}", { { "Scale", "Scale 1" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 5, nullptr } },
  { "{2E03B188-2616-457B-ACC2-F1B735370420}", { { "Scale", "Scale 2" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 11, nullptr } },
  { "{45AE23A8-8EC6-4901-BECE-CB631FDFAA4D}", { { "Scale", "Scale 3" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 17, nullptr } },
  { "{D2937D43-3DEE-4657-A9E0-BD2B1323FA3A}", { { "Scale", "Scale 4" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 23, nullptr } },
  { "{9B15F803-4776-459D-98D8-B5563F217159}", { { "Scale", "Scale 5" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 29, nullptr } }, 
  { "{87F14A8F-2462-4CAE-9E8F-A87F95A20369}", { { "Scale", "Scale 6" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 35, nullptr } },
  { "{DF70143D-441E-4C77-BCF5-6535B4316B64}", { { "Scale", "Scale 7" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 41, nullptr } },
  { "{12EE4A5C-1BA4-4DC5-80B7-0FEF76EA5A59}", { { "Scale", "Scale 8" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 47, nullptr } }
};
 
} // namespace inf::synth