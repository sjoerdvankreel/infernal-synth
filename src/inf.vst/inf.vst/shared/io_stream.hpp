#ifndef INF_VST_BASE_SHARED_IO_STREAM_HPP
#define INF_VST_BASE_SHARED_IO_STREAM_HPP

#include <base/source/fstreamer.h>
#include <inf.base/shared/io_stream.hpp>

#include <string>
#include <cstdint>

namespace inf::vst::base {

// Forwarding to vst3 I/O.
class vst_io_stream:
public inf::base::io_stream
{
private:
  Steinberg::IBStreamer* const _streamer;

public:
  explicit vst_io_stream(Steinberg::IBStreamer* streamer):
  _streamer(streamer) {}

  // Base needs these.
  bool read_int32(std::int32_t& val) override;
  bool read_uint32(std::uint32_t& val) override;
  bool read_string(std::string& val) override;
  bool write_string(std::string const& val) override;

  bool read_float(float& val) override { return _streamer->readFloat(val); }
  bool write_float(float val) override { return _streamer->writeFloat(val); }
  bool write_int32(std::int32_t val) override { return _streamer->writeInt32(val); }
  bool write_uint32(std::uint32_t val) override { return _streamer->writeInt32u(val); }
};

} // namespace inf::vst::base
#endif // INF_VST_BASE_SHARED_IO_STREAM_HPP