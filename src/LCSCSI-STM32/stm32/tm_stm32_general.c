/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#include "tm_stm32_general.h"

/* System speed in MHz */
uint16_t GENERAL_SystemSpeedInMHz = 0;
static uint16_t InterruptDisabledCount = 0;

/* Private functions */
static uint32_t x_na_y(uint32_t x, uint8_t y) {
	uint32_t output = 1;
	
	/* Make a "power" multiply */
	while (y--) {
		output *= x;
	}
	
	/* Return output value */
	return output;
}

void TM_GENERAL_DisableInterrupts(void) {
	/* Disable interrupts */
	__disable_irq();
	
	/* Increase number of disable interrupt function calls */
	InterruptDisabledCount++;
}

uint8_t TM_GENERAL_EnableInterrupts(void) {
	/* Decrease number of disable interrupt function calls */
	if (InterruptDisabledCount) {
		InterruptDisabledCount--;
	}
	
	/* Check if we are ready to enable interrupts */
	if (!InterruptDisabledCount) {
		/* Enable interrupts */
		__enable_irq();
	}
	
	/* Return interrupt enabled status */
	return !InterruptDisabledCount;
}

void TM_GENERAL_SystemReset(void) {
	/* Call user callback function */
	TM_GENERAL_SystemResetCallback();
	
	/* Perform a system software reset */
	NVIC_SystemReset();
}

uint32_t TM_GENERAL_GetClockSpeed(TM_GENERAL_Clock_t clock) {
	uint32_t c = 0;

	/* Return clock speed */
	switch (clock) {
		case TM_GENERAL_Clock_HSI:
			c = HSI_VALUE;
			break;
		case TM_GENERAL_Clock_HSE:
			c = HSE_VALUE;
			break;
		case TM_GENERAL_Clock_HCLK:
			c = HAL_RCC_GetHCLKFreq();
			break;
		case TM_GENERAL_Clock_PCLK1:
			c = HAL_RCC_GetPCLK1Freq();
			break;
		case TM_GENERAL_Clock_PCLK2:
			c = HAL_RCC_GetPCLK2Freq();
			break;
		case TM_GENERAL_Clock_SYSCLK:
			c = HAL_RCC_GetSysClockFreq();
			break;
		default:
			break;
	}
	
	/* Return clock */
	return c;
}

TM_GENERAL_ResetSource_t TM_GENERAL_GetResetSource(uint8_t reset_flags) {
	TM_GENERAL_ResetSource_t source = TM_GENERAL_ResetSource_None;

	/* Check bits */
	if (RCC->CSR & RCC_CSR_LPWRRSTF) {
		source = TM_GENERAL_ResetSource_LowPower;
	} else if (RCC->CSR & RCC_CSR_WWDGRSTF) {
		source = TM_GENERAL_ResetSource_WWDG;
#if defined(STM32F4xx)
	} else if (RCC->CSR & RCC_CSR_WDGRSTF) {
#else
	} else if (RCC->CSR & RCC_CSR_IWDGRSTF) {
#endif
		source = TM_GENERAL_ResetSource_IWDG;
	} else if (RCC->CSR & RCC_CSR_SFTRSTF) {
		source = TM_GENERAL_ResetSource_Software;
	} else if (RCC->CSR & RCC_CSR_PORRSTF) {
		source = TM_GENERAL_ResetSource_POR;
	} else if (RCC->CSR & RCC_CSR_BORRSTF) {
		source = TM_GENERAL_ResetSource_BOR;
#if defined(STM32F4xx)
	} else if (RCC->CSR & RCC_CSR_PADRSTF) {
#else
	} else if (RCC->CSR & RCC_CSR_PINRSTF) {
#endif		
		source = TM_GENERAL_ResetSource_PIN;
	}
	
	/* Check for clearing flags */
	if (reset_flags) {
		RCC->CSR = RCC_CSR_RMVF;
	}
	
	/* Return source */
	return source;
}

#if !defined(STM32F0xx)
uint8_t TM_GENERAL_DWTCounterEnable(void) {
	uint32_t c;
	
	/* Set clock speed if not already */
	if (GENERAL_SystemSpeedInMHz == 0) {
		/* Get clock speed in MHz */
		GENERAL_SystemSpeedInMHz = TM_GENERAL_GetClockSpeed(TM_GENERAL_Clock_SYSCLK) / 1000000;
	}
	
    /* Enable TRC */
    CoreDebug->DEMCR &= ~0x01000000;
    CoreDebug->DEMCR |=  0x01000000;

#if defined(STM32F7xx)
    /* Unclock DWT timer */
    DWT->LAR = 0xC5ACCE55;
#endif

    /* Enable counter */
    DWT->CTRL &= ~0x00000001;
    DWT->CTRL |=  0x00000001;
	
    /* Reset counter */
    DWT->CYCCNT = 0;
	
	/* Check if DWT has started */
	c = DWT->CYCCNT;
	
	/* 2 dummys */
	__ASM volatile ("NOP");
	__ASM volatile ("NOP");
	
	/* Return difference, if result is zero, DWT has not started */
	return (DWT->CYCCNT - c);
}
#endif

void TM_GENERAL_ConvertFloat(TM_GENERAL_Float_t* Float_Struct, float Number, uint8_t decimals) {
	/* Check decimals */
	if (decimals > 9) {
		decimals = 9;
	}
	
	/* Get integer part */
	Float_Struct->Integer = (int32_t)Number;
	
	/* Get decimal part */
	if (Number < 0) {
		Float_Struct->Decimal = (int32_t)((float)(Float_Struct->Integer - Number) * x_na_y(10, decimals));
	} else {
		Float_Struct->Decimal = (int32_t)((float)(Number - Float_Struct->Integer) * x_na_y(10, decimals));
	}
}

float TM_GENERAL_RoundFloat(float Number, uint8_t decimals) {
	float x;
		
	/* Check decimals */
	if (decimals > 9) {
		decimals = 9;
	}
	
	x = x_na_y(10, decimals);
	
	/* Make truncating */
	if (Number > 0) {
		return (float)(Number * x + (float)0.5) / x;
	} 
	if (Number < 0) {
		return (float)(Number * x - (float)0.5) / x;
	}
	
	/* Return number */
	return 0;
}

uint32_t TM_GENERAL_NextPowerOf2(uint32_t number) {
	/* Check number */
	if (number <= 1) {
		return 1;
	}
	
	/* Do some bit operations */
	number--;
	number |= number >> 1;
	number |= number >> 2;
	number |= number >> 4;
	number |= number >> 8;
	number |= number >> 16;
	number++;
	
	/* Return calculated number */
	return number;
}

void TM_GENERAL_ForceHardFaultError(void) {
	/* Create hard-fault-function typedef */
	typedef void (*hff)(void);
	hff hf_func = 0;
	
	/* Call function at zero location in memory = HARDFAULT */
	hf_func();
}

__weak void TM_GENERAL_SystemResetCallback(void) {
	/* NOTE: This function should not be modified, when the callback is needed,
            the TM_GENERAL_SystemResetCallback could be implemented in the user file
	*/
}

