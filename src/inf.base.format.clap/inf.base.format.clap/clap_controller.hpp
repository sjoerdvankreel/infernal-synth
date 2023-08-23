#ifndef INF_BASE_FORMAT_CLAP_CLAP_CONTROLLER_HPP
#define INF_BASE_FORMAT_CLAP_CLAP_CONTROLLER_HPP

#include <inf.base.ui/shared/ui.hpp>
#include <inf.base/plugin/plugin_controller.hpp>

namespace inf::base::format::clap 
{

struct inf_clap_plugin;

class clap_controller:
public inf::base::plugin_controller
{
protected:
  clap_controller();

public:
  // Allow access by clap_plugin_gui_t.
  void* _parent_window = {};
  std::unique_ptr<inf::base::ui::root_element> plugin_ui = {};
  virtual std::unique_ptr<inf::base::ui::root_element> create_ui() = 0;

  void restart() override;
  void save_preset(std::string const& path) override;
  bool load_preset(std::string const& path) override;
  void load_component_state(inf::base::param_value* state) override;
  void swap_param(std::int32_t source_tag, std::int32_t target_tag) override;
  void copy_param(std::int32_t source_tag, std::int32_t target_tag) override;

  std::string default_theme_name() const override { return "3D Default"; }
  std::string preset_file_extension() const override { return "isp"; }
  std::string themes_folder(std::string const& plugin_file) const override;
  std::string factory_presets_folder(std::string const& plugin_file) const override;

  void reload_editor(std::int32_t width) override;
  void editor_param_changed(std::int32_t index, param_value ui_value) override;
  std::unique_ptr<host_context_menu> host_menu_for_param_index(std::int32_t param_index) const override;

  void* current_editor_window() const override { return plugin_ui.get() ? plugin_ui->component() : nullptr; }
};

void
plugin_init_editor_api(inf_clap_plugin* plugin);

} // inf::base::format::clap
#endif // INF_BASE_FORMAT_CLAP_CLAP_CONTROLLER_HPP