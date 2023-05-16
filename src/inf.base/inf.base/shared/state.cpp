#include <inf.base/shared/state.hpp>
#include <algorithm>
#include <numeric>
#include <cassert>

namespace inf::base {

std::vector<std::string>
generate_names(std::int32_t count, std::string(*name)(std::int32_t n))
{
  std::vector<std::string> result;
  for(std::int32_t i = 0; i < count; i++)
    result.push_back(name(i));
  return result;
}

std::vector<std::vector<std::int32_t>> 
multi_list_table_init_in(std::int32_t const* counts, std::int32_t count)
{
  std::int32_t index = 0;
  std::vector<std::vector<std::int32_t>> result;
  for (std::int32_t i = 0; i < count; i++)
  {
    std::vector<std::int32_t> inner;
    for(std::int32_t j = 0; j < counts[i]; j++)
      inner.push_back(index++);
    result.push_back(inner);
  }
  return result;
}

std::vector<std::pair<std::int32_t, std::int32_t>>
multi_list_table_init_out(std::int32_t const* counts, std::int32_t count)
{
  std::vector<std::pair<std::int32_t, std::int32_t>> result;
  for(std::int32_t i = 0; i < count; i++)
    for(std::int32_t j = 0; j < counts[i]; j++)
      result.push_back(std::make_pair(i, j));
  return result;
}

std::vector<std::vector<std::vector<std::int32_t>>>
zip_list_table_init_in(std::int32_t const* counts1, std::int32_t const* counts2, std::int32_t count)
{
  std::int32_t index = 0;
  std::vector<std::vector<std::vector<std::int32_t>>> result;
  for(std::int32_t i = 0; i < count; i++)
  {
    std::vector<std::vector<std::int32_t>> inner1;
    for(std::int32_t j = 0; j < counts1[i]; j++)
    {
      std::vector<std::int32_t> inner2;
      if(counts2[i] == 0)
        inner2.push_back(index++);
      else
        for(std::int32_t k = 0; k < counts2[i]; k++)
          inner2.push_back(index++);
      inner1.push_back(inner2);
    }
    result.push_back(inner1);
  }
  return result;
}

std::vector<std::tuple<std::int32_t, std::int32_t, std::int32_t>>
zip_list_table_init_out(std::int32_t const* counts1, std::int32_t const* counts2, std::int32_t count)
{
  std::vector<std::tuple<std::int32_t, std::int32_t, std::int32_t>> result;
  for (std::int32_t i = 0; i < count; i++)
    for(std::int32_t j = 0; j < counts1[i]; j++)
      if(counts2[i] == 0)
        result.push_back(std::make_tuple(i, j, -1));
      else
        for(std::int32_t k = 0; k < counts2[i]; k++)
          result.push_back(std::make_tuple(i, j, k));
  return result;
}

std::vector<list_item>
list_items(list_item const& item, std::int32_t count)
{
  std::vector<list_item> result;
  for (std::int32_t i = 0; i < count; i++)
  {
    std::string name = item.name;
    name += std::to_string(i + 1);
    std::string id = std::string(item.id) + "-" + std::to_string(i);
    result.push_back({ id, name, {} });
  }
  return result;
}

std::vector<list_item>
multi_list_items(
  list_item const* items, char const* const* suffixes, 
  std::int32_t const* counts, std::int32_t count, bool sub_menu)
{
  std::vector<list_item> result;
  for(std::int32_t i = 0; i < count; i++)
    for(std::int32_t j = 0; j < counts[i]; j++)
    {
      std::vector<std::string> sub_menu_items;
      std::string name = items[i].name;
      if(sub_menu) sub_menu_items.push_back(name);
      if(counts[i] > 1) name += " " + std::to_string(j + 1);
      if(suffixes != nullptr && suffixes[i] != nullptr) name += " " + std::string(suffixes[i]);
      if(counts[i] > 1 && sub_menu) sub_menu_items.push_back(name);
      std::string id = std::string(items[i].id) + "-" + std::to_string(j);
      result.push_back({id, name, sub_menu_items });
    }
  return result;
} 

std::vector<list_item>
zip_list_items(list_item const* items1, std::int32_t const* counts1,
  list_item const* const* items2, std::int32_t const* counts2, std::int32_t count)
{
  std::vector<list_item> result;
  for (std::int32_t i = 0; i < count; i++)
    for (std::int32_t j = 0; j < counts1[i]; j++)
      if (counts2[i] == 0)
      {
        std::vector<std::string> submenu_path;
        std::string name = items1[i].name;
        submenu_path.push_back(name);
        std::string id = items1[i].id + "-" + std::to_string(j);
        if (counts1[i] > 1) 
        {
          name += " " + std::to_string(j + 1);
          submenu_path.push_back(name);
        }
        result.push_back({id, name, submenu_path});
      }
      else for(std::int32_t k = 0; k < counts2[i]; k++)
      {
        std::vector<std::string> submenu_path;
        std::string name = items1[i].name;
        submenu_path.push_back(name);
        std::string id = items1[i].id + "-" + std::to_string(j) + "-" + items2[i][k].id;
        if(counts1[i] > 1)
        { 
          name += " " + std::to_string(j + 1);
          submenu_path.push_back(name);
        }
        name = name + " " + items2[i][k].name;
        submenu_path.push_back(name);
        result.push_back({ id, name, submenu_path });
      }
  return result; 
}

std::vector<float>
synced_timesig_values(std::vector<time_signature> const& signatures)
{
  std::vector<float> result;
  for (std::size_t i = 0; i < signatures.size(); i++)
    if(signatures[i].num == 0)
      result.push_back(0.0f);
    else
      result.push_back(static_cast<float>(signatures[i].num) / signatures[i].den);
  return result;
}

std::vector<std::string>
synced_timesig_names(std::vector<time_signature> const& signatures)
{
  std::vector<std::string> result;
  for (std::size_t i = 0; i < signatures.size(); i++)
    if (signatures[i].num == 0)
      result.push_back("0");
    else
      result.push_back(std::to_string(signatures[i].num) + "/" + std::to_string(signatures[i].den));
  return result;
}

std::vector<time_signature>
synced_timesig(bool with_zero, std::pair<std::int32_t, std::int32_t> max,
  std::vector<std::pair<timesig_type, std::vector<std::int32_t>>> const& series)
{
  // Make all signatures. 
  std::vector<time_signature> all;
  if(with_zero)
    all.push_back(time_signature({ 0, 1 }));
  for(std::size_t i = 0; i < series.size(); i++)
    switch (series[i].first)
    {
    case timesig_type::one_over:
      for(std::int32_t j = 0; j < series[i].second.size(); j++)
        all.push_back(time_signature({ 1, series[i].second[j] }));
      break; 
    case timesig_type::over_one:
      for (std::int32_t j = 0; j < series[i].second.size(); j++)
        all.push_back(time_signature({ series[i].second[j], 1 }));
      break;
    case timesig_type::upper:
      for (std::int32_t j = 0; j < series[i].second.size(); j++)
        for (std::int32_t k = 0; k < series[i].second.size(); k++)
          if(series[i].second[k] <= series[i].second[j])
            all.push_back(time_signature({ series[i].second[j], series[i].second[k] }));
      break;
    case timesig_type::lower:
      for (std::int32_t j = 0; j < series[i].second.size(); j++)
        for (std::int32_t k = 0; k < series[i].second.size(); k++)
          if (series[i].second[k] >= series[i].second[j])
            all.push_back(time_signature({ series[i].second[j], series[i].second[k] }));
      break;
    default:
      assert(false);
      break;
    }

  // Filter out equivalents (e.g. 2/4 == 4/8) and stuff outside min/max.
  bool seen_this;
  std::vector<float> seen;
  std::vector<time_signature> result;
  float const epsilon = 1.0e-4;
  float const max_val = static_cast<float>(max.first) / max.second;
  for (std::size_t i = 0; i < all.size(); i++)
  {
    seen_this = false;
    float val = all[i].num == 0? 0.0f: static_cast<float>(all[i].num) / all[i].den;
    if(val > max_val + epsilon) continue;
    for (std::size_t j = 0; j < seen.size(); j++)
      if (val - epsilon <= seen[j] && seen[j] <= val + epsilon)
      {
        seen_this = true;
        break;
      }
    if (seen_this) continue;
    seen.push_back(val);
    std::int32_t gcd = std::gcd(all[i].num, all[i].den);
    result.push_back(time_signature({ all[i].num / gcd, all[i].den / gcd }));
  }

  // Sort ascending.
  std::sort(result.begin(), result.end(), [](auto const& l, auto const& r) -> bool
  { 
    float lval = static_cast<float>(l.num) / l.den;
    float rval = static_cast<float>(r.num) / r.den;
    return lval < rval;
  });

  return result;
}
 
} // namespace inf::base