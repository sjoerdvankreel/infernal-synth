#ifndef INF_BASE_UI_CONTROLS_PARAM_DROPDOWN_HPP
#define INF_BASE_UI_CONTROLS_PARAM_DROPDOWN_HPP

#include <inf.base.ui/shared/support.hpp>
#include <inf.base.ui/shared/look_and_feel.hpp>
#include <inf.base/topology/param_descriptor.hpp>
#include <juce_gui_basics/juce_gui_basics.h>

#include <map>
#include <cstdint>

namespace inf::base::ui {

inline std::int32_t constexpr dropdown_id_offset = 1000;
  
class inf_param_dropdown:
public juce::ComboBox
{
private:
  lnf_factory const _lnf_factory;
  std::int32_t const _param_index;
  base::plugin_controller* const _controller;
  std::map<std::int32_t, base::list_item const*> _items_by_id;
  base::list_item_enabled_selector const _item_enabled_selector;

  void update_items_enabled(juce::PopupMenu* menu);

public:
  void showPopup() override;
  void mouseUp(juce::MouseEvent const& e) override;
  std::int32_t param_index() const { return _param_index; }
  void set_item_id(base::list_item const* item, std::int32_t id) { _items_by_id[id] = item; }

  inf_param_dropdown(
    base::plugin_controller* controller, std::int32_t param_index, 
    lnf_factory lnf_factory, base::list_item_enabled_selector item_enabled_selector):
  _lnf_factory(lnf_factory), _param_index(param_index), _controller(controller), _items_by_id(), _item_enabled_selector(item_enabled_selector) {}
};

} // namespace inf::base::ui
#endif // INF_BASE_UI_CONTROLS_PARAM_DROPDOWN_HPP