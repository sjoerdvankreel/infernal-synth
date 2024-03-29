#ifndef INF_BASE_SHARED_STATE_HPP
#define INF_BASE_SHARED_STATE_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace inf::base {

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
  std::int32_t zip_index1; // For matching list items in enabled selectors.
  std::int32_t zip_index2; // For matching list items in enabled selectors.
  std::int32_t zip_index3; // For matching list items in enabled selectors.
  std::vector<std::string> const submenu_path; // Optional, for use with ziplist + regular dropdown only.

  list_item(std::string const& id, std::string const& name) :
  id(id), name(name), zip_index1(-1), zip_index2(-1), zip_index3(-1), submenu_path() {}
  list_item(std::string const& id, std::string const& name, std::vector<std::string> const& submenu_path, 
    std::int32_t zip_index1, std::int32_t zip_index2, std::int32_t zip_index3):
  id(id), name(name), zip_index1(zip_index1), zip_index2(zip_index2), zip_index3(zip_index3), submenu_path(submenu_path) {}
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

// E.g. ("id", Osc"), 2 -> ("id1", "Osc 1"), ("id2", "Osc 2").
std::vector<list_item>
list_items(list_item const& item, std::int32_t count);

// Generate time signatures.
std::vector<std::string>
synced_timesig_names(std::vector<time_signature> const& signatures);
std::vector<float>
synced_timesig_values(std::vector<time_signature> const& signatures);
std::vector<time_signature>
synced_timesig(bool with_zero, std::pair<std::int32_t, std::int32_t> max,
std::vector<std::pair<timesig_type, std::vector<std::int32_t>>> const& series);

// Pass list of id/name with count per name.
// E.g. [("idA", "Osc"), ("idB", "Flt")], [2, 1] -> [("idA1", "Osc 1"), ("idA2", "Osc 2"), ("idB", "Flt")].
std::vector<list_item>
multi_list_items(
  list_item const* items, char const* const* suffixes,
  std::int32_t const* counts, std::int32_t count, bool sub_menu);

// Pass list of id/name with count per name plus second list specific to item in first.
// E.g. [("idA", "Osc"), ("idB", "Flt")], [2, 1], [("idC", "Amp"), ("idD, "Pan")], [("idE", "Freq"), ("idF, "Res")] 
// -> [("idA1idC", "Osc 1 Amp"), ("idA1idD", "Osc 1 Pan"), ("idA2idC", "Osc 2 Amp"), ("idA2idD", "Osc 2 Pan"), ("idB1idE", "Flt Freq"), ("idB1idF", "Flt Res")] 
std::vector<list_item>
zip_list_items(
   list_item const* items1, std::int32_t const* counts1,
  list_item const* const* items2, std::int32_t const* counts2, 
  std::int32_t count);

} // namespace inf::base
#endif // INF_BASE_SHARED_STATE_HPP