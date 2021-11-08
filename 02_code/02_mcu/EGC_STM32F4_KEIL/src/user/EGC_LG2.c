/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : EGC_LG2.c
    Project name : EGC
    Module name  : 
    Date created : 2021年09月06日    15时26分4秒
    Author       : Ning.JianLi
    Description  : LG2(LCD Group2)模块源文件。
*******************************************************************************/

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "pub_type.h"
#include "pub_lib.h"

CODE_SECTION("==========================") ;
CODE_SECTION("==  LG2模块包含文件     ==") ;
CODE_SECTION("==========================") ;
/* <标准库> */
#include <string.h>

/* <第三方> */
#include "os.h"

/* <项目内部> */
#include "drv_spi_stm32f4xx.h"
#include "EGC_Boards.h"
#include "EGC_Config.h"
#include "EGC_LG2.h"

CODE_SECTION("==========================") ;
CODE_SECTION("==  LG2模块全局变量     ==") ;
CODE_SECTION("==========================") ;
/* SPI设备 */
STATIC DRV_SPI_DEVICE_S g_stLG2SPIDevice ;

/* 工作区 */
EGC_LG2_WORKAREA_S      g_stLG2WA ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  LG2模块内部函数     ==") ;
CODE_SECTION("==========================") ;

/*******************************************************************************
- Function    : __EGC_LG2_INIT_CONFIGGPIO_GetPinGroup
- Description : 本函数输入一个EGC_Boards.h文件中定义的引脚(EGC_BOARDS_GPIO_Pxx)，
                返回该引脚所在的GPIO组(GPIOx)。
- Input       : uiPin         ：EGC_BOARDS_GPIO_Pxx
- Output      : ppstGPIOGroup ：用于保存GPIOx的指针
- Return      : VOID
- Others      :
*******************************************************************************/
STATIC VOID __EGC_LG2_INIT_CONFIGGPIO_GetPinGroup(UINT uiPin, GPIO_TypeDef **ppstGPIOGroup)
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
- Function    : __EGC_LG2_INIT_CONFIGGPIO_GetPinIndex
- Description : 本函数输入一个EGC_Boards.h文件中定义的引脚(EGC_BOARDS_GPIO_Pxx)，
                返回该引脚的GPIO序号(GPIO_Pin_x)。
- Input       : uiPin    ：EGC_BOARDS_GPIO_Pxx
- Output      : puiIndex ：用于保存GPIO_Pin_x的指针
- Return      : VOID
- Others      :
*******************************************************************************/
STATIC VOID __EGC_LG2_INIT_CONFIGGPIO_GetPinIndex(UINT uiPin, UINT *puiIndex)
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
- Function    : __EGC_LG2_INIT_CONFIGGPIO_SetPinGPIOClock
- Description : 本函数输入一个EGC_Boards.h文件中定义的引脚(EGC_BOARDS_GPIO_Pxx)，
                使能该GPIO所在GPIO组的时钟。
- Input       : uiPin    ：EGC_BOARDS_GPIO_Pxx
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG2_INIT_CONFIGGPIO_SetPinGPIOClock(UINT uiPin)
{
    UINT          uiAHBAPBPeriphClockCtrl = 0 ;
    GPIO_TypeDef *pstGPIOGroup ;

    __EGC_LG2_INIT_CONFIGGPIO_GetPinGroup(uiPin, &pstGPIOGroup) ;

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
- Function    : __EGC_LG2_INIT_ConfigGPIO
- Description : 本函数用于配置LG2模块使用到的MCU的GPIO。这些GPIO包括：
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
VOID __EGC_LG2_INIT_ConfigGPIO(VOID)
{
    GPIO_InitTypeDef  stGPIOInitInfo ;
    GPIO_TypeDef     *pstLG2DCNGPIOGroup ;
    GPIO_TypeDef     *pstLG2RESETNGPIOGroup ;
    GPIO_TypeDef     *pstLG2SPICS1NGPIOGroup ;
    GPIO_TypeDef     *pstLG2SPICS2NGPIOGroup ;
    GPIO_TypeDef     *pstLG2SPICS3NGPIOGroup ;
    GPIO_TypeDef     *pstLG2SPICS4NGPIOGroup ;
    UINT              uiLG2DCNGPIOIndex ;
    UINT              uiLG2RESETNGPIOIndex ;
    UINT              uiLG2SPICS1NGPIOIndex ;
    UINT              uiLG2SPICS2NGPIOIndex ;
    UINT              uiLG2SPICS3NGPIOIndex ;
    UINT              uiLG2SPICS4NGPIOIndex ;

    /* 获取引脚的GPIO Group */
    __EGC_LG2_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP2_GPIO_DCN,      &pstLG2DCNGPIOGroup) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP2_GPIO_RESETN,   &pstLG2RESETNGPIOGroup) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP2_GPIO_SPI_CS5N, &pstLG2SPICS1NGPIOGroup) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP2_GPIO_SPI_CS6N, &pstLG2SPICS2NGPIOGroup) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP2_GPIO_SPI_CS7N, &pstLG2SPICS3NGPIOGroup) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP2_GPIO_SPI_CS8N, &pstLG2SPICS4NGPIOGroup) ;

    /* 获取引脚的GPIO   Index */
    __EGC_LG2_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP2_GPIO_DCN,      &uiLG2DCNGPIOIndex) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP2_GPIO_RESETN,   &uiLG2RESETNGPIOIndex) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP2_GPIO_SPI_CS5N, &uiLG2SPICS1NGPIOIndex) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP2_GPIO_SPI_CS6N, &uiLG2SPICS2NGPIOIndex) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP2_GPIO_SPI_CS7N, &uiLG2SPICS3NGPIOIndex) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP2_GPIO_SPI_CS8N, &uiLG2SPICS4NGPIOIndex) ;

    /* 使能GPIO时钟 */
    __EGC_LG2_INIT_CONFIGGPIO_SetPinGPIOClock(EGC_LCD_GROUP2_GPIO_DCN) ;
    __EGC_LG2_INIT_CONFIGGPIO_SetPinGPIOClock(EGC_LCD_GROUP2_GPIO_RESETN) ;
    __EGC_LG2_INIT_CONFIGGPIO_SetPinGPIOClock(EGC_LCD_GROUP2_GPIO_SPI_CS5N) ;
    __EGC_LG2_INIT_CONFIGGPIO_SetPinGPIOClock(EGC_LCD_GROUP2_GPIO_SPI_CS6N) ;
    __EGC_LG2_INIT_CONFIGGPIO_SetPinGPIOClock(EGC_LCD_GROUP2_GPIO_SPI_CS7N) ;
    __EGC_LG2_INIT_CONFIGGPIO_SetPinGPIOClock(EGC_LCD_GROUP2_GPIO_SPI_CS8N) ;

    /* 引脚信息配置 */
    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_OUT ;
    stGPIOInitInfo.GPIO_Speed = GPIO_Speed_50MHz ;
    stGPIOInitInfo.GPIO_OType = GPIO_OType_PP ;
    stGPIOInitInfo.GPIO_PuPd  = GPIO_PuPd_NOPULL ;

    stGPIOInitInfo.GPIO_Pin = uiLG2DCNGPIOIndex ;
    GPIO_Init(pstLG2DCNGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin = uiLG2RESETNGPIOIndex ;
    GPIO_Init(pstLG2RESETNGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin = uiLG2SPICS1NGPIOIndex ;
    GPIO_Init(pstLG2SPICS1NGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin = uiLG2SPICS2NGPIOIndex ;
    GPIO_Init(pstLG2SPICS2NGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin = uiLG2SPICS3NGPIOIndex ;
    GPIO_Init(pstLG2SPICS3NGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin = uiLG2SPICS4NGPIOIndex ;
    GPIO_Init(pstLG2SPICS4NGPIOGroup, &stGPIOInitInfo) ;

    GPIO_SetBits(pstLG2SPICS1NGPIOGroup, uiLG2SPICS1NGPIOIndex) ;
    GPIO_SetBits(pstLG2SPICS2NGPIOGroup, uiLG2SPICS2NGPIOIndex) ;
    GPIO_SetBits(pstLG2SPICS3NGPIOGroup, uiLG2SPICS3NGPIOIndex) ;
    GPIO_SetBits(pstLG2SPICS4NGPIOGroup, uiLG2SPICS4NGPIOIndex) ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_LG2_INIT_ConfigLCD
- Description : 本函数初始化LCD。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG2_SetLCDScreenReset(BOOL_T bReset) ;
VOID __EGC_LG2_WriteLCDScreenSetInfo(EGC_LG2_LCD_DEF_E enLCD, UCHAR ucCmd, UCHAR *pucData, UINT uiSize) ;

VOID __EGC_LG2_INIT_ConfigLCD(VOID)
{
    OS_ERR enErr ;
    UCHAR  aucConfigBuf[32] ;

    /* 复位LCD */
    __EGC_LG2_SetLCDScreenReset(BOOL_TRUE) ;

    /* 等待一段时间 */
    OSTimeDlyHMSM((CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT32U   )200u,
                  (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                  (OS_ERR      *)&enErr);

    /* 撤销复位 */
    __EGC_LG2_SetLCDScreenReset(BOOL_FALSE) ;

    /* 等待一段时间 */
    OSTimeDlyHMSM((CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT32U   )200u,
                  (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                  (OS_ERR      *)&enErr);

    /* sleep out */
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB1_SLPOUT, NULL, 0) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB1_SLPOUT, NULL, 0) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB1_SLPOUT, NULL, 0) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB1_SLPOUT, NULL, 0) ;

    /* 等待一段时间 */
    OSTimeDlyHMSM((CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT32U   )200u,
                  (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                  (OS_ERR      *)&enErr);

    /* MADCTL */
    aucConfigBuf[0] = 0x00 ;

    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB1_MADCTL, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB1_MADCTL, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB1_MADCTL, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB1_MADCTL, aucConfigBuf, 1) ;
    
    /* COLMOD */
    aucConfigBuf[0] = 0x05 ;

    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB1_COLMOD, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB1_COLMOD, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB1_COLMOD, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB1_COLMOD, aucConfigBuf, 1) ;

    /* INVON */
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB1_INVON, NULL, 0) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB1_INVON, NULL, 0) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB1_INVON, NULL, 0) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB1_INVON, NULL, 0) ;

    /* PORCTL */
    aucConfigBuf[0] = 0x05 ;
    aucConfigBuf[1] = 0x05 ;
    aucConfigBuf[2] = 0x00 ;
    aucConfigBuf[3] = 0x33 ;
    aucConfigBuf[4] = 0x33 ;

    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB2_PORCTRL, aucConfigBuf, 5) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB2_PORCTRL, aucConfigBuf, 5) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB2_PORCTRL, aucConfigBuf, 5) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB2_PORCTRL, aucConfigBuf, 5) ;

    /* GCTRL */
    aucConfigBuf[0] = 0x23 ;

    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB2_GCTRL, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB2_GCTRL, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB2_GCTRL, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB2_GCTRL, aucConfigBuf, 1) ;

    /* VCOMS */
    aucConfigBuf[0] = 0x22 ;

    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB2_VCOMS, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB2_VCOMS, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB2_VCOMS, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB2_VCOMS, aucConfigBuf, 1) ;

    /* LCMCTRL */
    aucConfigBuf[0] = 0x2C ;

    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB2_LCMCTRL, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB2_LCMCTRL, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB2_LCMCTRL, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB2_LCMCTRL, aucConfigBuf, 1) ;

    /* VDVVRHEN */
    aucConfigBuf[0] = 0x01 ;

    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB2_VDVVRHEN, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB2_VDVVRHEN, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB2_VDVVRHEN, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB2_VDVVRHEN, aucConfigBuf, 1) ;

    /* VRHS */
    aucConfigBuf[0] = 0x13 ;

    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB2_VRHS, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB2_VRHS, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB2_VRHS, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB2_VRHS, aucConfigBuf, 1) ;

    /* VDVS */
    aucConfigBuf[0] = 0x20 ;

    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB2_VDVS, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB2_VDVS, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB2_VDVS, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB2_VDVS, aucConfigBuf, 1) ;

    /* FRCTRL2 */
    aucConfigBuf[0] = 0x0F ;

    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB2_FRCTRL2, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB2_FRCTRL2, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB2_FRCTRL2, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB2_FRCTRL2, aucConfigBuf, 1) ;
        
    /* PWCTRL1 */
    aucConfigBuf[0] = 0xA4 ;
    aucConfigBuf[1] = 0xA1 ;

    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB2_PWCTRL1, aucConfigBuf, 2) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB2_PWCTRL1, aucConfigBuf, 2) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB2_PWCTRL1, aucConfigBuf, 2) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB2_PWCTRL1, aucConfigBuf, 2) ;

    /* ????? */
    aucConfigBuf[0] = 0xA1 ;

    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, 0xD6, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, 0xD6, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, 0xD6, aucConfigBuf, 1) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, 0xD6, aucConfigBuf, 1) ;

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

    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB2_PVGAMCTRL, aucConfigBuf, 14) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB2_PVGAMCTRL, aucConfigBuf, 14) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB2_PVGAMCTRL, aucConfigBuf, 14) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB2_PVGAMCTRL, aucConfigBuf, 14) ;

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

    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB2_NVGAMCTRL, aucConfigBuf, 14) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB2_NVGAMCTRL, aucConfigBuf, 14) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB2_NVGAMCTRL, aucConfigBuf, 14) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB2_NVGAMCTRL, aucConfigBuf, 14) ;

    /* DISPON */
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB1_DISPON, NULL, 0) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB1_DISPON, NULL, 0) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB1_DISPON, NULL, 0) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB1_DISPON, NULL, 0) ;

    /* CASET */
    aucConfigBuf[0]  = 0x00 ;
    aucConfigBuf[1]  = 0x34 ;
    aucConfigBuf[2]  = 0x00 ;
    aucConfigBuf[3]  = 0xBA ;

    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB1_CASET, aucConfigBuf, 4) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB1_CASET, aucConfigBuf, 4) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB1_CASET, aucConfigBuf, 4) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB1_CASET, aucConfigBuf, 4) ;

    /* RASET */
    aucConfigBuf[0]  = 0x00 ;
    aucConfigBuf[1]  = 0x28 ;
    aucConfigBuf[2]  = 0x01 ;
    aucConfigBuf[3]  = 0x17 ;

    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD5, EGC_LG2_ST7789V_CMD_TB1_RASET, aucConfigBuf, 4) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD6, EGC_LG2_ST7789V_CMD_TB1_RASET, aucConfigBuf, 4) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD7, EGC_LG2_ST7789V_CMD_TB1_RASET, aucConfigBuf, 4) ;
    __EGC_LG2_WriteLCDScreenSetInfo(LG2_LCD8, EGC_LG2_ST7789V_CMD_TB1_RASET, aucConfigBuf, 4) ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_LG2_Init
- Description : 本函数对LG2模块需要初始化的部分进行初始化。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG2_Init(VOID)
{
    DRV_SPI_INIT_INFO_S stSPIInitInfo ;
    
    /* 复位错误码 */
    g_stLG2WA.uiErrCode        = 0 ;

    /* 初始化GPIO */
    __EGC_LG2_INIT_ConfigGPIO() ;

    /* 创建SPI设备 */
    __EGC_LG2_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP2_GPIO_SPI_NSS_NC,  &stSPIInitInfo.pstNSSGPIOGroup) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP2_GPIO_SPI_SCK,     &stSPIInitInfo.pstSCKGPIOGroup) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP2_GPIO_SPI_MOSI,    &stSPIInitInfo.pstMOSIGPIOGroup) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP2_GPIO_SPI_MISO_NC, &stSPIInitInfo.pstMISOGPIOGroup) ;

    __EGC_LG2_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP2_GPIO_SPI_NSS_NC,  &stSPIInitInfo.uiNSSPinID) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP2_GPIO_SPI_SCK,     &stSPIInitInfo.uiSCKPinID) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP2_GPIO_SPI_MOSI,    &stSPIInitInfo.uiMOSIPinID) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP2_GPIO_SPI_MISO_NC, &stSPIInitInfo.uiMISOPinID) ;

    stSPIInitInfo.ucSCKPinAFDef     = EGC_BOARDS_PERIPH_SPI_LCD_GROUP2_AF_DEF ;
    stSPIInitInfo.ucMOSIPinAFDef    = EGC_BOARDS_PERIPH_SPI_LCD_GROUP2_AF_DEF ;
    stSPIInitInfo.ucMISOPinAFDef    = EGC_BOARDS_PERIPH_SPI_LCD_GROUP2_AF_DEF ;

    stSPIInitInfo.pstSPIPeriph      = EGC_BOARDS_PERIPH_SPI_LCD_GROUP2 ;
    stSPIInitInfo.usSPIPrescaler    = SPI_BaudRatePrescaler_2 ;
    stSPIInitInfo.ucSPIMode         = DRV_SPI_MODE_0 ;
    stSPIInitInfo.uiSPIIrqNumber    = EGC_BOARDS_PERIPH_SPI_LCD_GROUP2_INTN ;
    stSPIInitInfo.bUseDMA           = BOOL_TRUE ;

    stSPIInitInfo.pstSPIDMATxStream = EGC_BOARDS_PERIPH_SPI_LCD_GROUP2_DMA_TX_STREAM ;
    stSPIInitInfo.pstSPIDMARxStream = EGC_BOARDS_PERIPH_SPI_LCD_GROUP2_DMA_RX_STREAM ;
    stSPIInitInfo.uiSPIDMATxChannel = EGC_BOARDS_PERIPH_SPI_LCD_GROUP2_DMA_TX_CHANNEL ;
    stSPIInitInfo.uiSPIDMARxChannel = EGC_BOARDS_PERIPH_SPI_LCD_GROUP2_DMA_RX_CHANNEL ;
    stSPIInitInfo.uiSPIDMATxIrqNum  = EGC_BOARDS_PERIPH_SPI_LCD_GROUP2_DMA_TX_IRQ_NUM ;
    stSPIInitInfo.uiSPIDMARxIrqNum  = EGC_BOARDS_PERIPH_SPI_LCD_GROUP2_DMA_RX_IRQ_NUM ;

    DRV_SPI_Open(&g_stLG2SPIDevice, &stSPIInitInfo) ;

    /* 初始化LCD */
    __EGC_LG2_INIT_ConfigLCD() ;

    /* 部分变量初始化 */
    g_stLG2WA.uiLCD5UpdateView   = 1 ;
    g_stLG2WA.uiLCD6UpdateView   = 1 ;
    g_stLG2WA.uiLCD7UpdateView   = 1 ;
    g_stLG2WA.uiLCD8UpdateView   = 1 ;
    g_stLG2WA.uiLCD5UseSecondBuf = 0 ;
    g_stLG2WA.uiLCD6UseSecondBuf = 0 ;
    g_stLG2WA.uiLCD7UseSecondBuf = 0 ;
    g_stLG2WA.uiLCD8UseSecondBuf = 0 ;
    g_stLG2WA.uiTaskRun          = 0 ;

    /* 初始化完成 */
    g_stLG2WA.uiInitDone = 1 ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_LG2_SetLCDScreenCSN
- Description : 本函数对LG2模块控制的LCD的SPI CS信号进行控制。由于本模块的1个SPI
                控制4个LCD，因此需要4个CS信号。SPI驱动中的CS信号不使用。
- Input       : enCSSel ：
                    决定控制哪个LCD的CS信号。
                enCSCmd ：
                    决定信号的状态。
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG2_SetLCDScreenCSN(EGC_LG2_LCD_DEF_E enCSSel, EGC_LG2_SPI_CS_CMD_E enCSCmd)
{
    UINT          uiPin ;
    GPIO_TypeDef *pstGPIOGroup ;
    UINT          uiGPIOIndex ;

    /* 确定是哪个LCD的CS引脚 */
    switch(enCSSel)
    {
        case LG2_LCD5 :
            uiPin = EGC_LCD_GROUP2_GPIO_SPI_CS5N ;
            break ;
        case LG2_LCD6 :
            uiPin = EGC_LCD_GROUP2_GPIO_SPI_CS6N ;
            break ;
        case LG2_LCD7 :
            uiPin = EGC_LCD_GROUP2_GPIO_SPI_CS7N ;
            break ;
        case LG2_LCD8 :
        default :
            uiPin = EGC_LCD_GROUP2_GPIO_SPI_CS8N ;
            break ;
    }

    /* 确定GPIO组和ID */
    __EGC_LG2_INIT_CONFIGGPIO_GetPinGroup(uiPin, &pstGPIOGroup) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinIndex(uiPin, &uiGPIOIndex) ;

    /* 设置信号 */
    if(LG2_SPI_CS_CMD_START == enCSCmd)
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
- Function    : __EGC_LG2_SetLCDScreenDCxData
- Description : 本函数对LG2模块控制的LCD的D/C#信号进行控制。硬件上，这一个信号
                控制所有的4个LCD。本函数控制该信号为数据模式。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG2_SetLCDScreenDCxData(VOID)
{
    GPIO_TypeDef *pstGPIOGroup ;
    UINT          uiGPIOIndex ;

    /* 确定GPIO组和ID */
    __EGC_LG2_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP2_GPIO_DCN, &pstGPIOGroup) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP2_GPIO_DCN, &uiGPIOIndex) ;

    /* 设置引脚为高电平 */
    GPIO_SetBits(pstGPIOGroup, uiGPIOIndex) ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_LG2_SetLCDScreenDCxCommand
- Description : 本函数对LG2模块控制的LCD的D/C#信号进行控制。硬件上，这一个信号
                控制所有的4个LCD。本函数控制该信号为命令模式。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG2_SetLCDScreenDCxCommand(VOID)
{
    GPIO_TypeDef *pstGPIOGroup ;
    UINT          uiGPIOIndex ;

    /* 确定GPIO组和ID */
    __EGC_LG2_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP2_GPIO_DCN, &pstGPIOGroup) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP2_GPIO_DCN, &uiGPIOIndex) ;

    /* 设置引脚为低电平 */
    GPIO_ResetBits(pstGPIOGroup, uiGPIOIndex) ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_LG2_SetLCDScreenReset
- Description : 本函数对LG2模块控制的LCD的RESET#信号进行控制。硬件上，这一个信号
                控制所有的4个LCD。本函数实现LCD的复位或者复位撤销。
- Input       : bReset ：
                    BOOL_TRUE  : 激活复位；
                    BOOL_FALSE : 撤销复位。
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_LG2_SetLCDScreenReset(BOOL_T bReset)
{
    GPIO_TypeDef *pstGPIOGroup ;
    UINT          uiGPIOIndex ;

    /* 确定GPIO组和ID */
    __EGC_LG2_INIT_CONFIGGPIO_GetPinGroup(EGC_LCD_GROUP2_GPIO_RESETN, &pstGPIOGroup) ;
    __EGC_LG2_INIT_CONFIGGPIO_GetPinIndex(EGC_LCD_GROUP2_GPIO_RESETN, &uiGPIOIndex) ;

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
- Function    : __EGC_LG2_WriteLCDScreenSetInfo
- Description : 本函数对LG2模块控制的LCD的配置信息进行设置。
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
VOID __EGC_LG2_WriteLCDScreenSetInfo(EGC_LG2_LCD_DEF_E enLCD, UCHAR ucCmd, UCHAR *pucData, UINT uiSize)
{
    /* 设置CS信号 */
    __EGC_LG2_SetLCDScreenCSN(enLCD, LG2_SPI_CS_CMD_START) ;

    /* 设置D/C#信号 */
    __EGC_LG2_SetLCDScreenDCxCommand() ;

    /* 发送命令 */
    if(0 == uiSize)
    {
        DRV_SPI_Write((DRV_SPI_DEVICE_S *)&g_stLG2SPIDevice, 
                      (UCHAR            *)&ucCmd, 
                      (UCHAR            *)NULL, 
                      (UINT              )sizeof(ucCmd), 
                      (UINT              )(DRV_SPI_READ_WRITE_MODE_INT | DRV_SPI_READ_WRITE_WITH_START | DRV_SPI_READ_WRITE_WITH_STOP)) ;
    }
    else
    {
        DRV_SPI_Write((DRV_SPI_DEVICE_S *)&g_stLG2SPIDevice, 
                      (UCHAR            *)&ucCmd, 
                      (UCHAR            *)NULL, 
                      (UINT              )sizeof(ucCmd), 
                      (UINT              )(DRV_SPI_READ_WRITE_MODE_INT | DRV_SPI_READ_WRITE_WITH_START)) ;
    }

    /* 设置D/C#信号 */
    __EGC_LG2_SetLCDScreenDCxData() ;
    
    /* 如有数据，发送数据 */
    if(0 != uiSize)
    {
        DRV_SPI_Write((DRV_SPI_DEVICE_S *)&g_stLG2SPIDevice, 
                      (UCHAR            *)pucData, 
                      (UCHAR            *)NULL, 
                      (UINT              )uiSize, 
                      (UINT              )(DRV_SPI_READ_WRITE_MODE_INT | DRV_SPI_READ_WRITE_WITH_STOP)) ;
    }

    /* 设置CS信号 */
    __EGC_LG2_SetLCDScreenCSN(enLCD, LG2_SPI_CS_CMD_STOP) ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_LG2_WriteLCDScreenDatInfo
- Description : 本函数对LG2模块控制的LCD的显示数据进行传输。
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
VOID __EGC_LG2_WriteLCDScreenDatInfo(EGC_LG2_LCD_DEF_E enLCD, UCHAR ucCmd, UCHAR *pucData, UINT uiSize)
{
    /* 设置CS信号 */
    __EGC_LG2_SetLCDScreenCSN(enLCD, LG2_SPI_CS_CMD_START) ;

    /* 设置D/C#信号 */
    __EGC_LG2_SetLCDScreenDCxCommand() ;

    /* 发送命令 */
    if(0 == uiSize)
    {
        DRV_SPI_Write((DRV_SPI_DEVICE_S *)&g_stLG2SPIDevice, 
                      (UCHAR            *)&ucCmd, 
                      (UCHAR            *)NULL, 
                      (UINT              )sizeof(ucCmd), 
                      (UINT              )(DRV_SPI_READ_WRITE_MODE_INT | DRV_SPI_READ_WRITE_WITH_START | DRV_SPI_READ_WRITE_WITH_STOP)) ;
    }
    else
    {
        DRV_SPI_Write((DRV_SPI_DEVICE_S *)&g_stLG2SPIDevice, 
                      (UCHAR            *)&ucCmd, 
                      (UCHAR            *)NULL, 
                      (UINT              )sizeof(ucCmd), 
                      (UINT              )(DRV_SPI_READ_WRITE_MODE_INT | DRV_SPI_READ_WRITE_WITH_START)) ;
    }

    /* 设置D/C#信号 */
    __EGC_LG2_SetLCDScreenDCxData() ;
    
    /* 如有数据，发送数据 */
    if(0 != uiSize)
    {
        DRV_SPI_Write((DRV_SPI_DEVICE_S *)&g_stLG2SPIDevice, 
                      (UCHAR            *)pucData, 
                      (UCHAR            *)NULL, 
                      (UINT              )uiSize, 
                      (UINT              )(DRV_SPI_READ_WRITE_MODE_DMA | DRV_SPI_READ_WRITE_WITH_STOP)) ;
    }

    /* 设置CS信号 */
    __EGC_LG2_SetLCDScreenCSN(enLCD, LG2_SPI_CS_CMD_STOP) ;

    return ;
}

CODE_SECTION("==========================") ;
CODE_SECTION("==  LG2模块任务函数     ==") ;
CODE_SECTION("==========================") ;

/*******************************************************************************
- Function    : EGC_LG2_Task
- Description : 本函数为LG2模块的任务函数。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID EGC_LG2_Task(VOID)
{
    OS_ERR enErr ;

    /* 模块初始化 */
    __EGC_LG2_Init() ;

    /* 等待任务启动指令 */
    while(0 == g_stLG2WA.uiTaskRun)
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
            g_stLG2WA.uiErrCode = 0 ;
        }
    }

    while(DEF_TRUE)
    {
        /* 刷新LCD5显示 */
        if(1 == g_stLG2WA.uiLCD5UpdateView)
        {
            g_stLG2WA.uiLCD5UpdateView = 0 ;

            if(1 == g_stLG2WA.uiLCD5UseSecondBuf)
            {
                __EGC_LG2_WriteLCDScreenDatInfo(LG2_LCD5, 
                                                EGC_LG2_ST7789V_CMD_TB1_RAMWR, 
                                                (UCHAR *)EGC_CONFIG_SDRAM_ADDR_LCD5_BUF2, 
                                                EGC_LG2_LCD_FRAME_SIZE) ;
            }
            else
            {
                __EGC_LG2_WriteLCDScreenDatInfo(LG2_LCD5, 
                                                EGC_LG2_ST7789V_CMD_TB1_RAMWR, 
                                                (UCHAR *)EGC_CONFIG_SDRAM_ADDR_LCD5_BUF1, 
                                                EGC_LG2_LCD_FRAME_SIZE) ;
            }
        }

        /* 刷新LCD6显示 */
        if(1 == g_stLG2WA.uiLCD6UpdateView)
        {
            g_stLG2WA.uiLCD6UpdateView = 0 ;

            if(1 == g_stLG2WA.uiLCD6UseSecondBuf)
            {
                __EGC_LG2_WriteLCDScreenDatInfo(LG2_LCD6, 
                                                EGC_LG2_ST7789V_CMD_TB1_RAMWR, 
                                                (UCHAR *)EGC_CONFIG_SDRAM_ADDR_LCD6_BUF2, 
                                                EGC_LG2_LCD_FRAME_SIZE) ;
            }
            else
            {
                __EGC_LG2_WriteLCDScreenDatInfo(LG2_LCD6, 
                                                EGC_LG2_ST7789V_CMD_TB1_RAMWR, 
                                                (UCHAR *)EGC_CONFIG_SDRAM_ADDR_LCD6_BUF1, 
                                                EGC_LG2_LCD_FRAME_SIZE) ;
            }
        }

        /* 刷新LCD7显示 */
        if(1 == g_stLG2WA.uiLCD7UpdateView)
        {
            g_stLG2WA.uiLCD7UpdateView = 0 ;

            if(1 == g_stLG2WA.uiLCD7UseSecondBuf)
            {
                __EGC_LG2_WriteLCDScreenDatInfo(LG2_LCD7, 
                                                EGC_LG2_ST7789V_CMD_TB1_RAMWR, 
                                                (UCHAR *)EGC_CONFIG_SDRAM_ADDR_LCD7_BUF2, 
                                                EGC_LG2_LCD_FRAME_SIZE) ;
            }
            else
            {
                __EGC_LG2_WriteLCDScreenDatInfo(LG2_LCD7, 
                                                EGC_LG2_ST7789V_CMD_TB1_RAMWR, 
                                                (UCHAR *)EGC_CONFIG_SDRAM_ADDR_LCD7_BUF1, 
                                                EGC_LG2_LCD_FRAME_SIZE) ;
            }
        }

        /* 刷新LCD4显示 */
        if(1 == g_stLG2WA.uiLCD8UpdateView)
        {
            g_stLG2WA.uiLCD8UpdateView = 0 ;

            if(1 == g_stLG2WA.uiLCD8UseSecondBuf)
            {
                __EGC_LG2_WriteLCDScreenDatInfo(LG2_LCD8, 
                                                EGC_LG2_ST7789V_CMD_TB1_RAMWR, 
                                                (UCHAR *)EGC_CONFIG_SDRAM_ADDR_LCD8_BUF2, 
                                                EGC_LG2_LCD_FRAME_SIZE) ;
            }
            else
            {
                __EGC_LG2_WriteLCDScreenDatInfo(LG2_LCD8, 
                                                EGC_LG2_ST7789V_CMD_TB1_RAMWR, 
                                                (UCHAR *)EGC_CONFIG_SDRAM_ADDR_LCD8_BUF1, 
                                                EGC_LG2_LCD_FRAME_SIZE) ;
            }
        }
    
        /* 周期性任务 */
        OSTimeDlyHMSM((CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT32U   )EGC_LG2_TASK_CYCLE_PERIOD_MS,
                      (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                      (OS_ERR      *)&enErr);

        if(OS_ERR_NONE != enErr)
        {
            /* 延时函数错误 */
            g_stLG2WA.uiErrCode = 0 ;
        }
    }
}


/*******************************************************************************
- Function    : EGC_LG2_SPIIntHandler
- Description : 本函数为LG2模块所使用的SPI外设的中断服务函数。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID EGC_LG2_SPIIntHandler(VOID)
{
    DRV_SPI_GeneralIntHandler(&g_stLG2SPIDevice) ;
    
    return ;
}


/*******************************************************************************
- Function    : EGC_LG2_SPIDMAIntHandler
- Description : 本函数为LG2模块所使用的SPI外设的DMA中断服务函数。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID EGC_LG2_SPIDMAIntHandler(VOID)
{
    DRV_SPI_GeneralDMAIntHandler(&g_stLG2SPIDevice) ;

    return ;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file EGC_LG2.c. *******/  

