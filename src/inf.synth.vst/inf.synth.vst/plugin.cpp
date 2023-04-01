#include <inf.synth/lfo/topology.hpp>
#include <inf.synth/synth/topology.hpp>
#include <inf.synth/cv_bank/topology.hpp>
#include <inf.synth/envelope/topology.hpp>
#include <inf.synth/audio_bank/topology.hpp>
#include <inf.synth/oscillator/topology.hpp>

#include <inf.synth.vst/plugin.hpp>
#include <inf.base.ui/shared/bootstrap.hpp>
#include <inf.base.vst/sdk/vst_processor.hpp>
#include <inf.base.vst.ui/vst_ui_controller.hpp>

#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <public.sdk/source/main/pluginfactory_constexpr.h>

#if WIN32
#include <Windows.h>
#endif

using namespace inf::base;
using namespace inf::base::vst;
using namespace inf::base::vst::ui;
using namespace inf::synth;

using namespace Steinberg;
using namespace Steinberg::Vst;

extern bool InitModule();
extern bool DeinitModule();
void* moduleHandle = nullptr;
static std::int32_t _inf_module_counter = 0;

#if INF_VERSIONED
static const DECLARE_UID(fx_processor_id, 0x88B6D7ED, 0x1B3A4AAE, 0xA9612A67, 0x56F66197);
static const DECLARE_UID(fx_controller_id, 0x5E5E1287, 0xDFA148FE, 0xA543CCEE, 0x286FF4DE);
static const DECLARE_UID(instrument_processor_id, 0x6FC553EB, 0x790B4B19, 0xA0A9AB13, 0xFBB76E3E);
static const DECLARE_UID(instrument_controller_id, 0x0EB1168B, 0x2DE04E62, 0xAA3AD518, 0x07CCA298);
#elif !INF_VERSIONED
static const DECLARE_UID(fx_processor_id, 0x00AE86BB, 0x62E6495B, 0xBB947F2F, 0x7D98CC1A);
static const DECLARE_UID(fx_controller_id, 0x101EB406, 0x0A0F4361, 0xBBFF18B5, 0xF566FEBE);
static const DECLARE_UID(instrument_processor_id, 0xFBFCEDA8, 0x782047CE, 0xA12E8A8C, 0x8C3407E9);
static const DECLARE_UID(instrument_controller_id, 0x57068B2B, 0x63374143, 0x85FA79D9, 0xAC8A38A5);
#else
#error
#endif

static bool 
inf_init(void* module_handle)
{
  if (++_inf_module_counter != 1) return true;
  moduleHandle = module_handle;
  if (!InitModule()) return false;
  inf::base::ui::initialize();
  return true;
}

static
bool inf_exit()
{
  --_inf_module_counter;
  if (_inf_module_counter > 0) return true;
  if (_inf_module_counter < 0) return false;
  inf::base::ui::terminate();
  if (!DeinitModule()) return false;
  moduleHandle = nullptr;
  return true;
}

#if WIN32
BOOL WINAPI
DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
  if (reason != DLL_PROCESS_ATTACH) return TRUE;
  moduleHandle = instance;
  return TRUE;
}
extern "C" SMTG_EXPORT_SYMBOL bool ExitDll() { return inf_exit(); }
extern "C" SMTG_EXPORT_SYMBOL bool InitDll() { return inf_init(moduleHandle); }
#else
extern "C" SMTG_EXPORT_SYMBOL bool ModuleExit(void) { return inf_exit(); }
extern "C" SMTG_EXPORT_SYMBOL bool ModuleEntry(void* handle) { return inf_init(handle); }
#endif  

class synth_vst_topology :
public synth_topology
{
public:
  std::uint16_t version_major() const { return INF_SYNTH_VST_VERSION_MAJOR; }
  std::uint16_t version_minor() const { return INF_SYNTH_VST_VERSION_MINOR; }
};

class synth_vst_fx_topology :
public synth_vst_topology
{
public:
  bool is_instrument() const override { return false; }
  void init_clear_patch(param_value* state) const override;
  void init_factory_preset(param_value* state) const override;
  char const* plugin_name() const { return INF_SYNTH_VST_FX_NAME; }
};

class synth_vst_instrument_topology :
public synth_vst_topology
{
public:
  bool is_instrument() const override { return true; }
  void init_clear_patch(param_value* state) const override;
  void init_factory_preset(param_value* state) const override;
  char const* plugin_name() const { return INF_SYNTH_VST_INSTRUMENT_NAME; }
};

static topology_info*
create_fx_topology()
{
  topology_info* result = new synth_vst_fx_topology;
  topology_info::init(result, part_descriptors, part_type::count, synth_polyphony, synth_max_ui_height);
  return result;
}

static topology_info*
create_instrument_topology()
{
  topology_info* result = new synth_vst_instrument_topology;
  topology_info::init(result, part_descriptors, part_type::count, synth_polyphony, synth_max_ui_height);
  return result;
}

extern "C" SMTG_EXPORT_SYMBOL 
topology_info* inf_vst_create_topology2(std::int32_t is_instrument)
{ return is_instrument? create_instrument_topology(): create_fx_topology(); }

static FUnknown*
create_controller(std::int32_t is_instrument)
{
  auto topology = std::unique_ptr<topology_info>(inf_vst_create_topology2(is_instrument));
  auto controller = new vst_ui_controller(std::move(topology));
  return static_cast<IEditController*>(controller);
}

static FUnknown* 
create_processor(std::int32_t is_instrument)
{
  auto topology = std::unique_ptr<topology_info>(inf_vst_create_topology2(is_instrument));
  auto tuid = is_instrument ? instrument_controller_id : fx_controller_id;
  auto processor = new vst_processor(std::move(topology), FUID::fromTUID(tuid));
  return static_cast<IAudioProcessor*>(processor);
}

static FUnknown* create_fx_processor(void* context) { return create_processor(0); }
static FUnknown* create_fx_controller(void* context) { return create_controller(0); }
static FUnknown* create_instrument_processor(void* context) { return create_processor(1); }
static FUnknown* create_instrument_controller(void* context) { return create_controller(1); }

BEGIN_FACTORY_DEF(
  INF_SYNTH_VST_COMPANY_NAME,
  INF_SYNTH_VST_COMPANY_WEB,
  INF_SYNTH_VST_COMPANY_MAIL,
  4)

  // fx
  DEF_CLASS(fx_processor_id, PClassInfo::kManyInstances, kVstAudioEffectClass, 
    INF_SYNTH_VST_FX_NAME, Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kFx,
    INF_SYNTH_VST_VERSION, kVstVersionString, create_fx_processor, nullptr)
  DEF_CLASS(fx_controller_id, PClassInfo::kManyInstances, kVstComponentControllerClass,
    INF_SYNTH_VST_FX_CONTROLLER_NAME, 0, "",
    INF_SYNTH_VST_VERSION, kVstVersionString, create_fx_controller, nullptr)

  // instrument
  DEF_CLASS(instrument_processor_id, PClassInfo::kManyInstances, kVstAudioEffectClass,
    INF_SYNTH_VST_INSTRUMENT_NAME, Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kInstrumentSynth,
    INF_SYNTH_VST_VERSION, kVstVersionString, create_instrument_processor, nullptr)
  DEF_CLASS(instrument_controller_id, PClassInfo::kManyInstances, kVstComponentControllerClass,
    INF_SYNTH_VST_INSTRUMENT_CONTROLLER_NAME, 0, "",
    INF_SYNTH_VST_VERSION, kVstVersionString, create_instrument_controller, nullptr)
  
END_FACTORY
  
void
synth_vst_fx_topology::init_clear_patch(param_value* state) const
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
synth_vst_fx_topology::init_factory_preset(param_value* state) const
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
synth_vst_instrument_topology::init_clear_patch(param_value* state) const
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
synth_vst_instrument_topology::init_factory_preset(param_value* state) const
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
