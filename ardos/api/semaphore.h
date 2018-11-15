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

#ifndef __ARDOS_API_SEMAPHORE_H
#define __ARDOS_API_SEMAPHORE_H

#include "../kernel/types.h"
#include "../config.h"

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
#endif /* __ARDOS_API_SEMAPHORE_H */
