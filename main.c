

#include <avr/io.h>
#include <util/delay.h>

#include "ardos/ardos.h"

void f1()
{
    DDRB = 1;
    
    for (; ; )
    {
        ardos_process_sleep(100);
        PORTB ^= 1;
    }
}

void f2()
{
    uint8_t debounce = 0;
    uint8_t on = 1;
    pid_t pid;

    pid = ardos_process_create(f1);
    ardos_enable_eint(ARDOS_EINT_0, ARDOS_EINT_RISING_EDGE);
    
    for (; ; )
    {
        ardos_process_sleep(200);    
        ardos_process_wait_eint(ARDOS_EINT_0);
    
        if (!on)
        {
            ardos_process_resume(pid);
            on = 1;
        }
        else
        {
            ardos_process_suspend_other(pid);
            on = 0;
        }
    }
    
    ardos_process_exit();
}

int main()
{
    ardos_init(f2);

    return 0;
}
