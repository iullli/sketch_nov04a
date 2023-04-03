#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "ADC_Conversion.h"

float ADC_Configuration(uint8_t ADCx)
{
int  value = 0;
ADMUX &= ~(1<<MUX0) & ~(1<<MUX1) & ~(1<<MUX2) & ~(1<<MUX3);
ADMUX |= ADCx;
ADMUX &= ~(1<<REFS1) & ~(1<<REFS0);
ADMUX &= ~(1<<ADLAR);
ADCSRA |= (1<<ADPS1) | (1<<ADPS0);
ADCSRA |= (1<<ADEN) | (1<<ADSC);

while((ADCSRA & (1<<ADSC)));

value = ADCL | (ADCH<<8);
return value;

}

void Clean_ADC_reg(void)
{

ADCL &= ~(1<<ADCL) ;
ADCH &=  ~(1<<ADCH);
}