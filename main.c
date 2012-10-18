

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "ardos/ardos.h"


void f1()
{
    DDRD = 0x80;
    
    for (; ; )
    {
        PORTD ^= 0x80;
        ardos_process_sleep(100);
    }

    ardos_process_exit();
}

void f2()
{
    pid_t pid;
    uint8_t on = 1;
        
    pid = ardos_process_create(f1);
    ardos_enable_eint(ARDOS_EINT_0, ARDOS_EINT_RISING_EDGE);    
        
    for (; ; )
    {
        ardos_process_wait_eint(ARDOS_EINT_0);
        
        if (on)
        {
            ardos_process_suspend_other(pid);
        }
        else
        {
            ardos_process_resume(pid);
        }
        
        on = !on;
        ardos_process_sleep(200);
    }

    ardos_process_exit();
}

int main()
{
    ardos_init(f2);

    return 0;
}
