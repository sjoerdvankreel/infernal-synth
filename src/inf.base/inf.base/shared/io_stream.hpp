#ifndef SVN_BASE_SHARED_IO_STREAM_HPP
#define SVN_BASE_SHARED_IO_STREAM_HPP

#include <inf.base/shared/state.hpp>
#include <inf.base/topology/topology_info.hpp>

#include <string>
#include <cstdint>

namespace inf::base {

class io_stream
{
public:
  virtual ~io_stream() = default;

  // Plugin format must provide these.
  virtual bool read_float(float& val) = 0;
  virtual bool read_int32(std::int32_t& val) = 0;
  virtual bool read_string(std::string& val) = 0; 

  virtual bool write_float(float val) = 0;
  virtual bool write_int32(std::int32_t val) = 0;
  virtual bool write_string(std::string const& val) = 0;

  // Reads/writes arrays of param_value according to topology.
  bool load(topology_info const& topology, param_value* state);
  bool save(topology_info const& topology, param_value const* state);
};

} // namespace inf::base
#endif // SVN_BASE_SHARED_IO_STREAM_HPP