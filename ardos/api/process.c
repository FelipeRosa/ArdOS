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

#include "../kernel/process_management.h"
#include "../kernel/scheduling.h"

#include "process.h"

#include <avr/interrupt.h>


/* Interface implementation */
pid_t ardos_process_create(void (*thread)())
{
    /* Creates the process (critical section) */
    pid_t pid = ardos_kernel_create_process(thread);
    /* Restore interrupts */
    sei();
    return pid;
}

void ardos_process_kill(pid_t pid)
{
    /* Destroys the process (critical section) */
    ardos_kernel_destroy_process(pid);
    /* Restore interrupts */
    sei();
}

void ardos_process_exit()
{
    /* Destroy this process */
    ardos_kernel_destroy_process(ardos_process_pid());
    /* Yields */
    ardos_kernel_process_yield();
}

pid_t ardos_process_pid()
{
    /* Gets the PID (not critical) */
    return ardos_kernel_process_pid();
}

void ardos_process_sleep(time_t ms)
{
    struct wait_event_t we;

    /* Puts process to sleep and yields */
    we.code = ARDOS_TIME_WAIT_EVENT;
    we.e_info.time.wakeup_tick = ardos_kernel_millis() + ms;
    ardos_kernel_put_onwait(ardos_process_pid(), &we);
    /* Yields */
    ardos_kernel_process_yield();
}

void ardos_process_suspend()
{
    struct wait_event_t we;

    /* Puts this process to wait for no specific event (critical) */
    we.code = ARDOS_UNDEFINED_WAIT_EVENT;
    ardos_kernel_put_onwait(ardos_process_pid(), &we);
    /* Yields */
    ardos_kernel_process_yield();
}

void ardos_process_suspend_other(pid_t pid)
{
    struct wait_event_t we;

    /* Puts the process to wait for no specific event (critical) */
    we.code = ARDOS_UNDEFINED_WAIT_EVENT;
    ardos_kernel_put_onwait(pid, &we);
    /* Restores interrupts */
    sei();
}

void ardos_process_resume(pid_t pid)
{
    /* The process must be WAITING */
    if (ardos_kernel_get_process_state(pid) == ARDOS_PROCESS_STATE_WAIT)
    {
        /* Resumes */
        ardos_kernel_reschedule(pid);
    }
    /* Restores the interrupts */
    sei();
}

void ardos_process_join(pid_t pid)
{
    struct wait_event_t we;

    /* Puts the process to wait for no specific event (critical) */
    we.code = ARDOS_JOIN_WAIT_EVENT;
    we.e_info.join.joined_pid = pid;
    ardos_kernel_put_onwait(ardos_process_pid(), &we);
    /* Yields */
    ardos_kernel_process_yield();
}

time_t ardos_process_millis()
{

    /* Gets the ticks (critical) */
    time_t ticks = ardos_kernel_process_millis(ardos_process_pid());
    /* Restores the interrupts */
    sei();
    /* Returns the ticks */
    return ticks;
}

void ardos_process_yield()
{
    ardos_kernel_process_yield();
}


void ardos_process_wait_eint(ardos_eint_t int_num)
{
   struct wait_event_t we;

    /* Puts the process to wait for no specific event (critical) */
    we.code = ARDOS_INTERRUPT_WAIT_EVENT;
    we.e_info.interrupt.interrupt_num = int_num;
    ardos_kernel_put_onwait(ardos_process_pid(), &we);
    /* Yields */
    ardos_kernel_process_yield();
}
