#include <inf.base.format.clap/clap_entry.hpp>
#include <inf.base.format.clap/clap_plugin.hpp>
#include <inf.base.format.clap/clap_controller.hpp>

#include <clap/clap.h>

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
  plugin_ui(plugin)->component()->removeFromDesktop();
  plugin_cast(plugin)->controller->plugin_ui.reset();
}

static bool CLAP_ABI 
editor_create(clap_plugin_t const* plugin, char const* api, bool is_floating)
{
  plugin_controller(plugin)->plugin_ui = plugin_controller(plugin)->create_ui();
  plugin_ui(plugin)->build();
  plugin_ui(plugin)->layout();
  plugin_ui(plugin)->component()->setOpaque(true);
  return true;
}

static bool CLAP_ABI 
editor_set_parent(clap_plugin_t const* plugin, clap_window_t const* window)
{
  plugin_ui(plugin)->component()->addToDesktop(0, window->ptr);
  plugin_ui(plugin)->component()->setVisible(true);
  return true;
}


static bool CLAP_ABI 
editor_get_size(clap_plugin_t const* plugin, std::uint32_t* width, std::uint32_t* height)
{ 
  *width = plugin_ui(plugin)->component()->getWidth();
  *height = plugin_ui(plugin)->component()->getWidth();
  return true;
}

clap_controller::
clap_controller():
plugin_controller(create_topology()) {}

void 
clap_controller::reload_editor(std::int32_t width)
{
}

void 
clap_controller::editor_param_changed(std::int32_t index, param_value ui_value)
{
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
clap_controller::preset_file_extension()
{
  return "isp";
}

std::string 
clap_controller::default_theme_path(std::string const& plugin_file) const
{
  return {};
}

std::vector<inf::base::external_resource> 
clap_controller::themes(std::string const& plugin_file) const
{
  return {};
}

std::unique_ptr<host_context_menu> 
clap_controller::host_menu_for_param_index(std::int32_t param_index) const
{
  return {};
}

std::vector<inf::base::external_resource> 
clap_controller::factory_presets(std::string const& plugin_file) const
{  
  return {};
}

} // inf::base::format::clap