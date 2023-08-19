#include <inf.base.format.clap/clap_factory.hpp>

#include <cstdint>
#include <cstring>

namespace inf::base::format::clap
{

static std::uint32_t 
get_plugin_count(clap_plugin_factory const* factory) 
{ return 1; }

static clap_plugin_descriptor_t const* 
get_plugin_descriptor(clap_plugin_factory const* factory, std::uint32_t index) 
{ return &plugin_descriptor; }

static clap_plugin_t const*
create_plugin(clap_plugin_factory const* factory, clap_host_t const* host, char const* plugin_id)
{
  if(!clap_version_is_compatible(host->clap_version)) return nullptr;
  if(strcmp(plugin_id, plugin_descriptor.id)) return nullptr;
  return nullptr;
}

clap_plugin_factory_t const plugin_factory = 
{
  .get_plugin_count = get_plugin_count,
  .get_plugin_descriptor = get_plugin_descriptor,
  .create_plugin = create_plugin
};

} // inf::base::format::clap