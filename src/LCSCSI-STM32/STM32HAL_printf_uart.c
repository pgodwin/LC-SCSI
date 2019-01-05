/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stm32f4xx_hal_usart.h>



/* USER CODE BEGIN 0 */

/* PRINTF REDIRECT to UART BEGIN */
// @see    http://www.keil.com/forum/60531/
// @see    https://stackoverflow.com/questions/45535126/stm32-printf-redirect

struct __FILE {
	int handle;
	/* Whatever you require here. If the only file you are using is */
	/* standard output using printf() for debugging, no file handling */
	/* is required. */
}
;

FILE __stdout;

int fputc(int ch, FILE *f) {
	HAL_UART_Transmit(&uart1, (uint8_t *)&ch, 1, 0xFFFF);
	return ch;
}

int ferror(FILE *f) {
	/* Your implementation of ferror(). */
	return 0;
}
/* PRINTF REDIRECT to UART END */

/* USER CODE END 0 */