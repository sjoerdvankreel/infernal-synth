#include <inf.base.format.clap/clap_entry.hpp>
#include <inf.base.format.clap/clap_factory.hpp>

static const char* features[] = { 
  CLAP_PLUGIN_FEATURE_INSTRUMENT, 
  CLAP_PLUGIN_FEATURE_SYNTHESIZER, 
  CLAP_PLUGIN_FEATURE_STEREO, nullptr };

namespace inf::base::format::clap
{

clap_plugin_descriptor_t const inf_plugin_descriptor = 
{
  .clap_version = CLAP_VERSION_INIT,
  .id = "sjoerdvankreel.infernal_synth",
  .name = "Infernal Synth",
  .vendor = "Sjoerd van Kreel",
  .url = "https://sjoerdvankreel.github.io/infernal-synth/",
  .manual_url = "https://sjoerdvankreel.github.io/infernal-synth/",
  .support_url = "https://sjoerdvankreel.github.io/infernal-synth/",
  .version = "1.3",
  .description = "Infernal Synth",
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