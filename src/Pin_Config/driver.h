#include<Arduino.h>

#ifndef DRIVER_H
#define DRIVER_H

#endif

#define SET      1
#define RESET    0

#define OUTPUT   SET
#define INPUT    RESET



void WrtiePin(uint8_t, uint8_t, uint8_t);
void PinMode(uint8_t, uint8_t, uint8_t);