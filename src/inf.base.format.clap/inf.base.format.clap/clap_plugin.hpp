#ifndef INF_BASE_FORMAT_CLAP_CLAP_PLUGIN_HPP
#define INF_BASE_FORMAT_CLAP_CLAP_PLUGIN_HPP

#include <clap/clap.h>
#include <cstdint>

namespace inf::base::format::clap
{
struct inf_clap_plugin 
{
	clap_plugin_t plugin;
	clap_host_t const* host;
	float sample_rate;
  std::int32_t max_sample_count;
};
} // inf::base::format::clap
#endif // INF_BASE_FORMAT_CLAP_CLAP_PLUGIN_HPP