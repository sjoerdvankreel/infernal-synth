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
  info->supported_dialects = CLAP_NOTE_DIALECT_CLAP | CLAP_NOTE_DIALECT_MIDI;
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
  if (!strcmp(id, CLAP_EXT_STATE)) return &static_cast<inf_clap_plugin*>(plugin->plugin_data)->state;
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
  bool ok;
  std::uint8_t midi_note_on = 0x90;
  std::uint8_t midi_note_off = 0x80;
  std::uint32_t event_count = process->in_events->size(process->in_events);
  (void)ok;

  // Point-in-time events.
  for (std::uint32_t e = 0; e < event_count; e++)
  {
    clap_event_header_t const* header = process->in_events->get(process->in_events, e);
    if (header->space_id != CLAP_CORE_EVENT_SPACE_ID) continue;

    // CLAP notes.
    if (header->type == CLAP_EVENT_NOTE_ON || header->type == CLAP_EVENT_NOTE_OFF)
    {
      if (input.note_count < max_note_events)
      {
        auto& note = input.notes[input.note_count++];
        auto event = reinterpret_cast<clap_event_note_t const*>(header);
        note.midi = event->key;
        note.note_on = header->type == CLAP_EVENT_NOTE_ON;
        note.velocity = static_cast<float>(event->velocity);
        note.sample_index = static_cast<std::int32_t>(header->time);
      }
    } 

    // MIDI notes.
    // Note: need to disable DIALECT_CLAP and recompile to actually test this 
    // (f.e. using reaper virtual midi keyboard). Otherwise they get translated to CLAP_EVENT_NOTE_*.
    else if (header->type == CLAP_EVENT_MIDI)
    {
      auto event = reinterpret_cast<clap_event_midi const*>(header);
      if (event->port_index == 0)
      {
        std::uint8_t msg = event->data[0] & 0xF0;
        if(msg == midi_note_on || msg == midi_note_off)
        {
          if (input.note_count < max_note_events)
          {
            auto& note = input.notes[input.note_count++];
            note.midi = event->data[1];
            note.note_on = msg == midi_note_on;
            note.velocity = event->data[2] / 127.0f;
            note.sample_index = static_cast<std::int32_t>(header->time);
          }
        }
      }
    }

    // Regular automation.
    else if (header->type == CLAP_EVENT_PARAM_VALUE)
    {
      // Push stuff to the ui.
      // Note that we push stuff "ahead of time" but i really
      // dont want to break into the internal audio loop for this.
      // Continuous-valued automation streams will still work as expected
      // (parameters change at exact sample position) just the ui runs a bit ahead.
      audio_to_main_msg msg;
      auto event = reinterpret_cast<clap_event_param_value const*>(header);
      auto index = plugin->topology->param_id_to_index[event->param_id];
      msg.index = index;
      msg.value = event->value;
      ok = plugin->audio_to_main_queue.try_enqueue(msg);
      assert(ok);

      // For discrete automation events effectively we only pick up the last value.
      if (!plugin->topology->params[index].descriptor->data.is_continuous())
        plugin->audio_state[index] = format_normalized_to_base(plugin->topology.get(), false, index, event->value);
    }
  }

  // TODO handle _changed
  // Continuous automation events - build up the curve. TODO interpolation.
  for (std::int32_t s = 0; s < input.data.sample_count; s++)
    for (std::uint32_t e = 0; e < event_count; e++)
    {
      clap_event_header_t const* header = process->in_events->get(process->in_events, e);
      if (header->space_id != CLAP_CORE_EVENT_SPACE_ID) continue;

      // Regular automation.
      if (header->type == CLAP_EVENT_PARAM_VALUE)
      {
        auto event = reinterpret_cast<clap_event_param_value const*>(header);
        auto index = plugin->topology->param_id_to_index[event->param_id];
        if(plugin->topology->params[index].descriptor->data.is_continuous())
        {
          // Just make a "bumpy" curve for now.
          if (header->time == static_cast<std::uint32_t>(s))
            plugin->audio_state[index] = format_normalized_to_base(plugin->topology.get(), false, index, event->value);
          input.continuous_automation_raw[index][s] = plugin->audio_state[index].real;
        }
      }

      // Raw midi cc messages (14 bit values).
      else if (header->type == CLAP_EVENT_MIDI)
      {
        auto event = reinterpret_cast<clap_event_midi const*>(header);
        if (event->port_index == 0)
        {
          std::uint8_t msg = event->data[0] & 0xF0;
          auto iter = plugin->midi_map.find(msg);
          if (iter != plugin->midi_map.end())
          {
            auto param_index = plugin->topology->param_id_to_index[iter->second];
            if (plugin->topology->params[param_index].descriptor->data.is_continuous())
            {
              // Just make a "bumpy" curve for now.
              if (header->time == static_cast<std::uint32_t>(s))
              {
                // Mapping to 0..1, plug implementation should scale back.
                std::uint16_t val = (event->data[2] << 7) | event->data[1];
                float normalized = static_cast<float>(val) / static_cast<float>(1U << 14);
                plugin->audio_state[param_index] = format_normalized_to_base(plugin->topology.get(), false, param_index, normalized);
              }
              input.continuous_automation_raw[param_index][s] = plugin->audio_state[param_index].real;
            }
          }
        }
      }
    }
}

static clap_process_status CLAP_ABI
plugin_process(clap_plugin const* plugin, clap_process_t const* process)
{
  bool ok;
  (void)ok;
  if(process->audio_outputs_count != 1) return CLAP_PROCESS_CONTINUE;
  if(process->audio_outputs[0].channel_count != 2) return CLAP_PROCESS_CONTINUE;  

  // For timing.
  std::int64_t new_start_perf_count = performance_counter();

  // Process incoming events.
  auto inf_plugin = plugin_cast(plugin);
  inf_plugin->process_ui_queue(process->out_events);
  auto& input = inf_plugin->processor->prepare_block(static_cast<std::int32_t>(process->frames_count));
  plugin_process_events(inf_plugin, process, input, inf_plugin->topology->max_note_events);

  // Run the main loop.
  input.data.bpm = 0.0f;
  input.data.stream_position = process->steady_time;
  input.data.sample_count = static_cast<std::int32_t>(process->frames_count);
  if(process->transport != nullptr) input.data.bpm = static_cast<float>(process->transport->tempo);
  auto const& output = inf_plugin->processor->process(
    nullptr, process->audio_outputs[0].data32, false, 
    inf_plugin->prev_end_perf_count, new_start_perf_count);

  // Push output params to the ui.
  auto now = std::chrono::system_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - inf_plugin->output_updated);
  if (duration.count() >= output_param_update_msec)
  {
    inf_plugin->output_updated = now;
    for (std::int32_t i = 0; i < inf_plugin->topology->output_param_count; i++)
    {
      audio_to_main_msg msg;
      msg.index = inf_plugin->topology->input_param_count + i;
      msg.value = base_to_format_normalized(inf_plugin->topology.get(), false, msg.index, output.block_automation_raw[msg.index]);
      ok = inf_plugin->audio_to_main_queue.try_enqueue(msg);
      assert(ok);
    }
  }

  // For timing.
  inf_plugin->prev_end_perf_count = performance_counter();
  return CLAP_PROCESS_CONTINUE;
}

void
inf_clap_plugin::process_ui_queue(clap_output_events_t const* ov)
{
  main_to_audio_msg msg;
  while (main_to_audio_queue.try_dequeue(msg))
    switch (msg.type)
    {
    case main_to_audio_msg::begin_edit:
    case main_to_audio_msg::end_edit:
    {
      auto evt = clap_event_param_gesture();
      evt.header.time = 0;
      evt.header.flags = 0;
      evt.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
      evt.header.size = sizeof(clap_event_param_gesture);
      evt.param_id = topology->param_index_to_id[msg.index];
      evt.header.type = static_cast<std::uint16_t>(
        msg.type == main_to_audio_msg::begin_edit 
        ? CLAP_EVENT_PARAM_GESTURE_BEGIN 
        : CLAP_EVENT_PARAM_GESTURE_END);
      ov->try_push(ov, &evt.header);
      break;
    }
    case main_to_audio_msg::adjust_value:
    {
      audio_state[msg.index] = format_normalized_to_base(topology.get(), false, msg.index, msg.value);
      auto evt = clap_event_param_value();
      evt.header.time = 0;
      evt.header.flags = 0;
      evt.value = msg.value;
      evt.header.type = CLAP_EVENT_PARAM_VALUE;
      evt.header.space_id = CLAP_CORE_EVENT_SPACE_ID;
      evt.header.size = sizeof(clap_event_param_value);
      evt.param_id = topology->param_index_to_id[msg.index];
      ov->try_push(ov, &(evt.header));
    }
    }
}

} // inf::base::format::clap
