/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : drv_uart_stm32f4xx.c
    Project name : STM32F4XX版本UART外设驱动。
    Module name  : 
    Date created : 2020年9月30日   13时48分51秒
    Author       : Ning.JianLi
    Description  : STM32F4XX版本UART外设驱动源文件。
*******************************************************************************/

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include <stdlib.h>
#include <string.h>

#include "drv_uart_stm32f4xx.h"

/*******************************************************************************
- Function    : __DRV_UART_TailLeadZeros
- Description : 本函数检查一个输入数据,从最低位到最高位,遇到的第一个bit '1'之前,
                一共有多少个bit '0'.
- Input       : uiData : 要检查的数据.
- Output      : NULL 
- Return      : UCHAR  : 从低位到高位,第一个出现的bit '1'之前,一共出现的0的个数.
- Others      : 特殊情况 : 如果uiData为0,则返回为0.
*******************************************************************************/
UCHAR __DRV_UART_TailLeadZeros(UINT uiData)
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
- Function    : __CalcBufFreeSize
- Description : 本函数用于计算一个缓存（FIFO）有多少剩余空间可写入。
                函数接收缓存大小、读指针、写指针，并根据读指针和写指针的大小关系
                计算缓存剩余空间。
- Input       : uiBufTotalSize : 缓存总大小.
                uiBufWrPtr     : 缓存写指针.
                uiBufRdPtr     : 缓存读指针.
- Output      : NULL 
- Return      : UINT           : 缓存剩余空间.
- Others      : 1、返回的缓存剩余空间比实际空间小1.这样做是为了避免写入后，读写指
                   针重合，从而误判空间大小.
*******************************************************************************/
UINT __CalcBufFreeSize(UINT uiBufTotalSize, UINT uiBufWrPtr, UINT uiBufRdPtr)
{
    UINT uiRetVal ;
    
    if(uiBufWrPtr > uiBufRdPtr)
    {
        uiRetVal = uiBufTotalSize - uiBufWrPtr + uiBufRdPtr ;
    }
    else if(uiBufWrPtr < uiBufRdPtr)
    {
        uiRetVal = uiBufRdPtr - uiBufWrPtr ;
    }
    else
    {
        uiRetVal = uiBufTotalSize ;
    }

    /* 容量减1.这样容量比实际值小1.避免写入后WrPtr和RdPtr重合，被认为是缓存为空 */
    /* 例如，WrPtr为10，RdPtr为11，则按上述计算后可写入1，写入后WrPtr为11，
     * 再按上述运算，缓存编程空。
     */
    uiRetVal-- ;

    return uiRetVal ;
}

/*******************************************************************************
- Function    : __CalcBufUsedSize
- Description : 本函数用于计算一个缓存（FIFO）有多少剩余数据可读取。
                函数接收缓存大小、读指针、写指针，并根据读指针和写指针的大小关系
                计算缓存内的有效数据。
- Input       : uiBufTotalSize : 缓存总大小.
                uiBufWrPtr     : 缓存写指针.
                uiBufRdPtr     : 缓存读指针.
- Output      : NULL 
- Return      : UINT           : 缓存有效数据.
- Others      : NULL
*******************************************************************************/
UINT __CalcBufUsedSize(UINT uiBufTotalSize, UINT uiBufWrPtr, UINT uiBufRdPtr)
{
    UINT uiRetVal ;
    
    if(uiBufWrPtr > uiBufRdPtr)
    {
        uiRetVal = uiBufWrPtr - uiBufRdPtr ;
    }
    else if(uiBufWrPtr < uiBufRdPtr)
    {
        uiRetVal = uiBufTotalSize - uiBufRdPtr + uiBufWrPtr ;
    }
    else
    {
        uiRetVal = 0 ;
    }

    return uiRetVal ;
}

/*******************************************************************************
- Function    : DRV_UART_Open
- Description : 本函数创建一个基于STM32F4XX的UART外设的UART设备.函数流程主要包含如
                下几个步骤:
                1. 配置UART的GPIO;
                2. 配置UART外设;
                3. 创建UART设备.
- Input       : pstUARTInitInfo : 
                    配置信息,包括GPIO配置信息,UART外设配置信息.
- Output      : pstUARTDevice : 
                    UART设备指针. 
- Return      : DRV_UART_ERROR_CODE_E : 
                    错误码.
                    UART_ERROR_OK : 创建成功.
                    其他值           : 查看相关定义.
- Others      : 
*******************************************************************************/
DRV_UART_ERROR_CODE_E DRV_UART_Open(DRV_UART_DEVICE_S *pstUARTDevice, DRV_UART_INIT_INFO_S *pstUARTInitInfo)
{
    UINT               uiAHBAPBPeriphClockCtrl = 0 ;
    GPIO_InitTypeDef   stGPIOInitInfo ;
    USART_InitTypeDef  stUARTInitInfo ;
    NVIC_InitTypeDef   stNVICInitInfo ;
#ifdef DRV_UART_USE_UCOS_III
    OS_ERR           enErr ;
#endif

    /* 参数检查 */
    if(NULL == pstUARTDevice)
    {
        return UART_ERROR_OPEN_UART_DEVICE_PTR_NULL ;
    }

    if(NULL == pstUARTInitInfo)
    {
        return UART_ERROR_OPEN_CONFIG_INFO_PTR_NULL ;
    }

    if(0 == IS_GPIO_ALL_PERIPH(pstUARTInitInfo->pstTXGPIOGroup))
    {
        return UART_ERROR_OPEN_TX_PIN_GROUP_ERROR ;
    }
    
    if(0 == IS_GPIO_ALL_PERIPH(pstUARTInitInfo->pstRXGPIOGroup))
    {
        return UART_ERROR_OPEN_RX_PIN_GROUP_ERROR ;
    }

    if(0 == IS_GPIO_PIN(pstUARTInitInfo->uiTXPinID))
    {
        return UART_ERROR_OPEN_TX_PIN_ERROR ;
    }

    if(0 == IS_GPIO_PIN(pstUARTInitInfo->uiRXPinID))
    {
        return UART_ERROR_OPEN_RX_PIN_ERROR ;
    }

    if(0 == IS_GPIO_AF(pstUARTInitInfo->ucTXPinAFDef))
    {
        return UART_ERROR_OPEN_TX_AF_ERROR ;
    }

    if(0 == IS_GPIO_AF(pstUARTInitInfo->ucRXPinAFDef))
    {
        return UART_ERROR_OPEN_RX_AF_ERROR ;
    }

    if(0 == IS_USART_ALL_PERIPH(pstUARTInitInfo->pstUARTPeriph))
    {
        return UART_ERROR_OPEN_INVALID_UART_PERIPH ;
    }

    if(0 == IS_USART_BAUDRATE(pstUARTInitInfo->uiBaudrate))
    {
        return UART_ERROR_OPEN_BAUDRATE_ERROR ;
    }

    if(0 == IS_UART_BUFFER_SIZE(pstUARTInitInfo->uiTXBufSize))
    {
        return UART_ERROR_OPEN_TX_BUFFER_SIZE_ERROR ;
    }

    if(0 == IS_UART_BUFFER_SIZE(pstUARTInitInfo->uiRXBufSize))
    {
        return UART_ERROR_OPEN_RX_BUFFER_SIZE_ERROR ;
    }

    /* ##############################################################################################
     * STAGE 1 : GPIO 配置
     * ##############################################################################################
     */
    /* 使能GPIO时钟 */
    switch((UINT)pstUARTInitInfo->pstTXGPIOGroup)
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
            uiAHBAPBPeriphClockCtrl |= RCC_AHB1Periph_GPIOC ;
            break ;
        default :
            break ;
    }

    switch((UINT)pstUARTInitInfo->pstRXGPIOGroup)
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
            uiAHBAPBPeriphClockCtrl |= RCC_AHB1Periph_GPIOC ;
            break ;
        default :
            break ;
    }

    RCC_AHB1PeriphClockCmd(uiAHBAPBPeriphClockCtrl, ENABLE) ;

    /* 引脚信息配置 */ /* uiXXXPinID在底层配置函数有检查，这里不检查 */
    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_AF ;
    stGPIOInitInfo.GPIO_Speed = GPIO_High_Speed ;
    stGPIOInitInfo.GPIO_OType = GPIO_OType_PP ;
    stGPIOInitInfo.GPIO_PuPd  = GPIO_PuPd_NOPULL ;

    stGPIOInitInfo.GPIO_Pin = pstUARTInitInfo->uiTXPinID ;
    GPIO_Init(pstUARTInitInfo->pstTXGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin = pstUARTInitInfo->uiRXPinID ;
    GPIO_Init(pstUARTInitInfo->pstRXGPIOGroup, &stGPIOInitInfo) ;

    /* 复用功能配置 */
    GPIO_PinAFConfig(pstUARTInitInfo->pstTXGPIOGroup, 
                     __DRV_UART_TailLeadZeros(pstUARTInitInfo->uiTXPinID),  
                     pstUARTInitInfo->ucTXPinAFDef) ;
    GPIO_PinAFConfig(pstUARTInitInfo->pstRXGPIOGroup, 
                     __DRV_UART_TailLeadZeros(pstUARTInitInfo->uiRXPinID), 
                     pstUARTInitInfo->ucRXPinAFDef) ;

    /* ##############################################################################################
     * STAGE 2 : UART 配置
     * ##############################################################################################
     */
    /* 使能UART时钟 */
    switch((UINT)pstUARTInitInfo->pstUARTPeriph)
    {
        case (UINT)USART1 :
            uiAHBAPBPeriphClockCtrl = RCC_APB2Periph_USART1 ;
            break ;
        case (UINT)USART2 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_USART2 ;
            break ;
        case (UINT)USART3 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_USART3 ;
            break ;
        case (UINT)UART4 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_UART4 ;
            break ;
        case (UINT)UART5 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_UART5 ;
            break ;
        case (UINT)USART6 :
            uiAHBAPBPeriphClockCtrl = RCC_APB2Periph_USART6 ;
            break ;
        case (UINT)UART7 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_UART7 ;
            break ;
        case (UINT)UART8 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_UART8 ;
            break ;
        default :
            return UART_ERROR_OPEN_INVALID_UART_PERIPH ;
    }
    
    if(USART1 == pstUARTInitInfo->pstUARTPeriph || USART6 == pstUARTInitInfo->pstUARTPeriph)
    {
        RCC_APB2PeriphClockCmd(uiAHBAPBPeriphClockCtrl, ENABLE) ;
    }
    else
    {
        RCC_APB1PeriphClockCmd(uiAHBAPBPeriphClockCtrl, ENABLE) ;
    }

    /* 外设配置 */
    stUARTInitInfo.USART_BaudRate            = pstUARTInitInfo->uiBaudrate ;
    stUARTInitInfo.USART_WordLength          = USART_WordLength_8b ;
    stUARTInitInfo.USART_StopBits            = USART_StopBits_1 ;
    stUARTInitInfo.USART_Parity              = USART_Parity_No ;
    stUARTInitInfo.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx ;
    stUARTInitInfo.USART_HardwareFlowControl = USART_HardwareFlowControl_None ;

    USART_Init(pstUARTInitInfo->pstUARTPeriph, &stUARTInitInfo) ;

    /* 禁止发送中断，允许接收中断 */
    USART_ITConfig(pstUARTInitInfo->pstUARTPeriph, USART_IT_TXE,  DISABLE) ;
    USART_ITConfig(pstUARTInitInfo->pstUARTPeriph, USART_IT_RXNE, ENABLE) ;

    /* NVIC配置 */
	/* BEGIN : 增加UART外设中断号判断。用户无需设置，初始化程序自动判断中断号。 */
    switch((UINT)pstUARTInitInfo->pstUARTPeriph)
    {
        case (UINT)USART1 :
            stNVICInitInfo.NVIC_IRQChannel = DRV_UART_PERIPH_IRQ_NUMBER_USART1 ;
            break ;
        case (UINT)USART2 :
            stNVICInitInfo.NVIC_IRQChannel = DRV_UART_PERIPH_IRQ_NUMBER_USART2 ;
            break ;
        case (UINT)USART3 :
            stNVICInitInfo.NVIC_IRQChannel = DRV_UART_PERIPH_IRQ_NUMBER_USART3 ;
            break ;
        case (UINT)UART4 :
            stNVICInitInfo.NVIC_IRQChannel = DRV_UART_PERIPH_IRQ_NUMBER_UART4 ;
            break ;
        case (UINT)UART5 :
            stNVICInitInfo.NVIC_IRQChannel = DRV_UART_PERIPH_IRQ_NUMBER_UART5 ;
            break ;
        case (UINT)USART6 :
            stNVICInitInfo.NVIC_IRQChannel = DRV_UART_PERIPH_IRQ_NUMBER_USART6 ;
            break ;
        case (UINT)UART7 :
            stNVICInitInfo.NVIC_IRQChannel = DRV_UART_PERIPH_IRQ_NUMBER_UART7 ;
            break ;
        case (UINT)UART8 :
            stNVICInitInfo.NVIC_IRQChannel = DRV_UART_PERIPH_IRQ_NUMBER_UART8 ;
            break ;
        default :
			/* 上面代码有判断外设号是否正确，这里不再判断 */
            break ;
    }
	/* stNVICInitInfo.NVIC_IRQChannel    = pstUARTInitInfo->uiUARTIrqNumber ; */
	/* END   : 增加UART外设中断号判断。用户无需设置，初始化程序自动判断中断号。 */
    stNVICInitInfo.NVIC_IRQChannelCmd = ENABLE ;
    NVIC_Init(&stNVICInitInfo) ;

    /* 使能UART */
    USART_Cmd(pstUARTInitInfo->pstUARTPeriph, ENABLE) ;

    /* ##############################################################################################
     * STAGE 3 : 创建UART控制设备
     * ##############################################################################################
     */
    /* 分配缓存 */
    pstUARTDevice->pucTXBuf = malloc(pstUARTInitInfo->uiTXBufSize) ;

    if(NULL == pstUARTDevice->pucTXBuf)
    {
        /* 撤销对UART的配置 */
        USART_DeInit(pstUARTInitInfo->pstUARTPeriph) ;
        
        return UART_ERROR_OPEN_ALLOC_TX_BUFFER_SIZE_ERROR ;
    }

    pstUARTDevice->pucRXBuf = malloc(pstUARTInitInfo->uiRXBufSize) ;

    if(NULL == pstUARTDevice->pucRXBuf)
    {
        /* 撤销对UART的配置 */
        USART_DeInit(pstUARTInitInfo->pstUARTPeriph) ;
        
        /* 释放TX缓存 */
        free(pstUARTDevice->pucTXBuf) ;
        
        return UART_ERROR_OPEN_ALLOC_RX_BUFFER_SIZE_ERROR ;
    }

    /* 记录信息 */
    pstUARTDevice->pstUARTPeriph = pstUARTInitInfo->pstUARTPeriph ;
    
    pstUARTDevice->usTXBufWrPtr  = 0 ;
    pstUARTDevice->usTXBufRdPtr  = 0 ;
    pstUARTDevice->uiTXBufSize   = pstUARTInitInfo->uiTXBufSize ;

    pstUARTDevice->usRXBufWrPtr  = 0 ;
    pstUARTDevice->usRXBufRdPtr  = 0 ;
    pstUARTDevice->uiRXBufSize   = pstUARTInitInfo->uiRXBufSize ;
    
    pstUARTDevice->ucOpenFlag    = DRV_UART_OPEN_FLAG ;

#ifndef DRV_UART_USE_UCOS_III
    pstUARTDevice->ucTXDone = 0 ;
#else
    OSSemCreate((OS_SEM      *)&pstUARTDevice->stSemTxDone, 
                (CPU_CHAR    *)"SEM_UART_DEVICE_TX_DONE", 
                (OS_SEM_CTR   )0, 
                (OS_ERR      *)&enErr) ;
#endif

    return UART_ERROR_OK ;
}


/*******************************************************************************
- Function    : DRV_UART_Close
- Description : 本函数关闭一个基于STM32F4XX的UART外设的UART设备.
- Input       : pstUARTInitInfo : 
                    配置信息,包括GPIO配置信息,UART外设配置信息.
- Output      : pstUARTDevice : 
                    UART设备指针. 
- Return      : DRV_UART_ERROR_CODE_E : 
                    错误码.
                    UART_ERROR_OK : 操作成功.
                    其他值           : 查看相关定义.
- Others      : 
*******************************************************************************/
DRV_UART_ERROR_CODE_E DRV_UART_Close(DRV_UART_DEVICE_S *pstUARTDevice, DRV_UART_INIT_INFO_S *pstUARTInitInfo)
{
    UINT             uiAHBAPBPeriphClockCtrl = 0 ;
    GPIO_InitTypeDef stGPIOInitInfo ;
#ifdef DRV_UART_USE_UCOS_III
    OS_ERR           enErr ;
#endif

    /* 参数检查 */
    if(NULL == pstUARTDevice)
    {
        return UART_ERROR_CLOSE_UART_DEVICE_PTR_NULL ;
    }

    if(NULL == pstUARTInitInfo)
    {
        return UART_ERROR_CLOSE_CONFIG_INFO_PTR_NULL ;
    }

    if(0 == IS_GPIO_ALL_PERIPH(pstUARTInitInfo->pstTXGPIOGroup))
    {
        return UART_ERROR_CLOSE_TX_PIN_GROUP_ERROR ;
    }
    
    if(0 == IS_GPIO_ALL_PERIPH(pstUARTInitInfo->pstRXGPIOGroup))
    {
        return UART_ERROR_CLOSE_RX_PIN_GROUP_ERROR ;
    }

    if(0 == IS_GPIO_PIN(pstUARTInitInfo->uiTXPinID))
    {
        return UART_ERROR_CLOSE_TX_PIN_ERROR ;
    }

    if(0 == IS_GPIO_PIN(pstUARTInitInfo->uiRXPinID))
    {
        return UART_ERROR_CLOSE_RX_PIN_ERROR ;
    }

    if(0 == IS_USART_ALL_PERIPH(pstUARTInitInfo->pstUARTPeriph))
    {
        return UART_ERROR_CLOSE_INVALID_UART_PERIPH ;
    }

    if(DRV_UART_OPEN_FLAG == pstUARTDevice->ucOpenFlag)
    {
        return UART_ERROR_CLOSE_UART_NOT_OPENED ;
    }
    
    /* ##############################################################################################
     * STAGE 1 : GPIO 配置撤销
     * ##############################################################################################
     */
    /* 引脚信息配置 */ /* uiXXXPinID在底层配置函数有检查，这里不检查 */
    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_IN ;    /* 所有引脚配置为输入 */
    stGPIOInitInfo.GPIO_Speed = GPIO_Low_Speed ;
    stGPIOInitInfo.GPIO_OType = GPIO_OType_PP ;
    stGPIOInitInfo.GPIO_PuPd  = GPIO_PuPd_NOPULL ;

    stGPIOInitInfo.GPIO_Pin = pstUARTInitInfo->uiTXPinID ;
    GPIO_Init(pstUARTInitInfo->pstTXGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin = pstUARTInitInfo->uiRXPinID ;
    GPIO_Init(pstUARTInitInfo->pstRXGPIOGroup, &stGPIOInitInfo) ;

    /* ##############################################################################################
     * STAGE 2 : UART 配置撤销
     * ##############################################################################################
     */
    /* 禁止所有中断 */
    USART_ITConfig(pstUARTInitInfo->pstUARTPeriph, USART_IT_TXE,  DISABLE) ;
    USART_ITConfig(pstUARTInitInfo->pstUARTPeriph, USART_IT_RXNE, DISABLE) ;

    /* 禁止NVIC中相应UART中断 */
	/* BEGIN : 增加UART外设中断号判断。用户无需设置，初始化程序自动判断中断号。 */
    switch((UINT)pstUARTInitInfo->pstUARTPeriph)
    {
        case (UINT)USART1 :
			NVIC_DisableIRQ((IRQn_Type)DRV_UART_PERIPH_IRQ_NUMBER_USART1) ;
            break ;
        case (UINT)USART2 :
            NVIC_DisableIRQ((IRQn_Type)DRV_UART_PERIPH_IRQ_NUMBER_USART2) ;
            break ;
        case (UINT)USART3 :
            NVIC_DisableIRQ((IRQn_Type)DRV_UART_PERIPH_IRQ_NUMBER_USART3) ;
            break ;
        case (UINT)UART4 :
            NVIC_DisableIRQ((IRQn_Type)DRV_UART_PERIPH_IRQ_NUMBER_UART4) ;
            break ;
        case (UINT)UART5 :
            NVIC_DisableIRQ((IRQn_Type)DRV_UART_PERIPH_IRQ_NUMBER_UART5) ;
            break ;
        case (UINT)USART6 :
            NVIC_DisableIRQ((IRQn_Type)DRV_UART_PERIPH_IRQ_NUMBER_USART6) ;
            break ;
        case (UINT)UART7 :
            NVIC_DisableIRQ((IRQn_Type)DRV_UART_PERIPH_IRQ_NUMBER_UART7) ;
            break ;
        case (UINT)UART8 :
            NVIC_DisableIRQ((IRQn_Type)DRV_UART_PERIPH_IRQ_NUMBER_UART8) ;
            break ;
        default :
			/* 上面代码有判断外设号是否正确，这里不再判断 */
            break ;
    }
	/* NVIC_DisableIRQ((IRQn_Type)pstUARTInitInfo->uiUARTIrqNumber) ; */
	/* END   : 增加UART外设中断号判断。用户无需设置，初始化程序自动判断中断号。 */

    /* 禁止UART */
    USART_Cmd(pstUARTInitInfo->pstUARTPeriph, DISABLE) ;

    /* 禁止UART时钟 */
    switch((UINT)pstUARTInitInfo->pstUARTPeriph)
    {
        case (UINT)USART1 :
            uiAHBAPBPeriphClockCtrl = RCC_APB2Periph_USART1 ;
            break ;
        case (UINT)USART2 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_USART2 ;
            break ;
        case (UINT)USART3 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_USART3 ;
            break ;
        case (UINT)UART4 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_UART4 ;
            break ;
        case (UINT)UART5 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_UART5 ;
            break ;
        case (UINT)USART6 :
            uiAHBAPBPeriphClockCtrl = RCC_APB2Periph_USART6 ;
            break ;
        case (UINT)UART7 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_UART7 ;
            break ;
        case (UINT)UART8 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_UART8 ;
            break ;
        default :
            return UART_ERROR_OPEN_INVALID_UART_PERIPH ;
    }
    
    if(USART1 == pstUARTInitInfo->pstUARTPeriph || USART6 == pstUARTInitInfo->pstUARTPeriph)
    {
        RCC_APB2PeriphClockCmd(uiAHBAPBPeriphClockCtrl, DISABLE) ;
    }
    else
    {
        RCC_APB1PeriphClockCmd(uiAHBAPBPeriphClockCtrl, DISABLE) ;
    }

    /* ##############################################################################################
     * STAGE 3 : UART控制设备撤销
     * ##############################################################################################
     */
    /* 删除等待信号量 */
#ifdef DRV_UART_USE_UCOS_III
    OSSemDel((OS_SEM  *)&pstUARTDevice->stSemTxDone, 
             (OS_OPT   )OS_OPT_DEL_ALWAYS, 
             (OS_ERR  *)&enErr) ;
#endif
    pstUARTDevice->ucOpenFlag      = (~(DRV_UART_OPEN_FLAG)) ;

    return UART_ERROR_OK ;
}


/*******************************************************************************
- Function    : DRV_UART_Write
- Description : 本函数执行UART的写入操作。
- Input       : pstUARTDevice : 
                    UART设备指针. 
                pucData :
                    写入数据缓存.
                uiSize :
                    写入数据量。函数不会对该参数做检查，但是过大的数据量可能
                    无法写入缓存.
                uiMode :
                    指定写入模式.为以下内容的相或.
                        DRV_UART_WRITE_MODE_NONE      : 无模式指定。数据拷贝到设
                                                        备缓存后函数会立即返回;
                        DRV_UART_WRITE_MODE_WAIT_DONE : 等待传输完成。数据完全经
                                                        过UART发送完毕之后函数才
                                                        会返回;
- Output      : NULL
- Return      : DRV_UART_ERROR_CODE_E : 
                    错误码.
                    UART_ERROR_OK : 操作成功.
                    其他值           : 查看相关定义.
- Others      : 
*******************************************************************************/
DRV_UART_ERROR_CODE_E DRV_UART_Write(DRV_UART_DEVICE_S *pstUARTDevice, UCHAR *pucData, UINT uiSize, UINT uiMode)
{
    UINT   uiDestBufFreeSize ;
    UINT   uiCopySize ;
#ifdef DRV_UART_USE_UCOS_III
    OS_ERR enErr ;
#endif

    /* 参数检查 */
    if(NULL == pstUARTDevice)
    {
        return UART_ERROR_WRITE_UART_DEVICE_P_NULL ;
    }

    if(NULL == pucData)
    {
        return UART_ERROR_WRITE_BUF_P_NULL ;
    }

    if(DRV_UART_OPEN_FLAG != pstUARTDevice->ucOpenFlag)
    {
        return UART_ERROR_WRITE_UART_NOT_OPENED ;
    }

    if((DRV_UART_WRITE_MODE_NONE != uiMode) & (~DRV_UART_WRITE_MODE_WAIT_DONE))
    {
        return UART_ERROR_WRITE_MODE_ERROR ;
    }

    /* 计算目的缓存剩余空间 */
    uiDestBufFreeSize = __CalcBufFreeSize(pstUARTDevice->uiTXBufSize, pstUARTDevice->usTXBufWrPtr, pstUARTDevice->usTXBufRdPtr) ;

    /* 计算空间是否足够拷贝 */
    if(uiDestBufFreeSize < uiSize)
    {
        /* 空间不足，返回错误 */
        return UART_ERROR_WRITE_BUF_NO_SPACE ;
    }
    else
    {
        /* 将数据拷贝到缓存 */
        /* 此处需要考虑的细节：
         * 1、如果RdPtr >= WrPtr，则直接从WrPtr开始拷贝所有的数据即可；
         * 2、如果RdPtr <  WrPtr，则需要从WrPtr开始拷贝数据，直到缓存上限；然后从0开始拷贝剩余的数据。前提是有那么多得数据需要拷贝
         */
        /* BEGIN : 修改：取消if条件中读写指针相等的可能性判断；相等时，走else分支 */
        /* if(pstUARTDevice->usTXBufRdPtr >= pstUARTDevice->usTXBufWrPtr) */
        if(pstUARTDevice->usTXBufRdPtr > pstUARTDevice->usTXBufWrPtr)
        /* END   : 修改：取消if条件中读写指针相等的可能性判断；相等时，走else分支 */
        {
            memcpy(pstUARTDevice->pucTXBuf + pstUARTDevice->usTXBufWrPtr, pucData, uiSize) ;
            pstUARTDevice->usTXBufWrPtr += uiSize ;
            
            if(pstUARTDevice->usTXBufWrPtr == pstUARTDevice->uiTXBufSize)
            {
                pstUARTDevice->usTXBufWrPtr = 0 ;
            }
        }
        else
        {
            uiCopySize = ((pstUARTDevice->uiTXBufSize - pstUARTDevice->usTXBufWrPtr) >= uiSize) ? uiSize : (pstUARTDevice->uiTXBufSize - pstUARTDevice->usTXBufWrPtr) ;
            memcpy((pstUARTDevice->pucTXBuf) + pstUARTDevice->usTXBufWrPtr, pucData, uiCopySize) ;

            pstUARTDevice->usTXBufWrPtr += uiCopySize ;
            
            if(pstUARTDevice->usTXBufWrPtr == pstUARTDevice->uiTXBufSize)
            {
                pstUARTDevice->usTXBufWrPtr = 0 ;
            }

            if(uiCopySize !=  uiSize)
            {
                /* 第二次拷贝 */
                memcpy((pstUARTDevice->pucTXBuf + pstUARTDevice->usTXBufWrPtr), pucData + uiCopySize, uiSize - uiCopySize) ;
                pstUARTDevice->usTXBufWrPtr += (uiSize - uiCopySize) ;
            }
        }
    }
    
#ifndef DRV_UART_USE_UCOS_III
    pstUARTDevice->ucTXDone = 0 ;
#endif

    /* 使能发送中断，开启发送 */
    USART_ITConfig(pstUARTDevice->pstUARTPeriph, USART_IT_TXE,  ENABLE) ;

    /* 根据标记位，判断是立即返回还是等待全部传输完成再返回 */
    if((DRV_UART_WRITE_MODE_WAIT_DONE == uiMode) & DRV_UART_WRITE_MODE_WAIT_DONE)
    {
        /* 等待传输完成 */
#ifndef DRV_UART_USE_UCOS_III
        /* 阻塞等待传输完成 */
        while(0 == pstUARTDevice->ucTXDone) ;
#else
        /* 通过等待信号量，非阻塞等待传输完成 */
        OSSemPend((OS_SEM   *)&(pstUARTDevice->stSemTxDone), 
                  (OS_TICK   )0, 
                  (OS_OPT    )OS_OPT_PEND_BLOCKING, 
                  (CPU_TS   *)NULL, 
                  (OS_ERR   *)&enErr) ;
#endif
    }

    return UART_ERROR_OK ;
}


/*******************************************************************************
- Function    : DRV_UART_Read
- Description : 本函数执行UART的读取操作。
- Input       : pstUARTDevice : 
                    UART设备指针. 
                pucData       :
                    读取数据缓存.
                uiSize        :
                    写入数据量.
                puiReadSize   :
                    返回实际读取的数据量.
                uiMode :
                    指定读取模式.为以下内容的相或.
                        DRV_UART_READ_MODE_NONE : 不指定任何模式;
- Output      : NULL
- Return      : DRV_UART_ERROR_CODE_E : 
                    错误码.
                    UART_ERROR_OK : 操作成功.
                    其他值           : 查看相关定义.
- Others      : 
*******************************************************************************/
DRV_UART_ERROR_CODE_E DRV_UART_Read(DRV_UART_DEVICE_S *pstUARTDevice, UCHAR *pucData, UINT uiSize, UINT *puiReadSize, UINT uiMode)
{
    UINT uiDestBufDataSize ;
    UINT uiCopySize ;
    UINT ui2ndCopySize ;

    /* 参数检查 */
    if(NULL == pstUARTDevice)
    {
        return UART_ERROR_READ_UART_DEVICE_P_NULL ;
    }

    if(NULL == pucData)
    {
        return UART_ERROR_READ_BUF_P_NULL ;
    }

    if(NULL == puiReadSize)
    {
        return UART_ERROR_READ_READ_SIZE_P_NULL ;
    }

    if(DRV_UART_OPEN_FLAG != pstUARTDevice->ucOpenFlag)
    {
        return UART_ERROR_READ_UART_NOT_OPENED ;
    }

    if(DRV_UART_WRITE_MODE_NONE != uiMode)
    {
        return UART_ERROR_READ_MODE_ERROR ;
    }

    /* 计算有效数据量 */
    uiDestBufDataSize = __CalcBufUsedSize(pstUARTDevice->uiRXBufSize, pstUARTDevice->usRXBufWrPtr, pstUARTDevice->usRXBufRdPtr) ;

    uiCopySize = (uiDestBufDataSize >= uiSize) ? uiSize : uiDestBufDataSize ;

    /* 复制数据 */
    /* 如果WrPtr大于或者等于RdPtr，则直接从RtPtr开始复制相应数据即可。
     * 如果RdPtr大于WrPtr，根据数据量情况，可能要2次复制。第一次从RdPtr开始，复制到缓存末尾；
     * 第二次从缓存头部开始，复制剩余的数据量。
     */
	if(pstUARTDevice->usRXBufWrPtr >= pstUARTDevice->usRXBufRdPtr)
	{
		memcpy(pucData, pstUARTDevice->pucRXBuf + pstUARTDevice->usRXBufRdPtr, uiCopySize) ;
		pstUARTDevice->usRXBufRdPtr += uiCopySize ;
		
		if(pstUARTDevice->usRXBufRdPtr == pstUARTDevice->uiRXBufSize)
		{
			pstUARTDevice->usRXBufRdPtr = 0 ;
		}    
	}
	else
	{
		ui2ndCopySize = ((pstUARTDevice->uiRXBufSize - pstUARTDevice->usRXBufRdPtr) >= uiCopySize) ? uiCopySize : (pstUARTDevice->uiRXBufSize - pstUARTDevice->usRXBufRdPtr) ;
		memcpy(pucData, pstUARTDevice->pucRXBuf + pstUARTDevice->usRXBufRdPtr, ui2ndCopySize) ;

		pstUARTDevice->usRXBufRdPtr += ui2ndCopySize ;
		/* BEGIN : 仅仅考虑了数据源buf的指针变化，还需要考虑拷贝目的buf的指针变化。 */
		pucData                     += ui2ndCopySize ;
		/* END   : 仅仅考虑了数据源buf的指针变化，还需要考虑拷贝目的buf的指针变化。 */
		
		if(pstUARTDevice->usRXBufRdPtr == pstUARTDevice->uiRXBufSize)
		{
			pstUARTDevice->usRXBufRdPtr = 0 ;
		}

		if(ui2ndCopySize !=  uiCopySize)
		{
			/* 第二次拷贝 */
			memcpy(pucData, pstUARTDevice->pucRXBuf + pstUARTDevice->usRXBufRdPtr, uiCopySize - ui2ndCopySize) ;
			/* BEGIN : 明显的逻辑错误。指针应该增加第二次拷贝的内容长度。 */
			/* pstUARTDevice->usRXBufRdPtr += (uiSize - uiCopySize) ; */
			pstUARTDevice->usRXBufRdPtr += (uiCopySize - ui2ndCopySize) ;
			/* END   : 明显的逻辑错误。指针应该增加第二次拷贝的内容长度。 */
		}
	}

    *puiReadSize = uiCopySize ;

    return UART_ERROR_OK ;
}


/*******************************************************************************
- Function    : DRV_UART_GeneralIntHandler
- Description : 本函数是UART设备中断处理函数的一般形式.函数响应UART的RXNE中断和
                TXE中断.对于TXE中断,函数判断发送缓存是否为0,如果不为0,则继续发送
                数据,如果为0,则停止发送,并置位传输完成标记.
                对于RXNE中断,函数判断接收缓存是否有空间,如果有空间,则将数据写入
                到缓冲.如果没有空间,则丢弃数据.
                创建并使用UART设备时,调用者应定义新的中断处理函数(不带参数的),
                并在中断处理函数中调用本函数,指定入口参数(指定UART器件).
- Input       : pstUARTDevice : 
                    UART设备指针. 
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
__INLINE VOID DRV_UART_GeneralIntHandler(DRV_UART_DEVICE_S *pstUARTDevice)
{
#ifdef DRV_UART_USE_UCOS_III
    OS_ERR enErr ;
#endif
    UCHAR  ucRxTmp ;
    
    if(SET == USART_GetITStatus(pstUARTDevice->pstUARTPeriph, USART_IT_TXE))
    {
        /* 检查是否还有数据需要发送 */
        if(0 != __CalcBufUsedSize(pstUARTDevice->uiTXBufSize, pstUARTDevice->usTXBufWrPtr, pstUARTDevice->usTXBufRdPtr))
        {
            /* 继续发送数据 */
            USART_SendData(pstUARTDevice->pstUARTPeriph, *(pstUARTDevice->pucTXBuf + pstUARTDevice->usTXBufRdPtr)) ;
            
            pstUARTDevice->usTXBufRdPtr++ ;
            
            if(pstUARTDevice->usTXBufRdPtr == pstUARTDevice->uiTXBufSize)
            {
                pstUARTDevice->usTXBufRdPtr = 0 ;
            }
        }
        else
        {
            /* 发送完成，禁止TXE中断，发出发送完成信号 */
            USART_ITConfig(pstUARTDevice->pstUARTPeriph, USART_IT_TXE, DISABLE) ;
#ifndef DRV_UART_USE_UCOS_III
            pstUARTDevice->ucTXDone = 1 ;
#else
        /* 通过等待信号量，非阻塞等待传输完成 */
        OSSemPost((OS_SEM   *)&(pstUARTDevice->stSemTxDone), 
                  (OS_OPT    )OS_OPT_POST_1, 
                  (OS_ERR   *)&enErr) ;
#endif
        }
    }
    else if(SET == USART_GetITStatus(pstUARTDevice->pstUARTPeriph, USART_IT_RXNE))
    {
        /* 接收数据 */
        ucRxTmp = USART_ReceiveData(pstUARTDevice->pstUARTPeriph) ;

        if(0 != __CalcBufFreeSize(pstUARTDevice->uiRXBufSize, pstUARTDevice->usRXBufWrPtr, pstUARTDevice->usRXBufRdPtr))
        {
            *(pstUARTDevice->pucRXBuf + pstUARTDevice->usRXBufWrPtr) = ucRxTmp ;

            pstUARTDevice->usRXBufWrPtr++ ;

            if(pstUARTDevice->uiRXBufSize == pstUARTDevice->usRXBufWrPtr)
            {
                pstUARTDevice->usRXBufWrPtr = 0 ;
            }
        }
    }

    return ;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file drv_uart_stm32f4xx.c. *******/  
