
#ifndef __ARDOS_HARDWARE_CRITICAL_H
#define __ARDOS_HARDWARE_CRITICAL_H

#include <avr/interrupt.h>


/* Enters a critical section. This
 * is acomplished by disabling the
 * interrupts because of some hardware
 * limitations */
#define ARDOS_ENTER_CRITICAL_SECTION() cli()

#endif /* __ARDOS_HARDWARE_CRITICAL_H */
