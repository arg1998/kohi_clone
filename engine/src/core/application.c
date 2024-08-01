#include "application.h"
#include "core/logger.h"
#include "platform/platform.h"
#include "game_types.h"

typedef struct application_state {
    game *game_inst;
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

KAPI b8 application_create(game *game_inst) {
    if (initialized) {
        KERROR("application_create() called more than once.")
        return FALSE;
    }

    // update the application state with the user-created game_instance
    app_state.game_inst = game_inst;

    // initialize subsytems
    initialize_logging();

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;

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
    if(!app_state.game_inst->initialize(game_inst)){
        KFATAL("Game initalize() failed. Shutting down engine.")
        return FALSE;
    }


    app_state.game_inst->on_resize(game_inst, app_state.width, app_state.height);


    initialized = TRUE;
    return TRUE;
}

// this function handles platform messaging and game loop
KAPI b8 application_run() {
    while (app_state.is_running) {
        if (!platform_pump_messages(&app_state.platform)) {
            app_state.is_running = FALSE;
        }

        // check if we can update the viewport
        if(!app_state.is_suspended){
            // call the user-defined game update function
            if(!app_state.game_inst->update(app_state.game_inst, (f32)0)){
                KFATAL("Game update() failed. Shutting down engine.");
                app_state.is_running = FALSE;
                break;
            }

            if(!app_state.game_inst->render(app_state.game_inst, (f32)0)){
                KFATAL("Game render() failed. Shutting down engine.");
                app_state.is_running = FALSE;
                break;
            }
        }

    }

    //
    app_state.is_running = FALSE;

    platform_shutdown(&app_state.platform);

    return TRUE;
}