/****************************************Copyright (c)**************************************************
**                       ˼ �� �� �� Ƥ �� �� С ��
**                             stmfans ��̳
**
**                   QQ Ⱥ: 65081316  StmFans˼���� 1��
**                   QQ Ⱥ: 68584951  StmFans˼���� 2��
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
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: Main.c
**��   ��   ��: �º�
**����޸�����: 2008��10��23��
**��        ��: ��Ƥ������̳�
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: �º�
** ��  ��: v0.01
** �ա���: 2008��10��23��
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾��Ϣ----------------------------------------------------------------------------
** ������: �º�
** ��  ��: v0.01
** �ա���: 2008��10��23��
** �衡��: ��ǰ�汾
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

  //����ϵͳʱ��
  RCC_Configuration();

  //����  NVIC �� Vector Table 
  NVIC_Configuration();

  //�ı�ָ���ܽŵ�ӳ��  GPIO_Remap_SWJ_Disable SWJ ��ȫʧ�ܣ�JTAG+SW-DP��
  //GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
  
  //�ı�ָ���ܽŵ�ӳ��  GPIO_Remap_SWJ_JTAGDisable  JTAG-DP ʧ�� + SW-DPʹ��
//  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);
  
  //����ʹ�õ�GPIO��
  GPIO_Configuration();
  
  NVIC_RESETPRIMASK(); //enable all system interrupts

  
  //��ѭ�� 
  while (1)
  {
      delay();
      delay();
      //����ָ�������ݶ˿�λ
      GPIO_SetBits(GPIOB, GPIO_Pin_0);
      delay();
      delay();
      //���ָ�������ݶ˿�λ
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
  
  //������ RCC�Ĵ�������Ϊȱʡֵ
  RCC_DeInit();

  //�����ⲿ���پ���HSE��
  RCC_HSEConfig(RCC_HSE_ON);

  //�ȴ� HSE ���� 
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    //Ԥȡָ����ʹ��
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

     //���ô�����ʱֵ
    //FLASH_Latency_2  2 ��ʱ����
    FLASH_SetLatency(FLASH_Latency_2);

    //���� AHB ʱ�ӣ�HCLK��
    //RCC_SYSCLK_Div1  AHB ʱ�� =  ϵͳʱ�� 
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

     //���ø��� AHB ʱ�ӣ�PCLK2��
    //RCC_HCLK_Div2  APB1 ʱ��  = HCLK / 2 
    RCC_PCLK2Config(RCC_HCLK_Div2);

    //���õ��� AHB ʱ�ӣ�PCLK1��
    //RCC_HCLK_Div2  APB1 ʱ��  = HCLK / 2 
    RCC_PCLK1Config(RCC_HCLK_Div2);

    // PLLCLK = 8MHz * 9 = 72 MHz 
    //���� PLL ʱ��Դ����Ƶϵ��
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    //ʹ�ܻ���ʧ�� PLL
    RCC_PLLCmd(ENABLE);

    //�ȴ�ָ���� RCC ��־λ���óɹ� �ȴ�PLL��ʼ���ɹ�
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }


    //����ϵͳʱ�ӣ�SYSCLK�� ����PLLΪϵͳʱ��Դ
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    //�ȴ�PLL�ɹ�������ϵͳʱ�ӵ�ʱ��Դ
    //  0x00��HSI ��Ϊϵͳʱ�� 
    //  0x04��HSE��Ϊϵͳʱ�� 
    //  0x08��PLL��Ϊϵͳʱ��  
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }

  //ʹ�ܻ���ʧ�� APB2 ����ʱ��
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
