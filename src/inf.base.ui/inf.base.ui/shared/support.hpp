#ifndef INF_BASE_UI_SHARED_SUPPORT_HPP
#define INF_BASE_UI_SHARED_SUPPORT_HPP

#include <inf.base/shared/state.hpp>
#include <inf.base/topology/topology_info.hpp>

#include <string>
#include <cstdint>

namespace inf::base::ui 
{

enum class label_kind { label, value, both };

std::string
get_label_text(base::param_descriptor const* descriptor, label_kind kind, base::param_value value);

} // namespace inf::base::ui
#endif // INF_BASE_UI_SHARED_SUPPORT_HPP