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
