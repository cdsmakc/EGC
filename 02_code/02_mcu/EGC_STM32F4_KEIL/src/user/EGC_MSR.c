/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : EGC_MSR.c
    Project name : EGC
    Module name  : 
    Date created : 2020年02月12日    9时46分26秒
    Author       : Ning.JianLi
    Description  : Measure模块源文件。
*******************************************************************************/

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "pub_type.h"
#include "pub_lib.h"

CODE_SECTION("==========================") ;
CODE_SECTION("==  MSR模块包含文件     ==") ;
CODE_SECTION("==========================") ;

/* <第三方> */
#include "os.h"
#include "stm32f4xx.h"

/* <项目内部> */
#include "drv_i2c_stm32f4xx.h"
#include "drv_hdc1080.h"
#include "drv_msa300.h"
#include "EGC_Boards.h"
#include "EGC_MSR.h"

CODE_SECTION("==========================") ;
CODE_SECTION("==  MSR模块全局变量     ==") ;
CODE_SECTION("==========================") ;
/* 全局工作区 */
EGC_MSR_WORKAREA_S            g_stMSRWA ;

/* 外部变量 */
DRV_I2C_DEVICE_S              g_stI2CDevice ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  MSR模块内部函数     ==") ;
CODE_SECTION("==========================") ;
/*******************************************************************************
- Function    : __EGC_MSR_INIT_CONFIGGPIO_GetPinGroup
- Description : 本函数输入一个EGC_Boards.h文件中定义的引脚(EGC_BOARDS_GPIO_Pxx)，
                返回该引脚所在的GPIO组(GPIOx)。
- Input       : uiPin         ：EGC_BOARDS_GPIO_Pxx
- Output      : ppstGPIOGroup ：用于保存GPIOx的指针
- Return      : VOID
- Others      :
*******************************************************************************/
STATIC VOID __EGC_MSR_INIT_CONFIGGPIO_GetPinGroup(UINT uiPin, GPIO_TypeDef **ppstGPIOGroup)
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
STATIC VOID __EGC_MSR_INIT_CONFIGGPIO_GetPinIndex(UINT uiPin, UINT *puiIndex)
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
- Function    : __EGC_MSR_Init
- Description : 本函数为MSR模块的初始化函数。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __EGC_MSR_Init(VOID)
{
    OS_ERR               enErr ;
    DRV_I2C_INIT_INFO_S  stI2CInitInfo ;
    GPIO_TypeDef        *pstSCLGPIOGroup ;
    GPIO_TypeDef        *pstSDAGPIOGroup ;
    GPIO_TypeDef        *pstMSA300IntGPIOGroup ;
    UINT                 uiSCLGPIOIndex ;
    UINT                 uiSDAGPIOIndex ;
    UINT                 uiMSA300IntGPIOIndex ;
    volatile DRV_I2C_ERROR_CODE_E enErrCode ;
    
    /* 初始化I2C总线 */
    __EGC_MSR_INIT_CONFIGGPIO_GetPinGroup(EGC_BOARDS_I2C_GPIO_SCL, &pstSCLGPIOGroup) ;
    __EGC_MSR_INIT_CONFIGGPIO_GetPinGroup(EGC_BOARDS_I2C_GPIO_SDA, &pstSDAGPIOGroup) ;

    __EGC_MSR_INIT_CONFIGGPIO_GetPinIndex(EGC_BOARDS_I2C_GPIO_SCL, &uiSCLGPIOIndex) ;
    __EGC_MSR_INIT_CONFIGGPIO_GetPinIndex(EGC_BOARDS_I2C_GPIO_SDA, &uiSDAGPIOIndex) ;

    stI2CInitInfo.stGPIOInfo.pstSCLGPIOGroup = pstSCLGPIOGroup ;
    stI2CInitInfo.stGPIOInfo.pstSDAGPIOGroup = pstSDAGPIOGroup ;
    stI2CInitInfo.stGPIOInfo.uiSCLPinID      = uiSCLGPIOIndex ;
    stI2CInitInfo.stGPIOInfo.uiSDAPinID      = uiSDAGPIOIndex ;
    stI2CInitInfo.stGPIOInfo.ucSCLPinAFDef   = EGC_BOARDS_PERIPH_I2C_GPIO_AF_DEF ;
    stI2CInitInfo.stGPIOInfo.ucSDAPinAFDef   = EGC_BOARDS_PERIPH_I2C_GPIO_AF_DEF ;

    stI2CInitInfo.stI2CInfo.pstI2CPeriph     = EGC_BOARDS_PERIPH_I2C ;
    stI2CInitInfo.stI2CInfo.uiI2CClockSpeed  = EGC_BOARDS_PERIPH_I2C_SPEED ;
    stI2CInitInfo.stI2CInfo.usI2CDutyCycle   = I2C_DutyCycle_16_9 ;

    enErrCode = DRV_I2C_Open(&g_stI2CDevice, &stI2CInitInfo) ;

    /* 初始化MSA300 */
    __EGC_MSR_INIT_CONFIGGPIO_GetPinGroup(EGC_BOARDS_MSA300_GPIO_INT, &pstMSA300IntGPIOGroup) ;
    __EGC_MSR_INIT_CONFIGGPIO_GetPinIndex(EGC_BOARDS_MSA300_GPIO_INT, &uiMSA300IntGPIOIndex) ;

    DRV_MSA_Init(pstMSA300IntGPIOGroup, uiMSA300IntGPIOIndex, EGC_BOARDS_PERIPH_EXTI_MSA300_EXTI_LINE) ;

    /* 初始化HDC1080 */
    DRV_HDC_Init() ;

    /* HDC1080初始化需要一段时间 */
    OSTimeDlyHMSM((CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT16U   )5u, 
                  (CPU_INT32U   )0u,
                  (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                  (OS_ERR      *)&enErr);

    /* 变量初始化 */
    g_stMSRWA.uiTaskRun  = 0 ;
    g_stMSRWA.uiInitDone = 1 ;
    
    return ;
}

CODE_SECTION("==========================") ;
CODE_SECTION("==  MSR模块任务函数     ==") ;
CODE_SECTION("==========================") ;
/*******************************************************************************
- Function    : EGC_MSR_Task
- Description : 本函数为MSR模块的任务函数。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID EGC_MSR_Task(VOID)
{
    OS_ERR enErr ;
    USHORT usHDC1080DevID ;
    USHORT usHDC1080ManID ;
    
    /* 模块初始化 */
    __EGC_MSR_Init() ;

    /* 等待模块开始工作指令 */
    g_stMSRWA.uiTaskRun = 1 ;
    while(1 != g_stMSRWA.uiTaskRun)
    {
        OSTimeDlyHMSM((CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT32U   )10u,
                      (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                      (OS_ERR      *)&enErr);              
    }

    /* 任务主循环 */
    while(DEF_TRUE)
    {
        /* 温度和湿度测量 */ /* 先读取HDC1080的DEV  _ID和MAN_ID，正确的情况下再读取温度 */
        DRV_HDC_GetDeviceID(&usHDC1080DevID) ;
        DRV_HDC_GetManufacturerID(&usHDC1080ManID) ;

        if(DRV_HDC1080_DEV_ID == usHDC1080DevID && DRV_HDC1080_MAN_ID == usHDC1080ManID)
        {
            /* 获取测量结果 */
            DRV_HDC_Measure(&g_stMSRWA.fTemp, &g_stMSRWA.fHumi) ;
        }

        /* 触摸灵敏度设置 */
        switch(g_stMSRWA.enTouchSenseLevel)
        {
            case TOUCH_SENSE_LEVEL_VERY_HIGH :
                DRV_MSA_SensitivitySet(20) ;
                break ;
            case TOUCH_SENSE_LEVEL_HIGH :
                DRV_MSA_SensitivitySet(40) ;
                break ;
            case TOUCH_SENSE_LEVEL_MEDIUM :
                DRV_MSA_SensitivitySet(60) ;
                break ;
            case TOUCH_SENSE_LEVEL_LOW :
                DRV_MSA_SensitivitySet(80) ;
                break ;
            case TOUCH_SENSE_LEVEL_VERY_LOW :
                DRV_MSA_SensitivitySet(100) ;
                break ;
            default :
                DRV_MSA_SensitivitySet(20) ;
                break ;
        }

        /* 加速度测量 */
        DRV_MSA_ReadAcceleration(&g_stMSRWA.fXAxisAcc,
                                 &g_stMSRWA.fYAxisAcc,
                                 &g_stMSRWA.fZAxisAcc) ;

        /* 任务延时 */
        OSTimeDlyHMSM((CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT16U   )EGC_MSR_TASK_DELAY_S, 
                      (CPU_INT32U   )0u,
                      (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                      (OS_ERR      *)&enErr);     
    }
}

CODE_SECTION("==========================") ;
CODE_SECTION("==  MSR模块中断函数     ==") ;
CODE_SECTION("==========================") ;
/*******************************************************************************
- Function    : EGC_MSR_MSA300EXTIIntHandler
- Description : 本函数处理MSA300的外部中断。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID EGC_MSR_MSA300EXTIIntHandler(VOID)
{
    DRV_MSA_IRQHandler(EGC_BOARDS_PERIPH_EXTI_MSA300_EXTI_LINE) ;

    g_stMSRWA.uiTouchOccur = 1 ;
}

/*******************************************************************************
- Function    : EGC_MSR_I2CIntHandler
- Description : 本函数处理I2C中断。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID EGC_MSR_I2CIntHandler(VOID)
{
    DRV_I2C_GeneralIntHandler(&g_stI2CDevice) ;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file EGC_MSR.c. *******/  
