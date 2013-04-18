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

#include "external_interrupts.h"

#include <avr/io.h>


void ardos_enable_eint(ardos_eint_t int_num, ardos_eint_trigger_t trigger)
{
    /* Turn on the correct interrupt and triggering */
    switch (int_num)
    {
        case ARDOS_EINT_0:
            switch (trigger)
            {
                case ARDOS_EINT_LOW_LEVEL:
                    EICRA &= ~(1 << ISC00 | 1 << ISC01);
                    break;
                
                case ARDOS_EINT_ANY_LEVEL:
                    EICRA &= ~(1 << ISC01);
                    EICRA |= 1 << ISC00;
                    break;
                    
                case ARDOS_EINT_FALLING_EDGE:
                    EICRA &= ~(1 << ISC00);
                    EICRA |= 1 << ISC01;
                    break;
                    
                case ARDOS_EINT_RISING_EDGE:
                    EICRA |= 1 << ISC01 | 1 << ISC00;
                    break;
            }
        
            EIMSK |= 1 << INT0;
            break;
        
        case ARDOS_EINT_1:
            switch (trigger)
            {
                case ARDOS_EINT_LOW_LEVEL:
                    EICRA &= ~(1 << ISC10 | 1 << ISC11);
                    break;
                
                case ARDOS_EINT_ANY_LEVEL:
                    EICRA &= ~(1 << ISC11);
                    EICRA |= 1 << ISC10;
                    break;
                    
                case ARDOS_EINT_FALLING_EDGE:
                    EICRA &= ~(1 << ISC10);
                    EICRA |= 1 << ISC11;
                    break;
                    
                case ARDOS_EINT_RISING_EDGE:
                    EICRA |= 1 << ISC11 | 1 << ISC10;
                    break;
            }
            
            EIMSK |= 1 << INT1;
            break;
            
        default:
            break;
    }    
}

void ardos_disable_eint(ardos_eint_t int_num)
{
    switch (int_num)
    {
        case ARDOS_EINT_0:
            EIMSK &= ~(1 << INT0);
            break;
            
        case ARDOS_EINT_1:
            EIMSK &= ~(1 << INT1);
            break;
            
        default:
            break;
    }
}
