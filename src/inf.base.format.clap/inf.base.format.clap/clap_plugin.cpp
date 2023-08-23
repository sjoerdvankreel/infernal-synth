#include <inf.base/shared/support.hpp>
#include <inf.base.format.clap/clap_entry.hpp>
#include <inf.base.format.clap/clap_plugin.hpp>
#include <inf.base.format.clap/clap_parameter.hpp>

#include <clap/clap.h>
#include <cstring>

using namespace inf::base;

namespace inf::base::format::clap
{

static void CLAP_ABI plugin_reset(clap_plugin const* plugin) {}
static void CLAP_ABI plugin_destroy(clap_plugin const* plugin) {}
static void CLAP_ABI plugin_on_main_thread(clap_plugin const* plugin) {}
static void CLAP_ABI plugin_stop_processing(clap_plugin const* plugin) {}
static bool CLAP_ABI plugin_init(clap_plugin const* plugin) { return true; }
static bool CLAP_ABI plugin_start_processing(clap_plugin const* plugin) { return true; }
static void CLAP_ABI plugin_deactivate(clap_plugin const* plugin)
{ plugin_cast(plugin)->processor.reset(); }

static void const* CLAP_ABI plugin_get_extension(
  clap_plugin const* plugin, char const* id);
static clap_process_status CLAP_ABI plugin_process(
  clap_plugin const* plugin, clap_process_t const* process);
static bool CLAP_ABI plugin_activate(
  clap_plugin const* plugin, double sample_rate, 
  std::uint32_t min_frames_count, std::uint32_t max_frames_count);

static std::uint32_t CLAP_ABI
extension_note_ports_count(clap_plugin_t const* plugin, bool is_input) 
{ return is_input ? 1 : 0; }
static bool CLAP_ABI extension_note_ports_get(
  clap_plugin_t const* plugin, std::uint32_t index, 
  bool is_input, clap_note_port_info_t* info);

static std::uint32_t CLAP_ABI
extension_audio_ports_count(clap_plugin_t const* plugin, bool is_input) 
{ return is_input ? 0 : 1; }
static bool CLAP_ABI extension_audio_ports_get(
  clap_plugin_t const* plugin, std::uint32_t index, 
  bool is_input, clap_audio_port_info_t* info);

const clap_plugin_t plugin_class =
{
  .desc = &inf_plugin_descriptor,
  .plugin_data = nullptr,
  .init = plugin_init,
  .destroy = plugin_destroy,
  .activate = plugin_activate,
  .deactivate = plugin_deactivate,
  .start_processing = plugin_start_processing,
  .stop_processing = plugin_stop_processing,
  .reset = plugin_reset,
  .process = plugin_process,
  .get_extension = plugin_get_extension,
  .on_main_thread = plugin_on_main_thread
};

static const clap_plugin_note_ports_t extension_note_ports = 
{
  .count = extension_note_ports_count,
  .get = extension_note_ports_get
};

static const clap_plugin_audio_ports_t extension_audio_ports = 
{
  .count = extension_audio_ports_count,
  .get = extension_audio_ports_get
};

static bool CLAP_ABI
extension_note_ports_get(
  clap_plugin_t const* plugin, std::uint32_t index,
  bool is_input, clap_note_port_info_t* info)
{
  if (!is_input || index != 0) return false;
  info->id = 0;
  info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
  info->supported_dialects = CLAP_NOTE_DIALECT_CLAP;
  return true;
}

static bool CLAP_ABI
extension_audio_ports_get(
  clap_plugin_t const* plugin, std::uint32_t index,
  bool is_input, clap_audio_port_info_t* info)
{
  if (is_input || index != 0) return false;
  info->id = 0;
  info->channel_count = 2;
  info->port_type = CLAP_PORT_STEREO;
  info->in_place_pair = CLAP_INVALID_ID;
  info->flags = CLAP_AUDIO_PORT_IS_MAIN;
  return true;
}

static void const* CLAP_ABI
plugin_get_extension(clap_plugin const* plugin, char const* id)
{
  if (!strcmp(id, CLAP_EXT_NOTE_PORTS)) return &extension_note_ports;
  if (!strcmp(id, CLAP_EXT_AUDIO_PORTS)) return &extension_audio_ports;
  if (!strcmp(id, CLAP_EXT_GUI)) return &static_cast<inf_clap_plugin*>(plugin->plugin_data)->editor;
  if (!strcmp(id, CLAP_EXT_PARAMS)) return &static_cast<inf_clap_plugin*>(plugin->plugin_data)->params;
  return nullptr;
}

static bool CLAP_ABI
plugin_activate(
  clap_plugin const* plugin, double sample_rate, 
  std::uint32_t min_frames_count, std::uint32_t max_frames_count)
{
  auto inf_plugin = plugin_cast(plugin);
  inf_plugin->sample_rate = static_cast<float>(sample_rate);
  inf_plugin->max_sample_count = static_cast<std::int32_t>(max_frames_count);
  inf_plugin->processor = inf_plugin->topology->create_audio_processor(
    inf_plugin->audio_state.data(), inf_plugin->changed.data(), inf_plugin->sample_rate, inf_plugin->max_sample_count);
  return true;
};

// Translate from clap events.
static void
plugin_process_events(
  inf_clap_plugin* plugin, clap_process_t const* process, 
  block_input& input, std::int32_t max_note_events)
{
  // Note events.
  for (std::uint32_t e = 0; e < process->in_events->size(process->in_events); e++)
  {
    clap_event_header_t const* header = process->in_events->get(process->in_events, e);
    if (input.note_count == max_note_events) return;
    if (header->space_id != CLAP_CORE_EVENT_SPACE_ID) continue;
    if (header->type == CLAP_EVENT_NOTE_ON || header->type == CLAP_EVENT_NOTE_OFF)
    {
      auto& note = input.notes[input.note_count++];
      auto event = reinterpret_cast<clap_event_note_t const*>(header);
      note.midi = event->key;
      note.note_on = header->type == CLAP_EVENT_NOTE_ON;
      note.velocity = static_cast<float>(event->velocity);
      note.sample_index = static_cast<std::int32_t>(header->time);
    }
  }  

  // TODO handle _changed
  for (std::uint32_t e = 0; e < process->in_events->size(process->in_events); e++)
  {
    clap_event_header_t const* header = process->in_events->get(process->in_events, e);
    if (header->space_id != CLAP_CORE_EVENT_SPACE_ID) continue;
    if (header->type != CLAP_EVENT_PARAM_VALUE) continue; 
    auto event = reinterpret_cast<clap_event_param_value const*>(header);
    auto index = plugin->topology->param_id_to_index[event->param_id];

    // Push stuff to the ui.
    // Note that we push stuff "ahead of time" but i really
    // dont want to break into the internal audio loop for this.
    // Continuous-valued automation streams will still work as expected
    // (parameters change at exact sample position) just the ui runs a bit ahead.
    audio_to_main_msg msg;
    msg.index = index;
    msg.value = event->value;
    plugin->audio_to_main_queue.try_enqueue(msg);

    // Discrete automation events - effectively we only pick up the last value.
    if(plugin->topology->params[index].descriptor->data.is_continuous()) continue;
    plugin->audio_state[index] = format_normalized_to_base(plugin->topology.get(), false, index, event->value);
  }

  // TODO handle _changed + broadcast ui changes
  // Continuous automation events - build up the curve. TODO interpolation.
  for(std::int32_t s = 0; s < input.data.sample_count; s++)
    for (std::uint32_t e = 0; e < process->in_events->size(process->in_events); e++)
    {
      clap_event_header_t const* header = process->in_events->get(process->in_events, e);
      if (header->space_id != CLAP_CORE_EVENT_SPACE_ID) continue;
      if (header->type != CLAP_EVENT_PARAM_VALUE) continue;
      auto event = reinterpret_cast<clap_event_param_value const*>(header);
      auto index = plugin->topology->param_id_to_index[event->param_id];
      if (!plugin->topology->params[index].descriptor->data.is_continuous()) continue;

      // Just make a "bumpy" curve for now.
      if(header->time == static_cast<std::uint32_t>(s))
        plugin->audio_state[index] = format_normalized_to_base(plugin->topology.get(), false, index, event->value);
      input.continuous_automation_raw[index][s] = plugin->audio_state[index].real;
    }
}

static clap_process_status CLAP_ABI
plugin_process(clap_plugin const* plugin, clap_process_t const* process)
{
  if(process->audio_outputs_count != 1) return CLAP_PROCESS_CONTINUE;
  if(process->audio_outputs[0].channel_count != 2) return CLAP_PROCESS_CONTINUE;  

  auto inf_plugin = plugin_cast(plugin);
  auto& input = inf_plugin->processor->prepare_block(static_cast<std::int32_t>(process->frames_count));
  plugin_process_events(inf_plugin, process, input, inf_plugin->topology->max_note_events);

  input.data.bpm = 0.0f;
  input.data.stream_position = process->steady_time;
  input.data.sample_count = static_cast<std::int32_t>(process->frames_count);
  if(process->transport != nullptr) input.data.bpm = static_cast<float>(process->transport->tempo);

  inf_plugin->processor->process(nullptr, process->audio_outputs[0].data32, false, 0, 0);
  return CLAP_PROCESS_CONTINUE;
}

} // inf::base::format::clap
