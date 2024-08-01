#pragma once

#include "core/application.h"
#include "core/logger.h"
#include "game_types.h"

// externally defined function for the application.
// user must provide a declarion for this function definition
extern b8 create_game(game *game_out);

/**
 * The main entry point of the application
 */
int main(void) {
    // request the game instance from the application
    game game_inst;
    if (!create_game(&game_inst)) {
        KFATAL("Could not create game!");
        return -1;
    }

    // check if the user has provided a declarion for the function pointers inside the game instance
    if (!game_inst.initialize || !game_inst.on_resize || !game_inst.render || !game_inst.update) {
        KFATAL("All function pointers inside the game object must have a proper declaration by the user.")
        return -2;
    }

    if (!application_create(&game_inst)) {
        KFATAL("Application faield to create.")
        return 1;
    }

    // Begin game loop
    if( !application_run()){
        KINFO("Application did not shutdown gracefully.");
        return 2;
    }



    return 0;
}
