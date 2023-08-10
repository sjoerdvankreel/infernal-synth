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

  // 1.3 changed oscillator cents from +/-50 to +/-100.
  if(old_major < 1 || (old_major == 1 && old_minor < 3))
    for (std::int32_t i = 0; i < vosc_count; i++)
    {
      std::int32_t osc_start = param_bounds[part_type::vosc][i];
      if(index == osc_start + osc_param::cent)
      {
        float old_in_range = real_bounds::linear(-0.5f, 0.5f).to_range(old_value.real);
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
  // Active Audio/CV A/B selectors.
  else if (std::string("{4C161E6D-7487-4D98-86DC-73B2B414A299}") == id.param_guid
    || std::string("{2627D0E5-7522-4BE3-8659-AA526CF2186C}") == id.param_guid
    || std::string("{BFCAD318-CB76-4AF7-9B43-3F1776144813}") == id.param_guid
    || std::string("{A6203077-A2A1-4509-8F99-86A29BC2EE9B}") == id.param_guid)
  {
    can_be_ignored = true;
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
  // CV A enabled dropped.
  else if (std::string("{E6814824-7F56-4A9C-92B6-F5EB001B9513}") == id.part_guid
    && std::string("{1F6DEE15-DEE7-443B-B9F8-E65BFF9C9C4A}") == id.param_guid)
  {
    can_be_ignored = true;
    return -1;
  }
  // CV B enabled dropped.
  else if (std::string("{3F416415-4C1E-49B3-A59F-0C4472C11B69}") == id.part_guid
    && std::string("{C64A2AE2-E96D-487E-8373-3DA9DBB7B028}") == id.param_guid)
  {
    can_be_ignored = true;
    return -1;
  }
  // Audio A moved from 4x6 to 1x15 and transposed.
  else if (std::string("{7A77C027-FC8F-4425-9BF0-393267D92F0C}") == id.part_guid)
  {
    std::int32_t const param_count = 4;
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

    // Can we map to the new matrix ?
    for (std::int32_t i = 0; i < old_route_count; i++)
      if (id.param_guid == old_in[i]
        || id.param_guid == old_out[i]
        || id.param_guid == old_amt[i]
        || id.param_guid == old_bal[i])
      {
        std::int32_t new_route_index = id.part_index * old_route_count + i;
        if(new_route_index >= new_route_count) 
          return -1;
        std::int32_t new_param_index = new_route_index * param_count;
        if(id.param_guid == old_in[i]) new_param_index += 0;
        if (id.param_guid == old_out[i]) new_param_index += 1;
        if (id.param_guid == old_amt[i]) new_param_index += 2;
        if (id.param_guid == old_bal[i]) new_param_index += 3;
        return param_index({ part_type::vaudio_bank, 0 }, new_param_index);
      }

    // If not, don't bother with defaults.
    for (std::int32_t i = 0; i < old_route_count; i++)
      if ((id.param_guid == old_in[i] && old_value.discrete == 0)
        || (id.param_guid == old_out[i] && old_value.discrete == 0)
        || (id.param_guid == old_amt[i] && old_value.real == 0.5f)
        || (id.param_guid == old_bal[i] && old_value.real == 0.5f))
      {
        can_be_ignored = true;
        return -1;
      }

    return -1;
  }
  // Audio B moved from 3x6 to 1x15.
  else if (std::string("{B5B4A442-13ED-43ED-B9E0-3B2894D03838}") == id.part_guid)
  {
    std::int32_t const param_count = 4;
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

    // Can we map to the new matrix ?
    for (std::int32_t i = 0; i < old_route_count; i++)
      if (id.param_guid == old_in[i]
        || id.param_guid == old_out[i]
        || id.param_guid == old_amt[i]
        || id.param_guid == old_bal[i])
      {
        std::int32_t new_route_index = id.part_index * old_route_count + i;
        if (new_route_index >= new_route_count)
          return -1;
        std::int32_t new_param_index = new_route_index * param_count;
        if (id.param_guid == old_in[i]) new_param_index += 0;
        if (id.param_guid == old_out[i]) new_param_index += 1;
        if (id.param_guid == old_amt[i]) new_param_index += 2;
        if (id.param_guid == old_bal[i]) new_param_index += 3;
        return param_index({ part_type::vaudio_bank, 0 }, new_param_index);
      }

    // If not, don't bother with defaults.
    for (std::int32_t i = 0; i < old_route_count; i++)
      if ((id.param_guid == old_in[i] && old_value.discrete == 0)
        || (id.param_guid == old_out[i] && old_value.discrete == 0)
        || (id.param_guid == old_amt[i] && old_value.real == 0.5f)
        || (id.param_guid == old_bal[i] && old_value.real == 0.5f))
      {
        can_be_ignored = true;
        return -1;
      }

    return -1;
  }
  // CV A moved from 4x5 to 1x15.
  else if (std::string("{E6814824-7F56-4A9C-92B6-F5EB001B9513}") == id.part_guid)
  {
    std::int32_t const param_count = 6;
    std::int32_t const old_route_count = 5;
    std::int32_t const new_route_count = 15;
    char const* old_in[old_route_count] = {
      "{3B025C6A-0230-491A-A51F-7CF1C81B69C9}",
      "{2833E378-210B-404F-A4CB-0D6204A72CF0}",
      "{25041AB5-2A06-4305-8009-C26D56311D26}",
      "{B4C3E2E0-106D-4377-93F6-711561D5F1DA}",
      "{DE53604C-CC85-40EA-B0AC-6CB00050EB4C}" };
    char const* old_out[old_route_count] = {
      "{5FDD8C86-8F2D-4613-BB98-BB673F502412}",
      "{D0B28D9E-8888-42EB-8D3C-177FB4585E42}",
      "{37420523-6A9D-4125-BAAB-24A28B9E0992}",
      "{32C5980E-F41A-441C-85A8-3AC90A4AAD0A}",
      "{055B8D75-97ED-47E1-9A81-4FA5A3072E30}" };
    char const* old_op[old_route_count] = {
      "{18EBC834-CF60-4A68-BAF2-C57BC0BAE55E}",
      "{CD10C60A-C25E-46A5-879E-C692E50AE36B}",
      "{C6D7FDD5-102A-4972-B0B2-77F3977C9046}",
      "{927B6CF5-D576-42F9-80BA-4C83437F9041}",
      "{EDA84031-7061-448F-BCEB-6D941D772EFF}" };
    char const* old_amt[old_route_count] = {
      "{469D130F-2E4A-4960-871D-032B6F588313}",
      "{58AFE21F-7945-4919-BB67-60CE8892A8AF}",
      "{9C1F6472-6D48-42E6-B79E-3A00F33F70F5}",
      "{D04F0B81-2E96-42D7-895C-DFC47BA36B4C}",
      "{C0E8DD3E-DCAD-48FA-9C4E-BB0FDDFC2554}" };
    char const* old_off[old_route_count] = {
      "{0C1E5C81-01EE-4AE6-A05C-199210B904CC}",
      "{20F28A5F-2956-412E-BD97-9220836C22A3}",
      "{BFFA2360-5A5F-4575-A0E4-32B8B74977BD}",
      "{B46AC7BD-982C-4A9D-B74B-AE27C25BE811}",
      "{542280B8-942D-4FAA-AB26-5A39CE3FFF71}" };
    char const* old_scl[old_route_count] = {
      "{20ACF437-2158-4900-8DC4-D36767442BF1}",
      "{23EFCBDF-7744-4527-B82D-A69E56C7338B}",
      "{87087F9C-62A2-4804-9292-CD9089F025B7}",
      "{D58E30EB-8F46-4EB7-84F2-37AA48F81721}",
      "{62A7448C-3245-4B33-AF4E-42D98D3AD547}" };

    // Can we map to the new matrix ?
    for (std::int32_t i = 0; i < old_route_count; i++)
      if (id.param_guid == old_in[i]
        || id.param_guid == old_out[i]
        || id.param_guid == old_op[i]
        || id.param_guid == old_amt[i]
        || id.param_guid == old_off[i]
        || id.param_guid == old_scl[i])
      {
        std::int32_t new_route_index = id.part_index * old_route_count + i;
        if (new_route_index >= new_route_count)
          return -1;
        std::int32_t new_param_index = new_route_index * param_count;
        if (id.param_guid == old_in[i]) new_param_index += 0;
        if (id.param_guid == old_out[i]) new_param_index += 1;
        if (id.param_guid == old_op[i]) new_param_index += 2;
        if (id.param_guid == old_amt[i]) new_param_index += 3;
        if (id.param_guid == old_off[i]) new_param_index += 4;
        if (id.param_guid == old_scl[i]) new_param_index += 5;
        return param_index({ part_type::vcv_bank, 0 }, new_param_index);
      }

    // If not, don't bother with defaults.
    for (std::int32_t i = 0; i < old_route_count; i++)
      if ((id.param_guid == old_in[i] && old_value.discrete == 0)
        || (id.param_guid == old_out[i] && old_value.discrete == 0)
        || (id.param_guid == old_op[i] && old_value.discrete == 0)
        || (id.param_guid == old_amt[i] && old_value.discrete == 1.0f)
        || (id.param_guid == old_off[i] && old_value.real == 0.0f)
        || (id.param_guid == old_scl[i] && old_value.real == 1.0f))
      {
        can_be_ignored = true;
        return -1;
      }

    return -1;
  }
  // CV B moved from 2x8 to 1x15.
  else if (std::string("{3F416415-4C1E-49B3-A59F-0C4472C11B69}") == id.part_guid)
  {
    std::int32_t const param_count = 6;
    std::int32_t const old_route_count = 8;
    std::int32_t const new_route_count = 15;

    char const* old_in[old_route_count] = {
      "{84736ED7-FF72-4E69-AFFF-A8607B0F3041}", 
      "{36981D81-6FC1-42C1-A380-D0813C624D93}", 
      "{9517E1DC-8069-4F00-915C-A0686DD3FB26}", 
      "{EB0A819C-174B-41B8-B8E8-E91377441D66}", 
      "{460C97B9-5DA2-40D9-B553-B59784A972B7}", 
      "{C26316CE-3855-44C4-8C0D-0AD3A06443CD}", 
      "{5D4EB303-0B5C-48A3-BC62-79571DD51387}", 
      "{FA7C27DD-3EFB-48A1-96FD-89BD7F195C62}" };
    char const* old_out[old_route_count] = {
      "{4548DE68-1D70-4307-BBD2-09838CAC4701}", 
      "{A5857037-CB45-4F83-AF2F-2C4083628E63}", 
      "{DC95DEF0-DD36-493A-B97A-E580DEB8BEBD}", 
      "{86D57462-7C08-4DD6-8FAC-EEC7BD0ABE34}", 
      "{CF0921FB-E525-4217-A249-CBEDFDB58B72}", 
      "{326AEE47-2AFC-4140-8AD3-A8B1AE1FD1DF}", 
      "{C7A5A80A-5B69-4F42-BD92-57A0FA606DFE}", 
      "{02FE5916-CD9C-4AB9-8D47-25198095090F}" };
    char const* old_op[old_route_count] = {
      "{A526F21F-4C41-4925-90FD-D6BA442225E7}", 
      "{FDFF29BB-E361-4350-9958-8266013A3124}", 
      "{53EE242B-B967-499B-821B-4BF5419E08E5}", 
      "{CD0E58CA-C183-4B59-9F45-2F0805379A1D}", 
      "{2D718791-CF63-4A17-A1E3-EBB1DD675F38}", 
      "{9D3027D8-5146-45A6-BAF7-036DBF3D2345}", 
      "{DF3A60E1-42E1-4E13-AA33-F8C56C877A92}", 
      "{1E668318-1135-4325-A10A-7EC6F0A73D24}" };
    char const* old_amt[old_route_count] = {
      "{A927E108-FB33-4AFC-A46A-726705004F78}", 
      "{98AF1BBD-D01A-41EF-82DC-FD852FD7154B}", 
      "{48F63E35-4D42-4EC1-AC9A-8E0CF2278095}", 
      "{2EC4D924-1912-4111-8AC1-B24D84384618}", 
      "{E8B33588-CF41-44B4-A2CB-A12E520A8A84}", 
      "{9806BBDC-23A5-4038-B4D0-1B541238E915}", 
      "{7BC87BF9-DBFC-4092-A03F-E93292DA7963}", 
      "{B59683B5-2031-46CC-8C95-B5B291320601}" };
    char const* old_off[old_route_count] = {
      "{41F96374-D208-4A29-911E-18CF7C27A6A0}", 
      "{4CBAC51D-9597-48BF-9210-D9CA33E8DCD1}", 
      "{D2EF64CE-1737-4776-AD8D-F28D5E359960}", 
      "{44061640-FDE6-4E81-BDE0-6145A6EBD494}", 
      "{31E160CE-5424-42B6-858E-07FC00C5B502}", 
      "{72E153D2-4614-4F91-B348-DF7AE0453927}", 
      "{31D9A605-F16B-47AB-B5F4-C03B285D7EDA}", 
      "{9A949AC3-CFF8-49CC-81BC-78B59B2E4F8A}" };
    char const* old_scl[old_route_count] = {
      "{5ACBDB05-BFCF-4A85-809B-D81DB8E835DE}", 
      "{2E03B188-2616-457B-ACC2-F1B735370420}", 
      "{45AE23A8-8EC6-4901-BECE-CB631FDFAA4D}", 
      "{D2937D43-3DEE-4657-A9E0-BD2B1323FA3A}", 
      "{9B15F803-4776-459D-98D8-B5563F217159}", 
      "{87F14A8F-2462-4CAE-9E8F-A87F95A20369}", 
      "{DF70143D-441E-4C77-BCF5-6535B4316B64}", 
      "{12EE4A5C-1BA4-4DC5-80B7-0FEF76EA5A59}" };

    // Can we map to the new matrix ?
    for (std::int32_t i = 0; i < old_route_count; i++)
      if (id.param_guid == old_in[i]
        || id.param_guid == old_out[i]
        || id.param_guid == old_op[i]
        || id.param_guid == old_amt[i]
        || id.param_guid == old_off[i]
        || id.param_guid == old_scl[i])
      {
        std::int32_t new_route_index = id.part_index * old_route_count + i;
        if (new_route_index >= new_route_count)
          return -1;
        std::int32_t new_param_index = new_route_index * param_count;
        if (id.param_guid == old_in[i]) new_param_index += 0;
        if (id.param_guid == old_out[i]) new_param_index += 1;
        if (id.param_guid == old_op[i]) new_param_index += 2;
        if (id.param_guid == old_amt[i]) new_param_index += 3;
        if (id.param_guid == old_off[i]) new_param_index += 4;
        if (id.param_guid == old_scl[i]) new_param_index += 5;
        return param_index({ part_type::gcv_bank, 0 }, new_param_index);
      }

    // If not, don't bother with defaults.
    for (std::int32_t i = 0; i < old_route_count; i++)
      if ((id.param_guid == old_in[i] && old_value.discrete == 0)
        || (id.param_guid == old_out[i] && old_value.discrete == 0)
        || (id.param_guid == old_op[i] && old_value.discrete == 0)
        || (id.param_guid == old_amt[i] && old_value.discrete == 1.0f)
        || (id.param_guid == old_off[i] && old_value.real == 0.0f)
        || (id.param_guid == old_scl[i] && old_value.real == 1.0f))
      {
        can_be_ignored = true;
        return -1;
      }

    return -1;
  }
  // These are not in 1.1.3. No idea what they are. Just hope for the best.
  else if (std::string("{D316B9BA-4107-49B1-AAC1-864DE2B5A209}") == id.param_guid
    || std::string("{48F1B532-4837-4D52-A95D-3A1641F4E761}") == id.param_guid
    || std::string("{78C0C624-B34B-41AE-8294-5D99895753CB}") == id.param_guid
    || std::string("{07E509BE-891C-48A8-9076-81D9DE8B6845}") == id.param_guid
    || std::string("{06826D77-66A2-44F0-9AE8-DFA7035C95D8}") == id.param_guid)
  {
    can_be_ignored = true;
    return -1;
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
  topology_info::init_clear_patch(state);

  // Bare minimum to have a basic delay line.
  set_ui_value(state, part_type::geffect, 0, effect_param::on, "On");
  set_ui_value(state, part_type::geffect, 0, effect_param::type, "Delay");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in1, "Ext");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out1, "G.FX 1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in2, "G.FX 1");
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
  // Multi-filtered delay line.
  topology_info::init_factory_preset(state);

  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in1, "Ext");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out1, "M.Out");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in2, "Ext");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out2, "G.FX 1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in3, "G.FX 1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out3, "G.FX 2");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in4, "G.FX 2");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out4, "M.Out");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in5, "Ext");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out5, "G.FX 3");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in6, "G.FX 3");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out6, "G.FX 4");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in7, "G.FX 4");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out7, "M.Out");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in8, "Ext");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out8, "G.FX 5");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in9, "G.FX 5");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out9, "G.FX 6");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in10, "G.FX 6");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out10, "M.Out");

  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::in1, "G.LFO 1");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::out1, "G.FX 2 StVar Frq");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::amt1, "10");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::in2, "G.LFO 2");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::out2, "G.FX 4 StVar Frq");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::amt2, "20");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::in3, "G.LFO 3");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::out3, "G.FX 6 StVar Frq");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::amt3, "30");

  set_ui_value(state, part_type::geffect, 0, effect_param::on, "On");
  set_ui_value(state, part_type::geffect, 0, effect_param::type, "Delay");
  set_ui_value(state, part_type::geffect, 0, effect_param::delay_type, "Multitap");
  set_ui_value(state, part_type::geffect, 0, effect_param::dly_amt, "50");
  set_ui_value(state, part_type::geffect, 0, effect_param::dly_mix, "100");
  set_ui_value(state, part_type::geffect, 0, effect_param::dly_synced, "On");
  set_ui_value(state, part_type::geffect, 0, effect_param::dly_hold_tempo, "0");
  set_ui_value(state, part_type::geffect, 0, effect_param::dly_multi_taps, "6");
  set_ui_value(state, part_type::geffect, 0, effect_param::dly_multi_tempo, "3/16");
  set_ui_value(state, part_type::geffect, 0, effect_param::dly_multi_sprd, "33");

  set_ui_value(state, part_type::geffect, 1, effect_param::on, "On");
  set_ui_value(state, part_type::geffect, 1, effect_param::type, "Filter");
  set_ui_value(state, part_type::geffect, 1, effect_param::filter_type, "StVar");
  set_ui_value(state, part_type::geffect, 1, effect_param::flt_stvar_type, "BPF");
  set_ui_value(state, part_type::geffect, 1, effect_param::flt_stvar_res, "75");
  set_ui_value(state, part_type::geffect, 1, effect_param::flt_stvar_freq, "250");

  set_ui_value(state, part_type::geffect, 2, effect_param::on, "On");
  set_ui_value(state, part_type::geffect, 2, effect_param::type, "Delay");
  set_ui_value(state, part_type::geffect, 2, effect_param::delay_type, "Multitap");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_amt, "50");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_mix, "100");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_synced, "On");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_hold_tempo, "1/16");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_multi_taps, "6");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_multi_tempo, "3/16");
  set_ui_value(state, part_type::geffect, 0, effect_param::dly_multi_sprd, "0");

  set_ui_value(state, part_type::geffect, 3, effect_param::on, "On");
  set_ui_value(state, part_type::geffect, 3, effect_param::type, "Filter");
  set_ui_value(state, part_type::geffect, 3, effect_param::filter_type, "StVar");
  set_ui_value(state, part_type::geffect, 3, effect_param::flt_stvar_type, "BPF");
  set_ui_value(state, part_type::geffect, 3, effect_param::flt_stvar_res, "75");
  set_ui_value(state, part_type::geffect, 3, effect_param::flt_stvar_freq, "1000");

  set_ui_value(state, part_type::geffect, 4, effect_param::on, "On");
  set_ui_value(state, part_type::geffect, 4, effect_param::type, "Delay");
  set_ui_value(state, part_type::geffect, 4, effect_param::delay_type, "Multitap");
  set_ui_value(state, part_type::geffect, 4, effect_param::dly_amt, "50");
  set_ui_value(state, part_type::geffect, 4, effect_param::dly_mix, "100");
  set_ui_value(state, part_type::geffect, 4, effect_param::dly_synced, "On");
  set_ui_value(state, part_type::geffect, 4, effect_param::dly_hold_tempo, "1/8");  
  set_ui_value(state, part_type::geffect, 4, effect_param::dly_multi_taps, "6");
  set_ui_value(state, part_type::geffect, 4, effect_param::dly_multi_tempo, "3/16");
  set_ui_value(state, part_type::geffect, 0, effect_param::dly_multi_sprd, "-33");

  set_ui_value(state, part_type::geffect, 5, effect_param::on, "On");
  set_ui_value(state, part_type::geffect, 5, effect_param::type, "Filter");
  set_ui_value(state, part_type::geffect, 5, effect_param::filter_type, "StVar");
  set_ui_value(state, part_type::geffect, 5, effect_param::flt_stvar_type, "BPF");
  set_ui_value(state, part_type::geffect, 5, effect_param::flt_stvar_res, "75");
  set_ui_value(state, part_type::geffect, 5, effect_param::flt_stvar_freq, "3000");

  set_ui_value(state, part_type::glfo, 0, lfo_param::on, "On");
  set_ui_value(state, part_type::glfo, 0, lfo_param::synced, "On");
  set_ui_value(state, part_type::glfo, 0, lfo_param::tempo, "5/16");

  set_ui_value(state, part_type::glfo, 1, lfo_param::on, "On");
  set_ui_value(state, part_type::glfo, 1, lfo_param::synced, "On");
  set_ui_value(state, part_type::glfo, 1, lfo_param::tempo, "7/16");

  set_ui_value(state, part_type::glfo, 2, lfo_param::on, "On");
  set_ui_value(state, part_type::glfo, 2, lfo_param::synced, "On");
  set_ui_value(state, part_type::glfo, 2, lfo_param::tempo, "9/16");
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
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::out1, "V.FX 1");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::in2, "V.FX 1");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::out2, "V.FX 2");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::in3, "V.FX 2");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::out3, "V.Out");

  // gaudio 1 voice->fx1->fx2->master
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in1, "V.Out");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out1, "G.FX 1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in2, "G.FX 1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out2, "G.FX 2");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in3, "G.FX 2");
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
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::out3, "V.FX 2 StVar Frq");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::op3, "Mul");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::in4, "CVU 2");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::out4, "V.FX 2 StVar Frq");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::off4, "20");
  set_ui_value(state, part_type::vcv_bank, 0, cv_bank_param::op4, "Mul");

  // glfo 1 to filter freq & master cvs
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::in1, "G.LFO 1");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::out1, "G.FX 1 StVar Frq");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::op1, "Add");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::amt1, "66");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::in2, "CVU 1");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::out2, "M.Out Gain");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::op2, "Mul");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::in3, "CVB 1");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::out3, "M.Out Bal");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::op3, "Add");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::in4, "CVU 2");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::out4, "G.FX 1 StVar Frq");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::op4, "Mul");
  set_ui_value(state, part_type::gcv_bank, 0, cv_bank_param::off4, "20");

  // cv plots
  set_ui_value(state, part_type::gcv_plot, 0, cv_plot_param::length, "5");
  set_ui_value(state, part_type::gcv_plot, 0, cv_plot_param::target, "G.FX 1 StVar Frq");
  set_ui_value(state, part_type::vcv_plot, 0, cv_plot_param::target, "V.FX 2 StVar Frq");
}

// ---- params ----

static std::vector<list_item> const edit_selector_types = {
{ "{73A12F62-82E3-4FCF-B507-2DF2BB00521C}", "Edit Voice" },
{ "{F1C6C636-BDCB-48C8-8BBC-AAD2ECF5AD0D}", "Edit Global" } };

param_descriptor const
master_params[master_param::count] =
{
  { "{50F02184-1EAC-4A80-8832-0728B9EBF455}", { { "CVU 1", "CVU 1" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{D8332EF2-E4CE-4605-983A-6EC409194773}", { { "CVB 1", "CVB 1" }, "%", param_kind::continuous, percentage_m11_bounds(0.0f) } },
  { "{060B5E5E-2E49-4DF1-90B8-8D8F87E98707}", { { "CVU 2", "CVU 2" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{580FF8F1-4C06-4562-B4A0-702355B6E152}", { { "CVB 2", "CVB 2" }, "%", param_kind::continuous, percentage_m11_bounds(0.0f) } },
  { "{DD9A20AD-563A-4855-BAEF-2C53E6B94815}", { { "CVU 3", "CVU 3" }, "%", param_kind::continuous, percentage_01_bounds(1.0f) } },
  { "{B3B033A3-B615-4AD2-81AB-5CB769891BB0}", { { "CVB 3", "CVB 3" }, "%", param_kind::continuous, percentage_m11_bounds(0.0f) } },
  { "{632B9205-8EEC-4CB4-8071-B101195DCB80}", { { "PB", "MIDI Pitchbend" }, "%", param_kind::fixed, percentage_m11_bounds(0.0f) } }
};

param_descriptor const
edit_selector_params[edit_selector_param::count] =
{
  { "{5B22861B-88F6-446C-AA92-BA6B39712342}", { { "Edit", "Edit" }, "", param_kind::ui, param_type::list, { &edit_selector_types, edit_selector_type::edit_voice } } }
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
  { "{C972E264-1739-4DB6-B1DB-5D31057BD218}", { "Active", "Active" }, part_kind::selector, part_type::active, 1, active_params, active_param::count, nullptr },
  { "{FEEBA3F5-F248-4C1B-BD8C-F3A492D084E2}", { "Output", "Output" }, part_kind::output, part_type::output, 1, output_params, output_param::count, nullptr }
};         
                                    
} // namespace inf::synth                               