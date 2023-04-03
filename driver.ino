

#include "driver.h"
#include "math.h"


void WrtiePin(uint8_t port, uint8_t pin, uint8_t state)
{
    if (port == PORTB)
    {
        if (state == SET)
            PORTB |= (1 << pin);
        else
            PORTB &= ~(1 << pin);
    }
    else
    {
        if (port == PORTC)
        {
            if (state == SET)
                PORTC |= (1 << pin);
            else
                PORTC &= ~(1 << pin);
        }
        else
        {
            if (port == PORTD)
            {
                if (state == SET)
                    PORTD |= (1 << pin);
                else
                    PORTD &= ~(1 << pin);
            }
        }
    }
}

void PinMode(uint8_t port, uint8_t pin, uint8_t mode)
{
    if (port == PORTB)
    {
        DDRB |= (mode << pin);
    }
    else
    {
        if (port == PORTC)
        {
            DDRC |= (mode << pin);
        }
        else
        {

            DDRD |= (mode << pin);
        }
    }
}


