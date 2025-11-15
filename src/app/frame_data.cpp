#include "frame_data.hpp"

namespace frame {

FrameTimer makeFrameTimer() {
  FrameTimer timer{
    .perf_frequency = SDL_GetPerformanceFrequency(),
    .last_time = SDL_GetPerformanceCounter(),
    .frame_start_time = timer.last_time,
    .last_fps_update = timer.last_time,
    .frame_count = 0,
    .accumulated_time = 0.f
  };

  return timer;
}

void beginFrame(FrameTimer& timer) {
  timer.frame_start_time = SDL_GetPerformanceCounter();
}

// Updates frame rate data once per second
// Returns true if the data has been updated
// Returns false if the data has not been updated
bool endFrame(FrameTimer& timer, FrameData& data) {
  Uint64 current_time = SDL_GetPerformanceCounter();

  data.delta_time = (float)(current_time - timer.last_time) / timer.perf_frequency;
  timer.last_time = current_time;

  float frame_time = (float)(current_time - timer.frame_start_time) / timer.perf_frequency;

  timer.frame_count++;
  timer.accumulated_time += data.delta_time;

  float time_since_update = (float)(current_time - timer.last_fps_update) / timer.perf_frequency;
  if (time_since_update >= 1.f) {
    data.avg_fps = timer.frame_count / time_since_update;
    data.avg_frame_time = (timer.accumulated_time / timer.frame_count) * 1000.f;

    timer.frame_count = 0;
    timer.accumulated_time = 0.f;
    timer.last_fps_update = current_time;

    return true;
  }

  return false;
}

} // namespace frame
