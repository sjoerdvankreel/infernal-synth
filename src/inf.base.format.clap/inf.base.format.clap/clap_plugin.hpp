#ifndef INF_BASE_FORMAT_CLAP_CLAP_PLUGIN_HPP
#define INF_BASE_FORMAT_CLAP_CLAP_PLUGIN_HPP

#include <inf.base/plugin/audio_processor.hpp>
#include <inf.base/topology/topology_info.hpp>
#include <clap/clap.h>

#include <vector>
#include <cstdint>

namespace inf::base::format::clap
{
struct inf_clap_plugin 
{
	clap_plugin_t klass = {};
	clap_host_t const* host = {};
  clap_plugin_params_t params = {};
	float sample_rate = {};
  std::int32_t max_sample_count = {};
  std::unique_ptr<topology_info> topology = {};
  std::unique_ptr<audio_processor> processor = {};
  // This fully defines the audio_processor and thus the plugin state.
  std::vector<inf::base::param_value> audio_state = {};
  // Indicates whether any param index changed this block (0/1, since no vector<bool>).
  std::vector<std::int32_t> changed = {};
};

extern clap_plugin_t const plugin_class;
inline inf_clap_plugin* plugin_cast(clap_plugin const* plug)
{ return static_cast<inf_clap_plugin*>(plug->plugin_data); }

} // inf::base::format::clap
#endif // INF_BASE_FORMAT_CLAP_CLAP_PLUGIN_HPP