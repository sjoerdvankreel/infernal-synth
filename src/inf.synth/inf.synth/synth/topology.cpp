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
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out1, "FX B 1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in2, "FX B 1");
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
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out1, "FX B 1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in2, "FX B 1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out2, "FX B 2");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in3, "FX B 2");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out3, "Master");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in4, "Ext");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out4, "FX B 3");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in5, "FX B 3");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out5, "FX B 4");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in6, "FX B 4");
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
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in1, "CV U 1");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out1, "Audio B 1 Amt3");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op1, "Mul");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in2, "CV B 1");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out2, "FX B 2 SV Frq");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op2, "Add");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in3, "CV U 2");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out3, "Audio B 1 Amt6");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op3, "Mul");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in4, "CV B 2");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out4, "FX B 4 SV Frq");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op4, "Add");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in5, "LFO B 1");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out5, "FX B 2 SV Frq");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::amt5, "50");

  // cv plot
  set_ui_value(state, part_type::gcv_plot, 0, cv_plot_param::length, "10");
  set_ui_value(state, part_type::gcv_plot, 0, cv_plot_param::target, "FX B 2 SV Frq");
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
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::out1, "FX A 1");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::in2, "FX A 1");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::out2, "FX A 2");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::in3, "FX A 2");
  set_ui_value(state, part_type::vaudio_bank, 0, audio_bank_param::out3, "Voice");

  // audio b1 voice->fx1->fx2->master
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::on, "On");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in1, "Voice");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out1, "FX B 1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in2, "FX B 1");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::out2, "FX B 2");
  set_ui_value(state, part_type::gaudio_bank, 0, audio_bank_param::in3, "FX B 2");
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
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::in2, "LFO A 1");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::out2, "Osc 1 Cent");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::amt2, "10");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::in3, "Env 2");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::out3, "FX A 2 SV Frq");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::op3, "Mul");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::in4, "CV U 2");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::out4, "FX A 2 SV Frq");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::off4, "20");
  set_ui_value(state, part_type::vcv_bank, 0, vcv_bank_param::op4, "Mul");

  // lfo b1 to filter freq & master cvs
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::on, "On");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in1, "LFO B 1");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out1, "FX B 1 SV Frq");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op1, "Add");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::amt1, "66");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in2, "CV U 1");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out2, "Master Gain");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op2, "Mul");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in3, "CV B 1");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out3, "Master Bal");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op3, "Add");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in4, "CV U 2");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out4, "FX B 1 SV Frq");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op4, "Mul");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::off4, "20");

  // cv plots
  set_ui_value(state, part_type::gcv_plot, 0, cv_plot_param::length, "5");
  set_ui_value(state, part_type::gcv_plot, 0, cv_plot_param::target, "FX B 1 SV Frq");
  set_ui_value(state, part_type::vcv_plot, 0, cv_plot_param::target, "FX A 2 SV Frq");
}

// ---- part selector ----

static std::vector<std::string> const active_vosc_tab_items = list_tab_items(vosc_count);
static std::vector<std::string> const active_veffect_tab_items = list_tab_items(veffect_count);
static std::vector<std::string> const active_geffect_tab_items = list_tab_items(geffect_count);
static std::vector<std::string> const active_vaudio_bank_tab_items = list_tab_items(vaudio_bank_count);
static std::vector<std::string> const active_gaudio_bank_tab_items = list_tab_items(gaudio_bank_count);
static std::vector<std::string> const active_venv_tab_items = list_tab_items(venv_count);
static std::vector<std::string> const active_vlfo_tab_items = list_tab_items(vlfo_count);
static std::vector<std::string> const active_glfo_tab_items = list_tab_items(glfo_count);
static std::vector<std::string> const active_vcv_bank_tab_items = list_tab_items(vcv_bank_count);
static std::vector<std::string> const active_gcv_bank_tab_items = list_tab_items(gcv_bank_count);

static std::vector<list_item> const active_vosc_items = list_items({ "{3B2713C0-6DFF-4257-9CB4-DF5FD0DA603C}", "Osc" }, vosc_count);
static std::vector<list_item> const active_veffect_items = list_items({ "{33BB6F1B-3D4F-4D58-998D-417DA711D6C2}", "FX A" }, veffect_count);
static std::vector<list_item> const active_geffect_items = list_items({ "{1607D775-53C5-46BE-957A-F697831062CD}", "FX B" }, geffect_count);
static std::vector<list_item> const active_vaudio_bank_items = list_items({ "{8AD2E3E5-0DCF-42E3-B9B6-33F775DF917F}", "Audio A" }, vaudio_bank_count);
static std::vector<list_item> const active_gaudio_bank_items = list_items({ "{D14108D3-FC7A-49D5-A4CB-AB66BE6F1B1E}", "Audio B" }, gaudio_bank_count);
static std::vector<list_item> const active_venv_items = list_items({ "{8545D490-B40D-4154-BFC2-FE86A6F26BB5}", "Env" }, venv_count);
static std::vector<list_item> const active_vlfo_items = list_items({ "{5AADD04D-FC8B-4326-97B1-35326F645D09}", "LFO A" }, vlfo_count);
static std::vector<list_item> const active_glfo_items = list_items({ "{88E92673-4FA2-45C3-B1D0-71A2A63ECCF4}", "LFO B" }, glfo_count);
static std::vector<list_item> const active_vcv_bank_items = list_items({ "{9C450189-3395-4E03-8EB8-1788BE94042F}", "CV A" }, vcv_bank_count);
static std::vector<list_item> const active_gcv_bank_items = list_items({ "{8F6E11A2-C364-4301-9D0D-135909C4312B}", "CV B" }, gcv_bank_count);
 
static param_descriptor const 
active_params[active_param::count] =     
{
  { "{0EEADBD0-D37B-4B53-A2D5-F22E2154F2D8}", { { "Osc", "Oscillator" }, "", param_kind::block, param_type::list, { &active_vosc_items, &active_vosc_tab_items } } },
  { "{D5DD0DFC-AC9D-42E6-9D2B-924786382825}", { { "FX A", "FX A" }, "", param_kind::block, param_type::list, { &active_veffect_items, &active_veffect_tab_items } } },
  { "{379A6E9F-C7DA-40CE-950B-C51D202F0060}", { { "FX B", "FX B" }, "", param_kind::block, param_type::list, { &active_geffect_items, &active_geffect_tab_items } } },
  { "{4C161E6D-7487-4D98-86DC-73B2B414A299}", { { "Audio A", "Audio Bank A" }, "", param_kind::block, param_type::list, { &active_vaudio_bank_items, &active_vaudio_bank_tab_items } } },
  { "{2627D0E5-7522-4BE3-8659-AA526CF2186C}", { { "Audio B", "Audio Bank B" }, "", param_kind::block, param_type::list, { &active_gaudio_bank_items, &active_gaudio_bank_tab_items } } },
  { "{556BF692-55B0-48B4-BD6A-E2CEFA17B012}", { { "Env", "Envelope" }, "", param_kind::block, param_type::list, { &active_venv_items, &active_venv_tab_items } } },
  { "{EEA97414-8C1C-4378-A68B-409692FFA920}", { { "LFO A", "LFO A" }, "", param_kind::block, param_type::list, { &active_vlfo_items, &active_vlfo_tab_items } } },
  { "{1618ADF6-BE07-44F2-8839-076028834199}", { { "LFO B", "LFO B" }, "", param_kind::block, param_type::list, { &active_glfo_items, &active_glfo_tab_items } } },
  { "{BFCAD318-CB76-4AF7-9B43-3F1776144813}", { { "CV A", "CV Bank A" }, "", param_kind::block, param_type::list, { &active_vcv_bank_items, &active_vcv_bank_tab_items } } },
  { "{A6203077-A2A1-4509-8F99-86A29BC2EE9B}", { { "CV B", "CV Bank B" }, "", param_kind::block, param_type::list, { &active_gcv_bank_items, &active_gcv_bank_tab_items } } },
};

// ---- global topo ----

part_descriptor const                                  
part_descriptors[part_type::count] =                                             
{                       
  { "{5C9D2CD3-2D4C-4205-893E-6B5DE9D62ADE}", { "Osc", "Oscillator" }, part_kind::input, part_type::vosc, vosc_count, osc_params, osc_param::count },
  { "{2C377544-C124-48F5-A4F4-1E301B108C58}", { "FX A", "FX A" }, part_kind::input, part_type::veffect, veffect_count, veffect_params, effect_param::vfx_count }, 
  { "{E8F67736-5976-4FDE-939F-31C373B7F920}", { "FX B", "FX B" }, part_kind::input, part_type::geffect, geffect_count, geffect_params, effect_param::gfx_count },
  { "{7A77C027-FC8F-4425-9BF0-393267D92F0C}", { "Audio A", "Audio Bank A" }, part_kind::input, part_type::vaudio_bank, vaudio_bank_count, vaudio_bank_params, audio_bank_param::count },
  { "{B5B4A442-13ED-43ED-B9E0-3B2894D03838}", { "Audio B", "Audio Bank B" }, part_kind::input, part_type::gaudio_bank, gaudio_bank_count, gaudio_bank_params, audio_bank_param::count },
  { "{E6344937-C1F7-4F2A-83E7-EA27D48DEC4E}", { "Voice", "Voice" }, part_kind::input, part_type::voice, 1, voice_params, voice_param::count },
  { "{6DE3AAB2-6D43-41ED-9BBE-E281DB8F7B44}", { "Master", "Master" }, part_kind::input, part_type::master, 1, master_params, master_param::count },
  { "{FC4885FE-431C-477A-B5B7-84863DB8C07D}", { "Env", "Envelope" }, part_kind::input, part_type::venv, venv_count, envelope_params, envelope_param::count },
  { "{56DE75BB-BE73-4B27-B37F-77F6E408F986}", { "LFO A", "LFO A" }, part_kind::input, part_type::vlfo, vlfo_count, vlfo_params, lfo_param::count },
  { "{5BE4D402-BD27-478B-9C14-A570A4306FFA}", { "LFO B", "LFO B" }, part_kind::input, part_type::glfo, glfo_count, glfo_params, lfo_param::count },
  { "{E6814824-7F56-4A9C-92B6-F5EB001B9513}", { "CV A", "CV Bank A" }, part_kind::input, part_type::vcv_bank, vcv_bank_count, vcv_bank_params, vcv_bank_param::count },
  { "{3F416415-4C1E-49B3-A59F-0C4472C11B69}", { "CV B", "CV Bank B" }, part_kind::input, part_type::gcv_bank, gcv_bank_count, gcv_bank_params, gcv_bank_param::count },
  { "{B13B3846-DDDE-4CAE-9641-7C8AEAAA9C01}", { "CV A Plot", "CV Bank A Plot" }, part_kind::input, part_type::vcv_plot, 1, vcv_plot_params, cv_plot_param::count },
  { "{30B485EC-0EDC-4792-9ED1-8AE5A3349096}", { "CV B Plot", "CV Bank B Plot" }, part_kind::input, part_type::gcv_plot, 1, gcv_plot_params, cv_plot_param::count },
  { "{C972E264-1739-4DB6-B1DB-5D31057BD218}", { "Active", "Active" }, part_kind::selector, part_type::active, 1, active_params, active_param::count },
  { "{FEEBA3F5-F248-4C1B-BD8C-F3A492D084E2}", { "Output", "Output" }, part_kind::output, part_type::output, 1, output_params, output_param::count }
};         
                                    
} // namespace inf::synth                               