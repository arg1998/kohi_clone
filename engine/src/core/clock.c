#include "core/clock.h"
#include "platform/platform.h"

void clock_update(clock *clock) {
    if (clock->statrt_time != 0) {
        clock->elapsed = platform_get_absolute_time() - clock->statrt_time;
    }
}

void clock_start(clock *clock) {
    clock->statrt_time = platform_get_absolute_time();
    clock->elapsed = 0;
}

void clock_stop(clock *clock) {
    clock->statrt_time = 0;
}
