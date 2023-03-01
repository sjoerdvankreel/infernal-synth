#include <inf.vst.tool/ui/generator/generator.hpp>
#include <inf.vst.tool/ui/description/description.hpp>
#include <vstgui/uidescription/rapidjson/include/rapidjson/prettywriter.h>
#include <vstgui/uidescription/rapidjson/include/rapidjson/ostreamwrapper.h>

#if WIN32
#define NOMINMAX 1
#include <Windows.h>
#undef GetObject
#else
#include <dlfcn.h>
#endif

#include <fstream>
#include <iostream>

using namespace rapidjson;
using namespace inf::base;
using namespace inf::vst::tool::ui;

typedef bool (*inf_init_exit_dll_t)(void);
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

static std::int32_t
failed(char const* file, void* exit_dll)
{
  std::cout << "Failed to write " << file << ".\n";
  reinterpret_cast<inf_init_exit_dll_t>(exit_dll)();
  return 1;
}

static void
print_uidesc(topology_info const& topology, controller_ui_description const& desc)
{
  for (std::int32_t i = 0; i < desc.part_types.size(); i++)
  {
    auto const& type = desc.part_types[i];
    auto const& part = *type.parts[0].info->descriptor;
    std::cout << part.static_name.short_ << ": ";
    std::cout << part.part_count << " parts";
    std::cout << ", " << part.param_count << " params";
    std::cout << ", " << part.ui->graphs.size() << " graphs";
    if(type.selector_param.info != nullptr) std::cout << ", selector";
    if(type.parts[0].enabled_param.info != nullptr) std::cout << ", enabled";
    std::cout << "\n";
  }
  std::cout << "Topology: ";
  std::cout << topology.static_part_count << " static parts (including selector)";
  std::cout << ", " << topology.parts.size() << " runtime parts";
  std::cout << ", " << topology.params.size() << " runtime params";
  std::cout << "\n";
}

int
main(int argc, char** argv)
{
  void* library;
  void* init_dll;
  void* exit_dll;
  void* create_topology;
  std::unique_ptr<topology_info> topology;

  if (argc != 3)
    return std::cout << "Usage: infernal.vst.ui <full\\path\\to\\plugin.vst3> (dll) <full\\path\\to\\file.uidesc> (json)\n", 1;
  if ((library = load_library(argv[1])) == nullptr)
    return std::cout << "Library " << argv[1] << " not found.\n", 1;
  if ((init_dll = get_proc_address(library, "InitDll")) == nullptr)
    return std::cout << argv[1] << " does not export InitDll.\n", 1;
  if ((exit_dll = get_proc_address(library, "ExitDll")) == nullptr)
    return std::cout << argv[1] << " does not export ExitDll.\n", 1;
  if ((create_topology = get_proc_address(library, "inf_vst_create_topology")) == nullptr)
    return std::cout << argv[1] << " does not export inf_vst_create_topology.\n", 1;
  if (!reinterpret_cast<inf_init_exit_dll_t>(init_dll)())
    return std::cout << "InitDll returned false.\n", 1;

  if (!(topology = std::unique_ptr<topology_info>(reinterpret_cast<inf_create_topology_t>(create_topology)())))
  {
    std::cout << "inf_vst_create_topology returned NULL.\n";
    reinterpret_cast<inf_init_exit_dll_t>(exit_dll)();
    return 1;
  }

  try
  {
    std::cout << "Loaded library " << argv[1] << ".\n";
    controller_ui_description description = controller_ui_description::create(*topology);
    Document json(build_vstgui_json(*topology, description));
    std::ofstream os(argv[2]);
    OStreamWrapper wrapper(os);
    PrettyWriter<OStreamWrapper> writer(wrapper);
    json.Accept(writer);
    if (os.bad()) return failed(argv[2], exit_dll);
    os.flush();
    os.close();
    print_uidesc(*topology, description);
  }
  catch (std::exception const& e)
  {
    std::cout << e.what() << "\n";
    return failed(argv[2], exit_dll);
  }

  std::cout << "Ui description written to " << argv[2] << ".\n\n";
  reinterpret_cast<inf_init_exit_dll_t>(exit_dll)();
  return 0;
}
