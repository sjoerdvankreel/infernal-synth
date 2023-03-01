#include <inf.vst.tool/shared/load_topology.hpp>
#include <cassert>
#include <iostream>

#if WIN32
#define NOMINMAX 1
#include <Windows.h>
#undef GetObject
#else
#include <dlfcn.h>
#endif

using namespace inf::base;

namespace inf::vst::tool::shared {

typedef topology_info* (*inf_create_topology_t)(void);

static void*
load_library(char const* file)
{
#if WIN32
  return LoadLibraryA(file);
#else
  return dlopen(file, RTLD_NOW);
#endif
}

static void*
get_proc_address(void* module, char const* name)
{
#if WIN32
  return GetProcAddress(static_cast<HMODULE>(module), name);
#else
  return dlsym(module, name);
#endif
}

loaded_topology::
~loaded_topology()
{
  _topology.reset();
  _unloader();
}

loaded_topology::
loaded_topology(std::unique_ptr<inf::base::topology_info>&& topology, inf_init_exit_dll_t unloader):
_topology(std::move(topology)), _unloader(unloader)
{
  assert(unloader != nullptr);
  assert(_topology.get() != nullptr);
}

std::unique_ptr<loaded_topology>
load_topology(char const* library_path)
{
  void* library;
  void* init_dll;
  void* exit_dll;
  void* create_topology;
  topology_info* topology;
  std::unique_ptr<loaded_topology> result;
  assert(library_path != nullptr);

  if ((library = load_library(library_path)) == nullptr)
    return std::cout << "Library " << library_path << " not found.\n", std::move(result);
  std::cout << "Loaded library " << library_path << ".\n";
  if ((init_dll = get_proc_address(library, "InitDll")) == nullptr)
    return std::cout << library_path << " does not export InitDll.\n", std::move(result);
  if ((exit_dll = get_proc_address(library, "ExitDll")) == nullptr)
    return std::cout << library_path << " does not export ExitDll.\n", std::move(result);
  if ((create_topology = get_proc_address(library, "inf_vst_create_topology")) == nullptr)
    return std::cout << library_path << " does not export inf_vst_create_topology.\n", std::move(result);
  if (!reinterpret_cast<inf_init_exit_dll_t>(init_dll)())
    return std::cout << "InitDll returned false.\n", std::move(result);
  topology = reinterpret_cast<inf_create_topology_t>(create_topology)();
  if (topology == nullptr)
    return std::cout << "inf_vst_create_topology returned NULL.\n", std::move(result);
  return std::make_unique<loaded_topology>(
    std::unique_ptr<topology_info>(topology), 
    reinterpret_cast<inf_init_exit_dll_t>(exit_dll));
}

} // namespace inf::vst::tool::shared