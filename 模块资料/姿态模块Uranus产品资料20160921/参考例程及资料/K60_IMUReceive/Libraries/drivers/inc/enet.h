/**
  ******************************************************************************
  * @file    enet.h
  * @author  YANDLD
  * @version V3.0
  * @date    2016.2.3
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#ifndef __CH_LIB_ENET_H__
#define __CH_LIB_ENET_H__


#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
   
/**
 * \enum ENET_ITDMAConfig_Type
 * \brief ENET ÷–∂œDMA≈‰÷√
 */
typedef enum
{
    kENET_IntTx,
    kENET_IntRx,
}ENET_Int_t;


/* PHY API */
void ENET_PHY_Init(void);
uint32_t ENET_PHY_Read(uint16_t phy_addr, uint16_t reg_addr, uint16_t *data);
uint32_t ENET_PHY_Write(uint16_t phy_addr, uint16_t reg_addr, uint16_t data);

/* Controller API */
void ENET_Init(uint32_t MAP, uint8_t* mac);
uint32_t ENET_SendData(uint8_t *data, uint16_t len);
uint32_t ENET_ReceiveData(uint8_t *data);
void ENET_SetIntMode(ENET_Int_t mode, bool val);
uint32_t ENET_GetRxDataSize(void);

void ENET_SetMacAddr(uint32_t instance, uint8_t *mac);
void ENET_GetMacAddr(uint32_t instance, uint8_t *mac);

#ifdef __cplusplus
}
#endif


#endif


