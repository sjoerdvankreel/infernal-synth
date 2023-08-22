#include <inf.plugin.infernal_synth/plugin.hpp>
#include <inf.plugin.infernal_synth/synth/topology.hpp>
#include <inf.plugin.infernal_synth/synth/config.hpp>

#include <inf.base.format.clap/clap_entry.hpp>
#include <inf.base.format.clap/clap_factory.hpp>

using namespace inf::plugin::infernal_synth;

static const char* features[] = { 
  CLAP_PLUGIN_FEATURE_INSTRUMENT, 
  CLAP_PLUGIN_FEATURE_SYNTHESIZER, 
  CLAP_PLUGIN_FEATURE_STEREO, nullptr };

namespace inf::base::format::clap
{

clap_plugin_descriptor_t const inf_plugin_descriptor = 
{
  .clap_version = CLAP_VERSION_INIT,
  .id = "io.github.sjoerdvankreel.infernal_synth" IPIS_UNIQUE_ID,
  .name = IPIS_NAME,
  .vendor = IPIS_VENDOR_NAME,
  .url = IPIS_VENDOR_URL,
  .manual_url = IPIS_VENDOR_URL,
  .support_url = IPIS_VENDOR_URL,
  .version = IPIS_VERSION,
  .description = IPIS_NAME,
  .features = features
};

} // namespace inf::base::format::clap

extern "C" clap_plugin_entry_t const clap_entry =
{
  .clap_version = CLAP_VERSION_INIT,
  .init = inf::base::format::clap::entry_init,
  .deinit = inf::base::format::clap::entry_deinit,
  .get_factory = inf::base::format::clap::entry_get_factory
};