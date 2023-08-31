#include <inf.base/shared/support.hpp>
#include <inf.base.format.clap/clap_plugin.hpp>
#include <inf.base.format.clap/clap_parameter.hpp>

#include <clap/clap.h>
#include <string>
#include <cstdint>
#include <cstring>
#include <algorithm>

namespace inf::base::format::clap
{

static bool CLAP_ABI state_save(clap_plugin_t const* plugin, clap_ostream_t const* stream);
static bool CLAP_ABI state_load(clap_plugin_t const* plugin, clap_istream_t const* stream);

static std::uint32_t CLAP_ABI param_count(clap_plugin_t const* plugin);
static bool CLAP_ABI param_get_value(clap_plugin_t const* plugin, clap_id param_id, double* out_value);
static bool CLAP_ABI param_get_info(clap_plugin_t const* plugin, std::uint32_t param_index, clap_param_info_t* param_info);
static void CLAP_ABI param_flush(clap_plugin_t const* plugin, clap_input_events_t const* in, clap_output_events_t const* out);
static bool CLAP_ABI param_text_to_value(clap_plugin_t const* plugin, clap_id param_id, char const* param_value_text, double* out_value);
static bool CLAP_ABI param_value_to_text(clap_plugin_t const* plugin, clap_id param_id, double value, char* out_buffer, std::uint32_t out_buffer_capacity);

void
plugin_init_state_api(inf_clap_plugin* plugin)
{
  plugin->state.load = state_load;
  plugin->state.save = state_save;
}

void
plugin_init_params_api(inf_clap_plugin* plugin)
{
  plugin->params.count = param_count;
  plugin->params.flush = param_flush;
  plugin->params.get_info = param_get_info;
  plugin->params.get_value = param_get_value;
  plugin->params.text_to_value = param_text_to_value;
  plugin->params.value_to_text = param_value_to_text;
}

static bool CLAP_ABI 
state_save(clap_plugin_t const* plugin, clap_ostream_t const* stream)
{
  std::vector<std::uint8_t> data;
  if(!plugin_cast(plugin)->controller->save_plugin_preset(data)) return false;
  for(std::size_t i = 0; i < data.size(); i++)
    if(stream->write(stream, &data[i], 1) != 1) return false;
  return true;
}

static bool CLAP_ABI 
state_load(clap_plugin_t const* plugin, clap_istream_t const* stream)
{
  std::uint8_t byte;
  std::vector<std::uint8_t> data;
  while (true)
  {
    auto result = stream->read(stream, &byte, 1);
    if(result == 0) break;
    if(result < 0) return false;
    data.push_back(byte);
  }
  if(!plugin_cast(plugin)->controller->load_plugin_preset(data)) return false;
  return true;
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
  *out_value = base_to_format_normalized(inf_plugin->topology.get(), false, index, inf_plugin->audio_state[index]);
  return true;
}

static bool CLAP_ABI 
param_get_info(clap_plugin_t const* plugin, std::uint32_t param_index, clap_param_info_t* param_info)
{
  auto inf_plugin = plugin_cast(plugin);
  auto const& inf_info = inf_plugin->topology->params[param_index];
  param_info->cookie = nullptr;
  param_info->flags = param_flags(inf_info.descriptor);
  param_info->id = inf_plugin->topology->param_index_to_id[param_index];

  // Just go with 0/1 to be the same as vst3 for real-valued, seems easier to do it like this.
  // We still need to supply the full range for discrete values as clap doesn't do normalization for stepped params.
  bool is_real = inf_info.descriptor->data.type == param_type::real;
  param_info->min_value = is_real ? 0.0f: inf_info.descriptor->data.discrete.min;
  param_info->max_value = is_real ? 1.0f : inf_info.descriptor->data.discrete.max;
  param_info->default_value = param_default_to_format_normalized(inf_info, false);

  std::string const& module = inf_plugin->topology->parts[inf_info.part_index].runtime_name;
  memset(param_info->name, 0, sizeof(param_info->name));
  strncpy(param_info->name, inf_info.runtime_name.c_str(), sizeof(param_info->name));
  memset(param_info->module, 0, sizeof(param_info->module));
  strncpy(param_info->module, module.c_str(), sizeof(param_info->module));
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
  return text_to_format_normalized(inf_info, false, param_value_text, *out_value);
}

static bool CLAP_ABI
param_value_to_text(
  clap_plugin_t const* plugin, clap_id param_id,
  double value, char* out_buffer, std::uint32_t out_buffer_capacity)
{
  auto inf_plugin = plugin_cast(plugin);
  std::int32_t index = inf_plugin->topology->param_id_to_index[param_id];
  auto const& inf_info = inf_plugin->topology->params[index];
  std::string text = format_normalized_to_text(inf_info, false, value);
  auto text_size = std::min(static_cast<std::int32_t>(out_buffer_capacity) - 1, static_cast<std::int32_t>(text.size()));
  strncpy(out_buffer, text.data(), text_size);
  out_buffer[text_size] = '\0';
  return true;
}

static void CLAP_ABI 
param_flush(
  clap_plugin_t const* plugin,
  clap_input_events_t const* in,
  clap_output_events_t const* out)
{
  bool ok;
  (void)ok;

  auto inf_plugin = plugin_cast(plugin);
  for (std::uint32_t i = 0; i < in->size(in); i++)
  {    
    auto header = in->get(in, i);
    if (header->space_id != CLAP_CORE_EVENT_SPACE_ID) continue;
    if (header->type != CLAP_EVENT_PARAM_VALUE) continue;

    // Same as in the process() call, but without the discrete/continuous distinction.
    auto event = reinterpret_cast<clap_event_param_value const*>(header);
    auto index = inf_plugin->topology->param_id_to_index[event->param_id];
    inf_plugin->audio_state[index] = format_normalized_to_base(inf_plugin->topology.get(), false, index, event->value);

    audio_to_main_msg msg;
    msg.index = index;
    msg.value = event->value;
    ok = inf_plugin->audio_to_main_queue.try_enqueue(msg);
    assert(ok);
  }

  inf_plugin->process_ui_queue(out);
}

} // inf::base::format::clap