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

#ifndef __ARDOS_API_ARDOS_H
#define __ARDOS_API_ARDOS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Initializes the operating system and
 * starts the main thread */
extern void ardos_init(void (*main_thread)());

#ifdef __cplusplus
}
#endif
#endif /* __ARDOS_API_ARDOS_H */
