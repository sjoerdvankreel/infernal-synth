#ifndef INF_BASE_FORMAT_CLAP_CLAP_CONTROLLER_HPP
#define INF_BASE_FORMAT_CLAP_CLAP_CONTROLLER_HPP

#include <inf.base.ui/shared/ui.hpp>
#include <inf.base.format.clap/clap_support.hpp>
#include <inf.base/plugin/plugin_controller.hpp>

#include <clap/clap.h>
#include <readerwriterqueue.h>
#include <juce_events/juce_events.h>

// https://nakst.gitlab.io/tutorial/clap-part-1.html
// https://github.com/surge-synthesizer/clap-saw-demo
// https://github.com/free-audio/clap-juce-extensions/

namespace inf::base::format::clap 
{

class clap_controller;

class clap_timer:
public juce::Timer
{
  clap_controller* const _controller;
public:
  void timerCallback() override;
  clap_timer(clap_controller* controller): _controller(controller) {}
};

struct inf_clap_plugin;

class clap_controller:
public inf::base::plugin_controller
{
  clap_host_t const* _host;

  void do_edit(std::int32_t index, double normalized);

protected:
  clap_controller();

public:
  // Allow access by clap_plugin_gui_t.
  clap_timer timer;
  void* parent_window = {};
  std::unique_ptr<inf::base::ui::root_element> plugin_ui = {};
  moodycamel::ReaderWriterQueue<audio_to_main_msg, queue_size>* audio_to_main_queue = {};
  moodycamel::ReaderWriterQueue<main_to_audio_msg, queue_size>* main_to_audio_queue = {};

  // Defer to plugin.
  virtual std::unique_ptr<inf::base::ui::root_element> create_ui() = 0;

  void restart() override {}
  void load_component_state(inf::base::param_value* state) override;
  void swap_param(std::int32_t source_tag, std::int32_t target_tag) override;
  void copy_param(std::int32_t source_tag, std::int32_t target_tag) override;
  std::string themes_folder(std::string const& plugin_file) const override;
  std::string factory_presets_folder(std::string const& plugin_file) const override;

  void reload_editor(std::int32_t width) override;
  void editor_param_changed(std::int32_t index, param_value ui_value) override;
  std::unique_ptr<host_context_menu> host_menu_for_param_index(std::int32_t param_index) const override;
  void* current_editor_window() const override { return plugin_ui.get() ? plugin_ui->component() : nullptr; }

  void init(
    clap_host_t const* host, 
    moodycamel::ReaderWriterQueue<audio_to_main_msg, queue_size>* audio_to_main_queue_,
    moodycamel::ReaderWriterQueue<main_to_audio_msg, queue_size>* main_to_audio_queue_);
};

void
plugin_init_editor_api(inf_clap_plugin* plugin);

} // inf::base::format::clap
#endif // INF_BASE_FORMAT_CLAP_CLAP_CONTROLLER_HPP