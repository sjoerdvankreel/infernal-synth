#include <inf.synth/lfo/graph.hpp>
#include <inf.synth/amp/topology.hpp>
#include <inf.synth/lfo/topology.hpp>
#include <inf.synth/cv_bank/graph.hpp>
#include <inf.synth/effect/graph1.hpp>
#include <inf.synth/effect/graph2.hpp>
#include <inf.synth/voice/topology.hpp>
#include <inf.synth/synth/topology.hpp>
#include <inf.synth/envelope/graph.hpp>
#include <inf.synth/envelope/topology.hpp>
#include <inf.synth/effect/topology.hpp>
#include <inf.synth/output/topology.hpp>
#include <inf.synth/envelope/topology.hpp>
#include <inf.synth/synth/processor.hpp>
#include <inf.synth/oscillator/graph_wave.hpp>
#include <inf.synth/oscillator/graph_spectrum.hpp>
 
using namespace inf::base;

namespace inf::synth {

// ---- plugin entry ----

std::unique_ptr<base::audio_processor>
synth_topology::create_audio_processor(
  base::param_value* state, std::int32_t* changed,
  float sample_rate, std::int32_t max_sample_count) const
{ return std::make_unique<synth_processor>(this, state, changed, sample_rate, max_sample_count); }

std::unique_ptr<base::graph_processor>
synth_topology::create_graph_processor_effect(base::part_id id, std::int32_t graph_type) const
{
  switch (graph_type)
  {
  case effect_graph::graph1: return std::make_unique<effect_graph1>(this, id);
  case effect_graph::graph2: return std::make_unique<effect_graph2>(this, id);
  default: assert(false); return nullptr;
  }
}

std::unique_ptr<base::graph_processor>
synth_topology::create_graph_processor_oscillator(base::part_id id, std::int32_t graph_type) const
{
  switch (graph_type)
  {
  case osc_graph::left: return std::make_unique<oscillator_wave_graph>(this, id, false);
  case osc_graph::right: return std::make_unique<oscillator_wave_graph>(this, id, true);
  case osc_graph::spectrum: return std::make_unique<oscillator_spectrum_graph>(this, id);
  default: assert(false); return nullptr;
  }
}
 
std::unique_ptr<graph_processor>
synth_topology::create_graph_processor(part_id id, std::int32_t graph_type) const
{ 
  switch (id.type)
  {
  case part_type::venv: return std::make_unique<envelope_graph>(this, id);
  case part_type::vosc: return create_graph_processor_oscillator(id, graph_type);
  case part_type::vlfo: case part_type::glfo: return std::make_unique<lfo_graph>(this, id);
  case part_type::vcv_plot: case part_type::gcv_plot: return std::make_unique<cv_bank_graph>(this, id);
  case part_type::veffect: case part_type::geffect: return create_graph_processor_effect(id, graph_type);
  default: assert(false); return nullptr;
  }
}

// ---- file format conversion ----

base::param_value 
synth_topology::convert_param(
  std::int32_t index, 
  param_value old_value, std::string const& old_text,
  std::uint16_t old_major, std::uint16_t old_minor) const
{
  // 1.1 changed envelope time bounds from 10 to 20
  if (old_major < 1 || (old_major == 1 && old_minor < 1))
    for(std::int32_t i = 0; i < venv_count; i++)
    {
      std::int32_t env_start = param_bounds[part_type::venv][i];
      if (index == env_start + envelope_param::delay_time ||
        index == env_start + envelope_param::hold_time ||
        index == env_start + envelope_param::attack1_time ||
        index == env_start + envelope_param::attack2_time ||
        index == env_start + envelope_param::decay1_time ||
        index == env_start + envelope_param::decay2_time ||
        index == env_start + envelope_param::release1_time ||
        index == env_start + envelope_param::release2_time)
      {
        float old_in_range = real_bounds::quadratic(0.0f, 10.0f).to_range(old_value.real);
        return param_value(params[index].descriptor->data.real.dsp.from_range(old_in_range));
      }
    }

  return old_value;
}

std::int32_t 
synth_topology::try_move_stored_param(
  base::stored_param_id const& id, base::param_value old_value, std::string const& old_text,
  std::uint16_t old_major, std::uint16_t old_minor, bool& can_be_ignored) const
{
  can_be_ignored = false;
  if (!(old_major < 1 || (old_major == 1 && old_minor < 2))) return -1;

  // Oscillator gain dropped.
  if (std::string("{5C9D2CD3-2D4C-4205-893E-6B5DE9D62ADE}") == id.part_guid
    && std::string("{09E50DA8-2467-462F-9822-7E9074A51B53}") == id.param_guid)
  {
    can_be_ignored = old_value.real == 0.5f;
    return -1;
  }
  // Voice/global amp/bal param moved to voice/global amp section.
  else if (std::string("{5A2DF5BA-7D6F-4053-983E-AA6DC5084373}") == id.param_guid)
    return param_index({ part_type::vamp, 0 }, amp_param::gain);
  else if (std::string("{86782F43-7079-47BE-9C7F-8BF6D12A0950}") == id.param_guid)
    return param_index({ part_type::vamp, 0 }, amp_param::bal);
  else if (std::string("{536EBE78-85C2-461F-A3E5-2F7ADA11577C}") == id.param_guid)
    return param_index({ part_type::gamp, 0 }, amp_param::gain);
  else if (std::string("{7917BE01-867D-490B-BD72-3CCE267CE872}") == id.param_guid)
    return param_index({ part_type::gamp, 0 }, amp_param::bal);
  // Voice related stuff moved from master to new voice section.
  else if (std::string("{5E6A8C53-AE49-4FDF-944D-57CF37FC2C0E}") == id.param_guid)
    return param_index({ part_type::voice, 0 }, voice_param::oct);
  else if (std::string("{AE673674-1A16-4219-8EE5-048722BF52D1}") == id.param_guid)
    return param_index({ part_type::voice, 0 }, voice_param::note);
  else if (std::string("{F3DFD0D7-652D-4F80-9C97-38037BCF58A7}") == id.param_guid)
    return param_index({ part_type::voice, 0 }, voice_param::mode);
  else if (std::string("{511EE6C3-8798-4B7A-940D-100B8680517F}") == id.param_guid)
    return param_index({ part_type::voice, 0 }, voice_param::port_mode);
  else if (std::string("{921B5AE6-37FD-4953-94C3-16062C0D23ED}") == id.param_guid)
    return param_index({ part_type::voice, 0 }, voice_param::port_trig);
  else if (std::string("{09243EDB-2DBE-450F-800F-C37BF8A3C44B}") == id.param_guid)
    return param_index({ part_type::voice, 0 }, voice_param::port_sync);
  else if (std::string("{E27DEC2E-BA49-454C-8C28-F7532F6985DC}") == id.param_guid)
    return param_index({ part_type::voice, 0 }, voice_param::port_time);
  else if (std::string("{1DD53257-2104-4C66-BA00-2B73E9F6BA63}") == id.param_guid)
    return param_index({ part_type::voice, 0 }, voice_param::port_tempo);
  // Audio A enabled dropped.
  else if (std::string("{7A77C027-FC8F-4425-9BF0-393267D92F0C}") == id.part_guid 
    && std::string("{14096099-485D-4EB9-B055-E393DE2E993C}") == id.param_guid)
  {
    can_be_ignored = true;
    return -1;
  }
  // Audio B enabled dropped.
  else if (std::string("{B5B4A442-13ED-43ED-B9E0-3B2894D03838}") == id.part_guid
    && std::string("{85A0A7FB-8319-436E-9979-0A7267F1F636}") == id.param_guid)
  {
    can_be_ignored = true;
    return -1;
  }
  // Audio A moved from 4x6 to 1x15.
  else if (std::string("{7A77C027-FC8F-4425-9BF0-393267D92F0C}") == id.part_guid)
  {
    std::int32_t const old_route_count = 6;
    std::int32_t const new_route_count = 15;
    char const* old_in[old_route_count] = {
      "{2E9F0478-B911-43DF-BB51-0C5836E4853F}",
      "{A3A59082-CF73-4C28-A3FC-037729C9CB42}",
      "{A8E6882A-8945-4F59-92B9-78004EAF5818}",
      "{7DFD9AF0-7419-4B06-B875-F18D9C344D42}",
      "{2A252D70-9750-4385-8405-AE1EAF5E8018}",
      "{34E1B446-EF1B-4715-9993-64A177769033}" };
    char const* old_out[old_route_count] = {
      "{295DC5F0-FB32-4D43-8799-D79F23FD3AA9}",
      "{843EB41C-199F-4FAC-ACC4-841B3663248D}",
      "{D2E34E63-DE9E-4620-924D-1897614BF983}",
      "{C2C5B9BC-81B6-4D3E-947E-E0B900447BDF}",
      "{80FF4399-33CE-4A65-9A2C-C48271EAACDD}",
      "{DAA1F891-5A2A-47A7-B923-61932694B951}" };
    char const* old_amt[old_route_count] = {
      "{A3B15FE9-56DB-493B-A4E1-31A004F3C937}",
      "{810B55DF-6230-45C7-B478-7A3569DC9127}",
      "{6721B1EC-9688-48A3-B5B8-0ADD0A9CF16B}",
      "{51E69AB1-37F7-4361-84B9-2F1727F66C4A}",
      "{88EACE54-DAFA-4EEF-A7A0-65464C54A66E}",
      "{782750CE-319E-4ED2-906B-106C72A9A85C}" };
    char const* old_bal[old_route_count] = {
      "{91996C1F-4510-4BC1-97BA-135C9881263F}",
      "{265269F4-F6DF-474E-A696-44EE01681C65}",
      "{64C90CE2-2AC0-4675-8F99-B88E807F712A}",
      "{01B6309E-B045-48A7-9BC1-8E828A528A3F}",
      "{77F5DF0C-B9E8-4059-AE8B-75B9D6E3E0CE}",
      "{EF2AD9AF-0A3C-4DD7-8650-5DD6974C4625}" };
    // Don't bother with defaults.
    for (std::int32_t i = 0; i < old_route_count; i++)
      if (id.param_guid == old_in[i] && old_value.discrete == 0
        || id.param_guid == old_out[i] && old_value.discrete == 0
        || id.param_guid == old_amt[i] && old_value.real == 0.5f
        || id.param_guid == old_bal[i] && old_value.real == 0.5f)
      {
        can_be_ignored = true;
        return -1;
      }
    // Can we map to the new matrix ?
    for (std::int32_t i = 0; i < old_route_count; i++)
      if (id.param_guid == old_in[i]
        || id.param_guid == old_out[i]
        || id.param_guid == old_amt[i]
        || id.param_guid == old_bal[i])
      {
        std::int32_t new_route_index = id.part_index * old_route_count + i;
        if(new_route_index >= new_route_count) return -1;
        return param_index({ part_type::vaudio_bank, 0 }, new_route_index);
      }
  }
  // Audio B moved from 3x6 to 1x15.
  else if (std::string("{B5B4A442-13ED-43ED-B9E0-3B2894D03838}") == id.part_guid)
  {
    std::int32_t const old_route_count = 6;
    std::int32_t const new_route_count = 15;
    char const* old_in[old_route_count] = {
      "{CD5CD403-259F-4B25-9C33-E246931E973B}", 
      "{E83D6E12-47C4-4738-8CFA-A18CC8B86C67}", 
      "{EF89DAE6-59F6-4B5E-BA7F-F9F9F4FA64C0}", 
      "{3F1E4A05-2C73-418D-B490-841106011784}", 
      "{2D03FFCF-FD1D-42F7-B95B-BE3262A2900F}", 
      "{47517379-6D53-48C7-BFD0-A2D582C7971F}" };
    char const* old_out[old_route_count] = {
      "{6AD76233-62A6-4F5A-ADCB-797786E00C54}", 
      "{8BEAB138-D485-480C-B2BE-146354C5A2F9}", 
      "{33E9FBE4-4654-4B92-AD03-5EF2EC3FEEF2}", 
      "{9F28A209-5A25-4665-BB73-557BE1F3CC05}", 
      "{5452CCEE-C962-4239-9F5C-015445598B72}", 
      "{E6B0B447-2412-43D1-9BED-66BC67620272}" };
    char const* old_amt[old_route_count] = {
      "{E444E539-65EF-449F-8407-EB128C6082B8}", 
      "{FBCCD63D-1D9A-444C-A622-D9D3E8A771C7}", 
      "{C147FB0B-25A9-44F9-88E2-77CA415F83BB}", 
      "{E394ACC9-DA07-43DF-9BF7-02A57BF3F758}", 
      "{8A1B591B-62AD-4E53-9256-7C4BFB15525F}", 
      "{2D1B1704-9B20-44EE-AC48-E8B09FBA9488}" };
    char const* old_bal[old_route_count] = {
      "{3E7BA2FC-2984-4B2F-9936-754BEE44CFCE}", 
      "{D2672F61-8811-420C-A4CA-1ED78D49AC55}", 
      "{10073537-70C6-40FB-8F53-0ABB2C594944}", 
      "{EBBBAC9E-3D8F-482A-A41B-32CB47324647}", 
      "{80D09377-31F5-4162-A785-D0B841FCDBA6}", 
      "{7E72F00C-2C26-4BAD-98E5-ADC7168852F7}" };
    // Don't bother with defaults.
    for (std::int32_t i = 0; i < old_route_count; i++)
      if (id.param_guid == old_in[i] && old_value.discrete == 0
        || id.param_guid == old_out[i] && old_value.discrete == 0
        || id.param_guid == old_amt[i] && old_value.real == 0.5f
        || id.param_guid == old_bal[i] && old_value.real == 0.5f)
      {
        can_be_ignored = true;
        return -1;
      }
    // Can we map to the new matrix ?
    for (std::int32_t i = 0; i < old_route_count; i++)
      if (id.param_guid == old_in[i]
        || id.param_guid == old_out[i]
        || id.param_guid == old_amt[i]
        || id.param_guid == old_bal[i])
      {
        std::int32_t new_route_index = id.part_index * old_route_count + i;
        if (new_route_index >= new_route_count) return -1;
        return param_index({ part_type::gaudio_bank, 0 }, new_route_index);
      }
  }
  return -1;
}

// ---- clear / init patch ----

void 
synth_topology::init_clear_patch(param_value* state) const
{
  if(is_instrument()) init_instrument_clear_patch(state);
  else init_fx_clear_patch(state);
}

void 
synth_topology::init_factory_preset(param_value* state) const
{
  if(is_instrument()) init_instrument_factory_preset(state);
  else init_fx_factory_preset(state);
}

void 
synth_topology::init_fx_clear_patch(param_value* state) const
{
  assert(!"TODO");
  topology_info::init_clear_patch(state);

  // Bare minimum to have a basic delay line.
  set_ui_value(state, part_type::geffect, 0, effect_param::on, "On");
  set_ui_value(state, part_type::geffect, 0, effect_param::type, "Delay");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in1, "Ext");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out1, "FX 1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in2, "FX 1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out2, "M.Out");
}

void
synth_topology::init_instrument_clear_patch(param_value* state) const
{
  topology_info::init_clear_patch(state);

  // Bare minimum to have sound.
  set_ui_value(state, part_type::vosc, 0, osc_param::on, "On");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::in1, "Osc 1");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::out1, "V.Out");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in1, "V.Out");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out1, "M.Out");
}

void 
synth_topology::init_fx_factory_preset(param_value* state) const
{
  assert(!"TODO");
  topology_info::init_factory_preset(state);

  // gfx 1 reverb
  set_ui_value(state, part_type::geffect, 0, effect_param::on, "On");
  set_ui_value(state, part_type::geffect, 0, effect_param::type, "Reverb");
  set_ui_value(state, part_type::geffect, 0, effect_param::reverb_mix, "100");
  set_ui_value(state, part_type::geffect, 0, effect_param::reverb_damp, "99");
  set_ui_value(state, part_type::geffect, 0, effect_param::reverb_size, "99");
  set_ui_value(state, part_type::geffect, 0, effect_param::reverb_spread, "33");
  set_ui_value(state, part_type::geffect, 0, effect_param::reverb_apf, "10");

  // gfx 3 delay
  set_ui_value(state, part_type::geffect, 2, effect_param::on, "On");
  set_ui_value(state, part_type::geffect, 2, effect_param::type, "Delay");
  set_ui_value(state, part_type::geffect, 2, effect_param::delay_type, "Feedback");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_mix, "100");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_synced, "On");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_amt, "67");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_fdbk_sprd, "75");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_fdbk_tempo_l, "3/16");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_fdbk_tempo_r, "5/16");

  // gfx bell filters
  for (std::int32_t i = 1; i <= 3; i += 2)
  {
    set_ui_value(state, part_type::geffect, i, effect_param::on, "On");
    set_ui_value(state, part_type::geffect, i, effect_param::type, "Filter");
    set_ui_value(state, part_type::geffect, i, effect_param::filter_type, "StVar");
    set_ui_value(state, part_type::geffect, i, effect_param::flt_stvar_type, "BLL");
    set_ui_value(state, part_type::geffect, i, effect_param::flt_stvar_shlf_gain, "24");
    set_ui_value(state, part_type::geffect, i, effect_param::flt_stvar_freq, "2000");
  }

  // audio routing
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in1, "Ext");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out1, "FX 1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in2, "FX 1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out2, "FX 2");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in3, "FX 2");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out3, "Amp");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in4, "Ext");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out4, "FX 3");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in5, "FX 3");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out5, "FX 4");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in6, "FX 4");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out6, "Amp");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in7, "Ext");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out7, "Amp");

  // master
  set_ui_value(state, part_type::gamp, 0, amp_param::gain, "-18");
  set_ui_value(state, part_type::master, 0, master_param::gcv1_uni, "10");
  set_ui_value(state, part_type::master, 0, master_param::gcv1_bi, "-10");
  set_ui_value(state, part_type::master, 0, master_param::gcv2_bi, "33");

  // glfo1
  set_ui_value(state, part_type::glfo, 0, lfo_param::on, "On");
  set_ui_value(state, part_type::glfo, 0, lfo_param::synced, "On");
  set_ui_value(state, part_type::glfo, 0, lfo_param::tempo, "3/2");
  set_ui_value(state, part_type::glfo, 0, lfo_param::bipolar, "On");

  // cv routing
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::in1, "CVU 1");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::out1, "Audio 1 Amt3");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::op1, "Mul");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::in2, "CVB 1");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::out2, "FX 2 StVar Frq");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::op2, "Add");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::in3, "CVU 2");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::out3, "Audio 1 Amt6");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::op3, "Mul");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::in4, "CVB 2");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::out4, "FX 4 StVar Frq");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::op4, "Add");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::in5, "G.LFO 1");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::out5, "FX 2 StVar Frq");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::amt5, "50");

  // cv plot
  set_ui_value(state, part_type::gcv_plot, 0, cv_plot_param::length, "10");
  set_ui_value(state, part_type::gcv_plot, 0, cv_plot_param::target, "FX 2 StVar Frq");
}

void 
synth_topology::init_instrument_factory_preset(param_value* state) const
{
  topology_info::init_factory_preset(state);

  // osc 1 detuned saw
  set_ui_value(state, part_type::vosc, 0, osc_param::on, "On");
  set_ui_value(state, part_type::vosc, 0, osc_param::type, "Basic");
  set_ui_value(state, part_type::vosc, 0, osc_param::basic_type, "Saw");
  set_ui_value(state, part_type::vosc, 0, osc_param::uni_voices, "3");
  set_ui_value(state, part_type::vosc, 0, osc_param::uni_dtn, "33");
  set_ui_value(state, part_type::vosc, 0, osc_param::uni_sprd, "50");
  set_ui_value(state, part_type::vosc, 0, osc_param::uni_offset, "50");

  // vfx 1 tanh shaper
  set_ui_value(state, part_type::veffect, 0, osc_param::on, "On");
  set_ui_value(state, part_type::veffect, 0, effect_param::type, "Shape");
  set_ui_value(state, part_type::veffect, 0, effect_param::shaper_type, "Tanh");
  set_ui_value(state, part_type::veffect, 0, effect_param::shp_gain, "4");

  // vfx 2 state variable filter
  set_ui_value(state, part_type::veffect, 1, osc_param::on, "On");
  set_ui_value(state, part_type::veffect, 1, effect_param::type, "Filter");
  set_ui_value(state, part_type::veffect, 1, effect_param::filter_type, "StVar");
  set_ui_value(state, part_type::veffect, 1, effect_param::flt_stvar_kbd, "50");
  set_ui_value(state, part_type::veffect, 1, effect_param::flt_stvar_res, "50");
  set_ui_value(state, part_type::veffect, 1, effect_param::flt_stvar_freq, "7500");

  // gfx 1 state variable filter
  set_ui_value(state, part_type::geffect, 0, osc_param::on, "On");
  set_ui_value(state, part_type::geffect, 0, effect_param::type, "Filter");
  set_ui_value(state, part_type::geffect, 0, effect_param::filter_type, "StVar");
  set_ui_value(state, part_type::geffect, 0, effect_param::flt_stvar_freq, "7500");
  set_ui_value(state, part_type::geffect, 0, effect_param::flt_stvar_res, "50");

  // gfx 2 multitap delay
  set_ui_value(state, part_type::geffect, 1, osc_param::on, "On");
  set_ui_value(state, part_type::geffect, 1, effect_param::type, "Delay");
  set_ui_value(state, part_type::geffect, 1, effect_param::delay_type, "Multitap");
  set_ui_value(state, part_type::geffect, 1, effect_param::dly_synced, "On");
  set_ui_value(state, part_type::geffect, 1, effect_param::dly_amt, "33");
  set_ui_value(state, part_type::geffect, 1, effect_param::dly_multi_sprd, "50");
  set_ui_value(state, part_type::geffect, 1, effect_param::dly_multi_tempo, "3/16");

  // master gain and cvs
  set_ui_value(state, part_type::gamp, 0, amp_param::gain, "3.0");
  set_ui_value(state, part_type::master, 0, master_param::gcv1_uni, "50");
  set_ui_value(state, part_type::master, 0, master_param::gcv2_uni, "66");

  // vaudio 1 osc->fx1->fx2->voice
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::in1, "Osc");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::out1, "FX 1");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::in2, "FX 1");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::out2, "FX 2");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::in3, "FX 2");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::out3, "V.Out");

  // gaudio 1 voice->fx1->fx2->master
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in1, "V.Out");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out1, "FX 1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in2, "FX 1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out2, "FX 2");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in3, "FX 2");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out3, "M.Out");

  // vlfo1 on & bipolar
  set_ui_value(state, part_type::vlfo, 0, lfo_param::on, "On");
  set_ui_value(state, part_type::vlfo, 0, lfo_param::rate, "15");
  set_ui_value(state, part_type::vlfo, 0, lfo_param::bipolar, "On");

  // glfo1 on & bipolar & synced
  set_ui_value(state, part_type::glfo, 0, lfo_param::on, "On");
  set_ui_value(state, part_type::glfo, 0, lfo_param::synced, "On");
  set_ui_value(state, part_type::glfo, 0, lfo_param::tempo, "7/4");
  set_ui_value(state, part_type::glfo, 0, lfo_param::bipolar, "On");
  set_ui_value(state, part_type::glfo, 0, lfo_param::invert, "On");
  set_ui_value(state, part_type::glfo, 0, lfo_param::type, "Rand");
  set_ui_value(state, part_type::glfo, 0, lfo_param::rand_type, "Both");
  set_ui_value(state, part_type::glfo, 0, lfo_param::filter, "50");
  set_ui_value(state, part_type::glfo, 0, lfo_param::rand_seedy, "7");
  set_ui_value(state, part_type::glfo, 0, lfo_param::rand_steps, "11");

  // env 2 on and env 1 shows slopes
  set_ui_value(state, part_type::venv, 1, envelope_param::on, "On");
  set_ui_value(state, part_type::venv, 0, envelope_param::sustain_level, "66");
  set_ui_value(state, part_type::venv, 0, envelope_param::attack1_time, "0.025");
  set_ui_value(state, part_type::venv, 0, envelope_param::attack1_slope, "33");
  set_ui_value(state, part_type::venv, 0, envelope_param::attack_split_level, "50");
  set_ui_value(state, part_type::venv, 0, envelope_param::attack2_time, "0.025");
  set_ui_value(state, part_type::venv, 0, envelope_param::attack2_slope, "-33");
  set_ui_value(state, part_type::venv, 0, envelope_param::decay1_time, "0.1");
  set_ui_value(state, part_type::venv, 0, envelope_param::decay1_slope, "33");
  set_ui_value(state, part_type::venv, 0, envelope_param::decay_split_level, "50");
  set_ui_value(state, part_type::venv, 0, envelope_param::decay2_time, "0.1");
  set_ui_value(state, part_type::venv, 0, envelope_param::decay2_slope, "-33");
  set_ui_value(state, part_type::venv, 0, envelope_param::release1_time, "0.2");
  set_ui_value(state, part_type::venv, 0, envelope_param::release1_slope, "33");
  set_ui_value(state, part_type::venv, 0, envelope_param::release_split_level, "50");
  set_ui_value(state, part_type::venv, 0, envelope_param::release2_time, "0.2");
  set_ui_value(state, part_type::venv, 0, envelope_param::release2_slope, "-33");

  // vcv velo to voice amp, vlfo1 to osc 1 cent, env 2 to filter freq, cvu 2 to filter freq
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::op1, "Mul");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::in1, "Velocity");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::out1, "V.Out Gain");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::op2, "Add");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::in2, "V.LFO 1");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::out2, "Osc 1 Cent");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::amt2, "10");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::in3, "Env 2");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::out3, "FX 2 StVar Frq");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::op3, "Mul");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::in4, "CVU 2");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::out4, "FX 2 StVar Frq");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::off4, "20");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::op4, "Mul");

  // glfo 1 to filter freq & master cvs
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::in1, "LFO 1");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::out1, "FX 1 StVar Frq");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::op1, "Add");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::amt1, "66");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::in2, "CVU 1");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::out2, "M.Out Gain");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::op2, "Mul");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::in3, "CVB 1");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::out3, "M.Out Bal");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::op3, "Add");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::in4, "CVU 2");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::out4, "FX 1 StVar Frq");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::op4, "Mul");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::off4, "20");

  // cv plots
  set_ui_value(state, part_type::gcv_plot, 0, cv_plot_param::length, "5");
  set_ui_value(state, part_type::gcv_plot, 0, cv_plot_param::target, "FX 1 StVar Frq");
  set_ui_value(state, part_type::vcv_plot, 0, cv_plot_param::target, "FX 2 StVar Frq");
}

// ---- params ----

static std::vector<list_item> const edit_selector_types = {
{ "{73A12F62-82E3-4FCF-B507-2DF2BB00521C}", "Edit Voice" },
{ "{F1C6C636-BDCB-48C8-8BBC-AAD2ECF5AD0D}", "Edit Global" } };
static std::vector<list_item> const edit_ui_size_types = {
{ "{A1D66F6B-5B65-45C1-9DED-15D354E12CC4}", "XS UI" },
{ "{54E5A984-7274-4600-B553-BF19C405FA2F}", "Small UI" },
{ "{0B11E0C4-D075-4FD0-9BCC-28C9F9FF08D1}", "Medium UI" },
{ "{202B0244-1283-4ED5-A11A-238B8F7C5F84}", "Large UI" },
{ "{997F1746-CEA8-4736-ABA2-67C2C502B69D}", "XL UI" } };

param_descriptor const
master_params[master_param::count] =
{
  { "{50F02184-1EAC-4A80-8832-0728B9EBF455}", { { "CVU 1", "CVU 1" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{D8332EF2-E4CE-4605-983A-6EC409194773}", { { "CVB 1", "CVB 1" }, "%", param_kind::continuous, percentage_m11_bounds(0.0f) } },
  { "{060B5E5E-2E49-4DF1-90B8-8D8F87E98707}", { { "CVU 2", "CVU 2" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{580FF8F1-4C06-4562-B4A0-702355B6E152}", { { "CVB 2", "CVB 2" }, "%", param_kind::continuous, percentage_m11_bounds(0.0f) } },
  { "{DD9A20AD-563A-4855-BAEF-2C53E6B94815}", { { "CVU 3", "CVU 3" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{B3B033A3-B615-4AD2-81AB-5CB769891BB0}", { { "CVB 3", "CVB 3" }, "%", param_kind::continuous, percentage_m11_bounds(0.0f) } }
};

param_descriptor const
edit_selector_params[edit_selector_param::count] =
{
  { "{5B22861B-88F6-446C-AA92-BA6B39712342}", { { "Edit", "Edit" }, "", param_kind::ui, param_type::list, { &edit_selector_types, edit_selector_type::edit_voice } } },
  { "{15190018-F886-42EB-BE04-21AE783F43B0}", { { "UI Size", "UI Size" }, "", param_kind::ui, param_type::list, { &edit_ui_size_types, edit_ui_size_type::xs_ui } } }
};

param_descriptor const
patch_params[patch_param::count] =
{
  { "{75A5B509-3C0C-42BC-BD3B-272D21C3DB17}", { { "Preset", "Factory preset" }, "", param_kind::ui, linear_bounds(0.0f, 1.0f, 0.0f, 8) } }
};

// ---- part selector ----

static std::vector<list_item> const active_vosc_items = list_items({ "{3B2713C0-6DFF-4257-9CB4-DF5FD0DA603C}", "Osc" }, vosc_count);
static std::vector<list_item> const active_veffect_items = list_items({ "{33BB6F1B-3D4F-4D58-998D-417DA711D6C2}", "FX" }, veffect_count);
static std::vector<list_item> const active_geffect_items = list_items({ "{1607D775-53C5-46BE-957A-F697831062CD}", "FX" }, geffect_count);
static std::vector<list_item> const active_venv_items = list_items({ "{8545D490-B40D-4154-BFC2-FE86A6F26BB5}", "Env" }, venv_count);
static std::vector<list_item> const active_vlfo_items = list_items({ "{5AADD04D-FC8B-4326-97B1-35326F645D09}", "LFO" }, vlfo_count);
static std::vector<list_item> const active_glfo_items = list_items({ "{88E92673-4FA2-45C3-B1D0-71A2A63ECCF4}", "LFO" }, glfo_count);
 
static param_descriptor const 
active_params[active_param::count] =     
{
  { "{0EEADBD0-D37B-4B53-A2D5-F22E2154F2D8}", { { "Osc", "Oscillator" }, "", param_kind::ui, param_type::list, { &active_vosc_items, 0 } } },
  { "{D5DD0DFC-AC9D-42E6-9D2B-924786382825}", { { "V.FX", "Voice FX" }, "", param_kind::ui, param_type::list, { &active_veffect_items, 0 } } },
  { "{379A6E9F-C7DA-40CE-950B-C51D202F0060}", { { "G.FX", "Global FX" }, "", param_kind::ui, param_type::list, { &active_geffect_items, 0 } } },
  { "{556BF692-55B0-48B4-BD6A-E2CEFA17B012}", { { "Env", "Envelope" }, "", param_kind::ui, param_type::list, { &active_venv_items, 0 } } },
  { "{EEA97414-8C1C-4378-A68B-409692FFA920}", { { "V.LFO", "Voice LFO" }, "", param_kind::ui, param_type::list, { &active_vlfo_items, 0 } } },
  { "{1618ADF6-BE07-44F2-8839-076028834199}", { { "G.LFO", "Global LFO" }, "", param_kind::ui, param_type::list, { &active_glfo_items, 0 } } }
};

// ---- global topo ----

part_descriptor const                                  
part_descriptors[part_type::count] =                                             
{                       
  { "{5C9D2CD3-2D4C-4205-893E-6B5DE9D62ADE}", { "Osc", "Oscillator" }, part_kind::input, part_type::vosc, vosc_count, osc_params, osc_param::count, osc_graph_name_selector },
  { "{2C377544-C124-48F5-A4F4-1E301B108C58}", { "V.FX", "Voice FX" }, part_kind::input, part_type::veffect, veffect_count, veffect_params, effect_param::vfx_count, effect_graph_name_selector },
  { "{E8F67736-5976-4FDE-939F-31C373B7F920}", { "G.FX", "Global FX" }, part_kind::input, part_type::geffect, geffect_count, geffect_params, effect_param::gfx_count, effect_graph_name_selector },
  { "{7A77C027-FC8F-4425-9BF0-393267D92F0C}", { "V.Audio", "Voice Audio" }, part_kind::input, part_type::vaudio_bank, 1, vaudio_bank_params, audio_bank_param::count, nullptr },
  { "{B5B4A442-13ED-43ED-B9E0-3B2894D03838}", { "G.Audio", "Global Audio" }, part_kind::input, part_type::gaudio_bank, 1, gaudio_bank_params, audio_bank_param::count, nullptr },
  { "{E6344937-C1F7-4F2A-83E7-EA27D48DEC4E}", { "V.In", "Voice In" }, part_kind::input, part_type::voice, 1, voice_params, voice_param::count, nullptr },
  { "{6DE3AAB2-6D43-41ED-9BBE-E281DB8F7B44}", { "M.In", "Master In" }, part_kind::input, part_type::master, 1, master_params, master_param::count, nullptr },
  { "{62318254-6650-428D-9BD4-53A11E86D5DE}", { "V.Out", "Voice Out" }, part_kind::input, part_type::vamp, 1, vamp_params, amp_param::count, nullptr },
  { "{083A0CA3-8C8B-4B6A-BB38-3E327239E469}", { "M.Out", "Master Out" }, part_kind::input, part_type::gamp, 1, gamp_params, amp_param::count, nullptr },
  { "{FC4885FE-431C-477A-B5B7-84863DB8C07D}", { "Env", "Envelope" }, part_kind::input, part_type::venv, venv_count, envelope_params, envelope_param::count, env_graph_name_selector },
  { "{56DE75BB-BE73-4B27-B37F-77F6E408F986}", { "V.LFO", "Voice LFO" }, part_kind::input, part_type::vlfo, vlfo_count, vlfo_params, lfo_param::count, lfo_graph_name_selector },
  { "{5BE4D402-BD27-478B-9C14-A570A4306FFA}", { "G.LFO", "Global LFO" }, part_kind::input, part_type::glfo, glfo_count, glfo_params, lfo_param::count, lfo_graph_name_selector },
  { "{E6814824-7F56-4A9C-92B6-F5EB001B9513}", { "V.CV", "Voice CV" }, part_kind::input, part_type::vcv_bank, 1, vcv_bank_params, cv_bank_param::count, nullptr },
  { "{3F416415-4C1E-49B3-A59F-0C4472C11B69}", { "G.CV", "Global CV" }, part_kind::input, part_type::gcv_bank, 1, gcv_bank_params, cv_bank_param::count, nullptr },
  { "{B13B3846-DDDE-4CAE-9641-7C8AEAAA9C01}", { "V.CV Plot", "Voice CV Plot" }, part_kind::input, part_type::vcv_plot, 1, vcv_plot_params, cv_plot_param::count, cv_plot_graph_name_selector },
  { "{30B485EC-0EDC-4792-9ED1-8AE5A3349096}", { "G.CV Plot", "Global CV Plot" }, part_kind::input, part_type::gcv_plot, 1, gcv_plot_params, cv_plot_param::count, cv_plot_graph_name_selector },
  { "{52786612-2524-4F95-93F5-9D0228C96732}", { "Edit", "Edit" }, part_kind::input, part_type::edit_selector, 1, edit_selector_params, edit_selector_param::count, nullptr },
  { "{4EE0F930-E05E-4404-9805-D8C0347F8BB4}", { "Patch", "Patch" }, part_kind::input, part_type::patch, 1, patch_params, patch_param::count, nullptr },
  { "{C972E264-1739-4DB6-B1DB-5D31057BD218}", { "Active", "Active" }, part_kind::selector, part_type::active, 1, active_params, active_param::count, nullptr },
  { "{FEEBA3F5-F248-4C1B-BD8C-F3A492D084E2}", { "Output", "Output" }, part_kind::output, part_type::output, 1, output_params, output_param::count, nullptr }
};         
                                    
} // namespace inf::synth                               