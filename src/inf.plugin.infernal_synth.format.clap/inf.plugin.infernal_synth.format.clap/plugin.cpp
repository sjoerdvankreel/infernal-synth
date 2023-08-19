#include <inf.plugin.infernal_synth.format.clap/plugin.hpp>
#include <inf.plugin.infernal_synth/synth/topology.hpp>
#include <inf.plugin.infernal_synth/synth/config.hpp>
#include <inf.base.format.clap/clap_entry.hpp>
#include <inf.base.format.clap/clap_factory.hpp>

using namespace inf::plugin::infernal_synth;

static const char* features[] = { 
  CLAP_PLUGIN_FEATURE_INSTRUMENT, 
  CLAP_PLUGIN_FEATURE_SYNTHESIZER, 
  CLAP_PLUGIN_FEATURE_STEREO, nullptr };

class synth_clap_topology :
public synth_topology
{
public:
  synth_clap_topology(): synth_topology(true) {}
  char const* plugin_name() const override { return INF_PLUGIN_INFERNAL_SYNTH_FORMAT_CLAP_NAME; }
  char const* vendor_name() const override { return INF_PLUGIN_INFERNAL_SYNTH_FORMAT_CLAP_VENDOR; }
  std::uint16_t version_major() const { return INF_PLUGIN_INFERNAL_SYNTH_FORMAT_CLAP_VERSION_MAJOR; }
  std::uint16_t version_minor() const { return INF_PLUGIN_INFERNAL_SYNTH_FORMAT_CLAP_VERSION_MINOR; }
};

namespace inf::base::format::clap
{

std::unique_ptr<topology_info> create_topology()
{
  auto result = std::make_unique<synth_clap_topology>();
  topology_info::init(result.get(), part_descriptors, part_type::count, synth_polyphony);
  return result;
}

clap_plugin_descriptor_t const inf_plugin_descriptor = 
{
  .clap_version = CLAP_VERSION_INIT,
  .id = INF_PLUGIN_INFERNAL_SYNTH_FORMAT_CLAP_ID,
  .name = INF_PLUGIN_INFERNAL_SYNTH_FORMAT_CLAP_NAME,
  .vendor = INF_PLUGIN_INFERNAL_SYNTH_FORMAT_CLAP_VENDOR,
  .url = INF_PLUGIN_INFERNAL_SYNTH_FORMAT_CLAP_URL,
  .manual_url = INF_PLUGIN_INFERNAL_SYNTH_FORMAT_CLAP_URL,
  .support_url = INF_PLUGIN_INFERNAL_SYNTH_FORMAT_CLAP_URL,
  .version = INF_PLUGIN_INFERNAL_SYNTH_FORMAT_CLAP_VERSION,
  .description = INF_PLUGIN_INFERNAL_SYNTH_FORMAT_CLAP_NAME,
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