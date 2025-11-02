// update_flags.hpp
#pragma once
#include <stdint.h>

// UpdateFlags serves as a human-readable way to mark what needs to be updated if the program detects a user input
enum UpdateFlags : uint8_t {
  None    = 0,
  Stop    = 1 << 0,
  Render  = 1 << 1,
  Zoom    = 1 << 2,
  Resize  = 1 << 3,
  Orbit   = 1 << 4
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
