#include <inf.base.format.clap/clap_entry.hpp>
#include <inf.base.format.clap/clap_factory.hpp>

#include <cstdint>
#include <cstring>

namespace inf::base::format::clap
{

static std::uint32_t 
factory_get_plugin_count(clap_plugin_factory const* factory) 
{ return 1; }

static clap_plugin_descriptor_t const* 
factory_get_plugin_descriptor(clap_plugin_factory const* factory, std::uint32_t index)
{ return &inf_plugin_descriptor; }

static clap_plugin_t const*
factory_create_plugin(clap_plugin_factory const* factory, clap_host_t const* host, char const* plugin_id)
{
  if(!clap_version_is_compatible(host->clap_version)) return nullptr;
  if(strcmp(plugin_id, inf_plugin_descriptor.id)) return nullptr;
  return nullptr;
}

clap_plugin_factory_t const inf_plugin_factory = 
{
  .get_plugin_count = factory_get_plugin_count,
  .get_plugin_descriptor = factory_get_plugin_descriptor,
  .create_plugin = factory_create_plugin
};

} // inf::base::format::clap