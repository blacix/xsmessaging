#ifndef _RING_BUFFER_H
#define _RING_BUFFER_H

#include <array>
#include <cstdint>

namespace xsm {

// template class that implements a ring buffer
template <size_t N>
class RingBufferT {
public:
  RingBufferT() {
    mSize = N;
    mHead = mTail = 0;
  }

  ~RingBufferT() {}

  // returns the number of bytes in the buffer
  size_t capacity() const {
    if (mHead > mTail) {
      return mSize - (mHead - mTail);
    } else {
      return mTail - mHead;
    }
  }

  // returns the nuber of available bytes in the buffer
  size_t available() const {
    return mSize - capacity();
  }

  // clears the buffer
  void clear() {
    mHead = mTail = 0;
  }

  // adds 1 byte to the buffer.
  // returns true on success, false otherwise
  bool push(uint8_t v) {
    size_t tail = (mTail + 1) % mSize;
    if (tail == mHead) {
      // full
      return false;
    } else {
      mData[mTail] = v;
      mTail = tail;
      return true;
    }
  }

  // gets the last inserted byte from the buffer.
  // returns true on success, false otherwise
  bool pop(uint8_t& v) {
    if (mHead == mTail) {
      // empty
      return false;
    } else {
      v = mData[mHead];
      mHead = (mHead + 1) % mSize;
      return true;
    }
  }

  // removes the last inserted byte from the buffer.
  // returns true on success, false otherwise
  bool pop() {
    if (mHead != mTail) {
      mHead = (mHead + 1) % mSize;
      return true;
    }
    return false;
  }

  // removes the last "length" inserted byte from the buffer.
  // returns true on success, false otherwise
  bool pop(size_t length) {
    if (capacity() >= length) {
      mHead = (mHead + length) % mSize;
      return true;
    }
    return false;
  }

  // gets the element at index from the buffer.
  // returns true on success, false otherwise
  bool get(size_t index, uint8_t& v) const {
    v = mData[(mHead + index) % mSize];
    return capacity() > 0;
  }

  // gets "length" elements from the buffer starting from "index"
  // into buffer
  // returns true on success, false otherwise
  bool get(size_t index, uint8_t* buffer, size_t length) const {
    if (capacity() >= length) {
      while (length--) {
        get(index++, *buffer++);
      }
      return true;
    }
    return false;
  }


private:
  // standard array for the buffer
  std::array<uint8_t, N> mData;
  // size of the buffer
  size_t mSize;
  // head index
  size_t mHead;
  // tail index
  size_t mTail;
};

} // namespace xsm
#endif
