#include "core/input.h"
#include "core/event.h"
#include "core/kmemory.h"
#include "core/logger.h"

// default state of keys (KeyDown = FALSE, KeyUp = TRUE)
typedef struct keyboard_state {
    b8 keys[256];
} keyboard_state;

typedef struct mouse_state {
    i16 x;
    i16 y;
    u8 buttons[BUTTON_MAX_BUTTONS];
} mouse_state;

// since we need to keep track of both current and previous state
// we create a single structure to hold these states instead of making
// four individual global (static) states

typedef struct input_state {
    keyboard_state keyaboard_current;
    keyboard_state keyaboard_previous;
    mouse_state mouse_current;
    mouse_state mouse_previous;
} input_state;

static b8 initialized = FALSE;
static input_state state = {};

void input_initialize() {
    // we dont need this because static memory is zeroed-out but we are
    // bing pedantic!!!
    kzero_memory(&state, sizeof(state));
    initialized = TRUE;
    KINFO("Input subsystem initialized.");
}

void input_shutdown() {
    // TODO: add shutdown routines if needed
    // for when we add game controllers
    initialized = FALSE;
}

void input_update(f64 delta_time) {
    if (!initialized) {
        return;
    }

    // moving the current state to the previous state
    kcopy_memory(&state.keyaboard_previous, &state.keyaboard_current, sizeof(keyboard_state));
    kcopy_memory(&state.mouse_previous, &state.mouse_current, sizeof(mouse_state));
}

void input_process_key(keys key, b8 pressed) {
    // check if we have a state change for this particular key
    if (state.keyaboard_current.keys[key] != pressed) {
        // update the internal state of the keyboard
        state.keyaboard_current.keys[key] = pressed;

        // fire up an event
        event_context payload;
        payload.data.u16[0] = key;  // i dont understand why travis put this in a u16 instead of u8!
        event_fire(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED, 0, payload);
    }
}

void input_process_button(buttons button, b8 pressed) {
    // similar to input_process_key() function
    if (state.mouse_current.buttons[button] != pressed) {
        state.mouse_current.buttons[button] = pressed;

        event_context payload;
        payload.data.u16[0] = button;
        event_fire(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED, 0, payload);
    }
}

void input_process_mouse_move(i16 x, i16 y) {
    if (state.mouse_current.x != x || state.mouse_current.y != y) {
        // NOTE: enable this if debugging
        // KDEBUG("mouse pos: %i, %i", x, y);

        state.mouse_current.x = x;
        state.mouse_current.y = y;

        event_context payload;
        payload.data.u16[0] = x;
        payload.data.u16[1] = y;
        event_fire(EVENT_CODE_MOUSE_MOVED, 0, payload);
    }
}

void input_process_mouse_wheel(i8 z_delta) {
    // NOTE: no internal state update

    event_context payload;
    payload.data.u8[0] = z_delta;
    event_fire(EVENT_CODE_MOUSE_WHEEL, 0, payload);
}

b8 input_is_key_down(keys key) {
    if (!initialized) {
        return FALSE;
    }
    return state.keyaboard_current.keys[key] == TRUE;
}

b8 input_is_key_up(keys key) {
    if (!initialized) {
        return TRUE;
    }
    return state.keyaboard_current.keys[key] == FALSE;
}

b8 input_was_key_down(keys key) {
    if (!initialized) {
        return FALSE;
    }
    return state.keyaboard_previous.keys[key] == TRUE;
}
b8 input_was_key_up(keys key) {
    if (!initialized) {
        return TRUE;
    }
    return state.keyaboard_previous.keys[key] == FALSE;
}

b8 input_is_button_down(buttons button) {
    if (!initialized) {
        return FALSE;
    }
    return state.mouse_current.buttons[button] == TRUE;
}
b8 input_is_button_up(buttons button) {
    if (!initialized) {
        return TRUE;
    }
    return state.mouse_current.buttons[button] == FALSE;
}
b8 input_was_button_down(buttons button) {
    if (!initialized) {
        return FALSE;
    }
    return state.mouse_previous.buttons[button] == TRUE;
}
b8 input_was_button_up(buttons button) {
    if (!initialized) {
        return TRUE;
    }
    return state.mouse_previous.buttons[button] == FALSE;
}

void input_get_mouse_position(i32 *x, i32 *y) {
    if (!initialized) {
        *x = 0;
        *y = 0;
        return;
    }

    *x = state.mouse_current.x;
    *y = state.mouse_current.y;
}

void input_get_previous_mouse_position(i32 *x, i32 *y) {
    if (!initialized) {
        *x = 0;
        *y = 0;
        return;
    }

    *x = state.mouse_previous.x;
    *y = state.mouse_previous.y;
}