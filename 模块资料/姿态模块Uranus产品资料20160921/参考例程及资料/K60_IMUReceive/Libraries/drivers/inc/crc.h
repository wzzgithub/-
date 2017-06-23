/**
  ******************************************************************************
  * @file    crc.h
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.6.3
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#ifndef __CH_LIB_CRC_H__
#define __CH_LIB_CRC_H__

#include <stdint.h>
#include <stdbool.h>

/* CRC ģʽ */
typedef enum
{
    kCRC16_IBM,             /**< */
    kCRC16_MAXIM,           /**< */
    kCRC16_USB,             /**< */
    kCRC16_MODBUS,          /**< */
    kCRC16_CCITT,           /**< */
    kCRC16_CCITTFalse,      /**< */
    kCRC16_X25,             /**< */
    kCRC16_XMODEM,          /**< */
    kCRC16_DNP,             /**< */
    kCRC32,                 /**< */
    kCRC32_MPEG2,           /**< */
}CRC_Protocol_t;


/* API */
void CRC_Init(CRC_Protocol_t mode);
uint32_t CRC_Write(uint8_t* buf, uint32_t len);
uint32_t CRC_Read(void);
uint32_t CRC16_SoftGenerate(uint32_t mode, uint8_t *buf, uint32_t len);



#endif
