#include <inf.base.ui/controls/tab_header.hpp>
#include <inf.base.ui/controls/graph_plot.hpp>
#include <inf.base.ui/controls/rotary_knob.hpp>
#include <inf.base.ui/controls/part_connector.hpp> 
#include <inf.base.ui/controls/option_menu_fix.hpp>
#include <inf.base.ui/controls/nested_option_menu.hpp>
#include <inf.base.ui/controls/view_container_fix.hpp>
#include <inf.base.ui/controls/view_switch_container_fix.hpp>
#include <inf.base.ui/shared/bootstrap.hpp>
#include <vstgui/vstgui_uidescription.h>

using namespace VSTGUI;
using namespace inf::base;

namespace inf::base::ui {

static IViewCreator const* _tab_header_creator = nullptr;
static IViewCreator const* _graph_plot_creator = nullptr;
static IViewCreator const* _rotary_knob_creator = nullptr;
static IViewCreator const* _part_connector_creator = nullptr;
static IViewCreator const* _option_menu_fix_creator = nullptr;
static IViewCreator const* _nested_option_menu_creator = nullptr;
static IViewCreator const* _view_container_fix_creator = nullptr;
static IViewCreator const* _view_switch_container_fix_creator = nullptr;

void terminate()
{
  UIViewFactory::unregisterViewCreator(*_graph_plot_creator);
  delete _graph_plot_creator;
  _graph_plot_creator = nullptr;
  UIViewFactory::unregisterViewCreator(*_part_connector_creator);
  delete _part_connector_creator;
  _part_connector_creator = nullptr;
  UIViewFactory::unregisterViewCreator(*_view_switch_container_fix_creator);
  delete _view_switch_container_fix_creator;
  _view_switch_container_fix_creator = nullptr;
  UIViewFactory::unregisterViewCreator(*_nested_option_menu_creator);
  delete _nested_option_menu_creator;
  _nested_option_menu_creator = nullptr;
  UIViewFactory::unregisterViewCreator(*_option_menu_fix_creator);
  delete _option_menu_fix_creator;
  _option_menu_fix_creator = nullptr;
  UIViewFactory::unregisterViewCreator(*_view_container_fix_creator);
  delete _view_container_fix_creator;
  _view_container_fix_creator = nullptr;
  UIViewFactory::unregisterViewCreator(*_rotary_knob_creator);
  delete _rotary_knob_creator;
  _rotary_knob_creator = nullptr;
  UIViewFactory::unregisterViewCreator(*_tab_header_creator);
  delete _tab_header_creator;
  _tab_header_creator = nullptr;
}

void 
initialize(base::topology_info const* topology)
{
  _tab_header_creator = new tab_header_creator();
  UIViewFactory::registerViewCreator(*_tab_header_creator);
  _graph_plot_creator = new graph_plot_creator(topology);
  UIViewFactory::registerViewCreator(*_graph_plot_creator);
  _rotary_knob_creator = new rotary_knob_creator();
  UIViewFactory::registerViewCreator(*_rotary_knob_creator); 
  _part_connector_creator = new part_connector_creator();
  UIViewFactory::registerViewCreator(*_part_connector_creator);
  _view_container_fix_creator = new view_container_fix_creator();
  UIViewFactory::registerViewCreator(*_view_container_fix_creator);
  _nested_option_menu_creator = new nested_option_menu_creator(topology);
  UIViewFactory::registerViewCreator(*_nested_option_menu_creator);
  _option_menu_fix_creator = new option_menu_fix_creator();
  UIViewFactory::registerViewCreator(*_option_menu_fix_creator);
  _view_switch_container_fix_creator = new view_switch_container_fix_creator(topology);
  UIViewFactory::registerViewCreator(*_view_switch_container_fix_creator);
}

} // extern "C"