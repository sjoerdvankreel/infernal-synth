#include <inf.base/shared/support.hpp>
#include <inf.base/shared/spectrum_analyzer.hpp>
#include <cassert>

namespace inf::base {

// https://stackoverflow.com/questions/604453/analyze-audio-using-fast-fourier-transform
// https://dsp.stackexchange.com/questions/46692/calculating-1-3-octave-spectrum-from-fft-dft
float 
spectrum_analyzer::power_at_note(std::vector<std::complex<float>> const& fft, std::int32_t midi, float resolution) const
{
  float result = 0.0f;
  float this_note_freq = note_to_frequency_table(static_cast<float>(midi));
  float next_note_freq = note_to_frequency_table(static_cast<float>(midi + 1));
  float fend_bin = next_note_freq / resolution;
  float fstart_bin = this_note_freq / resolution;
  std::int32_t end_bin = static_cast<std::int32_t>(std::ceil(end_bin));
  std::int32_t start_bin = static_cast<std::int32_t>(std::floor(start_bin));
  for (std::int32_t i = start_bin; i < end_bin; i++)
  {
    float weight = 1.0f;
    if(i == start_bin && i == end_bin -1) 
      weight = (fend_bin - fstart_bin) / resolution;
    else if (i == end_bin - 1)
      weight = (fend_bin - static_cast<float>(end_bin)) / resolution;
    else if(i == start_bin)
      weight = (1.0f - (fstart_bin - static_cast<float>(start_bin))) / resolution;
    float real2 = fft[i].real() * fft[i].real();
    float imag2 = fft[i].imag() * fft[i].imag();
    result += (real2 + imag2) * weight;
  }
  return sanity(std::sqrt(result));
}

float const*
spectrum_analyzer::analyze(float const* audio, std::size_t count, float resolution)
{
  _output.clear();
  float max = 0.0f;
  std::vector<std::complex<float>> const& fft = _fft.transform(audio, count);
  for (std::int32_t oct = 0; oct < 12; oct++)
    for (std::int32_t note = 0; note < 12; note++)
      _output.push_back(power_at_note(fft, oct * 12 + note, resolution));
  for (std::size_t i = 0; i < _output.size(); i++) max = std::max(_output[i], max);
  for (std::size_t i = 0; i < _output.size(); i++) _output[i] = max == 0.0f ? 0.0f : sanity_unipolar(_output[i] / max);
  return _output.data();
}

} // namespace inf::base