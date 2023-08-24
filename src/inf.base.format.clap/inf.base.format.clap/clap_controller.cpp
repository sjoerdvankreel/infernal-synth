#include <inf.base/shared/support.hpp>
#include <inf.base.format.clap/clap_entry.hpp>
#include <inf.base.format.clap/clap_plugin.hpp>
#include <inf.base.format.clap/clap_io_stream.hpp>
#include <inf.base.format.clap/clap_controller.hpp>

#include <clap/clap.h>

#include <string>
#include <fstream>
#include <filesystem>

using namespace inf::base;
using namespace inf::base::ui;

namespace inf::base::format::clap
{

static char clap_file_magic[] = "{CEAE4474-EABA-46C2-B0AF-B44D8F0DC634}";

static clap_controller*
plugin_controller(clap_plugin_t const* plugin)
{ return plugin_cast(plugin)->controller.get(); }
static root_element* 
plugin_ui(clap_plugin_t const* plugin)
{ return plugin_controller(plugin)->plugin_ui.get(); }

static bool CLAP_ABI editor_show(clap_plugin_t const* plugin);
static bool CLAP_ABI editor_hide(clap_plugin_t const* plugin);
static void CLAP_ABI editor_destroy(clap_plugin_t const* plugin);
static bool CLAP_ABI editor_set_parent(clap_plugin_t const* plugin, clap_window_t const* window);
static bool CLAP_ABI editor_create(clap_plugin_t const* plugin, char const* api, bool is_floating);
static bool CLAP_ABI editor_is_api_supported(clap_plugin_t const* plugin, char const* api, bool is_floating);
static bool CLAP_ABI editor_get_size(clap_plugin_t const* plugin, std::uint32_t* width, std::uint32_t* height);
static bool CLAP_ABI editor_get_preferred_api(clap_plugin_t const* plugin, char const** api, bool* is_floating);

static void CLAP_ABI editor_suggest_title(clap_plugin_t const* plugin, char const* title) {}
static bool CLAP_ABI editor_can_resize(clap_plugin_t const* plugin) { return false; }
static bool CLAP_ABI editor_set_scale(clap_plugin_t const* plugin, double scale) { return false; }
static bool CLAP_ABI editor_set_size(clap_plugin_t const* plugin, uint32_t width, uint32_t height) { return false; }
static bool CLAP_ABI editor_set_transient(clap_plugin_t const* plugin, clap_window_t const* window) { return false; }
static bool CLAP_ABI editor_adjust_size(clap_plugin_t const* plugin, uint32_t* width, uint32_t* height) { return false; }
static bool CLAP_ABI editor_get_resize_hints(clap_plugin_t const* plugin, clap_gui_resize_hints_t* hints) { return false; }

void
plugin_init_editor_api(inf_clap_plugin* plugin)
{
  plugin->editor.show = editor_show;
  plugin->editor.hide = editor_hide;
  plugin->editor.create = editor_create;
  plugin->editor.destroy = editor_destroy;
  plugin->editor.set_size = editor_set_size;
  plugin->editor.get_size = editor_get_size;
  plugin->editor.set_scale = editor_set_scale;
  plugin->editor.can_resize = editor_can_resize;
  plugin->editor.set_parent = editor_set_parent;
  plugin->editor.adjust_size = editor_adjust_size;
  plugin->editor.suggest_title = editor_suggest_title;
  plugin->editor.set_transient = editor_set_transient;
  plugin->editor.get_resize_hints = editor_get_resize_hints;
  plugin->editor.is_api_supported = editor_is_api_supported;
  plugin->editor.get_preferred_api = editor_get_preferred_api;
}

static bool CLAP_ABI
editor_is_api_supported(clap_plugin_t const* plugin, char const* api, bool is_floating)
{
  if (is_floating) return false;
  return !strcmp(api, CLAP_WINDOW_API_WIN32);
}

static bool CLAP_ABI
editor_get_preferred_api(clap_plugin_t const* plugin, char const** api, bool* is_floating)
{
  *is_floating = false;
  *api = CLAP_WINDOW_API_WIN32;
  return true;
}

static bool CLAP_ABI
editor_show(clap_plugin_t const* plugin)
{
  plugin_ui(plugin)->component()->setVisible(true);
  return true;
}

static bool CLAP_ABI
editor_hide(clap_plugin_t const* plugin)
{
  plugin_ui(plugin)->component()->setVisible(false);
  return true;
}

static void CLAP_ABI
editor_destroy(clap_plugin_t const* plugin)
{
  plugin_cast(plugin)->controller->timer.stopTimer();
  plugin_ui(plugin)->component()->removeFromDesktop();
  plugin_cast(plugin)->controller->plugin_ui.reset();
}

static bool CLAP_ABI 
editor_create(clap_plugin_t const* plugin, char const* api, bool is_floating)
{
  juce::MessageManager::getInstance();
  auto w = plugin_controller(plugin)->get_editor_wanted_size().first;
  plugin_controller(plugin)->editor_current_width(w);
  plugin_controller(plugin)->plugin_ui = plugin_controller(plugin)->create_ui();
  plugin_ui(plugin)->build();
  plugin_ui(plugin)->layout();
  plugin_ui(plugin)->component()->setVisible(false);
  plugin_ui(plugin)->component()->setOpaque(true);
  return true;
}

static bool CLAP_ABI 
editor_set_parent(clap_plugin_t const* plugin, clap_window_t const* window)
{
  plugin_controller(plugin)->parent_window = window->ptr;
  plugin_ui(plugin)->component()->setTopLeftPosition(0, 0);
  plugin_ui(plugin)->component()->addToDesktop(0, window->ptr);
  plugin_ui(plugin)->component()->setVisible(true);
  plugin_controller(plugin)->timer.startTimer(1000 / 30);
  return true;
}

static bool CLAP_ABI 
editor_get_size(clap_plugin_t const* plugin, std::uint32_t* width, std::uint32_t* height)
{ 
  auto size = plugin_controller(plugin)->get_editor_wanted_size();
  *width = static_cast<std::uint32_t>(size.first);
  *height = static_cast<std::uint32_t>(size.second);
  return true;
}

void 
clap_timer::timerCallback()
{
  audio_to_main_msg msg;
  while (_controller->audio_to_main_queue->try_dequeue(msg))
  {
    std::int32_t id = _controller->topology()->param_index_to_id[msg.index];
    _controller->state()[msg.index] = format_normalized_to_base(_controller->topology(), false, msg.index, msg.value);
    _controller->controller_param_changed(id, _controller->state()[msg.index]);
  }
}

clap_controller::
clap_controller() : 
plugin_controller(create_topology()), 
timer(this) {}

void 
clap_controller::init(
  clap_host_t const* host,
  moodycamel::ReaderWriterQueue<audio_to_main_msg, queue_size>* audio_to_main_queue_,
  moodycamel::ReaderWriterQueue<main_to_audio_msg, queue_size>* main_to_audio_queue_)
{
  _host = host;
  audio_to_main_queue = audio_to_main_queue_;
  main_to_audio_queue = main_to_audio_queue_;
}

void 
clap_controller::reload_editor(std::int32_t width)
{
  if(width == -1) width = editor_current_width();
  plugin_ui->component()->removeFromDesktop();
  editor_current_width(width);
  auto new_bounds = get_editor_wanted_size();
  auto host_gui = static_cast<clap_host_gui_t const*>(_host->get_extension(_host, CLAP_EXT_GUI));
  host_gui->request_resize(_host, new_bounds.first, new_bounds.second);
  plugin_ui = create_ui();
  plugin_ui->build();
  plugin_ui->layout();
  plugin_ui->component()->setVisible(false);
  plugin_ui->component()->setOpaque(true);
  plugin_ui->component()->setTopLeftPosition(0, 0);
  plugin_ui->component()->addToDesktop(0, parent_window);
  plugin_ui->component()->setVisible(true);
}

std::unique_ptr<host_context_menu>
clap_controller::host_menu_for_param_index(std::int32_t param_index) const
{
  return {};
}

std::string
clap_controller::themes_folder(std::string const& plugin_file) const
{
  auto path = std::filesystem::path(plugin_file);
  return (path.parent_path() / "Themes").string();
}

std::string
clap_controller::factory_presets_folder(std::string const& plugin_file) const
{
  auto path = std::filesystem::path(plugin_file);
  return (path.parent_path() / "Presets").string();
}

void
clap_controller::editor_param_changed(std::int32_t index, param_value ui_value)
{
  auto base_value = topology()->ui_to_base_value(index, ui_value);
  do_edit(index, base_to_format_normalized(topology(), false, index, base_value));
}

void
clap_controller::load_component_state(inf::base::param_value* state)
{
  for (std::int32_t p = 0; p < _topology->input_param_count; p++)
    do_edit(p, base_to_format_normalized(_topology.get(), false, p, state[p]));
  restart();
}

void
clap_controller::copy_param(std::int32_t source_tag, std::int32_t target_tag)
{
  std::int32_t source_index = topology()->param_id_to_index.at(source_tag);
  std::int32_t target_index = topology()->param_id_to_index.at(target_tag);
  double source_norm = base_to_format_normalized(topology(), false, source_index, _state[source_index]);
  do_edit(target_index, source_norm);
}

void
clap_controller::swap_param(std::int32_t source_tag, std::int32_t target_tag)
{
  std::int32_t source_index = topology()->param_id_to_index.at(source_tag);
  std::int32_t target_index = topology()->param_id_to_index.at(target_tag);
  double source_norm = base_to_format_normalized(topology(), false, source_index, _state[source_index]);
  double target_norm = base_to_format_normalized(topology(), false, target_index, _state[target_index]);
  do_edit(source_index, target_norm);
  do_edit(target_index, source_norm);
}

// Preset handling:
// 1) Clap file magic.
// 2) Plugin unique id.
// 3) Processor (plugin) state.
// 4) Controller (metadata) state.
// Sort of same as vst3 but without whatever vst's PresetFile class is doing on top of it.
// TODO work out a way to load .vstpresets without touching the vst sdk.

void
clap_controller::save_preset(std::string const& path)
{
  clap_io_stream stream;
  if (!stream.write_string(std::string(clap_file_magic))) return;
  if (!stream.write_string(std::string(inf_plugin_descriptor.id))) return;
  // This better be in sync with audio thread.
  if (!stream.save_processor(*topology(), _state.data())) return; 
  if (!stream.save_controller(*topology(), patch_meta_data())) return;

  // Write preset format to disk.
  stream.reset();
  std::ofstream file(path, std::ios::out | std::ios::binary);
  if (file.bad()) return;
  file.write(reinterpret_cast<char const*>(stream.data()), stream.size());
  file.close();
}

bool
clap_controller::load_preset(std::string const& path)
{
  // Load preset format from disk and parse.
  std::ifstream file(path, std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);
  std::vector<char> buffer = std::vector<char>(size);
  if (!file.read(buffer.data(), size)) return false;

  std::string val;
  std::map<std::string, std::string> meta_data;
  std::vector<base::param_value> audio_state(static_cast<std::size_t>(topology()->input_param_count));
  clap_io_stream stream(reinterpret_cast<std::uint8_t const*>(buffer.data()), buffer.size());
  if(!stream.read_string(val) || val != std::string(clap_file_magic)) return false;
  if(!stream.read_string(val) || val != std::string(inf_plugin_descriptor.id)) return false;
  if(!stream.load_processor(*topology(), audio_state.data())) return false;
  if(!stream.load_controller(*topology(), patch_meta_data())) return false;

  // We have everything, start pushing the values.
  load_component_state(audio_state.data());
  return true;
}

void 
clap_controller::do_edit(std::int32_t index, double normalized)
{
  // TODO gesture stuff
  bool ok;
  (void)ok;

  main_to_audio_msg msg;
  msg.index = index;
  msg.value = normalized;
  msg.type = main_to_audio_msg::begin_edit;
  ok = main_to_audio_queue->try_enqueue(msg);
  assert(ok);

  auto base_value = format_normalized_to_base(_topology.get(), false, index, normalized);
  _state[index] = base_value;
  std::int32_t tag = topology()->param_index_to_id[index];
  controller_param_changed(tag, base_value);

  msg.type = main_to_audio_msg::adjust_value;
  ok = main_to_audio_queue->try_enqueue(msg);
  assert(ok);
  msg.type = main_to_audio_msg::end_edit;
  ok = main_to_audio_queue->try_enqueue(msg);
  assert(ok);

  auto host_params = static_cast<clap_host_params const*>(_host->get_extension(_host, CLAP_EXT_PARAMS));
  host_params->request_flush(_host);
}

} // inf::base::format::clap