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

#ifndef __ARDOS_KERNEL_PROCESS_MANAGEMENT_H
#define __ARDOS_KERNEL_PROCESS_MANAGEMENT_H

#include "types.h"
#include "timer.h"
#include "wait_events.h"


typedef uint8_t process_state_t;

#define ARDOS_PROCESS_STATE_UNKNOWN 0
#define ARDOS_PROCESS_STATE_READY   1
#define ARDOS_PROCESS_STATE_DEAD    2
#define ARDOS_PROCESS_STATE_WAIT    3

/* Initializes the PROCESS MANAGEMENT module */
extern void ardos_kernel_process_management_init();

/* Gets the number of existing processes */
extern uint8_t ardos_kernel_process_count();

/* Creates a new process that execute the given thread.
 *
 * Returns the PID of the newly created or -1 in case
 * no entry in the process table is available */
extern pid_t ardos_kernel_create_process(void (*thread)());

/* Destroys a process.
 * i.e. Remove it from any scheduling queue and
 * from the process table */
extern void ardos_kernel_destroy_process(pid_t pid);

/* Gets the number of milliseconds passed since
 * the process' creation */
extern time_t ardos_kernel_process_millis(pid_t pid);

/* Sets the state of a process */
extern void ardos_kernel_set_process_state(pid_t pid, process_state_t state);

/* Gets the state of a process */
extern process_state_t ardos_kernel_get_process_state(pid_t pid);

/* Sets the stack pointer for a process */
extern void ardos_kernel_set_process_stack(pid_t pid, void *stack);

/* Gets the stack pointer of a process */
extern void *ardos_kernel_get_process_stack(pid_t pid);


/* Sets the "wait event" infomation for a process */
extern void ardos_kernel_set_process_waitevent(pid_t pid, const struct wait_event_t *we);

/* Gets the "wait event" information of a process */
extern struct wait_event_t *ardos_kernel_get_process_waitevent(pid_t pid);

#endif /* __ARDOS_KERNEL_PROCESS_MANAGEMENT_H */
