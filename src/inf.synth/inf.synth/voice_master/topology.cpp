#include <inf.synth/voice_master/topology.hpp>
#include <inf.base/topology/part_descriptor.hpp>
#include <inf.base/topology/param_ui_descriptor.hpp>

using namespace inf::base;

namespace inf::synth {

static std::vector<list_item> const master_port_trigs = {
  { "{506D3FCE-5A35-4209-9A0E-0931C6EDDC0B}", "Note" },
  { "{8D81E1D4-6063-470A-A5E3-0E65308EA182}", "Voice" } }; 
static std::vector<list_item> const master_port_modes = {
  { "{8671F976-CB2C-4BB9-A79E-2BBA7A016967}", "Off" },
  { "{E3F6FCCB-B641-455D-BD0C-F52D7C65FF79}", "On" }, 
  { "{A270822E-E7E2-499F-8B24-62C4FEE3D912}", "Auto" } };
static std::vector<list_item> const master_modes = {
  { "{CF05B74A-F96C-47DF-B908-39477BB6A14B}", "Poly" },
  { "{5EFD4121-52DC-43F2-A853-CC4DB9F8342F}", "Mono" }, 
  { "{49996567-D045-488B-85D0-97EC55FE8CDB}", "Rlease" } };

static param_ui_descriptor master_cv1_bi_ui = { false, 0.0f, {
  { master_param::gcv1_switch_bi, [](std::int32_t v) { return v == 1; } } } };
static param_ui_descriptor master_cv2_bi_ui = { false, 0.0f, {
  { master_param::gcv2_switch_bi, [](std::int32_t v) { return v == 1; } } } };
static param_ui_descriptor master_cv3_bi_ui = { false, 0.0f, {
  { master_param::gcv3_switch_bi, [](std::int32_t v) { return v == 1; } } } };
static param_ui_descriptor master_cv1_uni_ui = { false, 0.0f, {
  { master_param::gcv1_switch_bi, [](std::int32_t v) { return v == 0; } } } };
static param_ui_descriptor master_cv2_uni_ui = { false, 0.0f, {
  { master_param::gcv2_switch_bi, [](std::int32_t v) { return v == 0; } } } }; 
static param_ui_descriptor master_cv3_uni_ui = { false, 0.0f, {
  { master_param::gcv3_switch_bi, [](std::int32_t v) { return v == 0; } } } }; 
static param_ui_descriptor const master_port_sync_ui = { false, 0.0f, {
  { master_param::port_mode, [](std::int32_t v) { return v != master_port_mode::off; } }, 
  { master_param::port_sync, [](std::int32_t v) { return v == 1; } } } };
static param_ui_descriptor const master_port_time_ui = { false, 0.0f, {
  { master_param::port_mode, [](std::int32_t v) { return v != master_port_mode::off; } },
  { master_param::port_sync, [](std::int32_t v) { return v == 0; } } } };
static param_ui_descriptor const master_port_on_ui = { false, 0.0f, {
  { master_param::port_mode, [](std::int32_t v) { return v != master_port_mode::off; } } } }; 

static std::vector<time_signature> const master_port_timesig = synced_timesig(false, { 4, 1 }, {
  { timesig_type::one_over, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 16, 32, 64 } },
  { timesig_type::lower, { 2, 3, 4, 5, 6, 7, 8, 9, 12, 16 } },
  { timesig_type::upper, { 2, 3, 4, 5, 6, 7, 8 } },
  { timesig_type::over_one, { 1, 2, 3, 4 } } });
std::vector<float> const master_port_timesig_values = synced_timesig_values(master_port_timesig);
static std::vector<std::string> const master_port_timesig_names = synced_timesig_names(master_port_timesig);

param_descriptor const
voice_params[voice_param::count] =
{
  { "{5A2DF5BA-7D6F-4053-983E-AA6DC5084373}", { { "Gain", "Gain" }, "dB", param_kind::continuous, decibel_bounds(2.0f), 1, nullptr } },
  { "{86782F43-7079-47BE-9C7F-8BF6D12A0950}", { { "Bal", "Stereo balance" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1), 3, nullptr } },
  { "{5E6A8C53-AE49-4FDF-944D-57CF37FC2C0E}", { { "Oct", "Octave" }, "", param_kind::voice, param_type::list_knob, { 0, 9, 4 }, 0, nullptr } },
  { "{AE673674-1A16-4219-8EE5-048722BF52D1}", { { "Note", "Note" }, "", param_kind::voice, param_type::knob_list, { &note_names, "C" }, 2, nullptr } }
};

param_descriptor const 
master_params[master_param::count] =       
{
  { "{536EBE78-85C2-461F-A3E5-2F7ADA11577C}", { { "Gain", "Gain" }, "dB", param_kind::continuous, decibel_bounds(2.0f), 11, nullptr } },
  { "{7917BE01-867D-490B-BD72-3CCE267CE872}", { { "Bal", "Stereo balance" }, "%", param_kind::continuous, linear_bounds(0.0f, 1.0f, 0.5f, -100.0f, 100.0f, 1), 12, nullptr } },
  { "{F3DFD0D7-652D-4F80-9C97-38037BCF58A7}", { { "Mode", "Voice mode" }, "", param_kind::block, param_type::list, { &master_modes, master_mode::poly }, 10, nullptr } },
  { "{511EE6C3-8798-4B7A-940D-100B8680517F}", { { "Port", "Portamento mode" }, "", param_kind::block, param_type::list, { &master_port_modes, master_port_mode::off }, 0, nullptr } },
  { "{921B5AE6-37FD-4953-94C3-16062C0D23ED}", { { "Trig", "Portamento trigger" }, "", param_kind::block, param_type::list, { &master_port_trigs, master_port_trig::note }, 3, &master_port_on_ui } },
  { "{09243EDB-2DBE-450F-800F-C37BF8A3C44B}", { { "Sync", "Portamento tempo sync" }, param_kind::block, false, 6, &master_port_on_ui } },
  { "{E27DEC2E-BA49-454C-8C28-F7532F6985DC}", { { "Time", "Portamento time" }, "Sec", param_kind::block, linear_bounds(0.01f, 10.0f, 1.0f, 2), 9, &master_port_time_ui } },
  { "{1DD53257-2104-4C66-BA00-2B73E9F6BA63}", { { "Tempo", "Portamento tempo" }, "", param_kind::block, param_type::knob_list, { &master_port_timesig_names, "1/16" }, 9, &master_port_sync_ui } },
  { "{F2E3B9B7-A0D5-44A3-8EA3-98B536B4E4C2}", { { "CV 1", "CV 1 unipolar" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 1, &master_cv1_uni_ui } },
  { "{B46FA59A-7F67-4F53-B02D-30942B14E082}", { { "CV 1", "CV 1 bipolar" }, "%", param_kind::continuous, percentage_m11_bounds(0.0f), 1, &master_cv1_bi_ui } },
  { "{1DAF5F89-EBB7-49BA-A80C-7F1F00F98F4E}", { { "Bipolar", "CV 1 bipolar on/off" }, param_kind::block, false, 2, nullptr } },
  { "{EAA17C14-3811-4152-BB68-D083FCE82398}", { { "CV 2", "CV 2 unipolar" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 4, &master_cv2_uni_ui } },
  { "{84A18512-60E4-4047-9806-1DCD9D80E76A}", { { "CV 2", "CV 2 bipolar" }, "%", param_kind::continuous, percentage_m11_bounds(0.0f), 4, &master_cv2_bi_ui } },
  { "{45B6CF48-E5FB-4492-BE68-F0802EA2A958}", { { "Bipolar", "CV 2 bipolar on/off" }, param_kind::block, false, 5, nullptr } },
  { "{A59EB157-224A-4617-8DCA-FAA52246FD0E}", { { "CV 3", "CV 3 unipolar" }, "%", param_kind::continuous, percentage_01_bounds(1.0f), 7, &master_cv3_uni_ui } },
  { "{0847DCB8-2956-4EAA-A864-63ADDFC3CA26}", { { "CV 3", "CV 3 bipolar" }, "%", param_kind::continuous, percentage_m11_bounds(0.0f), 7, &master_cv3_bi_ui } },
  { "{4ABC661F-2C3E-427A-90FE-A417C904B16F}", { { "Bipolar", "CV 3 bipolar on/off" }, param_kind::block, false, 8, nullptr } }
};
 
} // namespace inf::synth 