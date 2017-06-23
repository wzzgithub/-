//SystemInit中系统时钟被设为PLL输出，=72MHZ
#include "BSP.h"



/*   
* 函数名称：BSP_Init   
* 描    述：时钟初始化、硬件初始化   
* 输    入：无   
* 输    出：无   
*/  
void BSPInit(void)    
{ 

	GPIO_Configuration();   	//---IO配置

//	ZIGBEE_ON;				 	//开ZIGBEE供电
//	ZIGBEE_SLP_OFF;		   		//关闭睡眠模式
	
	LCD_ON;												//---LCD开背光

}  
void SysVarInit(void)
{
	
} 
/*   
* 函数名称：SysTick_init   
* 描    述：配置 SysTick 定时器,用于产生系统节拍   
* 输    入：无   
* 输    出：无   
*/  
void SysTick_init(void)    
{    

}
void BAT_ADC(void)
{

	
}
/*************************** 
*函数名称：GPIO_Configurtion
*函数功能：配置GPIO端口
****************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//---打开重映射时钟和IO引脚时钟
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);//---RCC_APB2Periph_AFIO需要重映射的时候才开启USART1需要重映射
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE);	
	
	//GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);   //串口3的完全重映像GPIOC			 
	/************************************LED_GPIO************************************/
	/*状态LED   GPIOB 0 1 */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/*MAIN_LED   GPIOE 0 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOE, &GPIO_InitStructure);
		
//	/*SENSOR_LED   GPIOE 1 */
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
//	GPIO_Init(GPIOE, &GPIO_InitStructure);
//	
	
	/************************************ZIGBEE_GPIO************************************/
	/*ZIGBEE_POWER  GPIOC 7 */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	/*ZIGBEE_RUN/NET/ALM    GPIOD 12/13/14*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 ; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	
//	//ZIGBEE_RXD  GPIOB 10 USART3_TX 非重映射引脚
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	
//	/*ZIGBEE_TXD  GPIOB 11 USART3_RX */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;  
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	/*******************配置GPIOA*******************/
//	/*OPTICAL_TXD  GPIOA2 USART2_TX */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //TXD
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	/*OPTICAL_RXD  GPIOA3 USART2_RX */ 
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //RXD
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOA, &GPIO_InitStructure); 
//	
//	//串口1
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //TXD
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //RXD
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOA, &GPIO_InitStructure); 
		
	/************************************CHR_T_GPIO************************************/
	/*CHR_T   GPIOC 2  *NC */ 
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);

//	/************************************FOR TF************************************/
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;  //TF_CD
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;  //TF_CS
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_15;  //SPI1_SCK, SPI1_MOSI
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   
//	GPIO_Init(GPIOB,&GPIO_InitStructure); 
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;       //SPI1_MISO
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    //上拉输入
//	GPIO_Init(GPIOB,&GPIO_InitStructure);

	/************************************LCD_GPIO************************************/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ;      //TXD5
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

//	/*-------------SDA双向IO--温湿度-------PE2-------------*/
//	/*  单总线  SDA */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //---STM32在输出模式下输入寄存器仍然有效
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	
//	/*ADC*/
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;      //12V_T, battery voltage test
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	//模拟输入
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	
//	
//	/*UART4*/
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //   UART4_TXD
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOC, &GPIO_InitStructure); 
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //    UART4_RXD
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_Init(GPIOC, &GPIO_InitStructure);
//		
//	/*USB*/
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;   
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
//	GPIO_Init(GPIOA, &GPIO_InitStructure);  	
//	
	/*******配置GPIOD***********/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;       //LCD_ON
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);	
//	//GPS_RXD   GPIOD 2 UART5_RX 
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
//	// 门禁 
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;       
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOD,&GPIO_InitStructure); 	
	//薄膜按键 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;       			//ESC   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD,&GPIO_InitStructure);  					
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;       			//DOWN
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;       			//UP
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;       			//OK    用作LCD开关
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD,&GPIO_InitStructure); 
//	/*RS485*/
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;          			//USART3_TXD_REMAP
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;          			//USART3_RXD_REMAP
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//	GPIO_Init(GPIOD, &GPIO_InitStructure); 
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;         //485_DIR, keep low the active receive but high when send.
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//GPIOD15 for ZIGBEE_SLP
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_15;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOD, &GPIO_InitStructure);		
}
/**************************
函数名称：ADC_Configuration	
函数功能：配置ADC
****************************/
//void  ADC_Configuration(void)
//{
//	ADC_InitTypeDef ADC_InitStructure;
//	DMA_InitTypeDef DMA_InitStructure;//
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO, ENABLE);
//	/********************* DMA channel1 configuration *******************/
//	DMA_DeInit(DMA1_Channel1);									//复位DMA通道1
//	DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;	//定义DMA通道外设基地址
//	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_value;		//定义存储地址
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;			//
//	DMA_InitStructure.DMA_BufferSize = 16;
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//源地址固定不变
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;			//目的地址递增
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//外设数据宽度16bit
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//存储器数据宽度16bit
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA通道操作模式位环形缓冲模式
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;//DMA通道优先级高
//	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//禁止DMA通道存储器到存储器传输
//	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
//	/* Enable DMA1 channel1 */
//	DMA_Cmd(DMA1_Channel1, ENABLE);
//	/**********************ADC1 configuration *****************************/
//	ADC_DeInit(ADC1);
//	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
//	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
//	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
//	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
//	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
//	ADC_InitStructure.ADC_NbrOfChannel = 2;
//	ADC_Init(ADC1, &ADC_InitStructure);
//	/* ADC1 regular channels configuration */ 
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 1, ADC_SampleTime_55Cycles5);//ADC1选择信道12,规则采样顺序,采样时间为55.5周期
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 2, ADC_SampleTime_55Cycles5);   
//	/* Enable ADC1 DMA */
//	ADC_DMACmd(ADC1, ENABLE);
//	/* Enable ADC1 */
//	ADC_Cmd(ADC1, ENABLE);
//	/* Enable ADC1 reset calibaration register */   
//	ADC_ResetCalibration(ADC1);
//	/* Check the end of ADC1 reset calibration register */
//	while(ADC_GetResetCalibrationStatus(ADC1));								   
//	/* Start ADC1 calibaration */
//	ADC_StartCalibration(ADC1);
//	/* Check the end of ADC1 calibration */
//	while(ADC_GetCalibrationStatus(ADC1));
//	/* Start ADC1 Software Conversion */ 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
//}
/************************
**函数名称：EXTI_Configuratiuon
**函数功能：配置外部中断
************************/
//void EXTI_Configuration(void)
//{
//	EXTI_InitTypeDef EXTI_InitStructure;
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource3);//ZIGBEE_RUN
//	EXTI_ClearITPendingBit(EXTI_Line3);
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStructure.EXTI_Line = EXTI_Line3;             // | EXTI_Line4 | EXTI_Line9;// | EXTI_Line2 | EXTI_Line3 | EXTI_Line4;
//	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
//	EXTI_Init(&EXTI_InitStructure);

//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource4); //PD4 for ESC
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource5); //PD5 for UP
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource6); //PD6 for DOWN
//	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource7); //PD7 for OK
//	EXTI_InitStructure.EXTI_Line = EXTI_Line4 | EXTI_Line5 | EXTI_Line6 | EXTI_Line7; //选择中断线路4,5,6,7
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	 	//设置为中断请求，非事件请求
//	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);

//}

/**************************
函数名称：NVIC_Configuration
函数功能：配置中断向量
**************************/
//void NVIC_Configuration(void)
//{
//	NVIC_InitTypeDef NVIC_InitStructure;
//	/* Configure the NVIC Preemption Priority Bits */  
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
//	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	// Enable the USART1 Interrupt
//	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	// Enable the USART2 Interrupt 
//	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	//Enable the USART3 Interrupt
//	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	// Enable the UART4 Interrupt
//	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	//Enable the UART5 Interrupt 
//	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

//	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;      //选择中断通道4
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 	//抢占式中断优先级设置为1
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  		//响应式中断优先级设置为1
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //使能中断
//	NVIC_Init(&NVIC_InitStructure);
//	
//	
//	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;  	//选择中断通道5
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 	//抢占式中断优先级设置为2
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  		//响应式中断优先级设置为2
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //使能中断
//	NVIC_Init(&NVIC_InitStructure);

//}
/************************
函数名称：IWDG_Configuration
函数功能：配置看门狗定时器
*************************/
//void IWDG_Configuration(void)
//{
//	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 	/* 写入0x5555,用于允许狗狗寄存器写入功能 */
//	IWDG_SetPrescaler(IWDG_Prescaler_256);            /* 内部低速时钟256分频 40K/256=156HZ(6.4ms) */ 
//	/* 看门狗定时器溢出时间 */
//	IWDG_SetReload(1875);							    /* 喂狗时间 5s/6.4MS=781 .注意不能大于0xfff*/
//	IWDG_ReloadCounter();								/* 喂狗*/
//	IWDG_Enable(); 									/* 使能看门狗*/
//}
/***********************
**函数名称:Time
**函数功能：
***********************/
//void Time_init(void)
//{
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
//	TIM_TimeBaseStructure.TIM_Period 		= 100;					//0.05s中断率
//	TIM_TimeBaseStructure.TIM_Prescaler 	= 36000 - 1;	  		//时钟预分频系数36000，时钟频率=72MHZ/(时钟预分频+1),所以定时器时钟为2K
//	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
//	TIM_TimeBaseStructure.TIM_CounterMode	= TIM_CounterMode_Up;	//设置定时器向上计数
//	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);					//配置定时器2
//	TIM_ClearFlag(TIM2, TIM_FLAG_Update); 
//	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);						//允许更新中断
//	TIM_Cmd(TIM2, ENABLE);											//使能

////	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
////	TIM_TimeBaseStructure.TIM_Period= 9999;
////	TIM_TimeBaseStructure.TIM_Prescaler= 719;
////	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
////	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
////	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
////	TIM_ClearFlag(TIM5, TIM_FLAG_Update); 
////	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);						//允许更新中断	
//}
