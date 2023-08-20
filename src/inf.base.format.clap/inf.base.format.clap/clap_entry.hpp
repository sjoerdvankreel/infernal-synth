#ifndef INF_BASE_FORMAT_CLAP_CLAP_ENTRY_HPP
#define INF_BASE_FORMAT_CLAP_CLAP_ENTRY_HPP

#include <inf.base/topology/topology_info.hpp>
#include <inf.base.format.clap/clap_factory.hpp>
#include <clap/clap.h>

#include <cstring>
#include <memory>

namespace inf::base::format::clap
{
inline void CLAP_ABI entry_deinit() {}
inline bool CLAP_ABI entry_init(char const* path) { return true; }

inline void const* CLAP_ABI entry_get_factory(char const* factory_id)
{ return strcmp(factory_id, CLAP_PLUGIN_FACTORY_ID) ? nullptr : &inf_plugin_factory; }

// These come from the plugin project.
extern std::unique_ptr<topology_info> create_topology();
extern const clap_plugin_descriptor_t inf_plugin_descriptor;
} // inf::base::format::clap
#endif // #define INF_BASE_FORMAT_CLAP_CLAP_ENTRY_HPP