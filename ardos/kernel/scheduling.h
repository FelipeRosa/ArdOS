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

#ifndef __ARDOS_KERNEL_SCHEDULING_H
#define __ARDOS_KERNEL_SCHEDULING_H

#include "types.h"
#include "wait_events.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Initializes the SCHEDULING module */
extern void ardos_kernel_scheduling_init();

/* Tells the scheduler to start scheduling
 * a given process */
extern void ardos_kernel_schedule(pid_t pid);
extern void ardos_kernel_reschedule(pid_t pid);

/* Puts some process to wait for
 * an specific event */
extern void ardos_kernel_put_onwait(pid_t pid, const struct wait_event_t *we);

/* Updates the scheduler READY queue
 * (i.e. Search for a process that isn't
 * in the READY state and remove it) */
extern void ardos_kernel_update_readyqueue();

/* Updates the scheduler WAIT queue
 * (i.e. Search for a process that isn't
 * in the WAIT state and remove it) */
extern void ardos_kernel_update_waitqueue();

/* Wakes up any process that is waiting
 * for the given process to terminate its execution */
extern void ardos_kernel_wakeup_joined(pid_t pid);

/* Makes a process to relinquish the CPU */
extern void ardos_kernel_process_yield();

/* Gets the PID of the caller process */
extern pid_t ardos_kernel_process_pid();

#ifdef __cplusplus
}
#endif
#endif /* __ARDOS_KERNEL_SCHEDULING_H */

