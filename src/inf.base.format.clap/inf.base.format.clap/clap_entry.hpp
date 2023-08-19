#include <inf.base.format.clap/clap_factory.hpp>

#include <clap/clap.h>
#include <cstring>

namespace inf::base::format::clap
{
inline void deinit() {}
inline bool init(char const* path) { return true; }
inline void const* get_factory(char const* factory_id)
{ return strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) ? nullptr : &plugin_factory; }

} // inf::base::format::clap
