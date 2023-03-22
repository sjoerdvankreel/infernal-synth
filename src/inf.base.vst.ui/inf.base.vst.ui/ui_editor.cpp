#include <inf.base.vst.ui/ui_editor.hpp>
#include <inf.base.vst/sdk/parameter.hpp>
#include <inf.base.vst/shared/support.hpp>
#include <inf.base/topology/param_ui_descriptor.hpp>

#include <cassert>
#include <algorithm>

using namespace VSTGUI;
using namespace inf::base;
using namespace inf::base::ui;

namespace inf::base::vst::ui {
    
vst_ui_editor::
vst_ui_editor(EditController* controller, UTF8StringPtr template_name,
  UTF8StringPtr xml_file, topology_info const* topology):
VST3Editor(controller, template_name, xml_file),
_graphs(), _controls(topology->params.size()), _topology(topology)
{ 
  assert(topology != nullptr);
  assert(xml_file != nullptr);
  assert(controller != nullptr);
  assert(template_name != nullptr);
}

// Set initial visibility when the plugin ui is opened.
bool PLUGIN_API
vst_ui_editor::open(void* parent, const PlatformType& type)
{
  if (!VST3Editor::open(parent, type)) return false;
  dynamic_cast<vst_ui_controller&>(*getController()).sync_ui_parameters();
  return true;
}

void 
vst_ui_editor::onViewAdded(CFrame* view_frame, CView* view)
{
  VST3Editor::onViewAdded(view_frame, view);

  // Keep track of graphs.
  graph_plot* graph = dynamic_cast<graph_plot*>(view);
  if (graph != nullptr)
    _graphs.push_back(graph);
    
  // Keep track of controls by tag.
  auto& view_controller = dynamic_cast<vst_controller&>(*getController());
  CControl* control = dynamic_cast<CControl*>(view);
  if (control != nullptr && control->getTag() >= 0)
  {
    // Control loses its value if removed and re-added.
    std::int32_t param_index = _topology->param_id_to_index.at(control->getTag());
    assert(param_index >= 0 && param_index < static_cast<std::int32_t>(_topology->params.size()));
    _controls[param_index].push_back(control);
    param_value val = view_controller.state()[param_index];
    control->setValue(base_to_vst_normalized(_topology, param_index, val));
  }
}
 
void 
vst_ui_editor::onViewRemoved(CFrame* view_frame, CView* view)
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

  VST3Editor::onViewRemoved(view_frame, view);
} 
    
void               
vst_ui_editor::update_dependent_visibility(ParamID tag)
{
  // Update graphs where needed.  
  std::int32_t param_index = _topology->param_id_to_index.at(tag);
  assert(param_index >= 0 && param_index < static_cast<std::int32_t>(_topology->params.size()));
  for (std::size_t i = 0; i < _graphs.size(); i++)
    if (_graphs[i]->processor()->needs_repaint(param_index))
      _graphs[i]->setDirty(true);
       
  auto const& dependents = _topology->ui_param_dependencies[param_index];
  for(std::size_t d = 0; d < dependents.size(); d++)
  {                  
    CView* visible_view = nullptr;
    CViewContainer* parent_view = nullptr;

    for(std::size_t c = 0; c < _controls[dependents[d]].size(); c++)
    {
      // Visible if all dependencies are met.
      bool visible = true;
      auto const& param = _topology->params[dependents[d]];
      if(param.descriptor->data.ui != nullptr)
        for (std::size_t i = 0; i < param.descriptor->data.ui->relevance.size(); i++)
        {
          auto& edit_controller = dynamic_cast<vst_controller&>(*getController());
          std::int32_t part_index = _topology->parts[param.part_index].type_index;
          std::int32_t part_type = _topology->parts[param.part_index].descriptor->type;
          std::int32_t offset = _topology->param_bounds[part_type][part_index];
          std::int32_t that_param = param.descriptor->data.ui->relevance[i].if_param;
          std::int32_t that_tag = _topology->param_index_to_id[that_param + offset];
          double normalized = edit_controller.getParamNormalized(that_tag);
          std::int32_t value = vst_normalized_to_base(_topology, that_param + offset, normalized).discrete;
          visible &= _topology->params[dependents[d]].descriptor->data.ui->relevance[i].predicate(value);
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