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

#ifndef __ARDOS_CONFIG_H
#define __ARDOS_CONFIG_H


/* Kernel's stack top address */
#define ARDOS_CONFIG_KERNEL_STACK_TOP 0x8FF

/* Kernel's stack size */
#define ARDOS_CONFIG_KERNEL_STACK_SIZE 0x80


/* Number of processes that can
 * be allocated by the OS */
#define ARDOS_CONFIG_MAX_PROCESSES 4

/* Processes' stack top.
 * The system starts allocating
 * the stack for processes at this address. */
#define ARDOS_CONFIG_PROCESSES_STACK_TOP (ARDOS_CONFIG_KERNEL_STACK_TOP - ARDOS_CONFIG_KERNEL_STACK_SIZE)

/* Size of the stack allocated for each
 * of the processes */
#define ARDOS_CONFIG_PROCESS_STACK_SIZE 0x100


/* Size (in bytes) of the processes' hardware context */
#define ARDOS_CONFIG_HWCONTEXT_SIZE 0x23

#endif /* __ARDOS_CONFIG_H */
