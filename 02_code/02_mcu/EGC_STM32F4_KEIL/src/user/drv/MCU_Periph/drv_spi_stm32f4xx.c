/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : drv_spi_stm32f4xx.c
    Project name : STM32F4XX版本SPI外设驱动。
    Module name  : 
    Date created : 2020年4月16日   16时40分51秒
    Author       : Ning.JianLi
    Description  : STM32F4XX版本SPI外设驱动源文件。
*******************************************************************************/

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "drv_spi_stm32f4xx.h"

/*******************************************************************************
- Function    : __DRV_SPI_TailLeadZeros
- Description : 本函数检查一个输入数据,从最低位到最高位,遇到的第一个bit '1'之前,
                一共有多少个bit '0'.
- Input       : uiData : 要检查的数据.
- Output      : NULL 
- Return      : UCHAR  : 从低位到高位,第一个出现的bit '1'之前,一共出现的0的个数.
- Others      : 特殊情况 : 如果uiData为0,则返回为0.
*******************************************************************************/
UCHAR __DRV_SPI_TailLeadZeros(UINT uiData)
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
- Function    : DRV_SPI_Open
- Description : 本函数创建一个基于STM32F4XX的SPI外设的SPI设备.函数流程主要包含如
                下几个步骤:
                1. 配置SPI的GPIO;
                2. 配置SPI外设;
                3. 如果使能了DMA,配置DMA的发送和接收的流.
                4. 创建SPI设备.
- Input       : pstSPIInitInfo : 
                    配置信息,包括GPIO配置信息,SPI外设配置信息,在DMA使能的情况下,
                    还包含DMA配置信息.
- Output      : pstSPIDevice : 
                    SPI设备指针. 
- Return      : DRV_SPI_ERROR_CODE_E : 
                    错误码.
                    SPI_ERROR_OK : 创建成功.
                    其他值          : 查看相关定义.
- Others      : 
*******************************************************************************/
DRV_SPI_ERROR_CODE_E DRV_SPI_Open(DRV_SPI_DEVICE_S *pstSPIDevice, DRV_SPI_INIT_INFO_S *pstSPIInitInfo)
{
    UINT             uiAHBAPBPeriphClockCtrl = 0 ;
    GPIO_InitTypeDef stGPIOInitInfo ;
    SPI_InitTypeDef  stSPIInitInfo ;
#ifdef DRV_SPI_USE_DMA 
    DMA_InitTypeDef  stSPIDMACfgInfo ;
#endif
#ifdef DRV_SPI_USE_UCOS_III
    OS_ERR           enErr ;
#endif

    /* 参数检查 */
    if(NULL == pstSPIDevice)
    {
        return SPI_ERROR_OPEN_SPI_DEVICE_PTR_NULL ;
    }

    if(NULL == pstSPIInitInfo)
    {
        return SPI_ERROR_OPEN_CONFIG_INFO_PTR_NULL ;
    }

    if(0 == IS_GPIO_ALL_PERIPH(pstSPIInitInfo->pstSCKGPIOGroup))
    {
        return SPI_ERROR_OPEN_SCK_PIN_GROUP_ERROR ;
    }
    
    if(0 == IS_GPIO_ALL_PERIPH(pstSPIInitInfo->pstNSSGPIOGroup))
    {
        return SPI_ERROR_OPEN_NSS_PIN_GROUP_ERROR ;
    }

    if(0 == IS_GPIO_ALL_PERIPH(pstSPIInitInfo->pstMISOGPIOGroup))
    {
        return SPI_ERROR_OPEN_MISO_PIN_GROUP_ERROR ;
    }

    if(0 == IS_GPIO_ALL_PERIPH(pstSPIInitInfo->pstMOSIGPIOGroup))
    {
        return SPI_ERROR_OPEN_MOSI_PIN_GROUP_ERROR ;
    }

    if(0 == IS_GPIO_PIN(pstSPIInitInfo->uiSCKPinID))
    {
        return SPI_ERROR_OPEN_SCK_PIN_ERROR ;
    }

    if(0 == IS_GPIO_PIN(pstSPIInitInfo->uiNSSPinID))
    {
        return SPI_ERROR_OPEN_NSS_PIN_ERROR ;
    }

    if(0 == IS_GPIO_PIN(pstSPIInitInfo->uiMOSIPinID))
    {
        return SPI_ERROR_OPEN_MOSI_PIN_ERROR ;
    }

    if(0 == IS_GPIO_PIN(pstSPIInitInfo->uiMISOPinID))
    {
        return SPI_ERROR_OPEN_MISO_PIN_ERROR ;
    }

    if(0 == IS_GPIO_AF(pstSPIInitInfo->ucSCKPinAFDef))
    {
        return SPI_ERROR_OPEN_SCK_AF_ERROR ;
    }

    if(0 == IS_GPIO_AF(pstSPIInitInfo->ucMOSIPinAFDef))
    {
        return SPI_ERROR_OPEN_MOSI_AF_ERROR ;
    }

    if(0 == IS_GPIO_AF(pstSPIInitInfo->ucMISOPinAFDef))
    {
        return SPI_ERROR_OPEN_MISO_AF_ERROR ;
    }

    if(0 == IS_SPI_ALL_PERIPH(pstSPIInitInfo->pstSPIPeriph))
    {
        return SPI_ERROR_OPEN_INVALID_SPI_PERIPH ;
    }

    if(0 == IS_SPI_BAUDRATE_PRESCALER(pstSPIInitInfo->usSPIPrescaler))
    {
        return SPI_ERROR_OPEN_INVALID_SPI_PRESCALER ;
    }

    if(DRV_SPI_MODE_0 != pstSPIInitInfo->ucSPIMode && 
       DRV_SPI_MODE_1 != pstSPIInitInfo->ucSPIMode && 
       DRV_SPI_MODE_2 != pstSPIInitInfo->ucSPIMode && 
       DRV_SPI_MODE_3 != pstSPIInitInfo->ucSPIMode)
    {
        return SPI_ERROR_OPEN_INVALID_SPI_MODE ;
    }
       
#ifdef DRV_SPI_USE_DMA
/* 20210830No01 : BEGIN : 需要在使能DMA功能的情况下判断以下参数。 */
    if(BOOL_TRUE == pstSPIInitInfo->bUseDMA)
    {
/* 20210830No01 : END   : 需要在使能DMA功能的情况下判断以下参数。 */
        if(0 == IS_DMA_ALL_PERIPH(pstSPIInitInfo->pstSPIDMARxStream))
        {
            return SPI_ERROR_OPEN_INVALID_DMA_RX_STREAM ;
        }

        if(0 == IS_DMA_ALL_PERIPH(pstSPIInitInfo->pstSPIDMATxStream))
        {
            return SPI_ERROR_OPEN_INVALID_DMA_TX_STREAM ;
        }

        if(0 == IS_DMA_CHANNEL(pstSPIInitInfo->uiSPIDMARxChannel))
        {
            return SPI_ERROR_OPEN_INVALID_DMA_RX_CHANNEL ;
        }

        if(0 == IS_DMA_CHANNEL(pstSPIInitInfo->uiSPIDMATxChannel))
        {
	        return SPI_ERROR_OPEN_INVALID_DMA_TX_CHANNEL ;
        }
/* 20210830No01 : BEGIN : 需要在使能DMA功能的情况下判断以上参数。 */
    }
/* 20210830No01 : END   : 需要在使能DMA功能的情况下判断以上参数。 */
#endif

    /* ##############################################################################################
     * STAGE 1 : GPIO 配置
     * ##############################################################################################
     */
    /* 使能GPIO时钟 */
    switch((UINT)pstSPIInitInfo->pstSCKGPIOGroup)
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

    switch((UINT)pstSPIInitInfo->pstNSSGPIOGroup)
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
    
    switch((UINT)pstSPIInitInfo->pstMOSIGPIOGroup)
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

    switch((UINT)pstSPIInitInfo->pstMISOGPIOGroup)
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

    /* 引脚信息配置 */ /* uiXXXPinID在底层配置函数有检查，这里不检查 */
    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_AF ;
    stGPIOInitInfo.GPIO_Speed = GPIO_High_Speed ;
    stGPIOInitInfo.GPIO_OType = GPIO_OType_PP ;
    stGPIOInitInfo.GPIO_PuPd  = GPIO_PuPd_NOPULL ;

    stGPIOInitInfo.GPIO_Pin = pstSPIInitInfo->uiSCKPinID ;
    GPIO_Init(pstSPIInitInfo->pstSCKGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin = pstSPIInitInfo->uiMISOPinID ;
    GPIO_Init(pstSPIInitInfo->pstMISOGPIOGroup, &stGPIOInitInfo) ;
    
    stGPIOInitInfo.GPIO_Pin = pstSPIInitInfo->uiMOSIPinID ;
    GPIO_Init(pstSPIInitInfo->pstMOSIGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Mode = GPIO_Mode_OUT ;
    stGPIOInitInfo.GPIO_Pin  = pstSPIInitInfo->uiNSSPinID ;
    GPIO_Init(pstSPIInitInfo->pstNSSGPIOGroup, &stGPIOInitInfo) ;

    /* 复用功能配置 */
    GPIO_PinAFConfig(pstSPIInitInfo->pstSCKGPIOGroup, 
                     __DRV_SPI_TailLeadZeros(pstSPIInitInfo->uiSCKPinID),  
                     pstSPIInitInfo->ucSCKPinAFDef) ;
    GPIO_PinAFConfig(pstSPIInitInfo->pstMISOGPIOGroup, 
                     __DRV_SPI_TailLeadZeros(pstSPIInitInfo->uiMISOPinID), 
                     pstSPIInitInfo->ucMISOPinAFDef) ;
    GPIO_PinAFConfig(pstSPIInitInfo->pstMOSIGPIOGroup, 
                     __DRV_SPI_TailLeadZeros(pstSPIInitInfo->uiMOSIPinID), 
                     pstSPIInitInfo->ucMOSIPinAFDef) ;

    /* 拉高NSS */
    GPIO_SetBits(pstSPIInitInfo->pstNSSGPIOGroup, pstSPIInitInfo->uiNSSPinID) ;

    /* ##############################################################################################
     * STAGE 2 : SPI 配置
     * ##############################################################################################
     */
    /* 使能SPI时钟 */
    switch((UINT)pstSPIInitInfo->pstSPIPeriph)
    {
        case (UINT)SPI1 :
            uiAHBAPBPeriphClockCtrl = RCC_APB2Periph_SPI1 ;
            break ;
        case (UINT)SPI2 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_SPI2 ;
            break ;
        case (UINT)SPI3 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_SPI3 ;
            break ;
        case (UINT)SPI4 :
            uiAHBAPBPeriphClockCtrl = RCC_APB2Periph_SPI4 ;
            break ;
        case (UINT)SPI5 :
            uiAHBAPBPeriphClockCtrl = RCC_APB2Periph_SPI5 ;
            break ;
        case (UINT)SPI6 :
            uiAHBAPBPeriphClockCtrl = RCC_APB2Periph_SPI6 ;
            break ;
        default :
            return SPI_ERROR_OPEN_INVALID_SPI_PERIPH ;
    }
    
    if(SPI2 == pstSPIInitInfo->pstSPIPeriph || SPI3 == pstSPIInitInfo->pstSPIPeriph)
    {
        RCC_APB1PeriphClockCmd(uiAHBAPBPeriphClockCtrl, ENABLE) ;
    }
    else
    {
        RCC_APB2PeriphClockCmd(uiAHBAPBPeriphClockCtrl, ENABLE) ;
    }

    /* SPI配置 */
    stSPIInitInfo.SPI_Direction         = SPI_Direction_2Lines_FullDuplex ;
    stSPIInitInfo.SPI_Mode              = SPI_Mode_Master ;
    stSPIInitInfo.SPI_DataSize          = SPI_DataSize_8b ;

    if(0 == pstSPIInitInfo->ucSPIMode || 1 == pstSPIInitInfo->ucSPIMode)
    {
        stSPIInitInfo.SPI_CPOL              = SPI_CPOL_Low ;
    }
    else
    {
        stSPIInitInfo.SPI_CPOL              = SPI_CPOL_High ;
    }

    if(0 == pstSPIInitInfo->ucSPIMode || 2 == pstSPIInitInfo->ucSPIMode)
    {
        stSPIInitInfo.SPI_CPHA              = SPI_CPHA_1Edge ;
    }
    else
    {
        stSPIInitInfo.SPI_CPHA              = SPI_CPHA_2Edge ;
    }

    stSPIInitInfo.SPI_NSS               = SPI_NSS_Soft ;
    stSPIInitInfo.SPI_BaudRatePrescaler = pstSPIInitInfo->usSPIPrescaler ;
    stSPIInitInfo.SPI_FirstBit          = SPI_FirstBit_MSB ;

    SPI_Init(pstSPIInitInfo->pstSPIPeriph, &stSPIInitInfo) ;

    /* 禁止所有中断 */
    SPI_I2S_ITConfig(pstSPIInitInfo->pstSPIPeriph, SPI_I2S_IT_TXE,  DISABLE) ;
    SPI_I2S_ITConfig(pstSPIInitInfo->pstSPIPeriph, SPI_I2S_IT_RXNE, DISABLE) ;
    SPI_I2S_ITConfig(pstSPIInitInfo->pstSPIPeriph, SPI_I2S_IT_ERR,  DISABLE) ;

    /* 使能NVIC中相应SPI中断 */
    NVIC_EnableIRQ((IRQn_Type)pstSPIInitInfo->uiSPIIrqNumber) ;

    /* 使能SPI */
    SPI_Cmd(pstSPIInitInfo->pstSPIPeriph, ENABLE) ;

    /* ##############################################################################################
     * STAGE 3 : SPI DMA 配置
     * ##############################################################################################
     */
#ifdef DRV_SPI_USE_DMA
    if(BOOL_TRUE == pstSPIInitInfo->bUseDMA)
    {
        /* 使能DMA时钟 */
        switch((UINT)pstSPIInitInfo->pstSPIPeriph)
        {
            case (UINT)SPI1 : /* SPI1使用DMA2 */
                uiAHBAPBPeriphClockCtrl = RCC_AHB1Periph_DMA2 ;
                break ;
            case (UINT)SPI2 : /* SPI2使用DMA1 */
                uiAHBAPBPeriphClockCtrl = RCC_AHB1Periph_DMA1 ;
                break ;
            case (UINT)SPI3 : /* SPI3使用DMA1 */
                uiAHBAPBPeriphClockCtrl = RCC_AHB1Periph_DMA1 ;
                break ;
            case (UINT)SPI4 : /* SPI4使用DMA2 */
                uiAHBAPBPeriphClockCtrl = RCC_AHB1Periph_DMA2 ;
                break ;
            case (UINT)SPI5 : /* SPI5使用DMA2 */
                uiAHBAPBPeriphClockCtrl = RCC_AHB1Periph_DMA2 ;
                break ;
            case (UINT)SPI6 : /* SPI6使用DMA2 */
                uiAHBAPBPeriphClockCtrl = RCC_AHB1Periph_DMA2 ;
                break ;
            default :
                break ;
        }

        RCC_AHB1PeriphClockCmd(uiAHBAPBPeriphClockCtrl, ENABLE) ;
        DMA_DeInit(pstSPIInitInfo->pstSPIDMARxStream) ;
        DMA_DeInit(pstSPIInitInfo->pstSPIDMATxStream) ;

        stSPIDMACfgInfo.DMA_Channel            = pstSPIInitInfo->uiSPIDMARxChannel ;         /* DMA SPI RX通道配置 */
        stSPIDMACfgInfo.DMA_PeripheralBaseAddr = (UINT)&(pstSPIInitInfo->pstSPIPeriph->DR) ; /* SPI的数据寄存器作为源 */
        stSPIDMACfgInfo.DMA_Memory0BaseAddr    = 0x0 ;                                       /* 内存作为目的，此值无效 */
        stSPIDMACfgInfo.DMA_DIR                = DMA_DIR_PeripheralToMemory ;                /* 方向从外设到内存 */
        stSPIDMACfgInfo.DMA_BufferSize         = 0x0 ;                                       /* 内存大小，此值无效 */
        stSPIDMACfgInfo.DMA_PeripheralInc      = DMA_PeripheralInc_Disable ;                 /* 外设地址不增加 */
        stSPIDMACfgInfo.DMA_MemoryInc          = DMA_MemoryInc_Enable ;                      /* 内存地址增加 */
        stSPIDMACfgInfo.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte ;               /* 外设数据宽度为1B */
        stSPIDMACfgInfo.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte ;                   /* 内存数据宽度为1B */
        stSPIDMACfgInfo.DMA_Mode               = DMA_Mode_Normal ;                           /* 单次模式 */
        stSPIDMACfgInfo.DMA_Priority           = DMA_Priority_High ;                         /* 优先级高 */
        stSPIDMACfgInfo.DMA_FIFOMode           = DMA_FIFOMode_Disable ;                      /* 不使用FIFO模式 */
        stSPIDMACfgInfo.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull ;                /* 无效 */
        stSPIDMACfgInfo.DMA_MemoryBurst        = DMA_MemoryBurst_Single ;                    /* 每次1B */
        stSPIDMACfgInfo.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single ;                /* 每次1B */

        /* 接收通道的配置 */
        DMA_Init(pstSPIInitInfo->pstSPIDMARxStream, &stSPIDMACfgInfo) ;

        stSPIDMACfgInfo.DMA_Channel            = pstSPIInitInfo->uiSPIDMATxChannel ;         /* DMA SPI TX通道配置 */
        stSPIDMACfgInfo.DMA_PeripheralBaseAddr = (UINT)&(pstSPIInitInfo->pstSPIPeriph->DR) ; /* SPI的数据寄存器作为目的 */
        stSPIDMACfgInfo.DMA_Memory0BaseAddr    = 0x0 ;                                       /* 内存作为源，此值无效 */
        stSPIDMACfgInfo.DMA_DIR                = DMA_DIR_MemoryToPeripheral ;                /* 方向从内存到外设 */
        stSPIDMACfgInfo.DMA_BufferSize         = 0x0 ;                                       /* 内存大小，此值无效 */
        stSPIDMACfgInfo.DMA_PeripheralInc      = DMA_PeripheralInc_Disable ;                 /* 外设地址不增加 */
        stSPIDMACfgInfo.DMA_MemoryInc          = DMA_MemoryInc_Enable ;                      /* 内存地址增加 */
        stSPIDMACfgInfo.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte ;               /* 外设数据宽度为1B */
        stSPIDMACfgInfo.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte ;                   /* 内存数据宽度为1B */
        stSPIDMACfgInfo.DMA_Mode               = DMA_Mode_Normal ;                           /* 单次模式 */
        stSPIDMACfgInfo.DMA_Priority           = DMA_Priority_High ;                         /* 优先级高 */
        stSPIDMACfgInfo.DMA_FIFOMode           = DMA_FIFOMode_Disable ;                      /* 不使用FIFO模式 */
        stSPIDMACfgInfo.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull ;                /* 无效 */
        stSPIDMACfgInfo.DMA_MemoryBurst        = DMA_MemoryBurst_Single ;                    /* 每次1B */
        stSPIDMACfgInfo.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single ;                /* 每次1B */

        /*发送通道配置 */
        DMA_Init(pstSPIInitInfo->pstSPIDMATxStream, &stSPIDMACfgInfo) ;

        /* 关闭DMA各种中断源 */
        DMA_ITConfig(pstSPIInitInfo->pstSPIDMARxStream, DMA_IT_TC | DMA_IT_HT | DMA_IT_TE | DMA_IT_FE, DISABLE) ;
        DMA_ITConfig(pstSPIInitInfo->pstSPIDMATxStream, DMA_IT_TC | DMA_IT_HT | DMA_IT_TE | DMA_IT_FE, DISABLE) ;

        /* 使能两条流的中断，此时还没有使能传输中断，因此不会触发中断 */
        NVIC_EnableIRQ((IRQn_Type)pstSPIInitInfo->uiSPIDMARxIrqNum) ;
        NVIC_EnableIRQ((IRQn_Type)pstSPIInitInfo->uiSPIDMATxIrqNum) ;
    }
#endif

    /* ##############################################################################################
     * STAGE 4 : 创建SPI控制设备
     * ##############################################################################################
     */
    pstSPIDevice->pstNSSGPIOGroup = pstSPIInitInfo->pstNSSGPIOGroup ;
    pstSPIDevice->uiNSSPinID      = pstSPIInitInfo->uiNSSPinID ;
    pstSPIDevice->ucSPIStart      = 0 ;
    pstSPIDevice->pstSPIPeriph    = pstSPIInitInfo->pstSPIPeriph ;
#ifndef DRV_SPI_USE_UCOS_III
    pstSPIDevice->ucOperationDone = 0 ;
#else
    OSSemCreate((OS_SEM      *)&pstSPIDevice->stSemOperationDone, 
                (CPU_CHAR    *)"SEM_SPI_DEVICE", 
                (OS_SEM_CTR   )0, 
                (OS_ERR      *)&enErr) ;
#endif

#ifdef DRV_SPI_USE_DMA
    pstSPIDevice->bUseDMA = pstSPIInitInfo->bUseDMA ;

    if(BOOL_TRUE == pstSPIInitInfo->bUseDMA)
    {
        pstSPIDevice->pstSPIDMATxStream    = pstSPIInitInfo->pstSPIDMATxStream ;    /* 指定TX DMA使用的流 */
        pstSPIDevice->pstSPIDMARxStream    = pstSPIInitInfo->pstSPIDMARxStream ;    /* 指定RX DMA使用的流 */
        
        switch((UINT)pstSPIDevice->pstSPIDMATxStream)
        {
            case (UINT)DMA1_Stream0 :
            case (UINT)DMA2_Stream0 :
                pstSPIDevice->uiSPIDMATxStreamTCIF = DMA_IT_TCIF0 ;
                break ;
            case (UINT)DMA1_Stream1 :
            case (UINT)DMA2_Stream1 :
                pstSPIDevice->uiSPIDMATxStreamTCIF = DMA_IT_TCIF1 ;
                break ;
            case (UINT)DMA1_Stream2 :
            case (UINT)DMA2_Stream2 :
                pstSPIDevice->uiSPIDMATxStreamTCIF = DMA_IT_TCIF2 ;
                break ;
            case (UINT)DMA1_Stream3 :
            case (UINT)DMA2_Stream3 :
                pstSPIDevice->uiSPIDMATxStreamTCIF = DMA_IT_TCIF3 ;
                break ;
            case (UINT)DMA1_Stream4 :
            case (UINT)DMA2_Stream4 :
                pstSPIDevice->uiSPIDMATxStreamTCIF = DMA_IT_TCIF4 ;
                break ;
            case (UINT)DMA1_Stream5 :
            case (UINT)DMA2_Stream5 :
                pstSPIDevice->uiSPIDMATxStreamTCIF = DMA_IT_TCIF5 ;
                break ;
            case (UINT)DMA1_Stream6 :
            case (UINT)DMA2_Stream6 :
                pstSPIDevice->uiSPIDMATxStreamTCIF = DMA_IT_TCIF6 ;
                break ;
            case (UINT)DMA1_Stream7 :
            case (UINT)DMA2_Stream7 :
                pstSPIDevice->uiSPIDMATxStreamTCIF = DMA_IT_TCIF7 ;
                break ;
            default :
                break ;
        }

        switch((UINT)pstSPIDevice->pstSPIDMARxStream)
        {
            case (UINT)DMA1_Stream0 :
            case (UINT)DMA2_Stream0 :
                pstSPIDevice->uiSPIDMARxStreamTCIF = DMA_IT_TCIF0 ;
                break ;
            case (UINT)DMA1_Stream1 :
            case (UINT)DMA2_Stream1 :
                pstSPIDevice->uiSPIDMARxStreamTCIF = DMA_IT_TCIF1 ;
                break ;
            case (UINT)DMA1_Stream2 :
            case (UINT)DMA2_Stream2 :
                pstSPIDevice->uiSPIDMARxStreamTCIF = DMA_IT_TCIF2 ;
                break ;
            case (UINT)DMA1_Stream3 :
            case (UINT)DMA2_Stream3 :
                pstSPIDevice->uiSPIDMARxStreamTCIF = DMA_IT_TCIF3 ;
                break ;
            case (UINT)DMA1_Stream4 :
            case (UINT)DMA2_Stream4 :
                pstSPIDevice->uiSPIDMARxStreamTCIF = DMA_IT_TCIF4 ;
                break ;
            case (UINT)DMA1_Stream5 :
            case (UINT)DMA2_Stream5 :
                pstSPIDevice->uiSPIDMARxStreamTCIF = DMA_IT_TCIF5 ;
                break ;
            case (UINT)DMA1_Stream6 :
            case (UINT)DMA2_Stream6 :
                pstSPIDevice->uiSPIDMARxStreamTCIF = DMA_IT_TCIF6 ;
                break ;
            case (UINT)DMA1_Stream7 :
            case (UINT)DMA2_Stream7 :
                pstSPIDevice->uiSPIDMARxStreamTCIF = DMA_IT_TCIF7 ;
                break ;
            default :
                break ;
        }
    }
#endif

    pstSPIDevice->ucOpenFlag           = DRV_SPI_OPEN_FLAG ;

    return SPI_ERROR_OK ;
}

/*******************************************************************************
- Function    : DRV_SPI_Close
- Description : 本函数关闭一个基于STM32F4XX的SPI外设的SPI设备.
- Input       : pstSPIInitInfo : 
                    配置信息,包括GPIO配置信息,SPI外设配置信息,在DMA使能的情况下,
                    还包含DMA配置信息.
- Output      : pstSPIDevice : 
                    SPI设备指针. 
- Return      : DRV_SPI_ERROR_CODE_E : 
                    错误码.
                    SPI_ERROR_OK : 操作成功.
                    其他值          : 查看相关定义.
- Others      : 
*******************************************************************************/
DRV_SPI_ERROR_CODE_E DRV_SPI_Close(DRV_SPI_DEVICE_S *pstSPIDevice, DRV_SPI_INIT_INFO_S *pstSPIInitInfo)
{
    UINT             uiAHBAPBPeriphClockCtrl = 0 ;
    GPIO_InitTypeDef stGPIOInitInfo ;
#ifdef DRV_SPI_USE_UCOS_III
    OS_ERR           enErr ;
#endif

    /* 参数检查 */
    if(NULL == pstSPIDevice)
    {
        return SPI_ERROR_CLOSE_SPI_DEVICE_PTR_NULL ;
    }

    if(NULL == pstSPIInitInfo)
    {
        return SPI_ERROR_CLOSE_CONFIG_INFO_PTR_NULL ;
    }

    if(0 == IS_GPIO_ALL_PERIPH(pstSPIInitInfo->pstSCKGPIOGroup))
    {
        return SPI_ERROR_CLOSE_SCK_PIN_GROUP_ERROR ;
    }
    
    if(0 == IS_GPIO_ALL_PERIPH(pstSPIInitInfo->pstNSSGPIOGroup))
    {
        return SPI_ERROR_CLOSE_NSS_PIN_GROUP_ERROR ;
    }

    if(0 == IS_GPIO_ALL_PERIPH(pstSPIInitInfo->pstMISOGPIOGroup))
    {
        return SPI_ERROR_CLOSE_MISO_PIN_GROUP_ERROR ;
    }

    if(0 == IS_GPIO_ALL_PERIPH(pstSPIInitInfo->pstMOSIGPIOGroup))
    {
        return SPI_ERROR_CLOSE_MOSI_PIN_GROUP_ERROR ;
    }

    if(0 == IS_GPIO_PIN(pstSPIInitInfo->uiSCKPinID))
    {
        return SPI_ERROR_CLOSE_SCK_PIN_ERROR ;
    }

    if(0 == IS_GPIO_PIN(pstSPIInitInfo->uiNSSPinID))
    {
        return SPI_ERROR_CLOSE_NSS_PIN_ERROR ;
    }

    if(0 == IS_GPIO_PIN(pstSPIInitInfo->uiMOSIPinID))
    {
        return SPI_ERROR_CLOSE_MOSI_PIN_ERROR ;
    }

    if(0 == IS_GPIO_PIN(pstSPIInitInfo->uiMISOPinID))
    {
        return SPI_ERROR_CLOSE_MISO_PIN_ERROR ;
    }

    if(0 == IS_SPI_ALL_PERIPH(pstSPIInitInfo->pstSPIPeriph))
    {
        return SPI_ERROR_CLOSE_INVALID_SPI_PERIPH ;
    }

#ifdef DRV_SPI_USE_DMA
/* 20210830No01 : BEGIN : 需要在使能DMA功能的情况下判断以下参数。 */
    if(BOOL_TRUE == pstSPIInitInfo->bUseDMA)
    {
/* 20210830No01 : END   : 需要在使能DMA功能的情况下判断以下参数。 */
        if(0 == IS_DMA_ALL_PERIPH(pstSPIInitInfo->pstSPIDMARxStream))
        {
            return SPI_ERROR_CLOSE_INVALID_DMA_RX_STREAM ;
        }

        if(0 == IS_DMA_ALL_PERIPH(pstSPIInitInfo->pstSPIDMATxStream))
        {
            return SPI_ERROR_CLOSE_INVALID_DMA_TX_STREAM ;
        }
/* 20210830No01 : BEGIN : 需要在使能DMA功能的情况下判断以上参数。 */
    }
/* 20210830No01 : END   : 需要在使能DMA功能的情况下判断以上参数。 */
#endif

    if(DRV_SPI_OPEN_FLAG == pstSPIDevice->ucOpenFlag)
    {
        return SPI_ERROR_CLOSE_SPI_NOT_OPENED ;
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

    stGPIOInitInfo.GPIO_Pin = pstSPIInitInfo->uiSCKPinID ;
    GPIO_Init(pstSPIInitInfo->pstSCKGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin = pstSPIInitInfo->uiMISOPinID ;
    GPIO_Init(pstSPIInitInfo->pstMISOGPIOGroup, &stGPIOInitInfo) ;
    
    stGPIOInitInfo.GPIO_Pin = pstSPIInitInfo->uiMOSIPinID ;
    GPIO_Init(pstSPIInitInfo->pstMOSIGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin  = pstSPIInitInfo->uiNSSPinID ;
    GPIO_Init(pstSPIInitInfo->pstNSSGPIOGroup, &stGPIOInitInfo) ;

    /* ##############################################################################################
     * STAGE 2 : SPI 配置撤销
     * ##############################################################################################
     */
    /* 禁止所有中断 */
    SPI_I2S_ITConfig(pstSPIInitInfo->pstSPIPeriph, SPI_I2S_IT_TXE,  DISABLE) ;
    SPI_I2S_ITConfig(pstSPIInitInfo->pstSPIPeriph, SPI_I2S_IT_RXNE, DISABLE) ;
    SPI_I2S_ITConfig(pstSPIInitInfo->pstSPIPeriph, SPI_I2S_IT_ERR,  DISABLE) ;

    /* 禁止NVIC中相应SPI中断 */
    NVIC_DisableIRQ((IRQn_Type)pstSPIInitInfo->uiSPIIrqNumber) ;

    /* 禁止SPI */
    SPI_Cmd(pstSPIInitInfo->pstSPIPeriph, DISABLE) ;

    /* 禁止SPI时钟 */
    switch((UINT)pstSPIInitInfo->pstSPIPeriph)
    {
        case (UINT)SPI1 :
            uiAHBAPBPeriphClockCtrl = RCC_APB2Periph_SPI1 ;
            break ;
        case (UINT)SPI2 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_SPI2 ;
            break ;
        case (UINT)SPI3 :
            uiAHBAPBPeriphClockCtrl = RCC_APB1Periph_SPI3 ;
            break ;
        case (UINT)SPI4 :
            uiAHBAPBPeriphClockCtrl = RCC_APB2Periph_SPI4 ;
            break ;
        case (UINT)SPI5 :
            uiAHBAPBPeriphClockCtrl = RCC_APB2Periph_SPI5 ;
            break ;
        case (UINT)SPI6 :
            uiAHBAPBPeriphClockCtrl = RCC_APB2Periph_SPI6 ;
            break ;
        default :
            return SPI_ERROR_OPEN_INVALID_SPI_PERIPH ;
    }
    
    if(SPI2 == pstSPIInitInfo->pstSPIPeriph || SPI3 == pstSPIInitInfo->pstSPIPeriph)
    {
        RCC_APB1PeriphClockCmd(uiAHBAPBPeriphClockCtrl, DISABLE) ;
    }
    else
    {
        RCC_APB2PeriphClockCmd(uiAHBAPBPeriphClockCtrl, DISABLE) ;
    }
    /* ##############################################################################################
     * STAGE 3 : SPI DMA 配置
     * ##############################################################################################
     */
#ifdef DRV_SPI_USE_DMA
    DMA_DeInit(pstSPIInitInfo->pstSPIDMARxStream) ;
    DMA_DeInit(pstSPIInitInfo->pstSPIDMATxStream) ;

    /* 关闭DMA各种中断源 */
    DMA_ITConfig(pstSPIInitInfo->pstSPIDMARxStream, DMA_IT_TC | DMA_IT_HT | DMA_IT_TE | DMA_IT_FE, DISABLE) ;
    DMA_ITConfig(pstSPIInitInfo->pstSPIDMATxStream, DMA_IT_TC | DMA_IT_HT | DMA_IT_TE | DMA_IT_FE, DISABLE) ;

    /* 使能两条流的中断，此时还没有使能传输中断，因此不会触发中断 */
    NVIC_DisableIRQ((IRQn_Type)pstSPIInitInfo->uiSPIDMARxIrqNum) ;
    NVIC_DisableIRQ((IRQn_Type)pstSPIInitInfo->uiSPIDMARxIrqNum) ;
#endif

    /* ##############################################################################################
     * STAGE 4 : SPI控制设备撤销
     * ##############################################################################################
     */
    /* 删除等待信号量 */
#ifdef DRV_SPI_USE_UCOS_III
    OSSemDel((OS_SEM  *)&pstSPIDevice->stSemOperationDone, 
             (OS_OPT   )OS_OPT_DEL_ALWAYS, 
             (OS_ERR  *)&enErr) ;
#endif
    pstSPIDevice->ucOpenFlag      = (~(DRV_SPI_OPEN_FLAG)) ;

    return SPI_ERROR_OK ;
}


/*******************************************************************************
- Function    : __DRV_SPI_Start
- Description : 本函数启动SPI传输.函数通过拉低NSS信号来启动DMA传输.
- Input       : pstSPIDevice : 
                    SPI设备指针. 
- Output      : NULL
- Return      : DRV_SPI_ERROR_CODE_E : 
                    错误码.
                    SPI_ERROR_OK : 操作成功.
                    其他值          : 查看相关定义.
- Others      : 
*******************************************************************************/
DRV_SPI_ERROR_CODE_E __DRV_SPI_Start(DRV_SPI_DEVICE_S *pstSPIDevice)
{
    /* 拉低NSS引脚，启动传输 */
    if(DRV_SPI_OPEN_FLAG != pstSPIDevice->ucOpenFlag)
    {
        return SPI_ERROR_START_SPI_NOT_OPENED ;
    }

    /* 拉低NSS以启动传输 */
    GPIO_ResetBits(pstSPIDevice->pstNSSGPIOGroup, pstSPIDevice->uiNSSPinID) ;

    /* 记录SPI启动状态 */
    pstSPIDevice->ucSPIStart = 1 ;

    return SPI_ERROR_OK ;
}

/*******************************************************************************
- Function    : __DRV_SPI_Stop
- Description : 本函数启动SPI传输.函数通过拉高NSS信号来停止DMA传输.
- Input       : pstSPIDevice : 
                    SPI设备指针. 
- Output      : NULL
- Return      : DRV_SPI_ERROR_CODE_E : 
                    错误码.
                    SPI_ERROR_OK : 操作成功.
                    其他值          : 查看相关定义.
- Others      : 
*******************************************************************************/
DRV_SPI_ERROR_CODE_E __DRV_SPI_Stop(DRV_SPI_DEVICE_S *pstSPIDevice)
{
    /* 拉低NSS引脚，启动传输 */
    if(DRV_SPI_OPEN_FLAG != pstSPIDevice->ucOpenFlag)
    {
        return SPI_ERROR_STOP_SPI_NOT_OPENED ;
    }

    if(0 == pstSPIDevice->ucSPIStart)
    {
        return SPI_ERROR_STOP_ALREADY_STOP ;
    }

    /* 拉高NSS以停止传输 */
    GPIO_SetBits(pstSPIDevice->pstNSSGPIOGroup, pstSPIDevice->uiNSSPinID) ;

    /* 记录SPI启动状态 */
    pstSPIDevice->ucSPIStart = 0 ;
    
    return SPI_ERROR_OK ;
}


/*******************************************************************************
- Function    : DRV_SPI_Write
- Description : 本函数执行SPI的写入操作。函数根据写入模式，支持三种写入：
                    1. 查询写入.此种情况下不使用SPI的中断,通过查询每个字节是否
                       写入完毕,来进行写入,直到写入结束.此种情况下写入是阻塞的.
                       适合于少量数据写入,例如几个字节.
                    2. 中断写入.此种情况下使用SPI的中断,中断中将会置位传输完成
                       标记(简单变量或者信号量,取决于是否定义使用UCOSIII).
                       如果不使能UCOSIII,则写入操作是阻塞的,函数将等待传输完
                       成标记,然后退出.
                       如果使能UCOSIII,则写入操作是非阻塞的,在信号量等待期间,
                       系统可以进行任务调度.
                       适合于中等数据量写入,或者在不能应用DMA的情况下,进行大数
                       据量的写入,例如几十字节到几千字节.
                    3. DMA写入.此种情况下使用DMA及其中断,中断中将会置位传输完成
                       标记(简单变量或者信号量,取决于是否定义使用UCOSIII).
                       如果不使能UCOSIII,则写入操作是阻塞的,函数将等待传输完
                       成标记,然后退出.
                       如果使能UCOSIII,则写入操作是非阻塞的,在信号量等待期间,
                       系统可以进行任务调度.
                       适合于中等或者较大数据量写入,例如几百字节到几千字节.
- Input       : pstSPIDevice : 
                    SPI设备指针. 
                pucWriteBuf :
                    写入数据缓存.
                pucBackBuf :
                    返回数据的缓存.
                uiSize :
                    写入数据量。必须小于或者等于65535.
                uiMode :
                    指定写入模式.为以下内容的相或.
                        DRV_SPI_READ_WRITE_MODE_QUERY : 查询方式写入;
                        DRV_SPI_READ_WRITE_MODE_INT   : 中断方式写入;
                        DRV_SPI_READ_WRITE_MODE_DMA   : DMA方式写入(需要使能DMA);以上3项3选1
                        DRV_SPI_READ_WRITE_WITH_START : 写入前产生Start,即拉低NSS
                        DRV_SPI_READ_WRITE_WITH_STOP  : 写入后产生Stop,即拉高NSS
                        DRV_SPI_WRITE_STORE_BACK_DATA : 写入时保存返回的数据
- Output      : NULL
- Return      : DRV_SPI_ERROR_CODE_E : 
                    错误码.
                    SPI_ERROR_OK : 操作成功.
                    其他值          : 查看相关定义.
- Others      : 
*******************************************************************************/
DRV_SPI_ERROR_CODE_E DRV_SPI_Write(DRV_SPI_DEVICE_S *pstSPIDevice, UCHAR *pucWriteBuf, UCHAR *pucBackBuf, UINT uiSize, UINT uiMode)
{
    UINT                 uiLoop ;
    DRV_SPI_ERROR_CODE_E enRetVal ;
#ifdef DRV_SPI_USE_UCOS_III
    OS_ERR enErr ;
#endif

    /* 参数检查 */
    if(NULL == pstSPIDevice)
    {
        return SPI_ERROR_WRITE_SPI_DEVICE_P_NULL ;
    }

    if(NULL == pucWriteBuf)
    {
        return SPI_ERROR_WRITE_BUF_P_NULL ;
    }

    if(NULL == pucBackBuf && (DRV_SPI_WRITE_STORE_BACK_DATA == (uiMode & DRV_SPI_WRITE_STORE_BACK_DATA)))
    {
        return SPI_ERROR_WRITE_BACK_BUF_P_NULL ;
    }
#ifdef DRV_SPI_USE_DMA
    if(DRV_SPI_WRITE_STORE_BACK_DATA == (uiMode & DRV_SPI_WRITE_STORE_BACK_DATA) && DRV_SPI_READ_WRITE_MODE_DMA == (uiMode & DRV_SPI_READ_WRITE_MODE_DMA))
    {
        return SPI_ERROR_WRITE_RECORD_BACK_DATA_WHEN_DMA ;
    }
#endif
    if(DRV_SPI_OPEN_FLAG != pstSPIDevice->ucOpenFlag)
    {
        return SPI_ERROR_WRITE_SPI_NOT_OPENED ;
    }

    /* 如果需要发送传输起始，则发送 */
    if(DRV_SPI_READ_WRITE_WITH_START == (uiMode & DRV_SPI_READ_WRITE_WITH_START))
    {
        enRetVal = __DRV_SPI_Start(pstSPIDevice) ;

        if(SPI_ERROR_OK != enRetVal)
        {
            return enRetVal ;
        }
    }

    /* 检查是否有RXNE，如果有则读取数据，消除RXNE状态 */
    if(SET == SPI_I2S_GetFlagStatus(pstSPIDevice->pstSPIPeriph, SPI_I2S_FLAG_RXNE))
    {
        SPI_I2S_ReceiveData(pstSPIDevice->pstSPIPeriph) ;
    }

    /* 记录模式 */
    pstSPIDevice->uiMode = uiMode ;

    /* 根据不同的工作模式进行写入 */
    if(DRV_SPI_READ_WRITE_MODE_QUERY == (uiMode & DRV_SPI_READ_WRITE_MODE_QUERY))
    {
        /* 非中断模式写入，适合小数据量 */
        for(uiLoop = 0 ; uiLoop < uiSize ; uiLoop++)
        {
            /* 发送数据 */
            SPI_I2S_SendData(pstSPIDevice->pstSPIPeriph, pucWriteBuf[uiLoop]) ;

            /* 等待传输完成 */
            while(SET != SPI_I2S_GetFlagStatus(pstSPIDevice->pstSPIPeriph, SPI_I2S_FLAG_RXNE)) ;
            
            /* 读取数据 */
            if(DRV_SPI_WRITE_STORE_BACK_DATA == (uiMode & DRV_SPI_WRITE_STORE_BACK_DATA))
            {
                *pucBackBuf = SPI_I2S_ReceiveData(pstSPIDevice->pstSPIPeriph) ;
                pucBackBuf++ ;
            }
            else
            {
                SPI_I2S_ReceiveData(pstSPIDevice->pstSPIPeriph) ;
            }
        }
    }
    else if(DRV_SPI_READ_WRITE_MODE_INT == (uiMode & DRV_SPI_READ_WRITE_MODE_INT))
    {
        /* 中断方式传输，适合中等数据量 */
        /* 传输标记 */
        pstSPIDevice->enCurrentOperation = SPI_CURRENT_OPERATION_WR ;
        pstSPIDevice->pucWriteBuf        = pucWriteBuf + 1 ; /* 第一个数据在本函数中发送，因此中断中从第二个开始发送 */
        pstSPIDevice->pucBackBuf         = pucBackBuf ;
        pstSPIDevice->uiSize             = uiSize ;
#ifndef DRV_SPI_USE_UCOS_III
        pstSPIDevice->ucOperationDone    = 0 ;
#endif

        /* 发送第一个数据以启动传输 */
        SPI_I2S_SendData(pstSPIDevice->pstSPIPeriph, pucWriteBuf[0]) ;
        
        /* 数据通过中断传输，使能RXNE中断 */ /* 先发送数据，RXNE状态会一直在，因此不必担心中断丢失 */
        SPI_I2S_ITConfig(pstSPIDevice->pstSPIPeriph, SPI_I2S_IT_RXNE, ENABLE) ;

#ifndef DRV_SPI_USE_UCOS_III
        /* 阻塞等待传输完成 */
        while(0 == pstSPIDevice->ucOperationDone) ;
#else
        /* 通过等待信号量，非阻塞等待传输完成 */
        OSSemPend((OS_SEM   *)&(pstSPIDevice->stSemOperationDone), 
                  (OS_TICK   )0, 
                  (OS_OPT    )OS_OPT_PEND_BLOCKING, 
                  (CPU_TS   *)NULL, 
                  (OS_ERR   *)&enErr) ;
#endif
        /* 写入结束，关中断 */
        SPI_I2S_ITConfig(pstSPIDevice->pstSPIPeriph, SPI_I2S_IT_RXNE, DISABLE) ;
    }
#ifdef DRV_SPI_USE_DMA
    else if(DRV_SPI_READ_WRITE_MODE_DMA == (uiMode & DRV_SPI_READ_WRITE_MODE_DMA))
    {
        if(BOOL_TRUE == pstSPIDevice->bUseDMA)
        {
            /* DMA方式传输 */
            pstSPIDevice->enCurrentOperation = SPI_CURRENT_OPERATION_WR ;
            pstSPIDevice->pucWriteBuf        = pucWriteBuf ;
            pstSPIDevice->uiSize             = uiSize ;
#ifndef DRV_SPI_USE_UCOS_III
            pstSPIDevice->ucOperationDone    = 0 ;
#endif
            
            /* 设置传输的起始地址设置 */
            DMA_MemoryTargetConfig(pstSPIDevice->pstSPIDMATxStream, (UINT)(pstSPIDevice->pucWriteBuf), DMA_Memory_0) ;

            /* 传输数据总量，DMA一次最多65535 */
            DMA_SetCurrDataCounter(pstSPIDevice->pstSPIDMATxStream, (pstSPIDevice->uiSize > 65535)? 65535 : (pstSPIDevice->uiSize)) ;

            /* 注意以下2句的先后顺序 */
            pstSPIDevice->pucWriteBuf += ((pstSPIDevice->uiSize > 65535)? 65535 : (pstSPIDevice->uiSize)) ;
            pstSPIDevice->uiSize      -= ((pstSPIDevice->uiSize > 65535)? 65535 : (pstSPIDevice->uiSize)) ;

            /* 允许SPI向DMA引擎发出TX请求 */
            SPI_I2S_DMACmd(pstSPIDevice->pstSPIPeriph, SPI_I2S_DMAReq_Tx, ENABLE) ;

            /* 使能传输完成中断 */
            DMA_ITConfig(pstSPIDevice->pstSPIDMATxStream, DMA_IT_TC, ENABLE) ;

            /* 开启DMA传输。注意SPI的TXE在空闲时为高，因此可以直接启动DMA传输 */
            DMA_Cmd(pstSPIDevice->pstSPIDMATxStream, ENABLE) ;

#ifndef DRV_SPI_USE_UCOS_III
            /* 阻塞等待传输完成 */
            while(0 == pstSPIDevice->ucOperationDone) ;
#else
            /* 通过等待信号量，非阻塞等待传输完成 */
            OSSemPend((OS_SEM   *)&(pstSPIDevice->stSemOperationDone), 
                      (OS_TICK   )0, 
                      (OS_OPT    )OS_OPT_PEND_BLOCKING, 
                      (CPU_TS   *)NULL, 
                      (OS_ERR   *)&enErr) ; 
#endif
        }
        else
        {
            return SPI_ERROR_WRITE_DMA_NOT_CONFIGED ;
        }
    }
#endif

    /* 如果传输完成后需要发送STOP，则发送 */
    if(DRV_SPI_READ_WRITE_WITH_STOP == (uiMode & DRV_SPI_READ_WRITE_WITH_STOP))
    {
        enRetVal = __DRV_SPI_Stop(pstSPIDevice) ;

        if(SPI_ERROR_OK != enRetVal)
        {
            return enRetVal ;
        }
    }

    return SPI_ERROR_OK ;
}

/*******************************************************************************
- Function    : DRV_SPI_Read
- Description : 本函数执行SPI的读取操作。函数根据读取模式，支持三种读取：
                    1. 查询读取.此种情况下不使用SPI的中断,通过查询每个字节是否
                       读取完毕,来进行读取,直到读取结束.此种情况下读取是阻塞的.
                       适合于少量数据读取,例如几个字节.
                    2. 中断读取.此种情况下使用SPI的中断,中断中将会置位传输完成
                       标记(简单变量或者信号量,取决于是否定义使用UCOSIII).
                       如果不使能UCOSIII,则读取操作是阻塞的,函数将等待传输完
                       成标记,然后退出.
                       如果使能UCOSIII,则读取操作是非阻塞的,在信号量等待期间,
                       系统可以进行任务调度.
                       适合于中等数据量读取,或者在不能应用DMA的情况下,进行大数
                       据量的读取,例如几十字节到几千字节.
                    3. DMA读取.此种情况下使用DMA及其中断,中断中将会置位传输完成
                       标记(简单变量或者信号量,取决于是否定义使用UCOSIII).
                       如果不使能UCOSIII,则读取操作是阻塞的,函数将等待传输完
                       成标记,然后退出.
                       如果使能UCOSIII,则读取操作是非阻塞的,在信号量等待期间,
                       系统可以进行任务调度.
                       适合于中等或者较大数据量读取,例如几百字节到几千字节.
- Input       : pstSPIDevice : 
                    SPI设备指针. 
                pucWriteBuf :
                    写入数据缓存.
                uiSize :
                    写入数据量。必须小于或者等于65535.
                uiMode :
                    指定写入模式.为以下内容的相或.
                        DRV_SPI_READ_WRITE_MODE_QUERY : 查询方式读取;
                        DRV_SPI_READ_WRITE_MODE_INT   : 中断方式读取;
                        DRV_SPI_READ_WRITE_MODE_DMA   : DMA方式读取(需要使能DMA);以上3项3选1
                        DRV_SPI_READ_WRITE_WITH_START : 读取前产生Start,即拉低NSS
                        DRV_SPI_READ_WRITE_WITH_STOP  : 读取后产生Stop,即拉高NSS
- Output      : NULL
- Return      : DRV_SPI_ERROR_CODE_E : 
                    错误码.
                    SPI_ERROR_OK : 操作成功.
                    其他值          : 查看相关定义.
- Others      : 
*******************************************************************************/
DRV_SPI_ERROR_CODE_E DRV_SPI_Read(DRV_SPI_DEVICE_S *pstSPIDevice, UCHAR *pucReadBuf, UINT uiSize, UINT uiMode)
{
    UINT                 uiLoop ;
    DRV_SPI_ERROR_CODE_E enRetVal ;
#ifdef DRV_SPI_USE_UCOS_III
    OS_ERR enErr ;
#endif

    /* 参数检查 */
    if(0 == pstSPIDevice)
    {
        return SPI_ERROR_READ_SPI_DEVICE_P_NULL ;
    }

    if(0 == pucReadBuf)
    {
        return SPI_ERROR_READ_BUF_P_NULL ;
    }

    if(DRV_SPI_OPEN_FLAG != pstSPIDevice->ucOpenFlag)
    {
        return SPI_ERROR_READ_SPI_NOT_OPENED ;
    }

    /* 如果需要发送传输起始，则发送 */
    if(DRV_SPI_READ_WRITE_WITH_START == (uiMode & DRV_SPI_READ_WRITE_WITH_START))
    {
        enRetVal = __DRV_SPI_Start(pstSPIDevice) ;

        if(SPI_ERROR_OK != enRetVal)
        {
            return enRetVal ;
        }
    }
    
    /* 检查是否有RXNE，如果有则读取数据，消除RXNE状态 */
    if(SET == SPI_I2S_GetFlagStatus(pstSPIDevice->pstSPIPeriph, SPI_I2S_FLAG_RXNE))
    {
        SPI_I2S_ReceiveData(pstSPIDevice->pstSPIPeriph) ;
    }
    
    /* 根据不同的工作模式进行读取 */
    if(DRV_SPI_READ_WRITE_MODE_QUERY == (uiMode & DRV_SPI_READ_WRITE_MODE_QUERY))
    {
        for(uiLoop = 0 ; uiLoop < uiSize ; uiLoop++)
        {
            /* 发送数据 */
            SPI_I2S_SendData(pstSPIDevice->pstSPIPeriph, 0x00) ;

            /* 等待传输完成 */
            while(SET != SPI_I2S_GetFlagStatus(pstSPIDevice->pstSPIPeriph, SPI_I2S_FLAG_RXNE)) ;
            
            /* 读取数据 */
            pucReadBuf[uiLoop] = SPI_I2S_ReceiveData(pstSPIDevice->pstSPIPeriph) ;
        }
    }
    else if(DRV_SPI_READ_WRITE_MODE_INT == (uiMode & DRV_SPI_READ_WRITE_MODE_INT))
    {
        /* 非DMA方式传输 */
        /* 传输标记 */
        pstSPIDevice->enCurrentOperation = SPI_CURRENT_OPERATION_RD ;
        pstSPIDevice->pucReadBuf         = pucReadBuf ;
        pstSPIDevice->uiSize             = uiSize ;
#ifndef DRV_SPI_USE_UCOS_III
        pstSPIDevice->ucOperationDone    = 0 ;
#endif

        /* 数据通过中断传输，使能RXNE中断 */
        SPI_I2S_ITConfig(pstSPIDevice->pstSPIPeriph, SPI_I2S_IT_RXNE, ENABLE) ;
        
        /* 发送第一个数据以启动传输 */
        SPI_I2S_SendData(pstSPIDevice->pstSPIPeriph, 0x00) ;

#ifndef DRV_SPI_USE_UCOS_III
        /* 阻塞等待传输完成 */
        while(0 == pstSPIDevice->ucOperationDone) ;
#else
        /* 通过等待信号量，非阻塞等待传输完成 */
        OSSemPend((OS_SEM   *)&(pstSPIDevice->stSemOperationDone), 
                  (OS_TICK   )0, 
                  (OS_OPT    )OS_OPT_PEND_BLOCKING, 
                  (CPU_TS   *)NULL, 
                  (OS_ERR   *)&enErr) ;
#endif
        /* 读取结束，关中断 */
        SPI_I2S_ITConfig(pstSPIDevice->pstSPIPeriph, SPI_I2S_IT_RXNE, DISABLE) ;
    }
#ifdef DRV_SPI_USE_DMA
    else if(DRV_SPI_READ_WRITE_MODE_DMA == (uiMode & DRV_SPI_READ_WRITE_MODE_DMA))
    {
        if(BOOL_TRUE == pstSPIDevice->bUseDMA)
        {
            /* DMA方式传输 */
            /* 传输标记 */
            pstSPIDevice->enCurrentOperation = SPI_CURRENT_OPERATION_RD ;
            pstSPIDevice->pucReadBuf         = pucReadBuf ;
            pstSPIDevice->uiSize             = uiSize ;
#ifndef DRV_SPI_USE_UCOS_III
            pstSPIDevice->ucOperationDone    = 0 ;
#endif
            
            /* 在发送通道，因为数据不关心，所以随便填写值 */
            DMA_MemoryTargetConfig(pstSPIDevice->pstSPIDMATxStream, (UINT)&uiLoop, DMA_Memory_0) ;

            /* 清除MINC位，这样每次发送时内存地址不增加，防止出现意外 */
            pstSPIDevice->pstSPIDMATxStream->CR &= (~(DMA_MemoryInc_Enable)) ;

            /* 发送通道传输数据总量 */
            DMA_SetCurrDataCounter(pstSPIDevice->pstSPIDMATxStream, (pstSPIDevice->uiSize > 65535)? 65535 : (pstSPIDevice->uiSize)) ;

            /* 允许SPI向DMA引擎发出TX请求 */
            SPI_I2S_DMACmd(pstSPIDevice->pstSPIPeriph, SPI_I2S_DMAReq_Tx, ENABLE) ;

            /* 在接收通道，传输的起始地址设置为Rx Buffer */
            DMA_MemoryTargetConfig(pstSPIDevice->pstSPIDMARxStream, (UINT)pstSPIDevice->pucReadBuf, DMA_Memory_0) ;

            /* 接收通道传输数据总量 */
            DMA_SetCurrDataCounter(pstSPIDevice->pstSPIDMARxStream, (pstSPIDevice->uiSize > 65535)? 65535 : (pstSPIDevice->uiSize)) ;

            /* 注意以下2句的先后顺序 */
            pstSPIDevice->pucReadBuf += ((pstSPIDevice->uiSize > 65535)? 65535 : (pstSPIDevice->uiSize)) ;
            pstSPIDevice->uiSize     -= ((pstSPIDevice->uiSize > 65535)? 65535 : (pstSPIDevice->uiSize)) ;

            /* 允许SPI向DMA引擎发出RX请求 */
            SPI_I2S_DMACmd(pstSPIDevice->pstSPIPeriph, SPI_I2S_DMAReq_Rx, ENABLE) ;

            /* 使能传输完成中断 */
            DMA_ITConfig(pstSPIDevice->pstSPIDMARxStream, DMA_IT_TC, ENABLE) ;

            /* 清除可能存在的RXNE标记 */
            SPI_I2S_ReceiveData(pstSPIDevice->pstSPIPeriph) ;

            /* 注意要先启动RX DMA的传输再启动TX DMA的传输。实际上TX传输发生在前，这样避免RX错过。 */
            DMA_Cmd(pstSPIDevice->pstSPIDMARxStream, ENABLE) ;
            DMA_Cmd(pstSPIDevice->pstSPIDMATxStream, ENABLE) ;

#ifndef DRV_SPI_USE_UCOS_III
            /* 阻塞等待传输完成 */
            while(0 == pstSPIDevice->ucOperationDone) ;
#else
            /* 通过等待信号量，非阻塞等待传输完成 */
            OSSemPend((OS_SEM   *)&(pstSPIDevice->stSemOperationDone), 
                      (OS_TICK   )0, 
                      (OS_OPT    )OS_OPT_PEND_BLOCKING, 
                      (CPU_TS   *)NULL, 
                      (OS_ERR   *)&enErr) ;
#endif
            /* 恢复MINC位，避免发送出错 */
            pstSPIDevice->pstSPIDMATxStream->CR |= DMA_MemoryInc_Enable ;
        }
        else
        {
            return SPI_ERROR_READ_DMA_NOT_CONFIGED ;
        }
    }
#endif

    /* 如果传输完成后需要发送STOP，则发送 */
    if(DRV_SPI_READ_WRITE_WITH_STOP == (uiMode & DRV_SPI_READ_WRITE_WITH_STOP))
    {
        enRetVal = __DRV_SPI_Stop(pstSPIDevice) ;

        if(SPI_ERROR_OK != enRetVal)
        {
            return enRetVal ;
        }
    }

    return SPI_ERROR_OK ;    
}

/*******************************************************************************
- Function    : DRV_SPI_GeneralIntHandler
- Description : 本函数是SPI设备中断处理函数的一般形式.函数只响应SPI的RXNE中断.
                对于写入模式,函数判断写入数据量是否为0,如果不为0,则继续发送数据,
                如果为0,则停止发送,并置位传输完成标记.
                对于读取模式,函数判断读取数据量是否为0,如果不为0,则发送数据以接
                收新的数据;如果为0,则停止发送和接收,并置位传输完成标记.
                创建并使用SPI设备时,调用者应定义新的中断处理函数(不带参数的),
                并在中断处理函数中调用本函数,指定入口参数(指定SPI器件).
- Input       : pstSPIDevice : 
                    SPI设备指针. 
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
__INLINE VOID DRV_SPI_GeneralIntHandler(DRV_SPI_DEVICE_S *pstSPIDevice)
{
#ifdef DRV_SPI_USE_UCOS_III
    OS_ERR enErr ;
#endif
    UCHAR  ucTmp ;
    
    if(1 == SPI_I2S_GetITStatus(pstSPIDevice->pstSPIPeriph, SPI_I2S_IT_RXNE))
    {
        ucTmp = SPI_I2S_ReceiveData(pstSPIDevice->pstSPIPeriph) ;
        
        if(SPI_CURRENT_OPERATION_WR == pstSPIDevice->enCurrentOperation)
        {
            /* 保存数据 */
            if(DRV_SPI_WRITE_STORE_BACK_DATA == ((pstSPIDevice->uiMode) & DRV_SPI_WRITE_STORE_BACK_DATA))
            {
                *pstSPIDevice->pucBackBuf = ucTmp ;
                pstSPIDevice->pucBackBuf++ ;
            }

            /* 减少发送数据量 */
            pstSPIDevice->uiSize-- ;

            /* 判断是否还有更多数据要发送 */
            if(0 != pstSPIDevice->uiSize)
            {
                SPI_I2S_SendData(pstSPIDevice->pstSPIPeriph, *pstSPIDevice->pucWriteBuf) ;
                
                pstSPIDevice->pucWriteBuf++ ;
            }
            else
            {
#ifndef DRV_SPI_USE_UCOS_III
                /* 置位传输完成标记 */
                pstSPIDevice->ucOperationDone = 1 ;
#else
                /* 释放信号量，表示SPI传输完成 */
                OSSemPost((OS_SEM  *)&(pstSPIDevice->stSemOperationDone), 
                          (OS_OPT   )OS_OPT_POST_1, 
                          (OS_ERR  *)&enErr) ;
#endif
            }
        }
        else
        {
            /* 读模式下，保存接收的数据 */
            *(pstSPIDevice->pucReadBuf) = ucTmp ;
            (pstSPIDevice->pucReadBuf)++ ;
                
            /* 减少接收数据量 */
            (pstSPIDevice->uiSize)-- ;

            /* 判断是否还有更多数据要发送 */
            if(0 != pstSPIDevice->uiSize)
            {
                SPI_I2S_SendData(pstSPIDevice->pstSPIPeriph, 0x00) ;
            }
            else
            {
#ifndef DRV_SPI_USE_UCOS_III
                /* 置位传输完成标记 */
                pstSPIDevice->ucOperationDone = 1 ;
#else
                /* 释放信号量，表示SPI传输完成 */
                OSSemPost((OS_SEM  *)&(pstSPIDevice->stSemOperationDone), 
                          (OS_OPT   )OS_OPT_POST_1, 
                          (OS_ERR  *)&enErr) ;
#endif
            }
        }
    }

    return ;
}

#ifdef DRV_SPI_USE_DMA
/*******************************************************************************
- Function    : DRV_SPI_GeneralDMAIntHandler
- Description : 本函数为当使用DMA方式时,DMA传输的中断处理函数.
                当SPI应用DMA方式传输时,会应用到Tx流和Rx流,对应着2个中断处理函
                数.本函数归一化处理2个中断,因此当创建并使用SPI设备时,调用者定义
                一个新的中断处理函数(不带参数的),在中断处理函数中调用本函数,指
                定入口参数(SPI器件);同时,将该新函数同时指向2个流的中断服务.
                对于SPI发送,只涉及到SPI的Tx流,并且使能了该中断;对于SPI接收,需要
                同时涉及SPI的Tx流和SPI的Rx流,仅使能Rx中断.因此,函数中根据发生哪
                个中断,来判断当前操作是发送还是接收.
                对于读取,将首先清除中断标志,然后判断剩余数据是否为0.如果为0,则
                读取完成,禁止中断,同时置位传输完成标记.如果不为0,则继续读取数据.
                对于写入,将首先清除中断标志,然后判断剩余数据是否为0.如果为0,则
                写入完成,禁止中断,同时置位传输完成标记.如果不为0,则继续写入数据.
- Input       : pstSPIDevice : 
                    SPI设备指针. 
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
__INLINE VOID DRV_SPI_GeneralDMAIntHandler(DRV_SPI_DEVICE_S *pstSPIDevice)
{
#ifdef DRV_SPI_USE_UCOS_III
    OS_ERR enErr ;
#endif

    /* 判断中断类型 */
    if(SET == DMA_GetITStatus(pstSPIDevice->pstSPIDMARxStream, pstSPIDevice->uiSPIDMARxStreamTCIF))
    {
        /* 清除两条流的中断标记。注意TX完成中断虽然没有使能，但是该完成标记会置位 */
        DMA_ClearITPendingBit(pstSPIDevice->pstSPIDMATxStream, pstSPIDevice->uiSPIDMATxStreamTCIF) ;
        DMA_ClearITPendingBit(pstSPIDevice->pstSPIDMARxStream, pstSPIDevice->uiSPIDMARxStreamTCIF) ;
        
        if(0 == pstSPIDevice->uiSize)
        {
            /* 禁止SPI向DMA引擎发起RX和TX的DMA请求 */
            SPI_I2S_DMACmd(pstSPIDevice->pstSPIPeriph, SPI_I2S_DMAReq_Tx, DISABLE) ;
            SPI_I2S_DMACmd(pstSPIDevice->pstSPIPeriph, SPI_I2S_DMAReq_Rx, DISABLE) ;

            /* 禁止RX流的传输完成中断 */
            DMA_ITConfig(pstSPIDevice->pstSPIDMARxStream, DMA_IT_TC, DISABLE) ;

#ifndef DRV_SPI_USE_UCOS_III
            /* 置位传输完成标记 */
            pstSPIDevice->ucOperationDone = 1 ;
#else
            /* 释放信号量，表示SPI传输完成 */
            OSSemPost((OS_SEM  *)&(pstSPIDevice->stSemOperationDone), 
                      (OS_OPT   )OS_OPT_POST_1, 
                      (OS_ERR  *)&enErr) ;
#endif
        }
        else
        {
            /* 传输未完成，再次发起传输 */
            /* 发送通道传输数据总量 */
            DMA_SetCurrDataCounter(pstSPIDevice->pstSPIDMATxStream, (pstSPIDevice->uiSize > 65535)? 65535 : (pstSPIDevice->uiSize)) ;

            /* 允许SPI向DMA引擎发出TX请求 */
            SPI_I2S_DMACmd(pstSPIDevice->pstSPIPeriph, SPI_I2S_DMAReq_Tx, ENABLE) ;

            /* 在接收通道，传输的起始地址设置为Rx Buffer */
            DMA_MemoryTargetConfig(pstSPIDevice->pstSPIDMARxStream, (UINT)pstSPIDevice->pucReadBuf, DMA_Memory_0) ;

            /* 接收通道传输数据总量 */
            DMA_SetCurrDataCounter(pstSPIDevice->pstSPIDMARxStream, (pstSPIDevice->uiSize > 65535)? 65535 : (pstSPIDevice->uiSize)) ;

             /* 注意以下2句的先后顺序 */
            pstSPIDevice->pucReadBuf += ((pstSPIDevice->uiSize > 65535)? 65535 : (pstSPIDevice->uiSize)) ;
            pstSPIDevice->uiSize     -= ((pstSPIDevice->uiSize > 65535)? 65535 : (pstSPIDevice->uiSize)) ;

            /* 允许SPI向DMA引擎发出RX请求 */
            SPI_I2S_DMACmd(pstSPIDevice->pstSPIPeriph, SPI_I2S_DMAReq_Rx, ENABLE) ;

            /* 使能传输完成中断 */
            DMA_ITConfig(pstSPIDevice->pstSPIDMARxStream, DMA_IT_TC, ENABLE) ;

            /* 清除可能存在的RXNE标记 */
            SPI_I2S_ReceiveData(pstSPIDevice->pstSPIPeriph) ;

            /* 注意要先启动RX DMA的传输再启动TX DMA的传输。实际上TX传输发生在前，这样避免RX错过。 */
            DMA_Cmd(pstSPIDevice->pstSPIDMARxStream, ENABLE) ;
            DMA_Cmd(pstSPIDevice->pstSPIDMATxStream, ENABLE) ;
        }
    }
    else if(SET == DMA_GetITStatus(pstSPIDevice->pstSPIDMATxStream, pstSPIDevice->uiSPIDMATxStreamTCIF))
    {
        /* 发送中断触发，先等待TXE=1，再等待BUSY=0。由于对DR寄存器的写入到BUSY置1有两个APB的时钟延时，
         * 所以先等待TXE=1是必要的，如果只判断BUSY=0则可能出错。
         */
        while(SET != SPI_I2S_GetFlagStatus(pstSPIDevice->pstSPIPeriph, SPI_I2S_FLAG_TXE)) ;
        while(SET == SPI_I2S_GetFlagStatus(pstSPIDevice->pstSPIPeriph, SPI_I2S_FLAG_BSY)) ;

        /* 清除中断标记 */
        DMA_ClearITPendingBit(pstSPIDevice->pstSPIDMATxStream, pstSPIDevice->uiSPIDMATxStreamTCIF) ;

        if(0 != pstSPIDevice->uiSize)
        {
            /* 再次发送 */
            /* 设置传输的起始地址设置 */
            DMA_MemoryTargetConfig(pstSPIDevice->pstSPIDMATxStream, (UINT)(pstSPIDevice->pucWriteBuf), DMA_Memory_0) ;

            /* 传输数据总量，DMA一次最多65535 */
            DMA_SetCurrDataCounter(pstSPIDevice->pstSPIDMATxStream, (pstSPIDevice->uiSize > 65535)? 65535 : (pstSPIDevice->uiSize)) ;

            /* 注意以下2句的先后顺序 */
            pstSPIDevice->pucWriteBuf += ((pstSPIDevice->uiSize > 65535)? 65535 : (pstSPIDevice->uiSize)) ;
            pstSPIDevice->uiSize      -= ((pstSPIDevice->uiSize > 65535)? 65535 : (pstSPIDevice->uiSize)) ;

            /* 允许SPI向DMA引擎发出TX请求 */
            SPI_I2S_DMACmd(pstSPIDevice->pstSPIPeriph, SPI_I2S_DMAReq_Tx, ENABLE) ;

            /* 开启DMA传输。注意SPI的TXE在空闲时为高，因此可以直接启动DMA传输 */
            DMA_Cmd(pstSPIDevice->pstSPIDMATxStream, ENABLE) ;
        }
        else
        {
            /* 传输完成 */
            /* 禁止SPI向DMA引擎发起TX 请求 */
            SPI_I2S_DMACmd(pstSPIDevice->pstSPIPeriph, SPI_I2S_DMAReq_Tx, DISABLE) ;

            /* 禁止传输完成中断 */
            DMA_ITConfig(pstSPIDevice->pstSPIDMATxStream, DMA_IT_TC, DISABLE) ;

#ifndef DRV_SPI_USE_UCOS_III
            /* 置位传输完成标记 */
            pstSPIDevice->ucOperationDone = 1 ;
#else
            /* 释放信号量，表示SPI传输完成 */
            OSSemPost((OS_SEM  *)&(pstSPIDevice->stSemOperationDone), 
                      (OS_OPT   )OS_OPT_POST_1, 
                      (OS_ERR  *)&enErr) ;
#endif
        }
    }

    return ;
}
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file drv_spi_stm32f4xx.c. *******/  
