#include <inf.base/shared/support.hpp>
#include <immintrin.h>

namespace inf::base {

note_to_frequency_table_init const
note_to_frequency_table_init::init;

std::uint64_t
disable_denormals()
{
  std::uint32_t ftz = _MM_GET_FLUSH_ZERO_MODE();
  std::uint32_t daz = _MM_GET_DENORMALS_ZERO_MODE();
  _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
  _MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);
  return ((static_cast<std::uint64_t>(ftz) << 32) & 0xFFFFFFFF00000000) | daz;
}
    
void
restore_denormals(std::uint64_t state)
{
  std::uint32_t daz = static_cast<std::uint32_t>(state & 0x00000000FFFFFFFF);
  std::uint32_t ftz = static_cast<std::uint32_t>((state & 0xFFFFFFFF00000000) >> 32);
  _MM_SET_FLUSH_ZERO_MODE(ftz);
  _MM_SET_DENORMALS_ZERO_MODE(daz);
}

note_to_frequency_table_init::
note_to_frequency_table_init():
table()
{
  for(std::int32_t o = 0; o < octave_count; o++)
    for(std::int32_t n = 0; n < note_count; n++)
      for(std::int32_t c = 0; c < cent_count; c++)
        table[o * (note_count * cent_count) + n * cent_count + c] = note_to_frequency(o * note_count + n + c / 100.0f);
}

} // namespace inf::base