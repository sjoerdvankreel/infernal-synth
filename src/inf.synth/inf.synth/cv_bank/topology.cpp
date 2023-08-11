#include <inf.synth/cv_bank/topology.hpp>
#include <inf.synth/envelope/topology.hpp>

#include <inf.base/shared/support.hpp>
#include <inf.base/topology/part_descriptor.hpp>

#include <ranges>

using namespace inf::base;

namespace inf::synth {

// operation
static std::vector<list_item> const cv_route_input_ops = {   
  { "{EFF13B0A-B9C3-4232-9EE3-38AB70434E0D}", "Add" },   
  { "{4FA2A827-DCE6-46FE-B78F-6D161982B538}", "Sub" },
  { "{C774C268-EF66-4482-9068-79AFCC60A090}", "Mul" } };

// output routing
static list_item const vcv_route_outputs[vcv_route_output::count] = {
  { "{C318FB21-F79F-46DB-9DD9-C92DAA04430A}", "Off" },
  { "{0CD174A0-699B-401F-AE22-0B716C84A764}", "Osc" },
  { "{A04AB56A-6653-44B2-87CA-5AAA9E50249F}", "V.FX" },
  { "{0A94D3C0-B00D-452D-A7F1-D1D30CC644C2}", "V.Audio" },
  { "{73DD8E5A-F210-4E55-8FDC-4A1052E71D7E}", "V.Out" } };
static list_item const gcv_route_outputs[gcv_route_output::count] = {
  { "{C80A4D13-6AE6-4D54-A477-E42E185F2C28}", "Off" },
  { "{F0CE73FF-7973-43A5-AD9C-75E67C689AE3}", "G.FX" },
  { "{4BD18162-21CE-4B19-BAFA-05432C653EB0}", "G.Audio" },
  { "{9E111E9D-8C9B-49B2-8D9D-A3A192E9DA4D}", "M.Out" } };

// input routing
static list_item const gcv_route_inputs[gcv_route_input::count] = {
  { "{E26F7364-965D-4CF3-AE95-178C1EEA8BCB}", "Off" },
  { "{AA6C4A15-05D1-4732-B90E-51980F44A294}", "MIDI PB" },
  { "{294CEAF7-C69B-4ECA-BB44-A727E7872DCA}", "CVU" },
  { "{C8CF0423-29D7-4FB3-8B17-2B1B5AE735DC}", "CVB" },
  { "{862452FE-1786-48A8-9F60-33030BB8B3C9}", "G.LFO" } };
static list_item const vcv_route_inputs[vcv_route_input::count] = {
  { "{76EDD4AA-E81B-4777-B46E-A0DE12B60541}", "Off" },
  { "{81C92941-AD92-47BD-B1DC-7A46AB6E088D}", "Velocity" },
  { "{6211FF30-9D1F-4011-B8AA-A3DF08D84B71}", "Key" },
  { "{9C81D502-DF00-4759-A2D0-B4FBCED9D9B0}", "Key Inv" },
  { "{D096E91B-0572-418A-B0E8-3412114228C5}", "MIDI PB" },
  { "{F935E7CE-CC76-4548-B22F-4476A304BCEA}", "Env" },
  { "{33038A70-7256-46A8-AF33-6676157709C4}", "CVU" },
  { "{1209DAFC-6F0B-452E-B613-01097DB6249A}", "CVU" }, // Hold
  { "{C6F469EE-9A9A-4319-9825-ACDD75A14D6D}", "CVB" },
  { "{85D4AE4A-D83D-41AC-BB2B-81F7839126F5}", "CVB" }, // Hold
  { "{3498B234-D9A4-4A71-822B-C4304C3D2A63}", "V.LFO" },
  { "{4D157622-02FA-4D9C-9B47-B999A7566D05}", "G.LFO" },
  { "{7262E745-9116-4071-ACA7-0D11464E1892}", "G.LFO" } }; // Hold

// target routing
static list_item const vgcv_route_amp_targets[vgcv_route_amp_target::count] = { 
  { "{4FD8B71A-3ACF-4D82-9070-4C1B5843C8A7}", "Gain" },
  { "{A3956376-EE40-45F1-BDA5-FE7A41F9BD60}", "Bal" } };
static list_item const vcv_route_audio_bank_targets[vgcv_route_audio_bank_target::count] = {
  { "{327E458F-4686-44BA-9F32-68C58C53EBFB}", "Amt1" },
  { "{B330BC3A-03AE-439E-A405-DE81D394F6CD}", "Bal1" },
  { "{0925FBB7-3F1B-428C-8A00-F22D359CDFE3}", "Amt2" },
  { "{29F02153-556B-4E0F-A315-5755A581A645}", "Bal2" },
  { "{0A9F9198-192B-466F-911E-E3A83925670E}", "Amt3" },
  { "{3D61A681-C22E-4F2C-9B02-4DBFE6F9E725}", "Bal3" },
  { "{A14EFB11-3FEF-4F0A-9398-E128E4C7F28D}", "Amt4" },
  { "{B27F9FD2-D838-44BB-8545-BA0BDD1B49D1}", "Bal4" },
  { "{BEF4E7A6-F689-4B42-AE39-949FB4CF7ACF}", "Amt5" },
  { "{B71BDA09-29E3-4B32-ACCE-AE7A88730F63}", "Bal5" },
  { "{BA00FCB5-3F1F-4588-9009-A0C1365CED72}", "Amt6" },
  { "{D6F1FFAF-BC53-45D0-89CE-AA1E25638BDA}", "Bal6" },
  { "{1B563DC8-558B-40AA-995C-1FA75A5AE3D0}", "Amt7" },
  { "{BE7A805A-87E8-4CBE-AB12-C894D3E50B69}", "Bal7" },
  { "{402FF0EB-D706-4AC8-ADA0-35D5B020189B}", "Amt8" },
  { "{4294FD8E-0E41-4EDD-B1D7-05D235DF6B58}", "Bal8" },
  { "{9E7D50F9-CCA4-4874-9C22-FF72ADC08592}", "Amt9" },
  { "{9094D14D-C741-45F0-AACD-1ED88F6FC44C}", "Bal9" },
  { "{5423F616-A0AA-47C3-BE88-42C04D42D7B0}", "Amt10" },
  { "{A965C0E3-A062-4AF5-B568-BA6BDA1C6908}", "Bal10" },
  { "{7B8259F2-A341-4988-B531-D646DC87F37B}", "Amt11" },
  { "{9C8D05E6-2F9F-43FF-8A4A-221494995506}", "Bal11" },
  { "{229B0A27-9A68-43EF-A538-12CFC75ABA99}", "Amt12" },
  { "{402A3B9B-F22D-4F2A-B094-9BDEC25D21F8}", "Bal12" },
  { "{C1BE56AD-6412-468C-BCFE-01C611474130}", "Amt13" },
  { "{841B348F-DD39-430D-9173-54411CE49413}", "Bal13" },
  { "{0B1DBF2D-7388-4F82-8D7F-E2AA33239618}", "Amt14" },
  { "{2931C673-222D-4E83-BCCB-4A3CE9B499B0}", "Bal14" },
  { "{315264AE-8EB9-4CBB-9441-EE849AE0F06C}", "Amt15" },
  { "{35BB478F-578E-411B-A762-178D37F0C5D5}", "Bal15" } };
static list_item const gcv_route_audio_bank_targets[vgcv_route_audio_bank_target::count] = {
  { "{AF20EA99-6827-4216-8CBA-5562F267F649}", "Amt1" },
  { "{5B761DB0-688C-4396-96C5-2EE97A5513D0}", "Bal1" },
  { "{44183897-5277-4A02-BED5-8D727E6C9C33}", "Amt2" },
  { "{95126B9B-953F-4C21-870A-44C2D9396832}", "Bal2" },
  { "{2FA260E6-6384-4FE6-9D90-46AC3A4FCF28}", "Amt3" },
  { "{CDB20D33-4427-4C70-BAFC-6B4A0A472757}", "Bal3" },
  { "{01B5D903-C453-4CB4-AEC2-39F88CC1A46B}", "Amt4" },
  { "{8308F1EE-2CB1-4F70-9644-118DC9798161}", "Bal4" },
  { "{04A4A4AF-B47A-4D35-B8DA-9500DE85506D}", "Amt5" },
  { "{9B7D5409-979C-40B3-8437-B012146C8C8D}", "Bal5" },
  { "{E77ADEBB-87A4-4329-9D9F-869E5AD0489F}", "Amt6" },
  { "{DFB56B42-17E3-4855-A6B4-7C3CC1F224D7}", "Bal6" },
  { "{A718629E-5B7D-4D5C-885C-459B3D801B3F}", "Amt7" },
  { "{BA035F8F-24A1-43C2-9EFF-0268A44655A0}", "Bal7" },
  { "{CDD16DDA-8E6F-4753-ACCE-3EA9475C8461}", "Amt8" },
  { "{C520E582-6BD1-434F-942E-92AAC58BDB0F}", "Bal8" },
  { "{73E97D9F-7FC2-4F81-8C58-C98CBDEEDAB4}", "Amt9" },
  { "{7FE08016-5752-49DE-B143-668F4E0AC446}", "Bal9" },
  { "{4831B4C8-E30B-428D-95F5-7890B0F0F1E1}", "Amt10" },
  { "{0FED3F46-A261-47D1-807D-DAE5DBE0115F}", "Bal10" },
  { "{63535BBC-6B00-48FF-9549-77C505A7E694}", "Amt11" },
  { "{73DFADC9-EE86-4644-94DF-3081E865E1EA}", "Bal11" },
  { "{C6572723-AFF4-436F-8C7B-8C90D0D0B36F}", "Amt12" },
  { "{99A2B515-4488-4D91-A5A0-235B56F157C3}", "Bal12" },
  { "{AF1D40FE-E307-42C3-B89A-3F9DE0B290C7}", "Amt13" },
  { "{B6195896-F455-4F79-B849-DF5CAD974D34}", "Bal13" },
  { "{EF9A3831-0CAC-4F35-8F06-7B707CBD07EC}", "Amt14" },
  { "{FE1F7209-9794-497C-9286-E68141225D44}", "Bal14" },
  { "{644F1C21-473A-4E35-9667-83AE48D1556C}", "Amt15" },
  { "{7CFE27C5-4CD5-4F25-93BD-17692639C007}", "Bal15" } };
static list_item const vcv_route_vosc_targets[vcv_route_vosc_target::count] = {
  { "{8F07B23B-8F7E-4388-BBA7-968A30957F40}", "PM" },
  { "{987BE249-E672-4055-BFEA-BCDA828DB269}", "FM" },
  { "{7580316C-1C46-46B3-A230-E3F0BAA3634C}", "Cent" },
  { "{C3F69DA2-C7DA-4624-B870-70894044503B}", "Pitch" },
  { "{495D49A2-97F4-4B4D-A6B1-0BE194B09B68}", "Uni Dtn" },
  { "{0348A9C4-0414-418E-AC83-965C6F58487D}", "Uni Sprd" },
  { "{DBCDDC0D-8CA2-4DC1-A06C-A93AC8AF828A}", "AM Ring" },
  { "{D1FB8DF5-9E17-493C-B564-324045FE18C1}", "AM Mix" },
  { "{C0D18C5A-2BA6-4F1D-A1F0-C96A06F428E3}", "Basic PW" },
  { "{6486FC45-83A2-4EAD-8782-84B6C31517C2}", "Mix Sine" },
  { "{EF423264-369A-417E-83A6-1CAEBEEC2A54}", "Mix Saw" },
  { "{EDDD2B9D-A472-4D58-A5B8-83984FE32F89}", "Mix Tri" },
  { "{D7D85D30-03A9-41AB-BB33-D7778491DB5A}", "Mix Pulse" },
  { "{54952EC5-1840-46D5-A73A-6E1BC0D09418}", "Mix PW" },
  { "{EF63EC0F-4F41-455A-9E89-B90A1F2365FA}", "DSF Dist" },
  { "{0B9DCACF-DD2F-4E05-8427-BE30E7914485}", "DSF Dcy" },
  { "{986660DA-E6EC-4B36-8AE0-D469B1BE0343}", "K+S Fltr" },
  { "{58C3BC8F-8C44-4C88-BECA-4ADD765AF13A}", "K+S Fdbk" },
  { "{F4CC7E11-407F-4C05-834E-6DD0CDAE774B}", "K+S Str" },
  { "{8EBA9BAA-529F-445C-BEA1-D9CC17BC42A2}", "Noise Clr" },
  { "{A3DF88E5-448D-47E1-B33E-AA0B932B4B38}", "Noise X" },
  { "{4C8CD2BD-54D3-466B-B733-68AE424724EF}", "Noise Y" } };
static list_item const vgcv_route_effect_targets[vgcv_route_effect_target::count] = {
  { "{682871AF-24F5-45FA-92E9-CDC0758CEFCF}", "StVar Frq" },
  { "{A86529CD-46D7-412B-BCEE-C1C48D8A86A5}", "StVar Res" },
  { "{2357A2F9-0189-4F01-9CA8-831FC5066A04}", "StVar Kbd" },
  { "{B4370576-DE84-413E-96E2-3573C6C3709E}", "StVar Shlf" },
  { "{1E877DD6-1CDE-4181-8301-7D3617225FC2}", "Cmb Dly+" },
  { "{2BC57564-B1DB-4CEA-8F88-A6621DC85AFE}", "Cmb Gain+" },
  { "{FF76F9FD-8DBD-4175-9C87-08C9A160A807}", "Cmb Dly-" },
  { "{CA428CA8-B31D-4E49-A99C-95CB3D35F1DD}", "Cmb Gain-" },
  { "{EC7157FC-13B0-4809-BEED-9ECD0123C0E1}", "Shp Mix" },
  { "{4475764D-B720-4A22-92F3-0BCFA511FB8B}", "Shp Gain" },
  { "{D9C994BE-7556-466D-AC84-F9BFA16C4E53}", "Shp Decay" },
  { "{84333BE8-E252-4436-A1DD-77A389CB52D5}", "Delay Mix" },
  { "{FDF44D09-9E79-494B-8F79-7D120DDA3AEC}", "Delay Amt" },
  { "{5638F581-86B7-4F70-9D80-4BB21EAAAE6E}", "FbDly Sprd" },
  { "{3B1A6BFA-198E-4B35-8D3A-05F938E439A9}", "MtDly Sprd" },
  { "{87D0C08F-FC9F-4751-B00C-1A6FADF8F73C}", "Reverb Mix" },
  { "{93352B10-5409-48DF-AAB2-E199CB5B7174}", "Reverb Size" },
  { "{AA53FF25-0415-4890-A061-0B9563AC4C82}", "Reverb Sprd" },
  { "{BCD6370B-D963-449F-B091-EE36C9893864}", "Reverb Damp" },
  { "{FD8D3763-B1C0-4F86-8985-8483D523815B}", "Reverb APF" } };

// output targets  
static list_item const* const gcv_route_output_targets[gcv_route_output::count] = {
  nullptr, vgcv_route_effect_targets, gcv_route_audio_bank_targets, vgcv_route_amp_targets };
static list_item const* const vcv_route_output_targets[vcv_route_output::count] = {
  nullptr, vcv_route_vosc_targets, vgcv_route_effect_targets, vcv_route_audio_bank_targets, vgcv_route_amp_targets };
static std::vector<list_item> const vcv_route_output_target_list = zip_list_items( 
  vcv_route_outputs, vcv_route_output_counts, vcv_route_output_targets, vcv_route_output_target_counts, vcv_route_output::count);
static std::vector<list_item> const gcv_route_output_target_list = zip_list_items(
  gcv_route_outputs, gcv_route_output_counts, gcv_route_output_targets, gcv_route_output_target_counts, gcv_route_output::count);

// input sources
static char const* const vcv_route_input_suffixes[vcv_route_input::count] = {
  nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, "Hold", nullptr, "Hold", nullptr, nullptr, "Hold" };
static std::vector<list_item> const vcv_route_input_sources_list = multi_list_items(
  vcv_route_inputs, vcv_route_input_suffixes, vcv_route_input_counts, vcv_route_input::count, true);
static std::vector<list_item> const gcv_route_input_sources_list = multi_list_items(
  gcv_route_inputs, nullptr, gcv_route_input_counts, gcv_route_input::count, false);

param_descriptor const
vcv_plot_params[cv_plot_param::count] =
{
  { "{CE1DC1C7-72C5-4811-8C35-8485FFAFFABC}", { {"Target", "Target"}, "", param_kind::ui, param_type::list, {&vcv_route_output_target_list, "V.Out Gain" } } },
  { "{41FB9033-220C-4DA7-836A-22808E17167F}", { { "Hold", "Key hold" }, "Sec", param_kind::ui, quad_bounds(0.01f, 70.0f, "0.5", 2) } }
};

param_descriptor const
gcv_plot_params[cv_plot_param::count] =
{
  { "{EBF618A3-D50D-439C-A2BA-95C06277276E}", { {"Target", "Target"}, "", param_kind::ui, param_type::list, {&gcv_route_output_target_list, "M.Out Gain" } } },
  { "{60ED3115-0BD0-4870-9938-6D37759FE7D0}", { { "Length", "Length" }, "Sec", param_kind::ui, quad_bounds(0.01f, 70.0f, "2", 2) } }
};

// voice params
param_descriptor const   
vcv_bank_params[cv_bank_param::count] =            
{        
  { "{3B025C6A-0230-491A-A51F-7CF1C81B69C9}", { { "In", "Input 1" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 } } },
  { "{5FDD8C86-8F2D-4613-BB98-BB673F502412}", { { "Out", "Output 1" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 } } },
  { "{18EBC834-CF60-4A68-BAF2-C57BC0BAE55E}", { { "Op", "Op 1" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{469D130F-2E4A-4960-871D-032B6F588313}", { { "Amt", "Amount 1" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{0C1E5C81-01EE-4AE6-A05C-199210B904CC}", { { "Off", "Offset 1" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{20ACF437-2158-4900-8DC4-D36767442BF1}", { { "Scl", "Scale 1" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{2833E378-210B-404F-A4CB-0D6204A72CF0}", { { "In", "Input 2" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 } } },
  { "{D0B28D9E-8888-42EB-8D3C-177FB4585E42}", { { "Out", "Output 2" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 } } },
  { "{CD10C60A-C25E-46A5-879E-C692E50AE36B}", { { "Op", "Op 2" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{58AFE21F-7945-4919-BB67-60CE8892A8AF}", { { "Amt", "Amount 2" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{20F28A5F-2956-412E-BD97-9220836C22A3}", { { "Off", "Offset 2" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{23EFCBDF-7744-4527-B82D-A69E56C7338B}", { { "Scl", "Scale 2" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{25041AB5-2A06-4305-8009-C26D56311D26}", { { "In", "Input 3" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 } } },
  { "{37420523-6A9D-4125-BAAB-24A28B9E0992}", { { "Out", "Output 3" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 } } },
  { "{C6D7FDD5-102A-4972-B0B2-77F3977C9046}", { { "Op", "Op 3" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{9C1F6472-6D48-42E6-B79E-3A00F33F70F5}", { { "Amt", "Amount 3" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{BFFA2360-5A5F-4575-A0E4-32B8B74977BD}", { { "Off", "Offset 3" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{87087F9C-62A2-4804-9292-CD9089F025B7}", { { "Scl", "Scale 3" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{B4C3E2E0-106D-4377-93F6-711561D5F1DA}", { { "In", "Input 4" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 } } },
  { "{32C5980E-F41A-441C-85A8-3AC90A4AAD0A}", { { "Out", "Output 4" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 } } },
  { "{927B6CF5-D576-42F9-80BA-4C83437F9041}", { { "Op", "Op 4" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{D04F0B81-2E96-42D7-895C-DFC47BA36B4C}", { { "Amt", "Amount 4" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{B46AC7BD-982C-4A9D-B74B-AE27C25BE811}", { { "Off", "Offset 4" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{D58E30EB-8F46-4EB7-84F2-37AA48F81721}", { { "Scl", "Scale 4" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{DE53604C-CC85-40EA-B0AC-6CB00050EB4C}", { { "In", "Input 5" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 } } },
  { "{055B8D75-97ED-47E1-9A81-4FA5A3072E30}", { { "Out", "Output 5" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 } } },
  { "{EDA84031-7061-448F-BCEB-6D941D772EFF}", { { "Op", "Op 5" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{C0E8DD3E-DCAD-48FA-9C4E-BB0FDDFC2554}", { { "Amt", "Amount 5" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{542280B8-942D-4FAA-AB26-5A39CE3FFF71}", { { "Off", "Offset 5" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{62A7448C-3245-4B33-AF4E-42D98D3AD547}", { { "Scl", "Scale 5" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{26B5C58F-4AA6-4759-A098-44D8321949F2}", { { "In", "Input 6" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 } } },
  { "{7BF980EB-C1DE-4A0E-B252-67495F0DF902}", { { "Out", "Output 6" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 } } },
  { "{3F9F43B9-057D-48D6-A6E6-5C81956FFFE0}", { { "Op", "Op 6" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{221B5FF0-17D2-43B2-870A-D707BF7D22E4}", { { "Amt", "Amount 6" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{89A38FAF-0A6F-4024-90A7-B22F972FC586}", { { "Off", "Offset 6" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{B4EBE087-9413-44BD-989B-033C3AFA9DE4}", { { "Scl", "Scale 6" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{93710629-0791-44EC-A4F3-CF02ADC16A8C}", { { "In", "Input 7" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 } } },
  { "{AC12D884-CC26-47FA-9EA7-3D95FB179F86}", { { "Out", "Output 7" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 } } },
  { "{021EABC5-802D-4FF8-822C-571D1B800E25}", { { "Op", "Op 7" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{3029E072-D6DC-4CDA-93D9-54B21B1D83C7}", { { "Amt", "Amount 7" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{D20276B8-CA72-48C1-8268-9B5EEC937519}", { { "Off", "Offset 7" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{33E2DA5D-9386-48FA-88E1-391A93A55F2E}", { { "Scl", "Scale 7" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{D06D06B8-1350-4632-A730-1B7D941DAF64}", { { "In", "Input 8" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 } } },
  { "{B5AC1646-25F9-4910-A293-416283EF6A4B}", { { "Out", "Output 8" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 } } },
  { "{A103F564-3365-45FF-A785-B24B776F4882}", { { "Op", "Op 8" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{B5B869E1-ADCA-4043-8791-B982E64FFD9A}", { { "Amt", "Amount 8" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{971441FE-D505-4654-A011-65B63456A7F6}", { { "Off", "Offset 8" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{936A217D-B9DF-48A2-B28D-BEB92B78D37D}", { { "Scl", "Scale 8" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{D2545C43-A073-42F5-BED8-DA4B52D47E3A}", { { "In", "Input 9" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 } } },
  { "{7F688151-56FC-42A5-BDD0-4EFC6AFF515E}", { { "Out", "Output 9" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 } } },
  { "{8E6F70C8-E410-47E5-A557-E33C3AC13DBD}", { { "Op", "Op 9" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{1D0A9F37-41CE-467B-BBA0-9728541D48BB}", { { "Amt", "Amount 9" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{828FA1DB-1F68-4B2F-9FD6-270B88D6ED97}", { { "Off", "Offset 9" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{9F16C13A-9FBE-409C-8AEB-6CFB6131F236}", { { "Scl", "Scale 9" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{115B6998-FAE6-4918-A2AC-CB6E2DA4B9E8}", { { "In", "Input 10" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 } } },
  { "{59B10A24-F942-4060-8121-88BA4CB6AE88}", { { "Out", "Output 10" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 } } },
  { "{DCDFF901-F63A-4843-A93E-DB2AFFF52C8A}", { { "Op", "Op 10" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{D0D169F3-0BDA-4A29-AF52-AAE44A828BF9}", { { "Amt", "Amount 10" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{D8547C44-04BA-4EF8-8946-E2F452909B42}", { { "Off", "Offset 10" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{17909599-8277-4A6E-8477-CF51FFAEA592}", { { "Scl", "Scale 10" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{518CFC42-103F-41A7-8C6F-4A59AF722592}", { { "In", "Input 11" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 } } },
  { "{4159BE96-7F56-47CD-9CA0-C9552E49C0DF}", { { "Out", "Output 11" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 } } },
  { "{F2752021-DC00-4D22-8450-90462E03DEBE}", { { "Op", "Op 11" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{8837CF44-DA48-43C2-8142-3143C5267D15}", { { "Amt", "Amount 11" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{1B9DE7C9-64AB-4973-9158-E4CE1F87CDC2}", { { "Off", "Offset 11" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{7AC765D5-586E-4047-9FF8-6449DA2B6A1D}", { { "Scl", "Scale 11" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{45943429-6C46-46D2-8195-2F4010707D74}", { { "In", "Input 12" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 } } },
  { "{F9560E57-F781-43E6-8B25-706C40377BE9}", { { "Out", "Output 12" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 } } },
  { "{D72CD661-505E-4417-9FE4-27E9CF8E2313}", { { "Op", "Op 12" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{A070D81C-F6BF-479B-9AE4-B9D941AA400D}", { { "Amt", "Amount 12" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{061B028C-F9A4-4963-9811-ED2902D7A0FF}", { { "Off", "Offset 12" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{8D9114F6-AFD4-4292-9F8A-4EA907261B24}", { { "Scl", "Scale 12" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{335EC864-CC46-452B-9764-54737B1A1EE2}", { { "In", "Input 13" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 } } },
  { "{BAB52907-B1CB-43AC-9C0D-C16B7F16FF3B}", { { "Out", "Output 13" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 } } },
  { "{C8A97D44-0CEC-4CF9-8B49-057A14278FF0}", { { "Op", "Op 13" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{2151FF1B-8B1B-4257-9D86-B6E719636743}", { { "Amt", "Amount 13" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{A36732E8-A7DD-46A2-B4E0-62AE68695994}", { { "Off", "Offset 13" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{1B49E9BD-56B4-453F-8109-43309CCF4567}", { { "Scl", "Scale 13" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{67B18BB3-7524-4C80-B6C8-BA8CEAF10B02}", { { "In", "Input 14" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 } } },
  { "{8517DB1A-2856-4526-88F8-7AAE066B7293}", { { "Out", "Output 14" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 } } },
  { "{2D2599C4-3868-4CA2-B112-F57B48F948C7}", { { "Op", "Op 14" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{2AAA5DB5-C5E9-4161-A32E-CCC2A51E921F}", { { "Amt", "Amount 14" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{C81D2513-0269-485D-A261-B640A615730D}", { { "Off", "Offset 14" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{392DB89B-29E3-4583-AC98-17A15CAEFC81}", { { "Scl", "Scale 14" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{91BF7B4F-64B5-441E-851B-7E30A455583F}", { { "In", "Input 15" }, "", param_kind::voice, param_type::list, { &vcv_route_input_sources_list, 0 } } },
  { "{B9A9EB2A-3425-4CC1-9B19-1D33B3F1A682}", { { "Out", "Output 15" }, "", param_kind::voice, param_type::list, { &vcv_route_output_target_list, 0 } } },
  { "{D8D58E18-0783-457B-A34F-938872D0EC44}", { { "Op", "Op 15" }, "", param_kind::voice, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{B9129F8C-EF20-40C3-A9FA-9DA6E4C40B26}", { { "Amt", "Amount 15" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{D040FA2E-9778-4C78-983B-D2FDC6172F61}", { { "Off", "Offset 15" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{81EB30B8-8BA3-4449-B375-62F22EC7C835}", { { "Scl", "Scale 15" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } }
};

// global params
param_descriptor const 
gcv_bank_params[cv_bank_param::count] = 
{ 
  { "{84736ED7-FF72-4E69-AFFF-A8607B0F3041}", { { "In", "Input 1" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 } } },
  { "{4548DE68-1D70-4307-BBD2-09838CAC4701}", { { "Out", "Output 1" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 } } },
  { "{A526F21F-4C41-4925-90FD-D6BA442225E7}", { { "Op", "Op 1" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{A927E108-FB33-4AFC-A46A-726705004F78}", { { "Amt", "Amount 1" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{41F96374-D208-4A29-911E-18CF7C27A6A0}", { { "Off", "Offset 1" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{5ACBDB05-BFCF-4A85-809B-D81DB8E835DE}", { { "Scl", "Scale 1" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{36981D81-6FC1-42C1-A380-D0813C624D93}", { { "In", "Input 2" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 } } },
  { "{A5857037-CB45-4F83-AF2F-2C4083628E63}", { { "Out", "Output 2" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 } } },
  { "{FDFF29BB-E361-4350-9958-8266013A3124}", { { "Op", "Op 2" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{98AF1BBD-D01A-41EF-82DC-FD852FD7154B}", { { "Amt", "Amount 2" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{4CBAC51D-9597-48BF-9210-D9CA33E8DCD1}", { { "Off", "Offset 2" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{2E03B188-2616-457B-ACC2-F1B735370420}", { { "Scl", "Scale 2" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{9517E1DC-8069-4F00-915C-A0686DD3FB26}", { { "In", "Input 3" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 } } },
  { "{DC95DEF0-DD36-493A-B97A-E580DEB8BEBD}", { { "Out", "Output 3" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 } } },
  { "{53EE242B-B967-499B-821B-4BF5419E08E5}", { { "Op", "Op 3" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{48F63E35-4D42-4EC1-AC9A-8E0CF2278095}", { { "Amt", "Amount 3" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{D2EF64CE-1737-4776-AD8D-F28D5E359960}", { { "Off", "Offset 3" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{45AE23A8-8EC6-4901-BECE-CB631FDFAA4D}", { { "Scl", "Scale 3" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{EB0A819C-174B-41B8-B8E8-E91377441D66}", { { "In", "Input 4" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 } } },
  { "{86D57462-7C08-4DD6-8FAC-EEC7BD0ABE34}", { { "Out", "Output 4" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 } } },
  { "{CD0E58CA-C183-4B59-9F45-2F0805379A1D}", { { "Op", "Op 4" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{2EC4D924-1912-4111-8AC1-B24D84384618}", { { "Amt", "Amount 4" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{44061640-FDE6-4E81-BDE0-6145A6EBD494}", { { "Off", "Offset 4" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{D2937D43-3DEE-4657-A9E0-BD2B1323FA3A}", { { "Scl", "Scale 4" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{460C97B9-5DA2-40D9-B553-B59784A972B7}", { { "In", "Input 5" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 } } },
  { "{CF0921FB-E525-4217-A249-CBEDFDB58B72}", { { "Out", "Output 5" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 } } },
  { "{2D718791-CF63-4A17-A1E3-EBB1DD675F38}", { { "Op", "Op 5" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{E8B33588-CF41-44B4-A2CB-A12E520A8A84}", { { "Amt", "Amount 5" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{31E160CE-5424-42B6-858E-07FC00C5B502}", { { "Off", "Offset 5" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{9B15F803-4776-459D-98D8-B5563F217159}", { { "Scl", "Scale 5" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{C26316CE-3855-44C4-8C0D-0AD3A06443CD}", { { "In", "Input 6" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 } } },
  { "{326AEE47-2AFC-4140-8AD3-A8B1AE1FD1DF}", { { "Out", "Output 6" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 } } },
  { "{9D3027D8-5146-45A6-BAF7-036DBF3D2345}", { { "Op", "Op 6" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{9806BBDC-23A5-4038-B4D0-1B541238E915}", { { "Amt", "Amount 6" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{72E153D2-4614-4F91-B348-DF7AE0453927}", { { "Off", "Offset 6" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{87F14A8F-2462-4CAE-9E8F-A87F95A20369}", { { "Scl", "Scale 6" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{5D4EB303-0B5C-48A3-BC62-79571DD51387}", { { "In", "Input 7" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 } } },
  { "{C7A5A80A-5B69-4F42-BD92-57A0FA606DFE}", { { "Out", "Output 7" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 } } },
  { "{DF3A60E1-42E1-4E13-AA33-F8C56C877A92}", { { "Op", "Op 7" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{7BC87BF9-DBFC-4092-A03F-E93292DA7963}", { { "Amt", "Amount 7" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{31D9A605-F16B-47AB-B5F4-C03B285D7EDA}", { { "Off", "Offset 7" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{DF70143D-441E-4C77-BCF5-6535B4316B64}", { { "Scl", "Scale 7" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{FA7C27DD-3EFB-48A1-96FD-89BD7F195C62}", { { "In", "Input 8" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 } } },
  { "{02FE5916-CD9C-4AB9-8D47-25198095090F}", { { "Out", "Output 8" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 } } },
  { "{1E668318-1135-4325-A10A-7EC6F0A73D24}", { { "Op", "Op 8" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{B59683B5-2031-46CC-8C95-B5B291320601}", { { "Amt", "Amount 8" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{9A949AC3-CFF8-49CC-81BC-78B59B2E4F8A}", { { "Off", "Offset 8" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{12EE4A5C-1BA4-4DC5-80B7-0FEF76EA5A59}", { { "Scl", "Scale 8" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{DB3A03C8-C163-4AE7-9D8F-C6BF208ED51E}", { { "In", "Input 9" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 } } },
  { "{B655DE93-2B12-43C9-96C1-57528EF75AD9}", { { "Out", "Output 9" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 } } },
  { "{D2039FFC-CE6D-4C4E-ACB0-D621F79CD552}", { { "Op", "Op 9" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{A200EEA9-702E-4A77-9533-9C9CC569F93C}", { { "Amt", "Amount 9" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{D0703188-6AAC-41B7-9099-6FACA021891A}", { { "Off", "Offset 9" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{CF790023-1BF7-4543-8C2B-3AA6C42413BF}", { { "Scl", "Scale 9" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{A1A1A746-3928-41D0-BC29-B3E7FF8F3920}", { { "In", "Input 10" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 } } },
  { "{55B9CBED-C488-442A-9061-AB4B1E6A98C5}", { { "Out", "Output 10" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 } } },
  { "{22A24A1C-D263-46AF-977D-ABA8167E1127}", { { "Op", "Op 10" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{7AAB027A-B56D-449F-A0DB-6037127F8104}", { { "Amt", "Amount 10" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{9B4C996F-CD24-472F-967D-CEE4358313E4}", { { "Off", "Offset 10" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{3C87E9C6-4493-457D-BA21-CD9D5AC2BD5F}", { { "Scl", "Scale 10" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{7E9E6396-6634-4D16-AF35-CD7504AEFCB5}", { { "In", "Input 11" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 } } },
  { "{0F4DC602-D207-4045-B6F0-D19324A98A1D}", { { "Out", "Output 11" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 } } },
  { "{78379D9C-C25D-4898-B9D4-22D6AA1BF9B9}", { { "Op", "Op 11" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{8564DD7A-B829-40B5-A986-A37CC7061C5A}", { { "Amt", "Amount 11" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{97E76AD5-697E-4039-BEEB-76D09A047E2E}", { { "Off", "Offset 11" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{F7BB8A0B-639F-48F1-807E-FC02EBA7DA68}", { { "Scl", "Scale 11" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{79BADA83-9984-4E8B-9E65-12FBE2F5DC4E}", { { "In", "Input 12" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 } } },
  { "{A9310EC6-3325-4FD2-9AA7-B8A3587D1585}", { { "Out", "Output 12" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 } } },
  { "{6D531B57-6225-4EE8-9657-49D441DE5117}", { { "Op", "Op 12" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{C8B2C165-FB14-49EB-9D14-516C2046C277}", { { "Amt", "Amount 12" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{D283ACD3-8E0C-4844-93FD-610F364E37E2}", { { "Off", "Offset 12" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{3F912550-4701-4A46-8675-38A4D79FE34E}", { { "Scl", "Scale 12" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{BF864886-EB19-4E63-9156-D2D578D2AE7A}", { { "In", "Input 13" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 } } },
  { "{A5E5A257-AD93-4B7B-8F8D-571D05D2438E}", { { "Out", "Output 13" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 } } },
  { "{C3EAD0E2-88A8-4E46-AA4D-ABC3C3959E4D}", { { "Op", "Op 13" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{36DED77D-8A0F-428C-814E-3565DF4987E6}", { { "Amt", "Amount 13" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{B668FD17-EEFA-4535-99DA-D595084D93F0}", { { "Off", "Offset 13" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{57718EAF-D53E-431E-BFFB-E89F51171C3A}", { { "Scl", "Scale 13" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{66CDA636-B4B0-4B90-B9B7-EA61113E185C}", { { "In", "Input 14" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 } } },
  { "{7553B893-B170-468D-9444-ABA035ADE101}", { { "Out", "Output 14" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 } } },
  { "{1314C553-B9B8-43E5-B587-9C1491E810DF}", { { "Op", "Op 14" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{F281733C-6314-4263-9CAE-4ECE7CE42075}", { { "Amt", "Amount 14" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{57AD39C6-C010-4857-A7AC-A6C8D5D909EE}", { { "Off", "Offset 14" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{F11FDFEE-10B0-4560-AA09-BD390FDFC724}", { { "Scl", "Scale 14" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{18C3211A-4503-45A5-8A90-C70AC3EB34FC}", { { "In", "Input 15" }, "", param_kind::block, param_type::list, { &gcv_route_input_sources_list, 0 } } },
  { "{AFAFF4DE-F954-4B3F-A714-58B0AECF7D05}", { { "Out", "Output 15" }, "", param_kind::block, param_type::list, { &gcv_route_output_target_list, 0 } } },
  { "{2A133861-BCFA-4DEE-A29B-5F28F75211A6}", { { "Op", "Op 15" }, "", param_kind::block, param_type::list, { &cv_route_input_ops, 0 } } },
  { "{84AF1052-D85C-4A76-B02D-6A3A3A12D48C}", { { "Amt", "Amount 15" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{820A36E3-3099-4060-96AC-BAFE3A139693}", { { "Off", "Offset 15" }, "%", param_kind::continuous, percentage_01_bounds(0.0f) } },
  { "{62B03471-CF37-4D16-A6C5-50057CE0E206}", { { "Scl", "Scale 15" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } }
};
 
} // namespace inf::synth