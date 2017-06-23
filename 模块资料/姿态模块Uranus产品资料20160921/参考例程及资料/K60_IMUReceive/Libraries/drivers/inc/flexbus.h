/**
  ******************************************************************************
  * @file    flexbus.h
  * @author  YANDLD
  * @version V3.0
  * @date    2016.2.20
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#ifndef __CH_LIB_FLEXBUS_H__
#define __CH_LIB_FLEXBUS_H__

#include <stdint.h>
#include <stdbool.h>

//!< Flexbus 数据对其方式选择
#define kFLEXBUS_DataLeftAligned   (0x00)  //数据左对齐
#define kFLEXBUS_DataRightAligned  (0x01)  //数据右对齐

//!< Flexbus 自动应答信号使能
#define kFLEXBUS_AutoAckEnable     (0x00)  //自动应答使能
#define kFLEXBUS_AutoAckDisable    (0x01)  //关闭自动应答

//!< Flexbus 端口位宽选择
#define kFLEXBUS_PortSize_8Bit     (0x01)  //端口数据宽度：8位
#define kFLEXBUS_PortSize_16Bit    (0x02)  //端口数据宽度：16位
#define kFLEXBUS_PortSize_32Bit    (0x00)  //端口数据宽度：32位

//!< Flexbus 片选信号选择 
#define kFLEXBUS_CS0               (0x00)  //0号片选信号
#define kFLEXBUS_CS1               (0x01)  //1号片选信号
#define kFLEXBUS_CS2               (0x02)  //2号片选信号
#define kFLEXBUS_CS3               (0x03)  //3号片选信号
#define kFLEXBUS_CS4               (0x04)  //4号片选信号
#define kFLEXBUS_CS5               (0x05)  //5号片选信号

//!< Flexbus 片选范围 参考选择
#define kFLEXBUS_ADSpace_64KByte        (0x00) //外挂SRAM尺寸 64k
#define kFLEXBUS_ADSpace_128KByte       (0x01) //外挂SRAM尺寸 128k
#define kFLEXBUS_ADSpace_512KByte       (0x07) //外挂SRAM尺寸 512k
#define kFLEXBUS_ADSpace_1MByte         (0x0F) //外挂SRAM尺寸 1m
#define kFLEXBUS_ADSpace_2MByte         (0x1F) // 2M
#define kFLEXBUS_ADSpace_4MByte         (0x3F) // 4M

//!< Flexbus BE信号控制模式选择
#define kFLEXBUS_BE_AssertedWrite       (0x00) //BE信号功能：写
#define kFLEXBUS_BE_AssertedReadWrite   (0x01) //BE信号功能：读写

//!< Flexus 控制信号复用选择
#define kFLEXBUS_CSPMCR_GROUP1_ALE      (0x00)
#define kFLEXBUS_CSPMCR_GROUP1_CS1      (0x01)
#define kFLEXBUS_CSPMCR_GROUP1_TS       (0x02)
#define kFLEXBUS_CSPMCR_GROUP2_CS4      (0x00)
#define kFLEXBUS_CSPMCR_GROUP2_TSIZ0    (0x01)
#define kFLEXBUS_CSPMCR_GROUP2_BE_31_24 (0x02)
#define kFLEXBUS_CSPMCR_GROUP3_CS5      (0x00)
#define kFLEXBUS_CSPMCR_GROUP3_TSIZ1    (0x01)
#define kFLEXBUS_CSPMCR_GROUP3_BE_23_16 (0x02)
#define kFLEXBUS_CSPMCR_GROUP4_TBST     (0x00)
#define kFLEXBUS_CSPMCR_GROUP4_CS2      (0x01)
#define kFLEXBUS_CSPMCR_GROUP4_BE_15_8  (0x02)
#define kFLEXBUS_CSPMCR_GROUP5_TA       (0x00)
#define kFLEXBUS_CSPMCR_GROUP5_CS3      (0x01)
#define kFLEXBUS_CSPMCR_GROUP5_BE_7_0   (0x02)

/**
 * \struct FLEXBUS_InitTypeDef
 * \brief FLEXBUS初始化结构体
 */
typedef struct
{
    uint32_t dataWidth;       ///< 数据总线数据宽度
    uint32_t baseAddress;     ///< 设备基地址
    uint32_t ADSpaceMask;     ///< 设备存储空间
    uint32_t dataAlignMode;   ///< 数据对齐方式
    uint32_t autoAckMode;     ///< 自动应答模式
    uint32_t ByteEnableMode;  ///< BE使能模式
    uint32_t CSn;             ///< 片选信号通道
    uint32_t PinGroup1:3;
    uint32_t PinGroup2:3;
    uint32_t PinGroup3:3;
    uint32_t PinGroup4:3;
    uint32_t PinGroup5:3;
}FLEXBUS_Init_t;

/**
 * \struct FLEXBUS_AdvancedConfigTypeDef
 * \brief FLEXBUS 高级配置选项
 */
typedef struct
{
    bool kFLEXBUS_brustWriteEnable; /*!< Burst-Write enable */
    bool kFLEXBUS_brustReadEnable;  /*!< Burst-Read enable */
    bool kFLEXBUS_EXTS;             /*!< Extend transfer start/extend address latch enable */
    bool kFLEXBUS_SWSEN;            /*!< Secondary wait states enable */
    uint32_t kFLEXBUS_SWS:6;        /*!< Secondary wait states number */
    uint32_t kFLEXBUS_ASET:2;       /*!< Address setup setting */
    uint32_t kFLEXBUS_RDAH:2;       /*!< Read address hold or deselect option */
    uint32_t kFLEXBUS_WRAH:2;       /*!< Write address hold or deselect option */
    uint32_t kFLEXBUS_WS:6;         /*!< Value of wait states */
}FLEXBUS_TimingConfig_t;

/* API functions */
void FLEXBUS_Init(FLEXBUS_Init_t* Init);
void FLEXBUS_TimingConfig(uint32_t CS, FLEXBUS_TimingConfig_t* config);

#endif

