
#ifndef __ARDOS_KERNEL_SCHEDULING_H
#define __ARDOS_KERNEL_SCHEDULING_H

#include "types.h"
#include "wait_events.h"


/* Initializes the SCHEDULING module */
extern void ardos_kernel_scheduling_init();

/* Tells the scheduler to start scheduling
 * a given process */
extern void ardos_kernel_schedule(pid_t pid);
extern void ardos_kernel_reschedule(pid_t pid);

/* Puts some process to wait for
 * an specific event */
extern bool_t ardos_kernel_put_onwait(pid_t pid, const struct wait_event_t *we);

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

#endif /* __ARDOS_KERNEL_SCHEDULING_H */

