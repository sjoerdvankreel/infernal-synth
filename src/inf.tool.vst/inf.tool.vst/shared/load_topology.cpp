#include <inf.tool.vst/shared/load_topology.hpp>
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

namespace inf::tool::vst::shared {

typedef topology_info* (*inf_create_topology_t)(void);
typedef topology_info* (*inf_create_topology2_t)(std::int32_t);

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
loaded_topology(std::unique_ptr<inf::base::topology_info>&& topology, inf_exit_library_t unloader):
_unloader(unloader), _topology(std::move(topology))
{
  assert(unloader != nullptr);
  assert(_topology.get() != nullptr);
}

std::unique_ptr<loaded_topology>
load_topology(char const* library_path)
{
  void* library;
  void* init_library;
  void* exit_library;
  void* create_topology;
  topology_info* topology;
  std::unique_ptr<loaded_topology> result;
  assert(library_path != nullptr);

  if ((library = load_library(library_path)) == nullptr)
    return std::cout << "Library " << library_path << " not found.\n", std::move(result);
  std::cout << "Loaded library " << library_path << ".\n";
#if WIN32
  if ((init_library = get_proc_address(library, "InitDll")) == nullptr)
    return std::cout << library_path << " does not export InitDll.\n", std::move(result);
  if (!reinterpret_cast<inf_init_library_t>(init_library)())
    return std::cout << "InitDll returned false.\n", std::move(result);
  if ((exit_library = get_proc_address(library, "ExitDll")) == nullptr)
    return std::cout << library_path << " does not export ExitDll.\n", std::move(result);
#else
  if ((init_library = get_proc_address(library, "ModuleEntry")) == nullptr)
    return std::cout << library_path << " does not export ModuleEntry.\n", std::move(result);
  if (!reinterpret_cast<inf_init_library_t>(init_library)(nullptr))
    return std::cout << "ModuleEntry returned false.\n", std::move(result);
  if ((exit_library = get_proc_address(library, "ModuleExit")) == nullptr)
    return std::cout << library_path << " does not export ModuleExit.\n", std::move(result);
#endif
  if ((create_topology = get_proc_address(library, "inf_vst_create_topology2")) != nullptr)
    topology = reinterpret_cast<inf_create_topology2_t>(create_topology)(1);
  else if ((create_topology = get_proc_address(library, "inf_vst_create_topology")) != nullptr)
    topology = reinterpret_cast<inf_create_topology_t>(create_topology)();
  else if ((create_topology = get_proc_address(library, "svn_vst_create_topology")) != nullptr)
    topology = reinterpret_cast<inf_create_topology_t>(create_topology)();
  else
    return std::cout << library_path << " does not export (svn/inf)_vst_create_topology(2).\n", std::move(result);
  if (topology == nullptr)
    return std::cout << "(svn/inf)_vst_create_topology(2) returned NULL.\n", std::move(result);
  return std::make_unique<loaded_topology>(
    std::unique_ptr<topology_info>(topology), 
    reinterpret_cast<inf_exit_library_t>(exit_library));
}

} // namespace inf::tool::vst::shared