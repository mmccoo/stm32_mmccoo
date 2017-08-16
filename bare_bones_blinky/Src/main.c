
#include <stdint.h>
#include "main.h"
#include "stm32f103xb.h"

#include "register_defines.h"

static void MX_GPIO_Init(void);

void delayLoop() {
	volatile uint32_t delayCount = 100000; // volatile, um "Wegoptimieren" zu vermeinden
                                               //(http://en.wikipedia.org/wiki/Volatile_variable)
	while (delayCount > 0) {
		delayCount--;
	}
}


int main(void)
{
  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /* Infinite loop */
  while (1) {
	  LED_GPIO_Port->ODR |= LED_Pin;
	  delayLoop();
	  delayLoop();
	  LED_GPIO_Port->ODR &= ~(LED_Pin);
	  delayLoop();
  }

}

static void MX_GPIO_Init(void)
{

  
  /* GPIO Ports Clock Enable */
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

  // Set LED pin to zero
  LED_GPIO_Port->BSRR =  (uint32_t)LED_Pin << 16;
    
  // get the pin position. ctz is "count trailing zeros".
  // in this case, it tells us the the lowest set bit is
  // in position 12.
  int pinnum = __builtin_ctz(LED_Pin);

  // config is two up from mode.
  int ctrl_val = GPIO_CR_CNF_GP_OUTPUT_PP*4 + GPIO_SPEED_FREQ_HIGH;
  if (pinnum < 8) {
    LED_GPIO_Port->CRL |= (ctrl_val << (pinnum*4));
  } else {
    LED_GPIO_Port->CRH |= (ctrl_val << ((pinnum-8)*4));
  }
}



