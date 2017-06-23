/**
  ******************************************************************************
  * @file    enet.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.3.26
  * @date    2015.10.08 FreeXc 完善了enet模块的相关注释
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */

#include "enet.h"
#include "common.h"
#include <string.h>

#if defined(ENET)

/* Control and status bit masks of the transmit buffer descriptor.  */
#define ENET_BUFFDESCRIPTOR_TX_READY_MASK 0x8000U       /*!< Ready bit mask. */
#define ENET_BUFFDESCRIPTOR_TX_SOFTOWENER1_MASK 0x4000U /*!< Software owner one mask. */
#define ENET_BUFFDESCRIPTOR_TX_WRAP_MASK 0x2000U        /*!< Wrap buffer descriptor mask. */
#define ENET_BUFFDESCRIPTOR_TX_SOFTOWENER2_MASK 0x1000U /*!< Software owner two mask. */
#define ENET_BUFFDESCRIPTOR_TX_LAST_MASK 0x0800U        /*!< Last BD of the frame mask. */
#define ENET_BUFFDESCRIPTOR_TX_TRANMITCRC_MASK 0x0400U  /*!< Transmit CRC mask. */

/* Control and status region bit masks of the receive buffer descriptor. */
#define ENET_BUFFDESCRIPTOR_RX_EMPTY_MASK 0x8000U       /*!< Empty bit mask. */
#define ENET_BUFFDESCRIPTOR_RX_SOFTOWNER1_MASK 0x4000U  /*!< Software owner one mask. */
#define ENET_BUFFDESCRIPTOR_RX_WRAP_MASK 0x2000U        /*!< Next buffer descriptor is the start address. */
#define ENET_BUFFDESCRIPTOR_RX_SOFTOWNER2_Mask 0x1000U  /*!< Software owner two mask. */
#define ENET_BUFFDESCRIPTOR_RX_LAST_MASK 0x0800U        /*!< Last BD of the frame mask. */
#define ENET_BUFFDESCRIPTOR_RX_MISS_MASK 0x0100U        /*!< Received because of the promiscuous mode. */
#define ENET_BUFFDESCRIPTOR_RX_BROADCAST_MASK 0x0080U   /*!< Broadcast packet mask. */
#define ENET_BUFFDESCRIPTOR_RX_MULTICAST_MASK 0x0040U   /*!< Multicast packet mask. */
#define ENET_BUFFDESCRIPTOR_RX_LENVLIOLATE_MASK 0x0020U /*!< Length violation mask. */
#define ENET_BUFFDESCRIPTOR_RX_NOOCTET_MASK 0x0010U     /*!< Non-octet aligned frame mask. */
#define ENET_BUFFDESCRIPTOR_RX_CRC_MASK 0x0004U         /*!< CRC error mask. */
#define ENET_BUFFDESCRIPTOR_RX_OVERRUN_MASK 0x0002U     /*!< FIFO overrun mask. */
#define ENET_BUFFDESCRIPTOR_RX_TRUNC_MASK 0x0001U       /*!< Frame is truncated mask. */


/* MII接口超时 */
#define MII_TIMEOUT		0x1FFFF

/* 以太帧相关定义 */
#define CFG_NUM_ENET_TX_BUFFERS     1     /* 发送缓冲区个数 */
#define CFG_NUM_ENET_RX_BUFFERS     1     /* 接收缓冲区个数  驱动程序设定必须为1了 改了就会出错 */
#define CFG_ENET_BUFFER_SIZE        1518    /* 以太发送帧缓冲区长度 */

/**
 * \struct NBUF
 * \brief 缓冲区描述符结构体
 */
typedef struct
{
    uint16_t length;  /*!< Buffer descriptor data length. */
    uint16_t control; /*!< Buffer descriptor control and status. */
    uint8_t *buffer;  /*!< Data buffer pointer. */
} NBUF;

//!< ENET DMA Tx&Rx Descriptors memory regin(must be 16 bit agiged)
ALIGN(16) static uint8_t ENET_TxDescriptors[(CFG_NUM_ENET_TX_BUFFERS*sizeof(NBUF))];
ALIGN(16) static   uint8_t ENET_RxDescriptors[(CFG_NUM_ENET_RX_BUFFERS*sizeof(NBUF))];
static NBUF *ENET_TxDescriptor;
static NBUF *ENET_RxDescriptor;

//!< enet received memory pool
ALIGN(16) static  uint8_t ENET_RxBuffer[CFG_NUM_ENET_RX_BUFFERS * CFG_ENET_BUFFER_SIZE];

static void _REV32(uint32_t *p)
{
    /* the DBSWP is not set!, it's a Z version silicon, we must softwarely swap the buffer: big endiness -> little endiness */
    if((ENET->ECR & ENET_ECR_DBSWP_MASK) == 0)
    {
        *p = __REV(*p);
    }
}

static void ENET_BDInit(void)
{
    int i;

	ENET_TxDescriptor = (NBUF*) ENET_TxDescriptors;
	ENET_RxDescriptor = (NBUF*) ENET_RxDescriptors;
    
    /* Tx Descriptor settings */
	for( i = 0; i < CFG_NUM_ENET_TX_BUFFERS; i++ )
	{
        ENET_TxDescriptor[i].length = 0;
        ENET_TxDescriptor[i].control = 0;
    }
    
	/* Rx Descriptor settings */
	for( i = 0; i < CFG_NUM_ENET_RX_BUFFERS; i++ )
	{
	    ENET_RxDescriptor[i].control = ENET_BUFFDESCRIPTOR_RX_EMPTY_MASK;
	    ENET_RxDescriptor[i].length = 0;
        ENET_RxDescriptor[i].buffer = ENET_RxBuffer;
	}
    
	/* set last Descriptor as a ring */
    ENET_TxDescriptor[CFG_NUM_ENET_TX_BUFFERS - 1].control |= ENET_BUFFDESCRIPTOR_TX_WRAP_MASK;
	ENET_RxDescriptor[CFG_NUM_ENET_RX_BUFFERS - 1].control |= ENET_BUFFDESCRIPTOR_RX_WRAP_MASK;
    
    _REV32((uint32_t*)&ENET_TxDescriptor[0].length);
    _REV32((uint32_t*)&ENET_TxDescriptor[0].buffer);
    _REV32((uint32_t*)&ENET_RxDescriptor[0].length);
    _REV32((uint32_t*)&ENET_RxDescriptor[0].buffer);
    
}

/**
 * @brief   设置ENET模块的接收MAC地址
 * @note  	内部函数
 * @param[in] pa  MAC地址
 * @retval  None
 */
void ENET_SetMacAddr(uint32_t instance, uint8_t *mac)
{
    ENET->PALR = (uint32_t)((mac[0]<<24) | (mac[1]<<16) | (mac[2]<<8) | mac[3]);
    ENET->PAUR = (uint32_t)((mac[4]<<24) | (mac[5]<<16));
}

void ENET_GetMacAddr(uint32_t instance, uint8_t *mac)
{
    uint32_t address;

    /* Get from physical address lower register. */
    address = ENET->PALR;
    mac[0] = 0xFFU & (address >> 24U);
    mac[1] = 0xFFU & (address >> 16U);
    mac[2] = 0xFFU & (address >> 8U);
    mac[3] = 0xFFU & address;

    /* Get from physical address high register. */
    address = (ENET->PAUR & ENET_PAUR_PADDR2_MASK) >> ENET_PAUR_PADDR2_SHIFT;
    mac[4] = 0xFFU & (address >> 8U);
    mac[5] = 0xFFU & address;
}

/**
 * @brief  初始化以太网 PHY
 * @retval None
 */
void ENET_PHY_Init(void)
{
    uint32_t i;
    uint32_t clock;
    
	/* enable the ENET clock. */
    SIM->SCGC2 |= SIM_SCGC2_ENET_MASK;

    /* select external clock(50Mhz) */
	MCG->C2 &= ~MCG_C2_EREFS0_MASK;
    
    /* enable OSCERCLK output */
#ifdef OSC0
    OSC0->CR |= OSC_CR_ERCLKEN_MASK;
#else
    OSC->CR |= OSC_CR_ERCLKEN_MASK;
#endif

    clock = GetClock(kBusClock);
    i = (clock/1000)/1000;
    ENET->MSCR = ENET_MSCR_MII_SPEED((2*i/5)+1) | ENET_MSCR_HOLDTIME(3);
}

/**
 * @brief  以太网 PHY写入数据
 * @param[in]   phy_addr  PHY地址
 * @param[in]   reg_addr  寄存器地址
 * @param[in]   data      写入的数据
 * @retval  CH_OK : 成功
 * \retval  其他  : 失败
 */
uint32_t ENET_PHY_Write(uint16_t phy_addr, uint16_t reg_addr, uint16_t data)
{
    bool ret;
    uint32_t timeout;
    
    ret = CH_ERR;
    
    /* clear MII it pending bit */
    ENET->EIR |= ENET_EIR_MII_MASK;
    
    /* initiatate the MII Management write */
    ENET->MMFR = 0
            | ENET_MMFR_ST(0x01)
            | ENET_MMFR_OP(0x01)
            | ENET_MMFR_PA(phy_addr)
            | ENET_MMFR_RA(reg_addr)
            | ENET_MMFR_TA(0x02)
            | ENET_MMFR_DATA(data);
    
    /* waitting for transfer complete */
    for (timeout = 0; timeout < MII_TIMEOUT; timeout++)
    {
        if (ENET->EIR & ENET_EIR_MII_MASK)
        {
            break;  
        }
    }
    
    if(timeout == MII_TIMEOUT)
    {
        LIB_TRACE("MII write:%d\r\n", timeout); 
    }
    else
    {
        ret = CH_OK;
    }
    
    ENET->EIR |= ENET_EIR_MII_MASK;
    return ret;
}

/**
 * @brief  以太网 PHY读取数据
 * @param[in]   phy_addr  PHY地址
 * @param[in]   reg_addr  寄存器地址
 * @param[in]   data      写入的数据
 * @retval  CH_OK : 成功
 * \retval  其他  : 失败
 */
uint32_t ENET_PHY_Read(uint16_t phy_addr, uint16_t reg_addr, uint16_t *data)
{
    uint32_t ret;
    uint32_t timeout;
    
    ret = CH_ERR;
    
    /* clear MII IT(interrupt) pending bit */
    ENET->EIR |= ENET_EIR_MII_MASK;
    
    /* initiatate the MII Management write */
    ENET->MMFR = 0
            | ENET_MMFR_ST(0x01)
            | ENET_MMFR_OP(0x02)
            | ENET_MMFR_PA(phy_addr)
            | ENET_MMFR_RA(reg_addr)
            | ENET_MMFR_TA(0x02);
  
	/* waitting for transfer complete */
    for (timeout=0; timeout<MII_TIMEOUT; timeout++)
    {
        if (ENET->EIR & ENET_EIR_MII_MASK)
        {
            break; 
        }
    }
    if(timeout == MII_TIMEOUT)
    {
        LIB_TRACE("MII read failed, timeout:%d\r\n", timeout); 
    }
    else
    {
        ret = CH_OK;
    }
    
    *data = ENET->MMFR & 0x0000FFFF;
    ENET->EIR |= ENET_EIR_MII_MASK;
    return ret;
}

/**
 * @brief   初始化以太网模块
 * @note    用户调用函数
 * @param[in]   MAP  以太网初始化结构
 * @param[in]   mac  Mac地址
 * @retval  None
 */
void ENET_Init(uint32_t MAP, uint8_t* mac)
{
    uint32_t i;
    map_t * pq = (map_t*)&(MAP);
    uint32_t instance = pq->ip;
    
    SIM->SCGC2 |= SIM_SCGC2_ENET_MASK;
    /* FSL: allow concurrent access to MPU controller. Example: ENET uDMA to SRAM, otherwise bus error */
    MPU->CESR = 0;
	ENET->ECR = ENET_ECR_RESET_MASK;
    for(i=0; i<1000; i++)
    {
        __NOP();
    }
    
    ENET->ECR = ENET_ECR_DBSWP_MASK;
    /* reinit enet MII */
    ENET_PHY_Init();
    
    /* init buffer desipicter */
    ENET_BDInit();

    ENET->IALR = 0;
    ENET->IAUR = 0;
    ENET->GALR = 0;
    ENET->GAUR = 0;
    
    /* set mac address */
    ENET_SetMacAddr(instance, mac);
    
    /* config Tx and Rx defualt attribute */
    ENET->RCR = ENET_RCR_MAX_FL(1518) | ENET_RCR_MII_MODE_MASK | ENET_RCR_CRCFWD_MASK | ENET_RCR_RMII_MODE_MASK;
    ENET->TCR = ENET_TCR_FDEN_MASK;

    /* max receiced packet size */
    ENET->MRBR |= ENET_MRBR_R_BUF_SIZE_MASK;
    
	/* tell NENT the descriptors address */
	ENET->RDSR = (uint32_t) ENET_RxDescriptor;
	ENET->TDSR = (uint32_t) ENET_TxDescriptor;
    
	/* clear all IT pending bit */
	ENET->EIR = 0xFFFFFFFF;
	
    /* enable moudle */
    ENET->ECR = ENET_ECR_ETHEREN_MASK | ENET_ECR_DBSWP_MASK;
    
    /* enable revieved */
	ENET->RDAR = ENET_RDAR_RDAR_MASK;
}

/**
 * @brief  发送一帧以太帧数据
 * @param[in]   data    发送数据指针
 * @param[in]   len     数据长度 (< 1500字节)
 * @retval  None
 */
uint32_t ENET_SendData(uint8_t *data, uint16_t len)
{
    _REV32((uint32_t*)&ENET_TxDescriptor[0].length);
    _REV32((uint32_t*)&ENET_TxDescriptor[0].buffer);
    
    /* check if buffer is readly */
    if(ENET_TxDescriptor->control & ENET_BUFFDESCRIPTOR_TX_READY_MASK)
    {
        return CH_ERR;
    }
    /* set Tx Descriptor */
    ENET_TxDescriptor->buffer = data;		
    ENET_TxDescriptor->length = len;
	ENET_TxDescriptor->control = ( ENET_BUFFDESCRIPTOR_TX_READY_MASK | ENET_BUFFDESCRIPTOR_TX_LAST_MASK | ENET_BUFFDESCRIPTOR_TX_TRANMITCRC_MASK | ENET_BUFFDESCRIPTOR_TX_WRAP_MASK );
        
    _REV32((uint32_t*)&ENET_TxDescriptor[0].length);
    _REV32((uint32_t*)&ENET_TxDescriptor[0].buffer);
    
    /* enable transmit */
    ENET->EIR = ENET_EIR_TXF_MASK;
    ENET->TDAR = ENET_TDAR_TDAR_MASK;
    
    /* polling or interrupt mode */
    if((ENET->EIMR & ENET_EIMR_TXF_MASK) == 0)
    {
        while((ENET->EIR & ENET_EIR_TXF_MASK) == 0);
    }
    return CH_OK;
}

uint32_t ENET_GetRxDataSize(void)
{
    NBUF RxBuf;
    memcpy(&RxBuf, ENET_RxDescriptor, sizeof(NBUF));
    
    _REV32((uint32_t*)&RxBuf.length);
    _REV32((uint32_t*)&RxBuf.buffer);
    
    if((RxBuf.control & ENET_BUFFDESCRIPTOR_RX_EMPTY_MASK ) == 0)
    {
		return  (RxBuf.length);
    }  
    return 0;
}

/**
 * @brief  接收一帧以太帧数据
 * @note    用户调用函数
 * @param[in]   data    数据指针
 * @retval  接收到的数据长度
 */
uint32_t ENET_ReceiveData(uint8_t *data)
{
    uint16_t len = 0;
    
    NBUF RxBuf;
    memcpy(&RxBuf, ENET_RxDescriptor, sizeof(NBUF));
    
    _REV32((uint32_t*)&RxBuf.length);
    _REV32((uint32_t*)&RxBuf.buffer);
    
    /* if buffer is ready */
    if((RxBuf.control & ENET_BUFFDESCRIPTOR_RX_EMPTY_MASK ) == 0)
    {
        /* copy data to user bufer */
        len =  (RxBuf.length);
        memcpy(data, RxBuf.buffer, len);
        
		/* buffer is ready and data is readed */
        _REV32((uint32_t*)&ENET_RxDescriptor[0].length);
		ENET_RxDescriptor[0].control |= ENET_BUFFDESCRIPTOR_RX_EMPTY_MASK;
        _REV32((uint32_t*)&ENET_RxDescriptor[0].length);
		ENET->RDAR = ENET_RDAR_RDAR_MASK;
        return len;
    }
	  return 0;
}

/**
 * @brief  设置以太网中断
 * @param[in]   ENET_Int_t  以太网中断源选项
 * @param[in]   val         true 开启 false关闭
 * @retval  None
 */
void ENET_SetIntMode(ENET_Int_t mode, bool val)
{
    NVIC_EnableIRQ(ENET_Transmit_IRQn);
    NVIC_EnableIRQ(ENET_Receive_IRQn);
    
    switch(mode)
    {
        case kENET_IntTx:
            (val)?( ENET->EIMR |= ENET_EIMR_TXF_MASK):( ENET->EIMR &= ~ENET_EIMR_TXF_MASK);
            break;
        case kENET_IntRx:
            (val)?( ENET->EIMR |= ENET_EIMR_RXF_MASK):( ENET->EIMR &= ~ENET_EIMR_RXF_MASK);
            break;
        default:
            break;
    }
}

#endif

