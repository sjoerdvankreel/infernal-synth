#include <inf.base/shared/generic_io_stream.hpp>

namespace inf::base {

bool
generic_io_stream::write_string(std::string const& val)
{
  write_int32(static_cast<std::int32_t>(val.size()));
  for (std::size_t i = 0; i < val.size(); i++)
    write(val[i]);
  return true;
}

bool 
generic_io_stream::read_string(std::string& val)
{
  char c;
  std::int32_t size;
  if(!read_int32(size)) return false;

  val.clear();
  for(std::int32_t i = 0; i < size; i++)
    if(!read(c)) return false;
    else val.push_back(c);
  return true;
}

} // namespace inf::base
