
#ifndef __ARDOS_HARDWARE_CRITICAL_H
#define __ARDOS_HARDWARE_CRITICAL_H

#include <avr/interrupt.h>


/* Entra em uma seção crítica.
 * Isto é feito desabilitando as interrupções
 * por causa das limitações do hardware
 */
#define ARDOS_ENTER_CRITICAL_SECTION() cli()

#endif /* __ARDOS_HARDWARE_CRITICAL_H */
