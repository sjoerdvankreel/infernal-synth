#ifndef INF_BASE_FORMAT_CLAP_CLAP_PLUGIN_HPP
#define INF_BASE_FORMAT_CLAP_CLAP_PLUGIN_HPP

#include <inf.base/plugin/audio_processor.hpp>
#include <inf.base/topology/topology_info.hpp>
#include <inf.base.format.clap/clap_support.hpp>
#include <inf.base.format.clap/clap_controller.hpp>

#include <clap/clap.h>
#include <readerwriterqueue.h>

#include <vector>
#include <cstdint>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4324)
#endif // padding warnings

namespace inf::base::format::clap
{

struct inf_clap_plugin 
{
	clap_plugin_t klass = {};
	clap_host_t const* host = {};
  clap_plugin_gui_t editor = {};
  clap_plugin_params_t params = {};
	float sample_rate = {};
  std::int32_t max_sample_count = {};
  std::unique_ptr<topology_info> topology = {};
  std::unique_ptr<audio_processor> processor = {};
  std::unique_ptr<clap_controller> controller = {};
  std::vector<inf::base::param_value> audio_state = {};
  std::vector<std::int32_t> changed = {};

  // For async main<>audio messaging.
  moodycamel::ReaderWriterQueue<audio_to_main_msg, queue_size> audio_to_main_queue;
  inf_clap_plugin(): audio_to_main_queue() {}
};

extern clap_plugin_t const plugin_class;
inline inf_clap_plugin* plugin_cast(clap_plugin const* plug)
{ return static_cast<inf_clap_plugin*>(plug->plugin_data); }

#ifdef _MSC_VER
#pragma warning(pop)
#endif // padding warnings

} // inf::base::format::clap
#endif // INF_BASE_FORMAT_CLAP_CLAP_PLUGIN_HPP