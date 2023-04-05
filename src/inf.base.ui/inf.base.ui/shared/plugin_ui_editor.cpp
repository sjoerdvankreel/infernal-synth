#include <inf.base.ui/shared/plugin_ui_editor.hpp>
#include <inf.base/topology/param_ui_descriptor.hpp>

#include <cassert>
#include <algorithm>

using namespace VSTGUI;
using namespace inf::base;
using namespace inf::base::ui;

namespace inf::base::ui {

plugin_ui_editor::
plugin_ui_editor(plugin_controller* controller, topology_info const* topology):
_controller(controller), _graphs(), _controls(topology->params.size()) {}

void 
plugin_ui_editor::view_added(CFrame* view_frame, CView* view)
{
  // Keep track of graphs.
  auto const* topology = controller()->topology();
  graph_plot* graph = dynamic_cast<graph_plot*>(view);
  if (graph != nullptr)
    _graphs.push_back(graph);
    
  // Keep track of controls by tag.
  CControl* control = dynamic_cast<CControl*>(view);
  if (control != nullptr && control->getTag() >= 0)
  {
    // Control loses its value if removed and re-added.
    auto tag_iter = topology->param_id_to_index.find(control->getTag());
    if (tag_iter != topology->param_id_to_index.end())
    {
      std::int32_t param_index = tag_iter->second;
      assert(param_index >= 0 && param_index < static_cast<std::int32_t>(topology->params.size()));
      _controls[param_index].push_back(control);
      param_value val = controller()->state()[param_index];
      control->setValue(static_cast<float>(_controller->base_to_plugin_param(param_index, val)));
    }
  }
}
 
void 
plugin_ui_editor::view_removed(CFrame* view_frame, CView* view)
{
  // Keep track of graphs.
  graph_plot* graph = dynamic_cast<graph_plot*>(view);
  if (graph != nullptr)
  {
    auto it = std::find(_graphs.begin(), _graphs.end(), graph);
    assert(it != _graphs.end());
    _graphs.erase(it);
  }

  // Keep track of controls by tag.
  CControl* control = dynamic_cast<CControl*>(view);
  if (control != nullptr && control->getTag() >= 0)
    for(std::size_t param = 0; param < _controls.size(); param++)
      for (std::size_t c = 0; c < _controls[param].size(); c++)
        if(_controls[param][c] == view)
          _controls[param].erase(_controls[param].begin() + c);
} 
    
void               
plugin_ui_editor::update_dependent_visibility(std::int32_t tag)
{
  // Update graphs where needed.  
  auto const* topology = controller()->topology();
  std::int32_t param_index = topology->param_id_to_index.at(tag);
  assert(param_index >= 0 && param_index < static_cast<std::int32_t>(topology->params.size()));
  for (std::size_t i = 0; i < _graphs.size(); i++)
    if (_graphs[i]->processor()->needs_repaint(param_index))
      _graphs[i]->setDirty(true);
       
  auto const& dependents = topology->ui_param_dependencies[param_index];
  for(std::size_t d = 0; d < dependents.size(); d++)
  {                  
    CView* visible_view = nullptr;
    CViewContainer* parent_view = nullptr;

    for(std::size_t c = 0; c < _controls[dependents[d]].size(); c++)
    {
      // Visible if all dependencies are met.
      bool visible = true;
      auto const& param = topology->params[dependents[d]];
      if(param.descriptor->data.ui != nullptr)
        for (std::size_t i = 0; i < param.descriptor->data.ui->relevance.size(); i++)
        {
          std::int32_t part_index = topology->parts[param.part_index].type_index;
          std::int32_t part_type = topology->parts[param.part_index].descriptor->type;
          std::int32_t offset = topology->param_bounds[part_type][part_index];
          std::int32_t that_param = param.descriptor->data.ui->relevance[i].if_param;
          std::int32_t that_tag = topology->param_index_to_id[that_param + offset];
          double normalized = _controller->get_plugin_param(that_tag);
          std::int32_t value = _controller->plugin_to_base_param(that_param + offset, normalized).discrete;
          visible &= topology->params[dependents[d]].descriptor->data.ui->relevance[i].predicate(value);
        }  

      // Show/hide dependent controls.
      CView* container = _controls[dependents[d]][c]->getParentView();
      container->setVisible(visible);
      if(visible) 
      {
        assert(visible_view == nullptr || visible_view == container);
        visible_view = container;
      }
      assert(parent_view == nullptr || parent_view == container->getParentView());
      parent_view = container->getParentView()->asViewContainer();
    }
             
    // Move to front for hittesting.
    if(parent_view != nullptr && visible_view != nullptr)
      parent_view->changeViewZOrder(visible_view, parent_view->getNbViews() - 1);
  }
}

} // namespace inf::base::vst::ui