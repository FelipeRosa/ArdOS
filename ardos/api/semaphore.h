
#ifndef __ARDOS_API_SEMAPHORE_H
#define __ARDOS_API_SEMAPHORE_H

#include "../kernel/types.h"
#include "../config.h"


/* Structure of a semaphore */
#define ARDOS_SEMAPHORE_QUEUE_SIZE ARDOS_CONFIG_MAX_PROCESSES

typedef struct
{
    int8_t value;                              /* Semaphore's value */
    
    uint8_t queue_size;                         /* Waiting processes queue's size */
    pid_t queue[ARDOS_SEMAPHORE_QUEUE_SIZE];    /* Waiting processes queue */
} ardos_semaphore_t;


/* Initializes a semaphore with a given count */
extern void ardos_semaphore_init(ardos_semaphore_t *sem, int8_t count);

/* Locks a semaphore */
extern void ardos_semaphore_wait(ardos_semaphore_t *sem);

/* Unlocks a semaphore */
extern void ardos_semaphore_signal(ardos_semaphore_t *sem);

#endif /* __ARDOS_API_SEMAPHORE_H */
