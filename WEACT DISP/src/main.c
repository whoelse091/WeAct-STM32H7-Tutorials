/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : ST7735 Hello World
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define LCD_CS_GPIO_Port   GPIOE
#define LCD_CS_Pin         GPIO_PIN_11

#define LCD_DC_GPIO_Port   GPIOE
#define LCD_DC_Pin         GPIO_PIN_13

#define LCD_BL_GPIO_Port   GPIOE
#define LCD_BL_Pin         GPIO_PIN_10

#define LCD_CS_LOW()       HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET)
#define LCD_CS_HIGH()      HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET)

#define LCD_DC_CMD()       HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_RESET)
#define LCD_DC_DATA()      HAL_GPIO_WritePin(LCD_DC_GPIO_Port, LCD_DC_Pin, GPIO_PIN_SET)

#define LCD_BL_ON()        HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_RESET)
#define LCD_BL_OFF()       HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, GPIO_PIN_SET)

#define ST7735_WIDTH   160
#define ST7735_HEIGHT   80
#define ST7735_XSTART    1
#define ST7735_YSTART   26

#define COLOR_BLACK     0x0000
#define COLOR_WHITE     0xFFFF
#define COLOR_RED       0x07FF
#define COLOR_BLUE      0xF800
#define COLOR_GREEN     0x07E0

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

DCMI_HandleTypeDef hdcmi;

SPI_HandleTypeDef hspi4;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI4_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_DCMI_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void ST7735_WriteCommand(uint8_t cmd)
{
  LCD_DC_CMD();
  LCD_CS_LOW();
  HAL_SPI_Transmit(&hspi4, &cmd, 1, HAL_MAX_DELAY);
  LCD_CS_HIGH();
}

static void ST7735_WriteData8(uint8_t data)
{
  LCD_DC_DATA();
  LCD_CS_LOW();
  HAL_SPI_Transmit(&hspi4, &data, 1, HAL_MAX_DELAY);
  LCD_CS_HIGH();
}

static void ST7735_WriteDataBuffer(uint8_t *data, uint16_t size)
{
  LCD_DC_DATA();
  LCD_CS_LOW();
  HAL_SPI_Transmit(&hspi4, data, size, HAL_MAX_DELAY);
  LCD_CS_HIGH();
}

static void ST7735_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
  uint8_t buf[4];
  x0 += ST7735_XSTART; x1 += ST7735_XSTART;
  y0 += ST7735_YSTART; y1 += ST7735_YSTART;

  ST7735_WriteCommand(0x2A);
  buf[0] = (uint8_t)(x0 >> 8); buf[1] = (uint8_t)(x0 & 0xFF);
  buf[2] = (uint8_t)(x1 >> 8); buf[3] = (uint8_t)(x1 & 0xFF);
  ST7735_WriteDataBuffer(buf, 4);

  ST7735_WriteCommand(0x2B);
  buf[0] = (uint8_t)(y0 >> 8); buf[1] = (uint8_t)(y0 & 0xFF);
  buf[2] = (uint8_t)(y1 >> 8); buf[3] = (uint8_t)(y1 & 0xFF);
  ST7735_WriteDataBuffer(buf, 4);

  ST7735_WriteCommand(0x2C);
}

static void ST7735_FillScreen(uint16_t color)
{
  uint32_t pixels = ST7735_WIDTH * ST7735_HEIGHT;
  uint8_t data[2] = { (uint8_t)(color >> 8), (uint8_t)(color & 0xFF) };

  ST7735_SetAddressWindow(0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1);
  LCD_DC_DATA();
  LCD_CS_LOW();
  for (uint32_t i = 0; i < pixels; i++) {
    HAL_SPI_Transmit(&hspi4, data, 2, HAL_MAX_DELAY);
  }
  LCD_CS_HIGH();
}

static void ST7735_Init(void)
{
  ST7735_WriteCommand(0x01); HAL_Delay(150);
  ST7735_WriteCommand(0x11); HAL_Delay(150);

  ST7735_WriteCommand(0xB1);
  uint8_t frm[] = {0x01, 0x2C, 0x2D};
  ST7735_WriteDataBuffer(frm, 3);
  ST7735_WriteCommand(0xB2);
  ST7735_WriteDataBuffer(frm, 3);
  ST7735_WriteCommand(0xB3);
  uint8_t frm3[] = {0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D};
  ST7735_WriteDataBuffer(frm3, 6);

  ST7735_WriteCommand(0xB4); ST7735_WriteData8(0x07);

  ST7735_WriteCommand(0xC0);
  uint8_t pw1[] = {0xA2, 0x02, 0x84}; ST7735_WriteDataBuffer(pw1, 3);
  ST7735_WriteCommand(0xC1); ST7735_WriteData8(0xC5);
  ST7735_WriteCommand(0xC2);
  uint8_t pw3[] = {0x0A, 0x00}; ST7735_WriteDataBuffer(pw3, 2);
  ST7735_WriteCommand(0xC3);
  uint8_t pw4[] = {0x8A, 0x2A}; ST7735_WriteDataBuffer(pw4, 2);
  ST7735_WriteCommand(0xC4);
  uint8_t pw5[] = {0x8A, 0xEE}; ST7735_WriteDataBuffer(pw5, 2);
  ST7735_WriteCommand(0xC5); ST7735_WriteData8(0x14);

  ST7735_WriteCommand(0x3A); ST7735_WriteData8(0x05);
  ST7735_WriteCommand(0x36); ST7735_WriteData8(0x78);

  ST7735_WriteCommand(0xE0);
  uint8_t gp[] = {0x02,0x1C,0x07,0x12,0x37,0x32,0x29,0x2D,0x29,0x25,0x2B,0x39,0x00,0x01,0x03,0x10};
  ST7735_WriteDataBuffer(gp, 16);
  ST7735_WriteCommand(0xE1);
  uint8_t gn[] = {0x03,0x1D,0x07,0x06,0x2E,0x2C,0x29,0x2D,0x2E,0x2E,0x37,0x3F,0x00,0x00,0x02,0x10};
  ST7735_WriteDataBuffer(gn, 16);

  ST7735_WriteCommand(0x13);
  ST7735_WriteCommand(0x29);
  HAL_Delay(100);
}

static void ST7735_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
  if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) return;

  uint16_t hw_x = (ST7735_WIDTH  - 1) - x;
  uint16_t hw_y = (ST7735_HEIGHT - 1) - y;

  uint8_t data[2] = { (uint8_t)(color >> 8), (uint8_t)(color & 0xFF) };
  ST7735_SetAddressWindow(hw_x, hw_y, hw_x, hw_y);
  LCD_DC_DATA();
  LCD_CS_LOW();
  HAL_SPI_Transmit(&hspi4, data, 2, HAL_MAX_DELAY);
  LCD_CS_HIGH();
}

static const uint8_t font_space[7] = {0,0,0,0,0,0,0};
static const uint8_t font_let[7]   = {0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F};

static const uint8_t font_A[7] = {0x0E,0x11,0x11,0x1F,0x11,0x11,0x11};
static const uint8_t font_B[7] = {0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E};
static const uint8_t font_C[7] = {0x0E,0x11,0x10,0x10,0x10,0x11,0x0E};
static const uint8_t font_D[7] = {0x1E,0x11,0x11,0x11,0x11,0x11,0x1E};
static const uint8_t font_E[7] = {0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F};
static const uint8_t font_H[7] = {0x11,0x11,0x11,0x1F,0x11,0x11,0x11};
static const uint8_t font_L[7] = {0x10,0x10,0x10,0x10,0x10,0x10,0x1F};
static const uint8_t font_O[7] = {0x0E,0x11,0x11,0x11,0x11,0x11,0x0E};
static const uint8_t font_R[7] = {0x1E,0x11,0x11,0x1E,0x14,0x12,0x11};
static const uint8_t font_W[7] = {0x11,0x11,0x11,0x15,0x15,0x1B,0x11};

static const uint8_t* ST7735_GetFont5x7(char c)
{
  if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 'A');
  switch (c) {
    case 'A': return font_A;
    case 'B': return font_B;
    case 'C': return font_C;
    case 'D': return font_D;
    case 'E': return font_E;
    case 'H': return font_H;
    case 'L': return font_L;
    case 'O': return font_O;
    case 'R': return font_R;
    case 'W': return font_W;
    case ' ': return font_space;
    default:  return font_let;
  }
}

static void ST7735_DrawStringBig(uint16_t x, uint16_t y, const char *text, uint16_t color, uint16_t bg, uint8_t scale)
{
  uint16_t start_x = x;
  while (*text)
  {
    char c = *text++;
    if (c == '\n') { y += (8 * scale); x = start_x; continue; }

    const uint8_t *glyph = ST7735_GetFont5x7(c);
    for (uint8_t row = 0; row < 7; row++) {
      uint8_t bits = glyph[row];
      for (uint8_t col = 0; col < 5; col++) {
        uint8_t mask = (uint8_t)(1U << (4 - col));
        uint16_t px_color = (bits & mask) ? color : bg;
        for (uint8_t dy = 0; dy < scale; dy++) {
          for (uint8_t dx = 0; dx < scale; dx++) {
            ST7735_DrawPixel(x + col*scale + dx, y + row*scale + dy, px_color);
          }
        }
      }
    }
    x += (6 * scale);
  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

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
  MX_SPI4_Init();
  MX_USART1_UART_Init();
  MX_DCMI_Init();
  /* USER CODE BEGIN 2 */
  LCD_BL_OFF();
  HAL_Delay(100);
  ST7735_Init();
  LCD_BL_ON();

  ST7735_FillScreen(COLOR_BLACK);

  ST7735_DrawStringBig(10, 20, "HELLO", COLOR_WHITE, COLOR_BLACK, 2);
  ST7735_DrawStringBig(10, 40, "WORLD", COLOR_RED,   COLOR_BLACK, 2);
  /* USER CODE END 2 */

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

  /*AXI clock gating */
  RCC->CKGAENR = 0xFFFFFFFF;

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = 64;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 35;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DCMI Initialization Function
  * @param None
  * @retval None
  */
static void MX_DCMI_Init(void)
{

  /* USER CODE BEGIN DCMI_Init 0 */

  /* USER CODE END DCMI_Init 0 */

  /* USER CODE BEGIN DCMI_Init 1 */

  /* USER CODE END DCMI_Init 1 */
  hdcmi.Instance = DCMI;
  hdcmi.Init.SynchroMode = DCMI_SYNCHRO_EMBEDDED;
  hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
  hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
  hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  hdcmi.Init.SyncroCode.FrameEndCode = 0;
  hdcmi.Init.SyncroCode.FrameStartCode = 0;
  hdcmi.Init.SyncroCode.LineStartCode = 0;
  hdcmi.Init.SyncroCode.LineEndCode = 0;
  hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
  hdcmi.Init.ByteSelectMode = DCMI_BSM_ALL;
  hdcmi.Init.ByteSelectStart = DCMI_OEBS_ODD;
  hdcmi.Init.LineSelectMode = DCMI_LSM_ALL;
  hdcmi.Init.LineSelectStart = DCMI_OELS_ODD;
  if (HAL_DCMI_Init(&hdcmi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DCMI_Init 2 */

  /* USER CODE END DCMI_Init 2 */

}

/**
  * @brief SPI4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI4_Init(void)
{

  /* USER CODE BEGIN SPI4_Init 0 */

  /* USER CODE END SPI4_Init 0 */

  /* USER CODE BEGIN SPI4_Init 1 */

  /* USER CODE END SPI4_Init 1 */
  /* SPI4 parameter configuration*/
  hspi4.Instance = SPI4;
  hspi4.Init.Mode = SPI_MODE_MASTER;
  hspi4.Init.Direction = SPI_DIRECTION_2LINES;
  hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi4.Init.NSS = SPI_NSS_SOFT;
  hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi4.Init.CRCPolynomial = 0x0;
  hspi4.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  hspi4.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi4.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi4.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi4.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi4.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi4.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi4.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi4.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi4.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI4_Init 2 */

  /* USER CODE END SPI4_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE10 PE11 PE13 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

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

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  * where the assert_param error has occurred.
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