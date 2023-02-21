#ifndef SVN_BASE_SHARED_FFT_HPP
#define SVN_BASE_SHARED_FFT_HPP

#include <vector>
#include <complex>
#include <cstdint>

namespace svn::base {

class fft
{
  std::vector<std::complex<float>> _fft;
  std::vector<std::complex<float>> _scratch;
  void transform(std::complex<float>* inout, std::complex<float>* scratch, std::size_t count);

public:
  fft();
  std::vector<std::complex<float>> const& transform(std::vector<float> const& audio);
  std::vector<std::complex<float>> const& transform(float const* audio, std::size_t count);
};

inline fft::fft(): 
_fft(), _scratch() {}

inline std::vector<std::complex<float>> const& 
fft::transform(std::vector<float> const& audio)
{ return transform(audio.data(), audio.size()); };

} // namespace svn::base
#endif // SVN_BASE_SHARED_FFT_HPP