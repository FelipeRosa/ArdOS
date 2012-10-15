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

#ifndef __ARDOS_HARDWARE_CRITICAL_H
#define __ARDOS_HARDWARE_CRITICAL_H

#include <avr/interrupt.h>


/* Enters a critical section. This
 * is acomplished by disabling the
 * interrupts because of some hardware
 * limitations */
#define ARDOS_ENTER_CRITICAL_SECTION() cli()

#endif /* __ARDOS_HARDWARE_CRITICAL_H */
