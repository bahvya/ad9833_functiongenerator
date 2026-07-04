/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ad9833.h"
#include "cmsis_os2.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdint.h>
#include <sys/_intsup.h>
#include "lcd.h"
#include <stdio.h>
volatile float frequency_rtry=1;
volatile uint8_t i =0;
volatile uint8_t mode=1;
volatile float increment[]={1,10,100,1000,10000,100000};
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

/* Definitions for button */
osThreadId_t buttonHandle;
const osThreadAttr_t button_attributes = {
  .name = "button",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ad */
osThreadId_t adHandle;
const osThreadAttr_t ad_attributes = {
  .name = "ad",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for screen */
osThreadId_t screenHandle;
const osThreadAttr_t screen_attributes = {
  .name = "screen",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
void buttons(void *argument);
void ad_spi(void *argument);
void lcd(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,GPIO_PIN_RESET);
  ad_setup();
  HAL_Delay(1000);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13,GPIO_PIN_SET);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of button */
  buttonHandle = osThreadNew(buttons, NULL, &button_attributes);

  /* creation of ad */
  adHandle = osThreadNew(ad_spi, NULL, &ad_attributes);

  /* creation of screen */
  screenHandle = osThreadNew(lcd, NULL, &screen_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3|en_Pin|d4_Pin|d5_Pin
                          |d6_Pin|d7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : A_rtry_Pin B_rtry_Pin PB15 */
  GPIO_InitStruct.Pin = A_rtry_Pin|B_rtry_Pin|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 en_Pin d4_Pin d5_Pin
                           d6_Pin d7_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_3|en_Pin|d4_Pin|d5_Pin
                          |d6_Pin|d7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_buttons */
/**
  * @brief  Function implementing the button thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_buttons */
void buttons(void *argument)
{
  /* USER CODE BEGIN 5 */
  
 

    GPIO_PinState prevA = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12);
    

    for(;;)
    {
        GPIO_PinState A = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12);

        // Falling edge of A
        if ((prevA == GPIO_PIN_SET) && (A == GPIO_PIN_RESET))
        {
            GPIO_PinState B = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13);

            if (B == GPIO_PIN_SET)
            {
                frequency_rtry+= increment[i];      // Clockwise
                ad_frequency(frequency_rtry);
                if(frequency_rtry>6000000)
                {
                  frequency_rtry=6000000;
                }
                
                
                
            }
            else
            {
                frequency_rtry-=increment[i];      // Counter-clockwise
                ad_frequency(frequency_rtry);
                if(frequency_rtry<=0)
                {
                  frequency_rtry=1;
                }
            }


            osDelay(10);
        }
        prevA = A;
        osDelay(1);
      }

  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_ad_spi */
/**
* @brief Function implementing the ad thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ad_spi */
void ad_spi(void *argument)
{
  /* USER CODE BEGIN ad_spi */
  /* Infinite loop */
  
  
  
  GPIO_PinState prevBtn = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);
  GPIO_PinState prevBtn_mode = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);
  for(;;)
  {
    GPIO_PinState btn = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);
    GPIO_PinState btn_mode = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);
    if ((prevBtn == GPIO_PIN_SET) && (btn == GPIO_PIN_RESET))
      {
        i++;
        if(i>5)
        {
          i=0;
            
        }
        osDelay(700);
      }
    if ((prevBtn_mode == GPIO_PIN_SET) && (btn_mode == GPIO_PIN_RESET))
      {
        mode++;
        if(mode>3)
        {
          mode=1;
          
        }
        ad_mode_select(mode);
        osDelay(700);
      }
    
    osDelay(1);
    
  }
  /* USER CODE END ad_spi */
}

/* USER CODE BEGIN Header_lcd */
/**
* @brief Function implementing the screen thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_lcd */
void lcd(void *argument)
{
  /* USER CODE BEGIN lcd */
  LCD_Init();
  LCD_Clear();
  LCD_SetCursor(0, 14);
  char hz[3];
  snprintf(hz, sizeof(hz), "hz");
  char mode1[]="sine";
  char mode2[]="triangle";
  char mode3[]="square";
  char tenraisedto[]="+10^";
  
  /* Infinite loop */
  for(;;)
  {
    char freq[16];
    snprintf(freq, sizeof(freq), "%d", (int)frequency_rtry);
    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_PrintString(freq);
    LCD_SetCursor(0, 14);
    LCD_PrintString(hz); 
    LCD_SetCursor(1, 0);
    if(mode==1)
    {
      
      LCD_PrintString(mode1);
    }
    else if (mode==2) {
      LCD_PrintString(mode2);
    }
    else if (mode==3) {
      LCD_PrintString(mode3);
    }
    LCD_SetCursor(1, 11);
    LCD_PrintString(tenraisedto);

    LCD_SetCursor(1, 15);
    char incr[16];
    snprintf(incr, sizeof(incr), "%d", (int)i);
    LCD_PrintString(incr);
    osDelay(700);
  }
  /* USER CODE END lcd */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
