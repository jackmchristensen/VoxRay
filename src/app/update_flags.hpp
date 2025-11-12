// update_flags.hpp
#pragma once
#include <stdint.h>

// UpdateFlags serves as a human-readable way to mark what needs to be updated if the program detects a user input
enum UpdateFlags : uint8_t {
  NONE    = 0,
  STOP    = 1 << 0,
  RENDER  = 1 << 1,
  ZOOM    = 1 << 2,
  RESIZE  = 1 << 3,
  ORBIT   = 1 << 4
};

inline UpdateFlags operator|(UpdateFlags a, UpdateFlags b) {
  return static_cast<UpdateFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline UpdateFlags operator&(UpdateFlags a, UpdateFlags b) {
  return static_cast<UpdateFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}

inline UpdateFlags& operator|=(UpdateFlags& a, UpdateFlags b) {
  a = a | b;
  return a;
}

inline UpdateFlags& operator&=(UpdateFlags& a, UpdateFlags b) {
  a = a & b;
  return a;
}

inline UpdateFlags operator~(UpdateFlags a) {
  return static_cast<UpdateFlags>(~static_cast<uint8_t>(a));
}
