#include <inf.plugin.infernal_synth/envelope/topology.hpp>
#include <inf.base/shared/support.hpp>
#include <inf.base/topology/part_descriptor.hpp>

using namespace inf::base;

namespace inf::synth {

static std::vector<list_item> const env_modes = {
  { "{D4FC491C-38AB-4BC6-9076-B74CDFA76772}", "Legato" },
  { "{715E0B2B-DC05-4263-92C6-19D73CBC7508}", "Retrig" },
  { "{3070141F-252A-4BC5-815F-5C0329A67B4B}", "Multi" } };
static std::vector<list_item> const env_types = {
  { "{F5743634-859A-40A1-8F2E-1B5ED874840D}", "Sustain" },
  { "{95846D91-B563-4AFE-ABFA-634C0A0FE27C}", "Release" },
  { "{51B32BBA-C18B-4811-8280-73E0F339F4BC}", "Follow" } }; 

// 8/1 = 16 seconds at 120 bpm, 1/256 = 7 ms at 120 bpm.
static std::vector<time_signature> const envelope_timesig = synced_timesig(true, { 8, 1 }, {
  { timesig_type::one_over, { 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 16, 32, 64, 128, 256 } },
  { timesig_type::lower, { 2, 3, 4, 5, 6, 7, 8, 9, 12, 16 } },
  { timesig_type::upper, { 2, 3, 4, 5, 6, 7, 8 } },
  { timesig_type::over_one, { 1, 2, 3, 4, 5, 6, 7, 8 } } });
std::vector<float> const envelope_timesig_values = synced_timesig_values(envelope_timesig);
static std::vector<std::string> const envelope_timesig_names = synced_timesig_names(envelope_timesig); 

param_descriptor const
envelope_params[envelope_param::count] =
{
  { "{FC3CE4BC-D8F0-487E-9BB6-826988B4B812}", { { "On", "On" }, param_kind::voice, false } },
  { "{D622D344-A08E-4109-84D8-C310B81C2769}", { { "Type", "Type" }, "", param_kind::voice, param_type::list, { &env_types, envelope_type::sustain } } },
  { "{3B059678-69D7-4427-B890-51C0B3D24E45}", { { "Mode", "Mode" }, "", param_kind::voice, param_type::list, { &env_modes, envelope_mode::legato } } },
  { "{82B2C892-1A12-4FDC-A26D-7733C3C3B782}", { { "Sync", "Tempo sync" }, param_kind::voice, false } },
  { "{2D8C38A2-99AD-4080-8262-26D24F7644F0}", { { "Bipolar", "Bipolar" }, param_kind::voice, false } },
  { "{E5588CB4-9582-4B07-A8EF-5098C5D65C88}", { { "Invert", "Invert" }, param_kind::voice, false } },
  { "{B1D30C73-FF09-452C-A9D5-00BB8B2CE58D}", { { "Dly", "Delay time" }, "Sec", param_kind::voice, quad_bounds(0.0f, 20.0f, "0", 3) } },
  { "{70C49337-7141-42BC-B336-78B28F4770E3}", { { "Dly", "Delay tempo" }, "", param_kind::voice, param_type::knob_list, { &envelope_timesig_names, "0" } } },
  { "{E0A45A06-3C0C-4409-BBFA-A2834F0C7BAD}", { { "A1", "A1 time" }, "Sec", param_kind::voice, quad_bounds(0.0f, 20.0f, "0.05", 3) } },
  { "{E0CFCBB7-B050-4C47-B78C-2511AD2EE6B6}", { { "A1", "A1 tempo" }, "", param_kind::voice, param_type::knob_list, { &envelope_timesig_names, "1/16" } } },
  { "{1400CEB5-25B1-42B1-ADA1-B5FDF8C6D1E5}", { { "S1", "A1 slope" }, "%", param_kind::voice, percentage_m11_bounds(0.0f) } },
  { "{9D1275BC-9206-4CEA-B109-51974951B3DB}", { { "Split", "Attack split" }, "%", param_kind::voice, percentage_01_bounds(1.0f) } },
  { "{80DA6D03-7A4A-4378-B1A4-89B7DFF510C0}", { { "A2", "A2 time" }, "Sec", param_kind::voice, quad_bounds(0.0f, 20.0f, "0", 3) } },
  { "{77B6D37E-AF81-4EFB-B7B9-D9BC588A865A}", { { "A2", "A2 tempo" }, "", param_kind::voice, param_type::knob_list, { &envelope_timesig_names, "0" } } },
  { "{A79C8824-2D1B-4236-B417-F0D6DBBF9A9C}", { { "S2", "A2 slope" }, "%", param_kind::voice, percentage_m11_bounds(0.0f) } },
  { "{AADD118E-B9CE-464E-83C0-1FAE5A62F530}", { { "Hld", "Hold time" }, "Sec", param_kind::voice, quad_bounds(0.0f, 20.0f, "0", 3) } },
  { "{AC5D88B7-8CDB-4401-9FB0-20EF0195ABD8}", { { "Hld", "Hold tempo" }, "", param_kind::voice, param_type::knob_list, { &envelope_timesig_names, "0" } } },
  { "{A62B5DB0-4D41-4DA4-AE14-E25A2C983B21}", { { "D1", "D1 time" }, "Sec", param_kind::voice, quad_bounds(0.0f, 20.0f, "0.2", 3) } },
  { "{09B32681-21EF-4C65-B5FD-9FA64ED4AF5E}", { { "D1", "D1 tempo" }, "", param_kind::voice, param_type::knob_list, { &envelope_timesig_names, "1/8" } } },
  { "{62381FB9-9060-4425-97A6-B57ECB2BECCA}", { { "S1", "D1 slope" }, "%", param_kind::voice, percentage_m11_bounds(0.0f) } },
  { "{57776D47-48CE-4912-A6D4-60153980B8A4}", { { "Split", "Decay split" }, "%", param_kind::voice, percentage_01_bounds(0.0f) } },
  { "{1E64D9B4-012C-4E5A-A0A3-B059002E22BD}", { { "D2", "D2 time" }, "Sec", param_kind::voice, quad_bounds(0.0f, 20.0f, "0", 3) } },
  { "{BCB90413-D4CE-41D8-8AAA-8BA48A4410B7}", { { "D2", "D2 tempo" }, "", param_kind::voice, param_type::knob_list, { &envelope_timesig_names, "0" } } },
  { "{06CC73A7-850F-431D-99B1-100D4953DF83}", { { "S2", "D2 slope" }, "%", param_kind::voice, percentage_m11_bounds(0.0f) } },
  { "{721AFEB5-A17E-4C37-BF3A-94F645234B73}", { { "Stn", "Sustain level" }, "%", param_kind::voice, percentage_01_bounds(0.5f) } },
  { "{75ACC8A6-F228-445C-9788-A367AE32EAAA}", { { "R1", "R1 time" }, "Sec", param_kind::voice, quad_bounds(0.0f, 20.0f, "0.4", 3) } },
  { "{DDA55E4D-A100-40BA-A7C2-F4C284CACE08}", { { "R1", "R1 tempo" }, "", param_kind::voice, param_type::knob_list, { &envelope_timesig_names, "1/4" } } },
  { "{6B978A93-54D1-4990-BD2E-BC143EA816AF}", { { "S1", "R1 slope" }, "%", param_kind::voice, percentage_m11_bounds(0.0f) } },
  { "{38F801F5-7F13-4B8C-AE46-0E94B3D75147}", { { "Split", "Release split" }, "%", param_kind::voice, percentage_01_bounds(0.0f) } },
  { "{BE15AD7C-1680-44CF-8A00-5CCA544E06FE}", { { "R2", "R2 time" }, "Sec", param_kind::voice, quad_bounds(0.0f, 20.0f, "0", 3) } },
  { "{6DBD336D-55FC-427A-95E8-708234A8631A}", { { "R2", "R2 tempo" }, "", param_kind::voice, param_type::knob_list, { &envelope_timesig_names, "0" } } },
  { "{AD5E39BE-C438-4448-A43E-9496352BB545}", { { "S2", "R2 slope" }, "%", param_kind::voice, percentage_m11_bounds(0.0f) } }, 
}; 
 
} // namespace inf::synth