
#include"pwm_time.h"

const unsigned int TOP = 0x03FF; // 11-bit resolution.  7812 Hz PWM

void Timer_init()
{
 pinMode(9,OUTPUT);
 ICR1 = TOP;
 TCCR1A |= (1<<COM1A1);
 TCCR1A |= (1<<WGM10) | (1 << WGM12);

TIMSK1 |= (1<<TOIE1);
  
TCCR1B |= (1 << CS11) | (1 << CS10);
}

void Enable_Timer()
{
     TCCR1A |= (1 << COM1A1);
     pinMode(9,OUTPUT);
}

void Duty_Cycle( uint16_t duty_cycle)
{
   OCR1A = 2*duty_cycle;  ///// duty cycle config
}

