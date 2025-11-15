// app/frame_data.hpp
#pragma once
#include <SDL3/SDL_timer.h>

namespace frame {

// Internal timing data
struct FrameTimer {
  Uint64 perf_frequency;
  Uint64 last_time;
  Uint64 frame_start_time;

  Uint64 last_fps_update;
  int frame_count;
  float accumulated_time;
};

// Public frame stats
struct FrameData {
  float delta_time;
  float avg_fps;
  float avg_frame_time;
};

FrameTimer makeFrameTimer();
void beginFrame(FrameTimer& timer);
bool endFrame(FrameTimer& timer, FrameData& data);

} // namespace frame
