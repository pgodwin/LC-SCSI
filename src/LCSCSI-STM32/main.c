#include "stm32fxxx_hal.h"
//#include "stm32f4xx_hal.h"

/* Include my libraries here */
#include "defines.h"
#include "tm_stm32_disco.h"
#include "tm_stm32_usart.h"
#include "tm_stm32_delay.h"


#include <stdbool.h>
#include <stdio.h>

#include "debug.h"
#include "scsi.h"
#include "hostadapter.h"
#include "filesystem.h"


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler */
	/* User can add his own implementation to report the HAL error return state */
	while (1) 
	{
	}
	/* USER CODE END Error_Handler */ 
}


int main(void) {
	
	//TM_RCC_InitSystem();
	//HAL_Init();
	SystemInit();
	
	//HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();
	/* Init system clock for maximum system speed */
	TM_RCC_InitSystem();
	
	/* Init HAL layer */
	HAL_Init();
	
	
	
	/* Initialize delays */
	TM_DELAY_Init();
	
	// old value for debug
	uint8_t oldValue = 0x0;
	
	
	TM_USART_Init(USART2, TM_USART_PinsPack_1, 115200);
	
		
	// Initialise the host adapter interface
	hostadapterInitialise();
	
	// Initialise the SD Card and FAT file system functions
	filesystemInitialise();
	
	// Initialise the SCSI emulation
	scsiInitialise();
	
	
	// Main processing loop
	while(1) 
	{
		
		
		// Process the SCSI emulation
		scsiProcessEmulation();
		
		if (oldValue != (uint8_t)GPIOC->IDR)
		{
			debugString("Status\r\n");
			debugString("=================\r\n");
			debugStringInt8Hex_P("BUS: ", hostadapterReadDatabus(), true);
		
			debugStringInt8Hex_P("MSG: ", TM_GPIO_GetInputPinValue(SCSI_MSG_GPIO_Port, SCSI_MSG_Pin), true);
			debugStringInt8Hex_P("BSY: ", TM_GPIO_GetInputPinValue(SCSI_BSY_GPIO_Port, SCSI_BSY_Pin), true);
			debugStringInt8Hex_P("REQ: ", TM_GPIO_GetInputPinValue(SCSI_REQ_GPIO_Port, SCSI_REQ_Pin), true);
			debugStringInt8Hex_P("I/O: ", TM_GPIO_GetInputPinValue(SCSI_I_O_GPIO_Port, SCSI_I_O_Pin), true);
			debugStringInt8Hex_P("C/D: ", TM_GPIO_GetInputPinValue(SCSI_C_D_GPIO_Port, SCSI_C_D_Pin), true);
			debugStringInt8Hex_P("ATN: ", TM_GPIO_GetInputPinValue(SCSI_ATN_Port, SCSI_ATN_Pin), true);
			debugStringInt8Hex_P("SEL: ", TM_GPIO_GetInputPinValue(SCSI_SEL_GPIO_Port, SCSI_SEL_Pin), true);
			debugStringInt8Hex_P("ACK: ", TM_GPIO_GetInputPinValue(SCSI_ACK_GPIO_Port, SCSI_ACK_Pin), true);
		
			debugStringInt8Hex_P("RST: ", TM_GPIO_GetInputPinValue(SCSI_RST_GPIO_Port, SCSI_RST_Pin), true);
		}
		
		oldValue = GPIOC->IDR;
		
		// Did the host reset?
		if(hostadapterReadResetFlag())
		{
			// Reset the host adapter
			hostadapterReset();
			
			// Reset the file system
			filesystemReset();
			
			// Reset the status LED
			//statusledReset();
			
			// Reset the SCSI emulation
			scsiReset();
			
			// Clear the reset condition in the host adapter
			hostadapterWriteResetFlag(false);
		}
	}
	
	
	
	

}

/** System Clock Configuration
	*/
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/**Configure the main internal regulator output voltage 
	*/
	__HAL_RCC_PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

	/**Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = 16;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 16;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/**Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
	                            | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}

	/**Configure the Systick interrupt time 
	*/
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/**Configure the Systick 
	*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}
