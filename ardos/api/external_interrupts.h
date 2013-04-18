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

#ifndef __ARDOS_API_EXTERNAL_INTERRUPTS
#define __ARDOS_API_EXTERNAL_INTERRUPTS

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* External interrupts number type */
typedef uint8_t ardos_eint_t;

/* External interrupts trigger type */
typedef uint8_t ardos_eint_trigger_t;

/* External interrupt numbers */
#define ARDOS_EINT_0 0
#define ARDOS_EINT_1 1

/* External interrupt trigger modes */
#define ARDOS_EINT_LOW_LEVEL    0
#define ARDOS_EINT_ANY_LEVEL    1
#define ARDOS_EINT_RISING_EDGE  2
#define ARDOS_EINT_FALLING_EDGE 3

/* Enables one of the available external interrupts and set it to be triggered
 * when the given event happens */
extern void ardos_enable_eint(ardos_eint_t int_num, ardos_eint_trigger_t trigger);

/* Disables one of the available external interrupts */
extern void ardos_disable_eint(ardos_eint_t int_num);

#ifdef __cplusplus
}
#endif

#endif /* __ARDOS_API_EXTERNAL_INTERRUPTS */
