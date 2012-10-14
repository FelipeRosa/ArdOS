
#ifndef __ARDOS_API_PROCESS_H
#define __ARDOS_API_PROCESS_H

#include "../kernel/types.h"
#include "../kernel/timer.h"


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

/* Suspends the execution of the given process */
extern void ardos_process_suspend();

/* Gets the number of milliseconds since the
 * process started */
extern time_t ardos_process_millis();

#endif /* __ARDOS_API_PROCESS_H */
