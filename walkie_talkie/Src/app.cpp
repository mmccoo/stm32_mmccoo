

#include "app.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"
#include "stm32f1xx_hal.h"
#include "usart.h"
#ifdef __cplusplus
 }
#endif
   
#include "ELECHOUSE_CC1101.h"

#include "string"
#include "cstring"

byte RX_buffer[11]={0};
uint8_t lighton = 0;
uint8_t state = 0;
int32_t count = 0;

extern "C"
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  uint8_t newstate =
    ((CC1101_GDO0_GPIO_Port->IDR & CC1101_GDO0_Pin) ? 0x1 : 0) +
    ((CC1101_GDO2_GPIO_Port->IDR & CC1101_GDO2_Pin) ? 0x2 : 0);

  // 0x1 rise leading 0x2 rise is +1
  // 0x1 fall leading 0x2 fall is +1
  // The sequence should be 00,01,11,10,00 or 00,10,11,01,00
  // or                      0,1,3,2,0        0,2,3,1,0

  const int8_t state_table[4][4] = {
    // to state
    // 0,   1,  2,  3
    {  0,  +1, -1,  0}, // from state 0
    { -1,   0,  0, +1}, // from state 1
    { +1,   0,  0, -1}, // from state 2
    {  0,  -1, +1,  0}  // from state 3
  };
  count += state_table[state][newstate];
  state = newstate;
  
  if (lighton) {
    MODULE_LED_GPIO_Port->ODR |= MODULE_LED_Pin;
  } else {
    MODULE_LED_GPIO_Port->ODR &= ~(MODULE_LED_Pin);
  }
  lighton = !lighton;

}

void UartTransmit(const char* msg)
{
  HAL_UART_StateTypeDef state = HAL_UART_GetState(&huart1);
  while (state != HAL_UART_STATE_READY &&
         state != HAL_UART_STATE_BUSY_RX) ;
      
  __attribute__((unused)) HAL_StatusTypeDef tstate;
  tstate = HAL_UART_Transmit(&huart1, (uint8_t*) msg, std::strlen(msg), HAL_MAX_DELAY);
}




void app_init()
{
  ELECHOUSE_cc1101.Init();
  ELECHOUSE_cc1101.SetReceive();

}

void main_loop()
{

  while (1) {
    if ((HAL_GetTick() % 1000) == 0) {
      ;
    }

    if(ELECHOUSE_cc1101.CheckReceiveFlag()) {
      byte size=ELECHOUSE_cc1101.ReceiveData(RX_buffer);
      UartTransmit("received ");
      UartTransmit(std::to_string(size).c_str());
      if (size>=sizeof(unsigned long)) {
        unsigned long val = *(unsigned int *)RX_buffer;
        ELECHOUSE_cc1101.SendData((byte*)&val,sizeof(val));
        UartTransmit("value ");
        UartTransmit(std::to_string(val).c_str());
        
        ELECHOUSE_cc1101.SetReceive();
      }
    }
    
  }

}
