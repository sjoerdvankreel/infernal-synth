#ifndef INF_BASE_GENERIC_IO_STREAM_HPP
#define INF_BASE_GENERIC_IO_STREAM_HPP

#include <inf.base/shared/io_stream.hpp>

#include <vector>
#include <string>
#include <cstdint>

namespace inf::base {

class generic_io_stream:
public inf::base::io_stream
{
private:
  std::size_t _pos = 0;
  std::vector<std::uint8_t> _data = {};
  
  template <class T> bool read(T& val)
  {
    if(_pos + sizeof(T) > _data.size()) return false;
    val = *reinterpret_cast<T*>(_data.data() + _pos);
    _pos += sizeof(T);
    return true;
  }

  template <class T> bool write(T val)
  {
    for (std::size_t i = 0; i < sizeof(T); i++)
      _data.push_back(0);
    *reinterpret_cast<T*>(_data.data() + _pos) = val;
    _pos += sizeof(T);
    return true;
  }

public:
  generic_io_stream() {}
  generic_io_stream(std::uint8_t const* data, std::size_t size)
  { _data.insert(_data.begin(), data, data + size); }

  void reset() { _pos = 0; }
  std::size_t size() const { return _data.size(); }
  std::uint8_t const* data() const { return _data.data(); }

  bool read_float(float& val) override { return read(val); }
  bool read_int32(std::int32_t& val) override { return read(val); }
  bool read_uint32(std::uint32_t& val) override { return read(val); }
  bool read_string(std::string& val) override;

  bool write_string(std::string const& val) override;
  bool write_float(float val) override { return write(val); }
  bool write_int32(std::int32_t val) override { return write(val); }
  bool write_uint32(std::uint32_t val) override { return write(val); }
};

} // namespace inf::base
#endif // INF_BASE_GENERIC_IO_STREAM_HPP