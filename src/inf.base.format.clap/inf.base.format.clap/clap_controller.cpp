#include <inf.base/shared/support.hpp>
#include <inf.base.format.clap/clap_entry.hpp>
#include <inf.base.format.clap/clap_plugin.hpp>
#include <inf.base.format.clap/clap_controller.hpp>

#include <clap/clap.h>
#include <filesystem>

using namespace inf::base;
using namespace inf::base::ui;

namespace inf::base::format::clap
{

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
  moodycamel::ReaderWriterQueue<audio_to_main_msg, queue_size>* audio_to_main_queue,
  moodycamel::ReaderWriterQueue<main_to_audio_msg, queue_size>* main_to_audio_queue)
{
  _host = host;
  this->audio_to_main_queue = audio_to_main_queue;
  this->main_to_audio_queue = main_to_audio_queue;
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

void 
clap_controller::restart()
{
}

void 
clap_controller::save_preset(std::string const& path)
{
}

bool 
clap_controller::load_preset(std::string const& path)
{
  return false;
}

void
clap_controller::load_component_state(inf::base::param_value* state)
{
}

void
clap_controller::swap_param(std::int32_t source_tag, std::int32_t target_tag)
{
}

void
clap_controller::copy_param(std::int32_t source_tag, std::int32_t target_tag)
{
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

std::unique_ptr<host_context_menu> 
clap_controller::host_menu_for_param_index(std::int32_t param_index) const
{
  return {};
}

void
clap_controller::editor_param_changed(std::int32_t index, param_value ui_value)
{
  // TODO gesture stuff
  main_to_audio_msg msg;
  msg.index = index;
  msg.value = base_to_format_normalized(topology(), false, index, topology()->ui_to_base_value(index, ui_value));
  msg.type = main_to_audio_msg::begin_edit;
  main_to_audio_queue->try_enqueue(msg);
  msg.type = main_to_audio_msg::adjust_value;
  main_to_audio_queue->try_enqueue(msg);
  msg.type = main_to_audio_msg::end_edit;
  main_to_audio_queue->try_enqueue(msg);
  
  auto host_params = static_cast<clap_host_params const*>(_host->get_extension(_host, CLAP_EXT_PARAMS));
  host_params->request_flush(_host);
}

} // inf::base::format::clap