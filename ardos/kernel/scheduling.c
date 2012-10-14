
#include "../config.h"
#include "../hardware/context_switch.h"
#include "../hardware/critical.h"

#include "process_management.h"
#include "scheduling.h"

#include <avr/io.h>
#include <avr/interrupt.h>


/* TIMER1 macros */
#define START_TIMER1()                   \
    TCCR1A = 0;                          \
    TCCR1B = (1 << WGM12) | (1 << CS10); \
    OCR1A = 15000;                       \
    TIMSK1 = 1 << OCIE1A
    
#define STOP_TIMER1() \
    TCCR1B = 0
    

/* Scheduling queues (implemented as circular buffers) */
static struct scheduling_queue_t
{
    uint8_t head;                               /* Queue's head */
    uint8_t tail;                               /* Queue's tail */
    uint8_t size;                               /* Queue's size, i.e. the number of elements in the queue */
    pid_t entries[ARDOS_CONFIG_MAX_PROCESSES];  /* Queue's entries */
} ready_queue, wait_queue;

static void sched_queue_init(struct scheduling_queue_t *queue)
{
    queue->size = 0;
    queue->head = 0;
    queue->tail = 0;
}

static void sched_queue_enqueue(struct scheduling_queue_t *queue, pid_t pid)
{
    if (queue->size < ARDOS_CONFIG_MAX_PROCESSES)
    {
        queue->entries[queue->tail++] = pid;
        
        if (queue->tail >= ARDOS_CONFIG_MAX_PROCESSES)
        {
            queue->tail = 0;
        }
        
        queue->size++;
    }
}

static pid_t sched_queue_dequeue(struct scheduling_queue_t *queue)
{
    if (queue->size > 0)
    {
        pid_t pid = queue->entries[queue->head++];
        
        if (queue->head >= ARDOS_CONFIG_MAX_PROCESSES)
        {
            queue->head = 0;
        }
        
        queue->size--;
        return pid;
    }
    
    return -1;
}


/* Interface implementation */
static pid_t sched_epid = -1;    /* PID of the process being executed */


void ardos_kernel_scheduling_init()
{
    sched_queue_init(&ready_queue);
    sched_queue_init(&wait_queue);
    START_TIMER1();
}

void ardos_kernel_schedule(pid_t pid)
{
    /* Critical section */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Puts the process in the READY queue */
    ardos_kernel_set_process_state(pid, ARDOS_PROCESS_STATE_READY);
    sched_queue_enqueue(&ready_queue, pid);
}

void ardos_kernel_reschedule(pid_t pid)
{
    ardos_kernel_schedule(pid);
    ardos_kernel_update_waitqueue();
}

bool_t ardos_kernel_put_onwait(pid_t pid, const struct wait_event_t *we)
{
    /* Critical section */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* It's only possible to put a process to WAIT
     * if it's READY */
    if (ardos_kernel_get_process_state(pid) == ARDOS_PROCESS_STATE_READY)
    {
        /* Sets the wait event */
        ardos_kernel_set_process_waitevent(pid, we);
        /* Puts the process in the WAIT state and adds
         * it to the WAIT queue */
        ardos_kernel_set_process_state(pid, ARDOS_PROCESS_STATE_WAIT);
        sched_queue_enqueue(&wait_queue, pid);
        /* Updates the READY queue */
        ardos_kernel_update_readyqueue();
        
        return TRUE;
    }

    return FALSE;
}

void ardos_kernel_update_readyqueue()
{
    uint8_t i;
    uint8_t s = ready_queue.size;
    
    /* Critical section */
    ARDOS_ENTER_CRITICAL_SECTION();
    
    for (i = 0; i < s; i++)
    {
        pid_t pid = sched_queue_dequeue(&ready_queue);
        
        if (ardos_kernel_get_process_state(pid) == ARDOS_PROCESS_STATE_READY)
        {
            sched_queue_enqueue(&ready_queue, pid);
        }
    }
}

void ardos_kernel_update_waitqueue()
{
    uint8_t i;
    uint8_t s = wait_queue.size;
 
    /* Critical section */
    ARDOS_ENTER_CRITICAL_SECTION();
    
    for (i = 0; i < s; i++)
    {
        pid_t pid = sched_queue_dequeue(&wait_queue);
        
        if (ardos_kernel_get_process_state(pid) == ARDOS_PROCESS_STATE_WAIT)
        {
            sched_queue_enqueue(&wait_queue, pid);
        }
    }
}

void ardos_kernel_wakeup_joined(pid_t pid)
{
    uint8_t i;

    /* Critical section */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Searchs and wakes up any joined process */
    for (i = wait_queue.size; i > 0; i--)
    {
        pid_t wakeup_pid = sched_queue_dequeue(&wait_queue);
        struct wait_event_t *we = ardos_kernel_get_process_waitevent(wakeup_pid);
        
        if (we->code == ARDOS_JOIN_WAIT_EVENT)
        {
            if (we->e_info.join.joined_pid == pid)
            {
                ardos_kernel_schedule(wakeup_pid);
                continue;
            }
        }
        
        sched_queue_enqueue(&wait_queue, wakeup_pid);
    }
}

void ardos_kernel_process_yield()
{
    /* Critical section */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Stops the timer */
    STOP_TIMER1();
    /* Sets the timer count in a
     * state that is going to generate an interrupt */
    TCNT1 = 14999;
    START_TIMER1();
    sei(); 
}

pid_t ardos_kernel_process_pid()
{
    return sched_epid;
}


/* Interuption */
static void run_scheduling()
{
    uint8_t i;
    time_t tick = ardos_kernel_millis();

    volatile time_t t;

    /* Already in a critical section,
     * it's only needed to stop TIMER1 */
    STOP_TIMER1();
    
    /* Returns (if it's meant to) the executing process
     * to the READY queue */
    if (sched_epid != -1)
    {
        if (ardos_kernel_get_process_state(sched_epid) == ARDOS_PROCESS_STATE_READY)
        {
            ardos_kernel_schedule(sched_epid);
        }
    }
    
    /* Wakes up any process waiting for a TIME event */
    for (i = wait_queue.size; i > 0; i--)
    {
        pid_t pid = sched_queue_dequeue(&wait_queue);
        struct wait_event_t *we = ardos_kernel_get_process_waitevent(pid);
        
        if (we->code == ARDOS_TIME_WAIT_EVENT)
        {
            if (tick >= we->e_info.time.wakeup_tick)
            {
                ardos_kernel_schedule(pid);
                continue;
            }
        }
        
        sched_queue_enqueue(&wait_queue, pid);
    }
    
    /* Gets the next process' PID. */
    if (ready_queue.size > 0)
    {   
        sched_epid = sched_queue_dequeue(&ready_queue);
    }
    else
    { 
        /* Schedules the IDLE process */
        sched_epid = ARDOS_CONFIG_MAX_PROCESSES;
    }
       
    /* Restarts TIMER1 */
    START_TIMER1();
}

ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
    /* Temporarily stores the STACK POINTER */
    static volatile uint16_t sp_tmp;
 
    /* Save the executing process' hardware context */
    ARDOS_SAVE_HWCONTEXT();
    /* Store the executing process' SP */
    sp_tmp = SP;
    /* Set the Kernel's SP */
    SP = ARDOS_CONFIG_KERNEL_STACK_TOP;
    /* Saves the executing process' STACK POINTER */
    ardos_kernel_set_process_stack(sched_epid, (void *)sp_tmp);
    /* Runs the scheduler */
    run_scheduling();
    /* Restores and executes the next process */
    SP = (uint16_t)ardos_kernel_get_process_stack(sched_epid);
    ARDOS_RESTORE_HWCONTEXT();
    reti();
}
