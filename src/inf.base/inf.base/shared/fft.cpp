#include <inf.base/shared/fft.hpp>
#include <inf.base/shared/support.hpp>
#include <cassert>

namespace inf::base {

std::vector<std::complex<float>> const& 
fft::transform(float const* audio, std::size_t count)
{
  _fft.clear();
  std::size_t pow2 = next_pow2(count);
  for (std::size_t i = 0; i < count; i++)
    _fft.push_back(std::complex<float>(audio[i], 0.0f));
  _fft.insert(_fft.end(), pow2 - count, std::complex<float>(0.0f, 0.0f));
  _scratch.resize(pow2);
  transform(_fft.data(), _scratch.data(), pow2);
  // Discard above nyquist.
  _fft.erase(_fft.begin() + _fft.size() / 2, _fft.end());
  return _fft;
}

// https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B
void
fft::transform(std::complex<float>* inout, std::complex<float>* scratch, std::size_t count)
{
  if (count < 2) return;
  assert(count == next_pow2(count));
  std::complex<float>* even = scratch;
  std::complex<float>* odd = scratch + count / 2;
  for (std::size_t i = 0; i < count / 2; i++) even[i] = inout[i * 2];
  for (std::size_t i = 0; i < count / 2; i++) odd[i] = inout[i * 2 + 1];
  transform(odd, inout, count / 2);
  transform(even, inout, count / 2);
  for (std::size_t i = 0; i < count / 2; i++)
  {
    float im = -2.0f * pi32 * i / count;
    std::complex<float> t = std::polar(1.0f, im) * odd[i];
    inout[i] = even[i] + t;
    inout[i + count / 2] = even[i] - t;
  }
}

} // namespace inf::base