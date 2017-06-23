/**
  ******************************************************************************
  * @file    IAP/src/download.c 
  * @author  MCD Application Team
  * @version V3.3.0
  * @date    10/15/2010
  * @brief   This file provides the software which allows to download an image 
  *          to internal Flash.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/** @addtogroup IAP
  * @{
  */
/* Includes ------------------------------------------------------------------*/
#include "common.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern uint8_t file_name[FILE_NAME_LENGTH];
extern pFunction Jump_To_Application;
extern uint32_t JumpAddress;

uint8_t tab_1024[1024] =
  {
    0
  };

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Download a file via serial port
  * @param  None
  * @retval None
  */
void SerialDownload(void)
{
  uint8_t Number[10] = "          ";
  int32_t Size = 0;

  SerialPutString("�ȴ����͵��ļ� ... \n\r");
  Size = Ymodem_Receive(&tab_1024[0]);
Int2Str(Number, Size);
  if (Size > 0)
  {
    SerialPutString("\n\n\r ��̳ɹ����!\n\r--------------------------------\r\n Name: ");
    SerialPutString(file_name);
    Int2Str(Number, Size);
    SerialPutString("\n\r ��С: ");
    SerialPutString(Number);
    SerialPutString(" Bytes\r\n");
    SerialPutString("-------------------\n");
  }
  else if (Size == -1)
  {
    SerialPutString("\n\n\r�ļ���С�����ռ��С!\n\r");
  }
  else if (Size == -2)
  {
    SerialPutString("\n\n\r��֤ʧ��!\n\r");
  }
  else if (Size == -3)
  {
    SerialPutString("\r\n\n���û���ֹ.\n\r");
  }
	else if(Size == -4)
	{
		JumpAddress = *(__IO uint32_t*) (ApplicationAddress + 4);

      /* Jump to user application */
      Jump_To_Application = (pFunction) JumpAddress;
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(__IO uint32_t*) ApplicationAddress);
      Jump_To_Application();
	}
  else
  {
    SerialPutString("\n\rû���յ��ļ�!\n\r");
  }
}

/**
  * @}
  */

/*******************(C)COPYRIGHT 2010 STMicroelectronics *****END OF FILE******/
