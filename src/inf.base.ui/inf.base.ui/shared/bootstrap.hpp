#ifndef INF_BASE_UI_SHARED_BOOTSTRAP_HPP
#define INF_BASE_UI_SHARED_BOOTSTRAP_HPP

#include <inf.base/topology/topology_info.hpp>

namespace inf::base::ui {

void terminate();
void initialize(base::topology_info const* topology);

} // namespace inf::base::ui
#endif // INF_BASE_UI_SHARED_BOOTSTRAP_HPP