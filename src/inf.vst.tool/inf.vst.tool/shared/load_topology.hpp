#ifndef INF_VST_TOOL_SHARED_LOAD_TOPOLOGY_HPP
#define INF_VST_TOOL_SHARED_LOAD_TOPOLOGY_HPP

#include <inf.base/topology/topology_info.hpp>
#include <memory>

namespace inf::vst::tool::shared {

// Unload library on release.
typedef bool (*inf_init_exit_dll_t)(void);
class loaded_topology
{
  inf_init_exit_dll_t _unloader;
  std::unique_ptr<inf::base::topology_info> _topology;
public:
  loaded_topology(loaded_topology&&) = default;
  loaded_topology(loaded_topology const&) = delete;
  loaded_topology& operator=(loaded_topology&&) = default;
  loaded_topology& operator=(loaded_topology const&) = delete;
  
  ~loaded_topology();
  loaded_topology(std::unique_ptr<inf::base::topology_info>&& topology, inf_init_exit_dll_t unloader);
  inf::base::topology_info const* topology() const { return _topology.get(); }
};

std::unique_ptr<loaded_topology>
load_topology(char const* library_path);

} // namespace inf::vst::tool::shared
#endif // INF_VST_TOOL_SHARED_LOAD_TOPOLOGY_HPP