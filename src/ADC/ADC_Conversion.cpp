#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "ADC_Conversion.h"


float ADC_Configuration(uint8_t ADCx)
{
float  value = 0.0;
ADCSRA |= (1<<ADEN);
ADMUX &= ~(1<<MUX0) & ~(1<<MUX1) & ~(1<<MUX2) & ~(1<<MUX3);
ADMUX = ( ADMUX & 0B11110000) | ADCx;

ADMUX &= ~(1<<REFS0) & ~(1<<REFS1);
ADMUX &= ~(1<<ADLAR);
ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
ADCSRA |= (1<<ADSC);

while((ADCSRA & (1<<ADSC)));

value = ADCL | (ADCH<<8);
return value;
}