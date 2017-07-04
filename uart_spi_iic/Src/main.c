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
#include "OLED.h"

/* USER CODE BEGIN Includes */

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

void byte_at_string(uint8_t *str, int i)
{
  
  str[2] = '0' + i%10;
  str[1] = '0' + (i/10)%10;
  str[0] = '0' + (i/100)%10;
  
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

  // this was a short attempt to reset i2c busy issue. it didn't work.
  // I probably just didn't take it far enough. "for now", will just
  // make sure I2C clock is enabled before the rest of i2c. The boot issue
  // is what I'm trying to solve today and I just want to go on.
  //SET_BIT(hi2c1.Instance->CR1, I2C_CR1_SWRST);
  //HAL_I2C_Init(&hi2c1);
  
  /* USER CODE BEGIN 2 */
  uint8_t themessage[] = "E.T. is phoning home. Anyone there? Be sure to check spi\n";
  uint8_t theothermessage[] = "Hey. I hate to interrupt, but...\n";
  uint8_t spimessage[] = "This is a secret side message over spi\n";
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t uart_poll = 1;
  uint8_t spi_poll = 1;
  uint8_t text_size = 0;

  InitOled();
  OLED_Fill(0);
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
    // p HAL_I2C_GetState (&hi2c1)
    // p __HAL_I2C_GET_FLAG(&hi2c1, I2C_FLAG_BUSY)


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
        HAL_StatusTypeDef iicstatus __attribute__ ((unused));
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
      
      OLED_ShowStr(0,3, (unsigned char*) "HelTec Automation", (text_size++ % 2));
    }
    

    if (uartstate == HAL_UART_STATE_READY) {
      HAL_StatusTypeDef tstate __attribute__ ((unused));
      if (uart_poll) {
        tstate = HAL_UART_Transmit(&huart1, themessage, sizeof(themessage), HAL_MAX_DELAY);
      } else {
        tstate = HAL_UART_Transmit_IT(&huart1, theothermessage, sizeof(theothermessage));     
      }
      uart_poll = !uart_poll;
      uartstate = HAL_UART_GetState(&huart1);
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
