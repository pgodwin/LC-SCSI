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
#include "tm_stm32_fft.h"
#include "stdlib.h"

/* Array with constants for CFFT module */
/* Requires ARM CONST STRUCTURES files */
const arm_cfft_instance_f32 CFFT_Instances[] = {
	{16, twiddleCoef_16, armBitRevIndexTable16, ARMBITREVINDEXTABLE__16_TABLE_LENGTH},
	{32, twiddleCoef_32, armBitRevIndexTable32, ARMBITREVINDEXTABLE__32_TABLE_LENGTH},
	{64, twiddleCoef_64, armBitRevIndexTable64, ARMBITREVINDEXTABLE__64_TABLE_LENGTH},
	{128, twiddleCoef_128, armBitRevIndexTable128, ARMBITREVINDEXTABLE_128_TABLE_LENGTH},
	{256, twiddleCoef_256, armBitRevIndexTable256, ARMBITREVINDEXTABLE_256_TABLE_LENGTH},
	{512, twiddleCoef_512, armBitRevIndexTable512, ARMBITREVINDEXTABLE_512_TABLE_LENGTH},
	{1024, twiddleCoef_1024, armBitRevIndexTable1024, ARMBITREVINDEXTABLE1024_TABLE_LENGTH},
	{2048, twiddleCoef_2048, armBitRevIndexTable2048, ARMBITREVINDEXTABLE2048_TABLE_LENGTH},
	{4096, twiddleCoef_4096, armBitRevIndexTable4096, ARMBITREVINDEXTABLE4096_TABLE_LENGTH}
};

uint8_t TM_FFT_Init_F32(TM_FFT_F32_t* FFT, uint16_t FFT_Size, uint8_t use_malloc) {
	uint8_t i;
	
	/* Set to zero */
	FFT->FFT_Size = 0;
	FFT->Count = 0;
	
	/* Check for proper pointer value */
	for (i = 0; i < 9; i++) {
		/* Check for proper number */
		if (FFT_Size == CFFT_Instances[i].fftLen) {
			/* Set FFT size */
			FFT->FFT_Size = FFT_Size;
			
			/* Save proper pointer */
			FFT->S = &CFFT_Instances[i];
			
			/* Stop for loop */
			break;
		}
	}
	
	/* Check if fft size valid */
	if (FFT->FFT_Size == 0) {
		/* There is not valid input, return */
		return 1;
	}
	
	/* If malloc selected for allocation, use it */
	if (use_malloc) {
		/* Allocate input buffer */
		FFT->Input = (float32_t *) LIB_ALLOC_FUNC((FFT->FFT_Size * 2) * sizeof(float32_t));
		
		/* Check for success */
		if (FFT->Input == NULL) {
			return 2;
		}
		
		/* Allocate input buffer */
		FFT->Output = (float32_t *) LIB_ALLOC_FUNC(FFT->FFT_Size * sizeof(float32_t));
		
		/* Check for success */
		if (FFT->Output == NULL) {
			/* Deallocate input buffer */
			LIB_FREE_FUNC(FFT->Input);
			
			/* Return error */
			return 3;
		}
		
		/* Malloc used, set flag */
		FFT->UseMalloc = 1;
	}
	
	/* Return OK */
	return 0;
}

void TM_FFT_SetBuffers_F32(TM_FFT_F32_t* FFT, float32_t* InputBuffer, float32_t* OutputBuffer) {
	/* If malloc is used, ignore */
	if (FFT->UseMalloc) {
		return;
	}
	
	/* Set pointers */
	FFT->Input = InputBuffer;
	FFT->Output = OutputBuffer;
}

uint8_t TM_FFT_AddToBuffer(TM_FFT_F32_t* FFT, float32_t sampleValue) {
	/* Return function status */
	return TM_FFT_AddToBufferWithImag(FFT, sampleValue, 0);
}

uint8_t TM_FFT_AddToBufferWithImag(TM_FFT_F32_t* FFT, float32_t Real, float32_t Imag) {
	/* Check if memory available */
	if (FFT->Count < FFT->FFT_Size) {
		/* Add to buffer, real part */
		FFT->Input[2 * FFT->Count] = Real;
		/* Imaginary part set to 0 */
		FFT->Input[2 * FFT->Count + 1] = Imag;
		
		/* Increase count */
		FFT->Count++;
	}
	
	/* Check if buffer full */
	if (FFT->Count >= FFT->FFT_Size) {
		/* Buffer full, samples ready to be calculated */
		return 1;
	}
	
	/* Buffer not full yet */
	return 0;
}

void TM_FFT_Process_F32(TM_FFT_F32_t* FFT) {
	/* Process FFT input data */
	arm_cfft_f32(FFT->S, FFT->Input, 0, 1);
	
	/* Process the data through the Complex Magniture Module for calculating the magnitude at each bin */
	arm_cmplx_mag_f32(FFT->Input, FFT->Output, FFT->FFT_Size);
	
	/* Calculates maxValue and returns corresponding value and index */
	arm_max_f32(FFT->Output, FFT->FFT_Size, &FFT->MaxValue, &FFT->MaxIndex);
	
	/* Reset count */
	FFT->Count = 0;
}

void TM_FFT_Free_F32(TM_FFT_F32_t* FFT) {
	/* Free input buffer */
	if (!FFT->UseMalloc) {
		/* Return, malloc was not used for allocation */
		return;
	}
	
	/* Check input buffer */
	if (FFT->Input) {
		LIB_FREE_FUNC(FFT->Input);
	}
	
	/* Check output buffer */
	if (FFT->Output) {
		LIB_FREE_FUNC(FFT->Output);
	}
}
