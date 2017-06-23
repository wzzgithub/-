/*
*********************************************************************************************************
*
*	模块名称 : 主程序模块。
*	文件名称 : main.c
*	版    本 : V1.3
*	说    明 : 对于大多数基于 Flash 的系统而言，在最终产品中安装之后，能够对固件进行更新，这一点非常重要。
*             这一功能被称为在应用中编程 (IAP   in-applicationprogramming)。 通过微处理器内置 Flash 的 IAP
*            （可以认为是boot代码），STM32F4xx 微控制器能够运行用户指定固件。借助这一特性，在重新编程
*             过程中可以使用任意类型的通信协议。下面的例子是利用串口下载一个可以执行的bin文件程序到开发板，
*             由于开发板已经内置好IAP程序，下载完bin文件后，用户就可以选择跳转到相应flash地址去执行这个程序。
*             
*             具体使用方法参考帖子：http://bbs.armfly.com/read.php?tid=7095
*	修改记录 :
*		版本号    日期       作者            说明
*		v1.3    2015-03-05 Eric2013     此例子是整理自ST官方的串口IAP
*
*	Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"			/* 底层硬件驱动 */
#include "common.h"
#include "flash_if.h"
#include "ymodem.h"


/* 定义例程名和例程发布日期 */
#define EXAMPLE_NAME	"V5-136_串口IAP"
#define EXAMPLE_DATE	"2015-03-05"
#define DEMO_VER		"V1.2"

/* 变量 */
typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;
__IO uint32_t FlashProtection = 0;
uint8_t tab_1024[1024] ={0};
uint8_t FileName[FILE_NAME_LENGTH];

/* 仅允许本文件内调用的函数声明 */
static void PrintfLogo(void);
static void SerialDownload(void);

/*
*********************************************************************************************************
*	函 数 名: main
*	功能说明: c程序入口
*	形    参：无
*	返 回 值: 错误代码(无需处理)
*********************************************************************************************************
*/
int main(void)
{
	uint8_t key = 0;

	bsp_Init();		/* 硬件初始化 */
	PrintfLogo();	/* 打印例程信息到串口1 */
	FLASH_If_Init(); /* 初始化flash操作 */
	
	/* 如果用户区flash有写保护 */
	if (FLASH_If_GetWriteProtectionStatus() == 0)   
	{
		/* 解保护 */
		if(FLASH_If_DisableWriteProtection() == 1)
		{
			/* 解保护成功 */
			printf("\n\r解保护成功\n\r");
		}
		else
		{
			/* 解保护失败 */
			SerialPutString("解保护失败\r\n");
		}
	}

	while (1)
	{
		/* 等待接收字符（通过电脑键盘输入） */
		key = GetKey();

		if (key == 0x31)
		{
			/* 下载新的用户代码到flash里面  */
			SerialDownload();
		}
		else if (key == 0x32) 
		{
			JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
			/* 用户应用地址 */
			Jump_To_Application = (pFunction) JumpAddress;
			/* 初始化MSP */
			__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
			Jump_To_Application();
		}
	}
}

/*
*********************************************************************************************************
*	函 数 名: SerialDownload
*	功能说明: 文件下载
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void SerialDownload(void)
{
	int32_t Size = 0;

	printf("等待发送文件..................按键盘上的字母a终止发送\n\r");
	Size = Ymodem_Receive(&tab_1024[0]);
	if (Size > 0)
	{
		printf("\n\r编程成功，文件名：%s，大小：%d字节\n\r", FileName, Size);
	}
	else if (Size == -1)
	{
		printf("\n\r下载的文件大小超出了用户区flash大小\n\r");
	}
	else if (Size == -2)
	{
		printf("\n\r校验失败\n\r");
	}
	else if (Size == -3)
	{
		printf("\n\r用户终止发送\n\r");
	}
	else
	{
		printf("\n\r文件接收失败\n\r");
	}
}

/*
*********************************************************************************************************
*	函 数 名: PrintfLogo
*	功能说明: 打印例程名称和例程发布日期, 接上串口线后，打开PC机的超级终端软件可以观察结果
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void PrintfLogo(void)
{
	/* 检测CPU ID */
	{
		/* 参考手册：
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
	printf("* 例程名称   : %s\r\n", EXAMPLE_NAME);	/* 打印例程名称 */
	printf("* 例程版本   : %s\r\n", DEMO_VER);		/* 打印例程版本 */
	printf("* 发布日期   : %s\r\n", EXAMPLE_DATE);	/* 打印例程日期 */

	/* 打印ST固件库版本，这3个定义宏在stm32f10x.h文件中 */
	printf("* 固件库版本 : V%d.%d.%d (STM32F4xx_StdPeriph_Driver)\r\n", __STM32F4XX_STDPERIPH_VERSION_MAIN,
			__STM32F4XX_STDPERIPH_VERSION_SUB1,__STM32F4XX_STDPERIPH_VERSION_SUB2);
	printf("* \r\n");	/* 打印一行空格 */
	printf("* QQ    : 1295744630 \r\n");
	printf("* 旺旺  : armfly\r\n");
	printf("* Email : armfly@qq.com \r\n");
	printf("* 淘宝店: armfly.taobao.com\r\n");
	printf("* Copyright www.armfly.com 安富莱电子\r\n");
	printf("*************************************************************\n\r");
	printf("1. 电脑键盘按下数字1进行新程序的bin文件下载\n\r");
	printf("2. 电脑键盘按下数字2跳转到新的程序执行\n\r");
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
