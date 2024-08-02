#include "application.h"

#include "core/event.h"
#include "core/kmemory.h"
#include "core/logger.h"
#include "game_types.h"
#include "platform/platform.h"
#include "core/input.h"

typedef struct application_state {
    game* game_inst;
    b8 is_running;
    b8 is_suspended;  // we dont update the app in cases like window minimized
    platform_state platform;
    i16 width;
    i16 height;
    f64 last_time;

} application_state;

// static but local to this file variables
static b8 initialized = FALSE;
static application_state app_state;

// forward declaring event handlers
// event handlers for the application layer. attention that we can cretae multiple listeners and even assign it to one event code if we wnat to!
// but we cannot register the same listener more than once to a event code
b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context data);
b8 application_on_key_event(u16 code, void* sender, void* listener_inst, event_context data);

b8 application_create(game* game_inst) {
    if (initialized) {
        KERROR("application_create() called more than once.")
        return FALSE;
    }

    // update the application state with the user-created game_instance
    app_state.game_inst = game_inst;

    // initialize subsytems
    initialize_logging();
    input_initialize();

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

    if (!event_initialize()) {
        KFATAL("Event system initialization failed. Application cannot continue.");
        return FALSE;
    }

    // now that our event system is initialized by the platform has not been started, we register our events
    // but dont forget to unregister events
    event_register(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    event_register(EVENT_CODE_KEY_PRESSED, 0, application_on_key_event);
    event_register(EVENT_CODE_KEY_RELEASED, 0, application_on_key_event);

    // start the application
    if (!platform_startup(
            &app_state.platform,
            game_inst->app_config.name,
            game_inst->app_config.start_pos_x,
            game_inst->app_config.start_pos_y,
            game_inst->app_config.start_width,
            game_inst->app_config.start_height)) {
        return FALSE;
    }

    // intialize the game
    if (!app_state.game_inst->initialize(game_inst)) {
        KFATAL("Game initalize() failed. Shutting down engine.")
        return FALSE;
    }

    app_state.game_inst->on_resize(game_inst, app_state.width, app_state.height);

    initialized = TRUE;
    return TRUE;
}

// this function handles platform messaging and game loop
b8 application_run() {
    KINFO(get_memory_usage_str());

    while (app_state.is_running) {
        if (!platform_pump_messages(&app_state.platform)) {
            app_state.is_running = FALSE;
        }

        // check if we can update the viewport
        if (!app_state.is_suspended) {
            // call the user-defined game update function
            if (!app_state.game_inst->update(app_state.game_inst, (f32)0)) {
                KFATAL("Game update() failed. Shutting down engine.");
                app_state.is_running = FALSE;
                break;
            }

            if (!app_state.game_inst->render(app_state.game_inst, (f32)0)) {
                KFATAL("Game render() failed. Shutting down engine.");
                app_state.is_running = FALSE;
                break;
            }

            // NOTE: Input update/state copying should always be handled
            // after any input should be recorded; I.E. before this line.
            // As a safety, input is the last thing to be updated before
            // this frame ends.
            input_update(0);
        }
    }

    //
    app_state.is_running = FALSE;
    event_unregister(EVENT_CODE_APPLICATION_QUIT, 0, application_on_event);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0, application_on_key_event);
    event_unregister(EVENT_CODE_KEY_RELEASED, 0, application_on_key_event);
    event_shutdown();
    input_shutdown();
    platform_shutdown(&app_state.platform);

    return TRUE;
}

b8 application_on_event(u16 code, void* sender, void* listener_inst, event_context context) {
    switch (code) {
        case EVENT_CODE_APPLICATION_QUIT:
            KINFO("'EVENT_CODE_APPLICATION_QUIT' received. shutting down the engine.");
            app_state.is_running = FALSE;
            // return true if this message has been handled and we dont want it to propagate to other listeners any more!
            return TRUE;
    }

    // if the code received is not what we registered for, just the event system we are not interested
    return FALSE;
}
b8 application_on_key_event(u16 code, void* sender, void* listener_inst, event_context context) {
    if (code == EVENT_CODE_KEY_PRESSED) {
        u16 key_code = context.data.u16[0];
        if (key_code == KEY_ESCAPE) {
            // NOTE: Technically firing an event to itself, but there may be other listeners.
            event_context data = {};
            event_fire(EVENT_CODE_APPLICATION_QUIT, 0, data);

            // Block anything else from processing this.
            return TRUE;
        } else if (key_code == KEY_A) {
            // Example on checking for a key
            KDEBUG("Explicit - A key pressed!");
        } else {
            KDEBUG("'%c' key pressed in window.", key_code);
        }
    } else if (code == EVENT_CODE_KEY_RELEASED) {
        u16 key_code = context.data.u16[0];
        if (key_code == KEY_B) {
            // Example on checking for a key
            KDEBUG("Explicit - B key released!");
        } else {
            KDEBUG("'%c' key released in window.", key_code);
        }
    }
    return FALSE;
}