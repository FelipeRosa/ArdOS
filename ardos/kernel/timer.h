
#ifndef __ARDOS_KERNEL_TIMER_H
#define __ARDOS_KERNEL_TIMER_H

#include <stdint.h>


typedef uint32_t time_t;


/* Inicializa o módulo do TIMER */
extern void ardos_kernel_timer_init();

/* Obtém o número de milisegundos
 * passados desde de o início do
 * timer
 */
extern time_t ardos_kernel_millis();


#endif /* __ARDOS_KERNEL_TIMER_H */
