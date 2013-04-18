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

#ifndef __ARDOS_API_PROCESS_H
#define __ARDOS_API_PROCESS_H

#include "../kernel/types.h"
#include "../kernel/timer.h"

#include "external_interrupts.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Creates a process that executes the given thread.
 * Returns -1 in case the process could not be created */
extern pid_t ardos_process_create(void (*thread)());

/* Kills the process with the given PID */
extern void ardos_process_kill(pid_t pid);

/* Terminates (properly) the execution of a process */
extern void ardos_process_exit();

/* Gets the PID of the current executing process */
extern pid_t ardos_process_pid();

/* Puts a process to sleep for a given
 * amount of time */
extern void ardos_process_sleep(time_t ms);

/* Suspends the execution of the caller process */
extern void ardos_process_suspend();

/* Supends the execution of another process */
extern void ardos_process_suspend_other(pid_t pid);

/* Resumes the execution of a suspended process */
extern void ardos_process_resume(pid_t pid);

/* Waits for a process to terminate its execution */
extern void ardos_process_join(pid_t pid);

/* Gets the number of milliseconds since the
 * process started */
extern time_t ardos_process_millis();

/* Give up the CPU for another process to execute */
extern void ardos_process_yield();

/* Waits for an external interrupt */
extern void ardos_process_wait_eint(ardos_eint_t int_num);

#ifdef __cplusplus
}
#endif
#endif /* __ARDOS_API_PROCESS_H */
