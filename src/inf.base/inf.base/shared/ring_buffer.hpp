#ifndef SVN_BASE_SHARED_RING_BUFFER_HPP
#define SVN_BASE_SHARED_RING_BUFFER_HPP

#include <memory>
#include <cstring>
#include <cassert>

namespace inf::base {

// Note, buffer is not zero-initialized by 
// default, call clear() if you need this.
template <class T>
class ring_buffer
{
  std::size_t _size;
  std::size_t _head;
  std::unique_ptr<T[]> _buffer;

public:
  void clear();
  void push(T val);
  T get(size_t pos) const;

public:
  ring_buffer() = default;
  ~ring_buffer() = default;
  ring_buffer& operator=(ring_buffer&&) = default;
  ring_buffer(std::size_t size);
};

template <class T> inline void 
ring_buffer<T>::push(T val) 
{ 
  assert(_buffer.get() != nullptr);
  _buffer[_head++] = val; 
  _head %= _size; 
}

template <class T> inline void 
ring_buffer<T>::clear() 
{ 
  assert(_buffer.get() != nullptr); 
  std::memset(_buffer.get(), 0, _size * sizeof(T)); 
}

template <class T> inline T 
ring_buffer<T>::get(size_t pos) const 
{
  assert(_buffer.get() != nullptr); 
  assert(pos < _size); 
  return _buffer[(_head + _size - pos - 1) % _size]; 
}

template <class T>
inline ring_buffer<T>::ring_buffer(std::size_t size): 
_size(size), _head(0), _buffer(std::unique_ptr<T[]>(new T[size])) 
{ assert(size > 0); }

} // namespace inf::base
#endif // SVN_BASE_SHARED_RING_BUFFER_HPP