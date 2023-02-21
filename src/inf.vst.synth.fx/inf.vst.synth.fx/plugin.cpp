#include <inf.synth/synth/topology.hpp>
#include <inf.synth/cv_bank/topology.hpp>
#include <inf.synth/audio_bank/topology.hpp>

#include <inf.vst.synth.fx/plugin.hpp>
#include <inf.vst.base/sdk/processor.hpp>
#include <inf.vst.base/sdk/controller.hpp>
#include <inf.vst.base/shared/bootstrap.hpp>

#include <public.sdk/source/main/pluginfactory.h>
#include <pluginterfaces/vst/ivstaudioprocessor.h>

using namespace inf::base;
using namespace inf::synth;
using namespace inf::vst::base;

using namespace Steinberg;
using namespace Steinberg::Vst;

#if SVN_VERSIONED
static FUID const inf_vst_processor_id(0x88B6D7ED, 0x1B3A4AAE, 0xA9612A67, 0x56F66197);
static FUID const inf_vst_controller_id(0x5E5E1287, 0xDFA148FE, 0xA543CCEE, 0x286FF4DE);
#elif !SVN_VERSIONED
static FUID const inf_vst_processor_id(0x00AE86BB, 0x62E6495B, 0xBB947F2F, 0x7D98CC1A);
static FUID const inf_vst_controller_id(0x101EB406, 0x0A0F4361, 0xBBFF18B5, 0xF566FEBE);
#else
#error
#endif

class synth_fx_topology :
public synth_topology
{
public:
  bool is_instrument() const override { return false; }
  void init_defaults(param_value* state) const override;
};

void
synth_fx_topology::init_defaults(param_value* state) const
{
  topology_info::init_defaults(state);
  state[param_bounds[part_type::geffect][0] + effect_param::on].discrete = 1; /* enabled */
  state[param_bounds[part_type::geffect][0] + effect_param::type].discrete = 2; /* delay */
  state[param_bounds[part_type::gaudio_bank][0] + gaudio_bank_param::on].discrete = 1; /* enabled */
  state[param_bounds[part_type::gaudio_bank][0] + gaudio_bank_param::in1].discrete = 2; /* external */ 
  state[param_bounds[part_type::gaudio_bank][0] + gaudio_bank_param::out1].discrete = 1; /* gfx 1 */
  state[param_bounds[part_type::gaudio_bank][0] + gaudio_bank_param::in2].discrete = 3; /* gfx 1 */
  state[param_bounds[part_type::gaudio_bank][0] + gaudio_bank_param::out2].discrete = 1 + geffect_count; /* master */
  return;
}

// Binding to vst base project.
extern "C" topology_info*
inf_vst_create_topology_impl()
{ 
  topology_info* result = new synth_fx_topology;
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
  SVN_VST_SEVEN_SYNTH_FX_COMPANY_NAME, 
  SVN_VST_SEVEN_SYNTH_FX_COMPANY_WEB, 
  SVN_VST_SEVEN_SYNTH_FX_COMPANY_MAIL)

  DEF_CLASS2(
    INLINE_UID_FROM_FUID(inf_vst_processor_id),
    PClassInfo::kManyInstances, kVstAudioEffectClass, SVN_VST_SEVEN_SYNTH_FX_NAME,
    Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kFx, 
    SVN_VST_SEVEN_SYNTH_FX_VERSION, kVstVersionString, inf_vst_processor_factory)
  
  DEF_CLASS2(
    INLINE_UID_FROM_FUID(inf_vst_controller_id),
    PClassInfo::kManyInstances, kVstComponentControllerClass, 
    SVN_VST_SEVEN_SYNTH_FX_CONTROLLER_NAME, 0, "", 
    SVN_VST_SEVEN_SYNTH_FX_VERSION, kVstVersionString, inf_vst_controller_factory)
END_FACTORY
