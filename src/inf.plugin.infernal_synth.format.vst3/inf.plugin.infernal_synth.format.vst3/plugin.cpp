#include <inf.plugin.infernal_synth/plugin.hpp>
#include <inf.plugin.infernal_synth/lfo/topology.hpp>
#include <inf.plugin.infernal_synth/synth/topology.hpp>
#include <inf.plugin.infernal_synth/cv_bank/topology.hpp>
#include <inf.plugin.infernal_synth/envelope/topology.hpp>
#include <inf.plugin.infernal_synth/audio_bank/topology.hpp>
#include <inf.plugin.infernal_synth/oscillator/topology.hpp>

#include <inf.plugin.infernal_synth.ui/ui.hpp>
#include <inf.base.ui/shared/support.hpp>
#include <inf.base.format.vst3/vst_editor.hpp>
#include <inf.base.format.vst3/vst_processor.hpp>
#include <inf.base.format.vst3/vst_controller.hpp>

#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <pluginterfaces/vst/ivstmidicontrollers.h>
#include <public.sdk/source/main/pluginfactory_constexpr.h>

#if WIN32
#include <Windows.h>
#endif

using namespace inf::base;
using namespace inf::base::ui;
using namespace inf::base::format::vst3;
using namespace inf::plugin::infernal_synth;
using namespace inf::plugin::infernal_synth::ui;

using namespace Steinberg;
using namespace Steinberg::Vst;

extern bool InitModule();
extern bool DeinitModule();
void* moduleHandle = nullptr;
static std::int32_t _inf_module_counter = 0;

#if IPISFV3_FX
#define IPISFV3_NAME IPIS_FX_NAME
#define IPISFV3_UNIQUE_ID_TEXT IPIS_FX_UNIQUE_ID_TEXT
#define IPISFV3_CONTROLLER_NAME "InfernalSynthFXController"
#define IPISFV3_PLUG_TYPE Steinberg::Vst::PlugType::kFx
static const DECLARE_UID(vst_controller_id, 0x35695F63, 0x837242BE, 0x88FB88A4, 0xE4F2D1B8);
static const DECLARE_UID(vst_processor_id, IPIS_FX_UNIQUE_ID_HEX1, IPIS_FX_UNIQUE_ID_HEX2, IPIS_FX_UNIQUE_ID_HEX3, IPIS_FX_UNIQUE_ID_HEX4);
#elif (!IPISFV3_FX)
#define IPISFV3_NAME IPIS_INST_NAME
#define IPISFV3_UNIQUE_ID_TEXT IPIS_INST_UNIQUE_ID_TEXT
#define IPISFV3_CONTROLLER_NAME "InfernalSynthController"
#define IPISFV3_PLUG_TYPE Steinberg::Vst::PlugType::kInstrumentSynth
static const DECLARE_UID(vst_controller_id, 0x612E5225, 0xD6A44771, 0xA581057D, 0x04034620);
static const DECLARE_UID(vst_processor_id, IPIS_INST_UNIQUE_ID_HEX1, IPIS_INST_UNIQUE_ID_HEX2, IPIS_INST_UNIQUE_ID_HEX3, IPIS_INST_UNIQUE_ID_HEX4);
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

class synth_vst_editor:
public vst_editor
{
public:
  synth_vst_editor(vst_controller* controller): vst_editor(controller) {}
  std::unique_ptr<root_element> create_ui() override 
  { return create_synth_ui(dynamic_cast<vst_controller*>(controller.get())); }
};

class synth_vst_controller :
public vst_controller
{
protected:
  bool map_midi_control(std::int32_t number, std::int32_t& target_tag) const override;
public:

  std::string plugin_unique_id() const { return IPISFV3_UNIQUE_ID_TEXT; }
  vst_editor* create_editor() override { return new synth_vst_editor(this); }
  std::string plugin_preset_file_extension() const { return IPIS_PRESET_EXTENSION; }
  std::string default_theme_name() const override { return IPIS_DEFAULT_THEME_NAME; }
  editor_properties get_editor_properties() const override { return get_synth_editor_properties(); }

  synth_vst_controller(std::unique_ptr<inf::base::topology_info>&& topology, FUID const& processor_id):
  vst_controller(std::move(topology), processor_id) {}
};

bool 
synth_vst_controller::map_midi_control(std::int32_t number, std::int32_t& target_tag) const
{
  if (number == Steinberg::Vst::ControllerNumbers::kCtrlModWheel)
    target_tag = topology()->param_id({ part_type::master, 0 }, master_param::midi_mod_wheel);
  else if (number == Steinberg::Vst::ControllerNumbers::kCtrlVolume)
    target_tag = topology()->param_id({ part_type::master, 0 }, master_param::midi_ch_vol);
  else if (number == Steinberg::Vst::ControllerNumbers::kAfterTouch)
    target_tag = topology()->param_id({ part_type::master, 0 }, master_param::midi_ch_press);
  else if(number == Steinberg::Vst::ControllerNumbers::kPitchBend)
    target_tag = topology()->param_id({ part_type::master, 0 }, master_param::midi_pitch_bend);
  else 
    return false;
  return true;
}

static FUnknown*
create_controller(void* context)
{
  FUID fuid;
  fuid.fromString(IPISFV3_UNIQUE_ID_TEXT);
  auto topology = std::make_unique<synth_topology>(part_descriptors, part_type::count, synth_polyphony, IPISFV3_FX == 0);
  auto controller = new synth_vst_controller(std::move(topology), fuid);
  return static_cast<IEditController*>(controller);
}

static FUnknown* 
create_processor(void* context)
{
  auto topology = std::make_unique<synth_topology>(part_descriptors, part_type::count, synth_polyphony, IPISFV3_FX == 0);
  auto processor = new vst_processor(std::move(topology), FUID::fromTUID(vst_controller_id));
  return static_cast<IAudioProcessor*>(processor);
}

BEGIN_FACTORY_DEF(
  IPIS_VENDOR_NAME,
  IPIS_VENDOR_URL,
  IPIS_VENDOR_MAIL,
  2)
  DEF_CLASS(vst_processor_id, PClassInfo::kManyInstances, kVstAudioEffectClass,
    IPISFV3_NAME, Steinberg::Vst::kDistributable, IPISFV3_PLUG_TYPE,
    IPIS_VERSION, kVstVersionString, create_processor, nullptr)
  DEF_CLASS(vst_controller_id, PClassInfo::kManyInstances, kVstComponentControllerClass,
    IPISFV3_CONTROLLER_NAME, 0, "",
    IPIS_VERSION, kVstVersionString, create_controller, nullptr)
END_FACTORY
