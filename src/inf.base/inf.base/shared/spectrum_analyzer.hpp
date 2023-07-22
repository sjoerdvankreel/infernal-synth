#ifndef INF_BASE_SHARED_SPECTRUM_ANALYZER_HPP
#define INF_BASE_SHARED_SPECTRUM_ANALYZER_HPP

#include <inf.base/shared/fft.hpp>

#include <vector>
#include <complex>
#include <cstdint>

namespace inf::base {

// Returns spectral power at midi notes [0 .. bucket_count) in range [0..1].
class spectrum_analyzer
{
  fft _fft;
  std::vector<float> _output;  

public:
  spectrum_analyzer();
  std::vector<float> const& analyze(std::vector<float> const& audio);
  std::vector<float> const& analyze(float const* audio, std::size_t count);
};

inline spectrum_analyzer::spectrum_analyzer(): 
_fft(), _output() {}

inline std::vector<float> const&
spectrum_analyzer::analyze(std::vector<float> const& audio)
{ return analyze(audio.data(), audio.size()); }

} // namespace inf::base
#endif // INF_BASE_SHARED_SPECTRUM_ANALYZER_HPP