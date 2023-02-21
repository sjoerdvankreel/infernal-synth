#ifndef SVN_BASE_SHARED_CONT_STORAGE_HPP
#define SVN_BASE_SHARED_CONT_STORAGE_HPP

#include <vector>
#include <cstring>
#include <cstdint>
#include <cassert>

namespace svn::base {

// buffer with contiguous allocated data plus pointers into it
template <class T>
class cont_storage
{
  std::vector<T> _storage;
  std::vector<T*> _values;
  std::int32_t _sample_count;
  std::int32_t _storage_count;

public:
  // Interior pointers.
  cont_storage() = default;
  cont_storage(cont_storage&&) = default;
  cont_storage& operator=(cont_storage&&) = default;
  cont_storage(cont_storage const&) = delete;
  cont_storage& operator=(cont_storage const&) = delete;

  T* const* buffers();
  T const* const* buffers() const;
  T* buffer(std::int32_t storage);
  T const* buffer(std::int32_t storage) const;

  void clear(std::int32_t sample_count);
  void resize(std::int32_t storage_count, std::int32_t sample_count);

  cont_storage(std::int32_t storage_count, std::int32_t sample_count);
};

template <class T>
inline cont_storage<T>::
cont_storage(std::int32_t storage_count, std::int32_t sample_count) :
_storage(), _values(), 
_sample_count(sample_count), 
_storage_count(storage_count)
{
  assert(sample_count > 0);
  assert(storage_count > 0);
  resize(storage_count, sample_count);
}

template <class T>
inline T* const* 
cont_storage<T>::buffers() 
{ return _values.data(); }

template <class T>
inline T const* const* 
cont_storage<T>::buffers() const 
{ return _values.data(); }

template <class T>
inline T*
cont_storage<T>::buffer(std::int32_t storage)
{
  assert(0 <= storage && storage < _storage_count);
  return _values[storage];
}

template <class T>
inline T const*
cont_storage<T>::buffer(std::int32_t storage) const
{
  assert(0 <= storage && storage < _storage_count);
  return _values[storage];
}

template <class T>
inline void
cont_storage<T>::clear(std::int32_t sample_count)
{
  assert(0 <= sample_count && sample_count <= _sample_count);
  for (std::size_t i = 0; i < _storage_count; i++)
    std::memset(_values[i], 0, sizeof(T) * sample_count);
}

template <class T>
inline void
cont_storage<T>::resize(std::int32_t storage_count, std::int32_t sample_count)
{
  assert(sample_count > 0);
  assert(storage_count > 0);
  _sample_count = sample_count;
  _storage_count = storage_count;
  _values.resize(storage_count);
  _storage.resize(storage_count * sample_count);
  for (std::size_t i = 0; i < storage_count; i++)
    _values[i] = _storage.data() + i * sample_count;
}

} // namespace svn::base
#endif // SVN_BASE_SHARED_CONT_STORAGE_HPP