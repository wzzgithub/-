/**
  ******************************************************************************
  * @file    crc.c
  * @author  YANDLD
  * @version V3.0
  * @date    2016.6.3
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "crc.h"
#include "common.h"

#if (defined(CRC0))

typedef enum
{
    kCRCNoTranspose    = 0U, /**< No transposition. @internal gui name="No Transpose" */
    kCRCTransposeBits  = 1U, /**< Bits in bytes are transposed; bytes are not transposed. @internal gui name="Transpose Bits" */
    kCRCTransposeBoth  = 2U, /**< Both bits in bytes and bytes are transposed. @internal gui name="Transpose Bits in Bytes and Bytes" */
    kCRCTransposeBytes = 3U  /**< Only bytes are transposed; no bits in a byte are transposed. @internal gui name="Transpose Bytes" */
}CRC_Transpose_Type;


typedef struct
{
    uint32_t name;                      /* name */
    uint8_t crcWidth;                   /* 16 bit or 32 bit */
    uint32_t seed;                      /* initialize value */
    uint32_t polynomial;                /* polynomial */
    CRC_Transpose_Type writeTranspose;  /* Defines transpose configuration of the data written to the CRC data register */
    CRC_Transpose_Type readTranspose;   /* Defines transpose configuration of the value read from the CRC data register */
    bool complementRead;                /* Enables complement read of CRC data register */
} CRC_t;


static CRC_t CRCTbl[] = 
{
    {kCRC16_IBM,        16, 0x0000U,        0x8005,     kCRCTransposeBits,  kCRCTransposeBoth,  false},     /* 16BIM */
    {kCRC16_MAXIM,      16, 0x0000U,        0x8005,     kCRCTransposeBits,  kCRCTransposeBoth,  true},      /* 16MAXIM */
    {kCRC16_USB,        16, 0xFFFFU,        0x8005,     kCRCTransposeBits,  kCRCTransposeBoth,  true},      /* 16USB */
    {kCRC16_MODBUS,     16, 0xFFFFU,        0x8005,     kCRCTransposeBits,  kCRCTransposeBoth,  false},     /* 16 MODBUS */
    {kCRC16_CCITT,      16, 0x0000U,        0x1021,     kCRCTransposeBits,  kCRCTransposeBoth,  false},     /* 16 CCITT */
    {kCRC16_CCITTFalse, 16, 0xFFFFU,        0x1021,     kCRCNoTranspose,    kCRCNoTranspose,    false},     /* 16 CCITT FALSE */
    {kCRC16_X25,        16, 0xFFFFU,        0x1021,     kCRCTransposeBits,  kCRCTransposeBoth,  true},      /* 16 X25 */
    {kCRC16_XMODEM,     16, 0x0000U,        0x1021,     kCRCNoTranspose,    kCRCNoTranspose,    false},     /* 16 XMODEM */
    {kCRC16_DNP,        16, 0x0000U,        0x3D65,     kCRCTransposeBits,  kCRCTransposeBoth,  true},      /* 16 DNP */
    {kCRC32,            32, 0xFFFFFFFFU,    0x04C11DB7, kCRCTransposeBits,  kCRCTransposeBoth,  true},      /* 32 */
    {kCRC32_MPEG2,      32, 0xFFFFFFFFU,    0x04C11DB7, kCRCNoTranspose,    kCRCNoTranspose,    false},     /* CRC32-MPEG2 */
};

#if defined(SIM_SCGC6_CRC_MASK)
static const Reg_t CRCClkGate[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_CRC_MASK},

};
#elif defined(PCC0_PCC_CRC_CGC_MASK)
static const Reg_t CRCClkGate[] =
{
    {(void*)&(PCC0->PCC_CRC), PCC0_PCC_CRC_CGC_MASK},
};
#endif

 /**
 * @brief  初始化配置CRC冗余校验
 * @note   None
 * @param  mode:
 *         @arg CRCTbl[] : 工作模式选择，详见crc.h
 * @retval None
 */
void CRC_Init(CRC_Protocol_t mode)
{
    /* enable clock */
    REG_SET(CRCClkGate, 0);
    
    CRC_t *crc = &CRCTbl[mode];
    /* crc width */
    (crc->crcWidth == 16)?
    (CRC0->CTRL &= ~CRC_CTRL_TCRC_MASK):
    (CRC0->CTRL |= CRC_CTRL_TCRC_MASK);
    
    /* set transpose and complement options */
    CRC0->CTRL &= ~CRC_CTRL_TOTR_MASK;
    CRC0->CTRL |= CRC_CTRL_TOTR(crc->readTranspose);
    
    CRC0->CTRL &= ~CRC_CTRL_TOT_MASK;
    CRC0->CTRL |= CRC_CTRL_TOT(crc->writeTranspose);
    
    (crc->complementRead)?
    (CRC0->CTRL |= CRC_CTRL_FXOR_MASK):
    (CRC0->CTRL &= ~CRC_CTRL_FXOR_MASK);
    
    /* Write polynomial */
    CRC0->GPOLY = crc->polynomial;
    
    /* 4. Set seed value */
    CRC0->CTRL |= CRC_CTRL_WAS_MASK;
#ifdef CRC_DATAL_DATAL_MASK
    CRC0->DATA = crc->seed;
#else
    CRC0->CRC = crc->seed;
#endif
    CRC0->CTRL &= ~CRC_CTRL_WAS_MASK;
}

 /**
 * @brief  读取CRC校验数据
 * @note   None
 * @retval 返回接收的CRC的数据
 */
uint32_t CRC_Read(void)
{
    uint32_t result = 0;
    CRC_Transpose_Type transpose;
    if(CRC0->CTRL & CRC_CTRL_TCRC_MASK)
    {
        /* 32 */
#ifdef CRC_DATAL_DATAL_MASK
        result = CRC0->DATA;
#else
        result = CRC0->CRC;
#endif
    }
    else
    {
        /* 16 */
        transpose = (CRC_Transpose_Type)((CRC0->CTRL & CRC_CTRL_TOTR_MASK) >> CRC_CTRL_TOTR_SHIFT);    
        if((transpose == kCRCTransposeBoth) || (transpose == kCRCTransposeBytes))
        {
            /* Return upper 16bits of CRC because of transposition in 16bit mode */
#ifdef CRC_DATAL_DATAL_MASK
            result = CRC0->ACCESS16BIT.DATAH;
#else
            result = CRC0->ACCESS16BIT.CRCH;
#endif
        }
        else
        {
#ifdef CRC_DATAL_DATAL_MASK
            result = CRC0->ACCESS16BIT.DATAL;
#else
            result = CRC0->ACCESS16BIT.CRCL;
#endif
        }
    }
    return result;
}

/**
 * @brief  写数据到CRC
 * @note   None
 * @param  buf: 准备发送的数据
 * @param  len: 数据的字节长度
 * @retval CH_OK：成功；其它：失败
 */
uint32_t CRC_Write(uint8_t* buf, uint32_t len)
{
    CRC_Transpose_Type oldInputTranspose;
    uint32_t *data32;
    uint8_t *data8;
    data32 = (uint32_t *)buf;

    /* flip bytes because of little endian architecture */
    oldInputTranspose = (CRC_Transpose_Type)((CRC0->CTRL & CRC_CTRL_TOT_MASK) >> CRC_CTRL_TOT_SHIFT);
    
    CRC0->CTRL &= ~CRC_CTRL_TOT_MASK;
    switch (oldInputTranspose)
    {
        case kCRCNoTranspose:
            CRC0->CTRL |= CRC_CTRL_TOT(kCRCTransposeBytes);
             break;
        case kCRCTransposeBits:
            CRC0->CTRL |= CRC_CTRL_TOT(kCRCTransposeBoth);
             break;
        case kCRCTransposeBoth:
            CRC0->CTRL |= CRC_CTRL_TOT(kCRCTransposeBits);
             break;
        case kCRCTransposeBytes:
            CRC0->CTRL |= CRC_CTRL_TOT(kCRCNoTranspose);
             break;
        default:
             break;
    }
    
    /* Start the checksum calculation */
    while (len >= sizeof(uint32_t))
    {
#ifdef CRC_DATAL_DATAL_MASK
        CRC0->DATA = *(data32++);
#else
        CRC0->CRC = *(data32++);
#endif
        len -= sizeof(uint32_t);
    }

    data8 = (uint8_t *)data32;

    switch(len)
    {
        case 3U:
#ifdef CRC_DATAL_DATAL_MASK
            CRC0->ACCESS16BIT.DATAL =  *(uint16_t *)data8;  /* 16 bit */
            CRC0->ACCESS8BIT.DATALL = *(data8+2U);          /* 8 bit */  
#else
            CRC0->ACCESS16BIT.CRCL =  *(uint16_t *)data8;  /* 16 bit */
            CRC0->ACCESS8BIT.CRCLL = *(data8+2U);          /* 8 bit */  
#endif        
            break;
        case 2U:
#ifdef CRC_DATAL_DATAL_MASK   
            CRC0->ACCESS16BIT.DATAL = *(uint16_t *)data8;   /* 16 bit */
        
#else
            CRC0->ACCESS16BIT.CRCL = *(uint16_t *)data8;   /* 16 bit */
#endif
            break;
        case 1U:
#ifdef CRC_DATAL_DATAL_MASK   
            CRC0->ACCESS8BIT.DATALL = *data8;               /* 8 bit */    
#else
            CRC0->ACCESS8BIT.CRCLL = *data8;               /* 8 bit */    
#endif
            break;
        default:
             break;
    }

    CRC0->CTRL &= ~CRC_CTRL_TOT_MASK;
    CRC0->CTRL |= CRC_CTRL_TOT(oldInputTranspose);

    return CH_OK;
}

#if (__CORTEX_M != 0x00)

/**
 * @brief  通过软件产生CRC16
 * @note   None
 * @param  mode:
 *         @arg CRCTbl[] : 工作模式选择
 * @param  buf: 准备发送的数据
 * @param  len: 数据的字节长度
 * @retval 产生的CRC
 */
uint32_t CRC16_SoftGenerate(uint32_t mode, uint8_t *buf, uint32_t len)
{
    CRC_t *crc_param = &CRCTbl[mode];
    uint32_t crc = crc_param->seed;
    uint32_t j;  
    for (j = 0; j < len; ++j)
    {
        uint32_t i;
        uint32_t byte = buf[j];
        
        if(crc_param->writeTranspose)
        {
            byte = __RBIT(byte)>>24;
        }
        
        crc ^= byte << 8;
        for (i = 0; i < 8; ++i)
        {
            uint32_t temp = crc << 1;
            if (crc & 0x8000)
            {
                temp ^= crc_param->polynomial;
            }
            crc = temp;
        }
    }
    
    if(crc_param->readTranspose)
    {
        crc = __RBIT(crc)>>16;
    }
    
    if(crc_param->complementRead)
    {
        crc ^= 0xFFFF;
    }
    
    crc &= 0x0000FFFF;
    
    return crc;
}

#endif

#endif

