#include <inf.synth/synth/topology.hpp>
#include <inf.synth/lfo/topology.hpp>
#include <inf.synth/cv_bank/topology.hpp>
#include <inf.synth/envelope/topology.hpp>
#include <inf.synth/audio_bank/topology.hpp>
#include <inf.synth/oscillator/topology.hpp>

#include <inf.vst.base/sdk/processor.hpp>
#include <inf.vst.base/sdk/controller.hpp>
#include <inf.vst.base/shared/bootstrap.hpp>
#include <inf.vst.synth.instrument/plugin.hpp>

#include <public.sdk/source/main/pluginfactory.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>

using namespace inf::base;
using namespace inf::synth;
using namespace inf::vst::base;

using namespace Steinberg;
using namespace Steinberg::Vst;

#if INF_VERSIONED
static FUID const inf_vst_processor_id(0x6FC553EB, 0x790B4B19, 0xA0A9AB13, 0xFBB76E3E);
static FUID const inf_vst_controller_id(0x0EB1168B, 0x2DE04E62, 0xAA3AD518, 0x07CCA298);
#elif !INF_VERSIONED
static FUID const inf_vst_processor_id(0xFBFCEDA8, 0x782047CE, 0xA12E8A8C, 0x8C3407E9);
static FUID const inf_vst_controller_id(0x57068B2B, 0x63374143, 0x85FA79D9, 0xAC8A38A5);
#else
#error
#endif

class synth_instrument_topology :
public synth_topology
{
public:
  bool is_instrument() const override { return true; }
  void init_defaults(param_value* state) const override;
};

void
synth_instrument_topology::init_defaults(param_value* state) const
{
  topology_info::init_defaults(state);

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
  set_ui_value(state, part_type::veffect, 1, effect_param::flt_stvar_freq, "5000");

  // fx b1 state variable filter
  set_ui_value(state, part_type::geffect, 0, osc_param::on, "On");
  set_ui_value(state, part_type::geffect, 0, effect_param::type, "Filter");
  set_ui_value(state, part_type::geffect, 0, effect_param::filter_type, "State Variable");
  set_ui_value(state, part_type::geffect, 0, effect_param::flt_stvar_freq, "5000");
  set_ui_value(state, part_type::geffect, 0, effect_param::flt_stvar_res, "50");

  // fx b2 multitap delay
  set_ui_value(state, part_type::geffect, 1, osc_param::on, "On");
  set_ui_value(state, part_type::geffect, 1, effect_param::type, "Delay");
  set_ui_value(state, part_type::geffect, 1, effect_param::delay_type, "Multitap");
  set_ui_value(state, part_type::geffect, 1, effect_param::dly_synced, "On");
  set_ui_value(state, part_type::geffect, 1, effect_param::dly_multi_sprd, "75");
  set_ui_value(state, part_type::geffect, 1, effect_param::dly_multi_tempo, "3/16");

  // master gain and cvs
  set_ui_value(state, part_type::master, 0, master_param::gain, "6.0");
  set_ui_value(state, part_type::master, 0, master_param::gcv1_uni, "50");

  // audio a1 osc->fx1->fx2->voice
  set_ui_value(state, part_type::vaudio_bank, 0, vaudio_bank_param::on, "On");
  set_ui_value(state, part_type::vaudio_bank, 0, vaudio_bank_param::in1, "Osc 1");
  set_ui_value(state, part_type::vaudio_bank, 0, vaudio_bank_param::out1, "FX A1");
  set_ui_value(state, part_type::vaudio_bank, 0, vaudio_bank_param::in2, "FX A1");
  set_ui_value(state, part_type::vaudio_bank, 0, vaudio_bank_param::out2, "FX A2");
  set_ui_value(state, part_type::vaudio_bank, 0, vaudio_bank_param::in3, "FX A2");
  set_ui_value(state, part_type::vaudio_bank, 0, vaudio_bank_param::out3, "Voice");

  // audio b1 voice->fx1->fx2->master
  set_ui_value(state, part_type::gaudio_bank, 0, vaudio_bank_param::on, "On");
  set_ui_value(state, part_type::gaudio_bank, 0, vaudio_bank_param::in1, "Voice");
  set_ui_value(state, part_type::gaudio_bank, 0, vaudio_bank_param::out1, "FX B1");
  set_ui_value(state, part_type::gaudio_bank, 0, vaudio_bank_param::in2, "FX B1");
  set_ui_value(state, part_type::gaudio_bank, 0, vaudio_bank_param::out2, "FX B2");
  set_ui_value(state, part_type::gaudio_bank, 0, vaudio_bank_param::in3, "FX B2");
  set_ui_value(state, part_type::gaudio_bank, 0, vaudio_bank_param::out3, "Master");

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

  // cv a velo to voice amp, lfo a1 to osc 1 cent, env 2 to filter freq
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

  // lfo b1 to filter freq & master cvs
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::on, "On");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in1, "LFO B1");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out1, "FX B1 SV Frq");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op1, "Add");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::amt1, "33");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in2, "CV U1");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out2, "Master Gain");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op2, "Mul");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::in3, "CV B1");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::out3, "Master Bal");
  set_ui_value(state, part_type::gcv_bank, 0, gcv_bank_param::op3, "Add");

  // cv plots
  set_ui_value(state, part_type::gcv_plot, 0, cv_plot_param::length, "5");
  set_ui_value(state, part_type::gcv_plot, 0, cv_plot_param::target, "FX B1 SV Frq");
  set_ui_value(state, part_type::vcv_plot, 0, cv_plot_param::target, "FX A2 SV Frq");

/*

  // enable
  state[param_bounds[part_type::vosc][0] + osc_param::on].discrete = 1;
  state[param_bounds[part_type::vcv_bank][0] + vaudio_bank_param::on].discrete = 1;
  state[param_bounds[part_type::gcv_bank][0] + gaudio_bank_param::on].discrete = 1;
  state[param_bounds[part_type::vaudio_bank][0] + vaudio_bank_param::on].discrete = 1;
  state[param_bounds[part_type::gaudio_bank][0] + gaudio_bank_param::on].discrete = 1;

  // param indices
  std::int32_t master_gain = param_bounds[part_type::master][0] + master_param::gain;
  std::int32_t vcv_in_1 = param_bounds[part_type::vcv_bank][0] + vcv_bank_param::in1;
  std::int32_t vcv_out_1 = param_bounds[part_type::vcv_bank][0] + vcv_bank_param::out1;
  std::int32_t vcv_op_1 = param_bounds[part_type::vcv_bank][0] + vcv_bank_param::op1;
  std::int32_t gcv_in_1 = param_bounds[part_type::gcv_bank][0] + gcv_bank_param::in1;
  std::int32_t gcv_out_1 = param_bounds[part_type::gcv_bank][0] + gcv_bank_param::out1;
  std::int32_t gcv_op_1 = param_bounds[part_type::gcv_bank][0] + gcv_bank_param::op1;
  std::int32_t vaudio_in_1 = param_bounds[part_type::vaudio_bank][0] + vaudio_bank_param::in1;
  std::int32_t vaudio_out_1 = param_bounds[part_type::vaudio_bank][0] + vaudio_bank_param::out1;
  std::int32_t gaudio_in_1 = param_bounds[part_type::gaudio_bank][0] + gaudio_bank_param::in1;
  std::int32_t gaudio_out_1 = param_bounds[part_type::gaudio_bank][0] + gaudio_bank_param::out1;

  // set defaults
  state[master_gain] = params[master_gain].descriptor->data.parse_ui("-6.0");
  state[vaudio_in_1] = params[vaudio_in_1].descriptor->data.parse_ui("Osc 1");
  state[vaudio_out_1] = params[vaudio_out_1].descriptor->data.parse_ui("Voice");
  state[gaudio_in_1] = params[gaudio_in_1].descriptor->data.parse_ui("Voice");
  state[gaudio_out_1] = params[gaudio_out_1].descriptor->data.parse_ui("Master");
  state[vcv_in_1] = params[vcv_in_1].descriptor->data.parse_ui("Velocity");
  state[vcv_out_1] = params[vcv_out_1].descriptor->data.parse_ui("Voice Gain");
  state[vcv_op_1] = params[vcv_op_1].descriptor->data.parse_ui("Mul");
  state[gcv_in_1] = params[gcv_in_1].descriptor->data.parse_ui("CV U1");
  state[gcv_out_1] = params[gcv_out_1].descriptor->data.parse_ui("Master Gain");
  state[gcv_op_1] = params[gcv_op_1].descriptor->data.parse_ui("Mul");

  return;

*/
}

// Binding to vst base project.
extern "C" topology_info*
inf_vst_create_topology_impl()
{ 
  topology_info* result = new synth_instrument_topology;
  topology_info::init(result, part_descriptors, part_type::count, synth_polyphony, synth_max_ui_height);
  return result;
}

static FUnknown* inf_vst_controller_factory(void* context)
{ 
  auto topology = std::unique_ptr<topology_info>(inf_vst_create_topology());
  auto controller = new vst_controller(std::move(topology));
  return static_cast<IEditController*>(controller);
}

static FUnknown* inf_vst_processor_factory(void* context)
{ 
  auto topology = std::unique_ptr<topology_info>(inf_vst_create_topology());
  auto processor = new vst_processor(std::move(topology), inf_vst_controller_id);
  return static_cast<IAudioProcessor*>(processor);
}

BEGIN_FACTORY_DEF(
  INF_VST_INFERNAL_SYNTH_INSTRUMENT_COMPANY_NAME,
  INF_VST_INFERNAL_SYNTH_INSTRUMENT_COMPANY_WEB,
  INF_VST_INFERNAL_SYNTH_INSTRUMENT_COMPANY_MAIL)

  DEF_CLASS2(
    INLINE_UID_FROM_FUID(inf_vst_processor_id),
    PClassInfo::kManyInstances, kVstAudioEffectClass, INF_VST_INFERNAL_SYNTH_INSTRUMENT_NAME,
    Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kInstrumentSynth, 
    INF_VST_INFERNAL_SYNTH_INSTRUMENT_VERSION, kVstVersionString, inf_vst_processor_factory)

  DEF_CLASS2(
    INLINE_UID_FROM_FUID(inf_vst_controller_id),
    PClassInfo::kManyInstances, kVstComponentControllerClass, 
    INF_VST_INFERNAL_SYNTH_INSTRUMENT_CONTROLLER_NAME, 0, "",
    INF_VST_INFERNAL_SYNTH_INSTRUMENT_VERSION, kVstVersionString, inf_vst_controller_factory)
END_FACTORY
