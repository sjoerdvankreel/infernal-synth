#include <inf.base/shared/support.hpp>
#include <inf.base.format.clap/clap_parameter.hpp>

#include <cstdint>
#include <cstring>

namespace inf::base::format::clap
{

static std::uint32_t CLAP_ABI param_count(clap_plugin_t const* plugin);
static bool CLAP_ABI param_get_value(clap_plugin_t const* plugin, clap_id param_id, double* out_value);
static bool CLAP_ABI param_get_info(clap_plugin_t const* plugin, std::uint32_t param_index, clap_param_info_t* param_info);
static bool CLAP_ABI param_text_to_value(clap_plugin_t const* plugin, clap_id param_id, char const* param_value_text, double* out_value);
static void CLAP_ABI param_flush(clap_plugin_t const* plugin, clap_input_events_t const* in, clap_output_events_t const* out) { /* TODO */ }
static bool CLAP_ABI param_value_to_text(clap_plugin_t const* plugin, clap_id param_id, double value, char* out_buffer, std::uint32_t out_buffer_capacity);

void 
plugin_init_params(inf_clap_plugin* plugin)
{
  plugin->params.count = param_count;
  plugin->params.flush = param_flush;
  plugin->params.get_info = param_get_info;
  plugin->params.get_value = param_get_value;
  plugin->params.text_to_value = param_text_to_value;
  plugin->params.value_to_text = param_value_to_text;
}

static std::uint32_t CLAP_ABI 
param_count(clap_plugin_t const* plugin)
{ return static_cast<std::uint32_t>(plugin_cast(plugin)->topology->params.size()); }

static bool CLAP_ABI 
param_get_value(clap_plugin_t const* plugin, clap_id param_id, double* out_value)
{
  auto inf_plugin = plugin_cast(plugin);
  std::int32_t index = inf_plugin->topology->param_id_to_index[param_id];
  *out_value = base_to_format_normalized(inf_plugin->topology.get(), index, inf_plugin->state[index]);
  return true;
}

static bool CLAP_ABI 
param_get_info(clap_plugin_t const* plugin, std::uint32_t param_index, clap_param_info_t* param_info)
{
  auto inf_plugin = plugin_cast(plugin);
  auto topo = inf_plugin->topology.get();
  param_info->cookie = nullptr;
  param_info->id = inf_plugin->topology->param_index_to_id[param_index];
  param_info->flags = 
  memset(param_info->name, 0, sizeof(param_info->name));
  strncpy(param_info->name, inf_plugin->topology->params[param_index].descriptor->data.static_name.short_, sizeof(param_info->name));
  memset(param_info->module, 0, sizeof(param_info->module));
  strncpy(param_info->name, inf_plugin->topology->parts[inf_plugin->topology->params[param_index].part_index].runtime_name.c_str(), sizeof(param_info->module));
}

static bool CLAP_ABI 
param_text_to_value(clap_plugin_t const* plugin, clap_id param_id, char const* param_value_text, double* out_value)
{
}

static bool CLAP_ABI 
param_value_to_text(clap_plugin_t const* plugin, clap_id param_id, double value, char* out_buffer, std::uint32_t out_buffer_capacity)
{
}

} // inf::base::format::clap