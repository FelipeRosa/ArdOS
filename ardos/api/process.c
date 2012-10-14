
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
    
    /* Puts the process to wait for no specific event */
    we.code = ARDOS_UNDEFINED_WAIT_EVENT;
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
