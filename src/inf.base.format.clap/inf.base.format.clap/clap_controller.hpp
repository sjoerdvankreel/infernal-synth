#ifndef INF_BASE_FORMAT_CLAP_CLAP_CONTROLLER_HPP
#define INF_BASE_FORMAT_CLAP_CLAP_CONTROLLER_HPP

#include <inf.base/plugin/plugin_controller.hpp>
#include <clap/clap.h>

namespace inf::base::format::clap 
{

class clap_controller:
public inf::base::plugin_controller
{
public:
  void* current_editor_window() const override;
  void reload_editor(std::int32_t width) override;
  void editor_param_changed(std::int32_t index, param_value ui_value) override;

  void restart() override;
  void save_preset(std::string const& path) override;
  bool load_preset(std::string const& path) override;
  void load_component_state(inf::base::param_value* state) override;
  void swap_param(std::int32_t source_tag, std::int32_t target_tag) override;
  void copy_param(std::int32_t source_tag, std::int32_t target_tag) override;

  std::string preset_file_extension();
  std::string default_theme_path(std::string const& plugin_file) const override;
  std::vector<inf::base::external_resource> themes(std::string const& plugin_file) const override;
  std::unique_ptr<host_context_menu> host_menu_for_param_index(std::int32_t param_index) const override;
  std::vector<inf::base::external_resource> factory_presets(std::string const& plugin_file) const override;
};

} // inf::base::format::clap
#endif // INF_BASE_FORMAT_CLAP_CLAP_CONTROLLER_HPP