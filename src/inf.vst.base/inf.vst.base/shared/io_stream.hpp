#ifndef SVN_VST_BASE_SHARED_IO_STREAM_HPP
#define SVN_VST_BASE_SHARED_IO_STREAM_HPP

#include <base/source/fstreamer.h>
#include <svn.base/shared/io_stream.hpp>

#include <string>
#include <cstdint>

namespace svn::vst::base {

// Forwarding to vst3 I/O.
class vst_io_stream:
public svn::base::io_stream
{
private:
  Steinberg::IBStreamer* const _streamer;

public:
  explicit vst_io_stream(Steinberg::IBStreamer* streamer):
  _streamer(streamer) {}

  // Base needs these.
  bool read_int32(std::int32_t& val) override;
  bool read_string(std::string& val) override;
  bool write_string(std::string const& val) override;

  bool read_float(float& val) override { return _streamer->readFloat(val); }
  bool write_float(float val) override { return _streamer->writeFloat(val); }
  bool write_int32(std::int32_t val) override { return _streamer->writeInt32(val); }
};

} // namespace svn::vst::base
#endif // SVN_VST_BASE_SHARED_IO_STREAM_HPP