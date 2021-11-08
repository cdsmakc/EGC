/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : EGC_LG1.c
    Project name : EGC
    Module name  : 
    Date created : 2021年09月06日    15时26分4秒
    Author       : Ning.JianLi
    Description  : LG1(LCD Group1)模块源文件。
*******************************************************************************/

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "pub_type.h"
#include "pub_lib.h"

CODE_SECTION("==========================") ;
CODE_SECTION("==  LG1模块包含文件     ==") ;
CODE_SECTION("==========================") ;
/* <标准库> */
#include <string.h>

/* <第三方> */
#include "os.h"

/* <项目内部> */
#include "drv_spi_stm32f4xx.h"

#include "EGC_Boards.h"
#include "EGC_Config.h"
#include "EGC_LG1.h"

CODE_SECTION("==========================") ;
CODE_SECTION("==  LG1模块全局变量     ==") ;
CODE_SECTION("==========================") ;
/* SPI设备 */
STATIC DRV_SPI_DEVICE_S g_stLG1SPIDevice ;

/* 工作区 */
EGC_LG1_WORKAREA_S      g_stLG1WA ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  LG1模块内部函数     ==") ;
CODE_SECTION("==========================") ;

/*******************************************************************************
- Function    : __EGC_LG1_INIT_TailLeadZeros
- Description : 本函数检查一个输入数据,从最低位到最高位,遇到的第一个bit '1'之前,
                一共有多少个bit '0'.
- Input       : uiData : 要检查的数据.
- Output      : NULL 
- Return      : UCHAR  : 从低位到高位,第一个出现的bit '1'之前,一共出现的0的个数.
- Others      : 特殊情况 : 如果uiData为0,则返回为0.
*******************************************************************************/
STATIC UCHAR __EGC_LG1_INIT_TailLeadZeros(UINT uiData)
{
    UCHAR ucLoop ;

    for(ucLoop = 0 ; ucLoop <= 31 ; ucLoop++)
    {
        if(0 != (uiData & (1 << ucLoop)))
        {
            return ucLoop ;
        }
    }

    return 0 ;
}

/*******************************************************************************
- Function    : __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup
- Description : 本函数输入一个EGC_Boards.h文件中定义的引脚(EGC_BOARDS_GPIO_Pxx)，
                返回该引脚所在的GPIO组(GPIOx)。
- Input       : uiPin         ：EGC_BOARDS_GPIO_Pxx
- Output      : ppstGPIOGroup ：用于保存GPIOx的指针
- Return      : VOID
- Others      :
*******************************************************************************/
STATIC VOID __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup(UINT uiPin, GPIO_TypeDef **ppstGPIOGroup)
{
    switch(EGC_BOARDS_GPIO_GROUP_MASK & uiPin)
    {
        case EGC_BOARDS_GPIO_GROUP_A :
            *ppstGPIOGroup = GPIOA ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_B :
            *ppstGPIOGroup = GPIOB ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_C :
            *ppstGPIOGroup = GPIOC ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_D :
            *ppstGPIOGroup = GPIOD ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_E :
            *ppstGPIOGroup = GPIOE ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_F :
            *ppstGPIOGroup = GPIOF ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_G :
            *ppstGPIOGroup = GPIOG ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_H :
            *ppstGPIOGroup = GPIOH ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_I :
            *ppstGPIOGroup = GPIOI ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_J :
            *ppstGPIOGroup = GPIOJ ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_K :
            *ppstGPIOGroup = GPIOK ;
            break ;            
        default:
            *ppstGPIOGroup = NULL ;
    }

    return ;
}


/*******************************************************************************
- Function    : __EGC_LG1_INIT_CONFIGGPIO_GetPinIndex
- Description : 本函数输入一个EGC_Boards.h文件中定义的引脚(EGC_BOARDS_GPIO_Pxx)，
                返回该引脚的GPIO序号(GPIO_Pin_x)。
- Input       : uiPin    ：EGC_BOARDS_GPIO_Pxx
- Output      : puiIndex ：用于保存GPIO_Pin_x的指针
- Return      : VOID
- Others      :
*******************************************************************************/
STATIC VOID __EGC_LG1_INIT_CONFIGGPIO_GetPinIndex(UINT uiPin, UINT *puiIndex)
{
    switch(EGC_BOARDS_GPIO_BIT_MASK & uiPin)
    {
        case EGC_BOARDS_GPIO_BIT_0 :
            *puiIndex = GPIO_Pin_0 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_1 :
            *puiIndex = GPIO_Pin_1 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_2 :
            *puiIndex = GPIO_Pin_2 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_3 :
            *puiIndex = GPIO_Pin_3 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_4 :
            *puiIndex = GPIO_Pin_4 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_5 :
            *puiIndex = GPIO_Pin_5 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_6 :
            *puiIndex = GPIO_Pin_6 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_7 :
            *puiIndex = GPIO_Pin_7 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_8 :
            *puiIndex = GPIO_Pin_8 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_9 :
            *puiIndex = GPIO_Pin_9 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_10 :
            *puiIndex = GPIO_Pin_10 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_11 :
            *puiIndex = GPIO_Pin_11 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_12 :
            *puiIndex = GPIO_Pin_12 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_13 :
            *puiIndex = GPIO_Pin_13 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_14 :
            *puiIndex = GPIO_Pin_14 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_15 :
            *puiIndex = GPIO_Pin_15 ;
            break ;
        default:
            *puiIndex = NULL ;
    }

    return ;
}


/*******************************************************************************
- Function    : __EGC_LG1_INIT_CONFIGGPIO_SetPinGPIOClock
- Description : 本函数输入一个EGC_Boards.h文件中定义的引脚(EGC_BOARDS_GPIO_Pxx)，
                使能该GPIO所在GPIO组的时钟。
- Input       : uiPin    ：EGC_BOARDS_GPIO_Pxx
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG1_INIT_CONFIGGPIO_SetPinGPIOClock(UINT uiPin)
{
    UINT          uiAHBAPBPeriphClockCtrl = 0 ;
    GPIO_TypeDef *pstGPIOGroup ;

    __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup(uiPin, &pstGPIOGroup) ;

    switch((UINT)pstGPIOGroup)
    {
        case (UINT)GPIOA : 
            uiAHBAPBPeriphClockCtrl |= RCC_AHB1Periph_GPIOA ;
            break ;
        case (UINT)GPIOB :
            uiAHBAPBPeriphClockCtrl |= RCC_AHB1Periph_GPIOB ;
            break ;
        case (UINT)GPIOC :
            uiAHBAPBPeriphClockCtrl |= RCC_AHB1Periph_GPIOC ;
            break ;
        case (UINT)GPIOD :
            uiAHBAPBPeriphClockCtrl |= RCC_AHB1Periph_GPIOD ;
            break ;
        case (UINT)GPIOE : 
            uiAHBAPBPeriphClockCtrl |= RCC_AHB1Periph_GPIOE ;
            break ;
        case (UINT)GPIOF :
            uiAHBAPBPeriphClockCtrl |= RCC_AHB1Periph_GPIOF ;
            break ;
        case (UINT)GPIOG :
            uiAHBAPBPeriphClockCtrl |= RCC_AHB1Periph_GPIOG ;
            break ;
        case (UINT)GPIOH :
            uiAHBAPBPeriphClockCtrl |= RCC_AHB1Periph_GPIOH ;
            break ;
        case (UINT)GPIOI : 
            uiAHBAPBPeriphClockCtrl |= RCC_AHB1Periph_GPIOI ;
            break ;
        case (UINT)GPIOJ :
            uiAHBAPBPeriphClockCtrl |= RCC_AHB1Periph_GPIOJ ;
            break ;
        case (UINT)GPIOK :
            uiAHBAPBPeriphClockCtrl |= RCC_AHB1Periph_GPIOK ;
            break ;
        default :
            break ;
    }

    RCC_AHB1PeriphClockCmd(uiAHBAPBPeriphClockCtrl, ENABLE) ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_LG1_INIT_ConfigGPIO
- Description : 本函数用于配置LG1模块使用到的MCU的GPIO。这些GPIO包括：
                D/C#     : 液晶屏指令/数据指示信号。
                RESET#   : 液晶屏复位信号。
                SPI_CS1# : 液晶屏1 SPI NSS信号。
                SPI_CS2# : 液晶屏2 SPI NSS信号。
                SPI_CS3# : 液晶屏3 SPI NSS信号。
                SPI_CS4# : 液晶屏4 SPI NSS信号。
                SPI_NSS  : 液晶屏SPI的NSS信号(需指定，但无硬件连接)。
                SPI_SCK  : 液晶屏SPI的SCK信号。
                SPI_MOSI : 液晶屏SPI的MOSI信号。
                SPI_MISO : 液晶屏SPI的MISO信号(需指定，但无硬件连接)。
                
                其中,SPI_NSS,SPI_SCK,SPI_MOSI,SPI_MISO相关引脚在SPI驱动中配置，
                此处不配置。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG1_INIT_ConfigGPIO(VOID)
{
    GPIO_InitTypeDef  stGPIOInitInfo ;
    GPIO_TypeDef     *pstLG1DCNGPIOGroup ;
    GPIO_TypeDef     *pstLG1RESETNGPIOGroup ;
    GPIO_TypeDef     *pstLG1SPICS1NGPIOGroup ;
    GPIO_TypeDef     *pstLG1SPICS2NGPIOGroup ;
    GPIO_TypeDef     *pstLG1SPICS3NGPIOGroup ;
    GPIO_TypeDef     *pstLG1SPICS4NGPIOGroup ;
    GPIO_TypeDef     *pstLG1PWMGPIOGroup ;
    UINT              uiLG1DCNGPIOIndex ;
    UINT              uiLG1RESETNGPIOIndex ;
    UINT              uiLG1SPICS1NGPIOIndex ;
    UINT              uiLG1SPICS2NGPIOIndex ;
    UINT              uiLG1SPICS3NGPIOIndex ;
    UINT              uiLG1SPICS4NGPIOIndex ;
    UINT              uiLG1PWMGPIOIndex ;

    /* 获取引脚的GPIO Group */
    __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP1_GPIO_DCN,      &pstLG1DCNGPIOGroup) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP1_GPIO_RESETN,   &pstLG1RESETNGPIOGroup) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP1_GPIO_SPI_CS1N, &pstLG1SPICS1NGPIOGroup) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP1_GPIO_SPI_CS2N, &pstLG1SPICS2NGPIOGroup) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP1_GPIO_SPI_CS3N, &pstLG1SPICS3NGPIOGroup) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP1_GPIO_SPI_CS4N, &pstLG1SPICS4NGPIOGroup) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_BRIGHTNESS_ADJ,       &pstLG1PWMGPIOGroup) ;

    /* 获取引脚的GPIO   Index */
    __EGC_LG1_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP1_GPIO_DCN,      &uiLG1DCNGPIOIndex) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP1_GPIO_RESETN,   &uiLG1RESETNGPIOIndex) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP1_GPIO_SPI_CS1N, &uiLG1SPICS1NGPIOIndex) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP1_GPIO_SPI_CS2N, &uiLG1SPICS2NGPIOIndex) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP1_GPIO_SPI_CS3N, &uiLG1SPICS3NGPIOIndex) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP1_GPIO_SPI_CS4N, &uiLG1SPICS4NGPIOIndex) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_BRIGHTNESS_ADJ,       &uiLG1PWMGPIOIndex) ;

    /* 使能GPIO时钟 */
    __EGC_LG1_INIT_CONFIGGPIO_SetPinGPIOClock(EGC_LCD_GROUP1_GPIO_DCN) ;
    __EGC_LG1_INIT_CONFIGGPIO_SetPinGPIOClock(EGC_LCD_GROUP1_GPIO_RESETN) ;
    __EGC_LG1_INIT_CONFIGGPIO_SetPinGPIOClock(EGC_LCD_GROUP1_GPIO_SPI_CS1N) ;
    __EGC_LG1_INIT_CONFIGGPIO_SetPinGPIOClock(EGC_LCD_GROUP1_GPIO_SPI_CS2N) ;
    __EGC_LG1_INIT_CONFIGGPIO_SetPinGPIOClock(EGC_LCD_GROUP1_GPIO_SPI_CS3N) ;
    __EGC_LG1_INIT_CONFIGGPIO_SetPinGPIOClock(EGC_LCD_GROUP1_GPIO_SPI_CS4N) ;
    __EGC_LG1_INIT_CONFIGGPIO_SetPinGPIOClock(EGC_LCD_BRIGHTNESS_ADJ) ;

    /* 引脚信息配置 */
    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_OUT ;
    stGPIOInitInfo.GPIO_Speed = GPIO_Speed_50MHz ;
    stGPIOInitInfo.GPIO_OType = GPIO_OType_PP ;
    stGPIOInitInfo.GPIO_PuPd  = GPIO_PuPd_NOPULL ;

    stGPIOInitInfo.GPIO_Pin = uiLG1DCNGPIOIndex ;
    GPIO_Init(pstLG1DCNGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin = uiLG1RESETNGPIOIndex ;
    GPIO_Init(pstLG1RESETNGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin = uiLG1SPICS1NGPIOIndex ;
    GPIO_Init(pstLG1SPICS1NGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin = uiLG1SPICS2NGPIOIndex ;
    GPIO_Init(pstLG1SPICS2NGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin = uiLG1SPICS3NGPIOIndex ;
    GPIO_Init(pstLG1SPICS3NGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin = uiLG1SPICS4NGPIOIndex ;
    GPIO_Init(pstLG1SPICS4NGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_AF ;

    stGPIOInitInfo.GPIO_Pin = uiLG1PWMGPIOIndex ;
    GPIO_Init(pstLG1PWMGPIOGroup, &stGPIOInitInfo) ;

    GPIO_PinAFConfig(pstLG1PWMGPIOGroup, __EGC_LG1_INIT_TailLeadZeros(uiLG1PWMGPIOIndex), GPIO_AF_TIM2) ;

    GPIO_SetBits(pstLG1SPICS1NGPIOGroup, uiLG1SPICS1NGPIOIndex) ;
    GPIO_SetBits(pstLG1SPICS2NGPIOGroup, uiLG1SPICS2NGPIOIndex) ;
    GPIO_SetBits(pstLG1SPICS3NGPIOGroup, uiLG1SPICS3NGPIOIndex) ;
    GPIO_SetBits(pstLG1SPICS4NGPIOGroup, uiLG1SPICS4NGPIOIndex) ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_LG1_INIT_ConfigTIM
- Description : 本函数用于配置模块使用到的定时器。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG1_INIT_ConfigTIM(VOID)
{
    UINT                    uiAHBAPBPeriphClockCtrl = 0 ;
    TIM_TimeBaseInitTypeDef stTimeBaseInitInfo ;
    RCC_ClocksTypeDef       stSystemClocks ;
    TIM_OCInitTypeDef       stOCInitInfo ;
    USHORT                  usPrescaler ;

    /* 使能TIM时钟 */
    switch((UINT)EGC_BOARDS_PERIPH_TIM_LCD_PWM)
    {
        case (UINT)TIM1 :
            uiAHBAPBPeriphClockCtrl = RCC_APB2Periph_TIM1 ;
            break ;
        case (UINT)TIM2 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_TIM2 ;
            break ;
        case (UINT)TIM3 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_TIM3 ;
            break ;
        case (UINT)TIM4 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_TIM4 ;
            break ;
        case (UINT)TIM5 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_TIM5 ;
            break ;
        case (UINT)TIM8 :
            uiAHBAPBPeriphClockCtrl = RCC_APB2Periph_TIM8 ;
            break ;
        case (UINT)TIM9 :
            uiAHBAPBPeriphClockCtrl = RCC_APB2Periph_TIM9 ;
            break ;
        case (UINT)TIM12 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_TIM12 ;
            break ;
        default :
            break ;
    }
    
    if(TIM1 == EGC_BOARDS_PERIPH_TIM_LCD_PWM || TIM8 == EGC_BOARDS_PERIPH_TIM_LCD_PWM || TIM9 == EGC_BOARDS_PERIPH_TIM_LCD_PWM)
    {
        RCC_APB2PeriphClockCmd(uiAHBAPBPeriphClockCtrl, ENABLE) ;
    }
    else
    {
        RCC_APB1PeriphClockCmd(uiAHBAPBPeriphClockCtrl, ENABLE) ;
    }

    /* 获取系统时钟信息 */
    RCC_GetClocksFreq(&stSystemClocks) ;

    if(TIM1 == EGC_BOARDS_PERIPH_TIM_LCD_PWM || TIM8 == EGC_BOARDS_PERIPH_TIM_LCD_PWM || TIM9 == EGC_BOARDS_PERIPH_TIM_LCD_PWM)
    {
        usPrescaler = stSystemClocks.PCLK2_Frequency * 2 / (EGC_BOARDS_PERIPH_TIM_LCD_PWM_FREQUENCY * EGC_BOARDS_PERIPH_TIM_LCD_PWM_COUNTER_NUM) ;
    }
    else
    {
        usPrescaler = stSystemClocks.PCLK1_Frequency * 2 / (EGC_BOARDS_PERIPH_TIM_LCD_PWM_FREQUENCY * EGC_BOARDS_PERIPH_TIM_LCD_PWM_COUNTER_NUM) ;
    }

    stTimeBaseInitInfo.TIM_Prescaler     = usPrescaler ;
    stTimeBaseInitInfo.TIM_CounterMode   = TIM_CounterMode_Up ;
    stTimeBaseInitInfo.TIM_Period        = EGC_BOARDS_PERIPH_TIM_LCD_PWM_COUNTER_NUM - 1 ;
    stTimeBaseInitInfo.TIM_ClockDivision = TIM_CKD_DIV1 ;

    TIM_TimeBaseInit(EGC_BOARDS_PERIPH_TIM_LCD_PWM, &stTimeBaseInitInfo) ;

    /* 使能ARR预装载 */
    TIM_ARRPreloadConfig(EGC_BOARDS_PERIPH_TIM_LCD_PWM, ENABLE) ;

    /* 配置比较通道 */
    stOCInitInfo.TIM_OCMode              = EGC_BOARDS_PERIPH_TIM_LCD_PWM_OC_MODE ;
    stOCInitInfo.TIM_OutputState         = TIM_OutputState_Enable ;
    stOCInitInfo.TIM_OutputNState        = TIM_OutputNState_Enable ;
    stOCInitInfo.TIM_Pulse               = 0 ;
    stOCInitInfo.TIM_OCPolarity          = TIM_OCPolarity_High ;
    stOCInitInfo.TIM_OCNPolarity         = TIM_OCNPolarity_High ;
    stOCInitInfo.TIM_OCIdleState         = TIM_OCIdleState_Reset ;
    stOCInitInfo.TIM_OCNIdleState        = TIM_OCNIdleState_Reset ;

    /* 配置通道 */
    switch((UINT)EGC_BOARDS_PERIPH_TIM_LCD_PWM_CHANNEL)
    {
        case TIM_Channel_1 :
            TIM_OC1Init(EGC_BOARDS_PERIPH_TIM_LCD_PWM, &stOCInitInfo) ;
            break ;
        case TIM_Channel_2 :
            TIM_OC2Init(EGC_BOARDS_PERIPH_TIM_LCD_PWM, &stOCInitInfo) ;
            break ;
        case TIM_Channel_3 :
            TIM_OC3Init(EGC_BOARDS_PERIPH_TIM_LCD_PWM, &stOCInitInfo) ;
            break ;
        case TIM_Channel_4 :
            TIM_OC4Init(EGC_BOARDS_PERIPH_TIM_LCD_PWM, &stOCInitInfo) ;
            break ;
        default :
            break ;
    }

    /* 配置中断 */
    TIM_ITConfig(EGC_BOARDS_PERIPH_TIM_LCD_PWM, TIM_IT_Update,  DISABLE) ;
    TIM_ITConfig(EGC_BOARDS_PERIPH_TIM_LCD_PWM, TIM_IT_CC1,     DISABLE) ;
    TIM_ITConfig(EGC_BOARDS_PERIPH_TIM_LCD_PWM, TIM_IT_CC2,     DISABLE) ;
    TIM_ITConfig(EGC_BOARDS_PERIPH_TIM_LCD_PWM, TIM_IT_CC3,     DISABLE) ;
    TIM_ITConfig(EGC_BOARDS_PERIPH_TIM_LCD_PWM, TIM_IT_CC4,     DISABLE) ;
    TIM_ITConfig(EGC_BOARDS_PERIPH_TIM_LCD_PWM, TIM_IT_Trigger, DISABLE) ;

    /* 禁止NVIC中断 */
    //NVIC_DisableIRQ() ;

    /* 启动定时器工作 */
    TIM_Cmd(EGC_BOARDS_PERIPH_TIM_LCD_PWM, ENABLE) ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_LG1_INIT_ConfigLCD
- Description : 本函数初始化LCD。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG1_SetLCDScreenReset(BOOL_T bReset) ;
VOID __EGC_LG1_WriteLCDScreenSetInfo(EGC_LG1_LCD_DEF_E enLCD, UCHAR ucCmd, UCHAR *pucData, UINT uiSize) ;

VOID __EGC_LG1_INIT_ConfigLCD(VOID)
{
    OS_ERR enErr ;
    UCHAR  aucConfigBuf[32] ;

    /* 复位LCD */
    __EGC_LG1_SetLCDScreenReset(BOOL_TRUE) ;

    /* 等待一段时间 */
    OSTimeDlyHMSM((CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT32U   )200u,
                  (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                  (OS_ERR      *)&enErr);

    /* 撤销复位 */
    __EGC_LG1_SetLCDScreenReset(BOOL_FALSE) ;

    /* 等待一段时间 */
    OSTimeDlyHMSM((CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT32U   )200u,
                  (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                  (OS_ERR      *)&enErr);

    /* sleep out */
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB1_SLPOUT, NULL, 0) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB1_SLPOUT, NULL, 0) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB1_SLPOUT, NULL, 0) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB1_SLPOUT, NULL, 0) ;

    /* 等待一段时间 */
    OSTimeDlyHMSM((CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT32U   )200u,
                  (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                  (OS_ERR      *)&enErr);

    /* MADCTL */
    aucConfigBuf[0] = 0x00 ;

    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB1_MADCTL, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB1_MADCTL, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB1_MADCTL, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB1_MADCTL, aucConfigBuf, 1) ;
    
    /* COLMOD */
    aucConfigBuf[0] = 0x05 ;

    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB1_COLMOD, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB1_COLMOD, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB1_COLMOD, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB1_COLMOD, aucConfigBuf, 1) ;

    /* INVON */
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB1_INVON, NULL, 0) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB1_INVON, NULL, 0) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB1_INVON, NULL, 0) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB1_INVON, NULL, 0) ;

    /* PORCTL */
    aucConfigBuf[0] = 0x05 ;
    aucConfigBuf[1] = 0x05 ;
    aucConfigBuf[2] = 0x00 ;
    aucConfigBuf[3] = 0x33 ;
    aucConfigBuf[4] = 0x33 ;

    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB2_PORCTRL, aucConfigBuf, 5) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB2_PORCTRL, aucConfigBuf, 5) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB2_PORCTRL, aucConfigBuf, 5) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB2_PORCTRL, aucConfigBuf, 5) ;

    /* GCTRL */
    aucConfigBuf[0] = 0x23 ;

    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB2_GCTRL, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB2_GCTRL, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB2_GCTRL, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB2_GCTRL, aucConfigBuf, 1) ;

    /* VCOMS */
    aucConfigBuf[0] = 0x22 ;

    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB2_VCOMS, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB2_VCOMS, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB2_VCOMS, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB2_VCOMS, aucConfigBuf, 1) ;

    /* LCMCTRL */
    aucConfigBuf[0] = 0x2C ;

    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB2_LCMCTRL, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB2_LCMCTRL, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB2_LCMCTRL, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB2_LCMCTRL, aucConfigBuf, 1) ;

    /* VDVVRHEN */
    aucConfigBuf[0] = 0x01 ;

    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB2_VDVVRHEN, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB2_VDVVRHEN, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB2_VDVVRHEN, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB2_VDVVRHEN, aucConfigBuf, 1) ;

    /* VRHS */
    aucConfigBuf[0] = 0x13 ;

    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB2_VRHS, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB2_VRHS, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB2_VRHS, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB2_VRHS, aucConfigBuf, 1) ;

    /* VDVS */
    aucConfigBuf[0] = 0x20 ;

    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB2_VDVS, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB2_VDVS, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB2_VDVS, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB2_VDVS, aucConfigBuf, 1) ;

    /* FRCTRL2 */
    aucConfigBuf[0] = 0x0F ;

    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB2_FRCTRL2, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB2_FRCTRL2, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB2_FRCTRL2, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB2_FRCTRL2, aucConfigBuf, 1) ;
        
    /* PWCTRL1 */
    aucConfigBuf[0] = 0xA4 ;
    aucConfigBuf[1] = 0xA1 ;

    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB2_PWCTRL1, aucConfigBuf, 2) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB2_PWCTRL1, aucConfigBuf, 2) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB2_PWCTRL1, aucConfigBuf, 2) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB2_PWCTRL1, aucConfigBuf, 2) ;

    /* ????? */
    aucConfigBuf[0] = 0xA1 ;

    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, 0xD6, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, 0xD6, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, 0xD6, aucConfigBuf, 1) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, 0xD6, aucConfigBuf, 1) ;

    /* PVGAMCTRL */
    aucConfigBuf[0]  = 0x70 ;
    aucConfigBuf[1]  = 0x06 ;
    aucConfigBuf[2]  = 0x0C ;
    aucConfigBuf[3]  = 0x08 ;
    aucConfigBuf[4]  = 0x09 ;
    aucConfigBuf[5]  = 0x27 ;
    aucConfigBuf[6]  = 0x2E ;
    aucConfigBuf[7]  = 0x34 ;
    aucConfigBuf[8]  = 0x46 ;
    aucConfigBuf[9]  = 0x37 ;
    aucConfigBuf[10] = 0x13 ;
    aucConfigBuf[11] = 0x13 ;
    aucConfigBuf[12] = 0x25 ;
    aucConfigBuf[13] = 0x2A ;

    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB2_PVGAMCTRL, aucConfigBuf, 14) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB2_PVGAMCTRL, aucConfigBuf, 14) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB2_PVGAMCTRL, aucConfigBuf, 14) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB2_PVGAMCTRL, aucConfigBuf, 14) ;

    /* NVGAMCTRL */
    aucConfigBuf[0]  = 0x70 ;
    aucConfigBuf[1]  = 0x04 ;
    aucConfigBuf[2]  = 0x08 ;
    aucConfigBuf[3]  = 0x09 ;
    aucConfigBuf[4]  = 0x07 ;
    aucConfigBuf[5]  = 0x03 ;
    aucConfigBuf[6]  = 0x2C ;
    aucConfigBuf[7]  = 0x42 ;
    aucConfigBuf[8]  = 0x42 ;
    aucConfigBuf[9]  = 0x38 ;
    aucConfigBuf[10] = 0x14 ;
    aucConfigBuf[11] = 0x14 ;
    aucConfigBuf[12] = 0x27 ;
    aucConfigBuf[13] = 0x2C ;

    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB2_NVGAMCTRL, aucConfigBuf, 14) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB2_NVGAMCTRL, aucConfigBuf, 14) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB2_NVGAMCTRL, aucConfigBuf, 14) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB2_NVGAMCTRL, aucConfigBuf, 14) ;

    /* DISPON */
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB1_DISPON, NULL, 0) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB1_DISPON, NULL, 0) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB1_DISPON, NULL, 0) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB1_DISPON, NULL, 0) ;

    /* CASET */
    aucConfigBuf[0]  = 0x00 ;
    aucConfigBuf[1]  = 0x34 ;
    aucConfigBuf[2]  = 0x00 ;
    aucConfigBuf[3]  = 0xBA ;

    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB1_CASET, aucConfigBuf, 4) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB1_CASET, aucConfigBuf, 4) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB1_CASET, aucConfigBuf, 4) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB1_CASET, aucConfigBuf, 4) ;

    /* RASET */
    aucConfigBuf[0]  = 0x00 ;
    aucConfigBuf[1]  = 0x28 ;
    aucConfigBuf[2]  = 0x01 ;
    aucConfigBuf[3]  = 0x17 ;

    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD1, EGC_LG1_ST7789V_CMD_TB1_RASET, aucConfigBuf, 4) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD2, EGC_LG1_ST7789V_CMD_TB1_RASET, aucConfigBuf, 4) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD3, EGC_LG1_ST7789V_CMD_TB1_RASET, aucConfigBuf, 4) ;
    __EGC_LG1_WriteLCDScreenSetInfo(LG1_LCD4, EGC_LG1_ST7789V_CMD_TB1_RASET, aucConfigBuf, 4) ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_LG1_Init
- Description : 本函数对LG1模块需要初始化的部分进行初始化。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG1_Init(VOID)
{
    DRV_SPI_INIT_INFO_S           stSPIInitInfo ;
    volatile DRV_SPI_ERROR_CODE_E enSPIRetval ;
    
    /* 复位错误码 */
    g_stLG1WA.uiErrCode        = 0 ;

    /* 初始化GPIO */
    __EGC_LG1_INIT_ConfigGPIO() ;

    /* 创建SPI设备 */
    __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP1_GPIO_SPI_NSS_NC,  &stSPIInitInfo.pstNSSGPIOGroup) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP1_GPIO_SPI_SCK,     &stSPIInitInfo.pstSCKGPIOGroup) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP1_GPIO_SPI_MOSI,    &stSPIInitInfo.pstMOSIGPIOGroup) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP1_GPIO_SPI_MISO_NC, &stSPIInitInfo.pstMISOGPIOGroup) ;

    __EGC_LG1_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP1_GPIO_SPI_NSS_NC,  &stSPIInitInfo.uiNSSPinID) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP1_GPIO_SPI_SCK,     &stSPIInitInfo.uiSCKPinID) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP1_GPIO_SPI_MOSI,    &stSPIInitInfo.uiMOSIPinID) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP1_GPIO_SPI_MISO_NC, &stSPIInitInfo.uiMISOPinID) ;

    stSPIInitInfo.ucSCKPinAFDef     = EGC_BOARDS_PERIPH_SPI_LCD_GROUP1_AF_DEF ;
    stSPIInitInfo.ucMOSIPinAFDef    = EGC_BOARDS_PERIPH_SPI_LCD_GROUP1_AF_DEF ;
    stSPIInitInfo.ucMISOPinAFDef    = EGC_BOARDS_PERIPH_SPI_LCD_GROUP1_AF_DEF ;

    stSPIInitInfo.pstSPIPeriph      = EGC_BOARDS_PERIPH_SPI_LCD_GROUP1 ;
    stSPIInitInfo.usSPIPrescaler    = SPI_BaudRatePrescaler_2 ;
    stSPIInitInfo.ucSPIMode         = DRV_SPI_MODE_0 ;
    stSPIInitInfo.uiSPIIrqNumber    = EGC_BOARDS_PERIPH_SPI_LCD_GROUP1_INTN ;
    stSPIInitInfo.bUseDMA           = BOOL_TRUE ;

    stSPIInitInfo.pstSPIDMATxStream = EGC_BOARDS_PERIPH_SPI_LCD_GROUP1_DMA_TX_STREAM ;
    stSPIInitInfo.pstSPIDMARxStream = EGC_BOARDS_PERIPH_SPI_LCD_GROUP1_DMA_RX_STREAM ;
    stSPIInitInfo.uiSPIDMATxChannel = EGC_BOARDS_PERIPH_SPI_LCD_GROUP1_DMA_TX_CHANNEL ;
    stSPIInitInfo.uiSPIDMARxChannel = EGC_BOARDS_PERIPH_SPI_LCD_GROUP1_DMA_RX_CHANNEL ;
    stSPIInitInfo.uiSPIDMATxIrqNum  = EGC_BOARDS_PERIPH_SPI_LCD_GROUP1_DMA_TX_IRQ_NUM ;
    stSPIInitInfo.uiSPIDMARxIrqNum  = EGC_BOARDS_PERIPH_SPI_LCD_GROUP1_DMA_RX_IRQ_NUM ;

    enSPIRetval = DRV_SPI_Open(&g_stLG1SPIDevice, &stSPIInitInfo) ;

    /* 初始化LCD */
    __EGC_LG1_INIT_ConfigLCD() ;

    /* 初始化LCD背光 */
    __EGC_LG1_INIT_ConfigTIM() ;

    /* 部分变量初始化 */
    g_stLG1WA.uiLCD1UpdateView   = 1 ;
    g_stLG1WA.uiLCD2UpdateView   = 1 ;
    g_stLG1WA.uiLCD3UpdateView   = 1 ;
    g_stLG1WA.uiLCD4UpdateView   = 1 ;
    g_stLG1WA.uiLCD1UseSecondBuf = 0 ;
    g_stLG1WA.uiLCD2UseSecondBuf = 0 ;
    g_stLG1WA.uiLCD3UseSecondBuf = 0 ;
    g_stLG1WA.uiLCD4UseSecondBuf = 0 ;
    g_stLG1WA.enBrightnessLevel  = BRIGHTNESS_LEVEL_MAX ;
    g_stLG1WA.uiTaskRun          = 0 ;

    /* 初始化完成 */
    g_stLG1WA.uiInitDone = 1 ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_LG1_SetLCDBrightness
- Description : 本函数调节LCD背光亮度。
- Input       : enBrightness ：
                    亮度等级设置。
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG1_SetLCDBrightness(EGC_CONFIG_LCD_BRSS_DEF_E enBrightness) 
{
    USHORT ausBrightness2Compare[11] = {0, 
                                        1 * (EGC_BOARDS_PERIPH_TIM_LCD_PWM_COUNTER_NUM / 10), 
                                        2 * (EGC_BOARDS_PERIPH_TIM_LCD_PWM_COUNTER_NUM / 10), 
                                        3 * (EGC_BOARDS_PERIPH_TIM_LCD_PWM_COUNTER_NUM / 10), 
                                        4 * (EGC_BOARDS_PERIPH_TIM_LCD_PWM_COUNTER_NUM / 10), 
                                        5 * (EGC_BOARDS_PERIPH_TIM_LCD_PWM_COUNTER_NUM / 10), 
                                        6 * (EGC_BOARDS_PERIPH_TIM_LCD_PWM_COUNTER_NUM / 10), 
                                        7 * (EGC_BOARDS_PERIPH_TIM_LCD_PWM_COUNTER_NUM / 10), 
                                        8 * (EGC_BOARDS_PERIPH_TIM_LCD_PWM_COUNTER_NUM / 10), 
                                        9 * (EGC_BOARDS_PERIPH_TIM_LCD_PWM_COUNTER_NUM / 10), 
                                        EGC_BOARDS_PERIPH_TIM_LCD_PWM_COUNTER_NUM - 1} ;

    /* 保护，避免溢出 */
    if(BRIGHTNESS_LEVEL_MAX < enBrightness)
    {
        enBrightness = BRIGHTNESS_LEVEL_MAX ;
    }

    switch(EGC_BOARDS_PERIPH_TIM_LCD_PWM_CHANNEL)
    {
        case TIM_Channel_1 :
            TIM_SetCompare1(EGC_BOARDS_PERIPH_TIM_LCD_PWM, ausBrightness2Compare[enBrightness]) ;
            break ;
        case TIM_Channel_2 :
            TIM_SetCompare2(EGC_BOARDS_PERIPH_TIM_LCD_PWM, ausBrightness2Compare[enBrightness]) ;
            break ;
        case TIM_Channel_3 :
            TIM_SetCompare3(EGC_BOARDS_PERIPH_TIM_LCD_PWM, ausBrightness2Compare[enBrightness]) ;
            break ;
        case TIM_Channel_4 :
            TIM_SetCompare4(EGC_BOARDS_PERIPH_TIM_LCD_PWM, ausBrightness2Compare[enBrightness]) ;
            break ;
        default :
            break ;
    }

    return ;
}


/*******************************************************************************
- Function    : __EGC_LG1_SetLCDScreenCSN
- Description : 本函数对LG1模块控制的LCD的SPI CS信号进行控制。由于本模块的1个SPI
                控制4个LCD，因此需要4个CS信号。SPI驱动中的CS信号不使用。
- Input       : enCSSel ：
                    决定控制哪个LCD的CS信号。
                enCSCmd ：
                    决定信号的状态。
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG1_SetLCDScreenCSN(EGC_LG1_LCD_DEF_E enCSSel, EGC_LG1_SPI_CS_CMD_E enCSCmd)
{
    UINT          uiPin ;
    GPIO_TypeDef *pstGPIOGroup ;
    UINT          uiGPIOIndex ;

    /* 确定是哪个LCD的CS引脚 */
    switch(enCSSel)
    {
        case LG1_LCD1 :
            uiPin = EGC_LCD_GROUP1_GPIO_SPI_CS1N ;
            break ;
        case LG1_LCD2 :
            uiPin = EGC_LCD_GROUP1_GPIO_SPI_CS2N ;
            break ;
        case LG1_LCD3 :
            uiPin = EGC_LCD_GROUP1_GPIO_SPI_CS3N ;
            break ;
        case LG1_LCD4 :
        default :
            uiPin = EGC_LCD_GROUP1_GPIO_SPI_CS4N ;
            break ;
    }

    /* 确定GPIO组和ID */
    __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup(uiPin, &pstGPIOGroup) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinIndex(uiPin, &uiGPIOIndex) ;

    /* 设置信号 */
    if(LG1_SPI_CS_CMD_START == enCSCmd)
    {
        GPIO_ResetBits(pstGPIOGroup, uiGPIOIndex) ;
    }
    else
    {
        GPIO_SetBits(pstGPIOGroup, uiGPIOIndex) ;
    }
    
    return ;
}


/*******************************************************************************
- Function    : __EGC_LG1_SetLCDScreenDCxData
- Description : 本函数对LG1模块控制的LCD的D/C#信号进行控制。硬件上，这一个信号
                控制所有的4个LCD。本函数控制该信号为数据模式。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG1_SetLCDScreenDCxData(VOID)
{
    GPIO_TypeDef *pstGPIOGroup ;
    UINT          uiGPIOIndex ;

    /* 确定GPIO组和ID */
    __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP1_GPIO_DCN, &pstGPIOGroup) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP1_GPIO_DCN, &uiGPIOIndex) ;

    /* 设置引脚为高电平 */
    GPIO_SetBits(pstGPIOGroup, uiGPIOIndex) ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_LG1_SetLCDScreenDCxCommand
- Description : 本函数对LG1模块控制的LCD的D/C#信号进行控制。硬件上，这一个信号
                控制所有的4个LCD。本函数控制该信号为命令模式。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG1_SetLCDScreenDCxCommand(VOID)
{
    GPIO_TypeDef *pstGPIOGroup ;
    UINT          uiGPIOIndex ;

    /* 确定GPIO组和ID */
    __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP1_GPIO_DCN, &pstGPIOGroup) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP1_GPIO_DCN, &uiGPIOIndex) ;

    /* 设置引脚为低电平 */
    GPIO_ResetBits(pstGPIOGroup, uiGPIOIndex) ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_LG1_SetLCDScreenReset
- Description : 本函数对LG1模块控制的LCD的RESET#信号进行控制。硬件上，这一个信号
                控制所有的4个LCD。本函数实现LCD的复位或者复位撤销。
- Input       : bReset ：
                    BOOL_TRUE  : 激活复位；
                    BOOL_FALSE : 撤销复位。
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG1_SetLCDScreenReset(BOOL_T bReset)
{
    GPIO_TypeDef *pstGPIOGroup ;
    UINT          uiGPIOIndex ;

    /* 确定GPIO组和ID */
    __EGC_LG1_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP1_GPIO_RESETN, &pstGPIOGroup) ;
    __EGC_LG1_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP1_GPIO_RESETN, &uiGPIOIndex) ;

    if(BOOL_TRUE == bReset)
    {
        GPIO_ResetBits(pstGPIOGroup, uiGPIOIndex) ;
    }
    else
    {
        GPIO_SetBits(pstGPIOGroup, uiGPIOIndex) ;
    }

    return ;
}

/*******************************************************************************
- Function    : __EGC_LG1_WriteLCDScreenSetInfo
- Description : 本函数对LG1模块控制的LCD的配置信息进行设置。
- Input       : enLCD ：
                    选择配置哪一个LCD。
                ucCmd ：
                    LCD指令字。
                pucData ：
                    参数指针。
                uiSize ：
                    参数长度。
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG1_WriteLCDScreenSetInfo(EGC_LG1_LCD_DEF_E enLCD, UCHAR ucCmd, UCHAR *pucData, UINT uiSize)
{
    /* 设置CS信号 */
    __EGC_LG1_SetLCDScreenCSN(enLCD, LG1_SPI_CS_CMD_START) ;

    /* 设置D/C#信号 */
    __EGC_LG1_SetLCDScreenDCxCommand() ;

    /* 发送命令 */
    if(0 == uiSize)
    {
        DRV_SPI_Write((DRV_SPI_DEVICE_S *)&g_stLG1SPIDevice, 
                      (UCHAR            *)&ucCmd, 
                      (UCHAR            *)NULL, 
                      (UINT              )sizeof(ucCmd), 
                      (UINT              )(DRV_SPI_READ_WRITE_MODE_INT | DRV_SPI_READ_WRITE_WITH_START | DRV_SPI_READ_WRITE_WITH_STOP)) ;
    }
    else
    {
        DRV_SPI_Write((DRV_SPI_DEVICE_S *)&g_stLG1SPIDevice, 
                      (UCHAR            *)&ucCmd, 
                      (UCHAR            *)NULL, 
                      (UINT              )sizeof(ucCmd), 
                      (UINT              )(DRV_SPI_READ_WRITE_MODE_INT | DRV_SPI_READ_WRITE_WITH_START)) ;
    }

    /* 设置D/C#信号 */
    __EGC_LG1_SetLCDScreenDCxData() ;
    
    /* 如有数据，发送数据 */
    if(0 != uiSize)
    {
        DRV_SPI_Write((DRV_SPI_DEVICE_S *)&g_stLG1SPIDevice, 
                      (UCHAR            *)pucData, 
                      (UCHAR            *)NULL, 
                      (UINT              )uiSize, 
                      (UINT              )(DRV_SPI_READ_WRITE_MODE_INT | DRV_SPI_READ_WRITE_WITH_STOP)) ;
    }

    /* 设置CS信号 */
    __EGC_LG1_SetLCDScreenCSN(enLCD, LG1_SPI_CS_CMD_STOP) ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_LG1_WriteLCDScreenDatInfo
- Description : 本函数对LG1模块控制的LCD的显示数据进行传输。
- Input       : enLCD ：
                    选择传输哪一个LCD。
                ucCmd ：
                    LCD指令字（0x2C）。
                pucData ：
                    数据指针。
                uiSize ：
                    数据长度。
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG1_WriteLCDScreenDatInfo(EGC_LG1_LCD_DEF_E enLCD, UCHAR ucCmd, UCHAR *pucData, UINT uiSize)
{ volatile DRV_SPI_ERROR_CODE_E enSPIRetVal ;
    /* 关闭显示 */
    /* 设置CS信号 */
    //__EGC_LG1_SetLCDScreenCSN(enLCD, LG1_SPI_CS_CMD_START) ;
    
    //__EGC_LG1_WriteLCDScreenSetInfo(enLCD, EGC_LG1_ST7789V_CMD_TB1_DISPOFF, NULL, 0) ;

    /* 设置CS信号 */
    //__EGC_LG1_SetLCDScreenCSN(enLCD, LG1_SPI_CS_CMD_STOP) ;

    /* 设置CS信号 */
    __EGC_LG1_SetLCDScreenCSN(enLCD, LG1_SPI_CS_CMD_START) ;

    /* 设置D/C#信号 */
    __EGC_LG1_SetLCDScreenDCxCommand() ;

    /* 发送命令 */
    if(0 == uiSize)
    {
        DRV_SPI_Write((DRV_SPI_DEVICE_S *)&g_stLG1SPIDevice, 
                      (UCHAR            *)&ucCmd, 
                      (UCHAR            *)NULL, 
                      (UINT              )sizeof(ucCmd), 
                      (UINT              )(DRV_SPI_READ_WRITE_MODE_INT | DRV_SPI_READ_WRITE_WITH_START | DRV_SPI_READ_WRITE_WITH_STOP)) ;
    }
    else
    {
        DRV_SPI_Write((DRV_SPI_DEVICE_S *)&g_stLG1SPIDevice, 
                      (UCHAR            *)&ucCmd, 
                      (UCHAR            *)NULL, 
                      (UINT              )sizeof(ucCmd), 
                      (UINT              )(DRV_SPI_READ_WRITE_MODE_INT | DRV_SPI_READ_WRITE_WITH_START)) ;
    }

    /* 设置D/C#信号 */
    __EGC_LG1_SetLCDScreenDCxData() ;
    
    /* 如有数据，发送数据 */
    if(0 != uiSize)
    {
        enSPIRetVal = DRV_SPI_Write((DRV_SPI_DEVICE_S *)&g_stLG1SPIDevice, 
                      (UCHAR            *)pucData, 
                      (UCHAR            *)NULL, 
                      (UINT              )uiSize, 
                      (UINT              )(DRV_SPI_READ_WRITE_MODE_DMA | DRV_SPI_READ_WRITE_WITH_STOP)) ;
    }

    /* 设置CS信号 */
    __EGC_LG1_SetLCDScreenCSN(enLCD, LG1_SPI_CS_CMD_STOP) ;


    /* 关闭显示 */
    /* 设置CS信号 */
    //__EGC_LG1_SetLCDScreenCSN(enLCD, LG1_SPI_CS_CMD_START) ;
    
    //__EGC_LG1_WriteLCDScreenSetInfo(enLCD, EGC_LG1_ST7789V_CMD_TB1_DISPON, NULL, 0) ;

    /* 设置CS信号 */
    //__EGC_LG1_SetLCDScreenCSN(enLCD, LG1_SPI_CS_CMD_STOP) ;
    
    return ;
}

CODE_SECTION("==========================") ;
CODE_SECTION("==  LG1模块任务函数     ==") ;
CODE_SECTION("==========================") ;

/*******************************************************************************
- Function    : EGC_LG1_Task
- Description : 本函数为LG1模块的任务函数。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID EGC_LG1_Task(VOID)
{
    OS_ERR enErr ;
        
    /* 模块初始化 */
    __EGC_LG1_Init() ;

    /* 等待任务启动指令 */
    while(0 == g_stLG1WA.uiTaskRun)
    {
        OSTimeDlyHMSM((CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT32U   )100u, 
                      (OS_OPT       )OS_OPT_TIME_HMSM_STRICT, 
                      (OS_ERR      *)&enErr) ;
        
        if(OS_ERR_NONE != enErr)
        {
            /* 延时函数错误，记录错误码 */
            g_stLG1WA.uiErrCode = 0 ;
        }
    }

    while(DEF_TRUE)
    {
        /* 亮度调节 */
        if(1 != g_stLG1WA.uiHide)
        {
            /* 按设定调节亮度 */
            __EGC_LG1_SetLCDBrightness(g_stLG1WA.enBrightnessLevel) ;
        }
        else
        {
            /* 关闭显示背光 */
            __EGC_LG1_SetLCDBrightness(BRIGHTNESS_LEVEL_OFF) ;
        }
        
        /* 刷新LCD1显示 */
        if(1 == g_stLG1WA.uiLCD1UpdateView)
        {
            g_stLG1WA.uiLCD1UpdateView = 0 ;

            if(1 == g_stLG1WA.uiLCD1UseSecondBuf)
            {
                __EGC_LG1_WriteLCDScreenDatInfo(LG1_LCD1, 
                                                EGC_LG1_ST7789V_CMD_TB1_RAMWR, 
                                                (UCHAR *)EGC_CONFIG_SDRAM_ADDR_LCD1_BUF2, 
                                                EGC_LG1_LCD_FRAME_SIZE) ;
            }
            else
            {
                __EGC_LG1_WriteLCDScreenDatInfo(LG1_LCD1, 
                                                EGC_LG1_ST7789V_CMD_TB1_RAMWR, 
                                                (UCHAR *)EGC_CONFIG_SDRAM_ADDR_LCD1_BUF1, 
                                                EGC_LG1_LCD_FRAME_SIZE) ;
            }
        }

        /* 刷新LCD2显示 */
        if(1 == g_stLG1WA.uiLCD2UpdateView)
        {
            g_stLG1WA.uiLCD2UpdateView = 0 ;

            if(1 == g_stLG1WA.uiLCD2UseSecondBuf)
            {
                __EGC_LG1_WriteLCDScreenDatInfo(LG1_LCD2, 
                                                EGC_LG1_ST7789V_CMD_TB1_RAMWR, 
                                                (UCHAR *)EGC_CONFIG_SDRAM_ADDR_LCD2_BUF2, 
                                                EGC_LG1_LCD_FRAME_SIZE) ;
            }
            else
            {
                __EGC_LG1_WriteLCDScreenDatInfo(LG1_LCD2, 
                                                EGC_LG1_ST7789V_CMD_TB1_RAMWR, 
                                                (UCHAR *)EGC_CONFIG_SDRAM_ADDR_LCD2_BUF1, 
                                                EGC_LG1_LCD_FRAME_SIZE) ;
            }
        }

        /* 刷新LCD3显示 */
        if(1 == g_stLG1WA.uiLCD3UpdateView)
        {
            g_stLG1WA.uiLCD3UpdateView = 0 ;

            if(1 == g_stLG1WA.uiLCD3UseSecondBuf)
            {
                __EGC_LG1_WriteLCDScreenDatInfo(LG1_LCD3, 
                                                EGC_LG1_ST7789V_CMD_TB1_RAMWR, 
                                                (UCHAR *)EGC_CONFIG_SDRAM_ADDR_LCD3_BUF2, 
                                                EGC_LG1_LCD_FRAME_SIZE) ;
            }
            else
            {
                __EGC_LG1_WriteLCDScreenDatInfo(LG1_LCD3, 
                                                EGC_LG1_ST7789V_CMD_TB1_RAMWR, 
                                                (UCHAR *)EGC_CONFIG_SDRAM_ADDR_LCD3_BUF1, 
                                                EGC_LG1_LCD_FRAME_SIZE) ;
            }
        }

        /* 刷新LCD4显示 */
        if(1 == g_stLG1WA.uiLCD4UpdateView)
        {
            g_stLG1WA.uiLCD4UpdateView = 0 ;

            if(1 == g_stLG1WA.uiLCD4UseSecondBuf)
            {
                __EGC_LG1_WriteLCDScreenDatInfo(LG1_LCD4, 
                                                EGC_LG1_ST7789V_CMD_TB1_RAMWR, 
                                                (UCHAR *)EGC_CONFIG_SDRAM_ADDR_LCD4_BUF2, 
                                                EGC_LG1_LCD_FRAME_SIZE) ;
            }
            else
            {
                __EGC_LG1_WriteLCDScreenDatInfo(LG1_LCD4, 
                                                EGC_LG1_ST7789V_CMD_TB1_RAMWR, 
                                                (UCHAR *)EGC_CONFIG_SDRAM_ADDR_LCD4_BUF1, 
                                                EGC_LG1_LCD_FRAME_SIZE) ;
            }
        }
    
        /* 周期性任务 */
        OSTimeDlyHMSM((CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT32U   )EGC_LG1_TASK_CYCLE_PERIOD_MS,
                      (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                      (OS_ERR      *)&enErr);

        if(OS_ERR_NONE != enErr)
        {
            /* 延时函数错误 */
            g_stLG1WA.uiErrCode = 0 ;
        }
    }
}


/*******************************************************************************
- Function    : EGC_LG1_SPIIntHandler
- Description : 本函数为LG1模块所使用的SPI外设的中断服务函数。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID EGC_LG1_SPIIntHandler(VOID)
{
    DRV_SPI_GeneralIntHandler(&g_stLG1SPIDevice) ;
    
    return ;
}


/*******************************************************************************
- Function    : EGC_LG1_SPIDMAIntHandler
- Description : 本函数为LG1模块所使用的SPI外设的DMA中断服务函数。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID EGC_LG1_SPIDMAIntHandler(VOID)
{
    DRV_SPI_GeneralDMAIntHandler(&g_stLG1SPIDevice) ;

    return ;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file EGC_LG1.c. *******/  

