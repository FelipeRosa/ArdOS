
#include "../hardware/critical.h"

#include "timer.h"

#include <avr/io.h>
#include <avr/interrupt.h>


#define START_TIMER0()                   \
    TCCR0A = 0;                          \
    TCCR0B = (1 << WGM02) | (3 << CS00); \
    OCR0A = 250;                         \
    TIMSK0 = 1 << OCIE0A;

/* Implementação da interface */
static time_t millis = 0;

void ardos_kernel_timer_init()
{
    millis = 0;
    
    /* Inicia o contador */
    START_TIMER0();
}

time_t ardos_kernel_millis()
{
    /* Seção crítica */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Retorna o contador */
    return millis;
}

/* Interrupção */
ISR(TIMER0_COMPA_vect)
{
    millis++;
}
