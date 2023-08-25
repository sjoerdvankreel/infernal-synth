#include <inf.plugin.infernal_synth/plugin.hpp>
#include <inf.plugin.infernal_synth/synth/topology.hpp>
#include <inf.plugin.infernal_synth/synth/config.hpp>
#include <inf.plugin.infernal_synth.ui/ui.hpp>

#include <inf.base.format.clap/clap_entry.hpp>
#include <inf.base.format.clap/clap_support.hpp>
#include <inf.base.format.clap/clap_factory.hpp>
#include <inf.base.format.clap/clap_controller.hpp>

#include <clap/clap.h>

using namespace inf::base;
using namespace inf::base::ui;
using namespace inf::base::format::clap;
using namespace inf::plugin::infernal_synth;
using namespace inf::plugin::infernal_synth::ui;

#if IPISFCLAP_FX
#define IPISFCLAP_NAME IPIS_FX_NAME
#define IPISFCLAP_UNIQUE_ID_TEXT IPIS_FX_UNIQUE_ID_TEXT
#define IPISFCLAP_FEATURE CLAP_PLUGIN_FEATURE_AUDIO_EFFECT
#elif (!IPISFCLAP_FX)
#define IPISFCLAP_NAME IPIS_INST_NAME
#define IPISFCLAP_UNIQUE_ID_TEXT IPIS_INST_UNIQUE_ID_TEXT
#define IPISFCLAP_FEATURE CLAP_PLUGIN_FEATURE_INSTRUMENT
#else
#error
#endif

// Select fx/instrument.
static const char* 
features[] = { IPISFCLAP_FEATURE, CLAP_PLUGIN_FEATURE_STEREO, nullptr };

class synth_clap_controller :
public inf::base::format::clap::clap_controller
{
  std::set<std::int32_t> midi_is_coarse() const override;
  std::map<std::int32_t, std::int32_t> map_midi_controls() const override;
  std::string plugin_unique_id() const { return IPISFCLAP_UNIQUE_ID_TEXT; }
  std::string plugin_preset_file_extension() const { return IPIS_PRESET_EXTENSION; }
  std::string default_theme_name() const override { return IPIS_DEFAULT_THEME_NAME; }
  std::unique_ptr<root_element> create_ui() override { return create_synth_ui(this); }
  inf::base::editor_properties get_editor_properties() const override { return get_synth_editor_properties(); }
};

std::set<std::int32_t>
synth_clap_controller::midi_is_coarse() const
{
  std::set<std::int32_t> result;
  result.insert(midi_cc_channel_pressure);
  return result;
}

std::map<std::int32_t, std::int32_t> 
synth_clap_controller::map_midi_controls() const
{
  std::map<std::int32_t, std::int32_t> result = {};
  result[midi_cc_mod_wheel] = topology()->param_id({ part_type::master, 0 }, master_param::midi_mod_wheel);
  result[midi_cc_channel_volume] = topology()->param_id({ part_type::master, 0 }, master_param::midi_ch_vol);
  result[midi_cc_channel_pressure] = topology()->param_id({ part_type::master, 0 }, master_param::midi_ch_press);
  result[midi_cc_pitch_bend] = topology()->param_id({ part_type::master, 0 }, master_param::midi_pitch_bend);
  return result;
}

namespace inf::base::format::clap
{

clap_plugin_descriptor_t const inf_plugin_descriptor = 
{
  .clap_version = CLAP_VERSION_INIT,
  .id = "io.github.sjoerdvankreel.infernal_synth" IPISFCLAP_UNIQUE_ID_TEXT,
  .name = IPISFCLAP_NAME,
  .vendor = IPIS_VENDOR_NAME,
  .url = IPIS_VENDOR_URL,
  .manual_url = IPIS_VENDOR_URL,
  .support_url = IPIS_VENDOR_URL,
  .version = IPIS_VERSION,
  .description = IPISFCLAP_NAME,
  .features = features
};

std::unique_ptr<clap_controller> create_controller()
{ return std::make_unique<synth_clap_controller>(); }
std::unique_ptr<topology_info> create_topology()
{ return std::make_unique<synth_topology>(part_descriptors, part_type::count, synth_polyphony, IPISFCLAP_FX == 0); }

} // namespace inf::base::format::clap

extern "C" clap_plugin_entry_t const clap_entry =
{
  .clap_version = CLAP_VERSION_INIT,
  .init = inf::base::format::clap::entry_init,
  .deinit = inf::base::format::clap::entry_deinit,
  .get_factory = inf::base::format::clap::entry_get_factory
};