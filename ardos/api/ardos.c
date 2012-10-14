
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
