#include "game.h"

#include <core/logger.h>

b8 game_initialize(game *game_inst) {
    KDEBUG("game_initialize() called.");

    KFATAL("A test message: %f", 3.14f);
    KERROR("A test message: %f", 3.14f);
    KWARN("A test message: %f", 3.14f);
    KINFO("A test message: %f", 3.14f);
    KDEBUG("A test message: %f", 3.14f);
    KTRACE("A test message: %f", 3.14f);

    return TRUE;
}

b8 game_update(game *game_inst, f32 delta_time) {
    return TRUE;
}

b8 game_render(game *game_inst, f32 delta_time) {
    return TRUE;
}

b8 game_on_resize(game *game_inst, u32 width, u32 height) {
    KDEBUG("game_on_resize() called.")
    return TRUE;
}