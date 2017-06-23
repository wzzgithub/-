/**
  ******************************************************************************
  * @file    flexcan.h
  * @author  YANDLD
  * @version V2.5
  * @date    2016.6.6
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#ifndef __CH_LIB_CAN_H__
#define __CH_LIB_CAN_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "common.h"  

/* hardware instances */
#define HW_CAN0  (0x00U)  /* CAN0模块，以下依次类推 */
#define HW_CAN1  (0x01U)    
     

#define CAN_RXFIFO_ID_STD(x)                     (CAN_ID_STD(x) << 1)
#define CAN_RXFIFO_ID_EXT(x)                     (CAN_ID_EXT(x) << 1)

/* CAN QuickInit macro */
#define CAN1_TX_PE24_RX_PE25    (0xB0A1U)
#define CAN0_TX_PA12_RX_PA13    (0x9880U)
#define CAN0_TX_PB18_RX_PB19    (0xA488U)
#define CAN1_TX_PC17_RX_PC16    (0xA091U)
#define CAN1_TX_PB03_RX_PB02    (0x000085C9U)

     
/**
 * \enum CAN_ITDMAConfig_Type
 * \brief CAN 中断DMA配置
 */
typedef enum
{
    kCAN_IntMB,          /**< 发送或接收完成中断*/
}CAN_Int_t;

/* API */
uint32_t CAN_Init(uint32_t MAP, uint32_t baudrate);
void CAN_SetRxMB(uint32_t instance, uint32_t mb, uint32_t id);
void CAN_SetTxMB(uint32_t instance, uint32_t mb);
uint32_t CAN_SendDataFrame(uint32_t instance, uint32_t mb, uint32_t id, uint8_t* buf, uint8_t len);
uint32_t CAN_SendRemoteFrame(uint32_t instance, uint32_t mb, uint32_t id, uint8_t req_len);
void CAN_SetIntMode(uint32_t instance, uint32_t mb, CAN_Int_t mode, bool val);
uint32_t CAN_ReceiveFrame(uint32_t instance, uint32_t mb, uint32_t *id, uint8_t *buf, uint8_t *len, bool *isRemote);
void CAN_SetRxStdFilterMask(uint32_t instance, uint32_t mb, uint32_t mask);
void CAN_SetRxExtFilterMask(uint32_t instance, uint32_t mb, uint32_t mask);
uint32_t CAN_SetBaudrate(uint32_t instance, uint32_t baudrate);

void CAN_SetRxFIFOIntMode(uint32_t instance, bool val);
uint32_t CAN_ReadFIFO(uint32_t instance, uint32_t *id, uint8_t *buf, uint8_t *len, bool *isRemote);
uint32_t CAN_SetRxFIFO(uint32_t instance, uint32_t *filter_table, uint8_t filter_len, bool val);

#ifdef __cplusplus
}
#endif

#endif
