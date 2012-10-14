
#include "../hardware/critical.h"

#include "timer.h"

#include <avr/io.h>
#include <avr/interrupt.h>


#define START_TIMER0()                   \
    TCCR0A = 0;                          \
    TCCR0B = (1 << WGM02) | (3 << CS00); \
    OCR0A = 250;                         \
    TIMSK0 = 1 << OCIE0A;

/* Interface implementation */
static time_t millis = 0;

void ardos_kernel_timer_init()
{
    millis = 0;
    /* Begins TIMER0 execution */
    START_TIMER0();
}

time_t ardos_kernel_millis()
{
    /* Critical section */
    ARDOS_ENTER_CRITICAL_SECTION();
    /* Returns the millisecond counter */
    return millis;
}

/* Interuption */
ISR(TIMER0_COMPA_vect)
{
    millis++;
}
