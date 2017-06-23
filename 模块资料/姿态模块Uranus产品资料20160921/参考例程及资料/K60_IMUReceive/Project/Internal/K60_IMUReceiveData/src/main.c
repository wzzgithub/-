//#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "mcg.h"
#include "packet.h"

#include <string.h>
#include <stdio.h>

static Packet_t pktB; /* used for data anaysis */
bool DataReceived = false;

uint8_t ID;
int16_t AccRaw[3];
int16_t GyoRaw[3];
int16_t MagRaw[3];
float Eular[3];
int32_t Pressure;

void UART4_RX_TX_IRQHandler(void)
{
    uint8_t ch;
    UART_Type *UARTx = (UART_Type*)UART4;

    if(UARTx->S1 & UART_S1_RDRF_MASK)
    {
        ch = (uint8_t)UARTx->D;
        Packet_Decode(ch);
    }
    
    if(UARTx->S1 & UART_S1_OR_MASK)
    {
        ch = (uint8_t)UARTx->D;
        UARTx->S1 |= UART_S1_OR_MASK;
    }
}

file:///C:/Users/Administrator/AppData/Local/Packages/microsoft.microsoftedge_8wekyb3d8bbwe/AC/#!001/MicrosoftEdge/User/Default/WebNotes/Microsoft-Edge-Web-Notes-?-???(Cov.jpg
void OnDataReceived(Packet_t *pkt)
{
    memcpy(&pktB, pkt, sizeof(Packet_t));
    DataReceived = true;
}


int main(void)
{
    DelayInit();
    DelayMs(10);
    
    /* 使用 D6 D7输出结果 */
    UART_Init(UART0_RX_PD06_TX_PD07, 115200);
    printf("IMU receive demo\r\n");
    
    /* 使用E24 E25接收IMU数据 */
    UART_Init(UART4_RX_PE25_TX_PE24, 115200); 

    Packet_t pktA; /* used for data receive */
    Packet_DecodeInit(&pktA, OnDataReceived);
    
    /* open interrupt */
    UART_SetIntMode(HW_UART4, kUART_IntRx, true);
    
    while(1)
    {
        if(DataReceived == true)
        {
            /* 按出厂默认输出协议接收
            输出 
            0x5A+0xA5+LEN_LOW+LEN_HIGH+CRC_LOW+CRC+HIGH+ 0x90+ID(1字节) + 0xA0+Acc(加速度6字节) + 0xB0+Gyo(角速度6字节) + 0xC0+Mag(地磁6字节) + 0xD0 +AtdE(欧拉角6字节) + 0xF0+Pressure(压力4字节)
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
               printf("Angle:    %0.2f %0.2f %0.2f\r\n", Eular[0], Eular[1], Eular[2]);
//             printf("Pressure: %d Pa\r\n", Pressure);
             DataReceived = false;
         }
    }
}

void HardFault_Handler(void)
{
    printf("HardFault_Handler\r\n");
    while(1);
}
