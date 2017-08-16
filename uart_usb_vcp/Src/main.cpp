/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "dma.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
#include <utility>

/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/



struct Queue {
  uint8_t  tosend[512];
  uint32_t tosend_begin;
  uint32_t tosend_end;
  uint8_t  num_rounds;
  Queue() {
    tosend_begin = 0;
    tosend_end   = 0;
    num_rounds   = 0;
  };

  void enqueue(uint8_t *data, uint32_t length) {
    for(uint32_t i=0; i<length; i++) {
      tosend[tosend_end] = data[i];
      tosend_end++;
      if (tosend_end>=sizeof(tosend)) {
        tosend_end = 0;
      }
      tosend[tosend_end] = 0;
    }    
  }

  std::pair<uint8_t*, uint32_t> dequeue() {
    // enqueue may move uart_tosend_end forward. Since it's called
    // from an interrupt, it needs to be treated a volatile.
    // uart_tosend_begin is also changed here.
    uint32_t end = tosend_end;
    if (tosend_begin == end) {
      return std::make_pair(tosend+tosend_begin, 0);
    }
    if (tosend_begin < end) {
      // begin is before end. haven't wrapped.
      // need to capture the current value of tosend_begin before changing it.
      std::pair<uint8_t*, uint32_t> retval =
        std::make_pair(tosend+tosend_begin, end-tosend_begin);
      tosend_begin = end;
      return retval;
    }

    // wrapping.
    std::pair<uint8_t*, uint32_t> retval =
      std::make_pair(tosend+tosend_begin, sizeof(tosend)-tosend_begin);
    tosend_begin = 0;
    return retval;
  }
};

Queue uart_queue;
Queue usb_queue;
Queue uart_rx_queue;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */


void UART_send_from_buffer(Queue &queue)
{

  HAL_UART_StateTypeDef state = HAL_UART_GetState(&huart1);
  if (state != HAL_UART_STATE_READY  &&
      state != HAL_UART_STATE_BUSY_RX) { return; }

  auto deq = queue.dequeue();
  if (deq.second == 0) { return; }

  HAL_UART_Transmit_DMA(&huart1, deq.first, deq.second);

}


// from the terminal that I'm using, the data comes in two chunks
// the first is the data to be send.
// the second is a return and newline
void SendUART(uint8_t *data, uint32_t length)
{
  uart_queue.enqueue(data, length);
  // don't need to add the newline stuff if the stuff we're echoing is
  // already newlined.
  //uint8_t extra[] = "\n\r";
  // need the -1 because sizeof also needs the extra null termination.
  //uart_queue.enqueue(extra, sizeof(extra)-1);
  
  UART_send_from_buffer(uart_queue);

}

void ProcessReceivedUart()
{
  // see page 287 of RM0008
  // CNDTR tells how much is left in the current DMA loop.
  uart_rx_queue.tosend_end = sizeof(uart_rx_queue.tosend) - huart1.hdmarx->Instance->CNDTR;
  
  auto deq = uart_rx_queue.dequeue();

  while(CDC_GetTxState()) { /* empty */ }
  CDC_Transmit_FS(deq.first, deq.second);
  
}

// to talk to UART minicom --baudrate 115200 --device /dev/ttyUSB0 
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  HAL_UART_StateTypeDef state = HAL_UART_GetState(huart);
  if (state == HAL_UART_STATE_READY) {

  } else {

  }

  UART_send_from_buffer(uart_queue);
}

void HAL_UART_RxCpltCallback (UART_HandleTypeDef *huart)
{
  uart_rx_queue.num_rounds++;

  // don't need to do anything. DMA is circular

# if 0
  // this is what we use if not dma
  __attribute__((unused)) HAL_StatusTypeDef state;
  
  // -3 because of \n \r and 0x0 to end the string
  state = HAL_UART_Receive_IT(&huart1, received, sizeof(received)-3);
#endif
  
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
  MX_USB_DEVICE_Init();

  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_DMA(&huart1, uart_rx_queue.tosend, sizeof(uart_rx_queue.tosend));
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  uint32_t lasttime = 0;
  uint8_t uartmessage[] = "periodic uart\n";
  uint8_t usbmessage[]  = "periodic usb\n";
  UNUSED(uartmessage);
  UNUSED(usbmessage);
  
  uint8_t  blinkstate = 0;
  while (1)
  {
    uint32_t curtime = HAL_GetTick();
    if (((curtime % 1000) != 0) || (curtime == lasttime)) {
      continue;
    }
    lasttime = curtime;

    HAL_UART_StateTypeDef state = HAL_UART_GetState(&huart1);
    if (state == HAL_UART_STATE_READY  ||
        state == HAL_UART_STATE_BUSY_RX) {
      //HAL_UART_Transmit_IT(&huart1, uartmessage, sizeof(uartmessage));
    }
    
    ProcessReceivedUart();
    
    if (blinkstate) {
      GPIOC->ODR |= GPIO_PIN_13;
    } else {
      GPIOC->ODR &= ~(GPIO_PIN_13);
    }
    blinkstate = !blinkstate;
    
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

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

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
void _Error_Handler(const char * file, int line)
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
