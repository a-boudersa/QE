/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "arm_math.h"
#include "ILI9341_GFX.h"
#include "ILI9341_STM32_Driver.h"
#include <stdio.h>
#include "BOUDISP.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BLOCKSIZE 5120
#define NUMTAPS 12
//#define BG BLACK
//#define FG WHITE
//#define TG WHITE

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t ping[BLOCKSIZE], pong[BLOCKSIZE],i;
q15_t pingo[BLOCKSIZE], pongo[BLOCKSIZE];
q15_t D1i[BLOCKSIZE/2], A1i[BLOCKSIZE/2],D1o[BLOCKSIZE/2], A1o[BLOCKSIZE/2];
q15_t D2i[BLOCKSIZE/4], A2i[BLOCKSIZE/4],D2o[BLOCKSIZE/4], A2o[BLOCKSIZE/4];
q15_t D3i[BLOCKSIZE/8], A3i[BLOCKSIZE/8],D3o[BLOCKSIZE/8], A3o[BLOCKSIZE/8];
q15_t D4i[BLOCKSIZE/16], A4i[BLOCKSIZE/16],D4o[BLOCKSIZE/16], A4o[BLOCKSIZE/16];
q15_t D5i[BLOCKSIZE/32], A5i[BLOCKSIZE/32],D5o[BLOCKSIZE/32], A5o[BLOCKSIZE/32];
q15_t D6i[BLOCKSIZE/64], A6i[BLOCKSIZE/64],D6o[BLOCKSIZE/64], A6o[BLOCKSIZE/64];
q15_t D7i[BLOCKSIZE/128], A7i[BLOCKSIZE/128],D7o[BLOCKSIZE/128], A7o[BLOCKSIZE/128];
q15_t D8i[BLOCKSIZE/256], A8i[BLOCKSIZE/256],D8o[BLOCKSIZE/256], A8o[BLOCKSIZE/256];
q15_t D9i[BLOCKSIZE/512], A9i[BLOCKSIZE/512],D9o[BLOCKSIZE/512], A9o[BLOCKSIZE/512];
q15_t Vtrms,Ef,A9,D[9],Df[2],THDu,THDd,THD;
uint8_t pipongf=2,ext=0;
q15_t offset=0xE000;
static q15_t LPFs[BLOCKSIZE + NUMTAPS - 1];
static q15_t HPFs[BLOCKSIZE + NUMTAPS - 1];
 q15_t LPFc[NUMTAPS]={
	0.1115* 32768.0,  0.4946* 32768.0, 0.7511* 32768.0,  0.3153* 32768.0,
 -0.2263* 32768.0, -0.1298* 32768.0, 0.0975* 32768.0,  0.0275* 32768.0, 
 -0.0316* 32768.0,  0.0006* 32768.0, 0.0048* 32768.0 ,-0.0011* 32768.0
}; 
 q15_t HPFc[NUMTAPS]={
 -0.0011* 32768.0, -0.0048* 32768.0,  0.0006* 32768.0,  0.0316* 32768.0,
  0.0275* 32768.0, -0.0975* 32768.0, -0.1298* 32768.0,  0.2262* 32768.0,
  0.3153* 32768.0, -0.7511* 32768.0,  0.4946* 32768.0, -0.1115* 32768.0,
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

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
	arm_fir_decimate_instance_q15 LPF,HPF;
	arm_fir_decimate_init_q15(&LPF,NUMTAPS,2,&LPFc[0],&LPFs[0],BLOCKSIZE);
	arm_fir_decimate_init_q15(&HPF,NUMTAPS,2,&HPFc[0],&HPFs[0],BLOCKSIZE);
//q15_t source,maxv;
//float32_t	dest,maxf;
//	uint32_t maxi;
//	source=0x0B4C;
//	
//	arm_q15_to_float(&source,&dest,1);
	 
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
HAL_TIM_Base_Start(&htim2);
HAL_ADC_Start_DMA(&hadc1,(uint32_t *)ping,BLOCKSIZE);
ILI9341_Init();
initdisp();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  { if(pipongf==0&&ext==1) //traitement ping
   {
		arm_scale_q15((q15_t *)ping,0,2,pingo,BLOCKSIZE); //mise à l'échelle
		arm_offset_q15 (pingo,offset,pingo,BLOCKSIZE);		//correction de décalage
		 //calcule des détails et approximations
		arm_fir_decimate_fast_q15(&LPF,pingo,A1i,BLOCKSIZE);	arm_fir_decimate_fast_q15(&HPF,pingo,D1i,BLOCKSIZE);
		arm_fir_decimate_fast_q15(&LPF,A1i,A2i,BLOCKSIZE/2);	arm_fir_decimate_fast_q15(&HPF,A1i,D2i,BLOCKSIZE/2);
		arm_fir_decimate_fast_q15(&LPF,A2i,A3i,BLOCKSIZE/4);	arm_fir_decimate_fast_q15(&HPF,A2i,D3i,BLOCKSIZE/4);
		arm_fir_decimate_fast_q15(&LPF,A3i,A4i,BLOCKSIZE/8);	arm_fir_decimate_fast_q15(&HPF,A3i,D4i,BLOCKSIZE/8);
		arm_fir_decimate_fast_q15(&LPF,A4i,A5i,BLOCKSIZE/16);	arm_fir_decimate_fast_q15(&HPF,A4i,D5i,BLOCKSIZE/16);
		arm_fir_decimate_fast_q15(&LPF,A5i,A6i,BLOCKSIZE/32);	arm_fir_decimate_fast_q15(&HPF,A5i,D6i,BLOCKSIZE/32);
		arm_fir_decimate_fast_q15(&LPF,A6i,A7i,BLOCKSIZE/64);	arm_fir_decimate_fast_q15(&HPF,A6i,D7i,BLOCKSIZE/64);
		arm_fir_decimate_fast_q15(&LPF,A7i,A8i,BLOCKSIZE/128);arm_fir_decimate_fast_q15(&HPF,A7i,D8i,BLOCKSIZE/128);
		arm_fir_decimate_fast_q15(&LPF,A8i,A9i,BLOCKSIZE/256);arm_fir_decimate_fast_q15(&HPF,A8i,D9i,BLOCKSIZE/256);
	  // extraction des indices
			
		arm_rms_q15 (pingo, BLOCKSIZE, &Vtrms); //VTRMS valeur efficace vrais
				//les valeurs efficaces des détails 
		arm_rms_q15 (D1i, BLOCKSIZE/2, &D[0]);	arm_rms_q15 (D2i, BLOCKSIZE/4, &D[1]);	arm_rms_q15 (D3i, BLOCKSIZE/8, &D[2]);
		arm_rms_q15 (D4i, BLOCKSIZE/16, &D[3]);	arm_rms_q15 (D5i, BLOCKSIZE/32, &D[4]);	arm_rms_q15 (D6i, BLOCKSIZE/64, &D[5]); 
		arm_rms_q15 (D7i, BLOCKSIZE/128, &D[6]);arm_rms_q15 (D8i, BLOCKSIZE/256, &Df[0]);arm_rms_q15 (D9i, BLOCKSIZE/256, &Df[1]);
				
		 arm_rms_q15 (A9i, BLOCKSIZE/256, &A9); //A9
		 arm_rms_q15(Df,2,&THDd);arm_rms_q15(D,7,&THDu);		 //THD, Ef	
		 updategraph(Vtrms,THDu,A9,THDd,i++);
//		 arm_max_q15(pingo,BLOCKSIZE,&maxv,&maxi);
//		 arm_q15_to_float(&maxv,&maxf,1);
		 ext=0;
	 }
else if(pipongf==1&&ext==1) //traitement pong
   {
		arm_scale_q15((q15_t *)pong,0,2,pongo,BLOCKSIZE);
		arm_offset_q15 (pongo,offset,pongo,BLOCKSIZE);
		arm_fir_decimate_fast_q15(&LPF,pongo,A1o,BLOCKSIZE);arm_fir_decimate_fast_q15(&HPF,pongo,D1o,BLOCKSIZE);
		arm_fir_decimate_fast_q15(&LPF,A1o,A2o,BLOCKSIZE/2);arm_fir_decimate_fast_q15(&HPF,A1o,D2o,BLOCKSIZE/2);
		arm_fir_decimate_fast_q15(&LPF,A2o,A3o,BLOCKSIZE/4);arm_fir_decimate_fast_q15(&HPF,A2o,D3o,BLOCKSIZE/4);
		arm_fir_decimate_fast_q15(&LPF,A3o,A4o,BLOCKSIZE/8);arm_fir_decimate_fast_q15(&HPF,A3o,D4o,BLOCKSIZE/8);
		arm_fir_decimate_fast_q15(&LPF,A4o,A5o,BLOCKSIZE/16);arm_fir_decimate_fast_q15(&HPF,A4o,D5o,BLOCKSIZE/16);
		arm_fir_decimate_fast_q15(&LPF,A5o,A6o,BLOCKSIZE/32);arm_fir_decimate_fast_q15(&HPF,A5o,D6o,BLOCKSIZE/32);
		arm_fir_decimate_fast_q15(&LPF,A6o,A7o,BLOCKSIZE/64);arm_fir_decimate_fast_q15(&HPF,A6o,D7o,BLOCKSIZE/64);
		arm_fir_decimate_fast_q15(&LPF,A7o,A8o,BLOCKSIZE/128);arm_fir_decimate_fast_q15(&HPF,A7o,D8o,BLOCKSIZE/128);
		arm_fir_decimate_fast_q15(&LPF,A8o,A9o,BLOCKSIZE/256);arm_fir_decimate_fast_q15(&HPF,A8o,D9o,BLOCKSIZE/256);
		 //indices
		arm_rms_q15 (pongo, BLOCKSIZE, &Vtrms);
		arm_rms_q15 (D1o, BLOCKSIZE/2, &D[0]);	arm_rms_q15 (D2o, BLOCKSIZE/4, &D[1]);	arm_rms_q15 (D3o, BLOCKSIZE/8, &D[2]);
		arm_rms_q15 (D4o, BLOCKSIZE/16, &D[3]);	arm_rms_q15 (D5o, BLOCKSIZE/32, &D[4]);	arm_rms_q15 (D6o, BLOCKSIZE/64, &D[5]); 
		arm_rms_q15 (D7o, BLOCKSIZE/128, &D[6]);arm_rms_q15 (D8o, BLOCKSIZE/256, &Df[0]);arm_rms_q15 (D9o, BLOCKSIZE/256, &Df[1]);
		 				
		//A9=A9o[2]; 
		 arm_rms_q15 (A9o, BLOCKSIZE/256, &A9);
		arm_rms_q15(Df,2,&THDd);arm_rms_q15(D,7,&THDu);		 //THD, Ef
		 		 updategraph(Vtrms,THDu,A9,THDd,i++);
ext=0;
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){

	if(pipongf==0){
		pipongf=1;
		ext=1;
		HAL_ADC_Stop_DMA(&hadc1);
HAL_ADC_Start_DMA(&hadc1,(uint32_t *)ping,BLOCKSIZE);

	}
else{
	pipongf=0;
	ext=1;
	HAL_ADC_Stop_DMA(&hadc1);
HAL_ADC_Start_DMA(&hadc1,(uint32_t *)pong,BLOCKSIZE);
	
}

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
