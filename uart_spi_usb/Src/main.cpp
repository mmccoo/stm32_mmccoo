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
#include "spi.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

#include <sstream>

/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

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
  MX_USB_DEVICE_Init();

  /* USER CODE BEGIN 2 */
  uint8_t themessage[] = "E.T. is phoning home. Anyone there? Be sure to check spi and usb\n";
  uint8_t theothermessage[] = "Hey. I hate to interrupt, but...\n";
  uint8_t spimessage[] = "This is a secret side message over spi\n";
  uint8_t usbmessage[] = "    this is a usb message\n";

  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t uart_poll = 1;
  uint8_t spi_poll = 1;
  uint8_t usb_cnt = 0;


  uint8_t initialmsg[] = "Stuff I heard from USB: ";
  memcpy(read_data, initialmsg, sizeof(initialmsg));
  read_idx = sizeof(initialmsg);
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
    HAL_UART_StateTypeDef state = HAL_UART_GetState(&huart1);
    if (state == HAL_UART_STATE_READY) {
      HAL_StatusTypeDef tstate __attribute__ ((unused));
      if (uart_poll) {
        tstate = HAL_UART_Transmit(&huart1, themessage, sizeof(themessage), HAL_MAX_DELAY);

        // concurrency issue here. USB may interrup and get more data.
        tstate = HAL_UART_Transmit(&huart1, read_data, read_idx, HAL_MAX_DELAY);
      } else {
        tstate = HAL_UART_Transmit_IT(&huart1, theothermessage, sizeof(theothermessage));     
      }
      uart_poll = !uart_poll;
      state = HAL_UART_GetState(&huart1);
    }

    HAL_SPI_StateTypeDef spistate =  HAL_SPI_GetState (&hspi1);
    if (spistate == HAL_SPI_STATE_READY) {
      HAL_StatusTypeDef hstatus __attribute__ ((unused));
      if (spi_poll) {
        hstatus = HAL_SPI_Transmit (&hspi1, spimessage, sizeof(spimessage), HAL_MAX_DELAY);
      }
      spi_poll = !spi_poll;
      spistate =  HAL_SPI_GetState (&hspi1);
    }

    // before this: -rwxrwxr-x 1 mmccoo mmccoo  24K Jun 30 12:59 uart_spi_usb.bin
    //std::stringstream ss;
    //ss << usbmessage;
    //ss << " ";
    //ss << usb_cnt++;
    //std::string str = ss.str();
    usbmessage[2] = (usb_cnt%10) + '0';
    usbmessage[1] = ((usb_cnt/10)%10) + '0';
    usbmessage[0] = ((usb_cnt/100)%10) + '0';
    usb_cnt++;
    CDC_Transmit_FS(usbmessage, sizeof(usbmessage));
    
    GPIOC->ODR |= GPIO_PIN_13;
    delayLoop();
    delayLoop();
    GPIOC->ODR &= ~(GPIO_PIN_13);
    delayLoop();
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
#pragma GCC diagnostic ignored "-Wwrite-strings"
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
