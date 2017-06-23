/****************************************Copyright (c)**************************************************
**                       思 蜕 盟 豆 皮 开 发 小 组
**                             stmfans 论坛
**
**                   QQ 群: 65081316  StmFans思蜕盟 1组
**                   QQ 群: 68584951  StmFans思蜕盟 2组
**                      http://www.stmfans.com/bbs/
**
** This program was produced by the
** IAR Embedded Workbench 4.0 Kickstart 442
** Copyright 2008-2009 stmfans 
** Chip type           : STM32F103VB
** Program type        : Application
** Clock frequency     : 8.000000 MHz
** Memory model        : 
** External SRAM size  : 
** Data Stack size     : 
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: Main.c
**创   建   人: 陈海
**最后修改日期: 2008年10月23日
**描        述: 豆皮开发板教程
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 陈海
** 版  本: v0.01
** 日　期: 2008年10月23日
** 描　述: 原始版本
**
**--------------当前版本信息----------------------------------------------------------------------------
** 创建人: 陈海
** 版  本: v0.01
** 日　期: 2008年10月23日
** 描　述: 当前版本
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void delay(void);

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
#ifdef DEBUG
  debug();
#endif

  //配置系统时钟
  RCC_Configuration();

  //配置  NVIC 和 Vector Table 
  NVIC_Configuration();

  //改变指定管脚的映射  GPIO_Remap_SWJ_Disable SWJ 完全失能（JTAG+SW-DP）
  //GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
  
  //改变指定管脚的映射  GPIO_Remap_SWJ_JTAGDisable  JTAG-DP 失能 + SW-DP使能
//  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
  
  //配置使用的GPIO口
  GPIO_Configuration();
  
  NVIC_RESETPRIMASK(); //enable all system interrupts

  
  //主循环 
  while (1)
  {
      delay();
      delay();
      //设置指定的数据端口位
      GPIO_SetBits(GPIOB, GPIO_Pin_0);
      delay();
      delay();
      //清除指定的数据端口位
      GPIO_ResetBits(GPIOB, GPIO_Pin_0);
      
/*      GPIO_SetBits(GPIOC, GPIO_Pin_11);
      delay();
      GPIO_ResetBits(GPIOC, GPIO_Pin_11);

      GPIO_SetBits(GPIOC, GPIO_Pin_10);
      delay();
      GPIO_ResetBits(GPIOC, GPIO_Pin_10);
    
      GPIO_SetBits(GPIOC, GPIO_Pin_12);
      delay();
      GPIO_ResetBits(GPIOC, GPIO_Pin_12);
    
      GPIO_SetBits(GPIOD, GPIO_Pin_2);
      delay();
      GPIO_ResetBits(GPIOD, GPIO_Pin_2);

      GPIO_SetBits(GPIOD, GPIO_Pin_3);
      delay();
      GPIO_ResetBits(GPIOD, GPIO_Pin_3);
      
      GPIO_SetBits(GPIOD, GPIO_Pin_4);
      delay();
      GPIO_ResetBits(GPIOD, GPIO_Pin_4);

      GPIO_SetBits(GPIOD, GPIO_Pin_5);
      delay();
      GPIO_ResetBits(GPIOD, GPIO_Pin_5); 
      
      delay();*/
  }
}


/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
  ErrorStatus HSEStartUpStatus;
  
  //将外设 RCC寄存器重设为缺省值
  RCC_DeInit();

  //设置外部高速晶振（HSE）
  RCC_HSEConfig(RCC_HSE_ON);

  //等待 HSE 起振 
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    //预取指缓存使能
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

     //设置代码延时值
    //FLASH_Latency_2  2 延时周期
    FLASH_SetLatency(FLASH_Latency_2);

    //设置 AHB 时钟（HCLK）
    //RCC_SYSCLK_Div1  AHB 时钟 =  系统时钟 
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

     //设置高速 AHB 时钟（PCLK2）
    //RCC_HCLK_Div2  APB1 时钟  = HCLK / 2 
    RCC_PCLK2Config(RCC_HCLK_Div2);

    //设置低速 AHB 时钟（PCLK1）
    //RCC_HCLK_Div2  APB1 时钟  = HCLK / 2 
    RCC_PCLK1Config(RCC_HCLK_Div2);

    // PLLCLK = 8MHz * 9 = 72 MHz 
    //设置 PLL 时钟源及倍频系数
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    //使能或者失能 PLL
    RCC_PLLCmd(ENABLE);

    //等待指定的 RCC 标志位设置成功 等待PLL初始化成功
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }


    //设置系统时钟（SYSCLK） 设置PLL为系统时钟源
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    //等待PLL成功用作于系统时钟的时钟源
    //  0x00：HSI 作为系统时钟 
    //  0x04：HSE作为系统时钟 
    //  0x08：PLL作为系统时钟  
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }

  //使能或者失能 APB2 外设时钟
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
  
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
/*
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_3 | GPIO_Pin_2 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure); 

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures NVIC and Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
  /* Set the Vector Table base location at 0x20000000 */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x2000);
#endif
}


void delay()
{
  int i;
  for (i=0; i<0xfffff; i++)
    ;
}

#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
