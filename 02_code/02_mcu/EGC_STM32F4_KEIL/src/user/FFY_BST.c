/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : iv18_bst.c
    Project name : IV18
    Module name  : 
    Date created : 10 January 2020   14时29分30秒
    Author       : Ning.JianLi
    Description  : Boost模块的源文件。
*******************************************************************************/

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "IV18_BST.h"

CODE_SECTION("==========================") ;
CODE_SECTION("==  BST模块全局变量     ==") ;
CODE_SECTION("==========================") ;
/* 全局工作区 */
IV18_BST_WORKAREA_S g_stBSTWA ;

/* 亮度等级到电压的对应表 */
FLOAT g_fBSTBrightness2Vol[101] = 
{
           /* xx0     xx1     xx2     xx3     xx4     xx5     xx6     xx7     xx8     xx9 */
/* 00x */    00.00 , 10.40 , 10.80 , 11.20 , 11.60 , 12.00 , 12.40 , 12.80 , 13.20 , 13.60 , 
/* 01x */    14.00 , 14.40 , 14.80 , 15.20 , 15.60 , 16.00 , 16.40 , 16.80 , 17.20 , 17.60 , 
/* 02x */    18.00 , 18.40 , 18.80 , 19.20 , 19.60 , 20.00 , 20.40 , 20.80 , 21.20 , 21.60 , 
/* 03x */    22.00 , 22.40 , 22.80 , 23.20 , 23.60 , 24.00 , 24.40 , 24.80 , 25.20 , 25.60 , 
/* 04x */    26.00 , 26.40 , 26.80 , 27.20 , 27.60 , 28.00 , 28.40 , 28.80 , 29.20 , 29.60 , 
/* 05x */    30.00 , 30.40 , 30.80 , 31.20 , 31.60 , 32.00 , 32.40 , 32.80 , 33.20 , 33.60 , 
/* 06x */    34.00 , 34.40 , 34.80 , 35.20 , 35.60 , 36.00 , 36.40 , 36.80 , 37.20 , 37.60 , 
/* 07x */    38.00 , 38.40 , 38.80 , 39.20 , 39.60 , 40.00 , 40.40 , 40.80 , 41.20 , 41.60 , 
/* 08x */    42.00 , 42.40 , 42.80 , 43.20 , 43.60 , 44.00 , 44.40 , 44.80 , 45.20 , 45.60 , 
/* 09x */    46.00 , 46.40 , 46.80 , 47.20 , 47.60 , 48.00 , 48.40 , 48.80 , 49.20 , 49.60 , 
/* 10x */    50.00 
} ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  BST模块内部函数     ==") ;
CODE_SECTION("==========================") ;
/*******************************************************************************
- Function    : __IV18_BST_INIT_GPIOConfig
- Description : 本函数配置BOOST电路的引脚。BOOST电路使用2个引脚，一个为定时器输出，
                用于驱动BOOST电路。一个为反馈电压，需配置为模拟输入，用于测量BOOST
                电路输出电压。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __IV18_BST_INIT_GPIOConfig(VOID)
{
    GPIO_InitTypeDef stGPIOInitInfo ;
    /* 使能GPIO时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE) ;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE) ;
    
    /* BOOST电路相关配置 */
    /*
     * PA0 : VFB，用于检测BOOST电路电压值，需要配置为AD输入
     * PB1 : BOOST，用于驱动BOOST电路，需要配置为TIM3的CH4输出
     */
    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_AIN ;
    stGPIOInitInfo.GPIO_Pin   = GPIO_Pin_0 ;
    stGPIOInitInfo.GPIO_Speed = GPIO_Speed_50MHz ;

    GPIO_Init(GPIOA, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_AF_PP ;
    stGPIOInitInfo.GPIO_Pin   = GPIO_Pin_1 ;
    stGPIOInitInfo.GPIO_Speed = GPIO_Speed_50MHz ;

    GPIO_Init(GPIOB, &stGPIOInitInfo) ;

    return ;
}

/*******************************************************************************
- Function    : __IV18_BST_INIT_ADCConfig
- Description : 本函数配置ADC转换器，用于获取BOOST电路反馈电压。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __IV18_BST_INIT_ADCConfig(VOID)
{
    ADC_InitTypeDef stADCConfigInfo ;
    
    /* 使能ADC时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE) ;

    /* 分频配置 */
    RCC_ADCCLKConfig(RCC_PCLK2_Div2) ;

    /* ADC配置 */
    stADCConfigInfo.ADC_Mode               = ADC_Mode_Independent ;
    stADCConfigInfo.ADC_ScanConvMode       = DISABLE ;
    stADCConfigInfo.ADC_ContinuousConvMode = DISABLE ;
    stADCConfigInfo.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None ;
    stADCConfigInfo.ADC_DataAlign          = ADC_DataAlign_Right ;
    stADCConfigInfo.ADC_NbrOfChannel       = 1 ;

    ADC_Init(ADC1, &stADCConfigInfo) ;

    /* 使能ADC */
    ADC_Cmd(ADC1, ENABLE) ;

    /* 复位校准 */
    ADC_ResetCalibration(ADC1) ;
    while(ADC_GetResetCalibrationStatus(ADC1)) ;

    /* 转换校准 */
    ADC_StartCalibration(ADC1) ;
    while(ADC_GetCalibrationStatus(ADC1)) ;

    return ;
}

/*******************************************************************************
- Function    : __IV18_BST_INIT_TIM3Config
- Description : 本函数配置定时器3，及其比较通道4，产生PWM波驱动BOOST电路。
                PWM周期为100KHz。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __IV18_BST_INIT_TIM3Config(VOID)
{
    TIM_TimeBaseInitTypeDef stTIM3InitInfo ;
    TIM_OCInitTypeDef       stOCInitInfo ;
    
    /* 使能TIM3时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE) ;

    /* 配置TIM3 */
    /*
     * TIM3的输入时钟为72MHz，配置重装载为719，则PWM频率为100KHz
     */
    stTIM3InitInfo.TIM_ClockDivision = TIM_CKD_DIV1 ;
    stTIM3InitInfo.TIM_CounterMode   = TIM_CounterMode_Up ;
    stTIM3InitInfo.TIM_Period        = 719 ;
    stTIM3InitInfo.TIM_Prescaler     = 0 ;

    TIM_TimeBaseInit(TIM3, &stTIM3InitInfo) ;
    
    /* 使能ARR预装载 */
    TIM_ARRPreloadConfig(TIM3, ENABLE); 
    
    /* 配置TIM3-CH4为PWM */
    stOCInitInfo.TIM_OCMode      = TIM_OCMode_PWM1 ;
    stOCInitInfo.TIM_OutputState = TIM_OutputState_Enable ;
    stOCInitInfo.TIM_Pulse       = 0 ;
    stOCInitInfo.TIM_OCPolarity  = TIM_OCPolarity_High ;

    TIM_OC4Init(TIM3, &stOCInitInfo) ;

    /* 使能CCR预装载 */
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable) ;

    /* 使能定时器，使能通道4 */
    TIM_Cmd(TIM3, ENABLE) ;

    return ;
}

/*******************************************************************************
- Function    : __IV18_BST_Init
- Description : 本函数完成BST模块的初始化。函数依次初始化GPIO，ADC，TIM3。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __IV18_BST_Init(VOID)
{
    /* GPIO初始化 */
    __IV18_BST_INIT_GPIOConfig() ;

    /* AD转换初始化 */
    __IV18_BST_INIT_ADCConfig() ;

    /* 定时器初始化 */
    __IV18_BST_INIT_TIM3Config() ;
}

/*******************************************************************************
- Function    : __IV18_BST_GetVOut
- Description : 查询BOOST电路反馈电压，然后根据该反馈电压计算BOOST电路输出电压。
- Input       : NULL
- Output      : NULL
- Return      : BOOST电路输出电压。
- Others      : 
*******************************************************************************/
FLOAT __IV18_BST_GetVOut(VOID)
{
    USHORT usADCVal ;
    FLOAT  fVFeedBack ;
    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5) ;
    ADC_SoftwareStartConvCmd(ADC1, ENABLE) ;
    while(RESET == ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)) ;

    usADCVal = ADC_GetConversionValue(ADC1) ;
    /*
     * VFB = (Vo / (270 + 10)) * 10
     * Vo  = (VFB / 10) * (270 + 10)
     */
    fVFeedBack = (((FLOAT)(usADCVal & 0xfff)) / 4096.0) * 3.3 ;

    return ((fVFeedBack / 10.0) * 280) ;
}

/*******************************************************************************
- Function    : __IV18_BST_SetBrightness
- Description : 本函数用于设置IV18的亮度。
- Input       : pucBrightness : 指向需要的亮度值变量的指针。
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __IV18_BST_SetBrightness(UCHAR *pucBrightness)
{
    UCHAR  ucBrightnessLevel ;
    USHORT usCH4CCR ;
    FLOAT  fVolOut ;
    FLOAT  fVolDiff ;

    ucBrightnessLevel = *pucBrightness ;
    
    if(100 < ucBrightnessLevel)
    {
        ucBrightnessLevel = 100 ;
    }

    /* 获取当前CCR的值 */
    usCH4CCR = TIM_GetCapture4(TIM3) ;

    /* 获取电压值 */
    fVolOut = __IV18_BST_GetVOut() ;

    /* 设置亮度，通过亮度查询电压值 */
    if(fVolOut < g_fBSTBrightness2Vol[ucBrightnessLevel])
    {
        /* 如果反馈电压小于期望电压，则需要增大输出电压 */
        /* 计算差值，进行比例调节 */
        fVolDiff = g_fBSTBrightness2Vol[ucBrightnessLevel] - fVolOut ;

        if(fVolDiff > 5)
        {
            usCH4CCR = (usCH4CCR > 400)? 400 : (usCH4CCR + 5) ;
        }
        else if(fVolDiff > 4)
        {
            usCH4CCR = (usCH4CCR > 400)? 400 : (usCH4CCR + 4) ;
        }
        else if(fVolDiff > 3)
        {
            usCH4CCR = (usCH4CCR > 400)? 400 : (usCH4CCR + 3) ;
        }
        else if(fVolDiff > 2)
        {
            usCH4CCR = (usCH4CCR > 400)? 400 : (usCH4CCR + 2) ;
        }
        else
        {
            usCH4CCR = (usCH4CCR > 400)? 400 : (usCH4CCR + 1) ;
        } 
    }
    else
    {
        /* 如果反馈电压大于期望电压，则需要减小反馈电压 */
        /* 计算差值，进行比例调节 */
        fVolDiff = fVolOut - g_fBSTBrightness2Vol[ucBrightnessLevel]  ;

        if(fVolDiff > 5)
        {
            usCH4CCR = (usCH4CCR < 6)? 1 : (usCH4CCR - 5) ;
        }
        else if(fVolDiff > 4)
        {
            usCH4CCR = (usCH4CCR < 5)? 1 : (usCH4CCR - 4) ;
        }
        else if(fVolDiff > 3)
        {
            usCH4CCR = (usCH4CCR < 4)? 1 : (usCH4CCR - 3) ;
        }
        else if(fVolDiff > 2)
        {
            usCH4CCR = (usCH4CCR < 3)? 1 : (usCH4CCR - 2) ;
        }
        else
        {
            usCH4CCR = (usCH4CCR < 2)? 1 : (usCH4CCR - 1) ;
        } 
    }

    /* 设置PWM */
    TIM_SetCompare4(TIM3, usCH4CCR) ;

    return ;
}


CODE_SECTION("==========================") ;
CODE_SECTION("==  BST模块任务函数     ==") ;
CODE_SECTION("==========================") ;
VOID IV18_BST_Task(VOID)
{
    OS_ERR enErr ;
    
    /* 初始化模块 */
    __IV18_BST_Init() ;

    g_stBSTWA.ucBrightness = 50 ;

    while(DEF_TRUE)
    {
        /* 大小限制 */
        g_stBSTWA.ucBrightness = (g_stBSTWA.ucBrightness > 100)? 100 : g_stBSTWA.ucBrightness ;

        /* 调整电压，改变亮度 */
        __IV18_BST_SetBrightness(&g_stBSTWA.ucBrightness) ;

        /* 任务延时 */
        OSTimeDlyHMSM((CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT32U   )IV18_BST_TASK_DELAY,
                      (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                      (OS_ERR      *)&enErr);        
    }
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file iv18_bst.c. *******/  
