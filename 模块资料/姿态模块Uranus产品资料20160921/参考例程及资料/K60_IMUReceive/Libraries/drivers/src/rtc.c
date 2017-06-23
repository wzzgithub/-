/**
  ******************************************************************************
  * @file    rtc.c
  * @author  YANDLD
  * @version V3.0.0
  * @date    2016.06.07
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
 #include "rtc.h"
 #include "common.h"
 
#if defined(RTC)
 
#if defined(SIM_SCGC6_RTC_MASK)
static const Reg_t RTCClkGate[] =
{
    {(void*)&(SIM->SCGC6), SIM_SCGC6_RTC_MASK, SIM_SCGC6_RTC_SHIFT},

};
#elif defined(PCC0_PCC_RTC_CGC_MASK)
static const Reg_t RTCClkGate[] =
{
    {(void*)&(PCC0->PCC_RTC), PCC0_PCC_RTC_CGC_MASK, PCC0_PCC_RTC_CGC_SHIFT},
};
#else
#error "No RTC clock gate defined!"
#endif /* SIM_SCGC6_RTC_MASK */

 
#define SECONDS_IN_A_DAY     (86400U)
#define SECONDS_IN_A_HOUR    (3600U)
#define SECONDS_IN_A_MIN     (60U)
#define DAYS_IN_A_YEAR       (365U)
#define DAYS_IN_A_LEAP_YEAR  (366U)
// Table of month length (in days) for the Un-leap-year
static const uint8_t ULY[] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};
// Table of month length (in days) for the Leap-year
static const uint8_t  LY[] = {0U, 31U, 29U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};
// Number of days from begin of the non Leap-year
static const uint16_t MONTH_DAYS[] = {0U, 0U, 31U, 59U, 90U, 120U, 151U, 181U, 212U, 243U, 273U, 304U, 334U};



int RTC_GetWeek(int year, int month, int days)
{  
    static int mdays[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30 };  
    int i, y = year - 1;  
    for (i=0; i<month; ++i) days += mdays[i];  
    if (month > 2) 
    { 
         /* Increment date if this is a leap year after February */
        if (((year%400) == 0) || ((year&3) == 0 && (year%100))) ++days;  
    }  
    return (y+y/4-y/100+y/400+days)%7;  
}
 
/**
 * @brief  由日期计算出秒
 * @note   None
 * @param  datetime ：指向RTC时钟数据结构体的指针，详见rtc.h
 * @param  seconds ：存放秒数据的地址
 * @retval None
 */
static void RTC_DateTimeToSecond(const RTC_DateTime_t * datetime, uint32_t * seconds)
{
    /* Compute number of days from 1970 till given year*/
    *seconds = (datetime->year - 1970U) * DAYS_IN_A_YEAR;
    /* Add leap year days */
    *seconds += ((datetime->year / 4) - (1970U / 4));
    /* Add number of days till given month*/
    *seconds += MONTH_DAYS[datetime->month];
    /* Add days in given month*/
    *seconds += datetime->day;
    /* For leap year if month less than or equal to Febraury, decrement day counter*/
    if ((!(datetime->year & 3U)) && (datetime->month <= 2U))
    {
        (*seconds)--;
    }

    *seconds = ((*seconds) * SECONDS_IN_A_DAY) + (datetime->hour * SECONDS_IN_A_HOUR) + 
               (datetime->minute * SECONDS_IN_A_MIN) + datetime->second;
    (*seconds)++;
}

/**
 * @brief  由秒计算出日期
 * @note   None
 * @param  seconds ：输入的秒（变量存储地址）
 * @param  datetime ：计指向年月日等信息结构体指针
 * @retval None
 */
static void RTC_SecondToDateTime(const uint32_t * seconds, RTC_DateTime_t * datetime)
{
    uint32_t x;
    uint32_t Seconds, Days, Days_in_year;
    const uint8_t *Days_in_month;
    /* Start from 1970-01-01*/
    Seconds = *seconds;
    /* days*/
    Days = Seconds / SECONDS_IN_A_DAY;
    /* seconds left*/
    Seconds = Seconds % SECONDS_IN_A_DAY;
    /* hours*/
    datetime->hour = Seconds / SECONDS_IN_A_HOUR;
    /* seconds left*/
    Seconds = Seconds % SECONDS_IN_A_HOUR;
    /* minutes*/
    datetime->minute = Seconds / SECONDS_IN_A_MIN;
    /* seconds*/
    datetime->second = Seconds % SECONDS_IN_A_MIN;
    /* year*/
    datetime->year = 1970;
    Days_in_year = DAYS_IN_A_YEAR;

    while (Days > Days_in_year)
    {
        Days -= Days_in_year;
        datetime->year++;
        if  (datetime->year & 3U)
        {
            Days_in_year = DAYS_IN_A_YEAR;
        }
        else
        {
            Days_in_year = DAYS_IN_A_LEAP_YEAR;    
        }
    }

    if  (datetime->year & 3U)
    {
        Days_in_month = ULY;
    }
    else
    {
        Days_in_month = LY;    
    }

    for (x=1U; x <= 12U; x++)
    {
        if (Days <= (*(Days_in_month + x)))
        {
            datetime->month = x;
            break;
        }
        else
        {
            Days -= (*(Days_in_month + x));
        }
    }
    datetime->day = Days;
}

/**
 * @brief  获得RTC的时间
 * @code
 *      //获得RTC的时间
 *      RTC_DateTime_t ts;   //申请一个结构体
 *      RTC_GetTime(&ts);    //将日期存储到ts中
 * @endcode
 * @note   None
 * @param  datetime ：返回计算出来的年月日等信息结构体
 * @retval None
 */
void RTC_GetTime(RTC_DateTime_t * datetime)
{
    if(!datetime)
    {
        return;
    }
    uint32_t i = RTC->TSR;
    RTC_SecondToDateTime(&i, datetime);
}

/**
 * @brief  判断当前RTC时钟模块时间是否有效
 * \code
 *      当时间无效（从来未执行过RTC时，初始化RTC的时间） 
 *    if(RTC_IsTimeValid())
 *    {
 *       printf("time invalid, reset time!\r\n");
 *       RTC_SetTime(&td);
 *    }
 * \endcode
 * @retval 0 ：有效 ；其它：无效
 */
bool RTC_IsTimeValid(void)
{
    if(RTC->TSR)
    {
        return true;
    }
    return false;
}

/**
 * @brief  设置闹钟时间
 * @note   None
 * @param  datetime ：时间戳结构体，详见rtc.h文件
 * @retval None
 */
void RTC_SetAlarm(RTC_DateTime_t * datetime)
{
    uint32_t seconds;
    RTC_DateTimeToSecond(datetime, &seconds);
    RTC->TAR = seconds - 1;
}

/**
 * @brief  获得闹钟时间
 * @note   None
 * @param  datetime ：时间戳结构体，详见rtc.h文件
 * @retval None
 */
void RTC_GetAlarm(RTC_DateTime_t * datetime)
{
    uint32_t seconds;
    seconds = RTC->TAR;
    RTC_SecondToDateTime(&seconds, datetime);
}

void RTC_SetCompensation(uint32_t compensationInterval, uint32_t timeCompensation)
{
    RTC->TCR &= ~RTC_TCR_CIR_MASK;
    RTC->TCR &= ~RTC_TCR_TCR_MASK;
    RTC->TCR |= RTC_TCR_CIR(compensationInterval);
    RTC->TCR |= RTC_TCR_TCR(timeCompensation);
}

/**
 * @brief  初始化RTC模块
 * @retval None
 */
void RTC_Init(void)
{
    volatile uint32_t i;
    REG_SET(RTCClkGate, 0);
    
    // RTC->CR = 0;
    
    /* dislabe TSR */
    RTC->SR &= ~RTC_SR_TCE_MASK;
    
    /* disable osc */
    RTC->CR &= ~RTC_CR_OSCE_MASK;
    
    /* 2PF load */
    RTC->CR |= RTC_CR_SC2P_MASK;
    
    /* enable OSC and wait startup */
    RTC->CR |= RTC_CR_OSCE_MASK;
	for(i=0;i<0x6000;i++) {};
        
    /* enable RTC */
    RTC->SR |= RTC_SR_TCE_MASK;
}

uint32_t RTC_GetCounter(void)
{
    return RTC->TSR;
}

uint32_t RTC_GetTAR(void)
{
    return RTC->TAR;
}

/**
 * @brief  设置RTC的时间
 * @note   None
 * @param  datetime ：时间戳结构体，详见rtc.h文件
 * @code
 *   RTC_Init();
 *   RTC_DateTime_t td = {0};
 *   td.day = 20;
 *   td.hour = 23;
 *   td.minute = 59;
 *   td.second = 50;
 *   td.year = 2014;
 *   td.month = 11;
 *   RTC_SetTime(&td);
 * @endcode
 * @retval None
 */
void RTC_SetTime(RTC_DateTime_t * datetime)
{
    uint32_t i;
    if(!datetime)
    {
        return;
    }
    RTC_DateTimeToSecond(datetime, &i);
    LIB_TRACE("Reconfig:%d  %s\r\n", i, __func__);
    RTC->SR &= ~RTC_SR_TCE_MASK;
    RTC->TSR = RTC_TSR_TSR(i);
    RTC->SR |= RTC_SR_TCE_MASK;
}

void RTC_SetTSR(uint32_t val)
{
    RTC->SR &= ~RTC_SR_TCE_MASK;
    RTC->TSR = RTC_TSR_TSR(val);
    RTC->SR |= RTC_SR_TCE_MASK;  
}

uint32_t RTC_GetTSR(void)
{
    return RTC->TSR;
}

/**
 * @brief  设置RTC中断功能
 * @code
 *      //设置RTC开启闹钟中断
 *      RTC_SetIntMode(kRTC_IntSecond, true); 
 * @endcode
 * @param  config ：配置中断类型
 *         @arg kRTC_IntSecond ：秒中断
 *         @arg kRTC_IntAlarm ：闹钟中断
 * \param  status 
 *         @arg false ：关闭中断
 *         @arg true ：打开中断   
 * @retval None
 */
void RTC_SetIntMode(RTC_Int_t mode, bool val)
{
    
    #if !defined(MKL28Z7)
    NVIC_EnableIRQ(RTC_IRQn);
    #else
    NVIC_EnableIRQ(RTC_Seconds_IRQn);
    NVIC_EnableIRQ(RTC_Alarm_IRQn);
    #endif
    
    switch(mode)
    {
        case kRTC_IntAlarm:
            
            (val)?(RTC->IER |= RTC_IER_TAIE_MASK):(RTC->IER &= ~RTC_IER_TAIE_MASK);
            break;
        case kRTC_IntSecond:
            NVIC_EnableIRQ(RTC_Seconds_IRQn);
            (val)?(RTC->IER |= RTC_IER_TSIE_MASK):(RTC->IER &= ~RTC_IER_TSIE_MASK);
            break;
        default:
            break;
    }
}

void RTCx_IRQHandler(void)
{
    if(RTC->SR & RTC_SR_TAF_MASK)
    {
        RTC->TAR = RTC->TAR;
        /* alarm interrupt */
    }
}

#endif /* RTC */

