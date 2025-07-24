/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include <stdio.h>
#include "fatfs.h"
#include "fatfs_sd.h"
#include "fatfs_storage.h"
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

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
char* pDirectoryFiles[MAX_BMP_FILES];
uint8_t str[20];
FRESULT fres;
FIL fil;
uint8_t fl_file_opened;
FATFS SD_FatFs;  /* File system object for SD card logical drive */
char SD_Path[4]; /* SD card logical drive path */

uint8_t DemoIndex = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
static void SDCard_Config(void);
static void SDCard_Test(void);
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
	BSP_DemoTypedef  BSP_examples[]=
{
  {Demo_DrawGraphic, 	"DrawGraphic", 	0},
  {Demo_ShowImages, 	"ShowImages", 	0},
  {Demo_Touchscreen, 	"Touchscreen", 	0},
	{Demo_SDwriteFile, 	"SDwriteFile", 	0},
};
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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	MX_FATFS_Init();
	HAL_Delay(100);
	driver_init();	// LCD Init
	
	/* Configure SD card */
	SDCard_Config();
	
	/* Initialize User_Button on STM32H7xx-Nucleo ------------------*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
	
	lcd_display_string(50, 16, (const uint8_t *)"STM32F411RE-NUCLEO", 16, RED);
	lcd_display_string(20, 48, (const uint8_t *)"Drivers examples:", 16, RED);
	for(uint8_t i=0; i<4; i++)
  {
		lcd_display_string(50, 64+(16*i), (const uint8_t *)BSP_examples[DemoIndex].DemoName, 16, RED);
		DemoIndex++;
  }
	lcd_display_string(20, 160, (const uint8_t *)"Press User Button to start", 16, RED);
	
	/* 0. Wait for User button to be pressed -------------------------------------*/
  while(CheckForUserInput() == 0)
  {}
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	DemoIndex = 0;
  while (1)
  {
    /* USER CODE END WHILE */
		BSP_examples[DemoIndex].DemoFunc();
		HAL_Delay(10);
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 400;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
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
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/* LCD GPIO Configuration
	PB6      ------> LCD_CS
	PA8 		 ------> LCD_DC
	PC7      ------> LCD_BL
	*/
	GPIO_InitStruct.Pin = LCD_CS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LCD_CS_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LCD_DC_PIN;
	HAL_GPIO_Init(LCD_DC_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LCD_BKL_PIN;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_BKL_GPIO, &GPIO_InitStruct);

	/* SD GPIO Configuration
	PB4      ------> SD_CS
	*/
	GPIO_InitStruct.Pin = SD_CS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStruct);

	/* Touch GPIO Configuration
	PB5      ------> XPT2046_CS
	PB3      ------> XPT2046_IRQ
	*/
	GPIO_InitStruct.Pin = XPT2046_CS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(XPT2046_CS_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = XPT2046_IRQ_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(XPT2046_IRQ_GPIO, &GPIO_InitStruct);
	
	/*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_CS_GPIO, LCD_CS_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(XPT2046_CS_GPIO, XPT2046_CS_PIN, GPIO_PIN_SET);
}

/* USER CODE BEGIN 4 */
/**
  * @brief  Output the image on LCD from SD.
  * @param  None
  * @retval None
  */
static void display_images(void)
{
	  uint32_t bmplen = 0x00;
    uint32_t checkstatus = 0x00;
    uint32_t filesnumbers = 0x00;
    uint32_t bmpcounter = 0x00;
    DIR directory;
    FRESULT res;
	/* Open directory */
    res= f_opendir(&directory, "/");
    if((res != FR_OK))
    {
      if(res == FR_NO_FILESYSTEM)
      {
        /* Display message: SD card not FAT formated */
        lcd_display_string(0, 32, (const uint8_t *)"SD_CARD_NOT_FORMATTED", 16, RED);  
          
      }
      else
      {
        /* Display message: Fail to open directory */
         lcd_display_string(0, 48, (const uint8_t *)"SD_CARD_OPEN_FAIL", 16, RED);           
      }
    }
    
    /* Get number of bitmap files */
    filesnumbers = Storage_GetDirectoryBitmapFiles ("/", pDirectoryFiles);    
    
		/* Set bitmap counter to display first image */
    bmpcounter = 1; 
    
    while (1)
    {     
			sprintf((char*)str, "%-11.11s", pDirectoryFiles[bmpcounter -1]);
			checkstatus = Storage_CheckBitmapFile((const char*)str, &bmplen);
			
			if(checkstatus == 0)
			{
				/* Format the string */
				Storage_OpenReadFile(0, 0, (const char*)str); 
			}
			else if (checkstatus == 1)
			{
				/* Display message: SD card does not exist */
				 lcd_display_string(0, 64, (const uint8_t *)"SD_CARD_NOT_FOUND", 16, RED);  
			}
			else
			{
				/* Display message: File not supported */
					lcd_display_string(0, 80, (const uint8_t *)"SD_CARD_FILE_NOT_SUPPORTED", 16, RED); 
			}
			
			bmpcounter ++;
			if(bmpcounter > filesnumbers)
			{
				bmpcounter = 1;
			}
			
			HAL_Delay(5000);
			
			if(DemoIndex>1){
				DemoIndex = 2;
				break;
			}
    }
}


/**
  * @brief  Configuration the SDcard.
  * @param  None
  * @retval None
  */
static void SDCard_Config(void)
{
  uint32_t counter = 0;
	
	/* Check the mounted device */
	if(f_mount(&SD_FatFs, (TCHAR const*)"/", 0) != FR_OK)
	{
		lcd_display_string(0, 16, (const uint8_t *)"FATFS_NOT_MOUNTED", 16, RED);
	}
	else
	{
		/* Initialize the Directory Files pointers (heap) */
		for (counter = 0; counter < MAX_BMP_FILES; counter++)
		{
			pDirectoryFiles[counter] = malloc(11);
		}
	}
}


/**
  * @brief  Test to write file on SDcard.
  * @param  None
  * @retval None
  */
static void SDCard_Test(void)
{
		fres = f_open(&fil, "log.txt", FA_OPEN_APPEND | FA_WRITE | FA_READ);
		if (fres == FR_OK) {
			lcd_display_string(0, 16, (const uint8_t *)"File log.txt opened", 16, BLACK);
		} else if (fres != FR_OK) {
			lcd_display_string(0, 16, (const uint8_t *)"FILE_NOT_OPENED", 16, RED);
		}
		
		for (uint8_t i = 0; i < 10; i++) {
			f_puts("This text is written in the file.\n", &fil);
		}
		
		if (fres == FR_OK) {
			lcd_display_string(0, 32, (const uint8_t *)"Text written in the file", 16, BLACK);
		}
		
		/* Close file */
		fres = f_close(&fil);
		if (fres == FR_OK) {
			lcd_display_string(0, 48, (const uint8_t *)"File log.txt closed", 16, BLACK);
		} else if (fres != FR_OK) {
			lcd_display_string(0, 48, (const uint8_t *)"FILE_NOT_CLOSED", 16, RED);
		}
		
		lcd_display_string(0, 300, (const uint8_t *)"Press User Button to continue", 16, BLACK);
		
		while(1){
			if(CheckForUserInput()){
				DemoIndex = 0;
				break;
			}
		}
}

/**
  * @brief  Demo Draw Graphic.
  * @param  None
  * @retval None
  */
void Demo_DrawGraphic(void)
{
	lcd_clear_screen(BLACK);
	
	lcd_display_string(60, 120, (const uint8_t *)"Hello, world !", FONT_1608, WHITE);
  lcd_display_string(30, 152, (const uint8_t *)"2.8' TFT Touch Shield", FONT_1608, WHITE);
	
	lcd_draw_circle(50,50,30,BLUE);
	lcd_fill_rect(160,200,60,100,GREEN);
	lcd_draw_rect(80,90,100,100,YELLOW);
	lcd_draw_h_line(18,180,200,RED);
	lcd_draw_v_line(30,120,100,GRAY);
	
	lcd_display_string(0, 300, (const uint8_t *)"Press User Button to continue", 16, WHITE);
	
	while(1){
		if(CheckForUserInput()){
			DemoIndex = 1;
			break;
		}
	}
}

/**
  * @brief  Demo Show Images.
  * @param  None
  * @retval None
  */
void Demo_ShowImages(void)
{
	lcd_clear_screen(WHITE);
	display_images();
}

/**
  * @brief  Demo Touchscreen.
  * @param  None
  * @retval None
  */
static void Demo_Touchscreen(void)
{
	lcd_clear_screen(WHITE);
	
	/* Slow SPI speed  */
	SET_SPI_BAUDRATEPRESCALER_128;
	/*-----------------*/
	
	tp_init();
	tp_adjust();
  tp_dialog();
	
	while(1){
		tp_draw_board();
		if(CheckForUserInput()){
			/* Fast SPI speed  */
			SET_SPI_BAUDRATEPRESCALER_16;
			/*-----------------*/
			DemoIndex = 3;
			break;
		}
	}
}

/**
  * @brief  Demo microSD write File.
  * @param  None
  * @retval None
  */
static void Demo_SDwriteFile(void)
{
	lcd_clear_screen(WHITE);
	SDCard_Test();
}


/**
  * @brief  Check for user input.
  * @param  None
  * @retval Input state (1 : active / 0 : Inactive)
  */
uint8_t CheckForUserInput(void)
{
  if(BSP_PB_GetState(BUTTON_USER) == BUTTON_PRESSED)
  {
    HAL_Delay(10);
    while (BSP_PB_GetState(BUTTON_USER) == BUTTON_PRESSED);
		return 1 ;
  }
  return 0;
}


/**
  * @brief  BSP Push Button callback
  * @param  Button Specifies the pin connected EXTI line
  * @retval None
  */
void BSP_PB_Callback(Button_TypeDef Button){
	DemoIndex++;
	if(DemoIndex > 3) DemoIndex = 0;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
