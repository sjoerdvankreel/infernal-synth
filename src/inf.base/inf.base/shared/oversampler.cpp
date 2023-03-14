#include <inf.base/shared/oversampler.hpp>

namespace inf::base {

static inline constexpr double coeff_16[2] = 
{ 0.10717745346023573, 0.53091435354504557 };
static inline constexpr double coeff_8[3] = 
{ 0.055748680811302048, 0.24305119574153072, 0.64669913119268196 };
static inline constexpr double coeff_4[4] = 
{ 0.041893991997656171, 0.16890348243995201, 0.39056077292116603, 0.74389574826847926 };
static inline constexpr double coeff_2[12] = { 
  0.036681502163648017, 0.13654762463195794, 0.27463175937945444, 
  0.42313861743656711, 0.56109869787919531,  0.67754004997416184, 
  0.76974183386322703, 0.83988962484963892, 0.89226081800387902, 
  0.9315419599631839, 0.96209454837808417, 0.98781637073289585 };
   
oversampler::
oversampler(std::int32_t max_channels, std::int32_t max_samples):
_order(0), _channels(0), 
_max_samples(max_samples), 
_max_channels(max_channels),
_up2_data(max_channels, 2 * max_samples), 
_up4_data(max_channels, 4 * max_samples),
_up8_data(max_channels, 8 * max_samples), 
_up16_data(max_channels, 16 * max_samples),
_down2_data(max_channels, 2 * max_samples), 
_down4_data(max_channels, 4 * max_samples),
_down8_data(max_channels, 8 * max_samples), 
_down16_data(max_channels, 16 * max_samples),
_up2(max_channels), _up4(max_channels),
_up8(max_channels), _up16(max_channels),
_down2(max_channels), _down4(max_channels),
_down8(max_channels), _down16(max_channels)
{
  for(std::int32_t c = 0; c < max_channels; c++)
  {
    _up2[c].set_coefs(coeff_2);
    _up4[c].set_coefs(coeff_4);
    _up8[c].set_coefs(coeff_8);
    _up16[c].set_coefs(coeff_16);
    _down2[c].set_coefs(coeff_2);
    _down4[c].set_coefs(coeff_4);
    _down8[c].set_coefs(coeff_8);
    _down16[c].set_coefs(coeff_16);
  }
}

void
oversampler::clear()
{
  for (std::int32_t c = 0; c < _channels; c++)
  {
    _up2[c].clear_buffers();
    _up4[c].clear_buffers();
    _up8[c].clear_buffers();
    _up16[c].clear_buffers();
    _down2[c].clear_buffers();
    _down4[c].clear_buffers();
    _down8[c].clear_buffers();
    _down16[c].clear_buffers();
  }
}

void 
oversampler::rearrange(std::int32_t channels, std::int32_t order)
{
  assert(0 <= order && order <= 4);
  assert(0 <= channels && channels <= _max_channels);
  if (_channels != channels || _order != order)
  {
    _order = order;
    _channels = channels;
    clear();
  }
}

} // namespace inf::base