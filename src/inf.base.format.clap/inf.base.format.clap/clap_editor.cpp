#include <inf.base.format.clap/clap_editor.hpp>
#include <cstdint>

namespace inf::base::format::clap
{

static bool CLAP_ABI editor_show(clap_plugin_t const* plugin);
static bool CLAP_ABI editor_hide(clap_plugin_t const* plugin);
static void CLAP_ABI editor_destroy(clap_plugin_t const* plugin);
static bool CLAP_ABI editor_can_resize(clap_plugin_t const* plugin);
static bool CLAP_ABI editor_set_scale(clap_plugin_t const* plugin, double scale);
static void CLAP_ABI editor_suggest_title(clap_plugin_t const* plugin, char const* title);
static bool CLAP_ABI editor_set_parent(clap_plugin_t const* plugin, clap_window_t const* window);
static bool CLAP_ABI editor_set_size(clap_plugin_t const* plugin, uint32_t width, uint32_t height);
static bool CLAP_ABI editor_create(clap_plugin_t const* plugin, char const* api, bool is_floating);
static bool CLAP_ABI editor_set_transient(clap_plugin_t const* plugin, clap_window_t const* window);
static bool CLAP_ABI editor_adjust_size(clap_plugin_t const* plugin, uint32_t* width, uint32_t* height);
static bool CLAP_ABI editor_get_resize_hints(clap_plugin_t const* plugin, clap_gui_resize_hints_t* hints);
static bool CLAP_ABI editor_is_api_supported(clap_plugin_t const* plugin, char const* api, bool is_floating);
static bool CLAP_ABI editor_get_size(clap_plugin_t const* plugin, std::uint32_t* width, std::uint32_t* height);
static bool CLAP_ABI editor_get_preferred_api(clap_plugin_t const* plugin, char const** api, bool* is_floating);

void
plugin_init_editor(inf_clap_plugin* plugin)
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

} // inf::base::format::clap