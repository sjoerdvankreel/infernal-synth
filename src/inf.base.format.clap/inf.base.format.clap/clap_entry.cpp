#include <inf.base.format.clap/clap_factory.hpp>

#include <clap/clap.h>
#include <cstring>

namespace inf::base::format::clap
{

static void 
deinit() {}

static bool 
init(char const* path) 
{ return true; }

static void const* 
get_factory(char const* factory_id)
{ return strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) ? nullptr : &plugin_factory; }

} // inf::base::format::clap

extern "C" clap_plugin_entry_t const clap_entry =
{
  .clap_version = CLAP_VERSION_INIT,
  .init = inf::base::format::clap::init,
  .deinit = inf::base::format::clap::deinit,
  .get_factory = inf::base::format::clap::get_factory,
};
