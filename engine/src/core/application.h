#pragma once 

#include "defines.h"

// forward declare game struct ro break circular dependency betweeb application.h and game_types.h
struct game;

typedef struct application_config
{
    // starting X position on the screen/monitor
    i16 start_pos_x;

    // starting Y position on the screen/monitor
    i16 start_pos_y;

    // starting height of the client are / view port / canvas
    i16 start_width;

    // starting height of the client are / view port / canvas
    i16 start_height;

    // application window title
    char *name;

} application_config;



KAPI b8 application_create(struct game *game_inst);


KAPI b8 application_run();