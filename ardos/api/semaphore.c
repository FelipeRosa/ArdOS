

#include "../hardware/critical.h"

#include "process.h"
#include "semaphore.h"


static void semaphore_queue_enqueue(ardos_semaphore_t *sem, pid_t pid)
{
    if (sem->queue_size < ARDOS_SEMAPHORE_QUEUE_SIZE)
    {
        sem->queue[sem->queue_size++] = pid;
    }
}

static pid_t semaphore_queue_dequeue(ardos_semaphore_t *sem)
{
    if (sem->queue_size > 0)
    {
        pid_t pid = sem->queue[0];
        uint8_t i;
        
        for (i = 1; i < sem->queue_size; i++)
        {
            sem->queue[i - 1] = sem->queue[i];
        }
    
        sem->queue_size--;
        return pid;
    }
    
    return -1;
}

/* Interface implementation */
void ardos_semaphore_init(ardos_semaphore_t *sem, int8_t count)
{
    sem->value = count;
    sem->queue_size = 0;
}

void ardos_semaphore_wait(ardos_semaphore_t *sem)
{
    /* Critical section */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Decrement the semaphore's value (only if there's resources available) */
    if (sem->value >= 0)
    {
        sem->value--;
    }
    /* Suspend the process if needed */
    if (sem->value == -1)
    {
        semaphore_queue_enqueue(sem, ardos_process_pid());
        ardos_process_suspend();
    }
}

void ardos_semaphore_signal(ardos_semaphore_t *sem)
{
    /* Critical section */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Increments the semaphore's value */
    sem->value++;
    /* If needed, wake up a waiting process */
    if (sem->value >= 0 && sem->queue_size > 0)
    {
        pid_t pid = semaphore_queue_dequeue(sem);
        ardos_process_resume(pid);
    }
    /* Restores interrupts */
    sei();
}
