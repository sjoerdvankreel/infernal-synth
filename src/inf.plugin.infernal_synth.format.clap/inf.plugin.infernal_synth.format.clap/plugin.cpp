#include <inf.plugin.infernal_synth/plugin.hpp>
#include <inf.plugin.infernal_synth/synth/topology.hpp>
#include <inf.plugin.infernal_synth/synth/config.hpp>
#include <inf.plugin.infernal_synth.ui/ui.hpp>

#include <inf.base.format.clap/clap_entry.hpp>
#include <inf.base.format.clap/clap_factory.hpp>
#include <inf.base.format.clap/clap_controller.hpp>

using namespace inf::base;
using namespace inf::base::format::clap;
using namespace inf::plugin::infernal_synth;
using namespace inf::plugin::infernal_synth::ui;

static const char* features[] = { 
  CLAP_PLUGIN_FEATURE_INSTRUMENT, 
  CLAP_PLUGIN_FEATURE_SYNTHESIZER, 
  CLAP_PLUGIN_FEATURE_STEREO, nullptr };

class synth_clap_controller :
public inf::base::format::clap::clap_controller
{
public:
  inf::base::editor_properties get_editor_properties() const override { return get_synth_editor_properties(); }
};

namespace inf::base::format::clap
{

clap_plugin_descriptor_t const inf_plugin_descriptor = 
{
  .clap_version = CLAP_VERSION_INIT,
  .id = "io.github.sjoerdvankreel.infernal_synth" IPIS_INST_UNIQUE_ID,
  .name = IPIS_INST_NAME,
  .vendor = IPIS_VENDOR_NAME,
  .url = IPIS_VENDOR_URL,
  .manual_url = IPIS_VENDOR_URL,
  .support_url = IPIS_VENDOR_URL,
  .version = IPIS_VERSION,
  .description = IPIS_INST_NAME,
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