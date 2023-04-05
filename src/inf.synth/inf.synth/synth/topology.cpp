#include <inf.synth/lfo/graph.hpp>
#include <inf.synth/lfo/topology.hpp>
#include <inf.synth/cv_bank/graph.hpp>
#include <inf.synth/effect/graph1.hpp>
#include <inf.synth/effect/graph2.hpp>
#include <inf.synth/synth/topology.hpp>
#include <inf.synth/envelope/graph.hpp>
#include <inf.synth/envelope/topology.hpp>
#include <inf.synth/effect/topology.hpp>
#include <inf.synth/output/topology.hpp>
#include <inf.synth/envelope/topology.hpp>
#include <inf.synth/synth/processor.hpp>
#include <inf.synth/voice_master/graph.hpp>
#include <inf.synth/oscillator/graph_wave.hpp>
#include <inf.synth/oscillator/graph_spectrum.hpp>
 
using namespace inf::base;

namespace inf::synth {

static part_ui_colors make_part_ui_colors();

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
  case part_type::voice: case part_type::master: return std::make_unique<amp_bal_graph>(this, id);
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
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::on, "On");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in1, "Ext");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out1, "FX B1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in2, "FX B1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out2, "Master");
}

void
synth_topology::init_instrument_clear_patch(param_value* state) const
{
  topology_info::init_clear_patch(state);

  // Bare minimum to have sound.
  set_ui_value(state, part_type::vosc, 0, osc_param::on, "On");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::on, "On");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::in1, "Osc 1");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::out1, "Voice");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::on, "On");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in1, "Voice");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out1, "Master");
}

void 
synth_topology::init_fx_factory_preset(param_value* state) const
{
  topology_info::init_factory_preset(state);

  // fx b1 reverb
  set_ui_value(state, part_type::geffect, 0, effect_param::on, "On");
  set_ui_value(state, part_type::geffect, 0, effect_param::type, "Reverb");
  set_ui_value(state, part_type::geffect, 0, effect_param::reverb_mix, "100");
  set_ui_value(state, part_type::geffect, 0, effect_param::reverb_damp, "99");
  set_ui_value(state, part_type::geffect, 0, effect_param::reverb_size, "99");
  set_ui_value(state, part_type::geffect, 0, effect_param::reverb_spread, "33");
  set_ui_value(state, part_type::geffect, 0, effect_param::reverb_apf, "10");

  // fx b3 delay
  set_ui_value(state, part_type::geffect, 2, effect_param::on, "On");
  set_ui_value(state, part_type::geffect, 2, effect_param::type, "Delay");
  set_ui_value(state, part_type::geffect, 2, effect_param::delay_type, "Feedback");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_mix, "100");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_synced, "On");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_amt, "67");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_fdbk_sprd, "75");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_fdbk_tempo_l, "3/16");
  set_ui_value(state, part_type::geffect, 2, effect_param::dly_fdbk_tempo_r, "5/16");

  // fx b bell filters
  for (std::int32_t i = 1; i <= 3; i += 2)
  {
    set_ui_value(state, part_type::geffect, i, effect_param::on, "On");
    set_ui_value(state, part_type::geffect, i, effect_param::type, "Filter");
    set_ui_value(state, part_type::geffect, i, effect_param::filter_type, "State Variable");
    set_ui_value(state, part_type::geffect, i, effect_param::flt_stvar_type, "BLL");
    set_ui_value(state, part_type::geffect, i, effect_param::flt_stvar_shlf_gain, "24");
    set_ui_value(state, part_type::geffect, i, effect_param::flt_stvar_freq, "2000");
  }

  // audio routing
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::on, "On");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in1, "Ext");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out1, "FX B1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in2, "FX B1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out2, "FX B2");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in3, "FX B2");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out3, "Master");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in4, "Ext");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out4, "FX B3");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in5, "FX B3");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out5, "FX B4");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in6, "FX B4");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out6, "Master");
  set_ui_value(state, part_type::gaudio_bank, 1, audio_bank_param::on, "On");
  set_ui_value(state, part_type::gaudio_bank, 1, audio_bank_param::in1, "Ext");
  set_ui_value(state, part_type::gaudio_bank, 1, audio_bank_param::out1, "Master");

  // master
  set_ui_value(state, part_type::master, 0, master_param::gain, "-18");
  set_ui_value(state, part_type::master, 0, master_param::gcv1_uni, "10");
  set_ui_value(state, part_type::master, 0, master_param::gcv1_bi, "-10");
  set_ui_value(state, part_type::master, 0, master_param::gcv2_bi, "33");

  // lfo b1
  set_ui_value(state, part_type::glfo, 0, lfo_param::on, "On");
  set_ui_value(state, part_type::glfo, 0, lfo_param::synced, "On");
  set_ui_value(state, part_type::glfo, 0, lfo_param::tempo, "3/2");
  set_ui_value(state, part_type::glfo, 0, lfo_param::bipolar, "On");

  // cv routing
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::on, "On");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in1, "CV U1");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out1, "Audio B1 Amt3");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op1, "Mul");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in2, "CV B1");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out2, "FX B2 SV Frq");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op2, "Add");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in3, "CV U2");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out3, "Audio B1 Amt6");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op3, "Mul");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in4, "CV B2");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out4, "FX B4 SV Frq");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op4, "Add");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in5, "LFO B1");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out5, "FX B2 SV Frq");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::amt5, "50");

  // cv plot
  set_ui_value(state, part_type::gcv_plot, 0, cv_plot_param::length, "10");
  set_ui_value(state, part_type::gcv_plot, 0, cv_plot_param::target, "FX B2 SV Frq");
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

  // fx a1 tanh shaper
  set_ui_value(state, part_type::veffect, 0, osc_param::on, "On");
  set_ui_value(state, part_type::veffect, 0, effect_param::type, "Shape");
  set_ui_value(state, part_type::veffect, 0, effect_param::shaper_type, "Tanh");
  set_ui_value(state, part_type::veffect, 0, effect_param::shp_gain, "4");

  // fx a2 state variable filter
  set_ui_value(state, part_type::veffect, 1, osc_param::on, "On");
  set_ui_value(state, part_type::veffect, 1, effect_param::type, "Filter");
  set_ui_value(state, part_type::veffect, 1, effect_param::filter_type, "State Variable");
  set_ui_value(state, part_type::veffect, 1, effect_param::flt_stvar_kbd, "50");
  set_ui_value(state, part_type::veffect, 1, effect_param::flt_stvar_res, "50");
  set_ui_value(state, part_type::veffect, 1, effect_param::flt_stvar_freq, "7500");

  // fx b1 state variable filter
  set_ui_value(state, part_type::geffect, 0, osc_param::on, "On");
  set_ui_value(state, part_type::geffect, 0, effect_param::type, "Filter");
  set_ui_value(state, part_type::geffect, 0, effect_param::filter_type, "State Variable");
  set_ui_value(state, part_type::geffect, 0, effect_param::flt_stvar_freq, "7500");
  set_ui_value(state, part_type::geffect, 0, effect_param::flt_stvar_res, "50");

  // fx b2 multitap delay
  set_ui_value(state, part_type::geffect, 1, osc_param::on, "On");
  set_ui_value(state, part_type::geffect, 1, effect_param::type, "Delay");
  set_ui_value(state, part_type::geffect, 1, effect_param::delay_type, "Multitap");
  set_ui_value(state, part_type::geffect, 1, effect_param::dly_synced, "On");
  set_ui_value(state, part_type::geffect, 1, effect_param::dly_amt, "33");
  set_ui_value(state, part_type::geffect, 1, effect_param::dly_multi_sprd, "50");
  set_ui_value(state, part_type::geffect, 1, effect_param::dly_multi_tempo, "3/16");

  // master gain and cvs
  set_ui_value(state, part_type::master, 0, master_param::gain, "3.0");
  set_ui_value(state, part_type::master, 0, master_param::gcv1_uni, "50");
  set_ui_value(state, part_type::master, 0, master_param::gcv2_uni, "66");

  // audio a1 osc->fx1->fx2->voice
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::on, "On");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::in1, "Osc All");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::out1, "FX A1");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::in2, "FX A1");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::out2, "FX A2");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::in3, "FX A2");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::out3, "Voice");

  // audio b1 voice->fx1->fx2->master
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::on, "On");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in1, "Voice");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out1, "FX B1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in2, "FX B1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out2, "FX B2");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in3, "FX B2");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out3, "Master");

  // lfo a1 on & bipolar
  set_ui_value(state, part_type::vlfo, 0, lfo_param::on, "On");
  set_ui_value(state, part_type::vlfo, 0, lfo_param::rate, "15");
  set_ui_value(state, part_type::vlfo, 0, lfo_param::bipolar, "On");

  // lfo b1 on & bipolar & synced
  set_ui_value(state, part_type::glfo, 0, lfo_param::on, "On");
  set_ui_value(state, part_type::glfo, 0, lfo_param::synced, "On");
  set_ui_value(state, part_type::glfo, 0, lfo_param::tempo, "7/4");
  set_ui_value(state, part_type::glfo, 0, lfo_param::bipolar, "On");
  set_ui_value(state, part_type::glfo, 0, lfo_param::invert, "On");
  set_ui_value(state, part_type::glfo, 0, lfo_param::type, "Rnd");
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

  // cv a velo to voice amp, lfo a1 to osc 1 cent, env 2 to filter freq, cvu 2 to filter freq
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::on, "On");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::op1, "Mul");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::in1, "Velocity");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::out1, "Osc 1 Gain");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::op2, "Add");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::in2, "LFO A1");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::out2, "Osc 1 Cent");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::amt2, "10");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::in3, "Env 2");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::out3, "FX A2 SV Frq");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::op3, "Mul");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::in4, "CV U2");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::out4, "FX A2 SV Frq");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::off4, "20");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::op4, "Mul");

  // lfo b1 to filter freq & master cvs
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::on, "On");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in1, "LFO B1");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out1, "FX B1 SV Frq");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op1, "Add");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::amt1, "66");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in2, "CV U1");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out2, "Master Gain");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op2, "Mul");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in3, "CV B1");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out3, "Master Bal");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op3, "Add");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in4, "CV U2");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out4, "FX B1 SV Frq");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op4, "Mul");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::off4, "20");

  // cv plots
  set_ui_value(state, part_type::gcv_plot, 0, cv_plot_param::length, "5");
  set_ui_value(state, part_type::gcv_plot, 0, cv_plot_param::target, "FX B1 SV Frq");
  set_ui_value(state, part_type::vcv_plot, 0, cv_plot_param::target, "FX A2 SV Frq");
}

// ---- part selector ----
 
struct active_param_t { enum value { 
  vosc, veffect, geffect, vaudio_bank, gaudio_bank, 
  venv, vlfo, glfo, vcv_bank, gcv_bank, count }; };
typedef active_param_t::value active_param;

static std::vector<std::string> const active_vosc_tab_items = list_tab_items(nullptr, vosc_count);
static std::vector<std::string> const active_veffect_tab_items = list_tab_items(nullptr, veffect_count);
static std::vector<std::string> const active_geffect_tab_items = list_tab_items(nullptr, geffect_count);
static std::vector<std::string> const active_vaudio_bank_tab_items = list_tab_items(nullptr, vaudio_bank_count);
static std::vector<std::string> const active_gaudio_bank_tab_items = list_tab_items(nullptr, gaudio_bank_count);
static std::vector<std::string> const active_venv_tab_items = list_tab_items(envelope_item_info, venv_count);
static std::vector<std::string> const active_vlfo_tab_items = list_tab_items(nullptr, vlfo_count);
static std::vector<std::string> const active_glfo_tab_items = list_tab_items(nullptr, glfo_count);
static std::vector<std::string> const active_vcv_bank_tab_items = list_tab_items(nullptr, vcv_bank_count);
static std::vector<std::string> const active_gcv_bank_tab_items = list_tab_items(nullptr, gcv_bank_count);

static std::vector<list_item> const active_vosc_items = list_items({ "{3B2713C0-6DFF-4257-9CB4-DF5FD0DA603C}", "Osc" }, nullptr, true, vosc_count);
static std::vector<list_item> const active_veffect_items = list_items({ "{33BB6F1B-3D4F-4D58-998D-417DA711D6C2}", "FX A" }, nullptr, false, veffect_count);
static std::vector<list_item> const active_geffect_items = list_items({ "{1607D775-53C5-46BE-957A-F697831062CD}", "FX B" }, nullptr, false, geffect_count);
static std::vector<list_item> const active_vaudio_bank_items = list_items({ "{8AD2E3E5-0DCF-42E3-B9B6-33F775DF917F}", "Audio A" }, nullptr, false, vaudio_bank_count);
static std::vector<list_item> const active_gaudio_bank_items = list_items({ "{D14108D3-FC7A-49D5-A4CB-AB66BE6F1B1E}", "Audio B" }, nullptr, false, gaudio_bank_count);
static std::vector<list_item> const active_venv_items = list_items({ "{8545D490-B40D-4154-BFC2-FE86A6F26BB5}", "Env" }, envelope_item_info, true, venv_count);
static std::vector<list_item> const active_vlfo_items = list_items({ "{5AADD04D-FC8B-4326-97B1-35326F645D09}", "LFO A" }, nullptr, false, vlfo_count);
static std::vector<list_item> const active_glfo_items = list_items({ "{88E92673-4FA2-45C3-B1D0-71A2A63ECCF4}", "LFO B" }, nullptr, false, glfo_count);
static std::vector<list_item> const active_vcv_bank_items = list_items({ "{9C450189-3395-4E03-8EB8-1788BE94042F}", "CV A" }, nullptr, false, vcv_bank_count);
static std::vector<list_item> const active_gcv_bank_items = list_items({ "{8F6E11A2-C364-4301-9D0D-135909C4312B}", "CV B" }, nullptr, false, gcv_bank_count);
 
static param_descriptor const 
active_params[active_param::count] =     
{
  { "{0EEADBD0-D37B-4B53-A2D5-F22E2154F2D8}", { { "Osc", "Oscillator" }, "", param_kind::ui, param_type::list, { &active_vosc_items, &active_vosc_tab_items }, 0, nullptr } },
  { "{D5DD0DFC-AC9D-42E6-9D2B-924786382825}", { { "FX A", "FX A" }, "", param_kind::ui, param_type::list, { &active_veffect_items, &active_veffect_tab_items }, 0, nullptr } },
  { "{379A6E9F-C7DA-40CE-950B-C51D202F0060}", { { "FX B", "FX B" }, "", param_kind::ui, param_type::list, { &active_geffect_items, &active_geffect_tab_items }, 0, nullptr } },
  { "{4C161E6D-7487-4D98-86DC-73B2B414A299}", { { "Audio A", "Audio Bank A" }, "", param_kind::ui, param_type::list, { &active_vaudio_bank_items, &active_vaudio_bank_tab_items }, 0, nullptr } },
  { "{2627D0E5-7522-4BE3-8659-AA526CF2186C}", { { "Audio B", "Audio Bank B" }, "", param_kind::ui, param_type::list, { &active_gaudio_bank_items, &active_gaudio_bank_tab_items }, 0, nullptr } },
  { "{556BF692-55B0-48B4-BD6A-E2CEFA17B012}", { { "Env", "Envelope" }, "", param_kind::ui, param_type::list, { &active_venv_items, &active_venv_tab_items }, 0, nullptr } },
  { "{EEA97414-8C1C-4378-A68B-409692FFA920}", { { "LFO A", "LFO A" }, "", param_kind::ui, param_type::list, { &active_vlfo_items, &active_vlfo_tab_items }, 0, nullptr } },
  { "{1618ADF6-BE07-44F2-8839-076028834199}", { { "LFO B", "LFO B" }, "", param_kind::ui, param_type::list, { &active_glfo_items, &active_glfo_tab_items }, 0, nullptr } },
  { "{BFCAD318-CB76-4AF7-9B43-3F1776144813}", { { "CV A", "CV Bank A" }, "", param_kind::ui, param_type::list, { &active_vcv_bank_items, &active_vcv_bank_tab_items }, 0, nullptr } },
  { "{A6203077-A2A1-4509-8F99-86A29BC2EE9B}", { { "CV B", "CV Bank B" }, "", param_kind::ui, param_type::list, { &active_gcv_bank_items, &active_gcv_bank_tab_items }, 0, nullptr } },
};

// ---- global topo ----

static part_ui_colors const part_colors = make_part_ui_colors();
 
static special_params_descriptor const vlfo_special = { lfo_param::on, 0, active_param::vlfo };
static special_params_descriptor const glfo_special = { lfo_param::on, 0, active_param::glfo };
static special_params_descriptor const vosc_special = { osc_param::on, 0, active_param::vosc };
static special_params_descriptor const venv_special = { envelope_param::on, 1, active_param::venv };
static special_params_descriptor const vcv_special = { vcv_bank_param::on, 0, active_param::vcv_bank }; 
static special_params_descriptor const gcv_special = { gcv_bank_param::on, 0, active_param::gcv_bank };
static special_params_descriptor const veffect_special = { effect_param::on, 0, active_param::veffect };
static special_params_descriptor const geffect_special = { effect_param::on, 0, active_param::geffect };
static special_params_descriptor const vaudio_bank_special = { audio_bank_param::on, 0, active_param::vaudio_bank };      
static special_params_descriptor const gaudio_bank_special = { audio_bank_param::on, 1, active_param::gaudio_bank };                   
           
static part_ui_descriptor const vosc_ui = { 4, vosc_special, osc_graphs, part_colors, osc_borders, nullptr, connector_direction::right, "background_oscillator.png", 50, "", true};
static part_ui_descriptor const veffect_ui = { 4, veffect_special, effect_graphs, part_colors, effect_borders, nullptr, connector_direction::none, "background_fx_a.png", 200, "", false};
static part_ui_descriptor const geffect_ui = { 4, geffect_special, effect_graphs, part_colors, effect_borders, nullptr, connector_direction::none, "background_fx_b.png", 100, "Global", false };
static part_ui_descriptor const vaudio_bank_ui = { 4, vaudio_bank_special, { }, part_colors, audio_bank_borders(), &audio_bank_table, connector_direction::left_right, "background_audio_a.png", 250, "", false};
static part_ui_descriptor const gaudio_bank_ui = { 4, gaudio_bank_special, { }, part_colors, audio_bank_borders(), &audio_bank_table, connector_direction::left_right | connector_direction::down, "background_audio_b.png", 150, "Global", false};
static part_ui_descriptor const voice_ui = { 4, part_no_special, voice_graphs, part_colors, voice_borders, nullptr, connector_direction::down, "background_voice.png", 150, "", true};
static part_ui_descriptor const master_ui = { 4, part_no_special, master_graphs, part_colors, master_borders, nullptr, connector_direction::left, "background_master.png", 200, "Global", true};
static part_ui_descriptor const venv_ui = { 4, venv_special, envelope_graphs, part_colors, envelope_borders, nullptr, connector_direction::up, "background_envelope.png", 200, "", true };
static part_ui_descriptor const vlfo_ui = { 4, vlfo_special, lfo_graphs, part_colors, lfo_borders, nullptr,  connector_direction::right, "background_lfo_a.png", 150, "", false };
static part_ui_descriptor const glfo_ui = { 4, glfo_special, lfo_graphs, part_colors, lfo_borders, nullptr, connector_direction::none, "background_lfo_b.png", 150, "Global", false };
static part_ui_descriptor const vcv_bank_ui = { 4, vcv_special, { }, part_colors, cv_bank_borders(vcv_bank_param::count), &cv_bank_table, connector_direction::up, "background_cv_a.png", 150, "", false};
static part_ui_descriptor const gcv_bank_ui = { 4, gcv_special, { }, part_colors, cv_bank_borders(gcv_bank_param::count), &cv_bank_table, connector_direction::up | connector_direction::halfway_left_up, "background_cv_b.png", 100, "Global", false};
static part_ui_descriptor const vcv_plot_ui = { 4, part_no_special, cv_plot_graph_descs, part_colors, cv_plot_borders, nullptr, connector_direction::none, "background_cv_a_plot.png", 175, "", true };
static part_ui_descriptor const gcv_plot_ui = { 4, part_no_special, cv_plot_graph_descs, part_colors, cv_plot_borders, nullptr, connector_direction::none, "background_cv_b_plot.png", 200, "Global", true};
static part_ui_descriptor const output_ui = { 4, part_no_special, { }, part_colors, output_borders, &output_table, connector_direction::down, "background_output.png", 275, "Global", true};
                   
part_descriptor const                                  
part_descriptors[part_type::count] =                                             
{                       
  { "{5C9D2CD3-2D4C-4205-893E-6B5DE9D62ADE}", { "Osc", "Oscillator" }, part_kind::input, part_type::vosc, vosc_count, osc_params, osc_param::count, 0, &vosc_ui },
  { "{2C377544-C124-48F5-A4F4-1E301B108C58}", { "FX A", "FX A" }, part_kind::input, part_type::veffect, veffect_count, veffect_params, effect_param::vfx_count, 10, &veffect_ui}, 
  { "{E8F67736-5976-4FDE-939F-31C373B7F920}", { "FX B", "FX B" }, part_kind::input, part_type::geffect, geffect_count, geffect_params, effect_param::gfx_count, 11, &geffect_ui},
  { "{7A77C027-FC8F-4425-9BF0-393267D92F0C}", { "Audio A", "Audio Bank A" }, part_kind::input, part_type::vaudio_bank, vaudio_bank_count, vaudio_bank_params, audio_bank_param::count, 5, &vaudio_bank_ui },
  { "{B5B4A442-13ED-43ED-B9E0-3B2894D03838}", { "Audio B", "Audio Bank B" }, part_kind::input, part_type::gaudio_bank, gaudio_bank_count, gaudio_bank_params, audio_bank_param::count, 7, &gaudio_bank_ui },
  { "{E6344937-C1F7-4F2A-83E7-EA27D48DEC4E}", { "Voice", "Voice" }, part_kind::input, part_type::voice, 1, voice_params, voice_param::count, 6, &voice_ui },
  { "{6DE3AAB2-6D43-41ED-9BBE-E281DB8F7B44}", { "Master", "Master" }, part_kind::input, part_type::master, 1, master_params, master_param::count, 1, &master_ui },
  { "{FC4885FE-431C-477A-B5B7-84863DB8C07D}", { "Env", "Envelope" }, part_kind::input, part_type::venv, venv_count, envelope_params, envelope_param::count, 14, &venv_ui },
  { "{56DE75BB-BE73-4B27-B37F-77F6E408F986}", { "LFO A", "LFO A" }, part_kind::input, part_type::vlfo, vlfo_count, vlfo_params, lfo_param::count, 8, &vlfo_ui },
  { "{5BE4D402-BD27-478B-9C14-A570A4306FFA}", { "LFO B", "LFO B" }, part_kind::input, part_type::glfo, glfo_count, glfo_params, lfo_param::count, 9, &glfo_ui },
  { "{E6814824-7F56-4A9C-92B6-F5EB001B9513}", { "CV A", "CV Bank A" }, part_kind::input, part_type::vcv_bank, vcv_bank_count, vcv_bank_params, vcv_bank_param::count, 13, &vcv_bank_ui },
  { "{3F416415-4C1E-49B3-A59F-0C4472C11B69}", { "CV B", "CV Bank B" }, part_kind::input, part_type::gcv_bank, gcv_bank_count, gcv_bank_params, gcv_bank_param::count, 4, &gcv_bank_ui },
  { "{B13B3846-DDDE-4CAE-9641-7C8AEAAA9C01}", { "CV A Plot", "CV Bank A Plot" }, part_kind::input, part_type::vcv_plot, 1, vcv_plot_params, cv_plot_param::count, 12, &vcv_plot_ui },
  { "{30B485EC-0EDC-4792-9ED1-8AE5A3349096}", { "CV B Plot", "CV Bank B Plot" }, part_kind::input, part_type::gcv_plot, 1, gcv_plot_params, cv_plot_param::count, 3, &gcv_plot_ui },
  { "{C972E264-1739-4DB6-B1DB-5D31057BD218}", { "Active", "Active" }, part_kind::selector, part_type::active, 1, active_params, active_param::count, -1, nullptr },
  { "{FEEBA3F5-F248-4C1B-BD8C-F3A492D084E2}", { "Output", "Output" }, part_kind::output, part_type::output, 1, output_params, output_param::count, 2, &output_ui }
};                 
 
part_ui_colors make_part_ui_colors()
{
  // No statics, must be in function body because static initialization order.

  ui_color const black = 0xFF000000;
  ui_color const white = 0xFFFFFFFF;  
  ui_color const transparent = 0x00000000;
  ui_color const background = 0xFF6D787E;
  ui_color const foreground1 = ui_color(0xFFFD9A4D); 
  ui_color const foreground2 = ui_color(0xFFD1DDE4).darken(0.85f);
  ui_color const foreground3 = ui_color(0xFF29A5BB); 
    
  ui_color const label_color = foreground2;
  ui_color const border_color = background.darken(0.6f);
  ui_color const info_label_color = foreground1;
  ui_color const header_label_color = foreground1;  
  ui_color const connector_color = foreground1;  
  ui_color const edit_font_color = foreground2.darken(0.85f); 
  ui_color const edit_back_color = black.alpha(0x50);
  ui_color const menu_font_color = foreground1.darken(0.9f);
  ui_color const menu_back_color = foreground3.darken(0.4).alpha(0x80); 
  ui_color const knob_menu_font_color = foreground2.darken(0.85f);
  ui_color const knob_menu_back_color = black.alpha(0x60); 
  ui_color const table_menu_font_color = foreground2.darken(0.85f);
  ui_color const table_menu_back_color = black.alpha(0x60);
  ui_color const check_mark_color = foreground1.alpha(0xC0);  
  ui_color const check_fill_color = black.alpha(0x80);   
  ui_color const check_frame_color = foreground3; 
  ui_color const knob_fill_color = background.darken(0.5f); 
  ui_color const knob_marker_color = foreground1.darken(0.9f);
  ui_color const knob_drag_color = foreground3; 
  ui_color const knob_inner_color = foreground2.darken(0.8f);
  ui_color const knob_outer_color = background.darken(0.25f);
  ui_color const knob_light_color = white;  
  ui_color const knob_shadow_color = black;   
  ui_color const graph_line_color = foreground3; 
  ui_color const graph_area_color = foreground3.alpha(0xC0);  
  ui_color const graph_grid_color = foreground1.alpha(0x18);    
  ui_color const graph_fill_color = transparent;   
  ui_color const graph_frame_color = background.darken(0.6f); 
  ui_color const param_back_color = background.alpha(0x28);
  ui_color const param_frame_color = background.darken(0.75f).alpha(0x60); 
  ui_color const header_back_color = foreground1.darken(0.25f).alpha(0x40);
  ui_color const header_frame_color = background.alpha(0xA0);  
  ui_color const container_back_color = black.alpha(0xC0);   
  ui_color const container_frame_color = background.darken(0.5f); 
  ui_color const tab_header_font_color = foreground1;   
  ui_color const tab_font_color = foreground2.darken(0.85f); 
  ui_color const tab_active_font_color = foreground2;   
  ui_color const tab_inner_frame_color = background.darken(0.33f);
  ui_color const tab_outer_frame_color = background.darken(0.67f);  
  ui_color const tab_back_color = background.darken(0.1f).alpha(0xC0);  
  ui_color const tab_active_back_color = background.darken(0.5f).alpha(0x80);
  ui_color const header_check_mark_color = foreground1.alpha(0xC0);
  ui_color const header_check_fill_color = black.alpha(0x80);     
  ui_color const header_check_frame_color = foreground3; 

  return { 
    { "label", label_color },
    { "border", border_color },
    { "info_label", info_label_color },
    { "header_label", header_label_color },
    { "connector", connector_color },
    { 
      { "knob_fill", knob_fill_color },
      { "knob_marker", knob_marker_color },
      { "knob_drag", knob_drag_color },
      { "knob_inner", knob_inner_color },
      { "knob_outer", knob_outer_color },
      { "knob_light", knob_light_color },
      { "knob_shadow", knob_shadow_color }
    },
    { 
      { "graph_line", graph_line_color },
      { "graph_area", graph_area_color },
      { "graph_grid", graph_grid_color },
      { "graph_fill", graph_fill_color },
      { "graph_frame", graph_frame_color }
    },
    { 
      { "edit_font", edit_font_color },
      { "edit_back", edit_back_color }
    },
    { 
      { "menu_font", menu_font_color },
      { "menu_back", menu_back_color }
    },
    { 
      { "knob_menu_font", knob_menu_font_color },
      { "knob_menu_back", knob_menu_back_color }
    },
    { 
      { "table_menu_font", table_menu_font_color },
      { "table_menu_back", table_menu_back_color }
    },
    { 
      { "tab_header_font", tab_header_font_color },
      { "tab_font", tab_font_color },
      { "tab_back", tab_back_color },
      { "tab_inner_frame", tab_inner_frame_color },
      { "tab_active_font", tab_active_font_color },
      { "tab_active_back", tab_active_back_color },
      { "tab_outer_frame", tab_outer_frame_color }
    },
    { 
      { "check_mark", check_mark_color },
      { "check_fill", check_fill_color },
      { "check_frame", check_frame_color }
    }, 
    { 
      { "header_check_mark", header_check_mark_color },
      { "header_check_fill", header_check_fill_color },
      { "header_check_frame", header_check_frame_color }
    },
    { 
      { "param_back", param_back_color },
      { "param_frame", param_frame_color }
    },
    { 
      { "container_back", container_back_color },
      { "container_frame", container_frame_color }
    },
    { 
      { "header_back", header_back_color },
      { "header_frame", header_frame_color }
    }
  };
}
                                    
} // namespace inf::synth                               