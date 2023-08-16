#include <inf.base.format.vst/vst_io_stream.hpp>

namespace inf::base::format::vst {

bool 
vst_io_stream::read_int32(std::int32_t& val)
{ 
  Steinberg::int32 sval;
  if(!_streamer->readInt32(sval)) return false;
  val = sval;
  return true;
}

bool
vst_io_stream::read_uint32(std::uint32_t& val)
{
  Steinberg::uint32 uval;
  if (!_streamer->readInt32u(uval)) return false;
  val = uval;
  return true;
}

bool
vst_io_stream::read_string(std::string& val)
{
  char chr;
  Steinberg::int32 size;
  val.clear();
  if (!_streamer->readInt32(size)) return false;
  for (std::int32_t i = 0; i < size; i++)
  {
    if (!_streamer->readChar8(chr)) return false;
    val.append(1, chr);
  }
  return true;
}

bool
vst_io_stream::write_string(std::string const& val)
{
  auto size = static_cast<std::int32_t>(val.size());
  if (!_streamer->writeInt32(size)) return false;
  for (std::int32_t i = 0; i < size; i++)
    if (!_streamer->writeChar8(val[i])) return false;
  return true;
}

} // namespace inf::base::format::vst