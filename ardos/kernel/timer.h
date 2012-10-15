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
