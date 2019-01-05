/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.net
 * @link    http://stm32f4-discovery.net/2015/07/hal-library-09-general-for-stm32fxxx/
 * @version v1.1
 * @ide     Keil uVision
 * @license MIT
 * @brief   GENERAL library for STM32F4xx devices
 *	
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef TM_GENERAL_H
#define TM_GENERAL_H 110

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup TM_STM32Fxxx_HAL_Libraries
 * @{
 */

/**
 * @defgroup TM_GENERAL
 * @brief    GENERAL library for STM32Fxxx devices - http://stm32f4-discovery.net/2015/07/hal-library-09-general-for-stm32fxxx/
 * @{
 *
 * This is a simple library, where its content can be used in a lot of places in your project.
 *
 *
 * \par Supported actions with library
 *
\verbatim
- Support for software reset with callback
- Get reset source, what resets your MCU
- Disable or enable global NVIC interrupts
- Get different clock speeds in your system
- Operate with Cortex-M4/z DWT hardware counter
- Check if number is odd or even and if it is power of 2
- Round float number with selectable decimal places
- Convert float number to integer and decimal part with selectable decimal places
\endverbatim
 *
 * \par Changelog
 *
\verbatim
 Version 1.0
  - First release

 Version 1.1
  - July 12, 2016
  - DWT counter did not start on STM32F7xx devices
\endverbatim
 *
 * \par Dependencies
 *
\verbatim
 - STM32Fxxx HAL
 - defines.h
 - attributes.h
\endverbatim
 */

#include "stm32fxxx_hal.h"
#include "defines.h"
#include "attributes.h"

/**
 * @defgroup TM_GENERAL_Macros
 * @brief    Library defines
 * @{
 */

/**
 * @brief  System speed in MHz
 */
extern uint16_t GENERAL_SystemSpeedInMHz;

/**
 * @}
 */
 
/**
 * @defgroup TM_GENERAL_Variables
 * @brief    Library variables
 * @{
 */
 
/**
 * @}
 */
 
/**
 * @defgroup TM_GENERAL_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  Clock speed enumeration
 */
typedef enum {
	TM_GENERAL_Clock_HSI,    /*!< High speed internal clock */
	TM_GENERAL_Clock_HSE,    /*!< High speed external clock */
	TM_GENERAL_Clock_SYSCLK, /*!< System core clock */
	TM_GENERAL_Clock_PCLK1,  /*!< PCLK1 (APB1) peripheral clock */
	TM_GENERAL_Clock_PCLK2,  /*!< PCLK2 (APB2) peripheral clock */
	TM_GENERAL_Clock_HCLK    /*!< HCLK (AHB1) high speed clock */
} TM_GENERAL_Clock_t;

/**
 * @brief  All possible reset sources
 */
typedef enum {
	TM_GENERAL_ResetSource_None = 0x00,     /*!< No reset source detected. Flags are already cleared */
	TM_GENERAL_ResetSource_LowPower = 0x01, /*!< Low-power management reset occurs */
	TM_GENERAL_ResetSource_WWDG = 0x02,     /*!< Window watchdog reset occurs */
	TM_GENERAL_ResetSource_IWDG = 0x03,     /*!< Independent watchdog reset occurs */
	TM_GENERAL_ResetSource_Software = 0x04, /*!< Software reset occurs */
	TM_GENERAL_ResetSource_POR = 0x05,      /*!< POR/PDR reset occurs */
	TM_GENERAL_ResetSource_PIN = 0x06,      /*!< NRST pin is set to low by hardware reset, hardware reset */
	TM_GENERAL_ResetSource_BOR = 0x07,      /*!< BOR reset occurs */
} TM_GENERAL_ResetSource_t;

/**
 * @brief  Float number operation structure
 */
typedef struct {
	int32_t Integer;  /*!< Integer part of float number */
	uint32_t Decimal; /*!< Decimal part of float number */
} TM_GENERAL_Float_t;

/**
 * @}
 */

/**
 * @defgroup TM_GENERAL_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Performs a system reset
 * @note   Before system will be reset, @ref TM_GENERAL_SoftwareResetCallback() will be called,
 *         where you can do important stuff if necessary
 * @param  None
 * @retval None
 */
void TM_GENERAL_SystemReset(void);

/**
 * @brief  Gets reset source why system was reset
 * @param  reset_flags: After read, clear reset flags
 *            - 0: Flags will stay untouched
 *            - > 0: All reset flags will reset
 * @retval Member of @ref TM_GENERAL_ResetSource_t containing reset source
 */
TM_GENERAL_ResetSource_t TM_GENERAL_GetResetSource(uint8_t reset_flags);

/**
 * @brief  Disables all interrupts in system
 * @param  None
 * @retval None
 */
void TM_GENERAL_DisableInterrupts(void);

/**
 * @brief  Enables interrupts in system.
 * @note   This function has nesting support. This means that if you call @ref TM_GENERAL_DisableInterrupts() 4 times,
 *         then you have to call this function also 4 times to enable interrupts.
 * @param  None
 * @retval Interrupt enabled status:
 *            - 0: Interrupts were not enabled
 *            - > 0: Interrupts were enabled
 */
uint8_t TM_GENERAL_EnableInterrupts(void);

/**
 * @brief  Checks if code execution is inside active IRQ
 * @param  None
 * @retval IRQ Execution status:
 *            - 0: Code execution is not inside IRQ, thread mode
 *            - > 0: Code execution is inside IRQ, IRQ mode
 * @note   Defines as macro for faster execution
 */
#define TM_GENERAL_IsIRQMode()               (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk)

/**
 * @brief  Gets specific clock speed value from STM32F4xx device
 * @param  clock: Clock type you want to know speed for. This parameter can be a value of @ref TM_GENERAL_Clock_t enumeration
 * @retval Clock speed in units of hertz
 */
uint32_t TM_GENERAL_GetClockSpeed(TM_GENERAL_Clock_t clock);

/**
 * @brief  Gets system clock speed in units of MHz
 * @param  None
 * @retval None
 * @note   Defined as macro for faster execution
 */
#define TM_GENERAL_GetSystemClockMHz()       ((uint16_t)(SystemCoreClock * (float)0.000001))

/**
 * @brief  Enables DWT counter in Cortex-M4 core
 * @param  None
 * @retval DWT Status:
 *            - 0: DWT has not started, hardware/software reset is required
 *            - > 0: DWT has started and is ready to use
 * @note   It may happen, that DWT counter won't start after reprogramming device.
 *         This happened to me when I use onboard ST-Link on Discovery or Nucleo boards.
 *         When I used external debugger (J-Link or ULINK2) it worked always without problems.
 *         If your DWT doesn't start, you should perform software/hardware reset by yourself.
 */
uint8_t TM_GENERAL_DWTCounterEnable(void);

/**
 * @brief  Disables DWT counter in Cortex-M4 core
 * @param  None
 * @retval None
 * @note   Defined as macro for faster execution
 */
#if !defined(STM32F0xx)
#define TM_GENERAL_DWTCounterDisable()       (DWT->CTRL &= ~0x00000001)
#endif


/**
 * @brief  Gets current DWT counter value
 * @param  None
 * @retval DWT counter value
 * @note   Defined as macro for faster execution
 */
#define TM_GENERAL_DWTCounterGetValue()      (DWT->CYCCNT)

/**
 * @brief  Sets DWT counter value
 * @param  x: Value to be set to DWT counter
 * @retval None
 * @note   Defined as macro for faster execution
 */
#define TM_GENERAL_DWTCounterSetValue(x)     (DWT->CYCCNT = (x))

/**
 * @brief  Delays for amount of microseconds using DWT counter
 * @param  micros: Number of micro seconds for delay 
 * @retval None
 * @note   DWT Counter HAVE to be initialized first using @ref TM_GENERAL_EnableDWTCounter()
 */
static __INLINE void TM_GENERAL_DWTCounterDelayus(uint32_t micros) {
	uint32_t c = TM_GENERAL_DWTCounterGetValue();
	
	/* Calculate clock cycles */
	micros *= (SystemCoreClock / 1000000);
	micros -= 12;
	
	/* Wait till done */
	while ((TM_GENERAL_DWTCounterGetValue() - c) < micros);
}

/**
 * @brief  Delays for amount of milliseconds using DWT counter
 * @param  millis: Number of micro seconds for delay 
 * @retval None
 * @note   DWT Counter HAVE to be initialized first using @ref TM_GENERAL_EnableDWTCounter()
 */
static __INLINE void TM_GENERAL_DWTCounterDelayms(uint32_t millis) {
	uint32_t c = TM_GENERAL_DWTCounterGetValue();
	
	/* Calculate clock cycles */
	millis *= (SystemCoreClock / 1000);
	millis -= 12;
	
	/* Wait till done */
	while ((TM_GENERAL_DWTCounterGetValue() - c) < millis);
}

/**
 * @brief  Checks if number is odd or even
 * @param  number: Number to check if it is odd or even
 * @retval Is number even status:
 *            - 0: Number is odd
 *            - > 0: Number is even
 * @note   Defined as macro for faster execution
 */
#define TM_GENERAL_IsNumberEven(number)          ((number & 1) == 0)

/**
 * @brief  Converts float coded number into integer and decimal part
 * @param  *Float_Struct: Pointer to empty @ref TM_GENERAL_Float_t to store result into
 * @param  Number: Float number to convert
 * @param  decimals: Number of decimal places for conversion, maximum 9 decimal places
 * @retval None
 * @note   Example: You have number 15.002 in float format.
 *            - You want to split this to integer and decimal part with 6 decimal places.
 *            - Call @ref TM_GENERAL_ConvertFloat(&Float_Struct, 15.002, 6);
 *            - Result will be: Integer: 15; Decimal: 2000 (0.002 * 10^6)
 */
void TM_GENERAL_ConvertFloat(TM_GENERAL_Float_t* Float_Struct, float Number, uint8_t decimals);

/**
 * @brief  Round float number to nearest number with custom number of decimal places
 * @param  Number: Float number to round
 * @param  decimals: Number of decimal places to round, maximum 9 decimal places
 * @retval Rounded float number
 */
float TM_GENERAL_RoundFloat(float Number, uint8_t decimals);

/**
 * @brief  Checks if number is power of 2
 * @note   It can be used to determine if number has more than just one bit set
 *         If only one bit is set, function will return > 0 because this is power of 2.
 * @param  number: Number to check if it is power of 2
 * @retval Is power of 2 status:
 *            - 0: Number if not power of 2
 *            - > 0: Number is power of 2
 * @note   Defined as macro for faster execution
 */
#define TM_GENERAL_IsNumberPowerOfTwo(number)    (number && !(number & (number - 1)))

/**
 * @brief  Calculates next power of 2 from given number
 * @param  number: Input number to be calculated
 * @retval Number with next power of 2
 *         Example:
 *            - Input number: 450
 *            - Next power of 2 is: 512 = 2^9
 */
uint32_t TM_GENERAL_NextPowerOf2(uint32_t number);

/**
 * @brief  Forces processor to jump to Hard-fault handler
 * @note   Function tries to call function at zero location in memory which causes hard-fault
 * @param  None
 * @retval None
 */
void TM_GENERAL_ForceHardFaultError(void);

/**
 * @brief  System reset callback.
 * @note   Function is called before software reset occurs.
 * @param  None
 * @retval None
 * @note   With __weak parameter to prevent link errors if not defined by user
 */
void TM_GENERAL_SystemResetCallback(void);

/**
 * @}
 */
 
/**
 * @}
 */
 
/**
 * @}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
