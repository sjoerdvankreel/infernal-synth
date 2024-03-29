#ifndef INF_BASE_SHARED_IO_STREAM_HPP
#define INF_BASE_SHARED_IO_STREAM_HPP

#include <inf.base/shared/state.hpp>
#include <inf.base/topology/topology_info.hpp>

#include <map>
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
  virtual bool read_uint32(std::uint32_t& val) = 0;
  virtual bool read_string(std::string& val) = 0; 

  virtual bool write_float(float val) = 0;
  virtual bool write_int32(std::int32_t val) = 0;
  virtual bool write_uint32(std::uint32_t val) = 0;
  virtual bool write_string(std::string const& val) = 0;

  // Reads/writes arrays of param_value according to topology.
  bool load_processor(topology_info const& topology, param_value* state);
  bool save_processor(topology_info const& topology, param_value const* state);

  // Reads/writes ui metadata.
  bool load_controller(topology_info const& topology, std::map<std::string, std::string>& meta_data);
  bool save_controller(topology_info const& topology, std::map<std::string, std::string> const& meta_data);
};

} // namespace inf::base
#endif // INF_BASE_SHARED_IO_STREAM_HPP