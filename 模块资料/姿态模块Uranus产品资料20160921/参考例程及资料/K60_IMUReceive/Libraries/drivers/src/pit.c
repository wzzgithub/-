/**
  ******************************************************************************
  * @file    pit.c
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.05.31
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "pit.h"
#include "common.h"

#if defined(PIT)

static uint32_t fac_us;

#if defined(SIM_SCGC6_PIT_MASK)
static const Reg_t ClkGate[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_PIT_MASK, SIM_SCGC6_PIT_SHIFT},
};
#else
#error "No PIT clock gate defined!"
#endif /* SIM_SCGC6_PIT_MASK */

#if (__CORTEX_M == 0)
static const IRQn_Type PIT_IRQTbl[] = 
{
    PIT_IRQn,
    PIT_IRQn,
    PIT_IRQn,
    PIT_IRQn,
};
#else
static const IRQn_Type PIT_IRQTbl[] = 
{
    PIT0_IRQn,
    PIT1_IRQn,
    PIT2_IRQn,
    PIT3_IRQn,
};
#endif

/**
 * @brief  ��ʼ������PITģ��
 * @note   ����PITͨ�������ڶ�ʱ����
 * @param  chl:
 *         @arg HW_PIT_CH0 : PITģ���0ͨ��
 *         @arg HW_PIT_CH1 : PITģ���1ͨ��
 * @param  us : ����ʱ����
 * @retval None
 */
void PIT_Init(uint32_t chl, uint32_t us)
{
    REG_SET(ClkGate, 0);
    
    /* get clock */
    fac_us = US_TO_COUNT(1, GetClock(kBusClock));
    PIT->MCR &= ~PIT_MCR_MDIS_MASK;
    
    PIT_SetValue(chl, fac_us*us);
}

/**
 * @brief  ����PITģ��ָ��ͨ����ʱ��
 * @note   ����PITͨ��������ʱ����
 * @param  chl:
 *         @arg HW_PIT_CH0 : PITģ���0ͨ��
 *         @arg HW_PIT_CH1 : PITģ���1ͨ��
 * @param  us : ����ʱ����
 * @retval None
 */
void PIT_SetTime(uint32_t chl, uint32_t us)
{
    PIT->CHANNEL[chl].TCTRL &= (~PIT_TCTRL_TEN_MASK);
    PIT->CHANNEL[chl].LDVAL = fac_us * us;
    PIT->CHANNEL[chl].TCTRL |= (PIT_TCTRL_TEN_MASK);
}

/**
 * @brief  ���PITģ��ָ��ͨ���ļ�ʱʱ��
 * @note   None
 * @param  chl:
 *         @arg HW_PIT_CH0 : PITģ���0ͨ��
 *         @arg HW_PIT_CH1 : PITģ���1ͨ��
 * @retval ��ʱ�������е�ʱ�䵥λΪus
 */
uint32_t PIT_GetTime(uint32_t chl)
{
    return PIT->CHANNEL[chl].CVAL/fac_us;
}

/**
 * @brief  ���PITģ��ָ��ͨ������ֵ
 * @note   None
 * @param  chl:
 *         @arg HW_PIT_CH0 : PITģ���0ͨ��
 *         @arg HW_PIT_CH1 : PITģ���1ͨ��
 * @retval ��ʱ�������е���ֵ
 */
uint32_t PIT_GetValue(uint32_t chl)
{
    return PIT->CHANNEL[chl].CVAL;
}

/**
 * @brief  ����PITͨ����ʱ��ֵ
 * @note   None
 * @param  chl:
 *         @arg HW_PIT_CH0 : PITͨ��0
 *         @arg HW_PIT_CH1 : PITͨ��1
 * @param  val :���ü���������ֵ
 */
void PIT_SetValue(uint8_t chl, uint32_t val)
{
    PIT->CHANNEL[chl].TCTRL &= (~PIT_TCTRL_TEN_MASK);
    PIT->CHANNEL[chl].LDVAL = val;
    PIT->CHANNEL[chl].TCTRL |= (PIT_TCTRL_TEN_MASK);
}

/**
 * @brief  PIT�жϿ���
 * @note   None
 * @param  chl:
 *         @arg HW_PIT_CH0 : PITͨ��0
 *         @arg HW_PIT_CH1 : PITͨ��1
 * @param  val :
 *         @arg false : �ر��ж�
 *         @arg true : �����ж�
 * @retval CH_OK �ɹ� ���� �������
 */
uint32_t PIT_SetIntMode(uint32_t chl, bool val)
{
    REG_SET(ClkGate, 0);
    if(val)
    {
        PIT->CHANNEL[chl].TFLG |= PIT_TFLG_TIF_MASK;
        PIT->CHANNEL[chl].TCTRL |= PIT_TCTRL_TIE_MASK;
        NVIC_EnableIRQ(PIT_IRQTbl[chl]);
    }
    else
    {
        PIT->CHANNEL[chl].TCTRL &= ~PIT_TCTRL_TIE_MASK;
    }
    return CH_OK;
}

//void PIT_IRQHandler(void)
//{
//    if(PIT->CHANNEL[0].TFLG)
//    {
//        PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
//    }
//    if(PIT->CHANNEL[1].TFLG)
//    {
//        PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;
//    }
//}

#endif

