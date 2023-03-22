#include <inf.synth/lfo/topology.hpp>
#include <inf.synth/synth/topology.hpp>
#include <inf.synth/cv_bank/topology.hpp>
#include <inf.synth/audio_bank/topology.hpp>

#include <inf.synth.vst.fx/plugin.hpp>
#include <inf.base.vst/sdk/processor.hpp>
#include <inf.base.vst/sdk/controller.hpp>
#include <inf.base.vst.ui/bootstrap.hpp>
#include <inf.base.vst.ui/ui_controller.hpp>

#include <public.sdk/source/main/pluginfactory.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>

using namespace inf::base;
using namespace inf::base::vst;
using namespace inf::base::vst::ui;
using namespace inf::synth;

using namespace Steinberg;
using namespace Steinberg::Vst;

#if INF_VERSIONED
static FUID const inf_synth_vst_processor_id(0x88B6D7ED, 0x1B3A4AAE, 0xA9612A67, 0x56F66197);
static FUID const inf_synth_vst_controller_id(0x5E5E1287, 0xDFA148FE, 0xA543CCEE, 0x286FF4DE);
#elif !INF_VERSIONED
static FUID const inf_synth_vst_processor_id(0x00AE86BB, 0x62E6495B, 0xBB947F2F, 0x7D98CC1A);
static FUID const inf_synth_vst_controller_id(0x101EB406, 0x0A0F4361, 0xBBFF18B5, 0xF566FEBE);
#else
#error
#endif

class synth_fx_topology :
public synth_topology
{
public:
  bool is_instrument() const override { return false; }
  void init_clear_patch(param_value* state) const override;
  void init_factory_preset(param_value* state) const override;
  char const* plugin_name() const { return INF_SYNTH_VST_NAME; }
  std::uint16_t version_major() const { return INF_SYNTH_VST_VERSION_MAJOR; }
  std::uint16_t version_minor() const { return INF_SYNTH_VST_VERSION_MINOR; }
};

void
synth_fx_topology::init_clear_patch(param_value* state) const
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
synth_fx_topology::init_factory_preset(param_value* state) const
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

// Binding to vst base project.
extern "C" topology_info*
inf_vst_create_topology_impl()
{ 
  topology_info* result = new synth_fx_topology;
  topology_info::init(result, part_descriptors, part_type::count, synth_polyphony, synth_max_ui_height);
  return result;
}

static FUnknown* inf_synth_vst_controller_factory(void* context)
{
  auto topology = std::unique_ptr<topology_info>(inf_vst_create_topology());
  auto controller = new vst_ui_controller(std::move(topology));
  return static_cast<IEditController*>(controller);
}

static FUnknown* inf_synth_vst_processor_factory(void* context)
{
  auto topology = std::unique_ptr<topology_info>(inf_vst_create_topology());
  auto processor = new vst_processor(std::move(topology), inf_synth_vst_controller_id);
  return static_cast<IAudioProcessor*>(processor);
}

BEGIN_FACTORY_DEF(
  INF_SYNTH_VST_COMPANY_NAME,
  INF_SYNTH_VST_COMPANY_WEB,
  INF_SYNTH_VST_COMPANY_MAIL)

  DEF_CLASS2(
    INLINE_UID_FROM_FUID(inf_synth_vst_processor_id),
    PClassInfo::kManyInstances, kVstAudioEffectClass, INF_SYNTH_VST_NAME,
    Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kFx, 
    INF_SYNTH_VST_VERSION, kVstVersionString, inf_synth_vst_processor_factory)
  
  DEF_CLASS2(
    INLINE_UID_FROM_FUID(inf_synth_vst_controller_id),
    PClassInfo::kManyInstances, kVstComponentControllerClass, 
    INF_SYNTH_VST_CONTROLLER_NAME, 0, "",
    INF_SYNTH_VST_VERSION, kVstVersionString, inf_synth_vst_controller_factory)
END_FACTORY
