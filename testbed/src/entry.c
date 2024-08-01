#include <entry.h>
#include <platform/platform.h> // TODO: remove this 

#include "game.h"

// declaration of the external function to create a game
b8 create_game(game *out_game) {
    out_game->app_config.name = "ARGOSTA Engine";
    out_game->app_config.start_pos_x = 100;
    out_game->app_config.start_pos_y = 100;
    out_game->app_config.start_width = 1280;
    out_game->app_config.start_height = 720;

    // override the function pointers inside the game_instance requested by the engine
    // note that these functions do not have to have the same name as long as they have the dame definition
    out_game->initialize = game_initialize;
    out_game->update = game_update;
    out_game->render = game_render;
    out_game->on_resize = game_on_resize;

    // create the game state
    out_game->state = platform_allocate(sizeof(game_state), FALSE);

    return TRUE;    
}
