

#include <avr/io.h>
#include <util/delay.h>

#include "ardos/ardos.h"

ardos_semaphore_t sem;

void f1()
{
    DDRD = 0x80;        
    PORTD = 0x0;
    
    for (; ; )
    {
        ardos_process_sleep(100);
        ardos_semaphore_wait(&sem);
        PORTD ^= 0x80;
        ardos_semaphore_signal(&sem);
    }
    
    ardos_process_exit();
}

void f2()
{
    pid_t cpid;

    DDRB = 1;
    PORTB = 0;
    
    cpid = ardos_process_create(f1);
    ardos_semaphore_init(&sem, 2);
    
    for (; ; )
    {    
        ardos_process_sleep(100);
        ardos_semaphore_wait(&sem);
        PORTB ^= 1;
        ardos_semaphore_signal(&sem);
    }

    ardos_process_exit();
}

int main()
{
    ardos_init(f2);
    for (; ; ) ;

    return 0;
}
