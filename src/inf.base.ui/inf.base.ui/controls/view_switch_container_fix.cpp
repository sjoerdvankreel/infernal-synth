#include <inf.base.ui/controls/view_switch_container_fix.hpp>
#include <vstgui/uidescription/uiviewcreator.h>

using namespace inf::base;
using namespace VSTGUI;
using namespace VSTGUI::UIViewCreator;

namespace inf::base::ui {

CView* 
view_switch_container_fix_creator::create(
  UIAttributes const& attrs, IUIDescription const* desc) const
{ 
  auto result = new view_switch_container_fix();
  new view_switch_controller_fix(_topology, result, desc, desc->getController());
  return result;
}

bool 
view_switch_container_fix_creator::apply(
  CView* view, UIAttributes const& attrs, IUIDescription const* desc) const
{
  CPoint offset;
  if(!UIViewSwitchContainerCreator::apply(view, attrs, desc)) return false;
  return true;
}

void
view_switch_controller_fix::valueChanged(VSTGUI::CControl* control)
{
  std::int32_t old_index = currentIndex;
  UIDescriptionViewSwitchController::valueChanged(control);
  if(old_index == -1) return;
  float normalized = control->getValueNormalized();
  std::int32_t new_index = std::min(static_cast<std::int32_t>(normalized * static_cast<float>(templateNames.size())), 
    static_cast<std::int32_t>(templateNames.size() - 1));
  if(old_index == new_index) return;

  // Repaint all dependent parameters for all discrete parameters of both the switched-in and switched-out view part.
  // Just fake all discrete parameters for the part have updated, real valued params can't be used for conditional visibility.
  std::int32_t param_tag = control->getTag();
  std::int32_t param_index = _topology->param_id_to_index.at(param_tag);
  auto selector_part_ids = _topology->inverse_bounds[param_index];
  std::int32_t selector_param_start = _topology->param_bounds[selector_part_ids.type][selector_part_ids.index];
  std::int32_t activation_index = param_index - selector_param_start;
  std::int32_t switched_part = _topology->selector_to_part[activation_index];

  std::int32_t param_count = _topology->static_parts[switched_part].param_count;
  std::int32_t start_param_old = _topology->param_bounds[switched_part][old_index];
  std::int32_t start_param_new = _topology->param_bounds[switched_part][new_index];
  for(std::int32_t i = 0; i < param_count; i++)
    if(_topology->params[start_param_old + i].descriptor->data.type != param_type::real)
    {
      editor->update_dependent_visibility(_topology->param_index_to_id[start_param_old + i]);
      editor->update_dependent_visibility(_topology->param_index_to_id[start_param_new + i]);
    }
}

} // namespace inf::base::ui