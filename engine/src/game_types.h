// this is the interface to the engine.
#pragma once

#include "core/application.h"

typedef struct game {
    // application configuration
    application_config app_config;

    // function pointer to game's initialize function
    b8 (*initialize)(struct game *game_inst);

    b8 (*update)(struct game *game_inst, f32 delta_time);

    b8 (*render)(struct game *game_inst, f32 delta_time);

    b8 (*on_resize)(struct game *game_inst, u32 width, u32 height);

    // Game specific state managed by the game.
    void *state;

} game;
