

#include <avr/io.h>
#include <util/delay.h>

#include "ardos/ardos.h"

ardos_semaphore_t sem;

void f1()
{
    ardos_semaphore_wait(&sem);
    PORTB = 1;
    ardos_process_sleep(1000);
    PORTB = PORTB * 2;
    ardos_process_sleep(2000);
    ardos_semaphore_signal(&sem);

    ardos_process_exit();
}

void f2()
{
    DDRB = 3;
    
    ardos_semaphore_init(&sem, 1);
    ardos_process_create(f1);
    
    ardos_semaphore_wait(&sem);
    PORTB = 2;
    ardos_process_sleep(2000);
    ardos_semaphore_signal(&sem);
    
    ardos_process_exit();
}

int main()
{
    ardos_init(f2);

    return 0;
}
