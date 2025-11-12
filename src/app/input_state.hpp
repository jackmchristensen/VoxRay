// input_state.hpp
#pragma once

struct InputState {
    // Mouse deltas
    float mouse_dx  = 0.f;
    float mouse_dy  = 0.f;
    float scroll_dx = 0.f;
    float scroll_dy = 0.f;

    // Mouse buttons
    bool lmb_held = false;
    bool rmb_held = false;
    bool mmb_held = false;

    // Modifier keys
    bool alt_held   = false;
    bool ctrl_held  = false;
    bool shift_held = false;
};
