

#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>

#include "ardos/kernel/process_management.h"
#include "ardos/kernel/scheduling.h"
#include "ardos/kernel/timer.h"


void f2();

void f1()
{
    struct wait_event_t we;

    DDRD = 0x80;        
    PORTD = 0x0;
    
    ardos_kernel_create_process(f2);
    we.code = ARDOS_JOIN_WAIT_EVENT;
    we.e_info.join.joined_pid = 1;
    ardos_kernel_put_onwait(0, &we);
    ardos_kernel_process_yield();
    
    for (; ; )
    {
        _delay_ms(100);
        PORTD ^= 0x80;
    }
}

void f2()
{
    DDRB = 1;
    PORTB = 0;
    
    _delay_ms(1000);
    PORTB ^= 1;
    
    ardos_kernel_destroy_process(1);
    ardos_kernel_process_yield();
}

int main()
{
    ardos_kernel_process_management_init();
    ardos_kernel_scheduling_init();
    ardos_kernel_timer_init();
    
    ardos_kernel_create_process(f1);
    sei();
    
    for (; ; ) ;

    return 0;
}
