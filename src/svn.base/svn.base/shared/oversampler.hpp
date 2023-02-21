#ifndef SVN_BASE_SHARED_OVERSAMPLER_HPP
#define SVN_BASE_SHARED_OVERSAMPLER_HPP

#include <svn.base/shared/cont_storage.hpp>
#include <hiir/Upsampler2xFpu.h>
#include <hiir/Downsampler2xFpu.h>

namespace svn::base
{

// Oversampling using HIIR http://ldesoras.free.fr/.
// Max order is 4, for 16x oversampling.
class oversampler
{
  std::int32_t _order;
  std::int32_t _channels;
  std::int32_t _max_samples;
  std::int32_t _max_channels;

  cont_storage<float> _up2_data;
  cont_storage<float> _up4_data;
  cont_storage<float> _up8_data;
  cont_storage<float> _up16_data;
  cont_storage<float> _down2_data;
  cont_storage<float> _down4_data;
  cont_storage<float> _down8_data;
  cont_storage<float> _down16_data;

  std::vector<hiir::Upsampler2xFpu<12>> _up2;
  std::vector<hiir::Upsampler2xFpu<4>> _up4;
  std::vector<hiir::Upsampler2xFpu<3>> _up8;
  std::vector<hiir::Upsampler2xFpu<2>> _up16;
  std::vector<hiir::Downsampler2xFpu<12>> _down2;
  std::vector<hiir::Downsampler2xFpu<4>> _down4;
  std::vector<hiir::Downsampler2xFpu<3>> _down8;
  std::vector<hiir::Downsampler2xFpu<2>> _down16;

  void clear();

public:

  oversampler() = default;
  ~oversampler() = default;
  oversampler(oversampler&&) = default;
  oversampler(oversampler const&) = delete;
  oversampler& operator=(oversampler&&) = default;
  oversampler& operator=(oversampler const&) = delete;
  oversampler(std::int32_t max_channels, std::int32_t max_samples);

  void rearrange(std::int32_t channels, std::int32_t order);
  template <typename op_type>
  void process(float const* const* in, float* const* out, std::int32_t sample_count, op_type op);
};

template <typename op_type>
inline void 
oversampler::process(float const* const* in, float* const* out, std::int32_t sample_count, op_type op)
{
  for(std::int32_t c = 0; c < _channels; c++)
  {
    if (_order > 0) 
      _up2[c].process_block(_up2_data.buffer(c), in[c], sample_count);
    if (_order > 1) 
      _up4[c].process_block(_up4_data.buffer(c), _up2_data.buffer(c), sample_count * 2);
    if (_order > 2) 
      _up8[c].process_block(_up8_data.buffer(c), _up4_data.buffer(c), sample_count * 4);
    if (_order > 3) 
      _up16[c].process_block(_up16_data.buffer(c), _up8_data.buffer(c), sample_count * 8);
  }

  for (std::int32_t c = 0; c < _channels; c++)
    switch (_order)
    {
    case 0: 
      for (std::int32_t i = 0; i < sample_count; i++) 
        out[c][i] = op(c, i, in[c][i]); 
      break;
    case 1: 
      for (std::int32_t i = 0; i < sample_count * 2; i++) 
        _down2_data.buffer(c)[i] = op(c, i / 2, _up2_data.buffer(c)[i]); 
      break;
    case 2: 
      for (std::int32_t i = 0; i < sample_count * 4; i++) 
        _down4_data.buffer(c)[i] = op(c, i / 4, _up4_data.buffer(c)[i]); 
      break;
    case 3: 
      for (std::int32_t i = 0; i < sample_count * 8; i++) 
        _down8_data.buffer(c)[i] = op(c, i / 8, _up8_data.buffer(c)[i]); 
      break;
    case 4: 
      for (std::int32_t i = 0; i < sample_count * 16; i++) 
        _down16_data.buffer(c)[i] = op(c, i / 16, _up16_data.buffer(c)[i]); 
      break;
    default: 
      assert(false); 
      break;
    }

  for (std::int32_t c = 0; c < _channels; c++)
  {
    if (_order > 3) 
      _down16[c].process_block(_down8_data.buffer(c), _down16_data.buffer(c), sample_count * 8);
    if (_order > 2) 
      _down8[c].process_block(_down4_data.buffer(c), _down8_data.buffer(c), sample_count * 4);
    if (_order > 1) 
      _down4[c].process_block(_down2_data.buffer(c), _down4_data.buffer(c), sample_count * 2);
    if (_order > 0) 
      _down2[c].process_block(out[c], _down2_data.buffer(c), sample_count);
  }
}

} // namespace svn::base
#endif // SVN_BASE_SHARED_OVERSAMPLER_HPP
