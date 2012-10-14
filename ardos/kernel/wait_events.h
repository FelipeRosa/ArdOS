
#ifndef __ARDOS_KERNEL_WAIT_EVENT_H
#define __ARDOS_KERNEL_WAIT_EVENT_H

#include "timer.h"
#include "types.h"

#include <stdint.h>


typedef uint8_t wevent_code_t;
#define ARDOS_TIME_WAIT_EVENT      1
#define ARDOS_JOIN_WAIT_EVENT      2
#define ARDOS_UNDEFINED_WAIT_EVENT 255


/* Time wait event */
struct time_wait_event_t
{
    time_t wakeup_tick;
};


/* Process wait event (join) */
struct join_wait_event_t
{
    pid_t joined_pid;
};


/* Union of wait event structs */
union wait_event_info_t
{
    struct time_wait_event_t time;
    struct join_wait_event_t join;
};


/* Struct to hold an event's code
 * and information */
struct wait_event_t
{
    wevent_code_t code;
    union wait_event_info_t e_info;
};

#endif /* __ARDOS_KERNEL_WAIT_EVENT_H */
