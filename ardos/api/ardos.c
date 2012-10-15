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

#include "../kernel/process_management.h"
#include "../kernel/scheduling.h"
#include "../kernel/timer.h"

#include "ardos.h"

#include <avr/interrupt.h>


void ardos_init(void (*main_thread)())
{
    /* Init kernel modules */
    ardos_kernel_process_management_init();
    ardos_kernel_scheduling_init();
    ardos_kernel_timer_init();
    /* Creates the main process */
    ardos_kernel_create_process(main_thread);
    /* Starts the operations and waits for the scheduler */
    sei();
    for (; ; ) ;
}
