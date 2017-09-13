

#include "app.h"
#include "main.h"
#include "stm32f1xx_hal.h"

void main_loop()
{
  uint8_t lighton = 0;
  while (1) {
    if ((HAL_GetTick() % 1000) == 0) {
      if (lighton) {
        MODULE_LED_GPIO_Port->ODR |= MODULE_LED_Pin;
      } else {
        MODULE_LED_GPIO_Port->ODR &= ~(MODULE_LED_Pin);
      }
      lighton = !lighton;
    }
  }

}
