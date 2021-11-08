/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : EGC_HST.c
    Project name : EGC
    Module name  : 
    Date created : 2020年2月12日   17时1分50秒
    Author       : Ning.JianLi
    Description  : Host模块源文件。
*******************************************************************************/

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "pub_type.h"
#include "pub_lib.h"

CODE_SECTION("==========================") ;
CODE_SECTION("==  HST模块包含文件     ==") ;
CODE_SECTION("==========================") ;

/* <标准库> */
#include <string.h>
#include <time.h>

/* <第三方> */
#include "os.h"
#include "stm32f4xx.h"

/* <项目内部> */
#include "drv_uart_stm32f4xx.h"
#include "drv_w25q.h"

#include "EGC_Boards.h"
#include "EGC_Config.h"
#include "EGC_HST.h"

CODE_SECTION("==========================") ;
CODE_SECTION("==  HST模块全局变量     ==") ;
CODE_SECTION("==========================") ;
/* UART设备 */
DRV_UART_DEVICE_S             g_stHSTUARTDevice ;

/* W25Q设备 */
extern DRV_W25Q_DEVICE_S      g_stW25QDevice ;

/* 配置信息 */
extern EGC_CONFIG_INFO_S      g_stEGCConfigInfo ;

/* 模块工作区定义 */
EGC_HST_WORKAREA_S            g_stHSTWA ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  HST模块内部函数定义 ==") ;
CODE_SECTION("==========================") ;

/*******************************************************************************
- Function    : __EGC_HST_INIT_CONFIGGPIO_GetPinGroup
- Description : 本函数输入一个EGC_Boards.h文件中定义的引脚(EGC_BOARDS_GPIO_Pxx)，
                返回该引脚所在的GPIO组(GPIOx)。
- Input       : uiPin         ：EGC_BOARDS_GPIO_Pxx
- Output      : ppstGPIOGroup ：用于保存GPIOx的指针
- Return      : VOID
- Others      :
*******************************************************************************/
STATIC VOID __EGC_HST_INIT_CONFIGGPIO_GetPinGroup(UINT uiPin, GPIO_TypeDef **ppstGPIOGroup)
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
- Function    : __EGC_HST_INIT_CONFIGGPIO_GetPinIndex
- Description : 本函数输入一个EGC_Boards.h文件中定义的引脚(EGC_BOARDS_GPIO_Pxx)，
                返回该引脚的GPIO序号(GPIO_Pin_x)。
- Input       : uiPin    ：EGC_BOARDS_GPIO_Pxx
- Output      : puiIndex ：用于保存GPIO_Pin_x的指针
- Return      : VOID
- Others      :
*******************************************************************************/
STATIC VOID __EGC_HST_INIT_CONFIGGPIO_GetPinIndex(UINT uiPin, UINT *puiIndex)
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
- Function    : __EGC_HST_INIT_InitUART
- Description : 本函数配置Host模块使用到的USART外设。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __EGC_HST_INIT_ConfigUART(VOID)
{
    DRV_UART_INIT_INFO_S  stUARTInitInfo ;
    GPIO_TypeDef         *pstHostUARTTXGPIOGroup ;
    GPIO_TypeDef         *pstHostUARTRXGPIOGroup ;
    UINT                  uiHostUARTTXGPIOIndex ;
    UINT                  uiHostUARTRXGPIOIndex ;

    __EGC_HST_INIT_CONFIGGPIO_GetPinGroup(EGC_BOARDS_GPIO_HOST_UART_TX, &pstHostUARTTXGPIOGroup) ;
    __EGC_HST_INIT_CONFIGGPIO_GetPinGroup(EGC_BOARDS_GPIO_HOST_UART_RX, &pstHostUARTRXGPIOGroup) ;
    __EGC_HST_INIT_CONFIGGPIO_GetPinIndex(EGC_BOARDS_GPIO_HOST_UART_TX, &uiHostUARTTXGPIOIndex) ;
    __EGC_HST_INIT_CONFIGGPIO_GetPinIndex(EGC_BOARDS_GPIO_HOST_UART_RX, &uiHostUARTRXGPIOIndex) ;

    stUARTInitInfo.pstTXGPIOGroup = pstHostUARTTXGPIOGroup ;
    stUARTInitInfo.pstRXGPIOGroup = pstHostUARTRXGPIOGroup ;
    stUARTInitInfo.uiTXPinID      = uiHostUARTTXGPIOIndex ;
    stUARTInitInfo.uiRXPinID      = uiHostUARTRXGPIOIndex ;
    stUARTInitInfo.ucTXPinAFDef   = EGC_BOARDS_PERIPH_UART_HOST_GPIO_AF_DEF ;
    stUARTInitInfo.ucRXPinAFDef   = EGC_BOARDS_PERIPH_UART_HOST_GPIO_AF_DEF ;
    stUARTInitInfo.pstUARTPeriph  = EGC_BOARDS_PERIPH_UART_HOST ;
    stUARTInitInfo.uiBaudrate     = EGC_BOARDS_PERIPH_UART_HOST_BAUDRATE ;
    stUARTInitInfo.uiTXBufSize    = 2048 ;
    stUARTInitInfo.uiRXBufSize    = 2048 ;

    DRV_UART_Open(&g_stHSTUARTDevice, &stUARTInitInfo) ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_HST_Init
- Description : 本函数初始化HST模块。包括GPIO配置和UART配置，以及全局变量等。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __EGC_HST_Init(VOID)
{
    /* 复位错误码 */
    g_stHSTWA.uiErrCode = 0 ;
    
    /* UART配置 */
    __EGC_HST_INIT_ConfigUART() ;

    /* 工作区初始化 */
    g_stHSTWA.uiConfigUpdateByHost = 0 ;
    g_stHSTWA.uiTaskRun            = 0 ; 
    g_stHSTWA.uiInitDone           = 1 ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_HST_UARTSend
- Description : 函数实现通过UART发送数据。
- Input       : pucBuf : 待发送的数据指针。
                uiSize : 待发送的数据量。
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __EGC_HST_UARTSend(UCHAR *pucBuf, UINT uiSize)
{
    DRV_UART_Write(&g_stHSTUARTDevice, pucBuf, uiSize, DRV_UART_WRITE_MODE_WAIT_DONE) ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_HST_L2_CalcChkSum
- Description : 本函数用于计算一串数据的校验和，校验和为USHORT类型。
- Input       : pucBuf：指向待计算数据的指针。
                uiSize：计算的长度（字节数）。
                bMerge：指示是否将计算结果置入数据中。如果为BOOL_TRUE，则会将结果
                        置入到pucBuf中，uiSize指示的位置之后（发送用）。如果为
                        BOOL_FALSE，则不会置入（接收校验用）。
- Output      : NULL
- Return      : USHORT：计算所得的校验和。
- Others      :
*******************************************************************************/
USHORT __EGC_HST_L2_CalcChkSum(UCHAR *pucBuf, UINT uiSize, BOOL_T bMerge)
{
    UINT   uiLoop ;
    USHORT usChkSum ;

    /* 循环计算，得到校验和 */
    for(uiLoop = 0, usChkSum = 0 ; uiLoop < uiSize ; uiLoop++)
    {
        usChkSum += pucBuf[uiLoop] ;
    }

    if(BOOL_TRUE == bMerge)
    {
        /* 结果置入到Buf中 */
        *((USHORT *)(pucBuf + uiSize)) = usChkSum ;
    }

    return usChkSum ;
}

/*******************************************************************************
- Function    : __EGC_HST_PRM_InitTime
- Description : 本函数根据上位机发来的初始化时间消息，初始化RTC的时间。
- Input       : uiRTC：RTC时间，秒数。
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __EGC_HST_PRM_InitTime(UINT uiRTC)
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
    pstTime->tm_wday += 1 ;   /* weekday 从0到6，因此要加1 */

    /* 年月日时分秒赋值 */
    stRTCTime.RTC_Seconds = pstTime->tm_sec ;
    stRTCTime.RTC_Minutes = pstTime->tm_min ;
    stRTCTime.RTC_Hours   = pstTime->tm_hour ;
    stRTCDate.RTC_Date    = pstTime->tm_mday ;
    stRTCDate.RTC_Month   = pstTime->tm_mon ;
    stRTCDate.RTC_Year    = pstTime->tm_year ;
    stRTCDate.RTC_WeekDay = pstTime->tm_wday ;

    /* 时间更新到RTC */
    RTC_SetTime(RTC_Format_BIN, &stRTCTime) ;
    RTC_SetDate(RTC_Format_BIN, &stRTCDate) ;

    return ;
}

/*******************************************************************************
- Function    : __EGC_HST_PRM_ReplyConfigInfo
- Description : 本函数用于响应主机给出的读配置信息指令。函数按照如下流程工作：
                1、申请用于加工发送数据的缓存；
                2、整理L2首部；
                3、读取和封装配置信息数据；
                4、计算校验和；
                5、发送数据；
                6、回收缓存资源。
- Input       : enSegment ：要响应的信息数据段。
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __EGC_HST_PRM_ReplyConfigInfo(EGC_HST_CONFIG_INFO_SEGMENT_E enSegment)
{
    UCHAR                  aucUARTSendBuf[280] ;
    UCHAR                 *pucSrcBuf ;
    UINT                   uiMsgLen ;
    EGC_HST_MSG_L2_HEAD_S *pstL2Head ;

    /* 整理L2头部 */
    pstL2Head          = (EGC_HST_MSG_L2_HEAD_S *)aucUARTSendBuf ;
    pstL2Head->ucHead1 = EGC_HST_L2_HEAD1 ;
    pstL2Head->ucHead2 = EGC_HST_L2_HEAD2 ;
    pstL2Head->ucDst   = EGC_HST_L2_DST_HOST ;
    
    switch(enSegment)
    {
        case CONFIG_INFO_SEGMENT_1 :
            pstL2Head->ucType  = EGC_HST_L2_TYPE_RPL_RD_CONFIG_SEG1 ;
            pucSrcBuf          = &g_stEGCConfigInfo.aucRawData[EGC_HST_CONFI_INFO_SEGMENT_SIZE * 0];
            break ;
        case CONFIG_INFO_SEGMENT_2 :
            pstL2Head->ucType  = EGC_HST_L2_TYPE_RPL_RD_CONFIG_SEG2 ;
            pucSrcBuf          = &g_stEGCConfigInfo.aucRawData[EGC_HST_CONFI_INFO_SEGMENT_SIZE * 1];
            break ;
        case CONFIG_INFO_SEGMENT_3 :
            pstL2Head->ucType  = EGC_HST_L2_TYPE_RPL_RD_CONFIG_SEG3 ;
            pucSrcBuf          = &g_stEGCConfigInfo.aucRawData[EGC_HST_CONFI_INFO_SEGMENT_SIZE * 2];
            break ;
        case CONFIG_INFO_SEGMENT_4 :
            pstL2Head->ucType  = EGC_HST_L2_TYPE_RPL_RD_CONFIG_SEG4 ;
            pucSrcBuf          = &g_stEGCConfigInfo.aucRawData[EGC_HST_CONFI_INFO_SEGMENT_SIZE * 3];
            break ;
        case CONFIG_INFO_SEGMENT_5 :
            pstL2Head->ucType  = EGC_HST_L2_TYPE_RPL_RD_CONFIG_SEG5 ;
            pucSrcBuf          = &g_stEGCConfigInfo.aucRawData[EGC_HST_CONFI_INFO_SEGMENT_SIZE * 4];
            break ;
        case CONFIG_INFO_SEGMENT_6 :
            pstL2Head->ucType  = EGC_HST_L2_TYPE_RPL_RD_CONFIG_SEG6 ;
            pucSrcBuf          = &g_stEGCConfigInfo.aucRawData[EGC_HST_CONFI_INFO_SEGMENT_SIZE * 5];
            break ;
        case CONFIG_INFO_SEGMENT_7 :
            pstL2Head->ucType  = EGC_HST_L2_TYPE_RPL_RD_CONFIG_SEG7 ;
            pucSrcBuf          = &g_stEGCConfigInfo.aucRawData[EGC_HST_CONFI_INFO_SEGMENT_SIZE * 6];
            break ;
        case CONFIG_INFO_SEGMENT_8 :
            pstL2Head->ucType  = EGC_HST_L2_TYPE_RPL_RD_CONFIG_SEG8 ;
            pucSrcBuf          = &g_stEGCConfigInfo.aucRawData[EGC_HST_CONFI_INFO_SEGMENT_SIZE * 7];
            break ;
        default :
            break ;
    }
    pstL2Head->ucL7Len = EGC_HST_CONFI_INFO_SEGMENT_SIZE ;

    /* 复制寄存器数据到Buf */
    memcpy(aucUARTSendBuf + sizeof(EGC_HST_MSG_L2_HEAD_S),
           pucSrcBuf,
           EGC_HST_CONFI_INFO_SEGMENT_SIZE) ;

    /* 计算校验和 */
    __EGC_HST_L2_CalcChkSum((UCHAR *)pstL2Head, 
                            sizeof(EGC_HST_MSG_L2_HEAD_S) + EGC_HST_CONFI_INFO_SEGMENT_SIZE, 
                            BOOL_TRUE) ;
    /* 发送 */
    uiMsgLen = sizeof(EGC_HST_MSG_L2_HEAD_S) + EGC_HST_CONFI_INFO_SEGMENT_SIZE + sizeof(USHORT) ;
    __EGC_HST_UARTSend((UCHAR *)pstL2Head, uiMsgLen) ;    

    return ;
}

/*******************************************************************************
- Function    : __EGC_HST_PRM_WriteConfigInfo
- Description : 本函数用于响应主机给出的写配置信息指令。
- Input       : enSegment ：要响应的信息数据段。
                pucBuf    ：保存待写入数据的缓存。
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __EGC_HST_PRM_WriteConfigInfo(EGC_HST_CONFIG_INFO_SEGMENT_E enSegment, UCHAR *pucBuf)
{
    UCHAR *pucDstBuf ;

    switch(enSegment)
    {
        case CONFIG_INFO_SEGMENT_1 :
            pucDstBuf = &g_stEGCConfigInfo.aucRawData[EGC_HST_CONFI_INFO_SEGMENT_SIZE * 0];
            break ;
        case CONFIG_INFO_SEGMENT_2 :
            pucDstBuf = &g_stEGCConfigInfo.aucRawData[EGC_HST_CONFI_INFO_SEGMENT_SIZE * 1];
            break ;
        case CONFIG_INFO_SEGMENT_3 :
            pucDstBuf = &g_stEGCConfigInfo.aucRawData[EGC_HST_CONFI_INFO_SEGMENT_SIZE * 2];
            break ;
        case CONFIG_INFO_SEGMENT_4 :
            pucDstBuf = &g_stEGCConfigInfo.aucRawData[EGC_HST_CONFI_INFO_SEGMENT_SIZE * 3];
            break ;
        case CONFIG_INFO_SEGMENT_5 :
            pucDstBuf = &g_stEGCConfigInfo.aucRawData[EGC_HST_CONFI_INFO_SEGMENT_SIZE * 4];
            break ;
        case CONFIG_INFO_SEGMENT_6 :
            pucDstBuf = &g_stEGCConfigInfo.aucRawData[EGC_HST_CONFI_INFO_SEGMENT_SIZE * 5];
            break ;
        case CONFIG_INFO_SEGMENT_7 :
            pucDstBuf = &g_stEGCConfigInfo.aucRawData[EGC_HST_CONFI_INFO_SEGMENT_SIZE * 6];
            break ;
        case CONFIG_INFO_SEGMENT_8 :
            pucDstBuf = &g_stEGCConfigInfo.aucRawData[EGC_HST_CONFI_INFO_SEGMENT_SIZE * 7];
            break ;
        default :
            break ;
    }

    /* 数据复制到目标地址 */
    memcpy(pucDstBuf,
           pucBuf,
           EGC_HST_CONFI_INFO_SEGMENT_SIZE) ;

    /* 如果为最后一段，则将数据保存到Flash */
    if(CONFIG_INFO_SEGMENT_8 == enSegment)
    {
        DRV_W25Q_Write(&g_stW25QDevice,
                       g_stEGCConfigInfo.aucRawData, 
                       EGC_CONFIG_W25Q_ADDR_CONFIG, 
                       sizeof(g_stEGCConfigInfo)) ;

        /* 系统配置信息被上位机修改 */
        g_stHSTWA.uiConfigUpdateByHost = 1 ;
    }

    return ;
}


/*******************************************************************************
- Function    : __EGC_HST_PRM_WriteFlash
- Description : 本函数用于响应主机给出的写Flash指令。
- Input       : pucBuf    ：保存待写入数据的缓存，并在头部放置地址，长度等信息。
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __EGC_HST_PRM_WriteFlash(UCHAR *pucBuf)
{
    UINT   uiFlashAddr = 0 ;
    UCHAR  ucLen ;

    uiFlashAddr |= pucBuf[0] ; uiFlashAddr = (uiFlashAddr << 8) ;
    uiFlashAddr |= pucBuf[1] ; uiFlashAddr = (uiFlashAddr << 8) ;
    uiFlashAddr |= pucBuf[2] ; uiFlashAddr = (uiFlashAddr << 8) ;
    uiFlashAddr |= pucBuf[3] ; 
    ucLen        = pucBuf[4] ;

    DRV_W25Q_Write(&g_stW25QDevice,
                   pucBuf + sizeof(uiFlashAddr) + sizeof(ucLen), 
                   uiFlashAddr, 
                   ucLen) ;

    return ;
}

/*******************************************************************************
- Function    : __EGC_HST_ProcessRecvMsg
- Description : 本函数处理发送到本处理器的消息。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __EGC_HST_ProcessRecvMsg(VOID)
{
    STATIC UCHAR               aucUARTRecvBuf[300] ;
    STATIC UCHAR               aucUARTShiftBuf[2] ;
    STATIC UINT                uiUARTRecvBufEndPos = 0 ;
    STATIC UINT                uiUartRecvFindHead  = 0 ;
    UINT                       uiRecvSize ;
    UCHAR                      ucRecvTemp ;
    EGC_HST_MSG_L2_HEAD_S     *pstL2Head ;
    UCHAR                     *pucL7 ;
    UINT                       uiPackageTotalLen ;
    USHORT                     usChkSum ;

LABEL_HST_PRM_PORCESS_START :
    /* 获取消息 */
    DRV_UART_Read(&g_stHSTUARTDevice, &ucRecvTemp, 1, &uiRecvSize, DRV_UART_READ_MODE_NONE) ;

    if(0 != uiRecvSize)
    {
        /* 将数据放入到缓存中 */
        if(0 == uiUartRecvFindHead)
        {
            aucUARTShiftBuf[0] = aucUARTShiftBuf[1] ;
            aucUARTShiftBuf[1] = ucRecvTemp ;

            if(EGC_HST_L2_HEAD1 == aucUARTShiftBuf[0] && EGC_HST_L2_HEAD2 == aucUARTShiftBuf[1])
            {
                uiUartRecvFindHead = 1 ;

                memset(aucUARTRecvBuf, 0, sizeof(aucUARTRecvBuf)) ;
                
                aucUARTRecvBuf[0]   = EGC_HST_L2_HEAD1 ;
                aucUARTRecvBuf[1]   = EGC_HST_L2_HEAD2 ;
                uiUARTRecvBufEndPos = 2 ;
            }
        }
        else
        {
            aucUARTRecvBuf[uiUARTRecvBufEndPos++] = ucRecvTemp ;
        }
        
        if(1 == uiUartRecvFindHead)
        {
            /* 缓存中的数据长度大于等于L2头部长度+校验和长度，则可进行一次分析。这样确保头部中的L7Len是有效的 */
            if(uiUARTRecvBufEndPos >= (sizeof(EGC_HST_MSG_L2_HEAD_S) + 2))
            {
                /* L2检查 */
                pstL2Head = (EGC_HST_MSG_L2_HEAD_S *)aucUARTRecvBuf ;

                /* 检查长度是否够一个包 */
                uiPackageTotalLen = sizeof(EGC_HST_MSG_L2_HEAD_S) + pstL2Head->ucL7Len + sizeof(USHORT) ;

                if(uiPackageTotalLen <= uiUARTRecvBufEndPos)
                {
                    /* 已经接收了一个完整的帧，可以开始处理 */
                    /* L2检查-->校验和检查 */
                    usChkSum = __EGC_HST_L2_CalcChkSum(aucUARTRecvBuf, 
                                                       sizeof(EGC_HST_MSG_L2_HEAD_S) + (pstL2Head->ucL7Len), 
                                                       BOOL_FALSE) ;

                    if(usChkSum == *((USHORT *)(aucUARTRecvBuf + sizeof(EGC_HST_MSG_L2_HEAD_S) + pstL2Head->ucL7Len)))
                    {
                        /* L7处理 */
                        pucL7 = aucUARTRecvBuf + sizeof(EGC_HST_MSG_L2_HEAD_S) ;

                        /* 目的地为本机，根据消息类型处理 */
                        switch (pstL2Head->ucType)
                        {
                            /****************************************************************************************
                             * 上位机发起的操作处理
                             ****************************************************************************************/
                            case EGC_HST_L2_TYPE_CMD_RD_CONFIG_SEG1 :
                                /* 上位机读取F4配置信息，给与响应 */
                                __EGC_HST_PRM_ReplyConfigInfo(CONFIG_INFO_SEGMENT_1) ;
                                break ;
                            case EGC_HST_L2_TYPE_CMD_RD_CONFIG_SEG2 :
                                /* 上位机读取F4配置信息，给与响应 */
                                __EGC_HST_PRM_ReplyConfigInfo(CONFIG_INFO_SEGMENT_2) ;
                                break ;
                            case EGC_HST_L2_TYPE_CMD_RD_CONFIG_SEG3 :
                                /* 上位机读取F4配置信息，给与响应 */
                                __EGC_HST_PRM_ReplyConfigInfo(CONFIG_INFO_SEGMENT_3) ;
                                break ;
                            case EGC_HST_L2_TYPE_CMD_RD_CONFIG_SEG4 :
                                /* 上位机读取F4配置信息，给与响应 */
                                __EGC_HST_PRM_ReplyConfigInfo(CONFIG_INFO_SEGMENT_4) ;            
                                break ;
                            case EGC_HST_L2_TYPE_CMD_RD_CONFIG_SEG5 :
                                /* 上位机读取F4配置信息，给与响应 */
                                __EGC_HST_PRM_ReplyConfigInfo(CONFIG_INFO_SEGMENT_5) ;            
                                break ;
                            case EGC_HST_L2_TYPE_CMD_RD_CONFIG_SEG6 :
                                /* 上位机读取F4配置信息，给与响应 */
                                __EGC_HST_PRM_ReplyConfigInfo(CONFIG_INFO_SEGMENT_6) ;            
                                break ;
                            case EGC_HST_L2_TYPE_CMD_RD_CONFIG_SEG7 :
                                /* 上位机读取F4配置信息，给与响应 */
                                __EGC_HST_PRM_ReplyConfigInfo(CONFIG_INFO_SEGMENT_7) ;
                                break ;
                            case EGC_HST_L2_TYPE_CMD_RD_CONFIG_SEG8 :
                                /* 上位机读取F4配置信息，给与响应 */
                                __EGC_HST_PRM_ReplyConfigInfo(CONFIG_INFO_SEGMENT_8) ;            
                                break ;
                            case EGC_HST_L2_TYPE_CMD_WR_CONFIG_SEG1 :
                                /* 上位机写入F4配置信息，给与响应 */
                                __EGC_HST_PRM_WriteConfigInfo(CONFIG_INFO_SEGMENT_1, pucL7) ;
                                break ;
                            case EGC_HST_L2_TYPE_CMD_WR_CONFIG_SEG2 :
                                /* 上位机写入F4配置信息，给与响应 */
                                __EGC_HST_PRM_WriteConfigInfo(CONFIG_INFO_SEGMENT_2, pucL7) ;
                                break ;
                            case EGC_HST_L2_TYPE_CMD_WR_CONFIG_SEG3 :
                                /* 上位机写入F4配置信息，给与响应 */
                                __EGC_HST_PRM_WriteConfigInfo(CONFIG_INFO_SEGMENT_3, pucL7) ;            
                                break ;
                            case EGC_HST_L2_TYPE_CMD_WR_CONFIG_SEG4 :
                                /* 上位机写入F4配置信息，给与响应 */
                                __EGC_HST_PRM_WriteConfigInfo(CONFIG_INFO_SEGMENT_4, pucL7) ;            
                                break ;
                            case EGC_HST_L2_TYPE_CMD_WR_CONFIG_SEG5 :
                                /* 上位机写入F4配置信息，给与响应 */
                                __EGC_HST_PRM_WriteConfigInfo(CONFIG_INFO_SEGMENT_5, pucL7) ;            
                                break ;
                            case EGC_HST_L2_TYPE_CMD_WR_CONFIG_SEG6 :
                                /* 上位机写入F4配置信息，给与响应 */
                                __EGC_HST_PRM_WriteConfigInfo(CONFIG_INFO_SEGMENT_6, pucL7) ;            
                                break ;
                            case EGC_HST_L2_TYPE_CMD_WR_CONFIG_SEG7 :
                                /* 上位机写入F4配置信息，给与响应 */
                                __EGC_HST_PRM_WriteConfigInfo(CONFIG_INFO_SEGMENT_7, pucL7) ;            
                                break ;
                            case EGC_HST_L2_TYPE_CMD_WR_CONFIG_SEG8 :
                                /* 上位机写入F4配置信息，给与响应 */
                                __EGC_HST_PRM_WriteConfigInfo(CONFIG_INFO_SEGMENT_8, pucL7) ;            
                                break ;
                            case EGC_HST_L2_TYPE_CMD_UPDATE_TIME :
                                /* 上位机发出的更新系统时间的消息，根据该消息更新系统RTC时间 */
                                __EGC_HST_PRM_InitTime(*(UINT *)pucL7) ;
                                break ;
                            case EGC_HST_L2_TYPE_CMD_WR_FLASH :
                                /* 上位机发出的写W25Q SPI NOR Flash命令 */
                                __EGC_HST_PRM_WriteFlash(pucL7) ;
                                break ;
                            default :
                                /* 不能响应的消息类型，丢弃处理 */
                                break ;
                        }
                    }

                    /* 程序运行到此处，有2种可能，1是校验和错误，那么需要释放掉整个帧；2是处理完成，还是释放掉整个帧 */
                    /* 数据是一个字节为单位接收的，因此一定是刚好接收了一个帧就开始处理 */
                    uiUartRecvFindHead = 0 ;
                }
            }
        }

        goto LABEL_HST_PRM_PORCESS_START ;
    }
    else
    {
        /* 不能获取到消息字节，直接退出 */
        return ;
    }
}


CODE_SECTION("==========================") ;
CODE_SECTION("==  HST模块任务函数定义 ==") ;
CODE_SECTION("==========================") ;
/*******************************************************************************
- Function    : EGC_HST_Task
- Description : Host模块任务函数。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID EGC_HST_Task(VOID)
{
    OS_ERR enErr ;
    
    /* 模块初始化 */
    __EGC_HST_Init() ;

    /* 等待任务启动指令 */
    while(0 == g_stHSTWA.uiTaskRun)
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
            g_stHSTWA.uiErrCode = 0 ;
        }
    }
    
    while(DEF_TRUE)
    {
        /* ##############################################################################################
         * 阶段1：获取上位机指令。如果没有指令，则跳过。
         * ##############################################################################################
         */
        /* 检查是否有需要处理的接收消息 */
        __EGC_HST_ProcessRecvMsg() ;

        /* ##############################################################################################
         * 阶段2：检查是否需要向主控汇报实时状态。
         * ##############################################################################################
         */
        
        /* 延时 */
        OSTimeDlyHMSM((CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u,
                      (CPU_INT32U   )EGC_HST_TASK_CYCLE_GAP_IN_MS, 
                      (OS_OPT       )OS_OPT_TIME_HMSM_STRICT, 
                      (OS_ERR      *)&enErr) ;
    }
}

CODE_SECTION("==========================") ;
CODE_SECTION("==  HST模块中断函数定义 ==") ;
CODE_SECTION("==========================") ;
/*******************************************************************************
- Function    : EGC_HST_UARTInterruptHandler
- Description : 与主机相连接的UART的中断处理函数。响应发送中断与接收中断。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID EGC_HST_UARTInterruptHandler(VOID)
{
    DRV_UART_GeneralIntHandler(&g_stHSTUARTDevice) ;
    return ;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file EGC_HST.c. *******/  
