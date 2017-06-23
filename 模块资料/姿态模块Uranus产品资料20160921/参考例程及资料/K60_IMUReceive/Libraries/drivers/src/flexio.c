/**
  ******************************************************************************
  * @file    flexio.c
  * @author  YANDLD
  * @version V3.0
  * @date    2016.6.13
  * @brief   www.beyondcore.net   http://upcmcu.taobao.com 
  ******************************************************************************
  */
#include "flexio.h"
#include "common.h"

#if (defined(FLEXIO0) || defined(FLEXIO))

#if !defined(FLEXIO_BASE_PTRS)
#define     FLEXIO_BASE_PTRS    FLEXIO_BASES
#endif

static FLEXIO_Type * const FLEXIOBases[] = FLEXIO_BASE_PTRS;

#define FX_UART_TX_TMR(x)       (x)         /* FLEXIO timer used for UART TX */
#define FX_UART_TX_SFT(x)       (x)         /* FLEXIO shifter used for UART TX */
#define FX_UART_RX_TMR(x)       (x+1)       /* FLEXIO timer used for UART RX */
#define FX_UART_RX_SFT(x)       (x+1)       /* FLEXIO shifter used for UART RX */


/*! @brief Define time of timer trigger polarity.*/
typedef enum _flexio_timer_trigger_polarity
{
    kFLEXIO_TimerTriggerPolarityActiveHigh = 0x0U, /*!< Active high. */
    kFLEXIO_TimerTriggerPolarityActiveLow = 0x1U,  /*!< Active low. */
} flexio_timer_trigger_polarity_t;

/*! @brief Define type of timer trigger source.*/
typedef enum _flexio_timer_trigger_source
{
    kFLEXIO_TimerTriggerSourceExternal = 0x0U, /*!< External trigger selected. */
    kFLEXIO_TimerTriggerSourceInternal = 0x1U, /*!< Internal trigger selected. */
} flexio_timer_trigger_source_t;

/*! @brief Define type of timer/shifter pin configuration.*/
typedef enum _flexio_pin_config
{
    kFLEXIO_PinConfigOutputDisabled = 0x0U,         /*!< Pin output disabled. */
    kFLEXIO_PinConfigOpenDrainOrBidirection = 0x1U, /*!< Pin open drain or bidirectional output enable. */
    kFLEXIO_PinConfigBidirectionOutputData = 0x2U,  /*!< Pin bidirectional output data. */
    kFLEXIO_PinConfigOutput = 0x3U,                 /*!< Pin output. */
} flexio_pin_config_t;

/*! @brief Definition of pin polarity.*/
typedef enum _flexio_pin_polarity
{
    kFLEXIO_PinActiveHigh = 0x0U, /*!< Active high. */
    kFLEXIO_PinActiveLow = 0x1U,  /*!< Active low. */
} flexio_pin_polarity_t;

/*! @brief Define type of timer work mode.*/
typedef enum _flexio_timer_mode
{
    kFLEXIO_TimerModeDisabled = 0x0U,        /*!< Timer Disabled. */
    kFLEXIO_TimerModeDual8BitBaudBit = 0x1U, /*!< Dual 8-bit counters baud/bit mode. */
    kFLEXIO_TimerModeDual8BitPWM = 0x2U,     /*!< Dual 8-bit counters PWM mode. */
    kFLEXIO_TimerModeSingle16Bit = 0x3U,     /*!< Single 16-bit counter mode. */
} flexio_timer_mode_t;

/*! @brief Define type of timer initial output or timer reset condition.*/
typedef enum _flexio_timer_output
{
    kFLEXIO_TimerOutputOneNotAffectedByReset = 0x0U,  /*!< Logic one when enabled and is not affected by timer
                                                       reset. */
    kFLEXIO_TimerOutputZeroNotAffectedByReset = 0x1U, /*!< Logic zero when enabled and is not affected by timer
                                                       reset. */
    kFLEXIO_TimerOutputOneAffectedByReset = 0x2U,     /*!< Logic one when enabled and on timer reset. */
    kFLEXIO_TimerOutputZeroAffectedByReset = 0x3U,    /*!< Logic zero when enabled and on timer reset. */
} flexio_timer_output_t;

/*! @brief Define type of timer decrement.*/
typedef enum _flexio_timer_decrement_source
{
    kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput = 0x0U,   /*!< Decrement counter on FlexIO clock, Shift clock
                                                                equals Timer output. */
    kFLEXIO_TimerDecSrcOnTriggerInputShiftTimerOutput = 0x1U,  /*!< Decrement counter on Trigger input (both edges),
                                                                Shift clock equals Timer output. */
    kFLEXIO_TimerDecSrcOnPinInputShiftPinInput = 0x2U,         /*!< Decrement counter on Pin input (both edges),
                                                                Shift clock equals Pin input. */
    kFLEXIO_TimerDecSrcOnTriggerInputShiftTriggerInput = 0x3U, /*!< Decrement counter on Trigger input (both edges),
                                                                Shift clock equals Trigger input. */
} flexio_timer_decrement_source_t;

/*! @brief Define type of timer reset condition.*/
typedef enum _flexio_timer_reset_condition
{
    kFLEXIO_TimerResetNever = 0x0U,                            /*!< Timer never reset. */
    kFLEXIO_TimerResetOnTimerPinEqualToTimerOutput = 0x2U,     /*!< Timer reset on Timer Pin equal to Timer Output. */
    kFLEXIO_TimerResetOnTimerTriggerEqualToTimerOutput = 0x3U, /*!< Timer reset on Timer Trigger equal to
                                                                Timer Output. */
    kFLEXIO_TimerResetOnTimerPinRisingEdge = 0x4U,             /*!< Timer reset on Timer Pin rising edge. */
    kFLEXIO_TimerResetOnTimerTriggerRisingEdge = 0x6U,         /*!< Timer reset on Trigger rising edge. */
    kFLEXIO_TimerResetOnTimerTriggerBothEdge = 0x7U,           /*!< Timer reset on Trigger rising or falling edge. */
} flexio_timer_reset_condition_t;

/*! @brief Define type of timer disable condition.*/
typedef enum _flexio_timer_disable_condition
{
    kFLEXIO_TimerDisableNever = 0x0U,                    /*!< Timer never disabled. */
    kFLEXIO_TimerDisableOnPreTimerDisable = 0x1U,        /*!< Timer disabled on Timer N-1 disable. */
    kFLEXIO_TimerDisableOnTimerCompare = 0x2U,           /*!< Timer disabled on Timer compare. */
    kFLEXIO_TimerDisableOnTimerCompareTriggerLow = 0x3U, /*!< Timer disabled on Timer compare and Trigger Low. */
    kFLEXIO_TimerDisableOnPinBothEdge = 0x4U,            /*!< Timer disabled on Pin rising or falling edge. */
    kFLEXIO_TimerDisableOnPinBothEdgeTriggerHigh = 0x5U, /*!< Timer disabled on Pin rising or falling edge provided
                                                          Trigger is high. */
    kFLEXIO_TimerDisableOnTriggerFallingEdge = 0x6U,     /*!< Timer disabled on Trigger falling edge. */
} flexio_timer_disable_condition_t;

/*! @brief Define type of timer enable condition.*/
typedef enum _flexio_timer_enable_condition
{
    kFLEXIO_TimerEnabledAlways = 0x0U,                    /*!< Timer always enabled. */
    kFLEXIO_TimerEnableOnPrevTimerEnable = 0x1U,          /*!< Timer enabled on Timer N-1 enable. */
    kFLEXIO_TimerEnableOnTriggerHigh = 0x2U,              /*!< Timer enabled on Trigger high. */
    kFLEXIO_TimerEnableOnTriggerHighPinHigh = 0x3U,       /*!< Timer enabled on Trigger high and Pin high. */
    kFLEXIO_TimerEnableOnPinRisingEdge = 0x4U,            /*!< Timer enabled on Pin rising edge. */
    kFLEXIO_TimerEnableOnPinRisingEdgeTriggerHigh = 0x5U, /*!< Timer enabled on Pin rising edge and Trigger high. */
    kFLEXIO_TimerEnableOnTriggerRisingEdge = 0x6U,        /*!< Timer enabled on Trigger rising edge. */
    kFLEXIO_TimerEnableOnTriggerBothEdge = 0x7U,          /*!< Timer enabled on Trigger rising or falling edge. */
} flexio_timer_enable_condition_t;

/*! @brief Define type of timer stop bit generate condition.*/
typedef enum _flexio_timer_stop_bit_condition
{
    kFLEXIO_TimerStopBitDisabled = 0x0U,                    /*!< Stop bit disabled. */
    kFLEXIO_TimerStopBitEnableOnTimerCompare = 0x1U,        /*!< Stop bit is enabled on timer compare. */
    kFLEXIO_TimerStopBitEnableOnTimerDisable = 0x2U,        /*!< Stop bit is enabled on timer disable. */
    kFLEXIO_TimerStopBitEnableOnTimerCompareDisable = 0x3U, /*!< Stop bit is enabled on timer compare and timer
                                                             disable. */
} flexio_timer_stop_bit_condition_t;

/*! @brief Define type of timer start bit generate condition.*/
typedef enum _flexio_timer_start_bit_condition
{
    kFLEXIO_TimerStartBitDisabled = 0x0U, /*!< Start bit disabled. */
    kFLEXIO_TimerStartBitEnabled = 0x1U,  /*!< Start bit enabled. */
} flexio_timer_start_bit_condition_t;

/*! @brief Define type of timer polarity for shifter control. */
typedef enum _flexio_shifter_timer_polarity
{
    kFLEXIO_ShifterTimerPolarityOnPositive = 0x0U, /* Shift on positive edge of shift clock. */
    kFLEXIO_ShifterTimerPolarityOnNegitive = 0x1U, /* Shift on negative edge of shift clock. */
} flexio_shifter_timer_polarity_t;

/*! @brief Define type of shifter working mode.*/
typedef enum _flexio_shifter_mode
{
    kFLEXIO_ShifterDisabled = 0x0U,            /*!< Shifter is disabled. */
    kFLEXIO_ShifterModeReceive = 0x1U,         /*!< Receive mode. */
    kFLEXIO_ShifterModeTransmit = 0x2U,        /*!< Transmit mode. */
    kFLEXIO_ShifterModeMatchStore = 0x4U,      /*!< Match store mode. */
    kFLEXIO_ShifterModeMatchContinuous = 0x5U, /*!< Match continuous mode. */
#if FSL_FEATURE_FLEXIO_HAS_STATE_MODE
    kFLEXIO_ShifterModeState = 0x6U, /*!< SHIFTBUF contents are used for storing
                                      programmable state attributes. */
#endif                               /* FSL_FEATURE_FLEXIO_HAS_STATE_MODE */
#if FSL_FEATURE_FLEXIO_HAS_LOGIC_MODE
    kFLEXIO_ShifterModeLogic = 0x7U, /*!< SHIFTBUF contents are used for implementing
                                     programmable logic look up table. */
#endif                               /* FSL_FEATURE_FLEXIO_HAS_LOGIC_MODE */
} flexio_shifter_mode_t;

/*! @brief Define type of shifter input source.*/
typedef enum _flexio_shifter_input_source
{
    kFLEXIO_ShifterInputFromPin = 0x0U,               /*!< Shifter input from pin. */
    kFLEXIO_ShifterInputFromNextShifterOutput = 0x1U, /*!< Shifter input from Shifter N+1. */
} flexio_shifter_input_source_t;

/*! @brief Define of STOP bit configuration.*/
typedef enum _flexio_shifter_stop_bit
{
    kFLEXIO_ShifterStopBitDisable = 0x0U, /*!< Disable shifter stop bit. */
    kFLEXIO_ShifterStopBitLow = 0x2U,     /*!< Set shifter stop bit to logic low level. */
    kFLEXIO_ShifterStopBitHigh = 0x3U,    /*!< Set shifter stop bit to logic high level. */
} flexio_shifter_stop_bit_t;

/*! @brief Define type of START bit configuration.*/
typedef enum _flexio_shifter_start_bit
{
    kFLEXIO_ShifterStartBitDisabledLoadDataOnEnable = 0x0U, /*!< Disable shifter start bit, transmitter loads
                                                             data on enable. */
    kFLEXIO_ShifterStartBitDisabledLoadDataOnShift = 0x1U,  /*!< Disable shifter start bit, transmitter loads
                                                             data on first shift. */
    kFLEXIO_ShifterStartBitLow = 0x2U,                      /*!< Set shifter start bit to logic low level. */
    kFLEXIO_ShifterStartBitHigh = 0x3U,                     /*!< Set shifter start bit to logic high level. */
} flexio_shifter_start_bit_t;

/*! @brief Define FlexIO shifter buffer type*/
typedef enum _flexio_shifter_buffer_type
{
    kFLEXIO_ShifterBuffer = 0x0U,               /*!< Shifter Buffer N Register. */
    kFLEXIO_ShifterBufferBitSwapped = 0x1U,     /*!< Shifter Buffer N Bit Byte Swapped Register. */
    kFLEXIO_ShifterBufferByteSwapped = 0x2U,    /*!< Shifter Buffer N Byte Swapped Register. */
    kFLEXIO_ShifterBufferBitByteSwapped = 0x3U, /*!< Shifter Buffer N Bit Swapped Register. */
#if defined(FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_NIBBLE_BYTE_SWAP) && FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_NIBBLE_BYTE_SWAP
    kFLEXIO_ShifterBufferNibbleByteSwapped = 0x4U, /*!< Shifter Buffer N Nibble Byte Swapped Register. */
#endif                                             /*FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_NIBBLE_BYTE_SWAP*/
#if defined(FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_HALF_WORD_SWAP) && FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_HALF_WORD_SWAP
    kFLEXIO_ShifterBufferHalfWordSwapped = 0x5U, /*!< Shifter Buffer N Half Word Swapped Register. */
#endif
#if defined(FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_NIBBLE_SWAP) && FSL_FEATURE_FLEXIO_HAS_SHFT_BUFFER_NIBBLE_SWAP
    kFLEXIO_ShifterBufferNibbleSwapped = 0x6U, /*!< Shifter Buffer N Nibble Swapped Register. */
#endif
} flexio_shifter_buffer_type_t;

/*! @brief Define FlexIO user configuration structure. */
typedef struct _flexio_config_
{
    bool enableFlexio;     /*!< Enable/disable FlexIO module */
    bool enableInDoze;     /*!< Enable/disable FlexIO operation in doze mode */
    bool enableInDebug;    /*!< Enable/disable FlexIO operation in debug mode */
    bool enableFastAccess; /*!< Enable/disable fast access to FlexIO registers, fast access requires
                           the FlexIO clock to be at least twice the frequency of the bus clock. */
} flexio_config_t;

/*! @brief Define FlexIO timer configuration structure. */
typedef struct _flexio_timer_config
{
    /* Trigger. */
    uint32_t triggerSelect;                          /*!< The internal trigger selection number using MACROs. */
    flexio_timer_trigger_polarity_t triggerPolarity; /*!< Trigger Polarity. */
    flexio_timer_trigger_source_t triggerSource;     /*!< Trigger Source, internal (see 'trgsel') or external. */
    /* Pin. */
    flexio_pin_config_t pinConfig;     /*!< Timer Pin Configuration. */
    uint32_t pinSelect;                /*!< Timer Pin number Select. */
    flexio_pin_polarity_t pinPolarity; /*!< Timer Pin Polarity. */
    /* Timer. */
    flexio_timer_mode_t timerMode;                  /*!< Timer work Mode. */
    flexio_timer_output_t timerOutput;              /*!< Configures the initial state of the Timer Output and
                                                    whether it is affected by the Timer reset. */
    flexio_timer_decrement_source_t timerDecrement; /*!< Configures the source of the Timer decrement and the
                                                    source of the Shift clock. */
    flexio_timer_reset_condition_t timerReset;      /*!< Configures the condition that causes the timer counter
                                                    (and optionally the timer output) to be reset. */
    flexio_timer_disable_condition_t timerDisable;  /*!< Configures the condition that causes the Timer to be
                                                    disabled and stop decrementing. */
    flexio_timer_enable_condition_t timerEnable;    /*!< Configures the condition that causes the Timer to be
                                                    enabled and start decrementing. */
    flexio_timer_stop_bit_condition_t timerStop;    /*!< Timer STOP Bit generation. */
    flexio_timer_start_bit_condition_t timerStart;  /*!< Timer STRAT Bit generation. */
    uint32_t timerCompare;                          /*!< Value for Timer Compare N Register. */
} flexio_timer_config_t;

/*! @brief Define FlexIO shifter configuration structure. */
typedef struct _flexio_shifter_config
{
    /* Timer. */
    uint32_t timerSelect;                          /*!< Selects which Timer is used for controlling the
                                                    logic/shift register and generating the Shift clock. */
    flexio_shifter_timer_polarity_t timerPolarity; /*!< Timer Polarity. */
    /* Pin. */
    flexio_pin_config_t pinConfig;     /*!< Shifter Pin Configuration. */
    uint32_t pinSelect;                /*!< Shifter Pin number Select. */
    flexio_pin_polarity_t pinPolarity; /*!< Shifter Pin Polarity. */
    /* Shifter. */
    flexio_shifter_mode_t shifterMode; /*!< Configures the mode of the Shifter. */
//#if FSL_FEATURE_FLEXIO_HAS_PARALLEL_WIDTH
    uint32_t parallelWidth;                    /*!< Configures the parallel width when using parallel mode.*/
//#endif                                         /* FSL_FEATURE_FLEXIO_HAS_PARALLEL_WIDTH */
    flexio_shifter_input_source_t inputSource; /*!< Selects the input source for the shifter. */
    flexio_shifter_stop_bit_t shifterStop;     /*!< Shifter STOP bit. */
    flexio_shifter_start_bit_t shifterStart;   /*!< Shifter START bit. */
} flexio_shifter_config_t;


/*! @brief Calculate FlexIO timer trigger.*/
#define FLEXIO_TIMER_TRIGGER_SEL_PININPUT(x) ((uint32_t)(x) << 1U)
#define FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(x) (((uint32_t)(x) << 2U) | 0x1U)
#define FLEXIO_TIMER_TRIGGER_SEL_TIMn(x) (((uint32_t)(x) << 2U) | 0x3U)

/**
 * @brief  设置移位
 * @note   None
 */
void FLEXIO_SetShifterConfig(FLEXIO_Type *base, uint8_t index, const flexio_shifter_config_t *shifterConfig)
{
    base->SHIFTCFG[index] = FLEXIO_SHIFTCFG_INSRC(shifterConfig->inputSource)
#if FLEXIO_SHIFTCFG_PWIDTH_MASK
                            | FLEXIO_SHIFTCFG_PWIDTH(shifterConfig->parallelWidth)
#endif /* FLEXIO_SHIFTCFG_PWIDTH_MASK */
                            | FLEXIO_SHIFTCFG_SSTOP(shifterConfig->shifterStop) |
                            FLEXIO_SHIFTCFG_SSTART(shifterConfig->shifterStart);

    base->SHIFTCTL[index] =
        FLEXIO_SHIFTCTL_TIMSEL(shifterConfig->timerSelect) | FLEXIO_SHIFTCTL_TIMPOL(shifterConfig->timerPolarity) |
        FLEXIO_SHIFTCTL_PINCFG(shifterConfig->pinConfig) | FLEXIO_SHIFTCTL_PINSEL(shifterConfig->pinSelect) |
        FLEXIO_SHIFTCTL_PINPOL(shifterConfig->pinPolarity) | FLEXIO_SHIFTCTL_SMOD(shifterConfig->shifterMode);
}

/**
 * @brief  设置定时器
 * @note   None
 */
void FLEXIO_SetTimerConfig(FLEXIO_Type *base, uint8_t index, const flexio_timer_config_t *timerConfig)
{
    base->TIMCFG[index] =
        FLEXIO_TIMCFG_TIMOUT(timerConfig->timerOutput) | FLEXIO_TIMCFG_TIMDEC(timerConfig->timerDecrement) |
        FLEXIO_TIMCFG_TIMRST(timerConfig->timerReset) | FLEXIO_TIMCFG_TIMDIS(timerConfig->timerDisable) |
        FLEXIO_TIMCFG_TIMENA(timerConfig->timerEnable) | FLEXIO_TIMCFG_TSTOP(timerConfig->timerStop) |
        FLEXIO_TIMCFG_TSTART(timerConfig->timerStart);

    base->TIMCMP[index] = FLEXIO_TIMCMP_CMP(timerConfig->timerCompare);

    base->TIMCTL[index] = FLEXIO_TIMCTL_TRGSEL(timerConfig->triggerSelect) |
                          FLEXIO_TIMCTL_TRGPOL(timerConfig->triggerPolarity) |
                          FLEXIO_TIMCTL_TRGSRC(timerConfig->triggerSource) |
                          FLEXIO_TIMCTL_PINCFG(timerConfig->pinConfig) | FLEXIO_TIMCTL_PINSEL(timerConfig->pinSelect) |
                          FLEXIO_TIMCTL_PINPOL(timerConfig->pinPolarity) | FLEXIO_TIMCTL_TIMOD(timerConfig->timerMode);
}

/**
 * @brief  初始化配置FlexIO模块
 * @note   None
 */
void FLEXIO_Init(void)
{
    #if defined(PCC1_PCC_FLEXIO0_CGC_MASK)
    PCC1->PCC_FLEXIO0 &= ~PCC1_PCC_FLEXIO0_CGC_MASK;
    PCC1->PCC_FLEXIO0 = PCC1_PCC_FLEXIO0_PCS(3);
    PCC1->PCC_FLEXIO0 |= PCC1_PCC_FLEXIO0_CGC_MASK;
    #endif
    
    #if defined(SIM_SCGC5_FLEXIO_MASK)
    SIM->SCGC5 |= SIM_SCGC5_FLEXIO_MASK;
    
    SIM->SOPT2 &= ~SIM_SOPT2_FLEXIOSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_FLEXIOSRC(0);  /*systemClock */
    SIM->MISCCTL &= ~SIM_MISCCTL_FlexIOS0_MASK;
    #endif
    
    FLEXIOBases[0]->CTRL |= FLEXIO_CTRL_SWRST_MASK;
    FLEXIOBases[0]->CTRL = FLEXIO_CTRL_DBGE_MASK | FLEXIO_CTRL_DOZEN_MASK | FLEXIO_CTRL_FLEXEN_MASK;

}

 /**
 * @brief  初始化配置FlexIO模块在PWM模式
 * @note   None
 * @param  chl ：pwm通道0或1
 * @param  pin ：引脚号
 * @param  freg：pwm波形频率
 * @retval None
 */
void FLEXIO_PWM_Init(uint32_t chl, uint32_t pin, uint32_t freq)
{
    uint32_t clk = GetClock(kCoreClock);
    clk = (clk/freq);
    
    flexio_timer_config_t tconfig;
    tconfig.pinConfig = kFLEXIO_PinConfigOutput;
    tconfig.pinPolarity = kFLEXIO_PinActiveLow;
    tconfig.pinSelect = pin;
    tconfig.timerCompare = ((clk/2)<<8) | (clk/2);
    tconfig.timerDecrement = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput;
    tconfig.timerDisable = kFLEXIO_TimerDisableNever;
    tconfig.timerEnable = kFLEXIO_TimerEnabledAlways;
    tconfig.timerMode = kFLEXIO_TimerModeDual8BitPWM;
    tconfig.timerOutput = kFLEXIO_TimerOutputOneNotAffectedByReset;
    tconfig.timerReset = kFLEXIO_TimerResetNever;
    tconfig.timerStart = kFLEXIO_TimerStartBitDisabled;
    tconfig.timerStop = kFLEXIO_TimerStopBitDisabled;
    FLEXIO_SetTimerConfig(FLEXIOBases[0], chl, &tconfig); 
}

 /**
 * @brief  开启PWM波形输出
 * @note   None
 * @param  chl ：pwm通道0或1
 * @param  pin ：引脚号
 * @retval None
 */
void FLEXIO_PWM_Start(uint32_t chl)
{
    FLEXIOBases[0]->TIMCTL[chl] &= ~FLEXIO_TIMCTL_PINCFG_MASK;
    FLEXIOBases[0]->TIMCTL[chl] |= FLEXIO_TIMCTL_PINCFG(kFLEXIO_PinConfigOutput); 
}

 /**
 * @brief  关闭PWM波形输出
 * @note   None
 * @param  chl ：pwm通道0或1
 * @param  pin ：引脚号
 * @retval None
 */
void FLEXIO_PWM_Stop(uint32_t chl, uint32_t pin)
{
    FLEXIOBases[0]->TIMCTL[chl] &= ~FLEXIO_TIMCTL_PINCFG_MASK;
    FLEXIOBases[0]->TIMCTL[chl] |= FLEXIO_TIMCTL_PINCFG(kFLEXIO_PinConfigOutputDisabled); 
}

/**
 * @brief  初始化配置FlexIO模块在串口模式
 * @note   None
 * @param  instance ：HW_FLEXIO_UART0或HW_FLEXIO_UART1
 * @param  baudrate ：通信波特率
 * @param  tx_pin : 发送引脚
 * @param  rx_pin : 接收引脚
 * @retval None
 */
void FLEXIO_UART_Init(uint32_t instance, uint32_t baudrate, uint32_t tx_pin, uint32_t rx_pin)
{
    flexio_shifter_config_t shifterConfig;
    flexio_timer_config_t timerConfig;
    uint16_t timerDiv = 0;
    uint16_t timerCmp = 0;
    
    /* Do hardware configuration. */
    /* 1. Configure the shifter 0 for tx. */
    shifterConfig.timerSelect = FX_UART_TX_TMR(instance);
    shifterConfig.timerPolarity = kFLEXIO_ShifterTimerPolarityOnPositive;
    shifterConfig.pinConfig = kFLEXIO_PinConfigOutput;
    shifterConfig.pinSelect = tx_pin;
    shifterConfig.pinPolarity = kFLEXIO_PinActiveHigh;
    shifterConfig.shifterMode = kFLEXIO_ShifterModeTransmit;
    shifterConfig.inputSource = kFLEXIO_ShifterInputFromPin;
    shifterConfig.shifterStop = kFLEXIO_ShifterStopBitHigh;
    shifterConfig.shifterStart = kFLEXIO_ShifterStartBitLow;
    shifterConfig.parallelWidth = 0;
    FLEXIO_SetShifterConfig(FLEXIOBases[0], FX_UART_TX_SFT(instance), &shifterConfig);

    /*2. Configure the timer 0 for tx. */
    timerConfig.triggerSelect = FLEXIO_TIMER_TRIGGER_SEL_SHIFTnSTAT(FX_UART_TX_SFT(instance));
    timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveLow;
    timerConfig.triggerSource = kFLEXIO_TimerTriggerSourceInternal;
    timerConfig.pinConfig = kFLEXIO_PinConfigOutputDisabled;
    timerConfig.pinSelect = tx_pin;
    timerConfig.pinPolarity = kFLEXIO_PinActiveHigh;
    timerConfig.timerMode = kFLEXIO_TimerModeDual8BitBaudBit;
    timerConfig.timerOutput = kFLEXIO_TimerOutputOneNotAffectedByReset;
    timerConfig.timerDecrement = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput;
    timerConfig.timerReset = kFLEXIO_TimerResetNever;
    timerConfig.timerDisable = kFLEXIO_TimerDisableOnTimerCompare;
    timerConfig.timerEnable = kFLEXIO_TimerEnableOnTriggerHigh;
    timerConfig.timerStop = kFLEXIO_TimerStopBitEnableOnTimerDisable;
    timerConfig.timerStart = kFLEXIO_TimerStartBitEnabled;

    timerDiv = (GetClock(kCoreClock) / baudrate);
    if((timerDiv / 2) > 0xFF)
    {
        LIB_TRACE("FlexIO ERR: over divider, cannot get correct baud!\r\n");
    }
    timerDiv = (timerDiv / 2) - 1;
    

    timerCmp = ((uint32_t)((8 * 2) - 1)) << 8U;
    timerCmp |= timerDiv;

    timerConfig.timerCompare = timerCmp;

    FLEXIO_SetTimerConfig(FLEXIOBases[0], FX_UART_TX_TMR(instance), &timerConfig);

    /* 3. Configure the shifter 1 for rx. */
    shifterConfig.timerSelect = FX_UART_RX_TMR(instance);
    shifterConfig.timerPolarity = kFLEXIO_ShifterTimerPolarityOnNegitive;
    shifterConfig.pinConfig = kFLEXIO_PinConfigOutputDisabled;
    shifterConfig.pinSelect = rx_pin;
    shifterConfig.pinPolarity = kFLEXIO_PinActiveHigh;
    shifterConfig.shifterMode = kFLEXIO_ShifterModeReceive;
    shifterConfig.inputSource = kFLEXIO_ShifterInputFromPin;
    shifterConfig.shifterStop = kFLEXIO_ShifterStopBitHigh;
    shifterConfig.shifterStart = kFLEXIO_ShifterStartBitLow;

    FLEXIO_SetShifterConfig(FLEXIOBases[0], FX_UART_RX_SFT(instance), &shifterConfig);

    /* 4. Configure the timer 1 for rx. */
    timerConfig.triggerSelect = FLEXIO_TIMER_TRIGGER_SEL_PININPUT(rx_pin);
    timerConfig.triggerPolarity = kFLEXIO_TimerTriggerPolarityActiveHigh;
    timerConfig.triggerSource = kFLEXIO_TimerTriggerSourceExternal;
    timerConfig.pinConfig = kFLEXIO_PinConfigOutputDisabled;
    timerConfig.pinSelect = rx_pin;
    timerConfig.pinPolarity = kFLEXIO_PinActiveLow;
    timerConfig.timerMode = kFLEXIO_TimerModeDual8BitBaudBit;
    timerConfig.timerOutput = kFLEXIO_TimerOutputOneAffectedByReset;
    timerConfig.timerDecrement = kFLEXIO_TimerDecSrcOnFlexIOClockShiftTimerOutput;
    timerConfig.timerReset = kFLEXIO_TimerResetOnTimerPinRisingEdge;
    timerConfig.timerDisable = kFLEXIO_TimerDisableOnTimerCompare;
    timerConfig.timerEnable = kFLEXIO_TimerEnableOnPinRisingEdge;
    timerConfig.timerStop = kFLEXIO_TimerStopBitEnableOnTimerDisable;
    timerConfig.timerStart = kFLEXIO_TimerStartBitEnabled;

    timerConfig.timerCompare = timerCmp;

    FLEXIO_SetTimerConfig(FLEXIOBases[0], FX_UART_RX_TMR(instance), &timerConfig);
}

static inline uint32_t FLEXIO_GetShifterStatusFlags(FLEXIO_Type *base)
{
    return ((base->SHIFTSTAT) & FLEXIO_SHIFTSTAT_SSF_MASK);
}

/**
 * @brief  串口发送一个字符
 * @note   None
 * @param  instance ：HW_FLEXIO_UART0或HW_FLEXIO_UART1
 * @param  ch: 需要发送的字符
 * @retval None
 */
void FLEXIO_UART_PutChar(uint32_t instance, uint8_t ch)
{
    /* Wait until data transfer complete. */
    while (!(FLEXIO_GetShifterStatusFlags(FLEXIOBases[0]) & (1U << FX_UART_TX_SFT(instance))))
    {
    }
        
    FLEXIOBases[0]->SHIFTBUF[FX_UART_TX_SFT(instance)] = ch;
}

/**
 * @brief  串口接收一个字符
 * @note   None
 * @param  instance ：HW_FLEXIO_UART0或HW_FLEXIO_UART1
 * @retval 返回接收到的字符
 */
uint8_t FLEXIO_UART_GetChar(uint32_t instance, uint8_t *ch)
{
    /* Wait until data transfer complete. */
    if(!(FLEXIO_GetShifterStatusFlags(FLEXIOBases[0]) & (1U << FX_UART_RX_SFT(instance))))
    {
        return CH_ERR;
    }
    *ch = FLEXIOBases[0]->SHIFTBUFBYS[FX_UART_RX_SFT(instance)];
    return CH_OK;
}

#endif


