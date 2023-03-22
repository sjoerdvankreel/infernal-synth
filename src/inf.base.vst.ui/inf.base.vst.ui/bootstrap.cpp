#include <pluginterfaces/base/funknown.h>
#include <inf.base.ui/shared/bootstrap.hpp>
#include <inf.base/topology/topology_info.hpp>

#include <cstdint>

using namespace inf::base;

extern bool InitModule();
extern bool DeinitModule();
static std::int32_t _inf_module_counter = 0;
 
extern "C" {

SMTG_EXPORT_SYMBOL
bool InitDll()
{
  if (++_inf_module_counter != 1) return true;
  if (!InitModule()) return false;
  inf::base::ui::initialize();
  return true;
}

SMTG_EXPORT_SYMBOL
bool ExitDll()
{
  --_inf_module_counter;
  if (_inf_module_counter > 0) return true;
  if (_inf_module_counter < 0) return false;
  if (!DeinitModule()) return false;
  inf::base::ui::terminate();
  return true;
}

} // extern "C"