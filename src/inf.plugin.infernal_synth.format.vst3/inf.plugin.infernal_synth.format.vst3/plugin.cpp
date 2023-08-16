#include <inf.plugin.infernal_synth/lfo/topology.hpp>
#include <inf.plugin.infernal_synth/synth/topology.hpp>
#include <inf.plugin.infernal_synth/cv_bank/topology.hpp>
#include <inf.plugin.infernal_synth/envelope/topology.hpp>
#include <inf.plugin.infernal_synth/audio_bank/topology.hpp>
#include <inf.plugin.infernal_synth/oscillator/topology.hpp>

#include <inf.plugin.infernal_synth.ui/ui.hpp>
#include <inf.plugin.infernal_synth.format.vst3/plugin.hpp>
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

#if INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_FX
#define INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_PLUG_TYPE Steinberg::Vst::PlugType::kFx
static const DECLARE_UID(vst_processor_id, 0xD1D38026, 0x92374AB7, 0xB6FC3A55, 0xA9AE3BCC);
static const DECLARE_UID(vst_controller_id, 0x35695F63, 0x837242BE, 0x88FB88A4, 0xE4F2D1B8);
#elif (!INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_FX)
#define INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_PLUG_TYPE Steinberg::Vst::PlugType::kInstrumentSynth
static const DECLARE_UID(vst_processor_id, 0x5626A8A2, 0x47C740E3, 0x895EF722, 0xE6C1D9C4);
static const DECLARE_UID(vst_controller_id, 0x612E5225, 0xD6A44771, 0xA581057D, 0x04034620);
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
public:
  float editor_aspect_ratio() const override { return 1.629f; }
  std::int32_t editor_min_width() const override { return 1200; }
  std::int32_t editor_max_width() const override { return 2000; }
  std::int32_t editor_font_scaling_min_width() const override { return 1360; }
  std::int32_t editor_font_scaling_max_width() const override { return 1800; }
  std::vector<char const*> ui_size_names() const override { return { "XS UI", "Small UI", "Medium UI", "Large UI", "XL UI" }; }

  vst_editor* create_editor() override { return new synth_vst_editor(this); }
  bool map_midi_control(std::int32_t number, std::int32_t& target_tag) const override;

  synth_vst_controller(std::unique_ptr<inf::base::topology_info>&& topology, FUID const& processor_id):
  vst_controller(std::move(topology), processor_id) {}
};

bool 
synth_vst_controller::map_midi_control(std::int32_t number, std::int32_t& target_tag) const
{
  if (number == Steinberg::Vst::ControllerNumbers::kCtrlModWheel)
  {
    target_tag = topology()->param_id({ part_type::master, 0 }, master_param::midi_mod_wheel);
    return true;
  }
  if (number == Steinberg::Vst::ControllerNumbers::kCtrlVolume)
  {
    target_tag = topology()->param_id({ part_type::master, 0 }, master_param::midi_ch_vol);
    return true;
  }
  if (number == Steinberg::Vst::ControllerNumbers::kAfterTouch)
  {
    target_tag = topology()->param_id({ part_type::master, 0 }, master_param::midi_ch_press);
    return true;
  }
  if(number == Steinberg::Vst::ControllerNumbers::kPitchBend)
  {
    target_tag = topology()->param_id({ part_type::master, 0 }, master_param::midi_pitch_bend);
    return true;
  }
  return false;
}

class synth_vst_topology :
public synth_topology
{
public:
  synth_vst_topology(bool is_instrument): synth_topology(is_instrument) {}
  char const* plugin_name() const override { return INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_NAME; }
  char const* vendor_name() const override { return INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_VENDOR_NAME; }
  std::uint16_t version_major() const { return INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_VERSION_MAJOR; }
  std::uint16_t version_minor() const { return INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_VERSION_MINOR; }
};

static
topology_info* inf_vst_create_topology()
{ 
  topology_info* result = new synth_vst_topology(INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_FX == 0);
  topology_info::init(result, part_descriptors, part_type::count, synth_polyphony);
  return result;
}

static FUnknown*
create_controller(void* context)
{
  auto topology = std::unique_ptr<topology_info>(inf_vst_create_topology());
  auto controller = new synth_vst_controller(std::move(topology), FUID::fromTUID(vst_processor_id));
  return static_cast<IEditController*>(controller);
}

static FUnknown* 
create_processor(void* context)
{
  auto topology = std::unique_ptr<topology_info>(inf_vst_create_topology());
  auto processor = new vst_processor(std::move(topology), FUID::fromTUID(vst_controller_id));
  return static_cast<IAudioProcessor*>(processor);
}

BEGIN_FACTORY_DEF(
  INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_COMPANY_NAME,
  INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_COMPANY_WEB,
  INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_COMPANY_MAIL,
  2)
  DEF_CLASS(vst_processor_id, PClassInfo::kManyInstances, kVstAudioEffectClass,
    INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_NAME, Steinberg::Vst::kDistributable, INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_PLUG_TYPE,
    INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_VERSION, kVstVersionString, create_processor, nullptr)
  DEF_CLASS(vst_controller_id, PClassInfo::kManyInstances, kVstComponentControllerClass,
    INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_CONTROLLER_NAME, 0, "",
    INF_PLUGIN_INFERNAL_SYNTH_FORMAT_VST3_VERSION, kVstVersionString, create_controller, nullptr)
END_FACTORY
