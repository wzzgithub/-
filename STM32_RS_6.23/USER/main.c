#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "packet.h"
#include <string.h>
#include "ds18b20.h" 
#include "spi.h"
#include "dac.h"
 
#define ACK                     (0x06)
static Packet_t pktB; /* used for data anaysis */
bool DataReceived = true;

uint8_t ID;
int16_t AccRaw[3];
int16_t GyoRaw[3];
int16_t MagRaw[3];
float Eular[3];		//��̬
short temperature;   //�¶�
int32_t Pressure;
unsigned char COM6_RecvBuf[20];
unsigned char COM6_RecvBufBck[20];
int COM6_RecvFin = 0;

char data_recv_ab[3]={0xab,0x1e};//ab ��������  �����ظ����ڣ������ȣ���������
char data_recv_ac[5];//ac ��������

/**
  * @brief  Print a character on the HyperTerminal
  * @param  c: The character to be printed
  * @retval None
  */
void SerialPutChar1(uint8_t c)
{
  USART_SendData(USART3, c);
  while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
  {
  }
}
/**
  * @brief  Send a byte
  * @param  c: Character
  * @retval 0: Byte sent
  */
static uint32_t Send_Byte1 (uint8_t c)
{
	SerialPutChar1(c);
	return 0;
}

int32_t Receive_Byte (uint8_t *c, uint32_t timeout)
{
	while (timeout-- > 0) 
  { 
    if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==SET)  
    {  
     *c =USART_ReceiveData(USART1); 
			
			return 0;
		}
	}
	return 0;
}


void OnDataReceived(Packet_t *pkt)
{
    memcpy(&pktB, pkt, sizeof(Packet_t));
    DataReceived = true;
}

void USART3_IRQHandler(void)
{
	unsigned char rCh;
	static char rCnt = 0;
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET)
	{
		rCh = USART_ReceiveData(USART3); 
		 COM6_RecvBuf[rCnt] = rCh;  
        if(rCnt == 0)     //֡ͷ0xAA      
        {  
            rCnt = (0x01 != rCh)?0:rCnt+1;  
        }  
        else if(rCnt == 1) //֡ͷ0x55    
        {  
        rCnt = (0x02 != rCh)?0:rCnt+1;  
        }  
        else if(rCnt == 2) //����len  
        {  
        rCnt++;  
            }             
        else if(rCnt > 2) //ֵvalue  
        {  
        rCnt++;  
        if(rCnt == 4+COM6_RecvBuf[2])  
        {               
            rCnt = 0;  
            memcpy(COM6_RecvBufBck,COM6_RecvBuf,80);//���� 
            COM6_RecvFin = 1;  //֪ͨ��ѭ������ 
        }  
         }  
	}
	
} 
void UART5_IRQHandler(void) //����9������
{
	u8 res;	
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
	{	 
		res=USART_ReceiveData(UART5); 
		Packet_Decode(res);
	}
} 
//fun:������. 0XA0~0XAF
//data:���ݻ����� ���28�ֽ�
//len:data����Ч���ݸ���
void uart_send(char fun,char*data,int len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//���28�ֽڻ���
	send_buf[len+3]=0;	//У��������
	send_buf[0]=0X02;	//֡ͷ
	send_buf[1]=fun;	//֡ͷ
	send_buf[2]=len;	//���ݳ���
	for(i=0;i<len;i++)
	{
		send_buf[3+i]=data[i];			//��ֵ����
		printf("com1:%x \n",data[i]);
	}
	for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];	//����У���	
	for(i=0;i<len+4;i++)Send_Byte1(send_buf[i]);	//�������ݵ�����
}


/***CRC����*****/
uint16_t UpdateCRC16(uint16_t crcIn, uint8_t byte)
{
 uint32_t crc = crcIn;
 uint32_t in = byte|0x100;
 do
 {
 crc <<= 1;
 in <<= 1;
 if(in&0x100)
 ++crc;
 if(crc&0x10000)
 crc ^= 0x1021;
 }
 while(!(in&0x10000));
 return crc&0xffffu;
}


/**
  * @brief  Cal CRC16 for YModem Packet
  * @param  data
  * @param  length
   * @retval None
  */
uint16_t Cal_CRC16(const uint8_t* data, uint32_t size)
{
 uint32_t crc = 0;
 const uint8_t* dataEnd = data+size;
 while(data<dataEnd)
  crc = UpdateCRC16(crc,*data++);
 
 crc = UpdateCRC16(crc,0);
 crc = UpdateCRC16(crc,0);
 return crc&0xffffu;
}

void  Test_data()
{
	u8 t=0;
	Packet_t pktA; /* used for data receive */
	Packet_DecodeInit(&pktA, OnDataReceived);
	//while(1)
   // {
				if(t%10==0)			//ÿ100ms��ȡһ��
				{	
					temperature=DS18B20_Get_Temp();
					//printf("%d.%d\n",temperature/10,temperature%10);		   
				}	
				delay_ms(1000);
				t++;
				if(t==20)
					t=0;
			
        if(DataReceived == true)
        {
						delay_ms(100);
            /* ������Ĭ�����Э�����
            ��� 
            0x5A+0xA5+LEN_LOW+LEN_HIGH+CRC_LOW+CRC+HIGH+ 0x90+ID(1�ֽ�) + 0xA0+Acc(���ٶ�6�ֽ�) + 0xB0+Gyo(���ٶ�6�ֽ�) + 0xC0+Mag(�ش�6�ֽ�) + 0xD0 +AtdE(ŷ����6�ֽ�) + 0xF0+Pressure(ѹ��4�ֽ�)
            */
            if(pktB.buf[0] == kItemID) /* user ID */
            {
                ID = pktB.buf[1];
            }
             
            if(pktB.buf[2] == kItemAccRaw)  /* Acc raw value */
            {
                memcpy(AccRaw, &pktB.buf[3], 6);
            }
             
            if(pktB.buf[9] == kItemGyoRaw)  /* gyro raw value */
            {
                memcpy(GyoRaw, &pktB.buf[10], 6);
            }
             
            if(pktB.buf[16] == kItemMagRaw)  /* mag raw value */
            {
                memcpy(MagRaw, &pktB.buf[17], 6);
            }
             
            if(pktB.buf[23] == kItemAtdE)  /* atd E */
            {
                Eular[0] = ((float)(int16_t)(pktB.buf[24] + (pktB.buf[25]<<8)))/100;
                Eular[1] = ((float)(int16_t)(pktB.buf[26] + (pktB.buf[27]<<8)))/100;
                Eular[2] = ((float)(int16_t)(pktB.buf[28] + (pktB.buf[29]<<8)))/10;
            }
            
            if(pktB.buf[30] == kItemPressure)
            {
                memcpy(&Pressure, &pktB.buf[31], 4);
            }
             
//             printf("AccRaw: %d %d %d\r\n", AccRaw[0], AccRaw[1], AccRaw[2]);
//             printf("GyoRaw: %d %d %d\r\n", GyoRaw[0], GyoRaw[1], GyoRaw[2]);
//             printf("MagRaw: %d %d %d\r\n", MagRaw[0], MagRaw[1], MagRaw[2]);
               //printf("Angle:    %0.2f %0.2f %0.2f\r\n", Eular[0], Eular[1], Eular[2]);
//             printf("Pressure: %d Pa\r\n", Pressure);
             DataReceived = false;
       //  }
			 }
}


void spi_receive()
{
	u8* pBuffer;
	u8 time;
	int i;
	SPI2_ReadWriteByte(0xAA);//aa+����������ֽ�,���ֽ�
	SPI2_ReadWriteByte(data_recv_ab[1]);
	SPI2_ReadWriteByte(0x00);
	delay_ms(10000);
	for(i = 1;i < 5;i++)
	{
		*pBuffer=SPI2_ReadWriteByte(0XFF);   	//ѭ������
		data_recv_ac[i] = *pBuffer;
	}
	if(data_recv_ac[1]==1)
	{
		time = data_recv_ac[2] | (data_recv_ac[3]<<8) | (data_recv_ac[4]<<16);
		Send_Byte1(time);
	}
	else
		Send_Byte1(0x00);
	
}

void spi_send()
{
		char cycle;
		SPI2_ReadWriteByte(0x55);//����ʱ55+һ��cycle(2�ֽ�)(�������*200(CPLD����/Ƶ��))
		SPI2_ReadWriteByte(data_recv_ab[1]);
		
	
}

int main(void)
{		
	
	char handshake1[20]={0xaa};//aa ����
//	int  test_data[20];
	//uint16_t tempCRC;
	//uint16_t i;
 
	//u16 dacval = 0;
	
	//NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x3000);
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
  delay_init();	    	 //��ʱ������ʼ��	  
	delay_ms(10);
	DS18B20_Init();
	uart_init(9600);	 //���ڳ�ʼ��Ϊ115200
	uart1_init(9600);
	uart5_init(9600);
	Dac1_Init();
	SPI2_Init();		   	//��ʼ��SPI
	SPI2_SetSpeed(SPI_BaudRatePrescaler_16);//����Ϊ8Mʱ��,����ģʽ
	delay_ms(100);
	
	DAC_SetChannel1Data(DAC_Align_12b_R,400);
	printf("cycl");
  /*while(1)
	{  
		//uart_send(0x01,handshake1,2);
		if(COM6_RecvFin == 1)
		{
			switch(COM6_RecvBufBck[3])
			{
				case 0xaa:			//�յ�aaˮ�Ϸ��������ź�
					uart_send(0x01,handshake1,1);
					COM6_RecvFin = 0;
					break;
				case 0xab:			//��������
					memset(data_recv_ab,0,3);
					memcpy(data_recv_ab,COM6_RecvBufBck+4,3);
					uart_send(0x01,data_recv_ab,3);
					COM6_RecvFin = 0;
					break;
				case 0xac:			//����ģʽ
					spi_receive();
					uart_send(0x01,data_recv_ac,3);
					COM6_RecvFin = 0;
					break;	
				case 0xad:    //����ģʽ
					spi_send();
			}
		}*/
		while(1)
		{
			spi_send();
			delay_ms(20000);
		
			delay_ms(20000);
			//delay_ms(20000);
			//delay_ms(20000);
			//delay_ms(20000);
			//delay_ms(800);
			spi_receive();
			delay_ms(20000);
		}

  
	//}
}
