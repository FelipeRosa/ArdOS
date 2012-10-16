/*
ArdOS Copyright (C) 2012 Felipe Soares Gonçalves Rosa

ArdOS is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ArdOS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __ARDOS_KERNEL_WAIT_EVENT_H
#define __ARDOS_KERNEL_WAIT_EVENT_H

#include "timer.h"
#include "types.h"

#include <stdint.h>


typedef uint8_t wevent_code_t;
#define ARDOS_TIME_WAIT_EVENT      1
#define ARDOS_JOIN_WAIT_EVENT      2
#define ARDOS_INTERRUPT_WAIT_EVENT 3
#define ARDOS_UNDEFINED_WAIT_EVENT 255


/* Wait events' structures */
struct time_wait_event_t
{
    time_t wakeup_tick;
};

struct join_wait_event_t
{
    pid_t joined_pid;
};

struct interrupt_wait_event_t
{
    uint8_t interrupt_num;
};


/* Union of wait event structs */
union wait_event_info_t
{
    struct time_wait_event_t time;
    struct join_wait_event_t join;
    struct interrupt_wait_event_t interrupt;
};


/* Struct to hold an event's code
 * and information */
struct wait_event_t
{
    wevent_code_t code;
    union wait_event_info_t e_info;
};

#endif /* __ARDOS_KERNEL_WAIT_EVENT_H */
