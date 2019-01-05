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
#include "tm_stm32_usb_device_cdc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USBD_VID                      0x0483
#define USBD_PID                      0x5740
#define USBD_LANGID_STRING            0x409
#define USBD_MANUFACTURER_STRING      "STMicroelectronics"
#define USBD_PRODUCT_HS_STRING        "STM32 Virtual ComPort in HS Mode"
#define USBD_PRODUCT_FS_STRING        "STM32 Virtual ComPort in FS Mode"
#define USBD_CONFIGURATION_HS_STRING  "VCP Config"
#define USBD_INTERFACE_HS_STRING      "VCP Interface"
#define USBD_CONFIGURATION_FS_STRING  "VCP Config"
#define USBD_INTERFACE_FS_STRING      "VCP Interface"

#define DEVICE_ID1                    (0x1FFF7A10)
#define DEVICE_ID2                    (0x1FFF7A14)
#define DEVICE_ID3                    (0x1FFF7A18)

#define USB_SIZ_STRING_SERIAL         0x1A

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
uint8_t *USBD_VCP_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_VCP_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_VCP_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_VCP_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_VCP_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_VCP_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
uint8_t *USBD_VCP_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
#ifdef USB_SUPPORT_USER_STRING_DESC
uint8_t *USBD_VCP_USRStringDesc (USBD_SpeedTypeDef speed, uint8_t idx, uint16_t *length);  
#endif /* USB_SUPPORT_USER_STRING_DESC */  

/* Private variables ---------------------------------------------------------*/
USBD_DescriptorsTypeDef VCP_Desc = {
	USBD_VCP_DeviceDescriptor,
	USBD_VCP_LangIDStrDescriptor, 
	USBD_VCP_ManufacturerStrDescriptor,
	USBD_VCP_ProductStrDescriptor,
	USBD_VCP_SerialStrDescriptor,
	USBD_VCP_ConfigStrDescriptor,
	USBD_VCP_InterfaceStrDescriptor,  
};

/* USB Standard Device Descriptor */
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
	#pragma data_alignment=4   
#endif
__ALIGN_BEGIN uint8_t USBD_DeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END = {
	0x12,                       /* bLength */
	USB_DESC_TYPE_DEVICE,       /* bDescriptorType */
	0x00,                       /* bcdUSB */
	0x02,
	0x00,                       /* bDeviceClass */
	0x00,                       /* bDeviceSubClass */
	0x00,                       /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,           /* bMaxPacketSize */
	LOBYTE(USBD_VID),           /* idVendor */
	HIBYTE(USBD_VID),           /* idVendor */
	LOBYTE(USBD_PID),           /* idVendor */
	HIBYTE(USBD_PID),           /* idVendor */
	0x00,                       /* bcdDevice rel. 2.00 */
	0x02,
	USBD_IDX_MFC_STR,           /* Index of manufacturer string */
	USBD_IDX_PRODUCT_STR,       /* Index of product string */
	USBD_IDX_SERIAL_STR,        /* Index of serial number string */
	USBD_MAX_NUM_CONFIGURATION  /* bNumConfigurations */
}; /* USB_DeviceDescriptor */

/* USB Standard Device Descriptor */
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END = {
	USB_LEN_LANGID_STR_DESC,         
	USB_DESC_TYPE_STRING,       
	LOBYTE(USBD_LANGID_STRING),
	HIBYTE(USBD_LANGID_STRING), 
};

uint8_t USBD_StringSerial[USB_SIZ_STRING_SERIAL] = {
	USB_SIZ_STRING_SERIAL,      
	USB_DESC_TYPE_STRING,    
};

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN uint8_t USBD_StrDesc[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;

/* Private functions ---------------------------------------------------------*/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
static void Get_SerialNum(void);

/* External variable */
extern USBD_CDC_LineCodingTypeDef linecoding[];

/* Buffer structure for received data */
#ifdef USB_USE_FS
TM_BUFFER_t USBD_CDC_Buffer_FS_RX;
TM_BUFFER_t USBD_CDC_Buffer_FS_TX;
uint8_t USBD_CDC_Buffer_Data_FS_RX[USBD_CDC_RECEIVE_BUFFER_SIZE_FS];
uint8_t USBD_CDC_Buffer_Data_FS_TX[USBD_CDC_TRANSMIT_BUFFER_SIZE_FS];
#endif
#ifdef USB_USE_HS
TM_BUFFER_t USBD_CDC_Buffer_HS_RX;
TM_BUFFER_t USBD_CDC_Buffer_HS_TX;
uint8_t USBD_CDC_Buffer_Data_HS_RX[USBD_CDC_RECEIVE_BUFFER_SIZE_HS];
uint8_t USBD_CDC_Buffer_Data_HS_TX[USBD_CDC_TRANSMIT_BUFFER_SIZE_HS];
#endif

/* Returns pointer to RX buffer for USB */
static TM_BUFFER_t* TM_USBD_CDC_INT_GetRXBuffer(TM_USB_t USB_Mode) {
	TM_BUFFER_t* Buffer = 0;
	
#ifdef USB_USE_FS
	if (USB_Mode == TM_USB_FS) {
		Buffer = &USBD_CDC_Buffer_FS_RX;
	}
#endif
#ifdef USB_USE_HS
	if (USB_Mode == TM_USB_HS) {
		Buffer = &USBD_CDC_Buffer_HS_RX;
	}
#endif

	/* Return pointer */
	return Buffer;
}

/* Returns pointer to TX buffer for USB */
static TM_BUFFER_t* TM_USBD_CDC_INT_GetTXBuffer(TM_USB_t USB_Mode) {
	TM_BUFFER_t* Buffer = 0;
	
#ifdef USB_USE_FS
	if (USB_Mode == TM_USB_FS) {
		Buffer = &USBD_CDC_Buffer_FS_TX;
	}
#endif
#ifdef USB_USE_HS
	if (USB_Mode == TM_USB_HS) {
		Buffer = &USBD_CDC_Buffer_HS_TX;
	}
#endif

	/* Return pointer */
	return Buffer;
}

/************************************************/
/*            USER PUBLIC FUNCTIONS             */
/************************************************/
TM_USBD_Result_t TM_USBD_CDC_Init(TM_USB_t USB_Mode) {
#ifdef USB_USE_FS
	/* Init FS mode */
	if (USB_Mode == TM_USB_FS || USB_Mode == TM_USB_Both) {
		/* Init FS */
		USBD_Init(&hUSBDevice_FS, &VCP_Desc, USB_ID_FS);

		/* Add Supported Class */
		USBD_RegisterClass(&hUSBDevice_FS, USBD_CDC_CLASS);

		/* Add CDC Interface Class */
		USBD_CDC_RegisterInterface(&hUSBDevice_FS, &USBD_CDC_fops[0]);
		
		/* Init buffers for TX and RX */
		TM_BUFFER_Init(&USBD_CDC_Buffer_FS_RX, USBD_CDC_RECEIVE_BUFFER_SIZE_FS, USBD_CDC_Buffer_Data_FS_RX);
		TM_BUFFER_Init(&USBD_CDC_Buffer_FS_TX, USBD_CDC_TRANSMIT_BUFFER_SIZE_FS, USBD_CDC_Buffer_Data_FS_TX);
	}
#endif
	
#ifdef USB_USE_HS
	/* Init HS mode */
	if (USB_Mode == TM_USB_HS || USB_Mode == TM_USB_Both) {
		/* Init HS */
		USBD_Init(&hUSBDevice_HS, &VCP_Desc, USB_ID_HS);

		/* Add Supported Class */
		USBD_RegisterClass(&hUSBDevice_HS, USBD_CDC_CLASS);

		/* Add CDC Interface Class */
		USBD_CDC_RegisterInterface(&hUSBDevice_HS, &USBD_CDC_fops[1]);
		
		/* Init buffers for TX and RX */
		TM_BUFFER_Init(&USBD_CDC_Buffer_HS_RX, USBD_CDC_RECEIVE_BUFFER_SIZE_HS, USBD_CDC_Buffer_Data_HS_RX);
		TM_BUFFER_Init(&USBD_CDC_Buffer_HS_TX, USBD_CDC_TRANSMIT_BUFFER_SIZE_HS, USBD_CDC_Buffer_Data_HS_TX);
	}
#endif
	
	/* Return OK */
	return TM_USBD_Result_Ok;
}

void TM_USBD_CDC_Process(TM_USB_t USB_Mode) {
#if defined(USB_USE_FS) || defined(USB_USE_HS)
	USBD_HandleTypeDef *pdev;
	USBD_CDC_HandleTypeDef *hcdc;
#endif

#ifdef USB_USE_FS
	if (USB_Mode == TM_USB_FS || USB_Mode == TM_USB_Both) {
		static uint8_t USBD_CDC_tmp_FS[USBD_CDC_TMP_TRANSMIT_BUFFER_SIZE];
		uint16_t readFS;
		
		/* Get pointer */
		pdev = TM_USBD_GetUSBPointer(TM_USB_FS);
		hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;
		
		/* If TX is not working */
		if (!hcdc->TxState) {
			/* Check number of elements in TX buffer for FS */
			readFS = TM_BUFFER_Read(&USBD_CDC_Buffer_FS_TX, USBD_CDC_tmp_FS, sizeof(USBD_CDC_tmp_FS));
			
			/* Check if read anything */
			if (readFS) {
				/* Send data */
				USBD_CDC_SetTxBuffer(pdev, USBD_CDC_tmp_FS, readFS);
				USBD_CDC_TransmitPacket(pdev);
			}
		}
	}
#endif
	
#ifdef USB_USE_HS
	if (USB_Mode == TM_USB_HS || USB_Mode == TM_USB_Both) {
		static uint8_t USBD_CDC_tmp_HS[USBD_CDC_TMP_TRANSMIT_BUFFER_SIZE];
		uint16_t readHS;
		
		/* Get pointer */
		pdev = TM_USBD_GetUSBPointer(TM_USB_HS);
		hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;
		
		/* If TX is not working */
		if (!hcdc->TxState) {
			/* Check number of elements in TX buffer for HS */
			readHS = TM_BUFFER_Read(&USBD_CDC_Buffer_HS_TX, USBD_CDC_tmp_HS, sizeof(USBD_CDC_tmp_HS));
			
			/* Check if read anything */
			if (readHS) {
				/* Send data */
				USBD_CDC_SetTxBuffer(pdev, USBD_CDC_tmp_HS, readHS);
				USBD_CDC_TransmitPacket(pdev);
			}
		}
	}
#endif
}

uint16_t TM_USBD_CDC_Putc(TM_USB_t USB_Mode, char ch) {
	/* Check for write */
	if (TM_BUFFER_Write(TM_USBD_CDC_INT_GetTXBuffer(USB_Mode), (uint8_t *)&ch, 1)) {
		/* Process */
		TM_USBD_CDC_Process(USB_Mode);
		
		/* Return OK */
		return 1;
	}
	
	/* Return error */
	return 0;
}

uint16_t TM_USBD_CDC_Puts(TM_USB_t USB_Mode, const char* str) {
	uint16_t ret;
	
	/* Write and process */
	if ((ret = TM_BUFFER_Write(TM_USBD_CDC_INT_GetTXBuffer(USB_Mode), (uint8_t *)str, strlen(str))) > 0) {
		TM_USBD_CDC_Process(USB_Mode);
	}
	
	/* Return number of elements added to buffer */
	return ret;
}

uint16_t TM_USBD_CDC_PutArray(TM_USB_t USB_Mode, uint8_t* buff, uint16_t count) {
	uint16_t ret;
	
	/* Write and process */
	if ((ret = TM_BUFFER_Write(TM_USBD_CDC_INT_GetTXBuffer(USB_Mode), buff, count)) > 0) {
		TM_USBD_CDC_Process(USB_Mode);
	}
	
	/* Return number of elements added to array */
	return ret;
}

uint8_t TM_USBD_CDC_Getc(TM_USB_t USB_Mode, char* ch) {
	/* Try to read from buffer */
	if (TM_BUFFER_Read(TM_USBD_CDC_INT_GetRXBuffer(USB_Mode), (uint8_t *)ch, 1)) {
		/* Character read */
		return 1;
	}
	
	/* Return error */
	return 0;
}

uint16_t TM_USBD_CDC_Gets(TM_USB_t USB_Mode, char* buff, uint16_t buffsize) {
	/* Return buffer value */
	return TM_BUFFER_ReadString(TM_USBD_CDC_INT_GetRXBuffer(USB_Mode), (char *)buff, buffsize);
}

uint16_t TM_USBD_CDC_GetArray(TM_USB_t USB_Mode, uint8_t* buff, uint16_t count) {
	/* Return buffer value */
	return TM_BUFFER_Read(TM_USBD_CDC_INT_GetRXBuffer(USB_Mode), buff, count);
}

void TM_USBD_CDC_GetSettings(TM_USB_t USB_Mode, TM_USBD_CDC_Settings_t* Settings) {
	USBD_CDC_LineCodingTypeDef* LineCoding;
	
#ifdef USB_USE_FS
	if (USB_Mode == TM_USB_FS) {
		LineCoding = &linecoding[0];
	}
#endif
#ifdef USB_USE_HS
	if (USB_Mode == TM_USB_HS) {
		LineCoding = &linecoding[1];
	}
#endif
	
	/* Check if valid */
	if (LineCoding &&
		(
			Settings->Baudrate != LineCoding->bitrate ||
			Settings->DataBits != LineCoding->datatype ||
			Settings->Parity != LineCoding->paritytype ||
			Settings->Stopbits != LineCoding->format
		)
	) {
		Settings->Baudrate = LineCoding->bitrate;
		Settings->DataBits = LineCoding->datatype;
		Settings->Parity = LineCoding->paritytype;
		Settings->Stopbits = LineCoding->format;
		
		/* Settings are updated */
		Settings->Updated = 1;
	} else {
		/* Settings not updated */
		Settings->Updated = 0;
	}
}

/************************************************/
/*               PRIVATE FUNCTIONS              */
/************************************************/
void TM_USBD_CDC_INT_AddToBuffer(USBD_HandleTypeDef* pdev, uint8_t* Values, uint16_t Num) {
#ifdef USB_USE_FS
	if (pdev->id == USB_ID_FS) {
		/* Write to FS RX buffer */
		TM_BUFFER_Write(&USBD_CDC_Buffer_FS_RX, Values, Num);
	}
#endif	
#ifdef USB_USE_HS
	if (pdev->id == USB_ID_HS) {
		/* Write to HS RX buffer */
		TM_BUFFER_Write(&USBD_CDC_Buffer_HS_RX, Values, Num);
	}
#endif
}

/************************************************/
/*       DESCRIPTION BASED IMPLEMENTATION       */
/************************************************/
/**
  * @brief  Returns the device descriptor. 
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_VCP_DeviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	*length = sizeof(USBD_DeviceDesc);
	return (uint8_t*)USBD_DeviceDesc;
}

/**
  * @brief  Returns the LangID string descriptor.        
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_VCP_LangIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	*length = sizeof(USBD_LangIDDesc);  
	return (uint8_t*)USBD_LangIDDesc;
}

/**
  * @brief  Returns the product string descriptor. 
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_VCP_ProductStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	if (speed == USBD_SPEED_HIGH) {   
		USBD_GetString((uint8_t *)USBD_PRODUCT_HS_STRING, USBD_StrDesc, length);
	} else {
		USBD_GetString((uint8_t *)USBD_PRODUCT_FS_STRING, USBD_StrDesc, length);    
	}
	return USBD_StrDesc;
}

/**
  * @brief  Returns the manufacturer string descriptor. 
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_VCP_ManufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	USBD_GetString((uint8_t *)USBD_MANUFACTURER_STRING, USBD_StrDesc, length);
	return USBD_StrDesc;
}

/**
  * @brief  Returns the serial number string descriptor.        
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_VCP_SerialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	*length = USB_SIZ_STRING_SERIAL;

	/* Update the serial number string descriptor with the data from the unique ID */
	Get_SerialNum();

	return (uint8_t*)USBD_StringSerial;
}

/**
  * @brief  Returns the configuration string descriptor.    
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_VCP_ConfigStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	if (speed == USBD_SPEED_HIGH) {  
		USBD_GetString((uint8_t *)USBD_CONFIGURATION_HS_STRING, USBD_StrDesc, length);
	} else {
		USBD_GetString((uint8_t *)USBD_CONFIGURATION_FS_STRING, USBD_StrDesc, length); 
	}
	return USBD_StrDesc;  
}

/**
  * @brief  Returns the interface string descriptor.        
  * @param  speed: Current device speed
  * @param  length: Pointer to data length variable
  * @retval Pointer to descriptor buffer
  */
uint8_t *USBD_VCP_InterfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	if (speed == USBD_SPEED_HIGH)	{
		USBD_GetString((uint8_t *)USBD_INTERFACE_HS_STRING, USBD_StrDesc, length);
	} else {
		USBD_GetString((uint8_t *)USBD_INTERFACE_FS_STRING, USBD_StrDesc, length);
	}
	return USBD_StrDesc;  
}

/**
  * @brief  Create the serial number string descriptor 
  * @param  None 
  * @retval None
  */
static void Get_SerialNum(void) {
	uint32_t deviceserial0, deviceserial1, deviceserial2;

	deviceserial0 = *(uint32_t*)DEVICE_ID1;
	deviceserial1 = *(uint32_t*)DEVICE_ID2;
	deviceserial2 = *(uint32_t*)DEVICE_ID3;

	deviceserial0 += deviceserial2;

	if (deviceserial0 != 0) {
		IntToUnicode (deviceserial0, &USBD_StringSerial[2] ,8);
		IntToUnicode (deviceserial1, &USBD_StringSerial[18] ,4);
	}
}

/**
  * @brief  Convert Hex 32Bits value into char 
  * @param  value: value to convert
  * @param  pbuf: pointer to the buffer 
  * @param  len: buffer length
  * @retval None
  */
static void IntToUnicode (uint32_t value, uint8_t *pbuf , uint8_t len) {
	uint8_t idx = 0;

	for (idx = 0; idx < len; idx++) {
		if (((value >> 28)) < 0xA) {
			pbuf[2 * idx] = (value >> 28) + '0';
		} else {
			pbuf[2 * idx] = (value >> 28) + 'A' - 10; 
		}
		
		value = value << 4;

		pbuf[ 2* idx + 1] = 0;
	}
}

