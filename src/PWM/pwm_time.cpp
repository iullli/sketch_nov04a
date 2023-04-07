
#include"pwm_time.h"


void Timer_init()
{
    TCCR1B &= ~(1<<WGM13);
    TCCR1B |= (1<<WGM12);
    TCCR1A |= (1<<WGM10) | (1<<WGM11);   /// TIMER 1 PWM 10 BIT MODE

    TCCR1A &= ~(1<<COM1A0);
    TCCR1A |= (1<<COM1A1);      ////PWM non invrting mode

    TCCR1B |=  (1<<CS10) | (1<<CS11);     /// prescaler = 64
    TCCR1B &= ~(1<<CS12);




// OCR1A = 1023;


}


void Duty_Cycle( volatile uint16_t duty_cycle)
{
   OCR1A = duty_cycle;  ///// duty cycle config
}