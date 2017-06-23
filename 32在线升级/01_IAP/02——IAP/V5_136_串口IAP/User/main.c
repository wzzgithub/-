/*
*********************************************************************************************************
*
*	ģ������ : ������ģ�顣
*	�ļ����� : main.c
*	��    �� : V1.3
*	˵    �� : ���ڴ�������� Flash ��ϵͳ���ԣ������ղ�Ʒ�а�װ֮���ܹ��Թ̼����и��£���һ��ǳ���Ҫ��
*             ��һ���ܱ���Ϊ��Ӧ���б�� (IAP   in-applicationprogramming)�� ͨ��΢���������� Flash �� IAP
*            ��������Ϊ��boot���룩��STM32F4xx ΢�������ܹ������û�ָ���̼���������һ���ԣ������±��
*             �����п���ʹ���������͵�ͨ��Э�顣��������������ô�������һ������ִ�е�bin�ļ����򵽿����壬
*             ���ڿ������Ѿ����ú�IAP����������bin�ļ����û��Ϳ���ѡ����ת����Ӧflash��ַȥִ���������
*             
*             ����ʹ�÷����ο����ӣ�http://bbs.armfly.com/read.php?tid=7095
*	�޸ļ�¼ :
*		�汾��    ����       ����            ˵��
*		v1.3    2015-03-05 Eric2013     ��������������ST�ٷ��Ĵ���IAP
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"			/* �ײ�Ӳ������ */
#include "common.h"
#include "flash_if.h"
#include "ymodem.h"


/* ���������������̷������� */
#define EXAMPLE_NAME	"V5-136_����IAP"
#define EXAMPLE_DATE	"2015-03-05"
#define DEMO_VER		"V1.2"

/* ���� */
typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;
__IO uint32_t FlashProtection = 0;
uint8_t tab_1024[1024] ={0};
uint8_t FileName[FILE_NAME_LENGTH];

/* �������ļ��ڵ��õĺ������� */
static void PrintfLogo(void);
static void SerialDownload(void);

/*
*********************************************************************************************************
*	�� �� ��: main
*	����˵��: c�������
*	��    �Σ���
*	�� �� ֵ: �������(���账��)
*********************************************************************************************************
*/
int main(void)
{
	uint8_t key = 0;

	bsp_Init();		/* Ӳ����ʼ�� */
	PrintfLogo();	/* ��ӡ������Ϣ������1 */
	FLASH_If_Init(); /* ��ʼ��flash���� */
	
	/* ����û���flash��д���� */
	if (FLASH_If_GetWriteProtectionStatus() == 0)   
	{
		/* �Ᵽ�� */
		if(FLASH_If_DisableWriteProtection() == 1)
		{
			/* �Ᵽ���ɹ� */
			printf("\n\r�Ᵽ���ɹ�\n\r");
		}
		else
		{
			/* �Ᵽ��ʧ�� */
			SerialPutString("�Ᵽ��ʧ��\r\n");
		}
	}

	while (1)
	{
		/* �ȴ������ַ���ͨ�����Լ������룩 */
		key = GetKey();

		if (key == 0x31)
		{
			/* �����µ��û����뵽flash����  */
			SerialDownload();
		}
		else if (key == 0x32) 
		{
			JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
			/* �û�Ӧ�õ�ַ */
			Jump_To_Application = (pFunction) JumpAddress;
			/* ��ʼ��MSP */
			__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
			Jump_To_Application();
		}
	}
}

/*
*********************************************************************************************************
*	�� �� ��: SerialDownload
*	����˵��: �ļ�����
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void SerialDownload(void)
{
	int32_t Size = 0;

	printf("�ȴ������ļ�..................�������ϵ���ĸa��ֹ����\n\r");
	Size = Ymodem_Receive(&tab_1024[0]);
	if (Size > 0)
	{
		printf("\n\r��̳ɹ����ļ�����%s����С��%d�ֽ�\n\r", FileName, Size);
	}
	else if (Size == -1)
	{
		printf("\n\r���ص��ļ���С�������û���flash��С\n\r");
	}
	else if (Size == -2)
	{
		printf("\n\rУ��ʧ��\n\r");
	}
	else if (Size == -3)
	{
		printf("\n\r�û���ֹ����\n\r");
	}
	else
	{
		printf("\n\r�ļ�����ʧ��\n\r");
	}
}

/*
*********************************************************************************************************
*	�� �� ��: PrintfLogo
*	����˵��: ��ӡ�������ƺ����̷�������, ���ϴ����ߺ󣬴�PC���ĳ����ն�������Թ۲���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	/* ���CPU ID */
	{
		/* �ο��ֲ᣺
			32.6.1 MCU device ID code
			33.1 Unique device ID register (96 bits)
		*/
		uint32_t CPU_Sn0, CPU_Sn1, CPU_Sn2;

		CPU_Sn0 = *(__IO uint32_t*)(0x1FFF7A10);
		CPU_Sn1 = *(__IO uint32_t*)(0x1FFF7A10 + 4);
		CPU_Sn2 = *(__IO uint32_t*)(0x1FFF7A10 + 8);

		printf("\r\nCPU : STM32F407IGT6, LQFP176, UID = %08X %08X %08X\n\r"
			, CPU_Sn2, CPU_Sn1, CPU_Sn0);
	}

	printf("\n\r");
	printf("*************************************************************\n\r");
	printf("* ��������   : %s\r\n", EXAMPLE_NAME);	/* ��ӡ�������� */
	printf("* ���̰汾   : %s\r\n", DEMO_VER);		/* ��ӡ���̰汾 */
	printf("* ��������   : %s\r\n", EXAMPLE_DATE);	/* ��ӡ�������� */

	/* ��ӡST�̼���汾����3���������stm32f10x.h�ļ��� */
	printf("* �̼���汾 : V%d.%d.%d (STM32F4xx_StdPeriph_Driver)\r\n", __STM32F4XX_STDPERIPH_VERSION_MAIN,
			__STM32F4XX_STDPERIPH_VERSION_SUB1,__STM32F4XX_STDPERIPH_VERSION_SUB2);
	printf("* \r\n");	/* ��ӡһ�пո� */
	printf("* QQ    : 1295744630 \r\n");
	printf("* ����  : armfly\r\n");
	printf("* Email : armfly@qq.com \r\n");
	printf("* �Ա���: armfly.taobao.com\r\n");
	printf("* Copyright www.armfly.com ����������\r\n");
	printf("*************************************************************\n\r");
	printf("1. ���Լ��̰�������1�����³����bin�ļ�����\n\r");
	printf("2. ���Լ��̰�������2��ת���µĳ���ִ��\n\r");
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
