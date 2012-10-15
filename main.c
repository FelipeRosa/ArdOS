

#include <avr/io.h>
#include <util/delay.h>

#include "ardos/ardos.h"

ardos_semaphore_t sem;

void f1()
{
    ardos_process_join(0);

    for (; ; )
    {
        ardos_process_sleep(100);
        ardos_semaphore_wait(&sem);
        if (PORTD & 0x80)
            PORTD &= ~0x80;
        else
            PORTD |= 0x80;
        ardos_semaphore_signal(&sem);    
    }
    
    ardos_process_exit();
}

void f2()
{
    pid_t cpid;
    uint8_t x = 0;
    
    DDRD = 0x80 | 0x40;

    ardos_semaphore_init(&sem, 1);
    cpid = ardos_process_create(f1);
    
    for (; ; )
    {    
        ardos_process_sleep(200);
        ardos_semaphore_wait(&sem);
        if (PORTD & 0x40)
            PORTD &= ~0x40;
        else
            PORTD |= 0x40;
        ardos_semaphore_signal(&sem);
            
        if (x < 8)
        {
            x++;
        }
        else if (x == 8)
        {
            x++;
            ardos_process_exit();
        }
    }

    ardos_process_exit();
}

int main()
{
    ardos_init(f2);
    for (; ; ) ;

    return 0;
}
