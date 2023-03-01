#include <inf.vst.tool/shared/load_topology.hpp>

#include <memory>
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
typedef bool (*inf_init_dll_t)(void);
typedef topology_info* (*inf_create_topology_t)(void);

namespace inf::vst::tool::shared {

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

std::unique_ptr<inf::base::topology_info>
load_topology(char const* library_path)
{
  void* library;
  void* init_dll;
  void* create_topology;
  topology_info* topology;
  std::unique_ptr<topology_info> result;
  assert(library_path != nullptr);

  if ((library = load_library(library_path)) == nullptr)
    return std::cout << "Library " << library_path << " not found.\n", std::move(result);
  std::cout << "Loaded library " << library_path << ".\n";
  if ((init_dll = get_proc_address(library, "InitDll")) == nullptr)
    return std::cout << library_path << " does not export InitDll.\n", std::move(result);
  if ((create_topology = get_proc_address(library, "inf_vst_create_topology")) == nullptr)
    return std::cout << library_path << " does not export inf_vst_create_topology.\n", std::move(result);
  if (!reinterpret_cast<inf_init_dll_t>(init_dll)())
    return std::cout << "InitDll returned false.\n", std::move(result);
  topology = reinterpret_cast<inf_create_topology_t>(create_topology)();
  if (topology == nullptr)
    return std::cout << "inf_vst_create_topology returned NULL.\n", std::move(result);
  return std::unique_ptr<topology_info>(topology);
}

} // namespace inf::vst::tool::shared