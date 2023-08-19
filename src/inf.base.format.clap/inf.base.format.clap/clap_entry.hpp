#ifndef INF_BASE_FORMAT_CLAP_CLAP_ENTRY_HPP
#define INF_BASE_FORMAT_CLAP_CLAP_ENTRY_HPP

#include <inf.base.format.clap/clap_factory.hpp>
#include <clap/clap.h>
#include <cstring>

namespace inf::base::format::clap
{
inline void entry_deinit() {}
inline bool entry_init(char const* path) { return true; }

inline void const* entry_get_factory(char const* factory_id)
{ return strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) ? nullptr : &inf_plugin_factory; }

extern const clap_plugin_descriptor_t inf_plugin_descriptor; // This comes from the plugin project.
} // inf::base::format::clap
#endif // #define INF_BASE_FORMAT_CLAP_CLAP_ENTRY_HPP