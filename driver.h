
#include "stdint.h"


#define SET      1
#define RESET    0

#define OUTPUT   SET
#define INPUT    RESET



 void WrtiePin(uint8_t port, uint8_t pin, uint8_t state);
 void PinMode(uint8_t port, uint8_t pin, uint8_t mode);
