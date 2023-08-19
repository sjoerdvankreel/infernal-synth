#ifndef INF_BASE_FORMAT_CLAP_CLAP_FACTORY_HPP
#define INF_BASE_FORMAT_CLAP_CLAP_FACTORY_HPP

#include <clap/clap.h>

namespace inf::base::format::clap 
{
extern const clap_plugin_factory_t plugin_factory;
extern const clap_plugin_descriptor_t plugin_descriptor; // This comes from the plugin project.
} // inf::base::format::clap
#endif // INF_BASE_FORMAT_CLAP_CLAP_FACTORY_HPP