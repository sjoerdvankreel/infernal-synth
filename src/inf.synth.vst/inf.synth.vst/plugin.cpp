#include <inf.synth/lfo/topology.hpp>
#include <inf.synth/synth/topology.hpp>
#include <inf.synth/cv_bank/topology.hpp>
#include <inf.synth/envelope/topology.hpp>
#include <inf.synth/audio_bank/topology.hpp>
#include <inf.synth/oscillator/topology.hpp>

#include <inf.synth.vst/plugin.hpp>
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
  return true;
}

static
bool inf_exit()
{
  --_inf_module_counter;
  if (_inf_module_counter > 0) return true;
  if (_inf_module_counter < 0) return false;
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
  synth_vst_topology(bool is_instrument): synth_topology(is_instrument) {}
  char const* plugin_name() const override;
  std::uint16_t version_major() const { return INF_SYNTH_VST_VERSION_MAJOR; }
  std::uint16_t version_minor() const { return INF_SYNTH_VST_VERSION_MINOR; }
};

char const* 
synth_vst_topology::plugin_name() const
{
  if(is_instrument()) return INF_SYNTH_VST_INSTRUMENT_NAME;
  else return INF_SYNTH_VST_FX_NAME;
}

extern "C" SMTG_EXPORT_SYMBOL 
topology_info* inf_vst_create_topology2(std::int32_t is_instrument)
{ 
  topology_info* result = new synth_vst_topology(is_instrument != 0);
  topology_info::init(result, part_descriptors, part_type::count, synth_polyphony, synth_max_ui_height);
  return result;
}

static FUnknown*
create_controller(std::int32_t is_instrument)
{
  auto topology = std::unique_ptr<topology_info>(inf_vst_create_topology2(is_instrument));
  auto processor_tuid = is_instrument ? instrument_processor_id : fx_processor_id;
  auto controller = new vst_ui_controller(std::move(topology), FUID::fromTUID(processor_tuid));
  return static_cast<IEditController*>(controller);
}

static FUnknown* 
create_processor(std::int32_t is_instrument)
{
  auto topology = std::unique_ptr<topology_info>(inf_vst_create_topology2(is_instrument));
  auto controller_tuid = is_instrument ? instrument_controller_id : fx_controller_id;
  auto processor = new vst_processor(std::move(topology), FUID::fromTUID(controller_tuid));
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
