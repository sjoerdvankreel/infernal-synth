#include <inf.base.format.clap/clap_entry.hpp>
#include <inf.base.format.clap/clap_plugin.hpp>
#include <cstring>

using namespace inf::base;

namespace inf::base::format::clap
{

static void plugin_reset(clap_plugin const* plugin) {}
static void plugin_destroy(clap_plugin const* plugin) {}
static void plugin_on_main_thread(clap_plugin const* plugin) {}
static void plugin_stop_processing(clap_plugin const* plugin) {}
static bool plugin_init(clap_plugin const* plugin) { return true; }
static bool plugin_start_processing(clap_plugin const* plugin) { return true; }
static void plugin_deactivate(clap_plugin const* plugin) 
{ static_cast<inf_clap_plugin*>(plugin->plugin_data)->processor.reset(); }

static void const* plugin_get_extension(
  clap_plugin const* plugin, char const* id);
static clap_process_status plugin_process(
  clap_plugin const* plugin, clap_process_t const* process);
static bool plugin_activate(
  clap_plugin const* plugin, double sample_rate, 
  std::uint32_t min_frames_count, std::uint32_t max_frames_count);

static std::uint32_t 
extension_note_ports_count(clap_plugin_t const* plugin, bool is_input) 
{ return is_input ? 1 : 0; }
static bool extension_note_ports_get(
  clap_plugin_t const* plugin, std::uint32_t index, 
  bool is_input, clap_note_port_info_t* info);

static std::uint32_t 
extension_audio_ports_count(clap_plugin_t const* plugin, bool is_input) 
{ return is_input ? 0 : 1; }
static bool extension_audio_ports_get(
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

static bool 
extension_note_ports_get(
  clap_plugin_t const* plugin, std::uint32_t index,
  bool is_input, clap_note_port_info_t* info)
{
  if (!is_input || index != 0) return false;
  info->id = 0;
  info->supported_dialects = CLAP_NOTE_DIALECT_CLAP;
  info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
  return true;
}

static bool 
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

static void const*
plugin_get_extension(clap_plugin const* plugin, char const* id)
{
  if (!strcmp(id, CLAP_EXT_NOTE_PORTS)) return &extension_note_ports;
  if (!strcmp(id, CLAP_EXT_AUDIO_PORTS)) return &extension_audio_ports;
  return nullptr;
}

static bool 
plugin_activate(
  clap_plugin const* plugin, double sample_rate, 
  std::uint32_t min_frames_count, std::uint32_t max_frames_count)
{
  auto inf_plugin = static_cast<inf_clap_plugin*>(plugin->plugin_data);
  inf_plugin->sample_rate = static_cast<float>(sample_rate);
  inf_plugin->max_sample_count = static_cast<std::int32_t>(max_frames_count);
  inf_plugin->processor = inf_plugin->topology->create_audio_processor(
    inf_plugin->state.data(), inf_plugin->changed.data(), inf_plugin->sample_rate, inf_plugin->max_sample_count);
  return true;
};

// Translate from clap note events.
static void
plugin_process_notes(clap_process_t const* process, block_input& input, std::int32_t max_note_events)
{
  for (std::uint32_t i = 0; i < process->in_events->size(process->in_events); i++)
  {
    clap_event_header_t const* header = process->in_events->get(process->in_events, i);
    if (input.note_count == max_note_events) return;
    if (header->space_id != CLAP_CORE_EVENT_SPACE_ID) continue;
    if (header->type != CLAP_EVENT_NOTE_ON && header->type != CLAP_EVENT_NOTE_OFF) continue;
    auto& note = input.notes[input.note_count++];
    clap_event_note_t const* event = reinterpret_cast<clap_event_note_t const*>(header);
    note.midi = event->key;
    note.note_on = header->type == CLAP_EVENT_NOTE_ON;
    note.velocity = static_cast<float>(event->velocity);
    note.sample_index = static_cast<std::int32_t>(header->time);
  }
}

static clap_process_status
plugin_process(clap_plugin const* plugin, clap_process_t const* process)
{
  if(process->audio_outputs_count != 1) return CLAP_PROCESS_CONTINUE;
  if(process->audio_outputs[0].channel_count != 2) return CLAP_PROCESS_CONTINUE;
  auto inf_plugin = static_cast<inf_clap_plugin*>(plugin->plugin_data);
  auto& input = inf_plugin->processor->prepare_block(static_cast<std::int32_t>(process->frames_count));
  plugin_process_notes(process, input, inf_plugin->topology->max_note_events);
  return CLAP_PROCESS_CONTINUE;
}

} // inf::base::format::clap
