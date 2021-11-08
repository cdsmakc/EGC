/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : EGC_ESP.c
    Project name : EGC
    Module name  : 
    Date created : 2021年08月18日    15时26分4秒
    Author       : Ning.JianLi
    Description  : ESP模块源文件。
*******************************************************************************/

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "pub_type.h"
#include "pub_lib.h"

CODE_SECTION("==========================") ;
CODE_SECTION("==  ESP模块包含文件     ==") ;
CODE_SECTION("==========================") ;
/* <标准库> */
#include <string.h>
#include <time.h>

/* <第三方> */
#include "os.h"
#include "stm32f4xx.h"

/* <项目内部> */
#include "drv_uart_stm32f4xx.h"
#include "EGC_Boards.h"
#include "EGC_ESP.h"

#define LOG2HOST

CODE_SECTION("==========================") ;
CODE_SECTION("==  ESP模块全局变量     ==") ;
CODE_SECTION("==========================") ;
/* UART设备 */
DRV_UART_DEVICE_S             g_stESPUARTDevice ;

#ifdef LOG2HOST
extern DRV_UART_DEVICE_S      g_stHSTUARTDevice ;
#endif

/* ESP模块工作区 */
EGC_ESP_WORKAREA_S            g_stESPWA ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  ESP模块内部函数     ==") ;
CODE_SECTION("==========================") ;

VOID __strreplace(CHAR *pcString, CHAR cMatchChar, CHAR cReplaceChar)
{
    UINT uiStrLen ;
    
    if(NULL == pcString || NULL == cMatchChar || NULL == cReplaceChar)
    {
        return ;
    }

    uiStrLen = strlen(pcString) ;

    while(0 != uiStrLen)
    {
        if(cMatchChar == pcString[uiStrLen - 1])
        {
            pcString[uiStrLen - 1] = cReplaceChar ;
        }

        uiStrLen-- ;
    }

    return ;
}

/*******************************************************************************
- Function    : __EGC_ESP_INIT_CONFIGGPIO_GetPinGroup
- Description : 本函数输入一个EGC_Boards.h文件中定义的引脚(EGC_BOARDS_GPIO_Pxx)，
                返回该引脚所在的GPIO组(GPIOx)。
- Input       : uiPin         ：EGC_BOARDS_GPIO_Pxx
- Output      : ppstGPIOGroup ：用于保存GPIOx的指针
- Return      : VOID
- Others      :
*******************************************************************************/
STATIC VOID __EGC_ESP_INIT_CONFIGGPIO_GetPinGroup(UINT uiPin, GPIO_TypeDef **ppstGPIOGroup)
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
- Function    : __EGC_ESP_INIT_CONFIGGPIO_GetPinIndex
- Description : 本函数输入一个EGC_Boards.h文件中定义的引脚(EGC_BOARDS_GPIO_Pxx)，
                返回该引脚的GPIO序号(GPIO_Pin_x)。
- Input       : uiPin    ：EGC_BOARDS_GPIO_Pxx
- Output      : puiIndex ：用于保存GPIO_Pin_x的指针
- Return      : VOID
- Others      :
*******************************************************************************/
STATIC VOID __EGC_ESP_INIT_CONFIGGPIO_GetPinIndex(UINT uiPin, UINT *puiIndex)
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
- Function    : __EGC_ESP_INIT_CONFIGGPIO_SetPinGPIOClock
- Description : 本函数输入一个EGC_Boards.h文件中定义的引脚(EGC_BOARDS_GPIO_Pxx)，
                使能该GPIO所在GPIO组的时钟。
- Input       : uiPin    ：EGC_BOARDS_GPIO_Pxx
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_ESP_INIT_CONFIGGPIO_SetPinGPIOClock(UINT uiPin)
{
    UINT          uiAHBAPBPeriphClockCtrl = 0 ;
    GPIO_TypeDef *pstGPIOGroup ;

    __EGC_ESP_INIT_CONFIGGPIO_GetPinGroup(uiPin, &pstGPIOGroup) ;

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
- Function    : __EGC_ESP_INIT_ConfigGPIO
- Description : 本函数配置MCU与ESP32连接的复位信号的引脚。UART的引脚将在驱动中
                配置。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __EGC_ESP_INIT_ConfigGPIO(VOID)
{
    GPIO_InitTypeDef  stGPIOInitInfo ;
    GPIO_TypeDef     *pstESP32RSTGPIOGroup ;
    UINT              uiESP32RSTGPIOIndex ;

    __EGC_ESP_INIT_CONFIGGPIO_GetPinGroup(EGC_BOARDS_ESP32_GPIO_RST, &pstESP32RSTGPIOGroup) ;
    __EGC_ESP_INIT_CONFIGGPIO_GetPinIndex(EGC_BOARDS_ESP32_GPIO_RST, &uiESP32RSTGPIOIndex) ;

    /* 使能GPIO组时钟 */
    __EGC_ESP_INIT_CONFIGGPIO_SetPinGPIOClock(EGC_BOARDS_ESP32_GPIO_RST) ;

    /* RST引脚配置 */
    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_OUT ;
    stGPIOInitInfo.GPIO_Speed = GPIO_Medium_Speed ;
    stGPIOInitInfo.GPIO_OType = GPIO_OType_PP ;
    stGPIOInitInfo.GPIO_PuPd  = GPIO_PuPd_UP ;
    stGPIOInitInfo.GPIO_Pin   = uiESP32RSTGPIOIndex ;

    GPIO_Init(pstESP32RSTGPIOGroup, &stGPIOInitInfo) ;

    return ;
}

/*******************************************************************************
- Function    : __EGC_ESP_INIT_ConfigUART
- Description : 本函数配置MCU与ESP32连接的UART外设。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __EGC_ESP_INIT_ConfigUART(VOID)
{
    DRV_UART_INIT_INFO_S  stUARTInitInfo ;
    GPIO_TypeDef         *pstESP32UARTTXGPIOGroup ;
    GPIO_TypeDef         *pstESP32UARTRXGPIOGroup ;
    UINT                  uiESP32UARTTXGPIOIndex ;
    UINT                  uiESP32UARTRXGPIOIndex ;

    __EGC_ESP_INIT_CONFIGGPIO_GetPinGroup(EGC_BOARDS_ESP32_GPIO_UART_TX, &pstESP32UARTTXGPIOGroup) ;
    __EGC_ESP_INIT_CONFIGGPIO_GetPinGroup(EGC_BOARDS_ESP32_GPIO_UART_RX, &pstESP32UARTRXGPIOGroup) ;
    __EGC_ESP_INIT_CONFIGGPIO_GetPinIndex(EGC_BOARDS_ESP32_GPIO_UART_TX, &uiESP32UARTTXGPIOIndex) ;
    __EGC_ESP_INIT_CONFIGGPIO_GetPinIndex(EGC_BOARDS_ESP32_GPIO_UART_RX, &uiESP32UARTRXGPIOIndex) ;

    stUARTInitInfo.pstTXGPIOGroup = pstESP32UARTTXGPIOGroup ;
    stUARTInitInfo.pstRXGPIOGroup = pstESP32UARTRXGPIOGroup ;
    stUARTInitInfo.uiTXPinID      = uiESP32UARTTXGPIOIndex ;
    stUARTInitInfo.uiRXPinID      = uiESP32UARTRXGPIOIndex ;
    stUARTInitInfo.ucTXPinAFDef   = EGC_BOARDS_PERIPH_UART_GPIO_AF_DEF ;
    stUARTInitInfo.ucRXPinAFDef   = EGC_BOARDS_PERIPH_UART_GPIO_AF_DEF ;
    stUARTInitInfo.pstUARTPeriph  = EGC_BOARDS_PERIPH_UART_ESP32 ;
    stUARTInitInfo.uiBaudrate     = EGC_BOARDS_PERIPH_UART_BAUDRATE ;
    stUARTInitInfo.uiTXBufSize    = 512 ;
    stUARTInitInfo.uiRXBufSize    = 512 ;

    DRV_UART_Open(&g_stESPUARTDevice, &stUARTInitInfo) ;

    return ;
}

/*******************************************************************************
- Function    : __EGC_ESP_Init
- Description : 本函数为ESP模块的初始化函数。函数完成如下工作：
                1、初始化GPIO；
                2、初始化UART；
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __EGC_ESP_Init(VOID)
{
    /* 配置GPIO */
    __EGC_ESP_INIT_ConfigGPIO() ;

    /* 配置UART */
    __EGC_ESP_INIT_ConfigUART() ;

    /* 变量初始化 */
    g_stESPWA.uiTaskRun          = 0 ;
    g_stESPWA.uiWIFIUpdateEnable = 0 ;
    g_stESPWA.uiAP1Valid         = 0 ;
    g_stESPWA.uiAP2Valid         = 0 ;
    g_stESPWA.uiNTPServer1Valid  = 0 ;
    g_stESPWA.uiNTPServer2Valid  = 0 ;
    g_stESPWA.uiNTPServer3Valid  = 0 ;
    g_stESPWA.uiInitDone         = 1 ;

    return ;
}

/*******************************************************************************
- Function    : __EGC_ESP_HardwareReset
- Description : 本函数硬件复位ESP32。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __EGC_ESP_HardwareReset(VOID)
{
    OS_ERR            enErr ;
    GPIO_TypeDef     *pstESP32RSTGPIOGroup ;
    UINT              uiESP32RSTGPIOIndex ;

    __EGC_ESP_INIT_CONFIGGPIO_GetPinGroup(EGC_BOARDS_ESP32_GPIO_RST, &pstESP32RSTGPIOGroup) ;
    __EGC_ESP_INIT_CONFIGGPIO_GetPinIndex(EGC_BOARDS_ESP32_GPIO_RST, &uiESP32RSTGPIOIndex) ;
    
    /* 拉低引脚，以复位ESP32 */
    GPIO_ResetBits(pstESP32RSTGPIOGroup, uiESP32RSTGPIOIndex) ;

    /* 等待复位 */
    OSTimeDlyHMSM((CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT16U   )1u, 
                  (CPU_INT32U   )0u,
                  (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                  (OS_ERR      *)&enErr);

    /* 撤销复位 */
    GPIO_SetBits(pstESP32RSTGPIOGroup, uiESP32RSTGPIOIndex) ;

    /* 等待启动 */
    OSTimeDlyHMSM((CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT16U   )5u, 
                  (CPU_INT32U   )0u,
                  (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                  (OS_ERR      *)&enErr);

    return ;
}


/*******************************************************************************
- Function    : __EGC_ESP_AT_Reset
- Description : 本函数用于软件复位ESP32模块。
- Input       : NULL 
- Output      : NULL
- Return      : 0      : 操作正常；
                others : 操作失败；
- Others      :
*******************************************************************************/
INT __EGC_ESP_AT_Reset(VOID)
{
    OS_ERR enErr ;
    CHAR   acSendBuf[384] ;
    CHAR   acRecvBuf[384] ;
    UINT   uiRecvSize ;
    INT    iRetVal ;
    
    /* 整理发送的信息 */
    sprintf(acSendBuf, "AT+RST\r\n") ;
    
    /* 清空UART接收缓存 */
    while(0 != uiRecvSize)
    {
        DRV_UART_Read(&g_stESPUARTDevice, (UCHAR *)acRecvBuf, sizeof(acRecvBuf), &uiRecvSize, DRV_UART_READ_MODE_NONE) ;
    }

    /* 发送数据 */
    DRV_UART_Write(&g_stESPUARTDevice, (UCHAR *)acSendBuf, strlen(acSendBuf), DRV_UART_WRITE_MODE_NONE) ;

    /* 等待一段时间，以确保所有的信息都返回了 */
    OSTimeDlyHMSM((CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT16U   )1u, 
                  (CPU_INT32U   )0u,
                  (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                  (OS_ERR      *)&enErr);
    
    /* 接收数据 */
    memset(acRecvBuf, 0, sizeof(acRecvBuf)) ;
    DRV_UART_Read(&g_stESPUARTDevice, (UCHAR *)acRecvBuf, sizeof(acRecvBuf) - 1, &uiRecvSize, DRV_UART_READ_MODE_NONE) ;

    __strreplace(acRecvBuf, '\r', ' ') ;
    __strreplace(acRecvBuf, '\n', ' ') ;
    
#ifdef LOG2HOST
    sprintf(acSendBuf, "AT Reset return : %s\n", acRecvBuf) ;
    DRV_UART_Write(&g_stHSTUARTDevice, (UCHAR *)acSendBuf, strlen(acSendBuf), DRV_UART_WRITE_MODE_NONE) ;
#endif
    
    /* 检查是否成功 */
    if(NULL != strstr(acRecvBuf, "OK"))
    {
        /* 返回OK，命令成功 */
        iRetVal = 0 ;
    }
    else
    {
        iRetVal = -1 ;
    }

    /* 清空UART接收缓存 */
    while(0 != uiRecvSize)
    {
        DRV_UART_Read(&g_stESPUARTDevice, (UCHAR *)acRecvBuf, sizeof(acRecvBuf), &uiRecvSize, DRV_UART_READ_MODE_NONE) ;
    }

    return iRetVal ;
}

/*******************************************************************************
- Function    : __EGC_ESP_AT_SetModSTA
- Description : 本函数用于设置ESP32模块为STA模式。
- Input       : NULL 
- Output      : NULL
- Return      : 0      : 操作正常；
                others : 操作失败；
- Others      :
*******************************************************************************/
INT __EGC_ESP_AT_SetModSTA(VOID)
{
    OS_ERR enErr ;
    CHAR   acSendBuf[384] ;
    CHAR   acRecvBuf[384] ;
    UINT   uiRecvSize ;
    INT    iRetVal ;

    /* 整理发送的信息 */
    sprintf(acSendBuf, "AT+CWMODE=1\r\n") ;

    /* 发送数据 */
    DRV_UART_Write(&g_stESPUARTDevice, (UCHAR *)acSendBuf, strlen(acSendBuf), DRV_UART_WRITE_MODE_NONE) ;

    __strreplace(acSendBuf, '\r', ' ') ;
    __strreplace(acSendBuf, '\n', ' ') ;
    
    /* 等待一段时间，以确保所有的信息都返回了 */
    OSTimeDlyHMSM((CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT16U   )5u, 
                  (CPU_INT32U   )0u,
                  (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                  (OS_ERR      *)&enErr);

    /* 接收数据 */
    memset(acRecvBuf, 0, sizeof(acRecvBuf)) ;
    DRV_UART_Read(&g_stESPUARTDevice, (UCHAR *)acRecvBuf, sizeof(acRecvBuf) - 1, &uiRecvSize, DRV_UART_READ_MODE_NONE) ;

    __strreplace(acRecvBuf, '\r', ' ') ;
    __strreplace(acRecvBuf, '\n', ' ') ;

#ifdef LOG2HOST
    sprintf(acSendBuf, "AT Set Mod STA return : %s\n", acRecvBuf) ;
    DRV_UART_Write(&g_stHSTUARTDevice, (UCHAR *)acSendBuf, strlen(acSendBuf), DRV_UART_WRITE_MODE_NONE) ;
#endif

    /* 检查是否成功 */
    if(NULL != strstr(acRecvBuf, "OK"))
    {
        /* 返回OK，命令成功 */
        iRetVal = 0 ;
    }
    else
    {
        iRetVal = -1 ;
    }

    /* 清空UART接收缓存 */
    while(0 != uiRecvSize)
    {
        DRV_UART_Read(&g_stESPUARTDevice, (UCHAR *)acRecvBuf, sizeof(acRecvBuf), &uiRecvSize, DRV_UART_READ_MODE_NONE) ;
    }
    
    return iRetVal ;
}

/*******************************************************************************
- Function    : __EGC_ESP_AT_SetModDisable
- Description : 本函数用于设置ESP32模块为无WIFI模式。此时WIFI RF将被关闭。
- Input       : NULL 
- Output      : NULL
- Return      : 0      : 操作正常；
                others : 操作失败；
- Others      :
*******************************************************************************/
INT __EGC_ESP_AT_SetModDisable(VOID)
{
    OS_ERR enErr ;
    CHAR   acSendBuf[384] ;
    CHAR   acRecvBuf[384] ;
    UINT   uiRecvSize ;
    INT    iRetVal ;

    /* 整理发送的信息 */
    sprintf(acSendBuf, "AT+CWMODE=0\r\n") ;

    /* 发送数据 */
    DRV_UART_Write(&g_stESPUARTDevice, (UCHAR *)acSendBuf, strlen(acSendBuf), DRV_UART_WRITE_MODE_NONE) ;

    __strreplace(acSendBuf, '\r', ' ') ;
    __strreplace(acSendBuf, '\n', ' ') ;
    
    /* 等待一段时间，以确保所有的信息都返回了 */
    OSTimeDlyHMSM((CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT16U   )1u, 
                  (CPU_INT32U   )0u,
                  (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                  (OS_ERR      *)&enErr);

    /* 接收数据 */
    memset(acRecvBuf, 0, sizeof(acRecvBuf)) ;
    DRV_UART_Read(&g_stESPUARTDevice, (UCHAR *)acRecvBuf, sizeof(acRecvBuf) - 1, &uiRecvSize, DRV_UART_READ_MODE_NONE) ;

    __strreplace(acRecvBuf, '\r', ' ') ;
    __strreplace(acRecvBuf, '\n', ' ') ;

#ifdef LOG2HOST
    sprintf(acSendBuf, "AT Set Mod Disable return : %s\n", acRecvBuf) ;
    DRV_UART_Write(&g_stHSTUARTDevice, (UCHAR *)acSendBuf, strlen(acSendBuf), DRV_UART_WRITE_MODE_NONE) ;
#endif

    /* 检查是否成功 */
    if(NULL != strstr(acRecvBuf, "OK"))
    {
        /* 返回OK，命令成功 */
        iRetVal = 0 ;
    }
    else
    {
        iRetVal = -1 ;
    }

    /* 清空UART接收缓存 */
    while(0 != uiRecvSize)
    {
        DRV_UART_Read(&g_stESPUARTDevice, (UCHAR *)acRecvBuf, sizeof(acRecvBuf), &uiRecvSize, DRV_UART_READ_MODE_NONE) ;
    }
    
    return iRetVal ;
}

/*******************************************************************************
- Function    : __EGC_ESP_AT_ConnectAP
- Description : 本函数用于控制ESP32模块连接到AP之上。
- Input       : pcSSID     : SSID字符串指针；
                pcPassword : 密码字符串指针；
- Output      : NULL
- Return      : 0      : 操作正常；
                others : 操作失败；
- Others      :
*******************************************************************************/
INT __EGC_ESP_AT_ConnectAP(CHAR *pcSSID, CHAR *pcPassword)
{
    OS_ERR enErr ;
    CHAR   acSendBuf[384] ;
    CHAR   acRecvBuf[384] ;
    UINT   uiRecvSize ;
    INT    iRetVal ;

    /* 整理发送的信息 */
    sprintf(acSendBuf, "AT+CWJAP=\"%s\",\"%s\"\r\n", pcSSID, pcPassword) ;

    /* 发送数据 */
    DRV_UART_Write(&g_stESPUARTDevice, (UCHAR *)acSendBuf, strlen(acSendBuf), DRV_UART_WRITE_MODE_NONE) ;

    __strreplace((CHAR *)acSendBuf, '\r', ' ') ;
    __strreplace((CHAR *)acSendBuf, '\n', ' ') ;
    
    /* 等待一段时间，以确保所有的信息都返回了 */
    OSTimeDlyHMSM((CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT16U   )10u, 
                  (CPU_INT32U   )0u,
                  (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                  (OS_ERR      *)&enErr);

    /* 接收数据 */
    memset(acRecvBuf, 0, sizeof(acRecvBuf)) ;
    DRV_UART_Read(&g_stESPUARTDevice, (UCHAR *)acRecvBuf, sizeof(acRecvBuf) - 1, &uiRecvSize, DRV_UART_READ_MODE_NONE) ;

    __strreplace(acRecvBuf, '\r', ' ') ;
    __strreplace(acRecvBuf, '\n', ' ') ;

#ifdef LOG2HOST
    sprintf(acSendBuf, "AT Connect AP return : %s\n", acRecvBuf) ;
    DRV_UART_Write(&g_stHSTUARTDevice, (UCHAR *)acSendBuf, strlen(acSendBuf), DRV_UART_WRITE_MODE_NONE) ;
#endif

    /* 检查是否成功 */
    if(NULL != strstr(acRecvBuf, "OK"))
    {
        /* 返回OK，命令成功 */
        iRetVal = 0 ;
    }
    else
    {
        iRetVal = -1 ;
    }

    /* 清空UART接收缓存 */
    while(0 != uiRecvSize)
    {
        DRV_UART_Read(&g_stESPUARTDevice, (UCHAR *)acRecvBuf, sizeof(acRecvBuf), &uiRecvSize, DRV_UART_READ_MODE_NONE) ;
    }
    
    return iRetVal ;
}

/*******************************************************************************
- Function    : __EGC_ESP_AT_DisConnectAP
- Description : 本函数用于控制ESP32断开与AP的连接。
- Input       : VOID
- Output      : NULL
- Return      : 0      : 操作正常；
                others : 操作失败；
- Others      :
*******************************************************************************/
INT __EGC_ESP_AT_DisConnectAP(VOID)
{
    OS_ERR enErr ;
    CHAR   acSendBuf[384] ;
    CHAR   acRecvBuf[384] ;
    UINT   uiRecvSize ;
    INT    iRetVal ;

    /* 整理发送的信息 */
    sprintf(acSendBuf, "AT+CWQAP\r\n") ;

    /* 发送数据 */
    DRV_UART_Write(&g_stESPUARTDevice, (UCHAR *)acSendBuf, strlen(acSendBuf), DRV_UART_WRITE_MODE_NONE) ;

    __strreplace(acSendBuf, '\r', ' ') ;
    __strreplace(acSendBuf, '\n', ' ') ;
    
    /* 等待一段时间，以确保所有的信息都返回了 */
    OSTimeDlyHMSM((CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT16U   )10u, 
                  (CPU_INT32U   )0u,
                  (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                  (OS_ERR      *)&enErr);

    /* 接收数据 */
    memset(acRecvBuf, 0, sizeof(acRecvBuf)) ;
    DRV_UART_Read(&g_stESPUARTDevice, (UCHAR *)acRecvBuf, sizeof(acRecvBuf) - 1, &uiRecvSize, DRV_UART_READ_MODE_NONE) ;

    __strreplace(acRecvBuf, '\r', ' ') ;
    __strreplace(acRecvBuf, '\n', ' ') ;

#ifdef LOG2HOST
    sprintf(acSendBuf, "AT Disconnect AP return : %s\n", acRecvBuf) ;
    DRV_UART_Write(&g_stHSTUARTDevice, (UCHAR *)acSendBuf, strlen(acSendBuf), DRV_UART_WRITE_MODE_NONE) ;
#endif

    /* 检查是否成功 */
    if(NULL != strstr(acRecvBuf, "OK"))
    {
        /* 返回OK，命令成功 */
        iRetVal = 0 ;
    }
    else
    {
        iRetVal = -1 ;
    }

    /* 清空UART接收缓存 */
    while(0 != uiRecvSize)
    {
        DRV_UART_Read(&g_stESPUARTDevice, (UCHAR *)acRecvBuf, sizeof(acRecvBuf), &uiRecvSize, DRV_UART_READ_MODE_NONE) ;
    }
    
    return iRetVal ;
}

/*******************************************************************************
- Function    : __EGC_ESP_AT_ConfigSNTP
- Description : 本函数用于配置SNTP服务器的网络地址。
- Input       : pcServer1 : 服务端字符串指针。
- Output      : NULL
- Return      : 0      : 操作正常；
                others : 操作失败；
- Others      :
*******************************************************************************/
INT __EGC_ESP_AT_ConfigSNTP(CHAR *pcServer1, CHAR *pcServer2, CHAR *pcServer3)
{
    OS_ERR enErr ;
    UINT   uiPrintPos = 0 ;
    CHAR   acSendBuf[384] ;
    CHAR   acRecvBuf[384] ;
    UINT   uiRecvSize ;
    INT    iRetVal ;

    if(NULL == pcServer1 && NULL == pcServer2 && NULL == pcServer3)
    {
        return -1 ;
    }

    /* 整理发送的信息 */
    uiPrintPos += scnprintf(acSendBuf, 
                            sizeof(acSendBuf), 
                            "AT+CIPSNTPCFG=1,0,") ;

    if(NULL != pcServer1)
    {
        uiPrintPos += scnprintf(&acSendBuf[uiPrintPos], 
                                sizeof(acSendBuf) - uiPrintPos,
                                "\"%s\"",
                                pcServer1) ;
    }

    if(NULL != pcServer2)
    {
        uiPrintPos += scnprintf(&acSendBuf[uiPrintPos], 
                                sizeof(acSendBuf) - uiPrintPos,
                                ",\"%s\"",
                                pcServer2) ;
    }

    if(NULL != pcServer3)
    {
        uiPrintPos += scnprintf(&acSendBuf[uiPrintPos], 
                                sizeof(acSendBuf) - uiPrintPos,
                                ",\"%s\"",
                                pcServer3) ;        
    }

    scnprintf(&acSendBuf[uiPrintPos], 
              sizeof(acSendBuf) - uiPrintPos,
              "\r\n") ;

    /* 发送数据 */
    DRV_UART_Write(&g_stESPUARTDevice, (UCHAR *)acSendBuf, strlen(acSendBuf), DRV_UART_WRITE_MODE_NONE) ;


    if(NULL != pcServer1)
    {
    }
    else
    {
    }
    
    if(NULL != pcServer2)
    {
    }
    else
    {
    }

    if(NULL != pcServer3)
    {
    }
    else
    {
    }

    __strreplace(acSendBuf, '\r', ' ') ;
    __strreplace(acSendBuf, '\n', ' ') ;
    
    /* 等待一段时间，以确保所有的信息都返回了 */
    OSTimeDlyHMSM((CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT16U   )5u, 
                  (CPU_INT32U   )0u,
                  (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                  (OS_ERR      *)&enErr);

    /* 接收数据 */
    memset(acRecvBuf, 0, sizeof(acRecvBuf)) ;
    DRV_UART_Read(&g_stESPUARTDevice, (UCHAR *)acRecvBuf, sizeof(acRecvBuf) - 1, &uiRecvSize, DRV_UART_READ_MODE_NONE) ;

    __strreplace(acRecvBuf, '\r', ' ') ;
    __strreplace(acRecvBuf, '\n', ' ') ;

#ifdef LOG2HOST
    sprintf(acSendBuf, "AT Config SNTP return : %s\n", acRecvBuf) ;
    DRV_UART_Write(&g_stHSTUARTDevice, (UCHAR *)acSendBuf, strlen(acSendBuf), DRV_UART_WRITE_MODE_NONE) ;
#endif

    /* 检查是否成功 */
    if(NULL != strstr(acRecvBuf, "OK"))
    {
        /* 返回OK，命令成功 */
        iRetVal = 0 ;
    }
    else
    {
        iRetVal = -1 ;
    }

    /* 清空UART接收缓存 */
    while(0 != uiRecvSize)
    {
        DRV_UART_Read(&g_stESPUARTDevice, (UCHAR *)acRecvBuf, sizeof(acRecvBuf), &uiRecvSize, DRV_UART_READ_MODE_NONE) ;
    }
    
    return iRetVal ;    
}

/*******************************************************************************
- Function    : __EGC_ESP_AT_GetSNTPTime
- Description : 本函数用于控制ESP32模块通过SNTP协议获取网络时间。
- Input       : VOID
- Output      : puiTime : 用于保存返回时间的指针。
- Return      : 0      : 操作正常；
                others : 操作失败；
- Others      :
*******************************************************************************/
INT __EGC_ESP_AT_GetSNTPTime(UINT *puiTime)
{
    OS_ERR      enErr ;
    CHAR       *pStr ;
    BOOL_T      bGetFail = BOOL_FALSE ;
    struct tm   stTime ;
    UINT        uiRetryTimes ;
    CHAR        acSendBuf[384] ;
    CHAR        acRecvBuf[384] ;
    UINT        uiRecvSize ;
    UINT        uiRecvBufPos = 0 ;

    /* 整理发送的信息 */
    sprintf(acSendBuf, "AT+CIPSNTPTIME?\r\n") ;

    /* 发送数据 */
    DRV_UART_Write(&g_stESPUARTDevice, (UCHAR *)acSendBuf, strlen(acSendBuf), DRV_UART_WRITE_MODE_NONE) ;

    __strreplace(acSendBuf, '\r', ' ') ;
    __strreplace(acSendBuf, '\n', ' ') ;

    /* 本函数内不能简单地等待，因为要达到秒级误差，必须尽可能块地响应返回的数字，这里使用快速查询多次的方式来确定是否收到OK */
    uiRetryTimes = 1000 ;
    memset(acRecvBuf, 0, sizeof(acRecvBuf)) ;
    
    while(uiRetryTimes--)
    {
        /* 每10ms查询一次 */
        OSTimeDlyHMSM((CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT32U   )10u,
                      (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                      (OS_ERR      *)&enErr);
        
        /* 读取数据 */
        DRV_UART_Read(&g_stESPUARTDevice, 
                       (UCHAR *)(acRecvBuf + uiRecvBufPos), 
                       sizeof(acRecvBuf) - uiRecvBufPos - 1, 
                      &uiRecvSize, 
                      DRV_UART_READ_MODE_NONE) ;

        uiRecvBufPos += uiRecvSize ;

        /* 检查是否成功 */
        if(NULL != strstr(acRecvBuf, "OK"))
        {
            __strreplace(acRecvBuf, '\r', ' ') ;
            __strreplace(acRecvBuf, '\n', ' ') ;

#ifdef LOG2HOST
            sprintf(acSendBuf, "AT Get SNTP Time return : %s\n", acRecvBuf) ;
            DRV_UART_Write(&g_stHSTUARTDevice, (UCHAR *)acSendBuf, strlen(acSendBuf), DRV_UART_WRITE_MODE_NONE) ;
#endif
            
            /* 返回OK，命令成功 */
            /* 查找时间开始的位置 */
            pStr = strstr(acRecvBuf, "CIPSNTPTIME:") ;
            
            if(NULL == pStr)
            {
                /* 异常退出 */
                return -1 ;
            }
            else
            {
                /* 偏移过头部和星期，处理月份 */
                pStr    += (strlen("CIPSNTPTIME:") + strlen("Mon ")) ;
                pStr[3]  = 0 ;                                           /* 人为添加一个字符串结束符 */

                if(0 == strcmp(pStr, "Jan"))
                {
                    stTime.tm_mon = 0 ;
                }
                else if(0 == strcmp(pStr, "Feb"))
                {
                    stTime.tm_mon = 1 ;
                }
                else if(0 == strcmp(pStr, "Mar"))
                {
                    stTime.tm_mon = 2 ;
                }
                else if(0 == strcmp(pStr, "Apr"))
                {
                    stTime.tm_mon = 3 ;
                }
                else if(0 == strcmp(pStr, "May"))
                {
                    stTime.tm_mon = 4 ;
                }
                else if(0 == strcmp(pStr, "Jun"))
                {
                    stTime.tm_mon = 5 ;
                }
                else if(0 == strcmp(pStr, "Jul"))
                {
                    stTime.tm_mon = 6 ;
                }
                else if(0 == strcmp(pStr, "Aug"))
                {
                    stTime.tm_mon = 7 ;
                }
                else if(0 == strcmp(pStr, "Sep"))
                {
                    stTime.tm_mon = 8 ;
                }
                else if(0 == strcmp(pStr, "Oct"))
                {
                    stTime.tm_mon = 9 ;
                }
                else if(0 == strcmp(pStr, "Nov"))
                {
                    stTime.tm_mon = 10 ;
                }
                else if(0 == strcmp(pStr, "Dec"))
                {
                    stTime.tm_mon = 11 ;
                }
                
                /* 偏移过月份，处理日 */
                pStr += strlen("Jan ") ;
                
                if(pStr[0] == ' ')
                {
                    /* 日期只有1位数 */
                    stTime.tm_mday  = pStr[1] - '0' ;
                    pStr           += 3 ;
                }
                else
                {
                    /* 日期有2位数 */
                    stTime.tm_mday  = ((pStr[0] - '0') * 10) + (pStr[1] - '0') ;
                    pStr           += 3 ;
                }

                /* 处理时 */
                stTime.tm_hour  = ((pStr[0] - '0') * 10) + (pStr[1] - '0') ;
                pStr           += 3 ;

                /* 处理分 */
                stTime.tm_min  = ((pStr[0] - '0') * 10) + (pStr[1] - '0') ;
                pStr          += 3 ;

                /* 处理秒 */
                stTime.tm_sec  = ((pStr[0] - '0') * 10) + (pStr[1] - '0') ;
                pStr          += 3 ;

                /* 处理年 */
                stTime.tm_year = ((pStr[2] - '0') * 10) + (pStr[3] - '0') + 100 ;

                if(170 == stTime.tm_year)
                {
                    /* sntp操作获得了OK的响应，但是时间却是1970年1月1日，此时不可更新时间 */
                    bGetFail = BOOL_TRUE ;
                }

                /* 年月日时分秒都具备，将其转换为秒数 */
                *puiTime = mktime(&stTime) ;

                return 0 ;
            }
        }
        else
        {
            continue ;
        }
    }

    if(0 == uiRetryTimes)
    {
        return -1 ;
    }
    else if(BOOL_TRUE == bGetFail)
    {
        return -1 ;
    }
    else
    {
        return 0 ;
    }
}

/*******************************************************************************
- Function    : __EGC_ESP_SetRTC
- Description : 本函数初始化RTC的时间。
- Input       : uiRTC：RTC时间，秒数。
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __EGC_ESP_SetRTC(UINT uiRTC)
{
    struct tm       *pstTime ;
    RTC_TimeTypeDef  stRTCTime ;
    RTC_DateTypeDef  stRTCDate ;

    /* 适应东8区，需要加上8个小时的时间 */
    uiRTC += (8 * 60 * 60) ;

    /* 将秒数转换为struct        tm类型 */
    pstTime = localtime(&uiRTC) ; 

    /* 时间调整 */
    pstTime->tm_year -= 100 ; /* year 是从1900年开始算的，所以2000年后减去100年 */
    pstTime->tm_mon  += 1 ;   /* month 从0到11，因此要加1 */

    /* 年月日时分秒赋值 */
    stRTCTime.RTC_Seconds = pstTime->tm_sec ;
    stRTCTime.RTC_Minutes = pstTime->tm_min ;
    stRTCTime.RTC_Hours   = pstTime->tm_hour ;
    stRTCDate.RTC_Date    = pstTime->tm_mday ;
    stRTCDate.RTC_Month   = pstTime->tm_mon ;
    stRTCDate.RTC_Year    = pstTime->tm_year ;
    
    if(0 == pstTime->tm_wday)
    {
        /* tm结构体中的tm_wday从0~6，周日算起 */
        stRTCDate.RTC_WeekDay = RTC_Weekday_Sunday ;
    }
    else
    {
        stRTCDate.RTC_WeekDay = pstTime->tm_wday ;
    }

    /* 时间更新到RTC */
    RTC_SetTime(RTC_Format_BIN, &stRTCTime) ;
    RTC_SetDate(RTC_Format_BIN, &stRTCDate) ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_ESP_UpdateTime
- Description : 本函数用于控制ESP32模块通过SNTP协议获取网络时间。
- Input       : VOID
- Output      : VOID
- Return      : 0      : 操作正常；
                others : 操作失败；
- Others      :
*******************************************************************************/
INT __EGC_ESP_UpdateTime(VOID)
{
    UINT    uiTime ;
    BOOL_T  bSkipAP2 = BOOL_FALSE ;
    CHAR   *pcNTPServer1 ;
    CHAR   *pcNTPServer2 ;
    CHAR   *pcNTPServer3 ;
    OS_ERR  enErr ;
        
    /* 复位ESP32 */
    if(0 != __EGC_ESP_AT_Reset())
    {
        return -1 ;
    }

    /* 设置ESP32为STA模式 */
    if(0 != __EGC_ESP_AT_SetModSTA())
    {
        return -2 ;
    }

    /* 连接AP，连接之前需要检查AP是否有效，确保AP的SSID、Password是否为有效的字符串 */
    if(0 == g_stESPWA.uiAP1Valid && 0 == g_stESPWA.uiAP2Valid)
    {
        /* 没有有效的AP，因此退出 */
        return -3 ;
    }
    
    if(1 == g_stESPWA.uiAP1Valid)
    {
        
        /* 尝试连接AP1 */
        if(0 != __EGC_ESP_AT_ConnectAP(g_stESPWA.pcAP1SSID, g_stESPWA.pcAP1Password))
        {
            /* 如果连接AP1失败，判断AP2是否可用 */
            if(0 == g_stESPWA.uiAP2Valid)
            {
                /* AP2无效，直接返回 */
                return -4 ;
            }
        }
        else
        {
            /* AP1连接成功，跳过AP2的连接尝试 */
            bSkipAP2 = BOOL_TRUE ;
        }
    }

    OSTimeDlyHMSM((CPU_INT16U   )0u, 
                  (CPU_INT16U   )0u, 
                  (CPU_INT16U   )20u, 
                  (CPU_INT32U   )0u,
                  (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                  (OS_ERR      *)&enErr);
    
    /* AP2有效，并且AP1无效的情况下，才会连接AP2 */
    if(1 == g_stESPWA.uiAP2Valid && BOOL_FALSE == bSkipAP2)
    {
        /* 尝试连接AP2 */
        if(0 != __EGC_ESP_AT_ConnectAP(g_stESPWA.pcAP2SSID, g_stESPWA.pcAP2Password))
        {
            return -5 ;
        }
    }

    /* 配置SNTP服务器。配置之前需要确保字符串合法 */
    if(0 == g_stESPWA.uiNTPServer1Valid && 
       0 == g_stESPWA.uiNTPServer2Valid &&
       0 == g_stESPWA.uiNTPServer3Valid)
    {
        /* 未能设置有效的服务器 */
        return -6 ;
    }

    /* 配置SNTP */
    pcNTPServer1 = (1 == g_stESPWA.uiNTPServer1Valid)? g_stESPWA.pcNTPServer1 : NULL ;
    pcNTPServer2 = (1 == g_stESPWA.uiNTPServer2Valid)? g_stESPWA.pcNTPServer2 : NULL ;
    pcNTPServer3 = (1 == g_stESPWA.uiNTPServer3Valid)? g_stESPWA.pcNTPServer3 : NULL ;

    __EGC_ESP_AT_ConfigSNTP(pcNTPServer1,
                            pcNTPServer2,
                            pcNTPServer3) ;

    /* 获取SNTP时间 */
    if(0 != __EGC_ESP_AT_GetSNTPTime(&uiTime))
    {
        return 0 ;
    }
    else
    {
        /* 设置RTC时间 */
        __EGC_ESP_SetRTC(uiTime) ;
    }

    /* 记录下GetSNTPTime之后的UART接收缓存 */

    /* 断开与AP的连接 */
    if(0 != __EGC_ESP_AT_DisConnectAP())
    {
        __EGC_ESP_AT_Reset() ;
        return -7 ;
    }

    /* 禁止WIFI */
    if(0 != __EGC_ESP_AT_SetModDisable())
    {
        __EGC_ESP_AT_Reset() ;
        return -8 ;
    }

    return 0 ;
}

CODE_SECTION("==========================") ;
CODE_SECTION("==  ESP模块任务函数     ==") ;
CODE_SECTION("==========================") ;
/*******************************************************************************
- Function    : EGC_ESP_Task
- Description : 本函数为ESP模块的任务函数。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID EGC_ESP_Task(VOID)
{
    OS_ERR       enErr ;
    UINT         uiTaskDelayHour ;
    volatile INT iUpdateTimeRetVal ;
    CHAR         acSendBuf[64] ;
    
    /* 模块初始化 */
    __EGC_ESP_Init() ;

    /* 复位ESP32 */
    __EGC_ESP_HardwareReset() ;

    /* 等待Host模块就绪 */
    while(1 != g_stESPWA.uiTaskRun)
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
        /* 允许网络获取时间，则获取时间 */
        if(1 == g_stESPWA.uiWIFIUpdateEnable)
        {
            iUpdateTimeRetVal = __EGC_ESP_UpdateTime() ;

#ifdef LOG2HOST
        sprintf(acSendBuf, "Update Time Retval : %d\n", iUpdateTimeRetVal) ;
        DRV_UART_Write(&g_stHSTUARTDevice, (UCHAR *)acSendBuf, strlen(acSendBuf), DRV_UART_WRITE_MODE_NONE) ;
#endif

            switch(g_stESPWA.enUpdateInterval)
            {
                case UPDATE_TIME_1_HOUR :
                    uiTaskDelayHour = 1 ;
                    break ;
                case UPDATE_TIME_4_HOUR :
                    uiTaskDelayHour = 4 ;
                    break ;
                case UPDATE_TIME_12_HOUR :
                    uiTaskDelayHour = 12 ;
                    break ;
                case UPDATE_TIME_1_DAY :
                    uiTaskDelayHour = 24 ;
                    break ;
                case UPDATE_TIME_1_WEEK :
                    uiTaskDelayHour = 24 * 7 ;
                    break ;
                case UPDATE_TIME_2_WEEK :
                    uiTaskDelayHour = 24 * 14 ;
                    break ;
                case UPDATE_TIME_1_MONTH :
                    uiTaskDelayHour = 24 * 30 ;
                    break ;
                default :
                    uiTaskDelayHour = 1 ;
                    break ;
            }
        }
        else
        {
            uiTaskDelayHour = 1 ;
        }

        /* 任务循环间隔 */
        OSTimeDlyHMSM((CPU_INT16U   )uiTaskDelayHour, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT32U   )0u,
                      (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                      (OS_ERR      *)&enErr);
    }
}

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块中断函数定义    ==") ;
CODE_SECTION("==========================") ;
/*******************************************************************************
- Function    : EGC_ESP_ESP32UARTInterruptHandler
- Description : 与ESP32相连接的UART的中断处理函数。响应发送中断与接收中断。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID EGC_ESP_ESP32UARTIntHandler(VOID)
{
    DRV_UART_GeneralIntHandler(&g_stESPUARTDevice) ;

    return ;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file EGC_ESP.c. *******/  
