#ifndef SVN_BASE_SHARED_STATE_HPP
#define SVN_BASE_SHARED_STATE_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace svn::base {

// Extra info for list items.
typedef char const* (*list_item_info)(std::int32_t index);

inline std::vector<std::string> const note_names = {
  { "C" }, { "C#" }, { "D" }, { "D#" }, { "E" }, { "F" },
  { "F#" }, { "G" }, { "G#" }, { "A" }, { "A#" }, { "B" } };

// Timesig generator base type.
struct timesig_type_t { enum value { one_over, over_one, upper, lower }; };
typedef timesig_type_t::value timesig_type;

// 1/1, 1/2, 1/4 etc.
struct time_signature
{
  std::int32_t num;
  std::int32_t den;  
  bool operator==(time_signature const&) const = default;
};

// Value container for both real and discrete valued params.
union param_value
{
  float real;
  std::int32_t discrete;

  param_value() : discrete(0) {}
  explicit param_value(float val) : real(val) {}
  explicit param_value(std::int32_t val) : discrete(val) {}
};

// For list items, allows us to serialize the guid and change 
// the name while still be able to load previous versions.
struct list_item
{
  // Item id. 
  // Should be a single guid or concatenation of guids/indices in case of list/zip_list/multi_list, 
  // or identity for timesig.
  std::string const id; 
  std::string const name; // Item name.
  std::vector<std::string> const submenu_path; // Optional, for use with ziplist + regular dropdown only.

  list_item(std::string const& id, std::string const& name) :
  id(id), name(name), submenu_path() {}

  list_item(std::string const& id, std::string const& name, std::vector<std::string> const& submenu_path):
  id(id), name(name), submenu_path(submenu_path) {}
};

// Generic short name/detailed description.
struct item_name
{
  char const* const short_; // Short name, e.g. "Pw".
  char const* const detail; // Long name, e.g. "Pulse width".
  item_name(char const* short_, char const* detail): short_(short_), detail(detail) {}
};

// (Part x, index y, param z) <-> zip list index.
std::vector<std::vector<std::vector<std::int32_t>>>
zip_list_table_init_in(std::int32_t const* counts1, 
std::int32_t const* counts2, std::int32_t count);
std::vector<std::tuple<std::int32_t, std::int32_t, std::int32_t>>
zip_list_table_init_out(std::int32_t const* counts1, 
std::int32_t const* counts2, std::int32_t count);

// Make list of names.
std::vector<std::string>
generate_names(std::int32_t count, std::string(*name)(std::int32_t));

// (Part x, index y) <-> multi list index.
std::vector<std::vector<std::int32_t>> 
multi_list_table_init_in(std::int32_t const* counts, std::int32_t count);
std::vector<std::pair<std::int32_t, std::int32_t>>
multi_list_table_init_out(std::int32_t const* counts, std::int32_t count);

// E.g. "ExtraInfo", "2", "3".
std::vector<std::string>
list_tab_items(list_item_info info, std::int32_t count);
// E.g. ("id", Osc"), 2 -> ("id1", "Osc 1"), ("id2", "Osc 2").
std::vector<list_item>
list_items(list_item const& item, list_item_info info, bool space, std::int32_t count);

// Generate time signatures.
std::vector<std::string>
synced_timesig_names(std::vector<time_signature> const& signatures);
std::vector<float>
synced_timesig_values(std::vector<time_signature> const& signatures);
std::vector<time_signature>
synced_timesig(bool with_zero, std::pair<std::int32_t, std::int32_t> max,
std::vector<std::pair<timesig_type, std::vector<std::int32_t>>> const& series);

// Pass list of id/name with count per name plus second list specific to item in first.
// E.g. [("idA", "Osc"), ("idB", "Flt")], [2, 1], [("idC", "Amp"), ("idD, "Pan")], [("idE", "Freq"), ("idF, "Res")] 
// -> [("idA1idC", "Osc 1 Amp"), ("idA1idD", "Osc 1 Pan"), ("idA2idC", "Osc 2 Amp"), ("idA2idD", "Osc 2 Pan"), ("idB1idE", "Flt Freq"), ("idB1idF", "Flt Res")] 
std::vector<list_item>
zip_list_items(
list_item const* items1, bool const* space1, std::int32_t const* counts1,
list_item const* const* items2, std::int32_t const* counts2, std::int32_t count);

// Pass list of id/name with count per name.
// E.g. [("idA", "Osc"), ("idB", "Flt")], [2, 1] -> [("idA1", "Osc 1"), ("idA2", "Osc 2"), ("idB", "Flt")].
std::vector<list_item>
multi_list_items(list_item const* items, list_item_info const* infos, bool const* spaces, 
char const* const* suffixes, std::int32_t const* counts, std::int32_t count, bool sub_menu);

} // namespace svn::base
#endif // SVN_BASE_SHARED_STATE_HPP