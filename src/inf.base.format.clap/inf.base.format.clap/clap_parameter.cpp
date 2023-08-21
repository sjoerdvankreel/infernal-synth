#include <inf.base/shared/support.hpp>
#include <inf.base.format.clap/clap_parameter.hpp>

#include <string>
#include <cstdint>
#include <cstring>
#include <algorithm>

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

static clap_param_info_flags
param_flags(param_descriptor const* param_desc)
{
  clap_param_info_flags result = 0;
  switch (param_desc->data.kind)
  {
  case param_kind::ui:
    result = 0;
    break;
  case param_kind::block: 
  case param_kind::voice:
  case param_kind::continuous:
    result = CLAP_PARAM_IS_AUTOMATABLE;
    break;
  case param_kind::output: 
    result = CLAP_PARAM_IS_READONLY;
    break;
  case param_kind::fixed: 
    result = CLAP_PARAM_IS_READONLY | CLAP_PARAM_IS_HIDDEN;
    break;
  default: assert(false); break;
  }
  switch (param_desc->data.type)
  {  
  case param_type::list:
  case param_type::toggle:
  case param_type::list_knob:
  case param_type::knob_list:
    result |= CLAP_PARAM_IS_STEPPED;
    break;
  default:
    break;
  }
  return result;
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
  auto const& inf_info = inf_plugin->topology->params[param_index];
  // Just go with 0/1 to be the same as vst3, seems easier to do it like this.
  param_info->min_value = 0.0;
  param_info->max_value = 1.0;
  param_info->cookie = nullptr;
  param_info->flags = param_flags(inf_info.descriptor);
  param_info->id = inf_plugin->topology->param_index_to_id[param_index];
  std::string name = inf_info.descriptor->data.static_name.short_;
  std::string const& module = inf_plugin->topology->parts[inf_info.part_index].runtime_name;
  std::string full_name = module + std::string(" ") + name;
  memset(param_info->name, 0, sizeof(param_info->name));
  strncpy(param_info->name, full_name.c_str(), sizeof(param_info->name));
  memset(param_info->module, 0, sizeof(param_info->module));
  strncpy(param_info->module, module.c_str(), sizeof(param_info->module));
  param_info->default_value = param_default_to_format_normalized(inf_info);
  return true;
}

static bool CLAP_ABI
param_text_to_value(
  clap_plugin_t const* plugin, clap_id param_id,
  char const* param_value_text, double* out_value)
{
  auto inf_plugin = plugin_cast(plugin);
  std::int32_t index = inf_plugin->topology->param_id_to_index[param_id];
  auto const& inf_info = inf_plugin->topology->params[index];
  return text_to_format_normalized(inf_info, param_value_text, *out_value);
}

static bool CLAP_ABI
param_value_to_text(
  clap_plugin_t const* plugin, clap_id param_id,
  double value, char* out_buffer, std::uint32_t out_buffer_capacity)
{
  auto inf_plugin = plugin_cast(plugin);
  std::int32_t index = inf_plugin->topology->param_id_to_index[param_id];
  auto const& inf_info = inf_plugin->topology->params[index];
  double normalized = base_to_format_normalized(inf_plugin->topology.get(), index, inf_plugin->state[index]);
  std::string text = format_normalized_to_text(inf_info, normalized);
  auto text_size = std::min(static_cast<std::int32_t>(out_buffer_capacity) - 1, static_cast<std::int32_t>(text.size()));
  strncpy(out_buffer, text.data(), text_size);
  out_buffer[text_size] = '\0';
  return true;
}

} // inf::base::format::clap