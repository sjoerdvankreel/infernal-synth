#include <inf.synth/effect/config.hpp>
#include <inf.synth/effect/topology.hpp>

#include <inf.base/shared/support.hpp>
#include <inf.base/topology/part_descriptor.hpp>
#include <inf.base/topology/param_ui_descriptor.hpp>

using namespace inf::base;

namespace inf::synth {
 
static std::vector<list_item> const effect_delay_types = {
  { "{B130A459-A336-4AE6-93F5-AE8CF8D09F0E}", "Feedback" },
  { "{40083B26-3878-400F-991B-97254FDE8107}", "Multitap" } };
static std::vector<list_item> const effect_filter_types = {
  { "{4D124C3B-DC17-4241-AF3B-F9590CEC837B}", "State Variable" },
  { "{DFF83170-5380-4CC0-B418-3BC2C5638D88}", "Comb" } };
static std::vector<list_item> const effect_types = {
  { "{A9F10C03-1757-4759-B127-82CD86B61ED7}", "Filter" },
  { "{E748B96C-B027-477D-A044-FD2ACBC10F6B}", "Shape" },
  { "{2579F57B-4E61-4727-8EC5-766F4711FA2D}", "Delay" },
  { "{26C05CA1-6B5C-4B78-B2CB-662B43EF72AC}", "Reverb" } };
static std::vector<list_item> const effect_shp_over_orders = {
  { "{5FC4A099-B447-420F-97A5-FEF5F8817829}", "None" },
  { "{A513F4DD-6D41-4B08-BC1F-8C1D02A72A48}", "2X" },
  { "{72E89613-58DE-4529-9DCD-7D95C57D977B}", "4X" },
  { "{D663C977-1DC3-4F7D-9CE5-D7CD209E43E0}", "8X" },
  { "{4E3D87FD-72B2-477A-8B50-CDA4EB576CA6}", "16X" } }; 
static std::vector<list_item> const effect_shaper_types = {
  { "{AA2D4655-102E-42E5-8F3A-B6E6F5B04BA6}", "Clip" },
  { "{99EF60E2-B366-4184-88B7-B4C9C5514A7D}", "Tanh" },
  { "{7B8F970B-3087-4097-8AF6-49260E57DBEC}", "Fold" },
  { "{254ECC12-1396-442F-93B8-40505171613E}", "Sine" },
  { "{60285A7E-D1D3-45DB-BD58-98BA17349627}", "Chebyshev 1" }, 
  { "{5749E885-1196-4E1B-B464-B95C8B0393B5}", "Chebyshev 2" } }; 
static std::vector<list_item> const effect_flt_stvar_types = {
  { "{DA174004-DDF9-4DA1-8414-AAC6C204690A}", "LPF" },
  { "{8F9774E2-990C-415A-8DB3-732D4091C7A6}", "HPF" },
  { "{D7A94A2D-F64E-4357-AE0E-60F21BAF46E3}", "BPF" },
  { "{90494596-B7C0-4FAA-87EB-3C2944A0EBF9}", "BSF" },
  { "{18F9843C-1420-4203-82DC-75BF345C5092}", "APF" },
  { "{F3F23041-EF9C-4EA4-A2DE-70C28D6EC81C}", "PEQ" },
  { "{C2233B2C-5ADB-4293-93B9-C81C5D0CFC4B}", "BLL" },
  { "{113998D9-764D-44C8-981F-E531C2CCD230}", "LSH" },
  { "{3E7BEBD7-CA16-40BD-8869-A80E4E112532}", "HSH" } };

// 2/1 = 4 seconds at 120 bpm
static std::vector<time_signature> const effect_dly_timesig = synced_timesig(false, { 2, 1 }, {
  { timesig_type::one_over, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 16 } },
  { timesig_type::lower, { 2, 3, 4, 5, 6, 7, 8, 9, 12, 16 } },
  { timesig_type::upper, { 2, 3, 4, 5, 6, 7, 8 } },
  { timesig_type::over_one, { 1, 2 } } }); 
std::vector<float> const effect_dly_timesig_values = 
synced_timesig_values(effect_dly_timesig);
static std::vector<std::string> const effect_dly_timesig_names =
synced_timesig_names(effect_dly_timesig);

static std::vector<list_item> const effect_global_types =
std::vector<list_item>(effect_types.begin(), effect_types.end());
static std::vector<list_item> const effect_voice_types =
std::vector<list_item>(effect_types.begin(), effect_types.begin() + effect_type::delay);
    
static param_ui_descriptor const effect_filter_type_ui = 
{ true, 0.0f, { { effect_param::type, [](std::int32_t v) { return v == effect_type::filter; } } } };
static param_ui_descriptor const effect_shaper_ui = 
{ false, 0.0f, { { effect_param::type, [](std::int32_t v) { return v == effect_type::shaper; } } } };
static param_ui_descriptor const effect_shaper_type_ui =
{ true, 0.0f, { { effect_param::type, [](std::int32_t v) { return v == effect_type::shaper; } } } };
static param_ui_descriptor const effect_delay_ui =
{ false, 0.0f, { { effect_param::type, [](std::int32_t v) { return v == effect_type::delay; } } } };
static param_ui_descriptor const effect_delay_type_ui =
{ true, 0.0f, { { effect_param::type, [](std::int32_t v) { return v == effect_type::delay; } } } };
static param_ui_descriptor const effect_reverb_ui =
{ false, 0.0f, { { effect_param::type, [](std::int32_t v) { return v == effect_type::reverb; } } } };

static param_ui_descriptor const effect_dly_multi_time_ui = { false, 0.0f, {
{ effect_param::type, [](std::int32_t v) { return v == effect_type::delay; } },
{ effect_param::delay_type, [](std::int32_t v) { return v == effect_delay_type::multi; } },
{ effect_param::dly_synced, [](std::int32_t v) { return v == 0; } } } };
static param_ui_descriptor const effect_dly_multi_tempo_ui = { false, 0.0f, {
{ effect_param::type, [](std::int32_t v) { return v == effect_type::delay; } },
{ effect_param::delay_type, [](std::int32_t v) { return v == effect_delay_type::multi; } },
{ effect_param::dly_synced, [](std::int32_t v) { return v == 1; } } } };
static param_ui_descriptor const effect_dly_fdbk_time_ui = { false, 0.0f, {
{ effect_param::type, [](std::int32_t v) { return v == effect_type::delay; } },
{ effect_param::delay_type, [](std::int32_t v) { return v == effect_delay_type::feedback; } },
{ effect_param::dly_synced, [](std::int32_t v) { return v == 0; } } } };
static param_ui_descriptor const effect_dly_fdbk_tempo_ui = { false, 0.0f, {
{ effect_param::type, [](std::int32_t v) { return v == effect_type::delay; } },
{ effect_param::delay_type, [](std::int32_t v) { return v == effect_delay_type::feedback; } },
{ effect_param::dly_synced, [](std::int32_t v) { return v == 1; } } } };
static param_ui_descriptor const effect_dly_multi_ui = { false, 0.0f, {
{ effect_param::type, [](std::int32_t v) { return v == effect_type::delay; } },
{ effect_param::delay_type, [](std::int32_t v) { return v == effect_delay_type::multi; } } } };
static param_ui_descriptor const effect_dly_fdbk_ui = { false, 0.0f, {
{ effect_param::type, [](std::int32_t v) { return v == effect_type::delay; } },
{ effect_param::delay_type, [](std::int32_t v) { return v == effect_delay_type::feedback; } } } };

static param_ui_descriptor const effect_flt_comb_ui = { false, 0.0f, {
{ effect_param::type, [](std::int32_t v) { return v == effect_type::filter; } },
{ effect_param::filter_type, [](std::int32_t v) { return v == effect_filter_type::comb; } } } };
static param_ui_descriptor const effect_flt_stvar_ui = { false, 0.0f, {
{ effect_param::type, [](std::int32_t v) { return v == effect_type::filter; } },
{ effect_param::filter_type, [](std::int32_t v) { return v == effect_filter_type::state_var; } } } };
static param_ui_descriptor const effect_flt_stvar_shlf_ui = { false, 0.0f, {
{ effect_param::type, [](std::int32_t v) { return v == effect_type::filter; } },
{ effect_param::filter_type, [](std::int32_t v) { return v == effect_filter_type::state_var; } },
{ effect_param::flt_stvar_type, [](std::int32_t v) { return v >= effect_flt_stvar_type::bll; } } } };

static param_ui_descriptor const effect_shp_cheby_sum_ui = { false, 0.0f, {
{ effect_param::type, [](std::int32_t v) { return v == effect_type::shaper; } },
{ effect_param::shaper_type, [](std::int32_t v) { return v == effect_shaper_type::cheby_sum; } } } };
static param_ui_descriptor const effect_shp_cheby_ui = { false, 0.0f, {
{ effect_param::type, [](std::int32_t v) { return v == effect_type::shaper; } },
{ effect_param::shaper_type, [](std::int32_t v) { return v == effect_shaper_type::cheby_one || v == effect_shaper_type::cheby_sum; } } } };
 
static param_descriptor_data const effect_on_data = { { "On", "Enabled" }, param_kind::voice, false, -1, nullptr }; 
static param_descriptor_data const effect_voice_type_data = { { "Type", "Type" }, "", param_kind::voice, param_type::list, { &effect_voice_types, effect_type::filter }, 0, nullptr };
static param_descriptor_data const effect_global_type_data = { { "Type", "Type" }, "", param_kind::block, param_type::list, { &effect_global_types, effect_type::filter }, 0, nullptr };
static param_descriptor_data const effect_filter_type_data = { { "Flt", "Filter type" }, "", param_kind::voice, param_type::list, { &effect_filter_types, effect_filter_type::state_var }, 1, &effect_filter_type_ui };
static param_descriptor_data const effect_flt_stvar_type_data = { { "SV", "State variable filter type" }, "", param_kind::voice, param_type::list, { &effect_flt_stvar_types, effect_flt_stvar_type::lpf }, 2, &effect_flt_stvar_ui };
static param_descriptor_data const effect_flt_stvar_freq_data = { { "Freq", "State variable filter frequency" }, "Hz", param_kind::continuous, quad_bounds(effect_flt_stvar_min_freq, effect_flt_stvar_max_freq, "1000", 0), 4, &effect_flt_stvar_ui };
static param_descriptor_data const effect_flt_stvar_res_data = { { "Res", "State variable filter resonance" }, "%", param_kind::continuous, percentage_01_bounds(0.0f), 5, &effect_flt_stvar_ui };
static param_descriptor_data const effect_flt_stvar_kbd_data = { { "Kbd", "State variable filter keyboard tracking" }, "%", param_kind::continuous, percentage_m11_bounds(0.0f), 3, &effect_flt_stvar_ui };
static param_descriptor_data const effect_flt_stvar_shlf_gain_data = { { "Gain", "State variable filter shelf gain" }, "dB", param_kind::continuous, linear_bounds(-24.0f, 24.0f, 0.0f, 2), 6, &effect_flt_stvar_shlf_ui };
static param_descriptor_data const effect_flt_comb_dly_plus_data = { { "Dly+", "Comb filter delay+" }, "Ms", param_kind::continuous, millisecond_bounds(0.0f, effect_flt_comb_max_ms, 2.5f, 2), 2, &effect_flt_comb_ui };
static param_descriptor_data const effect_flt_comb_gain_plus_data = { { "Gain+", "Comb filter gain+" }, "%", param_kind::continuous, percentage_m11_bounds(0.5f), 3, &effect_flt_comb_ui };
static param_descriptor_data const effect_flt_comb_dly_min_data = { { "Dly-", "Comb filter delay-" }, "Ms", param_kind::continuous, millisecond_bounds(0.0f, effect_flt_comb_max_ms, 2.5f, 2), 4, &effect_flt_comb_ui };
static param_descriptor_data const effect_flt_comb_gain_min_data = { { "Gain-", "Comb filter gain-" }, "%", param_kind::continuous, percentage_m11_bounds(0.5f), 5, &effect_flt_comb_ui };
static param_descriptor_data const effect_shaper_type_data = { { "Shp", "Shaper type" }, "", param_kind::voice, param_type::list, { &effect_shaper_types, effect_shaper_type::tanh }, 1, &effect_shaper_type_ui };
static param_descriptor_data const effect_shp_over_data = { { "Over", "Shaper oversampling" }, "", param_kind::voice, param_type::list, { &effect_shp_over_orders, effect_shp_over_order::over_4 }, 2, &effect_shaper_ui };
static param_descriptor_data const effect_shp_mix_data = { { "Mix", "Shaper mix" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 3, &effect_shaper_ui };
static param_descriptor_data const effect_shp_gain_data = { { "Gain", "Shaper gain" }, "", param_kind::continuous, quad_bounds(effect_shp_min_gain, effect_shp_max_gain, "1", 1), 4, &effect_shaper_ui };
static param_descriptor_data const effect_shp_cheby_terms_data = { { "Terms", "Chebyshev shaper terms" }, "", param_kind::voice, param_type::list_knob, { effect_shp_cheby_min_terms, effect_shp_cheby_max_terms, effect_shp_cheby_min_terms + 1 }, 5, &effect_shp_cheby_ui };
static param_descriptor_data const effect_shp_cheby_sum_decay_data = { { "Decay", "Chebyshev shaper sum decay" }, "", param_kind::continuous, percentage_01_bounds(1.0f), 6, &effect_shp_cheby_sum_ui };
static param_descriptor_data const effect_delay_type_data = { { "Dly", "Delay type" }, "", param_kind::voice, param_type::list, { &effect_delay_types, effect_delay_type::feedback }, 1, &effect_delay_type_ui };
static param_descriptor_data const effect_dly_synced_data = { { "Sync", "Delay tempo sync" }, param_kind::voice, false, 3, &effect_delay_ui };  
static param_descriptor_data const effect_dly_mix_data = { { "Mix", "Delay mix" }, "%", param_kind::continuous, percentage_01_bounds(0.5f), 2, &effect_delay_ui }; 
static param_descriptor_data const effect_dly_amt_data = { { "Amt", "Delay amount" }, "%", param_kind::continuous, percentage_01_bounds(0.25f), 4, &effect_delay_ui };
static param_descriptor_data const effect_dly_fdbk_sprd_data = { { "Sprd", "Feedback delay stereo spread" }, "%", param_kind::continuous, percentage_01_bounds(0.5f), 5, &effect_dly_fdbk_ui };
static param_descriptor_data const effect_dly_multi_sprd_data = { { "Sprd", "Multitap delay stereo spread" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1), 5, &effect_dly_multi_ui };
static param_descriptor_data const effect_dly_fdbk_time_l_data = { { "L", "Feedback delay left time" }, "Sec", param_kind::voice, quad_bounds(effect_dly_min_time_sec, effect_dly_max_time_sec, "1", 3), 6, &effect_dly_fdbk_time_ui };
static param_descriptor_data const effect_dly_fdbk_time_r_data = { { "R", "Feedback delay right time" }, "Sec", param_kind::voice, quad_bounds(effect_dly_min_time_sec, effect_dly_max_time_sec, "1", 3), 7, &effect_dly_fdbk_time_ui };
static param_descriptor_data const effect_dly_fdbk_tempo_l_data = { { "L", "Feedback delay left tempo" }, "", param_kind::voice, param_type::knob_list, { &effect_dly_timesig_names, "1/4" }, 6, &effect_dly_fdbk_tempo_ui };
static param_descriptor_data const effect_dly_fdbk_tempo_r_data = { { "R", "Feedback delay right tempo" }, "", param_kind::voice, param_type::knob_list, { &effect_dly_timesig_names, "1/4" }, 7, &effect_dly_fdbk_tempo_ui };
static param_descriptor_data const effect_dly_multi_time_data = { { "Time", "Multitap delay time" }, "Sec", param_kind::voice, quad_bounds(effect_dly_min_time_sec, effect_dly_max_time_sec, "1", 3), 6, &effect_dly_multi_time_ui };
static param_descriptor_data const effect_dly_multi_tempo_data = { { "Tempo", "Multitap delay tempo" }, "", param_kind::voice, param_type::knob_list, { &effect_dly_timesig_names, "1/4" }, 6, &effect_dly_multi_tempo_ui };
static param_descriptor_data const effect_dly_multi_taps_data = { { "Taps", "Multitap delay tap count" }, "", param_kind::voice, param_type::list_knob, { 1, effect_dly_max_taps, effect_dly_max_taps / 2 }, 7, &effect_dly_multi_ui };
static param_descriptor_data const effect_reverb_mix_data = { { "Mix", "Reverb mix" }, "%", param_kind::continuous, percentage_01_bounds(0.5f), 1, &effect_reverb_ui };
static param_descriptor_data const effect_reverb_size_data = { { "Size", "Reverb room size" }, "%", param_kind::continuous, percentage_01_bounds(0.5f), 2, &effect_reverb_ui };
static param_descriptor_data const effect_reverb_spread_data = { { "Sprd", "Reverb stereo spread" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 3, &effect_reverb_ui };
static param_descriptor_data const effect_reverb_damp_data = { { "Damp", "Reverb damping" }, "%", param_kind::continuous, percentage_01_bounds(0.5f), 4, &effect_reverb_ui };
static param_descriptor_data const effect_reverb_apf_data = { { "APF", "Reverb APF" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 5, &effect_reverb_ui };
 
param_descriptor const  
veffect_params[effect_param::vfx_count] =     
{  
  { "{5813593C-E25A-4C8B-B7E5-7C95DAC257C2}", effect_on_data },
  { "{956F08A1-B3C6-412B-AEEA-55E722744AE8}", effect_voice_type_data }, 
  { "{668F7637-D4BA-4844-AF02-C8C028F9E7E0}", effect_filter_type_data },
  { "{7498AB5E-4BE6-45B0-8CBF-8D166FF82E32}", effect_flt_stvar_type_data },
  { "{99F7D808-53EB-4A4F-9FBF-4F1F57E4B742}", effect_flt_stvar_freq_data },
  { "{AA394A33-B616-4EAF-830D-313F2FE316AF}", effect_flt_stvar_res_data },
  { "{BF8775C6-FD22-4653-BC21-64CDC8D24AFF}", effect_flt_stvar_kbd_data },
  { "{510FF4B1-DB08-47D8-9260-CFD4D22C4977}", effect_flt_stvar_shlf_gain_data },
  { "{36AA381A-82F4-4259-9D87-431B8C49EAB4}", effect_flt_comb_dly_plus_data },
  { "{9B330083-CF9C-4161-9A66-74AF468EF521}", effect_flt_comb_gain_plus_data },
  { "{25A32954-6975-4591-87B0-F0718206E965}", effect_flt_comb_dly_min_data },
  { "{E67FACFD-E13D-4618-85B6-5FED1E3761F3}", effect_flt_comb_gain_min_data },
  { "{D8B7796F-9FB3-423B-91BB-11844306F69F}", effect_shaper_type_data },
  { "{C8BDD9B4-E7C4-4E8E-8C7A-4697DFE3A273}", effect_shp_over_data },  
  { "{D08721F8-A84F-41A8-A5F6-2FB3C8AB5D29}", effect_shp_mix_data },
  { "{564D7D79-615D-4537-ACF1-5035E8B34FD5}", effect_shp_gain_data },
  { "{85A0B1C4-A4E3-44B6-8992-97A73678286A}", effect_shp_cheby_terms_data },
  { "{D8895A29-190A-4936-8572-71E6E7DC0569}", effect_shp_cheby_sum_decay_data }
};

param_descriptor const
geffect_params[effect_param::gfx_count] =
{
  { "{F654747A-E462-4D53-A225-CD50A6E66C09}", effect_on_data },
  { "{03B06519-F6B6-4C4E-A925-0DE0BD75279A}", effect_global_type_data },
  { "{69D186F0-31E0-4F68-BA1C-38B2CCBA5369}", effect_filter_type_data },
  { "{7B96990C-0DEB-4B8E-A276-4A602E131692}", effect_flt_stvar_type_data },
  { "{4B966B02-A94F-4444-8EC2-C70F6A0D6DAF}", effect_flt_stvar_freq_data }, 
  { "{C7753965-69D5-4439-9110-B58C6CEA2E85}", effect_flt_stvar_res_data },
  { "{0F6B0464-F13A-40C0-9FAD-93B32A73BC8D}", effect_flt_stvar_kbd_data },
  { "{983866C8-C466-428F-B9B4-62C125AE8684}", effect_flt_stvar_shlf_gain_data },
  { "{088FDEB4-0791-4E19-AFB2-6F50C002730E}", effect_flt_comb_dly_plus_data },
  { "{DCE4689C-B10D-48CC-9BBE-F9F661203858}", effect_flt_comb_gain_plus_data },
  { "{BC1D7ACE-5E6C-4F37-9F45-8EA89E503B02}", effect_flt_comb_dly_min_data },
  { "{942571A7-611D-4587-8234-F35403B40993}", effect_flt_comb_gain_min_data },
  { "{9B6BC212-3ACA-4D84-95AD-A196C84EB434}", effect_shaper_type_data },
  { "{58335194-D03D-462D-B65F-E9C746E2C0A8}", effect_shp_over_data },
  { "{F1A5E8DF-7CA6-4E73-8FA7-B1F457D06205}", effect_shp_mix_data },
  { "{A56102DC-7FE3-4968-AB52-CFD878A950CA}", effect_shp_gain_data },
  { "{6371E0F5-1743-4FEE-BFFF-7FB548EBDC4A}", effect_shp_cheby_terms_data },
  { "{73135F8F-C0A0-481E-AA9C-7A749A5E415A}", effect_shp_cheby_sum_decay_data },
  { "{FA1D3AB3-0173-4881-90A9-A4830810CED1}", effect_delay_type_data },
  { "{02064D73-819F-4F19-B048-477C3AE810EE}", effect_dly_synced_data },
  { "{4B7E75E7-152D-4646-806C-8DA3587458EF}", effect_dly_mix_data },
  { "{681460BC-F34E-4D26-9A5F-DE36D78B0EC1}", effect_dly_amt_data },
  { "{A4CBA645-C23D-4372-90CA-8E9419601485}", effect_dly_fdbk_sprd_data },
  { "{983FB1AE-E7F3-4FD5-B904-C383E68402E3}", effect_dly_multi_sprd_data },
  { "{979FCE21-44D4-4CE3-88F6-04A27D14D19A}", effect_dly_fdbk_time_l_data },
  { "{30FFAD26-D7E5-406A-92E3-93AE10A23979}", effect_dly_fdbk_time_r_data },
  { "{E9BF0869-F1BD-4EF3-A509-690CD7C28361}", effect_dly_fdbk_tempo_l_data },
  { "{E2867983-8B3E-46E2-BBDA-31C0CE8128DC}", effect_dly_fdbk_tempo_r_data },
  { "{7CC600A0-8F63-4BC9-96BD-AA13E3471CA0}", effect_dly_multi_time_data },
  { "{BF2739BE-DAE4-4FD1-B1DB-E983940B8A8C}", effect_dly_multi_tempo_data },
  { "{868EA5D4-F2B3-4060-8436-A286AD0CE7C1}", effect_dly_multi_taps_data },
  { "{1C1CB2CF-321D-45A9-BAC4-B14A288C0F71}", effect_reverb_mix_data },
  { "{DADD125B-644B-4466-B16C-12A1BE491178}", effect_reverb_size_data },
  { "{1AA07F1E-6BFB-4B12-9145-1470688CF018}", effect_reverb_spread_data },
  { "{02746829-F580-4E49-B763-C02B96A863E7}", effect_reverb_damp_data },
  { "{46B7C3CF-3BBA-4BFF-AA7F-4DBEB87DDBE8}", effect_reverb_apf_data }
}; 

} // namespace inf::synth