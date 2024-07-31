#pragma once

#include "defines.h"

// this structure will hold the platform state inside
// it has a void* type becuase we don't know what type of platform we are puting into
typedef struct platform_state
{
    void *internal_state;
} platform_state;

KAPI b8 platform_startup(
    platform_state *plat_state,
    const char *application_name, // window title on desktop environment
    i32 x, i32 y,                 // position of the window
    i32 width, i32 height         // width and height of the window on the desktop environemnt
);

KAPI void platform_shutdown(platform_state *plat_state);

// continuosly loop over messages and channel them to the application
KAPI b8 platform_pump_messages(platform_state *plat_state);

void *platform_allocate(u64 size, b8 aligned);
void platform_free(void *block, b8 aligned);
void *platform_zero_memory(void *block, u64 size);
void *platform_copy_memory(void *dest, const void *source, u64 size);
void *platform_set_memory(void *dest, i32 value, u64 size);


void platform_console_write(const char *message, u8 color); 
void platform_console_write_error(const char *message, u8 color); 


f64 platform_get_absolute_time();

 
void platform_sleep(u64 ms);