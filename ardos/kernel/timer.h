
#ifndef __ARDOS_KERNEL_TIMER_H
#define __ARDOS_KERNEL_TIMER_H

#include <stdint.h>


/* Type to hold time variables */
typedef uint32_t time_t;


/* Initializes the TIMER module */
extern void ardos_kernel_timer_init();

/* Gets the number of milliseconds
 * passed since the timer began executing */
extern time_t ardos_kernel_millis();

#endif /* __ARDOS_KERNEL_TIMER_H */
