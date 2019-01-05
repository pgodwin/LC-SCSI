/**
  ******************************************************************************
  * @file    usbd_cdc_if_template.c
  * @author  MCD Application Team
  * @version V2.4.1
  * @date    19-June-2015
  * @brief   Generic media access Layer.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_if.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_CDC 
  * @brief usbd core module
  * @{
  */ 

/** @defgroup USBD_CDC_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_CDC_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_CDC_Private_Macros
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_CDC_Private_FunctionPrototypes
  * @{
  */
#ifdef USB_USE_FS
static int8_t VCP_InitFS(void);
static int8_t VCP_DeInitFS(void);
static int8_t VCP_ControlFS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t VCP_ReceiveFS(uint8_t* pbuf, uint32_t *Len);
#endif
#ifdef USB_USE_HS
static int8_t VCP_InitHS(void);
static int8_t VCP_DeInitHS(void);
static int8_t VCP_ControlHS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t VCP_ReceiveHS(uint8_t* pbuf, uint32_t *Len);
#endif

/* Internal functions */
static int8_t VCP_Init(USBD_HandleTypeDef* pdev);
static int8_t VCP_DeInit(USBD_HandleTypeDef* pdev);
static int8_t VCP_Control(USBD_HandleTypeDef* pdev, uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t VCP_Receive(USBD_HandleTypeDef* pdev, uint8_t* pbuf, uint32_t *Len);

USBD_CDC_ItfTypeDef USBD_CDC_fops[] = {
	{
#ifdef USB_USE_FS
		VCP_InitFS,
		VCP_DeInitFS,
		VCP_ControlFS,
		VCP_ReceiveFS
#else
		0, 0, 0, 0
#endif
	},
	{
#ifdef USB_USE_HS
		VCP_InitHS,
		VCP_DeInitHS,
		VCP_ControlHS,
		VCP_ReceiveHS
#else
		0, 0, 0, 0
#endif
	}
};

USBD_CDC_LineCodingTypeDef linecoding[] = {
	{
		115200, /* baud rate*/
		0x00,   /* stop bits-1*/
		0x00,   /* parity - none*/
		0x08    /* nb. of bits 8*/
	},
	{
		115200, /* baud rate*/
		0x00,   /* stop bits-1*/
		0x00,   /* parity - none*/
		0x08    /* nb. of bits 8*/
	}
};

/* Receive buffers for HS and FS modes */
#ifdef USB_USE_FS
static uint8_t FS_TMP_RX_Buff[USBD_CDC_TMP_RECEIVE_BUFFER_SIZE];
#endif
#ifdef USB_USE_HS
static uint8_t HS_TMP_RX_Buff[USBD_CDC_TMP_RECEIVE_BUFFER_SIZE];
#endif
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  TEMPLATE_Init
  *         Initializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
#ifdef USB_USE_FS
static int8_t VCP_InitFS(void) {
	return VCP_Init(&hUSBDevice_FS);
}
#endif
#ifdef USB_USE_HS
static int8_t VCP_InitHS(void) {
	return VCP_Init(&hUSBDevice_HS);
}
#endif

/**
  * @brief  TEMPLATE_DeInit
  *         DeInitializes the CDC media low layer
  * @param  None
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
#ifdef USB_USE_FS
static int8_t VCP_DeInitFS(void) {
	return VCP_DeInit(&hUSBDevice_FS);
}
#endif
#ifdef USB_USE_HS
static int8_t VCP_DeInitHS(void) {
	return VCP_DeInit(&hUSBDevice_HS);
}
#endif

/**
  * @brief  TEMPLATE_Control
  *         Manage the CDC class requests
  * @param  Cmd: Command code            
  * @param  Buf: Buffer containing command data (request parameters)
  * @param  Len: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
#ifdef USB_USE_FS
static int8_t VCP_ControlFS(uint8_t cmd, uint8_t* pbuf, uint16_t length) { 
	return VCP_Control(&hUSBDevice_FS, cmd, pbuf, length);
}
#endif
#ifdef USB_USE_HS
static int8_t VCP_ControlHS(uint8_t cmd, uint8_t* pbuf, uint16_t length) { 
	return VCP_Control(&hUSBDevice_HS, cmd, pbuf, length);
}
#endif

/**
  * @brief  TEMPLATE_Receive
  *         Data received over USB OUT endpoint are sent over CDC interface 
  *         through this function.
  *           
  *         @note
  *         This function will issue a NAK packet on any OUT packet received on 
  *         USB endpoint untill exiting this function. If you exit this function
  *         before transfer is complete on CDC interface (ie. using DMA controller)
  *         it will result in receiving more data while previous ones are still 
  *         not sent.
  *                 
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
#ifdef USB_USE_FS
static int8_t VCP_ReceiveFS(uint8_t* Buf, uint32_t *Len) {
	return VCP_Receive(&hUSBDevice_FS, Buf, Len);
}
#endif
#ifdef USB_USE_HS
static int8_t VCP_ReceiveHS(uint8_t* Buf, uint32_t *Len) {
	return VCP_Receive(&hUSBDevice_HS, Buf, Len);
}
#endif

/********************************************************/
/*          Single functions for both USB modes         */
/********************************************************/

static int8_t VCP_Init(USBD_HandleTypeDef* pdev) {
#ifdef USB_USE_FS
	/* Set RX buffer */
	if (pdev->id == USB_ID_FS) {
		USBD_CDC_SetRxBuffer(pdev, FS_TMP_RX_Buff);
	}
#endif
#ifdef USB_USE_HS
	/* Set RX buffer */
	if (pdev->id == USB_ID_HS) {
		USBD_CDC_SetRxBuffer(pdev, HS_TMP_RX_Buff);
	}
#endif
	
	/* Return OK */
	return USBD_OK;
}

static int8_t VCP_DeInit(USBD_HandleTypeDef* pdev) {
	/* Return OK */
	return USBD_OK;
}

static int8_t VCP_Receive(USBD_HandleTypeDef* pdev, uint8_t* pbuf, uint32_t *Len) {
	/* Add to RX buffer */
	TM_USBD_CDC_INT_AddToBuffer(pdev, pbuf, *Len);
	
	/* Prepare for next  */
	USBD_CDC_ReceivePacket(pdev);
    
	/* Return OK */
	return USBD_OK;
}

static int8_t VCP_Control(USBD_HandleTypeDef* pdev, uint8_t cmd, uint8_t* pbuf, uint16_t length) {
	switch (cmd) {
		case CDC_SEND_ENCAPSULATED_COMMAND:
			/* Add your code here */
			break;

		case CDC_GET_ENCAPSULATED_RESPONSE:
			/* Add your code here */
			break;

		case CDC_SET_COMM_FEATURE:
			/* Add your code here */
			break;

		case CDC_GET_COMM_FEATURE:
			/* Add your code here */
			break;

		case CDC_CLEAR_COMM_FEATURE:
			/* Add your code here */
			break;

		case CDC_SET_LINE_CODING:
			/* Copy structure to buffer */
			memcpy((uint8_t *)&linecoding[pdev->id], pbuf, sizeof(USBD_CDC_LineCodingTypeDef));

			/* Add your code here */
			break;

		case CDC_GET_LINE_CODING:
			/* Copy structure to buffer */
			memcpy(pbuf, (uint8_t *)&linecoding[pdev->id], sizeof(USBD_CDC_LineCodingTypeDef));

			/* Add your code here */
			break;

		case CDC_SET_CONTROL_LINE_STATE:
			/* Add your code here */
			break;

		case CDC_SEND_BREAK:
			/* Add your code here */
			break;    

		default:
			break;
	}

	/* Return OK */
	return USBD_OK;
}


/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

