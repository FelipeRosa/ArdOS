

#include <avr/io.h>
#include <util/delay.h>

#include "ardos/ardos.h"

void f1()
{
    DDRD = 0x80;        
    PORTD = 0x0;
    
    for (; ; )
    {
        ardos_process_sleep(100);
        PORTD ^= 0x80;
    }
    
    ardos_process_exit();
}

void f2()
{
    pid_t cpid;

    DDRB = 1;
    PORTB = 0;
    
    cpid = ardos_process_create(f1);
    
    for (; ; )
    {    
        ardos_process_sleep(100);
        PORTB ^= 1; 
    }

    ardos_process_exit();
}

int main()
{
    ardos_init(f2);
    for (; ; ) ;

    return 0;
}
