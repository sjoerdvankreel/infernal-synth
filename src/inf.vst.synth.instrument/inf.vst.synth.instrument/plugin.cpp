#include <inf.synth/synth/topology.hpp>
#include <inf.synth/cv_bank/topology.hpp>
#include <inf.synth/audio_bank/topology.hpp>

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

#if SVN_VERSIONED
static FUID const inf_vst_processor_id(0x6FC553EB, 0x790B4B19, 0xA0A9AB13, 0xFBB76E3E);
static FUID const inf_vst_controller_id(0x0EB1168B, 0x2DE04E62, 0xAA3AD518, 0x07CCA298);
#elif !SVN_VERSIONED
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
  state[param_bounds[part_type::vosc][0] + osc_param::on].discrete = 1; /* enabled */
  state[param_bounds[part_type::master][0] + master_param::gain].real = 0.25; /* -6dB */
  state[param_bounds[part_type::vaudio_bank][0] + vaudio_bank_param::on].discrete = 1; /* enabled */
  state[param_bounds[part_type::vaudio_bank][0] + vaudio_bank_param::in1].discrete = 1; /* osc 1 */
  state[param_bounds[part_type::vaudio_bank][0] + vaudio_bank_param::out1].discrete = 1 + veffect_count; /* voice */
  state[param_bounds[part_type::gaudio_bank][0] + gaudio_bank_param::on].discrete = 1; /* enabled */
  state[param_bounds[part_type::gaudio_bank][0] + gaudio_bank_param::in1].discrete = 1; /* voice */
  state[param_bounds[part_type::gaudio_bank][0] + gaudio_bank_param::out1].discrete = 1 + geffect_count; /* master */
  return;
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
  SVN_VST_SEVEN_SYNTH_INSTRUMENT_COMPANY_NAME, 
  SVN_VST_SEVEN_SYNTH_INSTRUMENT_COMPANY_WEB, 
  SVN_VST_SEVEN_SYNTH_INSTRUMENT_COMPANY_MAIL)

  DEF_CLASS2(
    INLINE_UID_FROM_FUID(inf_vst_processor_id),
    PClassInfo::kManyInstances, kVstAudioEffectClass, SVN_VST_SEVEN_SYNTH_INSTRUMENT_NAME,
    Steinberg::Vst::kDistributable, Steinberg::Vst::PlugType::kInstrumentSynth, 
    SVN_VST_SEVEN_SYNTH_INSTRUMENT_VERSION, kVstVersionString, inf_vst_processor_factory)

  DEF_CLASS2(
    INLINE_UID_FROM_FUID(inf_vst_controller_id),
    PClassInfo::kManyInstances, kVstComponentControllerClass, 
    SVN_VST_SEVEN_SYNTH_INSTRUMENT_CONTROLLER_NAME, 0, "", 
    SVN_VST_SEVEN_SYNTH_INSTRUMENT_VERSION, kVstVersionString, inf_vst_controller_factory)
END_FACTORY
