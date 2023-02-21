#ifndef SVN_BASE_TOPOLOGY_PARAM_UI_DESCRIPTOR_HPP
#define SVN_BASE_TOPOLOGY_PARAM_UI_DESCRIPTOR_HPP

#include <vector>
#include <cstdint>

// For ui generator.
namespace svn::base {

// Relevant if predicate holds.
struct param_relevance
{
  std::int32_t const if_param;
  bool (*predicate)(std::int32_t value);
};

// Menu settings and conditional visibility.
struct param_ui_descriptor
{
  bool menu_wide; // Put label in knob space, menu takes up both label and control width.
  std::vector<param_relevance> const relevance; // Relevant if all other params relevance[i].if_param satisfy relevance[i].predicate.
};

} // namespace svn::base
#endif // SVN_BASE_TOPOLOGY_PARAM_UI_DESCRIPTOR_HPP
