

#ifndef ARDUINO_HH
#define ARDUINO_HH

#include "stm32f1xx_hal.h"

typedef uint8_t byte;

struct ArduinoPin {
  GPIO_TypeDef *port;
  uint16_t      pin;
};

enum ArduinoState {
  LOW = 0,
  HIGH = 1
};

enum ArduinoDirection {
  INPUT, OUTPUT
};

inline
void digitalWrite(ArduinoPin &pin, ArduinoState state)
{
  if (state==LOW) {
    pin.port->BSRR = pin.pin << 16;
  } else {
    pin.port->BSRR = pin.pin;
  }
}

inline
ArduinoState digitalRead(ArduinoPin &pin)
{
  return (pin.port->IDR & pin.pin) ? HIGH : LOW;
}

inline
void pinMode(ArduinoPin &pin, ArduinoDirection dir)
{

}

inline
void delay(uint32_t d)
{
  uint32_t exittime = HAL_GetTick() + d;
  while(HAL_GetTick() != exittime) ;
}

#endif
