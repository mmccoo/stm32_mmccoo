/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

#include "OLED.h"
#include "u8g2.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
u8g2_t u8g2;

#define UNUSEDVAR __attribute__ ((unused))

#define test_width 16
#define test_height 7
 UNUSEDVAR static const uint8_t test_bits[] = {
   0x13, 0x00,
   0x15, 0x00,
   0x93, 0xcd,
   0x55, 0xa5,
   0x93, 0xc5,
   0x00, 0x80,
   0x00, 0x60
 };

#define xlogo64_width 64
#define xlogo64_height 64
UNUSEDVAR static unsigned char xlogo64_bits[] = {
   0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xfe, 0xff, 0x01, 0x00,
   0x00, 0x00, 0x00, 0xf8, 0xfc, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x7c,
   0xf8, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0x3e, 0xf8, 0xff, 0x07, 0x00,
   0x00, 0x00, 0x00, 0x1f, 0xf0, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x80, 0x0f,
   0xe0, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x80, 0x0f, 0xc0, 0xff, 0x3f, 0x00,
   0x00, 0x00, 0xc0, 0x07, 0xc0, 0xff, 0x3f, 0x00, 0x00, 0x00, 0xe0, 0x03,
   0x80, 0xff, 0x7f, 0x00, 0x00, 0x00, 0xf0, 0x01, 0x00, 0xff, 0xff, 0x00,
   0x00, 0x00, 0xf8, 0x00, 0x00, 0xfe, 0xff, 0x01, 0x00, 0x00, 0xf8, 0x00,
   0x00, 0xfe, 0xff, 0x01, 0x00, 0x00, 0x7c, 0x00, 0x00, 0xfc, 0xff, 0x03,
   0x00, 0x00, 0x3e, 0x00, 0x00, 0xf8, 0xff, 0x07, 0x00, 0x00, 0x1f, 0x00,
   0x00, 0xf0, 0xff, 0x0f, 0x00, 0x80, 0x0f, 0x00, 0x00, 0xf0, 0xff, 0x0f,
   0x00, 0xc0, 0x07, 0x00, 0x00, 0xe0, 0xff, 0x1f, 0x00, 0xc0, 0x07, 0x00,
   0x00, 0xc0, 0xff, 0x3f, 0x00, 0xe0, 0x03, 0x00, 0x00, 0x80, 0xff, 0x7f,
   0x00, 0xf0, 0x01, 0x00, 0x00, 0x80, 0xff, 0x7f, 0x00, 0xf8, 0x00, 0x00,
   0x00, 0x00, 0xff, 0xff, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xff,
   0x01, 0x7c, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x03, 0x3e, 0x00, 0x00,
   0x00, 0x00, 0xfc, 0xff, 0x03, 0x1f, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff,
   0x87, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xcf, 0x07, 0x00, 0x00,
   0x00, 0x00, 0xe0, 0xff, 0xcf, 0x07, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff,
   0xe7, 0x03, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xf3, 0x01, 0x00, 0x00,
   0x00, 0x00, 0x80, 0xff, 0xf9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
   0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x7e, 0xfe, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e,
   0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9f, 0xff, 0x07, 0x00, 0x00,
   0x00, 0x00, 0x80, 0xcf, 0xff, 0x07, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xe7,
   0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xe7, 0xff, 0x1f, 0x00, 0x00,
   0x00, 0x00, 0xe0, 0xc3, 0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xc1,
   0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x80, 0xff, 0x7f, 0x00, 0x00,
   0x00, 0x00, 0x7c, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00,
   0xfe, 0xff, 0x01, 0x00, 0x00, 0x00, 0x3e, 0x00, 0xfe, 0xff, 0x01, 0x00,
   0x00, 0x00, 0x1f, 0x00, 0xfc, 0xff, 0x03, 0x00, 0x00, 0x80, 0x0f, 0x00,
   0xf8, 0xff, 0x07, 0x00, 0x00, 0xc0, 0x07, 0x00, 0xf0, 0xff, 0x0f, 0x00,
   0x00, 0xe0, 0x03, 0x00, 0xf0, 0xff, 0x0f, 0x00, 0x00, 0xe0, 0x03, 0x00,
   0xe0, 0xff, 0x1f, 0x00, 0x00, 0xf0, 0x01, 0x00, 0xc0, 0xff, 0x3f, 0x00,
   0x00, 0xf8, 0x00, 0x00, 0x80, 0xff, 0x7f, 0x00, 0x00, 0x7c, 0x00, 0x00,
   0x80, 0xff, 0x7f, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00,
   0x00, 0x3e, 0x00, 0x00, 0x00, 0xfe, 0xff, 0x01, 0x00, 0x1f, 0x00, 0x00,
   0x00, 0xfc, 0xff, 0x03, 0x80, 0x0f, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x03,
   0xc0, 0x07, 0x00, 0x00, 0x00, 0xf8, 0xff, 0x07, 0xe0, 0x03, 0x00, 0x00,
   0x00, 0xf0, 0xff, 0x0f, 0xe0, 0x03, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x1f,
   0xf0, 0x01, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x1f, 0xf8, 0x00, 0x00, 0x00,
   0x00, 0xc0, 0xff, 0x3f, 0x7c, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0x7f,
   0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff};


// convert lydia.jpg -crop 64x64 -monochrome -negate lydia.xbm
#define lydia_width 64
#define lydia_height 64
static unsigned char lydia_bits[] = {
  0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xDF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0x7F, 0xAB, 0xFA, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBA, 
  0xFF, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0x6A, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0x2F, 0xF5, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF5, 
  0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEB, 0xDE, 0xEA, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xD7, 0xAE, 0x37, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x5B, 0x7B, 0xB3, 
  0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xA5, 0xDD, 0xDE, 0xFC, 0xFF, 0xFF, 0xFF, 
  0x7F, 0xDA, 0xFF, 0x4F, 0xFD, 0xFF, 0xFF, 0xFF, 0xFF, 0xA5, 0xFB, 0xBB, 
  0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xEA, 0xFF, 0x67, 0xFD, 0xFF, 0xFF, 0xFF, 
  0x7F, 0xD6, 0xFF, 0xBF, 0xFA, 0xFF, 0xFF, 0xFF, 0x3F, 0xF0, 0xFF, 0xBF, 
  0xFE, 0xFF, 0xFF, 0xFF, 0xBF, 0xEC, 0xFF, 0xDF, 0xF9, 0xFF, 0xFF, 0xFF, 
  0x7F, 0x18, 0xF9, 0x6B, 0xFA, 0xFF, 0xED, 0xFF, 0x7F, 0x40, 0xA6, 0xBF, 
  0xF6, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x96, 0x9B, 0xFB, 0xFF, 0xFF, 0xFF, 
  0x3F, 0x00, 0xFD, 0x5A, 0xFE, 0xFF, 0xFB, 0xFF, 0x3F, 0x00, 0xB4, 0xD5, 
  0xFE, 0x7F, 0xFA, 0xFF, 0x3F, 0x00, 0xD8, 0x1A, 0xFB, 0xFF, 0xFF, 0xFF, 
  0x7F, 0x00, 0x62, 0xC3, 0xFA, 0xFF, 0xFB, 0xFF, 0x7F, 0x00, 0x98, 0x00, 
  0xFD, 0xFF, 0xFA, 0xFF, 0x7F, 0x00, 0xA0, 0x04, 0xF4, 0x7F, 0x77, 0xFF, 
  0x3F, 0x00, 0x40, 0x23, 0xA0, 0xFF, 0xE9, 0xFF, 0x3F, 0x00, 0x98, 0x02, 
  0xCC, 0x7F, 0xFE, 0xFF, 0x3F, 0x00, 0x60, 0x05, 0xF0, 0xFF, 0xA5, 0xFF, 
  0x3F, 0x00, 0xC0, 0xC2, 0xEB, 0xBE, 0xE9, 0xFF, 0x7F, 0x00, 0x58, 0x81, 
  0xF0, 0xFF, 0xAE, 0xFF, 0x3F, 0x00, 0xF8, 0x01, 0xE0, 0x7F, 0xF5, 0xFF, 
  0x3F, 0x00, 0x22, 0x00, 0xF4, 0xFF, 0xAA, 0xFF, 0x7F, 0x00, 0x34, 0x40, 
  0xF4, 0xFF, 0xE5, 0xFF, 0x7F, 0x00, 0x5A, 0x00, 0xA9, 0xFB, 0xBB, 0xFF, 
  0x3F, 0x00, 0x24, 0x00, 0x64, 0xFF, 0xA7, 0xFF, 0x7F, 0x00, 0x5A, 0x00, 
  0xD9, 0xFF, 0xE5, 0xFF, 0xFF, 0x00, 0x1A, 0x00, 0xFA, 0xFF, 0x67, 0xFF, 
  0x7F, 0x80, 0xAD, 0xB6, 0xFA, 0xFF, 0xE7, 0xFF, 0xFF, 0x00, 0xF8, 0xA5, 
  0xED, 0xFA, 0xD7, 0xFF, 0x7F, 0x00, 0xFE, 0x01, 0xA8, 0xFF, 0xE7, 0xFF, 
  0xFF, 0x01, 0xDE, 0x03, 0x00, 0xFF, 0xE3, 0xFF, 0xFF, 0x01, 0xA8, 0x07, 
  0xC0, 0xFF, 0xDB, 0xFF, 0xFF, 0x03, 0x3E, 0x06, 0xFC, 0xFF, 0xF7, 0xFF, 
  0xFF, 0x03, 0x00, 0x20, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x0D, 0x80, 
  0xED, 0xFF, 0xF7, 0xFF, 0xFF, 0x8F, 0x00, 0x00, 0xF6, 0xFF, 0xF9, 0xFF, 
  0xFF, 0x1F, 0x00, 0xC0, 0xD5, 0xFF, 0xFF, 0xFF, 0xFF, 0x5F, 0x02, 0x00, 
  0xDA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x6A, 0xFE, 0xFF, 0xFF, 
  0xFF, 0xFF, 0x01, 0x00, 0x95, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 
  0xEC, 0xF6, 0xFF, 0xFF, 0xFF, 0xFF, 0x1F, 0x00, 0x40, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0x0F, 0x00, 0xB0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0x00, 
  0x80, 0xFE, 0xFF, 0xFF, 0xFE, 0xFF, 0x7F, 0x00, 0xA8, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0x00, 0x90, 0xFF, 0xFF, 0xFF, };


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void delayLoop() {
        volatile uint32_t delayCount = 100000; // volatile, um "Wegoptimieren" zu vermeinden
                                               //(http://en.wikipedia.org/wiki/Volatile_variable)
        while (delayCount > 0) {
                delayCount--;
        }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  HAL_UART_StateTypeDef state = HAL_UART_GetState(huart);
  if (state == HAL_UART_STATE_READY) {

  } else {

  }
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
  HAL_SPI_StateTypeDef state = HAL_SPI_GetState(hspi);
  if (state == HAL_SPI_STATE_READY) {

  } else {

  }
  
}

void byte_at_string(uint8_t *str, int i)
{
  
  str[2] = '0' + i%10;
  str[1] = '0' + (i/10)%10;
  str[0] = '0' + (i/100)%10;
  
}
void btoa(uint8_t i, uint8_t *str)
{  
  str[2] = '0' + i%10;
  str[1] = '0' + (i/10)%10;
  str[0] = '0' + (i/100)%10; 
}

void itoa(uint32_t i, uint8_t *str)
{  
  str[4] = '0' + i%10;
  str[3] = '0' + (i/10)%10;
  str[2] = '0' + (i/100)%10;
  str[1] = '0' + (i/1000)%10;
  str[0] = '0' + (i/10000)%10; 
}


void hex_at_string(uint8_t *str, int i)
{
  uint8_t lower = i&0xF;
  uint8_t upper = (i&0xF0) >> 4;

  if (lower < 10) {
    str[1] = '0' + lower;
  } else {
    str[1] = 'A' + lower - 10;    
  }
  if (upper < 10) {
    str[0] = '0' + upper;
  } else {
    str[0] = 'A' + upper - 10;    
  }  
}


int my_I2C_GET_FLAG(I2C_HandleTypeDef* __HANDLE__,
                    uint32_t __FLAG__)
{
  // #define I2C_FLAG_BUSY                   ((uint32_t)0x0010 0002)
  // #define I2C_FLAG_MASK                   ((uint32_t)0x0000 FFFF)
  if (((uint8_t)((__FLAG__) >> 16)) == 0x01) {
    return (__HANDLE__->Instance->SR1 & __FLAG__ & I2C_FLAG_MASK) == (__FLAG__ & I2C_FLAG_MASK);
  } else {
    return (__HANDLE__->Instance->SR2 & __FLAG__ & I2C_FLAG_MASK) == (__FLAG__ & I2C_FLAG_MASK);
  }
}

void sendUARTmsgPoll(uint8_t *msg, uint16_t length) {
  while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY) {/*empty*/}
  HAL_UART_Transmit(&huart1, msg, length, HAL_MAX_DELAY);

}


uint32_t amountsent =0;
//extern "C"

uint8_t
u8x8_byte_my_hw_i2c(
  U8X8_UNUSED u8x8_t *u8x8,
  U8X8_UNUSED uint8_t msg,
  U8X8_UNUSED uint8_t arg_int,
  U8X8_UNUSED void *arg_ptr)
{
#define MAX_LEN 32
  static uint8_t vals[MAX_LEN];
  static uint8_t length=0;
  
  U8X8_UNUSED uint8_t *args = arg_ptr;
  switch(msg)  {
  case U8X8_MSG_BYTE_SEND: {
    if ((arg_int+length) <= MAX_LEN) {
      for(int i=0; i<arg_int; i++) {
        vals[length] = args[i];
        length++;
      }
    } else {
      uint8_t umsg[] = "MSG_BYTE_SEND arg too long xxx\n";
      byte_at_string(umsg+27, arg_int);
      sendUARTmsgPoll(umsg, sizeof(umsg));
    }
    uint8_t umsg[] = "MSG_BYTE_SEND 0xxx\n";
    // 00 AE = display off
    // 00 D5 = set display clock
    // 00 80    divide ratio
    // 00 A8 = set multiplex ratio
    // 00 3F    64 mux
    // 00 D3 = set display offset
    // 00 00
    // 00 40 - 00 7F set display start line
    
    
    hex_at_string(umsg+16, args[0]);
    //sendUARTmsgPoll(umsg, sizeof(umsg));
    break;
  }
  case U8X8_MSG_BYTE_INIT: {
    uint8_t umsg[] = "MSG_BYTE_INIT xxx\n";
    byte_at_string(umsg+14, arg_int);
    //sendUARTmsgPoll(umsg, sizeof(umsg));
    break;
  }
  case U8X8_MSG_BYTE_SET_DC: {
    uint8_t umsg[] = "MSG_BYTE_SET_DC xxx\n";
    byte_at_string(umsg+15, arg_int);
    sendUARTmsgPoll(umsg, sizeof(umsg));
    break;
  }
  case U8X8_MSG_BYTE_START_TRANSFER: {
    UNUSEDVAR uint8_t umsg[] = "MSG_BYTE_START\n";
    //sendUARTmsgPoll(umsg, sizeof(umsg));
    length = 0;
    break;
  }
  case U8X8_MSG_BYTE_END_TRANSFER: {
    UNUSEDVAR uint8_t umsg[] = "MSG_BYTE_END xxxxx\n";
    itoa(length, umsg+13);
    //sendUARTmsgPoll(umsg, sizeof(umsg));
    while(HAL_I2C_GetState (&hi2c1) != HAL_I2C_STATE_READY) { /* empty */ }
    const uint8_t addr = 0x78;
    HAL_I2C_Master_Transmit(&hi2c1, addr, vals, length, 10);
    amountsent+= length;
    break;
  }
  default: {
    uint8_t umsg[] = "MSG_BYTE_DEFAULT xxx\n";
    byte_at_string(umsg+17, arg_int);
    //sendUARTmsgPoll(umsg, sizeof(umsg));
    return 0;
  }
  }
  return 1;
}

uint8_t u8x8_gpio_and_delay_mine(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  UNUSEDVAR uint8_t umsg[] = "calling gpio and delay\n";
  //sendUARTmsgPoll(umsg, sizeof(umsg));
  
  switch(msg)
  {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
      /* only support for software I2C*/
    
     
      break;
    case U8X8_MSG_DELAY_NANO:
      /* not required for SW I2C */
      break;
    
    case U8X8_MSG_DELAY_10MICRO:
      /* not used at the moment */
      break;
    
    case U8X8_MSG_DELAY_100NANO:
      /* not used at the moment */
      break;
   
    case U8X8_MSG_DELAY_MILLI:
      //delay_micro_seconds(arg_int*1000UL);
      break;
    case U8X8_MSG_DELAY_I2C:
      /* arg_int is 1 or 4: 100KHz (5us) or 400KHz (1.25us) */
      //delay_micro_seconds(arg_int<=2?5:1);
      break;
    
    case U8X8_MSG_GPIO_I2C_CLOCK:
      
      break;
    case U8X8_MSG_GPIO_I2C_DATA:
      
      break;
/*
    case U8X8_MSG_GPIO_MENU_SELECT:
      u8x8_SetGPIOResult(u8x8, Chip_GPIO_GetPinState(LPC_GPIO, KEY_SELECT_PORT, KEY_SELECT_PIN));
      break;
    case U8X8_MSG_GPIO_MENU_NEXT:
      u8x8_SetGPIOResult(u8x8, Chip_GPIO_GetPinState(LPC_GPIO, KEY_NEXT_PORT, KEY_NEXT_PIN));
      break;
    case U8X8_MSG_GPIO_MENU_PREV:
      u8x8_SetGPIOResult(u8x8, Chip_GPIO_GetPinState(LPC_GPIO, KEY_PREV_PORT, KEY_PREV_PIN));
      break;
    
    case U8X8_MSG_GPIO_MENU_HOME:
      u8x8_SetGPIOResult(u8x8, Chip_GPIO_GetPinState(LPC_GPIO, KEY_HOME_PORT, KEY_HOME_PIN));
      break;
*/
    default:
      //u8x8_SetGPIOResult(u8x8, 1);
      break;
  }
  return 1;
}

void HAL_SYSTICK_Callback(void)
{
  static uint8_t ledon = 1;
  if ((HAL_GetTick() % 1000) == 0) {
    if (ledon) {
      GPIOC->ODR |= GPIO_PIN_13;
    } else {
      GPIOC->ODR &= ~(GPIO_PIN_13);
    }
    ledon = !ledon;
  }
}

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();

  /* USER CODE BEGIN 2 */
  UNUSEDVAR uint8_t themessage[] = "E.T. is phoning home. Anyone there? Be sure to check spi\n";
  UNUSEDVAR uint8_t theothermessage[] = "Hey. I hate to interrupt, but...\n";
  UNUSEDVAR uint8_t spimessage[] = "This is a secret side message over spi\n";
  UNUSEDVAR uint8_t elapsedmessage[] = "time elapsed to draw xxx xxxxx xxx\n";
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  UNUSEDVAR uint8_t uart_poll = 1;
  UNUSEDVAR uint8_t spi_poll = 1;
  //uint8_t text_size = 0;

  u8g2_Setup_ssd1306_i2c_128x64_noname_1(&u8g2,
                                         U8G2_R0,
                                         u8x8_byte_my_hw_i2c,
                                         u8x8_gpio_and_delay_mine);
  
  u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
  { uint8_t umsg[] = "done with init\n"; sendUARTmsgPoll(umsg, sizeof(umsg)); }
  u8g2_SetPowerSave(&u8g2, 0); // wake up display
  { uint8_t umsg[] = "first page\n"; sendUARTmsgPoll(umsg, sizeof(umsg)); }

#if 0
  u8g2_FirstPage(&u8g2);
  do {
    { uint8_t umsg[] = "set font\n"; sendUARTmsgPoll(umsg, sizeof(umsg)); }
    u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tr);
    { uint8_t umsg[] = "draw hello\n"; sendUARTmsgPoll(umsg, sizeof(umsg)); }
    u8g2_DrawStr(&u8g2, 0,24,"Hi World!");
    { uint8_t umsg[] = "done\n"; sendUARTmsgPoll(umsg, sizeof(umsg)); }
  } while ( u8g2_NextPage(&u8g2) );
#endif

#if 1
  //u8g2_FirstPage(&u8g2);
  //do {
  //u8g2_DrawBox(&u8g2, 10,20, 20, 30);
  u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tr);
  //u8g2_DrawStr(&u8g2, 0,15,"This World!");
  //u8g2_DrawStr(&u8g2, 0,30,"Other World!");
  //u8g2_DrawXBM(&u8g2, 0, 20, LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT, logo16_glcd_bmp);
  u8g2_SetDrawColor(&u8g2, 0);
  u8g2_DrawXBM(&u8g2, 0, 0, lydia_width, lydia_height, lydia_bits);
  u8g2_DrawXBM(&u8g2, 64, 0, xlogo64_width, xlogo64_height, xlogo64_bits);
#endif

#if 0
  u8g2_SetFontMode(&u8g2, 1);	// Transparent
  u8g2_SetFontDirection(&u8g2, 0);
  u8g2_SetFont(&u8g2, u8g2_font_inb24_mf);
  u8g2_DrawStr(&u8g2, 0, 30, "U");
    
  u8g2_SetFontDirection(&u8g2, 1);
  u8g2_SetFont(&u8g2, u8g2_font_inb30_mn);
  u8g2_DrawStr(&u8g2, 21,8,"8");
        
  u8g2_SetFontDirection(&u8g2, 0);
  u8g2_SetFont(&u8g2, u8g2_font_inb24_mf);
  u8g2_DrawStr(&u8g2, 51,30,"g");
  u8g2_DrawStr(&u8g2, 67,30,"\xb2");
    
  u8g2_DrawHLine(&u8g2, 2, 35, 47);
  u8g2_DrawHLine(&u8g2, 3, 36, 47);
  u8g2_DrawVLine(&u8g2, 45, 32, 12);
  u8g2_DrawVLine(&u8g2, 46, 33, 12);
  
  u8g2_SetFont(&u8g2, u8g2_font_4x6_tr);
  u8g2_DrawStr(&u8g2, 1,54,"github.com/olikraus/u8g2");
#endif
    
  u8g2_SendBuffer(&u8g2);
  //} while ( u8g2_NextPage(&u8g2) );

  UNUSEDVAR uint32_t myfreq = HAL_RCC_GetSysClockFreq();
  UNUSEDVAR uint32_t mysize = * (uint32_t*) FLASHSIZE_BASE;
  UNUSEDVAR uint32_t myid   = * (uint32_t*) UID_BASE;
  
  //InitOled();
  //OLED_Fill(0);
  uint8_t lasttime = 0;
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
    // p HAL_I2C_GetState (&hi2c1)
    // p __HAL_I2C_GET_FLAG(&hi2c1, I2C_FLAG_BUSY)

    uint32_t curtime = HAL_GetTick();

    //OLED_ShowStr(0,3, (unsigned char*) "HelTec Automation", (text_size++ % 2));

    u8g2_ClearBuffer(&u8g2);
    u8g2_FirstPage(&u8g2);
    UNUSEDVAR uint32_t cleartime = HAL_GetTick();

    u8g2_SetDrawColor(&u8g2, 1);
    u8g2_SetFontMode(&u8g2, 1);	// Transparent
    u8g2_SetFontDirection(&u8g2, 0);
    u8g2_SetFont(&u8g2, u8g2_font_inb16_mf);
    char time[] = "xxxxx";
    itoa(curtime,(uint8_t*) time);

    amountsent=0;
    uint8_t pages = 2;
    do {
      u8g2_DrawStr(&u8g2, 0, 16, time);
      //NextPage calls SendBuffer
      //u8g2_SendBuffer(&u8g2);
      pages--;
    } while (u8g2_NextPage(&u8g2) && pages>0);
    UNUSEDVAR uint32_t finaltime = HAL_GetTick();
    UNUSEDVAR uint32_t elapsed =  finaltime-curtime;
    if (HAL_UART_GetState(&huart1) == HAL_UART_STATE_READY) {
      btoa(elapsed, elapsedmessage+21);
      itoa(amountsent,  elapsedmessage+25);
      HAL_UART_Transmit_IT(&huart1, elapsedmessage, sizeof(elapsedmessage));
    };
    
    if (((curtime % 1000) != 0) || (curtime == lasttime)) {
      continue;
    }
    lasttime = curtime;
    
    HAL_I2C_StateTypeDef iicstate =  HAL_I2C_GetState (&hi2c1);
    HAL_UART_StateTypeDef uartstate = HAL_UART_GetState(&huart1);
    if ((iicstate  == HAL_I2C_STATE_READY) &&
        (uartstate == HAL_UART_STATE_READY)) {
#if 0
      // this is basically a I2C address scanner.
      // I used it to verify that oled is at 120 and 121.
      // the actual addresses are half that and the second is simply the other
      // of read/write.
      
      uint8_t ii2messagealive[] = "I2C channel xxx is alive\n";
      uint8_t ii2messagedead[]  = "I2C channel xxx is dead\n";

      for(uint8_t i=0; i<128; i++) {
        uint8_t pData[] = "hello";
        HAL_StatusTypeDef iicstatus UNUSEDVAR;
        iicstatus = HAL_I2C_Master_Transmit(&hi2c1, i, pData, sizeof(pData), 10);
        if (iicstatus == HAL_OK) {
          byte_at_string(ii2messagealive+12, i);
          HAL_UART_Transmit(&huart1, ii2messagealive, sizeof(ii2messagealive), HAL_MAX_DELAY);
        } else {
          byte_at_string(ii2messagedead+12, i);
          HAL_UART_Transmit(&huart1, ii2messagedead, sizeof(ii2messagedead), HAL_MAX_DELAY);
        }
      }
#endif
            
    }

    
#if 0
    if (uartstate == HAL_UART_STATE_READY) {
      HAL_StatusTypeDef tstate UNUSEDVAR;
      if (uart_poll) {
        //tstate = HAL_UART_Transmit(&huart1, themessage, sizeof(themessage), HAL_MAX_DELAY);
        sendUARTmsgPoll(themessage, sizeof(themessage));
      } else {
        tstate = HAL_UART_Transmit_IT(&huart1, theothermessage, sizeof(theothermessage));     
      }
      uart_poll = !uart_poll;
      uartstate = HAL_UART_GetState(&huart1);
    }
#endif

#if 0
    HAL_SPI_StateTypeDef spistate =  HAL_SPI_GetState (&hspi1);
    if (spistate == HAL_SPI_STATE_READY) {
      HAL_StatusTypeDef hstatus UNUSEDVAR;
      if (spi_poll) {
        hstatus = HAL_SPI_Transmit (&hspi1, spimessage, sizeof(spimessage), HAL_MAX_DELAY);
      }
      spi_poll = !spi_poll;
      spistate =  HAL_SPI_GetState (&hspi1);
    }
#endif
    
#if 0
    GPIOC->ODR |= GPIO_PIN_13;
    delayLoop();
    delayLoop();
    GPIOC->ODR &= ~(GPIO_PIN_13);
    delayLoop();
#endif
  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
