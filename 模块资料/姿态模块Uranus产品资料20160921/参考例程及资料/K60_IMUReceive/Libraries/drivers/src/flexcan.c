/**
  ******************************************************************************
  * @file    flexcan.c
  * @author  YANDLD
  * @version V2.5
  * @date    2014.4.10
  * @date    2016.6.6 ������ can ģ������ע��
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
  
#include "flexcan.h"
#include "gpio.h"

#if defined(CAN0)

#define CAN_RXFIFO_ENTRY            (0)     /* MB0 is RxFIFO entry */
#define CAN_RXFIFO_AVAILABLE        (5)     /* MB5 flag is RxFIFO available */

#if (!defined(CAN_BASES))
#if defined(CAN1)
#define CAN_BASES   {CAN0, CAN1}
#else
#define CAN_BASES   {CAN0}
#endif
#endif

/* global vars */
CAN_Type * const FLEXCANBases[] = CAN_BASES;

static const Reg_t ClkTbl[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_FLEXCAN0_MASK},
#ifdef CAN1
    #if defined(SIM_SCGC3_FLEXCAN1_MASK)
    {(void*)&(SIM->SCGC3), SIM_SCGC3_FLEXCAN1_MASK},
    #elif defined(SIM_SCGC6_FLEXCAN1_MASK)
    {(void*)&(SIM->SCGC6), SIM_SCGC6_FLEXCAN1_MASK},
    #endif
#endif
};

static const IRQn_Type FLEXCAN_IRQTbl[] = 
{
    CAN0_ORed_Message_buffer_IRQn,
#ifdef CAN1
    CAN1_ORed_Message_buffer_IRQn,
#endif
};

typedef enum
{
    kFlexCanTX_Inactive  = 0x08, 	/* MB is not active.*/
    kFlexCanTX_Abort     = 0x09, 	/* MB is aborted.*/
    kFlexCanTX_Data      = 0x0C, 	/* MB is a TX Data Frame(MB RTR must be 0).*/
    kFlexCanTX_Remote    = 0x1C, 	/* MB is a TX Remote Request Frame (MB RTR must be 1).*/
    kFlexCanTX_Tanswer   = 0x0E, 	/* MB is a TX Response Request Frame from.*/
																	/*  an incoming Remote Request Frame.*/
    kFlexCanTX_NotUsed   = 0xF,  	/* Not used*/
    kFlexCanRX_Inactive  = 0x0, 	/* MB is not active.*/
    kFlexCanRX_Full      = 0x2, 	/* MB is full.*/
    kFlexCanRX_Empty     = 0x4, 	/* MB is active and empty.*/
    kFlexCanRX_Overrun   = 0x6, 	/* MB is overwritten into a full buffer.*/
    kFlexCanRX_Busy      = 0x8,     /* FlexCAN is updating the contents of the MB.*/
																	/*  The CPU must not access the MB.*/
    kFlexCanRX_Ranswer   = 0xA, 	/* A frame was configured to recognize a Remote Request Frame*/
																	/*  and transmit a Response Frame in return.*/
    kFlexCanRX_NotUsed   = 0xF, 	/* Not used*/
}CAN_MBCode_Type;

#define CAN_GET_MB_CODE(cs)         (((cs) & CAN_CS_CODE_MASK)>> CAN_CS_CODE_SHIFT)


static void CAN_EnterFreezeMode(uint32_t instance)
{
    FLEXCANBases[instance]->MCR |= (CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while(!(CAN_MCR_FRZACK_MASK & (FLEXCANBases[instance]->MCR))) {}; 
}

static void CAN_ExitFreezeMode(uint32_t instance)
{
    FLEXCANBases[instance]->MCR &= ~(CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while((CAN_MCR_FRZACK_MASK & (FLEXCANBases[instance]->MCR)));
}

uint32_t CAN_SetBaudrate(uint32_t instance, uint32_t baudrate)
{
    uint32_t ps ,p1, p2, pd;
    uint32_t sclock = GetClock(kBusClock)/baudrate;
    uint32_t time_seg1, time_seg2, total_tq;
    CAN_Type *CANx = FLEXCANBases[instance];

    for(pd=0xFF; pd>0; pd--)
    {
        for(ps=1; ps<8; ps++)
        {
            for(p1=1; p1<8; p1++)
            {
                for(p2=1; p2<8; p2++)
                {
                    time_seg1 = ps+p1+2;
                    time_seg2 = p2+1;
                    total_tq = time_seg1+time_seg2+1;

                    if(ABS((int32_t)(total_tq*(pd+1) - sclock)) < 2)
                    {
                        if((time_seg1 < (time_seg2+8)) && ((time_seg2+2) > time_seg1))
                        {
                            CAN_EnterFreezeMode(instance);
                            
                            CANx->CTRL1 &= ~(CAN_CTRL1_PROPSEG_MASK | CAN_CTRL1_RJW_MASK | CAN_CTRL1_PSEG1_MASK | CAN_CTRL1_PSEG2_MASK | CAN_CTRL1_PRESDIV_MASK);
                            CANx->CTRL1 |=  CAN_CTRL1_PROPSEG(ps) 
                                            | CAN_CTRL1_RJW(2)
                                            | CAN_CTRL1_PSEG1(p1) 
                                            | CAN_CTRL1_PSEG2(p2)
                                            | CAN_CTRL1_PRESDIV(pd);
                            LIB_TRACE("Get baudrate param! pd %d ps %d p1 %d p2 %d\r\n", pd, ps,p1,p2);
                            
                            /* enable module */
                            CAN_ExitFreezeMode(instance);
                            
                            return CH_OK;
                        }
                    }
                }
            }
        }
    }
	return CH_ERR;
}


/**
 * @brief  ����CAN ��׼֡��������
 * @note   ��׼֡ �������� �� ��չ֡�������� ����ͬʱ����
 * @param  instance��CANͨ��ģ���
 *         @arg HW_CAN0��CAN0ģ��
 *         @arg HW_CAN1��CAN1ģ��
 * @param  mb �������� 0~15
 * @param  mask ��CANͨ�Ž��չ������� (��׼ID 11λ)
 * @retval None
 */
void CAN_SetRxStdFilterMask(uint32_t instance, uint32_t mb, uint32_t mask)
{
    CAN_EnterFreezeMode(instance);
        
    FLEXCANBases[instance]->RXIMR[mb] = CAN_ID_STD((mask & 0x7FF)); 
    LIB_TRACE("Standard filter mb%d,  0x%X\r\n", mb, FLEXCANBases[instance]->RXIMR[mb]);
    
    CAN_ExitFreezeMode(instance);
}

/**
 * @brief  ����CAN ��չ֡��������
 * @note   ��׼֡ �������� �� ��չ֡�������� ����ͬʱ����
 * @param  instance��CANͨ��ģ���
 *         @arg HW_CAN0��CAN0ģ��
 *         @arg HW_CAN1��CAN1ģ��
 * @param  mb �������� 0~15
 * @param  mask ��CANͨ�Ž��չ������� (��չ֡ID 29λ)
 * @retval None
 */
void CAN_SetRxExtFilterMask(uint32_t instance, uint32_t mb, uint32_t mask)
{
    CAN_EnterFreezeMode(instance);
         
    LIB_TRACE("Extend filter mb%d,  0x%X\r\n", mb, mask);
    FLEXCANBases[instance]->RXIMR[mb] = CAN_ID_EXT(mask & 0x1FFFFFFF); 
    
    CAN_ExitFreezeMode(instance);
}


/**
 * @brief  ����CANͨѶ��������
 * @note   None
 * @param  instance��CANͨ��ģ���
 *         @arg HW_CAN0��CAN0ģ��
 *         @arg HW_CAN1��CAN1ģ��
 * @param  mb �������� 0~15
 * @param  id ��CANͨ�Ž���ID��11λ��׼��ַ����28λ��չ��ַ
 * @retval None
 */
void CAN_SetRxMB(uint32_t instance, uint32_t mb, uint32_t id)
{
    FLEXCANBases[instance]->MB[mb].WORD0 = 0;
    FLEXCANBases[instance]->MB[mb].WORD1 = 0;
    FLEXCANBases[instance]->MB[mb].CS = 0;
    if(id > 0x7FF)
    {
        FLEXCANBases[instance]->MB[mb].ID = id;  /* ID [28-0]*/
        FLEXCANBases[instance]->MB[mb].CS |= (CAN_CS_SRR_MASK | CAN_CS_IDE_MASK);  
    }
    else
    {
        FLEXCANBases[instance]->MB[mb].ID = CAN_ID_STD(id);  /* ID[28-18] */
        FLEXCANBases[instance]->MB[mb].CS &= ~(CAN_CS_IDE_MASK | CAN_CS_SRR_MASK); 
    }
    
	FLEXCANBases[instance]->MB[mb].CS |= CAN_CS_CODE(kFlexCanRX_Empty) | CAN_CS_RTR_MASK;
}

 /**
 * @brief  ����CANͨѶ��������
 * @note   None
 * @param  instance��CANͨ��ģ���
 *         @arg HW_CAN0��CAN0ģ��
 *         @arg HW_CAN1��CAN1ģ��
 * @param  mb �������� 0~15
 * @retval None
 */
void CAN_SetTxMB(uint32_t instance, uint32_t mb)
{
    FLEXCANBases[instance]->MB[mb].CS = CAN_CS_CODE(kFlexCanTX_Inactive);
    FLEXCANBases[instance]->MB[mb].ID = 0x0;
    FLEXCANBases[instance]->MB[mb].WORD0 = 0x0;
    FLEXCANBases[instance]->MB[mb].WORD1 = 0x0;
}
  
/**
 * @brief  CANͨѶ��ʼ�����ã���Ҫ���ʹ�ã�
 * @note   None
 * @param  MAP ��CANͨ��ģ���ʼ�����ýṹ��(ָ��)
 *               ���CAN_QuickInit() and can.h����ض���
 * @param  baudrate ��CANͨ���ٶ� 
 * @retval CH_OK ���ɹ����������룺����
 */
uint32_t CAN_Init(uint32_t MAP, uint32_t baudrate)
{
    uint32_t i;
    CAN_Type *CANx;
    map_t * pq = (map_t*)&(MAP); 

    REG_SET(ClkTbl, pq->ip);
    CANx = FLEXCANBases[pq->ip];
    
    /* set clock source is bus clock */
    CANx->CTRL1 |= CAN_CTRL1_CLKSRC_MASK;

    /* enable module */
    CANx->MCR &= ~CAN_MCR_MDIS_MASK;
    
    /* software reset */
	CANx->MCR |= CAN_MCR_SOFTRST_MASK;	
	while(CAN_MCR_SOFTRST_MASK & (CANx->MCR)) {}; 
        
    CAN_EnterFreezeMode(pq->ip);
        
    /* init all mb */
    for(i=0; i<ARRAY_SIZE(CAN0->MB); i++)
	{
		CANx->MB[i].CS = 0x00000000;
		CANx->MB[i].ID = 0x00000000;
		CANx->MB[i].WORD0 = 0x00000000;
		CANx->MB[i].WORD1 = 0x00000000;
        CANx->RXIMR[i] = 0x00000000; /* received all frame */
        CANx->IMASK1 = 0x00000000;
        CANx->IFLAG1 = 0xFFFFFFFF;
	}
    
    CANx->CTRL2 = CAN_CTRL2_TASD(0x16) | CAN_CTRL2_RRS_MASK | CAN_CTRL2_EACEN_MASK;
    
	/* set all masks */
	//CANx->RXMGMASK = CAN_ID_EXT(CAN_RXMGMASK_MG_MASK); 
   // CANx->RX14MASK = CAN_ID_EXT(CAN_RX14MASK_RX14M_MASK); 
   // CANx->RX15MASK = CAN_ID_EXT(CAN_RX15MASK_RX15M_MASK);
    /* use indviual mask, do not use RXMGMASK, RX14MASK and RX15MASK */
    CANx->MCR |= CAN_MCR_IRMQ_MASK;
    CANx->MCR &= ~CAN_MCR_IDAM_MASK;
    
    /* bypass the frame sended by itself */
    CANx->MCR |= CAN_MCR_SRXDIS_MASK; 
    
    /* setting baudrate */
	CAN_SetBaudrate(pq->ip, baudrate);
    
    /* enable module */
    CAN_ExitFreezeMode(pq->ip);
	while((CANx->MCR)&CAN_MCR_NOTRDY_MASK);
    
    PIN_SET_MUX;
    return CH_OK;
}

 /**
 * @brief  CAN��������֡
 * @note   None
 * @param  instance ��CANͨ��ģ���
 *         @arg HW_CAN0 ��0��CANͨ��ģ��
 *         @arg HW_CAN1 ��1��CANͨ��ģ��
 * @param  mb �������� 0~15
 * @param  id �����շ�ID��ַ(ʹ���˲�ģʽ)
 * @param  buf ���������ݵ��׵�ַָ��
 * @param  len �����ݳ���
 * @retval CH_OK ���ɹ����������룺����
 */
uint32_t CAN_SendDataFrame(uint32_t instance, uint32_t mb, uint32_t id, uint8_t* buf, uint8_t len)
{
    uint32_t i, cs_temp, timeout, word[2];

    word[0] = 0;
    word[1] = 0;
    cs_temp = 0;
    timeout = 0;
    
    while((FLEXCANBases[instance]->MB[mb].CS & CAN_CS_CODE_MASK) != CAN_CS_CODE(kFlexCanTX_Inactive) && timeout < 100000)
    {
        timeout++;
    }
    if(timeout == 100000)
    {
        LIB_TRACE("CAN_SendDataFrame timeout!\r\n");
        return CH_TIMEOUT;
    }

    /* setting data */
	for(i=0; i<len; i++)
	{
        (i<4)?( word[0] |= (*(buf+i)<<((3-i)*8))):(word[1] |= (*(buf+i)<<((7-i)*8)));
	}
    
    FLEXCANBases[instance]->MB[mb].WORD0 = word[0];
    FLEXCANBases[instance]->MB[mb].WORD1 = word[1];
    
    /* len field */
    cs_temp |= CAN_CS_DLC(len);
    
    /* ID and IDE */
    if(id > 0x7FF)
    {
        FLEXCANBases[instance]->MB[mb].ID = id;  /* ID [28-0]*/
        cs_temp |= (CAN_CS_SRR_MASK | CAN_CS_IDE_MASK);  
    }
    else
    {
        FLEXCANBases[instance]->MB[mb].ID = CAN_ID_STD(id);  /* ID[28-18] */
    }
    
    cs_temp |= CAN_CS_CODE(kFlexCanTX_Data);
    
    FLEXCANBases[instance]->MB[mb].CS = cs_temp;
    return CH_OK;
}

 /**
 * @brief  CAN����Զ��֡
 * @note   Զ��֡û�����ݲ��ֵ����ֽڳ��Ȳ���
 *         Զ��֡���ͺ�������Զ���ɽ�������
 * @param  instance ��CANͨ��ģ���
 *         @arg HW_CAN0 ��0��CANͨ��ģ��
 *         @arg HW_CAN1 ��1��CANͨ��ģ��
 * @param  mb �������� 0~15
 * @param  id  �����շ�ID��ַ(ʹ���˲�ģʽ)
 * @param  buf ���������ݵ��׵�ַָ��
 * @param  len �����ݳ���
 * @retval CH_OK ���ɹ����������룺����
 */
uint32_t CAN_SendRemoteFrame(uint32_t instance, uint32_t mb, uint32_t id, uint8_t req_len)
{
    uint32_t cs_temp, timeout;

    cs_temp = 0;
    timeout = 0;
    
    while((FLEXCANBases[instance]->MB[mb].CS & CAN_CS_CODE_MASK) != CAN_CS_CODE(kFlexCanTX_Inactive) && timeout < 100000)
    {
        timeout++;
    }
    
    if(timeout == 100000)
    {
        LIB_TRACE("CAN_SendRemoteFrame timeout!\r\n");
        return CH_TIMEOUT;
    }
    
    /* DLC field, remote frame still has DLC filed, it's request len */
    cs_temp |= CAN_CS_DLC(req_len) | CAN_CS_RTR_MASK;
    
    /* ID and IDE */
    if(id > 0x7FF)
    {
        FLEXCANBases[instance]->MB[mb].ID = id;  /* ID [28-0]*/
        cs_temp |= (CAN_CS_SRR_MASK | CAN_CS_IDE_MASK);  
    }
    else
    {
        FLEXCANBases[instance]->MB[mb].ID = CAN_ID_STD(id);  /* ID[28-18] */
    }
    
    cs_temp |= CAN_CS_CODE(kFlexCanTX_Remote);
    FLEXCANBases[instance]->MB[mb].CS = cs_temp;
    
    return CH_OK;
}

/**
 * @brief  ����CANģ����ж�����
 * @note   None
 * @param  instance ��CANͨ��ģ���
 *         @arg HW_CAN0 ��0��CANͨ��ģ��
 *         @arg HW_CAN1 ��1��CANͨ��ģ��
 * @param  mb �������� 0~15
 * @param  config ������ģʽ
 *         @arg kCAN_IntTx �������ж�
 *         @arg kCAN_IntRx �������ж�
 * @param  val :
 *         @arg true �������ж�
 *         @arg false ���ر��ж�
 * @retval None
 */
void CAN_SetIntMode(uint32_t instance, uint32_t mb, CAN_Int_t mode, bool val)
{
    NVIC_EnableIRQ(FLEXCAN_IRQTbl[instance]);
    switch(mode)
    {
        case kCAN_IntMB:
            (val)?(FLEXCANBases[instance]->IMASK1 |= (1 << mb)):(FLEXCANBases[instance]->IMASK1 &= ~(1 << mb));
            break;
        default:
            break;
    }
}

/**
 * @brief  ��ȡCAN������յ�������
 * @note   None
 * @param  instance ��CANͨ��ģ���
 *         @arg HW_CAN0 ��0��CANͨ��ģ��
 *         @arg HW_CAN1 ��1��CANͨ��ģ��
 * @param  mb �������� 0~15
 * @param  id ��CANͨ�Ž���IDָ���ַ
 * @param  buf ��CANͨ�Ž�������ָ���ַ
 * @param  len ��CANͨ�Ž������ݳ���ָ���ַ
 * @retval CH_OK ������������������
 */
uint32_t CAN_ReceiveFrame(uint32_t instance, uint32_t mb, uint32_t *id, uint8_t *buf, uint8_t *len, bool *isRemote)
{
	uint32_t code, i;
	uint32_t word[2] = {0};
    uint32_t cs_temp;
    cs_temp = FLEXCANBases[instance]->MB[mb].CS;
    code = CAN_GET_MB_CODE(cs_temp);
    
    if(code & 0x01)
    {
        return CH_IO_ERR; /* MB is busy and controlled by hardware */
    }
    
    if(FLEXCANBases[instance]->IFLAG1 & (1<<mb))
    {
        /* clear IT pending bit */
        FLEXCANBases[instance]->IFLAG1 = (1 << mb);
        
        /* read content */
        *len = (cs_temp & CAN_CS_DLC_MASK) >> CAN_CS_DLC_SHIFT;
        word[0] = FLEXCANBases[instance]->MB[mb].WORD0;
        word[1] = FLEXCANBases[instance]->MB[mb].WORD1;
        
        (cs_temp & CAN_CS_RTR_MASK)?(*isRemote = true):(*isRemote = false);

        for(i = 0; i < *len; i++)
        {  
            (i < 4)?(*(buf + i))=(word[0]>>((3-i)*8)):((*(buf + i))=(word[1]>>((7-i)*8)));
        }
        
        *id = (FLEXCANBases[instance]->MB[mb].ID & (CAN_ID_EXT_MASK | CAN_ID_STD_MASK));
        
        if(!(cs_temp & CAN_CS_IDE_MASK))
        {
            *id >>= CAN_ID_STD_SHIFT;
        }
        
        i = FLEXCANBases[instance]->TIMER; /* unlock MB */
        return CH_OK;
    }
    i = FLEXCANBases[instance]->TIMER; /* unlock MB */
    return CH_IO_ERR;
}

/**
 * @brief  ����RXFIFO�ж�
 * @param  instance CANģ���
 *         			@arg HW_CAN0 оƬ��CANģ��0��
 *         			@arg HW_CAN1 оƬ��CANģ��1��
 * @param  val :
 *         @arg true : �����ж�
 *         @arg false : �ر��ж�
 * @retval None
 */
void CAN_SetRxFIFOIntMode(uint32_t instance, bool val)
{
    NVIC_EnableIRQ(FLEXCAN_IRQTbl[instance]);
    (val)?(FLEXCANBases[instance]->IMASK1 |= (1 << 5)):(FLEXCANBases[instance]->IMASK1 &= ~(1 << 5));
}

/**
 * @brief  ���ؽ���FIFOģʽ
 * @param  instance CANģ���
 *         			@arg HW_CAN0 оƬ��CANģ��0��
 *         			@arg HW_CAN1 оƬ��CANģ��1��
 * @param  val :
 *         @arg true : �����ж�
 *         @arg false : �ر��ж�
 * @retval None
 */
uint32_t CAN_SetRxFIFO(uint32_t instance, uint32_t *filter_table, uint8_t filter_len, bool val)
{
    int i;
    volatile uint32_t *idFilterRegion = (volatile uint32_t *)(&FLEXCANBases[instance]->MB[6].CS);
    LIB_TRACE("CAN RxFIFO IFR:0x%X fitler_len:%d\r\n", (uint32_t)idFilterRegion, filter_len);
    
    CAN_EnterFreezeMode(instance);
    
    if(filter_len > 8) filter_len = 8;
    
    for (i=0; i<8; i++)
    {
        idFilterRegion[i] = 0x0;
    }
    
    if(val)
    {
        /* Disable unused Rx FIFO Filter. */
        for (i=filter_len; i<8; i++)
        {
            idFilterRegion[i] = 0xFFFFFFFFU;
        }

        /* Copy ID filter table to Message Buffer Region. */
        for (i=0; i<filter_len; i++)
        {
            LIB_TRACE("RxFIFO:ID Filter Table: 0x%X\r\n", filter_table[i]);
            idFilterRegion[i] = filter_table[i];
        }
        
        FLEXCANBases[instance]->MCR = (FLEXCANBases[instance]->MCR & ~CAN_MCR_IDAM_MASK) | CAN_MCR_IDAM(0x0);
        FLEXCANBases[instance]->MCR |= CAN_MCR_RFEN_MASK;
    }
    else
    {
        /* Disable Rx Message FIFO. */
        FLEXCANBases[instance]->MCR &= ~CAN_MCR_RFEN_MASK;

        /* Clean MB0 ~ MB5. */
        CAN_SetRxMB(instance, 0, 0);
        CAN_SetRxMB(instance, 1, 0);
        CAN_SetRxMB(instance, 2, 0);
        CAN_SetRxMB(instance, 3, 0);
        CAN_SetRxMB(instance, 4, 0);
        CAN_SetRxMB(instance, 5, 0);
    }
    
    CAN_ExitFreezeMode(instance);
    return CH_OK;
}


uint32_t CAN_ReadFIFO(uint32_t instance, uint32_t *id, uint8_t *buf, uint8_t *len, bool *isRemote)
{
    if(FLEXCANBases[instance]->MCR & CAN_MCR_RFEN_MASK)
    {
        if(FLEXCANBases[instance]->IFLAG1 & (1<<CAN_RXFIFO_AVAILABLE))
        {
            
            uint32_t i;
            uint32_t word[2] = {0};
            uint32_t cs_temp;
            cs_temp = FLEXCANBases[instance]->MB[CAN_RXFIFO_ENTRY].CS;
            //code = CAN_GET_MB_CODE(cs_temp);
            
            
            /* read content */
            *len = (cs_temp & CAN_CS_DLC_MASK) >> CAN_CS_DLC_SHIFT;
            word[0] = FLEXCANBases[instance]->MB[CAN_RXFIFO_ENTRY].WORD0;
            word[1] = FLEXCANBases[instance]->MB[CAN_RXFIFO_ENTRY].WORD1;
            
            (cs_temp & CAN_CS_RTR_MASK)?(*isRemote = true):(*isRemote = false);

            for(i = 0; i < *len; i++)
            {  
                (i < 4)?(*(buf + i))=(word[0]>>((3-i)*8)):((*(buf + i))=(word[1]>>((7-i)*8)));
            }
            
            *id = (FLEXCANBases[instance]->MB[CAN_RXFIFO_ENTRY].ID & (CAN_ID_EXT_MASK | CAN_ID_STD_MASK));
            
            if(!(cs_temp & CAN_CS_IDE_MASK))
            {
                *id >>= CAN_ID_STD_SHIFT;
            }
            
            i = FLEXCANBases[instance]->TIMER; /* unlock MB */
            FLEXCANBases[instance]->IFLAG1 = (1 << CAN_RXFIFO_AVAILABLE);
            return CH_OK;
        }
    }

    return CH_ERR;
}


#endif
