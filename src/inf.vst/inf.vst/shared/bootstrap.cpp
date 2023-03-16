#include <inf.vst.base/ui/tab_header.hpp>
#include <inf.vst.base/ui/graph_plot.hpp>
#include <inf.vst.base/ui/rotary_knob.hpp>
#include <inf.vst.base/ui/part_connector.hpp> 
#include <inf.vst.base/ui/option_menu_fix.hpp>
#include <inf.vst.base/ui/nested_option_menu.hpp>
#include <inf.vst.base/ui/view_container_fix.hpp>
#include <inf.vst.base/ui/view_switch_container_fix.hpp>
#include <inf.vst.base/shared/bootstrap.hpp>
#include <vstgui/vstgui_uidescription.h>

#if WIN32
#include <Windows.h>
void* moduleHandle = nullptr;
#endif  

using namespace VSTGUI;
using namespace inf::base;
using namespace inf::vst::base;

extern bool InitModule();
extern bool DeinitModule();

static std::int32_t _inf_module_counter = 0;
static topology_info const* _topology = nullptr;
static IViewCreator const* _tab_header_creator = nullptr;
static IViewCreator const* _graph_plot_creator = nullptr;
static IViewCreator const* _rotary_knob_creator = nullptr;
static IViewCreator const* _part_connector_creator = nullptr;
static IViewCreator const* _option_menu_fix_creator = nullptr;
static IViewCreator const* _nested_option_menu_creator = nullptr;
static IViewCreator const* _view_container_fix_creator = nullptr;
static IViewCreator const* _view_switch_container_fix_creator = nullptr;
 
extern "C" { 

SMTG_EXPORT_SYMBOL topology_info* 
inf_vst_create_topology()
{ return inf_vst_create_topology_impl(); }

#if WIN32
BOOL WINAPI
DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
  if (reason != DLL_PROCESS_ATTACH) return TRUE;
  moduleHandle = instance;
  return TRUE;
}
#endif

SMTG_EXPORT_SYMBOL
bool ExitDll()
{
  --_inf_module_counter;
  if (_inf_module_counter > 0) return true;
  if (_inf_module_counter < 0) return false;
  if (!DeinitModule()) return false;
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
  delete _topology;
  _topology = nullptr;
  return true;
}

SMTG_EXPORT_SYMBOL
bool InitDll()
{
  if (++_inf_module_counter != 1) return true;
  if (!InitModule()) return false;
  _topology = inf_vst_create_topology();
  _tab_header_creator = new tab_header_creator();
  UIViewFactory::registerViewCreator(*_tab_header_creator);
  _graph_plot_creator = new graph_plot_creator(_topology);
  UIViewFactory::registerViewCreator(*_graph_plot_creator);
  _rotary_knob_creator = new rotary_knob_creator();
  UIViewFactory::registerViewCreator(*_rotary_knob_creator); 
  _part_connector_creator = new part_connector_creator();
  UIViewFactory::registerViewCreator(*_part_connector_creator);
  _view_container_fix_creator = new view_container_fix_creator();
  UIViewFactory::registerViewCreator(*_view_container_fix_creator);
  _nested_option_menu_creator = new nested_option_menu_creator(_topology);
  UIViewFactory::registerViewCreator(*_nested_option_menu_creator);
  _option_menu_fix_creator = new option_menu_fix_creator();
  UIViewFactory::registerViewCreator(*_option_menu_fix_creator);
  _view_switch_container_fix_creator = new view_switch_container_fix_creator();
  UIViewFactory::registerViewCreator(*_view_switch_container_fix_creator);
  return true;
}

} // extern "C"