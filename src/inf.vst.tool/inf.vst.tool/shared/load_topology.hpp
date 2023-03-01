#ifndef INF_VST_TOOL_SHARED_LOAD_TOPOLOGY_HPP
#define INF_VST_TOOL_SHARED_LOAD_TOPOLOGY_HPP

#include <inf.base/topology/topology_info.hpp>
#include <memory>

namespace inf::vst::tool::shared {

std::unique_ptr<inf::base::topology_info>
load_topology(char const* library_path);

} // namespace inf::vst::tool::shared
#endif // INF_VST_TOOL_SHARED_LOAD_TOPOLOGY_HPP