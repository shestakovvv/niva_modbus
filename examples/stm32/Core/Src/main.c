/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "niva_modbus.h"
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MODBUS_BUFFER_LEN 256
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile bool RX_BUFFER_IS_FIRST_PART = true;
volatile uint8_t* NEW_MESSAGE_RECEIVED = NULL;
volatile size_t NEW_MESSAGE_RECEIVED_LEN = 0;
uint8_t RX_BUFFER[MODBUS_BUFFER_LEN * 2];
uint8_t TX_BUFFER[MODBUS_BUFFER_LEN];

#define DISCRETE_INPUTS_COUNT   100
#define COILS_COUNT             100
#define INPUT_REGISTERS_COUNT   100
#define HOLDING_REGISTERS_COUNT 100
DiscreteInput DISCRETE_INPUTS[DISCRETE_INPUTS_COUNT] = {0};
Coil COILS[COILS_COUNT] = {0};
InputRegister INPUT_REGISTERS[INPUT_REGISTERS_COUNT] = {0};
HoldingRegister HOLDING_REGISTERS[HOLDING_REGISTERS_COUNT] = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static inline uint8_t* rx_buffer_get_active(void) {
  return (RX_BUFFER_IS_FIRST_PART) ? RX_BUFFER : RX_BUFFER + MODBUS_BUFFER_LEN;
}

static inline void rx_init() {
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx_buffer_get_active(), MODBUS_BUFFER_LEN);
}
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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USB_PCD_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  rx_init();

  ModbusServer server = {
      .address = 1,
      .discrete_inputs = DISCRETE_INPUTS,
      .discrete_inputs_count = DISCRETE_INPUTS_COUNT,
      .coils = COILS,
      .coils_count = COILS_COUNT,
      .input_registers = INPUT_REGISTERS,
      .input_registers_count = INPUT_REGISTERS_COUNT,
      .holding_registers = HOLDING_REGISTERS,
      .holding_registers_count = HOLDING_REGISTERS_COUNT,
  };

  server.coils[1] = 1;
  server.coils[5] = 1;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    if (NEW_MESSAGE_RECEIVED != NULL) {
      size_t tx_buffer_len = 0;
      int8_t result = modbus_server_poll(
        &server, 
        NEW_MESSAGE_RECEIVED, 
        NEW_MESSAGE_RECEIVED_LEN, 
        TX_BUFFER, 
        &tx_buffer_len);

      NEW_MESSAGE_RECEIVED = NULL;
      NEW_MESSAGE_RECEIVED_LEN = 0;

      if (server.response_required) {
          if (result != MODBUS_OK) {
            HAL_GPIO_TogglePin(LD9_GPIO_Port, LD9_Pin);
          }
          HAL_UART_Transmit_DMA(&huart1, TX_BUFFER, tx_buffer_len);
          server.holding_registers[3] += 1;
      } else {
        HAL_GPIO_TogglePin(LD8_GPIO_Port, LD8_Pin);
      }
      HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);
    }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */
static inline void set_new_message_received(uint16_t len) {
  NEW_MESSAGE_RECEIVED = rx_buffer_get_active();
  NEW_MESSAGE_RECEIVED_LEN = len;
}

static inline void continue_receive(UART_HandleTypeDef *huart) {
  RX_BUFFER_IS_FIRST_PART = !RX_BUFFER_IS_FIRST_PART;
  uint8_t* rx_buffer = rx_buffer_get_active();
  HAL_UARTEx_ReceiveToIdle_DMA(huart, rx_buffer, MODBUS_BUFFER_LEN);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t len)
{
  set_new_message_received(len);
  continue_receive(huart);
}
/* USER CODE END 4 */

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

#ifdef  USE_FULL_ASSERT
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
