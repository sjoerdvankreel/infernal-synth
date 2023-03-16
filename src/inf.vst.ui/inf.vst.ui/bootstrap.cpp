#include <inf.vst.ui/bootstrap.hpp>
#include <inf.base.ui/shared/bootstrap.hpp>
#include <cstdint>

#if WIN32
#include <Windows.h>
void* moduleHandle = nullptr;
#endif  

using namespace inf::base;

extern bool InitModule();
extern bool DeinitModule();

static std::int32_t _inf_module_counter = 0;
static topology_info const* _topology = nullptr;
 
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
  inf::base:ui::terminate(_topology);
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
  inf::base::ui::initialize(_topology);
  return true;
}

} // extern "C"