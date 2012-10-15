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

#include "../config.h"
#include "../hardware/critical.h"

#include "scheduling.h"
#include "process_management.h"

#include <string.h>


/* Kernel's process table */
struct process_table_entry_t
{
    bool_t used;                        /* Indicates whether the entry is being used or not */
    pid_t pid;                          /* PID of the process stored in the entry */
    process_state_t state;              /* State of the process stored in the entry */

    time_t creation_tick;               /* Process' creation tick (1 tick = 1 millisecond) */
    
    struct wait_event_t wait_event;     /* If the process is in the WAIT state, 
                                         * this holds infomation about the wait event */
    
    void *stack;                        /* Stack pointer of the process stored in the entry.
                                         * The stack also holds the hardware context */
};

static struct process_table_t
{
    uint8_t process_count; /* Existing processes counter */
    struct process_table_entry_t entries[ARDOS_CONFIG_MAX_PROCESSES + 1]; /* Process table entries.
                                                                           * One of which is used to hold
                                                                           * information about the IDLE process */
} process_table;


/* IDLE Process */
void ardos_idle_process_thread() __attribute__((naked));
void ardos_idle_process_thread()
{

    for (; ; )
        sei();
}

static void configure_idle_process()
{
    uint8_t i;
    struct process_table_entry_t *idle;
    uint8_t *sp;

    /* Configures the IDLE process */
    idle = process_table.entries + ARDOS_CONFIG_MAX_PROCESSES;
    idle->used = TRUE;
    idle->pid = ARDOS_CONFIG_MAX_PROCESSES;
    idle->state = ARDOS_PROCESS_STATE_DEAD;
    idle->creation_tick = 0;
    
    /* IDLE process' stack */
    sp = (uint8_t *)(ARDOS_CONFIG_KERNEL_STACK_TOP - ARDOS_CONFIG_KERNEL_STACK_SIZE + ARDOS_CONFIG_HWCONTEXT_SIZE);
    idle->stack = (void *)(sp - ARDOS_CONFIG_HWCONTEXT_SIZE);
    
    /* Pushes the return address */   
    *sp-- = (uint8_t)(uint16_t)ardos_idle_process_thread;
    *sp-- = (uint8_t)(((uint16_t)ardos_idle_process_thread) >> 8);
    /* Pushes R0 */
    *sp-- = 0;
    /* Pushes SREG */
    *sp-- = 0x80;
    /* Pushes the remaining registers */
    for (i = 0; i < 31; i++)
    {
        *sp-- = 0;
    }
}


static void init_process_table()
{
    uint8_t i;
    
    /* Initialize */
    for (i = 0; i < ARDOS_CONFIG_MAX_PROCESSES; i++)
    {
        process_table.entries[i].used = FALSE;
        process_table.entries[i].state = ARDOS_PROCESS_STATE_UNKNOWN;
    }

    configure_idle_process();    
}

static void configure_process_table_entry(pid_t pid, void (*thread)())
{
    uint16_t thread_addr = (uint16_t)thread;
    uint8_t *sp;
    uint8_t i;

    /* Sets the entry as USED  */
    process_table.entries[pid].used = TRUE;
    /* Asserts the right PID */
    process_table.entries[pid].pid = pid;
    /* Sets the creation tick */
    process_table.entries[pid].creation_tick = ardos_kernel_millis();
    /* Puts the event in an UNKNOWN state */
    process_table.entries[pid].state = ARDOS_PROCESS_STATE_UNKNOWN;
    /* Sets the process' stack for the first execution */
    sp = (uint8_t *)(ARDOS_CONFIG_PROCESSES_STACK_TOP - (ARDOS_CONFIG_PROCESS_STACK_SIZE + ARDOS_CONFIG_HWCONTEXT_SIZE) * pid);
    process_table.entries[pid].stack = (void *)(sp - ARDOS_CONFIG_HWCONTEXT_SIZE);
 
    /* Pushes the return address */   
    *sp-- = (uint8_t)thread_addr;
    *sp-- = (uint8_t)(thread_addr >> 8);
    /* Pushes R0 */
    *sp-- = 0;
    /* Pushes SREG */
    *sp-- = 0x80;
    /* Pushes the remaining registers */
    for (i = 0; i < 31; i++)
    {
        *sp-- = 0;
    }
}


/* Interface implementation */
void ardos_kernel_process_management_init()
{
    /* Initializes the process table */
    process_table.process_count = 0;
    init_process_table();    
}

uint8_t ardos_kernel_process_count()
{
    return process_table.process_count;
}

pid_t ardos_kernel_create_process(void (*thread)())
{
    pid_t pid = -1;

    /* Critical section */
    ARDOS_ENTER_CRITICAL_SECTION();
    
    /* The process will only be created if there's an available
     * entry in the process table */
    if (process_table.process_count < ARDOS_CONFIG_MAX_PROCESSES)
    {
        /* Finds a available process table entry */
        for (pid = 0; pid < ARDOS_CONFIG_MAX_PROCESSES; pid++)
        {
            if (process_table.entries[pid].used == FALSE)
            {
                break;
            }
        }
        /* Configures the entry */
        configure_process_table_entry(pid, thread);
        /* Starts scheduling */
        ardos_kernel_schedule(pid);
        /* Counts +1 process */
        process_table.process_count++;
    }
    
    return pid;
}

void ardos_kernel_destroy_process(pid_t pid)
{
    struct process_table_entry_t *entry = process_table.entries + pid;
    process_state_t prev_state = entry->state;

    /* Critical section */
    ARDOS_ENTER_CRITICAL_SECTION();
    
    /* Sets the entry as NOT USED */
    entry->used = FALSE;
    /* Puts the process in the DEAD state */
    entry->state = ARDOS_PROCESS_STATE_DEAD;
    /* Decrements the process count */
    process_table.process_count--;
    /* Updates the scheduler queues (only the ones needed) */
    if (prev_state == ARDOS_PROCESS_STATE_WAIT)
    {
        ardos_kernel_update_waitqueue();
    }
    else if (prev_state == ARDOS_PROCESS_STATE_READY)
    {
        ardos_kernel_update_readyqueue();
    }
    /* Wakes up any joined processes */
    ardos_kernel_wakeup_joined(pid);
}

time_t ardos_kernel_process_millis(pid_t pid)
{
    /* Critical section */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Computes ticks */
    return (ardos_kernel_millis() - process_table.entries[pid].creation_tick);
}

void ardos_kernel_set_process_state(pid_t pid, process_state_t state)
{
    /* Critical section */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Sets the process' state */
    process_table.entries[pid].state = state;
}

process_state_t ardos_kernel_get_process_state(pid_t pid)
{
    return process_table.entries[pid].state;
}

void ardos_kernel_set_process_stack(pid_t pid, void *stack)
{
    /* Critical section */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Sets the stack pointer */
    if (pid != -1 && pid != ARDOS_CONFIG_MAX_PROCESSES)
    {
        process_table.entries[pid].stack = stack;
    }
}

void *ardos_kernel_get_process_stack(pid_t pid)
{
    /* Critical section */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Returns the pointer */
    return process_table.entries[pid].stack;
}

void ardos_kernel_set_process_waitevent(pid_t pid, const struct wait_event_t *we)
{
    struct process_table_entry_t *entry = process_table.entries + pid;

    /* Critical section */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Copies the information */
    memcpy((void *)(&entry->wait_event), (const void *)we, sizeof(struct wait_event_t));
}

struct wait_event_t *ardos_kernel_get_process_waitevent(pid_t pid)
{
    /* Critical section (pointer = 2 bytes) */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Returns */
    return &process_table.entries[pid].wait_event;   
}
