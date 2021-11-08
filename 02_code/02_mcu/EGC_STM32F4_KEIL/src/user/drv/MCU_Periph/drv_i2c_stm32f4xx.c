/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : drv_i2c_stm32f4xx.c
    Project name : 
    Module name  : 
    Date created : 2021年9月14日   10时37分29秒
    Author       : Ning.JianLi
    Description  : 基于STM32F4xx微控制器的I2C外设驱动程序源文件。
*******************************************************************************/

/* #############################################################################
 * 设计思路
 * #############################################################################
 * 1. 对于单个字节的接收：
 *    1.1 启动函数中，使能EVENT中断和BUF中断。
 *    1.2 当ADDR中断发生时，设置POS和ACK，使得不响应本字节接收。然后清除ADDR中断，
 *        开始数据传输。
 *    1.3 当RxNE中断发生时，设置STOP；然后禁止中断，读取数据，清除工作区记录消息。
 * 2. 对于2个字节的接收：
 *    2.1 启动函数中，使能EVENT中断，但是禁止BUF中断。这样，不会产生RxNE中断。
 *    2.2 当ADDR中断发生时，设置POS和ACK，使得不响应下个字节接收。然后清除ADDR中
 *        断，开始数据传输。
 *    2.3 当BTF 中断发生时，设置STOP；然后禁止中断，读取数据，清除工作区记录消息。
 * 3. 对于N个字节的接收（N > 2）：
 *    3.1 启动函数中，使能EVENT中断，但是禁止BUF中断。这样，不会产生RxNE中断。
 *    3.2 ADDR中断发生时，仅需简单设置ACK为响应；
 *    3.3 当BTF 中断发生时：
 *        3.3.1 如果剩余字节 > 2, 读取2个数据以清除BTF中断；
 *        3.3.2 如果剩余字节 = 2, 设置POS和ACK，使得不响应下个字节接收。然后读取
 *              2个字节的数据，以清除BTF中断。
 *        3.3.3 如果剩余字节 = 1, 设置POS和ACK，使得不响应本字节的接收，使能BUF/
 *              RxNE中断，然后读2个字节的数据，以清除BTF中断。
 *        3.3.4 如果剩余字节 = 0, 设置STOP，以；然后禁止中断，读取数据，清除工作
 *              区记录消息。
 */


#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "drv_i2c_stm32f4xx.h"

/*******************************************************************************
- Function    : __DRV_I2C_TailLeadZeros
- Description : 本函数检查一个输入数据,从最低位到最高位,遇到的第一个bit '1'之前,
                一共有多少个bit '0'.
- Input       : uiData : 要检查的数据.
- Output      : NULL 
- Return      : UCHAR  : 从低位到高位,第一个出现的bit '1'之前,一共出现的0的个数.
- Others      : 特殊情况 : 如果uiData为0,则返回为0.
*******************************************************************************/
STATIC UCHAR __DRV_I2C_TailLeadZeros(UINT uiData)
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
- Function    : DRV_I2C_Open
- Description : 本函数创建一个基于STM32F4XX的I2C外设的I2C设备.函数流程主要包含如
                下几个步骤:
                1. 配置I2C的GPIO;
                2. 配置I2C外设;
                3. 创建I2C设备.
- Input       : pstI2CInitInfo : 配置信息。
- Output      : pstI2CDevice   : I2C设备驱动指针。
- Return      : I2C_ERROR_OK   : 操作成功；
                其他值            : 查看相关定义.
- Others      :
*******************************************************************************/
DRV_I2C_ERROR_CODE_E DRV_I2C_Open(DRV_I2C_DEVICE_S *pstI2CDevice, DRV_I2C_INIT_INFO_S *pstI2CInitInfo)
{
#ifdef DRV_I2C_USE_UCOS_III
    OS_ERR           enErr ;
#endif
    UINT             uiAHBAPBPeriphClockCtrl = 0 ;
    GPIO_InitTypeDef stGPIOInitInfo ;
    I2C_InitTypeDef  stI2CInitInfo ;
    NVIC_InitTypeDef stI2CNVICInitInfo ;

    /* 参数检查 */
    /* 检查输入指针 */
    if(NULL == pstI2CDevice)
    {
        return I2C_ERROR_OPEN_I2C_DEVICE_PTR_NULL ;
    }

    if(NULL == pstI2CInitInfo)
    {
        return I2C_ERROR_OPEN_CONFIG_INFO_PTR_NULL ;
    }

    /* 检查GPIO */
    if(0 == IS_GPIO_ALL_PERIPH(pstI2CInitInfo->stGPIOInfo.pstSCLGPIOGroup))
    {
        return I2C_ERROR_OPEN_SCL_PIN_GROUP_ERROR ;
    }
    
    if(0 == IS_GPIO_ALL_PERIPH(pstI2CInitInfo->stGPIOInfo.pstSDAGPIOGroup))
    {
        return I2C_ERROR_OPEN_SDA_PIN_GROUP_ERROR ;
    }

    if(0 == IS_GPIO_PIN(pstI2CInitInfo->stGPIOInfo.uiSCLPinID))
    {
        return I2C_ERROR_OPEN_SCL_PIN_ERROR ;
    }

    if(0 == IS_GPIO_PIN(pstI2CInitInfo->stGPIOInfo.uiSDAPinID))
    {
        return I2C_ERROR_OPEN_SDA_PIN_ERROR ;
    }

    if(0 == IS_GPIO_AF(pstI2CInitInfo->stGPIOInfo.ucSCLPinAFDef))
    {
        return I2C_ERROR_OPEN_SCL_AF_ERROR ;
    }

    if(0 == IS_GPIO_AF(pstI2CInitInfo->stGPIOInfo.ucSDAPinAFDef))
    {
        return I2C_ERROR_OPEN_SDA_AF_ERROR ;
    }

    /* 检查I2C */
    if(0 == IS_I2C_ALL_PERIPH(pstI2CInitInfo->stI2CInfo.pstI2CPeriph))
    {
        return I2C_ERROR_OPEN_INVALID_I2C_PERIPH ;
    }

    if(0 == IS_I2C_CLOCK_SPEED(pstI2CInitInfo->stI2CInfo.uiI2CClockSpeed))
    {
        return I2C_ERROR_OPEN_INVALID_I2C_CLOCK_SPEED ;
    }

    if(0 == IS_I2C_DUTY_CYCLE(pstI2CInitInfo->stI2CInfo.usI2CDutyCycle))
    {
        return I2C_ERROR_OPEN_INVALID_I2C_DUTY_CYCLE ;
    }

    if(DRV_I2C_OPEN_FLAG != pstI2CDevice->ucOpenFlag)
    {
        /* GPIO配置 */
        /* 使能GPIO时钟 */
        switch((UINT)pstI2CInitInfo->stGPIOInfo.pstSCLGPIOGroup)
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

        switch((UINT)pstI2CInitInfo->stGPIOInfo.pstSDAGPIOGroup)
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

        /* 引脚属性配置 */
        stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_AF ;
        stGPIOInitInfo.GPIO_OType = GPIO_OType_OD ;
        stGPIOInitInfo.GPIO_PuPd  = GPIO_PuPd_NOPULL ;
        stGPIOInitInfo.GPIO_Speed = GPIO_Speed_2MHz ;

        stGPIOInitInfo.GPIO_Pin   = pstI2CInitInfo->stGPIOInfo.uiSCLPinID ;
        GPIO_Init(pstI2CInitInfo->stGPIOInfo.pstSCLGPIOGroup, &stGPIOInitInfo) ;

        stGPIOInitInfo.GPIO_Pin   = pstI2CInitInfo->stGPIOInfo.uiSDAPinID ;
        GPIO_Init(pstI2CInitInfo->stGPIOInfo.pstSDAGPIOGroup, &stGPIOInitInfo) ;

        /* 引脚复用定义 */
        GPIO_PinAFConfig(pstI2CInitInfo->stGPIOInfo.pstSCLGPIOGroup, 
                         __DRV_I2C_TailLeadZeros(pstI2CInitInfo->stGPIOInfo.uiSCLPinID), 
                         pstI2CInitInfo->stGPIOInfo.ucSCLPinAFDef) ;
        
        GPIO_PinAFConfig(pstI2CInitInfo->stGPIOInfo.pstSDAGPIOGroup, 
                         __DRV_I2C_TailLeadZeros(pstI2CInitInfo->stGPIOInfo.uiSDAPinID), 
                         pstI2CInitInfo->stGPIOInfo.ucSDAPinAFDef) ;

        /* 使能I2C时钟 */
        if(I2C1 == pstI2CInitInfo->stI2CInfo.pstI2CPeriph)
        {
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE) ;
        }
        else if(I2C2 == pstI2CInitInfo->stI2CInfo.pstI2CPeriph)
        {
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE) ;
        }
        else if(I2C3 == pstI2CInitInfo->stI2CInfo.pstI2CPeriph)
        {
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE) ;
        }

        /* I2C属性配置 */
        stI2CInitInfo.I2C_ClockSpeed  = pstI2CInitInfo->stI2CInfo.uiI2CClockSpeed ;
        stI2CInitInfo.I2C_Mode        = I2C_Mode_I2C ;
        stI2CInitInfo.I2C_DutyCycle   = pstI2CInitInfo->stI2CInfo.usI2CDutyCycle ;
        stI2CInitInfo.I2C_OwnAddress1 = 0x01 ;
        stI2CInitInfo.I2C_Ack         = I2C_Ack_Enable ;

        I2C_Init(pstI2CInitInfo->stI2CInfo.pstI2CPeriph, &stI2CInitInfo) ;

        /* 禁止I2C中断 */
        I2C_ITConfig(pstI2CInitInfo->stI2CInfo.pstI2CPeriph, I2C_IT_BUF | I2C_IT_EVT | I2C_IT_ERR, DISABLE) ;

        /* 中断配置 */
        if(I2C1 == pstI2CInitInfo->stI2CInfo.pstI2CPeriph)
        {
            stI2CNVICInitInfo.NVIC_IRQChannel               = I2C1_EV_IRQn ;
        }
        else if(I2C2 == pstI2CInitInfo->stI2CInfo.pstI2CPeriph)
        {
            stI2CNVICInitInfo.NVIC_IRQChannel               = I2C2_EV_IRQn ;
        }
        else if(I2C3 == pstI2CInitInfo->stI2CInfo.pstI2CPeriph)
        {
            stI2CNVICInitInfo.NVIC_IRQChannel               = I2C3_EV_IRQn ;
        }
        
        stI2CNVICInitInfo.NVIC_IRQChannelCmd                = ENABLE ;
        stI2CNVICInitInfo.NVIC_IRQChannelSubPriority        = 0 ; /* 高优先级 */
        stI2CNVICInitInfo.NVIC_IRQChannelPreemptionPriority = 0 ; /* 高优先级 */

        NVIC_Init(&stI2CNVICInitInfo) ;

#ifdef DRV_I2C_USE_UCOS_III
        /* 创建信号量 */
        /* 创建I2C访问锁 */
        OSSemCreate(&pstI2CDevice->stI2CACCSem, "Semaphore-I2C", (OS_SEM_CTR)1, &enErr) ;

        if(OS_ERR_NONE != enErr)
        {
            return I2C_ERROR_OPEN_CREATE_SEM_I2C_ACC ;
        }

        /* 创建I2C写操作锁 */
        OSSemCreate(&pstI2CDevice->stI2CWRSem, "Semaphore-I2C-WR", (OS_SEM_CTR)0, &enErr) ;

        if(OS_ERR_NONE != enErr)
        {
            /* 释放总线资源 */
            OSSemDel(&pstI2CDevice->stI2CACCSem, OS_OPT_DEL_ALWAYS, &enErr) ;
            
            return I2C_ERROR_OPEN_CREATE_SEM_I2C_WR ;
        }
        
        /* 创建I2C读操作锁 */
        OSSemCreate(&pstI2CDevice->stI2CRDSem, "Semaphore-I2C-RD", (OS_SEM_CTR)0, &enErr) ;

        if(OS_ERR_NONE != enErr)
        {
            /* 释放总线资源和写操作信号量 */
            OSSemDel(&pstI2CDevice->stI2CACCSem, OS_OPT_DEL_ALWAYS, &enErr) ;
            OSSemDel(&pstI2CDevice->stI2CWRSem, OS_OPT_DEL_ALWAYS, &enErr) ;
            
            return I2C_ERROR_OPEN_CREATE_SEM_I2C_RD ;
        }
#else
        pstI2CDevice->ucI2CACCSem     = DRV_I2C_BUS_UNOCCUPY ;
        pstI2CDevice->ucI2CWRSem      = DRV_I2C_WR_DONE ;
        pstI2CDevice->ucI2CRDSem      = DRV_I2C_RD_DONE ;
#endif
        /* 完成创建，登记信息 */
        pstI2CDevice->pstSCLGPIOGroup = pstI2CInitInfo->stGPIOInfo.pstSCLGPIOGroup ;
        pstI2CDevice->pstSDAGPIOGroup = pstI2CInitInfo->stGPIOInfo.pstSDAGPIOGroup ;
        pstI2CDevice->uiSCLPinID      = pstI2CInitInfo->stGPIOInfo.uiSCLPinID ;
        pstI2CDevice->uiSDAPinID      = pstI2CInitInfo->stGPIOInfo.uiSDAPinID ;
        pstI2CDevice->ucSCLPinAFDef   = pstI2CInitInfo->stGPIOInfo.ucSCLPinAFDef ;
        pstI2CDevice->ucSDAPinAFDef   = pstI2CInitInfo->stGPIOInfo.ucSDAPinAFDef ;
        pstI2CDevice->pstI2CPeriph    = pstI2CInitInfo->stI2CInfo.pstI2CPeriph ;
        pstI2CDevice->enWorkMode      = I2C_MODE_IDLE ;
        pstI2CDevice->ucOpenFlag      = DRV_I2C_OPEN_FLAG ;
    }
    else
    {
        return I2C_ERROR_OPEN_DEVICE_ALREADY_CREATED ;
    }

    return I2C_ERROR_OK ;
}

/*******************************************************************************
- Function    : __DRV_I2C_ReadPhase1
- Description : 本函数用于实现I2C读事务的第一相操作。
- Input       : pstI2CDevice   : I2C驱动设备。
                ucDevAddr      : I2C设备地址。 
                ucRegAddr      : I2C寄存器地址。
- Output      : NULL
- Return      : I2C_ERROR_OK   : 操作成功；
                其他值            : 查看相关定义.
- Others      :
*******************************************************************************/
DRV_I2C_ERROR_CODE_E __DRV_I2C_ReadPhase1(DRV_I2C_DEVICE_S *pstI2CDevice, UCHAR ucDevAddr, UCHAR ucRegAddr)
{
#ifdef DRV_I2C_USE_UCOS_III
    OS_ERR enErr ;
#endif

    if(NULL == pstI2CDevice)
    {
        return I2C_ERROR_READPHASE1_DEVICE_PTR_NULL ;
    }

    if(DRV_I2C_OPEN_FLAG != pstI2CDevice->ucOpenFlag)
    {
        return I2C_ERROR_READPHASE1_DEVICE_NOT_OPENED ;
    }

    if(I2C_MODE_IDLE != pstI2CDevice->enWorkMode)
    {
        return I2C_ERROR_READPHASE1_DEVICE_NOT_IDLE ;
    }

    /* 信息保存 */
    pstI2CDevice->enWorkMode = I2C_MODE_RD_P1 ;
    pstI2CDevice->ucDevAddr  = (ucDevAddr << 1) ;
    pstI2CDevice->ucRegAddr  = ucRegAddr ;

#ifndef DRV_I2C_USE_UCOS_III
    /* 撤销传输完成标记 */
    pstI2CDevice->ucI2CRDSem = DRV_I2C_RD_PEND ;
#endif

    /* 允许事件中断，禁止BUF中断 */
    I2C_ITConfig(pstI2CDevice->pstI2CPeriph, I2C_IT_EVT, ENABLE) ;
    I2C_ITConfig(pstI2CDevice->pstI2CPeriph, I2C_IT_BUF, DISABLE) ;

    /* 允许ACK */
    I2C_AcknowledgeConfig(pstI2CDevice->pstI2CPeriph, ENABLE) ;
    
    /* 产生起始位 */
    I2C_GenerateSTART(pstI2CDevice->pstI2CPeriph, ENABLE) ;

    /* 等待传输完成 */
#ifdef DRV_I2C_USE_UCOS_III
    OSSemPend(&pstI2CDevice->stI2CRDSem, (OS_TICK)0, OS_OPT_PEND_BLOCKING, NULL, &enErr) ;  
#else
    while(DRV_I2C_RD_PEND == pstI2CDevice->ucI2CRDSem) ;
#endif

    return I2C_ERROR_OK ;
}

/*******************************************************************************
- Function    : __DRV_I2C_ReadPhase2
- Description : 本函数用于实现I2C读事务的第二相操作。
- Input       : pstI2CDevice   : I2C驱动设备。
                ucDevAddr      : I2C设备地址。
                uiSize         : 读取的数据量。
- Output      : pucBuf         : 保存读取数据的缓存。
- Return      : I2C_ERROR_OK   : 操作成功；
                其他值            : 查看相关定义.
- Others      :
*******************************************************************************/
DRV_I2C_ERROR_CODE_E __DRV_I2C_ReadPhase2(DRV_I2C_DEVICE_S *pstI2CDevice, UCHAR ucDevAddr, UCHAR *pucBuf, UINT uiSize)
{
#ifdef DRV_I2C_USE_UCOS_III
    OS_ERR enErr ;
#endif

    if(NULL == pstI2CDevice)
    {
        return I2C_ERROR_READPHASE2_DEVICE_PTR_NULL ;
    }

    if(NULL == pucBuf)
    {
        return I2C_ERROR_READPHASE2_BUFFER_PTR_NULL ;
    }

    if(0 == uiSize)
    {
        return I2C_ERROR_READPHASE2_READ_SIZE_ZERO ;
    }

    if(DRV_I2C_OPEN_FLAG != pstI2CDevice->ucOpenFlag)
    {
        return I2C_ERROR_READPHASE2_DEVICE_NOT_OPENED ;
    }

    if(I2C_MODE_IDLE != pstI2CDevice->enWorkMode)
    {
        return I2C_ERROR_READPHASE2_DEVICE_NOT_IDLE ;
    }

    /* 信息保存 */
    pstI2CDevice->enWorkMode      = I2C_MODE_RD_P2 ;
    pstI2CDevice->pucBuf          = pucBuf ;
    pstI2CDevice->ucDevAddr       = (ucDevAddr << 1) ;
    pstI2CDevice->uiSize          = uiSize ;

#ifndef DRV_I2C_USE_UCOS_III
    /* 撤销传输完成标记 */
    pstI2CDevice->ucI2CRDSem      = DRV_I2C_RD_PEND ;
#endif

    /* 允许ACK */
    I2C_AcknowledgeConfig(pstI2CDevice->pstI2CPeriph, ENABLE) ;
    
    /* 产生起始位 */
    I2C_GenerateSTART(pstI2CDevice->pstI2CPeriph, ENABLE) ;

    /* 等待START发送完毕，这样才能清除Phase1中的中断状态 */
    while(RESET == I2C_GetFlagStatus(pstI2CDevice->pstI2CPeriph, I2C_FLAG_SB)) ;

    /* 允许事件中断 */
    I2C_ITConfig(pstI2CDevice->pstI2CPeriph, I2C_IT_EVT, ENABLE) ;

    /* 单字节接收则使能RxNE中断，否则禁止 */
    if(1 == uiSize)
    {
        I2C_ITConfig(pstI2CDevice->pstI2CPeriph, I2C_IT_BUF, ENABLE) ;
    }
    else
    {
        I2C_ITConfig(pstI2CDevice->pstI2CPeriph, I2C_IT_BUF, DISABLE) ;
    }

    /* 等待传输完成 */
#ifdef DRV_I2C_USE_UCOS_III
    OSSemPend(&pstI2CDevice->stI2CRDSem, (OS_TICK)0, OS_OPT_PEND_BLOCKING, NULL, &enErr) ;  
#else
    while(DRV_I2C_RD_PEND == pstI2CDevice->ucI2CRDSem) ;
#endif

    return I2C_ERROR_OK ;
}


/*******************************************************************************
- Function    : DRV_I2C_Write
- Description : 本函数用于实现I2C写事务。
- Input       : pstI2CDevice : I2C驱动设备。
                ucDevAddr    : I2C设备地址。 
                ucRegAddr    : I2C寄存器地址。
                pucBuf       : 保存待写入的数据的缓存。
                uiSize       : 写入的数据量。
- Output      : NULL
- Return      : I2C_ERROR_OK : 操作成功；
                其他值          : 查看相关定义.
- Others      :
*******************************************************************************/
DRV_I2C_ERROR_CODE_E DRV_I2C_Write(DRV_I2C_DEVICE_S *pstI2CDevice, UCHAR ucDevAddr, UCHAR ucRegAddr, UCHAR *pucBuf, UINT uiSize)
{
#ifdef DRV_I2C_USE_UCOS_III
    OS_ERR enErr ;
#endif

    if(NULL == pstI2CDevice)
    {
        return I2C_ERROR_WRITE_DEVICE_PTR_NULL ;
    }

    if(NULL == pucBuf)
    {
        return I2C_ERROR_WRITE_BUFFER_PTR_NULL ;
    }

    if(0 == uiSize)
    {
        return I2C_ERROR_WRITE_READ_SIZE_ZERO ;
    }

    if(DRV_I2C_OPEN_FLAG != pstI2CDevice->ucOpenFlag)
    {
        return I2C_ERROR_WRITE_DEVICE_NOT_OPENED ;
    }

    if(I2C_MODE_IDLE != pstI2CDevice->enWorkMode)
    {
        return I2C_ERROR_WRITE_DEVICE_NOT_IDLE ;
    }

    /* 给总线加锁 */
#ifdef DRV_I2C_USE_UCOS_III
    OSSemPend(&pstI2CDevice->stI2CACCSem, (OS_TICK)0, OS_OPT_PEND_BLOCKING, NULL, &enErr) ;
#else
    while(DRV_I2C_BUS_UNOCCUPY != pstI2CDevice->ucI2CACCSem) ;
    pstI2CDevice->ucI2CACCSem = DRV_I2C_BUS_OCCUPY ;
#endif

    /* 信息保存 */
    pstI2CDevice->enWorkMode = I2C_MODE_WR ;
    pstI2CDevice->pucBuf     = pucBuf ;
    pstI2CDevice->ucDevAddr  = (ucDevAddr << 1) ;
    pstI2CDevice->ucRegAddr  = ucRegAddr ;
    pstI2CDevice->uiSize     = uiSize ;

#ifndef DRV_I2C_USE_UCOS_III
    /* 撤销传输完成标记 */
    pstI2CDevice->ucI2CWRSem = DRV_I2C_WR_PEND ;
#endif

    /* 允许事件中断 */
    I2C_ITConfig(pstI2CDevice->pstI2CPeriph, I2C_IT_EVT, ENABLE) ;

    /* 产生起始位 */
    I2C_GenerateSTART(pstI2CDevice->pstI2CPeriph, ENABLE) ;

    /* 等待传输完成 */
#ifdef DRV_I2C_USE_UCOS_III
    OSSemPend(&(pstI2CDevice->stI2CWRSem), (OS_TICK)0, OS_OPT_PEND_BLOCKING, NULL, &enErr) ;
#else
    while(DRV_I2C_WR_PEND == pstI2CDevice->ucI2CWRSem) ;
#endif

    /* 等待总线空闲 */
    while(SET == I2C_GetFlagStatus(pstI2CDevice->pstI2CPeriph, I2C_FLAG_BUSY)) ;
    
    /* 释放总线资源 */
#ifdef DRV_I2C_USE_UCOS_III
    OSSemPost(&pstI2CDevice->stI2CACCSem, OS_OPT_POST_1, &enErr) ;
#else
    pstI2CDevice->ucI2CACCSem = DRV_I2C_BUS_UNOCCUPY ;
#endif

    return I2C_ERROR_OK ;
}


/*******************************************************************************
- Function    : DRV_I2C_Read
- Description : 本函数用于实现I2C读事务。
- Input       : pstI2CDevice : I2C驱动设备。
                ucDevAddr    : I2C设备地址。 
                ucRegAddr    : I2C寄存器地址。
                uiSize       : 写入的数据量。
                usAddGapMs   : 指定在读操作第一相和第二相之间插入的等待时间。某些
                               器件的读取需要等待就绪。
- Output      : pucBuf       : 保存待读取的数据的缓存。
- Return      : I2C_ERROR_OK : 操作成功；
                其他值          : 查看相关定义.
- Others      :
*******************************************************************************/
DRV_I2C_ERROR_CODE_E DRV_I2C_Read(DRV_I2C_DEVICE_S *pstI2CDevice, UCHAR ucDevAddr, UCHAR ucRegAddr, UCHAR *pucBuf, UINT uiSize, USHORT usAddGapMs)
{
#ifdef DRV_I2C_USE_UCOS_III
    OS_ERR enErr ;
#endif

    if(NULL == pstI2CDevice)
    {
        return I2C_ERROR_READ_DEVICE_PTR_NULL ;
    }

    if(NULL == pucBuf)
    {
        return I2C_ERROR_READ_BUFFER_PTR_NULL ;
    }

    if(0 == uiSize)
    {
        return I2C_ERROR_READ_READ_SIZE_ZERO ;
    }

    if(DRV_I2C_OPEN_FLAG != pstI2CDevice->ucOpenFlag)
    {
        return I2C_ERROR_READ_DEVICE_NOT_OPENED ;
    }

    if(I2C_MODE_IDLE != pstI2CDevice->enWorkMode)
    {
        return I2C_ERROR_READ_DEVICE_NOT_IDLE ;
    }

    /* 给总线加锁 */
#ifdef DRV_I2C_USE_UCOS_III
    OSSemPend(&pstI2CDevice->stI2CACCSem, (OS_TICK)0, OS_OPT_PEND_BLOCKING, NULL, &enErr) ;
#else
    while(DRV_I2C_BUS_UNOCCUPY != pstI2CDevice->ucI2CACCSem) ;
    pstI2CDevice->ucI2CACCSem = DRV_I2C_BUS_OCCUPY ;
#endif

    __DRV_I2C_ReadPhase1(pstI2CDevice, ucDevAddr, ucRegAddr) ;

    if(0 != usAddGapMs)
    {
        OSTimeDlyHMSM((CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT32U   )((usAddGapMs > 999)? 999 : usAddGapMs),
                      (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                      (OS_ERR      *)&enErr);
    }

    __DRV_I2C_ReadPhase2(pstI2CDevice, ucDevAddr, pucBuf, uiSize) ;

    /* 等待总线空闲 */
    while(SET == I2C_GetFlagStatus(pstI2CDevice->pstI2CPeriph, I2C_FLAG_BUSY)) ;

    /* 释放总线资源 */
#ifdef DRV_I2C_USE_UCOS_III
    OSSemPost(&pstI2CDevice->stI2CACCSem, OS_OPT_POST_1, &enErr) ;
#else
    pstI2CDevice->ucI2CACCSem = DRV_I2C_BUS_UNOCCUPY ;
#endif

    return I2C_ERROR_OK ;
}


/*******************************************************************************
- Function    : DRV_I2C_Close
- Description : 本函数关闭一个基于STM32F4XX的I2C外设的I2C驱动设备.函数流程主要包
                含如下几个步骤:
                1. 删除信号量;
                2. 关闭I2C外设;
                3. 配置I2C GPIO为输入状态.
- Input       : pstI2CDevice   : I2C设备驱动指针。
- Output      : NULL
- Return      : I2C_ERROR_OK   : 操作成功；
                其他值            : 查看相关定义.
- Others      :
*******************************************************************************/
DRV_I2C_ERROR_CODE_E DRV_I2C_Close(DRV_I2C_DEVICE_S *pstI2CDevice)
{
#ifdef DRV_I2C_USE_UCOS_III
    OS_ERR enErr ;
#endif
    GPIO_InitTypeDef stGPIOInitInfo ;

    if(NULL == pstI2CDevice)
    {
        return I2C_ERROR_CLOSE_DEVICE_PTR_NULL ;
    }

    if(DRV_I2C_OPEN_FLAG != pstI2CDevice->ucOpenFlag)
    {
        return I2C_ERROR_CLOSE_DEVICE_NOT_OPENED ;
    }

    /* 检查GPIO */
    if(0 == IS_GPIO_ALL_PERIPH(pstI2CDevice->pstSCLGPIOGroup))
    {
        return I2C_ERROR_CLOSE_SCL_PIN_GROUP_ERROR ;
    }
    
    if(0 == IS_GPIO_ALL_PERIPH(pstI2CDevice->pstSDAGPIOGroup))
    {
        return I2C_ERROR_CLOSE_SDA_PIN_GROUP_ERROR ;
    }

    if(0 == IS_GPIO_PIN(pstI2CDevice->uiSCLPinID))
    {
        return I2C_ERROR_CLOSE_SCL_PIN_ERROR ;
    }

    if(0 == IS_GPIO_PIN(pstI2CDevice->uiSDAPinID))
    {
        return I2C_ERROR_CLOSE_SDA_PIN_ERROR ;
    }

    if(0 == IS_GPIO_AF(pstI2CDevice->ucSCLPinAFDef))
    {
        return I2C_ERROR_CLOSE_SCL_AF_ERROR ;
    }

    if(0 == IS_GPIO_AF(pstI2CDevice->ucSDAPinAFDef))
    {
        return I2C_ERROR_CLOSE_SDA_AF_ERROR ;
    }

    /* 检查I2C */
    if(0 == IS_I2C_ALL_PERIPH(pstI2CDevice->pstI2CPeriph))
    {
        return I2C_ERROR_CLOSE_INVALID_I2C_PERIPH ;
    }

    /* 删除信号量 */
#ifdef DRV_I2C_USE_UCOS_III
    OSSemDel(&pstI2CDevice->stI2CACCSem, OS_OPT_DEL_ALWAYS, &enErr) ;
    OSSemDel(&pstI2CDevice->stI2CWRSem, OS_OPT_DEL_ALWAYS, &enErr) ;
    OSSemDel(&pstI2CDevice->stI2CRDSem, OS_OPT_DEL_ALWAYS, &enErr) ;
#endif

    /* 关闭I2C外设 */
    I2C_Cmd(pstI2CDevice->pstI2CPeriph, DISABLE) ;
    
    /* 复位GPIO为输入 */
    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_IN ;
    stGPIOInitInfo.GPIO_OType = GPIO_OType_PP ;
    stGPIOInitInfo.GPIO_PuPd  = GPIO_PuPd_NOPULL ;
    stGPIOInitInfo.GPIO_Speed = GPIO_Speed_2MHz ;

    stGPIOInitInfo.GPIO_Pin   = pstI2CDevice->uiSCLPinID ;
    GPIO_Init(pstI2CDevice->pstSCLGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin   = pstI2CDevice->uiSDAPinID ;
    GPIO_Init(pstI2CDevice->pstSDAGPIOGroup, &stGPIOInitInfo) ;

    /* 清除打开标记 */
    pstI2CDevice->ucOpenFlag = ~DRV_I2C_OPEN_FLAG ;
    
    return I2C_ERROR_OK ;
}


/*******************************************************************************
- Function    : DRV_I2C_ResetBus
- Description : 本函数当I2C总线死锁时，解锁总线。
- Input       : pstI2CDevice : I2C驱动设备。
- Output      : NULL
- Return      : I2C_ERROR_OK : 操作成功；
                其他值          : 查看相关定义.
- Others      :
*******************************************************************************/
DRV_I2C_ERROR_CODE_E DRV_I2C_ResetBus(DRV_I2C_DEVICE_S *pstI2CDevice)
{
    GPIO_InitTypeDef stGPIOInitInfo ;
    UINT             uiLoop ;
    UINT             uiDelay ;

    if(NULL == pstI2CDevice)
    {
        return I2C_ERROR_RESETBUS_DEVICE_PTR_NULL ;
    }

    if(DRV_I2C_OPEN_FLAG != pstI2CDevice->ucOpenFlag)
    {
        return I2C_ERROR_RESETBUS_DEVICE_NOT_OPENED ;
    }

    /* GPIO引脚配置 */
    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_OUT ;
    stGPIOInitInfo.GPIO_OType = GPIO_OType_OD ;
    stGPIOInitInfo.GPIO_PuPd  = GPIO_PuPd_NOPULL ;
    stGPIOInitInfo.GPIO_Speed = GPIO_Speed_2MHz ;

    stGPIOInitInfo.GPIO_Pin   = pstI2CDevice->uiSCLPinID ;
    GPIO_Init(pstI2CDevice->pstSCLGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin   = pstI2CDevice->uiSDAPinID ;
    GPIO_Init(pstI2CDevice->pstSDAGPIOGroup, &stGPIOInitInfo) ;

    /* 生成连续的SCL以复位总线 */
    for(uiLoop = 0 ; uiLoop < 16 ; uiLoop++)
    {
        GPIO_ResetBits(pstI2CDevice->pstSCLGPIOGroup, pstI2CDevice->uiSCLPinID) ;

        uiDelay = 100 ;
        while(uiDelay--) ;
        
        GPIO_SetBits(pstI2CDevice->pstSCLGPIOGroup, pstI2CDevice->uiSCLPinID) ;

        uiDelay = 200 ;
        while(uiDelay--) ; 

        GPIO_ResetBits(pstI2CDevice->pstSCLGPIOGroup, pstI2CDevice->uiSCLPinID) ;

        uiDelay = 100 ;
        while(uiDelay--) ;
    }

    /* 产生一个STOP */
    GPIO_SetBits(pstI2CDevice->pstSCLGPIOGroup, pstI2CDevice->uiSCLPinID) ;
    
    uiDelay = 100 ;
    while(uiDelay--) ;
    
    GPIO_SetBits(pstI2CDevice->pstSDAGPIOGroup, pstI2CDevice->uiSDAPinID) ;

    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_AF ;
    stGPIOInitInfo.GPIO_OType = GPIO_OType_OD ;
    stGPIOInitInfo.GPIO_PuPd  = GPIO_PuPd_NOPULL ;
    stGPIOInitInfo.GPIO_Speed = GPIO_Speed_2MHz ;

    stGPIOInitInfo.GPIO_Pin   = pstI2CDevice->uiSCLPinID ;
    GPIO_Init(pstI2CDevice->pstSCLGPIOGroup, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin   = pstI2CDevice->uiSDAPinID ;
    GPIO_Init(pstI2CDevice->pstSDAGPIOGroup, &stGPIOInitInfo) ;

    return I2C_ERROR_OK ;
}

/*******************************************************************************
- Function    : DRV_I2C_GeneralIntHandler
- Description : 本函数为硬件I2C的事件中断处理函数。
- Input       : NULL
- Output      : NULL
- Return      : I2C_ERROR_OK : 操作成功；
                其他值          : 查看相关定义.
- Others      :
*******************************************************************************/
DRV_I2C_ERROR_CODE_E DRV_I2C_GeneralIntHandler(DRV_I2C_DEVICE_S *pstI2CDevice)
{
#ifdef DRV_I2C_USE_UCOS_III
    OS_ERR enErr ;
#endif

    if(NULL == pstI2CDevice)
    {
        return I2C_ERROR_INTHANDLER_DEVICE_PTR_NULL ;
    }

    if(DRV_I2C_OPEN_FLAG != pstI2CDevice->ucOpenFlag)
    {
        return I2C_ERROR_INTHANDLER_DEVICE_NOT_OPENED ;
    }
        
    if(I2C_MODE_WR == pstI2CDevice->enWorkMode)
    {
        /* ##########################################################################################
         * CASE 1 : Start发送完毕，需要发送地址
         * ##########################################################################################
         */
        if(SET == I2C_GetITStatus(pstI2CDevice->pstI2CPeriph, I2C_IT_SB))
        {
            /* 发送总线地址 */
            I2C_Send7bitAddress(pstI2CDevice->pstI2CPeriph, pstI2CDevice->ucDevAddr, I2C_Direction_Transmitter) ;
        }
        /* ##########################################################################################
         * CASE 2 : ADDR置位，发送寄存器地址
         * ##########################################################################################
         */
        else if(SET == I2C_GetITStatus(pstI2CDevice->pstI2CPeriph, I2C_IT_ADDR))
        {
            /* 读取SR2以清除中断 */
            pstI2CDevice->pstI2CPeriph->SR2 ;
            
            /* 发送寄存器地址 */
            I2C_SendData(pstI2CDevice->pstI2CPeriph, pstI2CDevice->ucRegAddr) ;
        }
        /* ##########################################################################################
         * CASE 3 : BTF置位，发送数据
         * ##########################################################################################
         */
        else if(SET == I2C_GetITStatus(pstI2CDevice->pstI2CPeriph, I2C_IT_BTF))
        {
            if(0 == pstI2CDevice->uiSize)
            {
                /* 生成停止位 */
                I2C_GenerateSTOP(pstI2CDevice->pstI2CPeriph, ENABLE) ;

                pstI2CDevice->enWorkMode = I2C_MODE_IDLE ;
                pstI2CDevice->pucBuf     = 0 ;
                pstI2CDevice->uiSize     = 0 ;

                /* 禁止中断 */
                I2C_ITConfig(pstI2CDevice->pstI2CPeriph, I2C_IT_BUF | I2C_IT_EVT, DISABLE) ;

#ifdef DRV_I2C_USE_UCOS_III
                /* 释放写操作完成信号量 */
                OSSemPost(&pstI2CDevice->stI2CWRSem, OS_OPT_POST_1, &enErr) ;
#else
                pstI2CDevice->ucI2CWRSem = DRV_I2C_WR_DONE ;
#endif
            }
            else
            {
                /* 发送数据 */
                I2C_SendData(pstI2CDevice->pstI2CPeriph, *pstI2CDevice->pucBuf) ;
                pstI2CDevice->pucBuf++ ;
                pstI2CDevice->uiSize-- ;
            }
        }
        else
        {
            if(I2C_EVENT_MASTER_BYTE_TRANSMITTING == I2C_GetLastEvent(pstI2CDevice->pstI2CPeriph))
            {
                if(0 == pstI2CDevice->uiSize)
                {
                    /* 生成停止位 */
                    I2C_GenerateSTOP(pstI2CDevice->pstI2CPeriph, ENABLE) ;

                    pstI2CDevice->enWorkMode = I2C_MODE_IDLE ;
                    pstI2CDevice->pucBuf     = 0 ;
                    pstI2CDevice->uiSize     = 0 ;

                    /* 禁止中断 */
                    I2C_ITConfig(pstI2CDevice->pstI2CPeriph, I2C_IT_BUF | I2C_IT_EVT, DISABLE) ;

#ifdef DRV_I2C_USE_UCOS_III
                    /* 释放写操作完成信号量 */
                    OSSemPost(&pstI2CDevice->stI2CWRSem, OS_OPT_POST_1, &enErr) ;
#else
                    pstI2CDevice->ucI2CWRSem = DRV_I2C_WR_DONE ;
#endif
                }
                else
                {
                    /* 发送数据 */
                    I2C_SendData(pstI2CDevice->pstI2CPeriph, *pstI2CDevice->pucBuf) ;
                    pstI2CDevice->pucBuf++ ;
                    pstI2CDevice->uiSize-- ;
                }
            }
        }
    }
    else if(I2C_MODE_RD_P1 == pstI2CDevice->enWorkMode)
    {
        /* ##########################################################################################
         * CASE 1 : Start发送完毕，需要发送总线地址
         * ##########################################################################################
         */
        if(SET == I2C_GetITStatus(pstI2CDevice->pstI2CPeriph, I2C_IT_SB))
        {
            /* 发送总线地址 */
            I2C_Send7bitAddress(pstI2CDevice->pstI2CPeriph, pstI2CDevice->ucDevAddr, I2C_Direction_Transmitter) ;
        }
        /* ##########################################################################################
         * CASE 2 : ADDR置位，发送寄存器地址
         * ##########################################################################################
         */
        else if(SET == I2C_GetITStatus(pstI2CDevice->pstI2CPeriph, I2C_IT_ADDR))
        {
            /* 读取SR2以清除中断 */
            pstI2CDevice->pstI2CPeriph->SR2 ; 
            
            /* 发送寄存器地址 */
            I2C_SendData(pstI2CDevice->pstI2CPeriph, pstI2CDevice->ucRegAddr) ;
        }
        /********************************************************************************************
         * CASE 3 : BTF置位，完成第一相的操作
         ********************************************************************************************
         */
        else if(SET == I2C_GetITStatus(pstI2CDevice->pstI2CPeriph, I2C_IT_BTF))
        {
            /* 禁止中断 */
            I2C_ITConfig(pstI2CDevice->pstI2CPeriph, I2C_IT_BUF | I2C_IT_EVT, DISABLE) ;

            /* 恢复就绪 */
            pstI2CDevice->enWorkMode = I2C_MODE_IDLE ;
            
            /* 释放读操作完成信号量 */
#ifdef DRV_I2C_USE_UCOS_III
            OSSemPost(&pstI2CDevice->stI2CRDSem, OS_OPT_POST_1, &enErr) ;
#else
            pstI2CDevice->ucI2CRDSem = DRV_I2C_RD_DONE ;
#endif
        }
        else
        {
            /* 错误处理 */
        }
    }
    else if(I2C_MODE_RD_P2 == pstI2CDevice->enWorkMode)
    {
        /* ##########################################################################################
         * CASE 1 : Start发送完毕，需要发送地址
         * ##########################################################################################
         */
        if(SET == I2C_GetITStatus(pstI2CDevice->pstI2CPeriph, I2C_IT_SB))
        {
            /* 发送总线地址 */
            I2C_Send7bitAddress(pstI2CDevice->pstI2CPeriph, pstI2CDevice->ucDevAddr, I2C_Direction_Receiver) ;
        }
        /* ##########################################################################################
         * CASE 2 : ADDR置位，发送寄存器地址
         * 注意：为了避免DR和DSR导致的错误，需要做如下处理：
         * 1. 如果仅接收1个字节，则清空ADDR之前，设置POS=0，以使得ACK控制下一个字节；ACK=0以禁止下个
         *    字节应答，然后清除ADDR。
         * 2. 如果接收2个字节，则清空ADDR之前，设置POS=1， 以使得ACK控制下下个字节；ACK=0以禁止下下个
         *    字节应答，然后清除ADDR。
         * 3. 如果接收大于2个字节，可直接清除ADDR。
         * ##########################################################################################
         */
        else if(SET == I2C_GetITStatus(pstI2CDevice->pstI2CPeriph, I2C_IT_ADDR))
        {
            if(1 == pstI2CDevice->uiSize)
            {
                I2C_PECPositionConfig(pstI2CDevice->pstI2CPeriph, I2C_PECPosition_Current) ;
                I2C_AcknowledgeConfig(pstI2CDevice->pstI2CPeriph, DISABLE) ;
            }
            else if(2 == pstI2CDevice->uiSize)
            {
                I2C_PECPositionConfig(pstI2CDevice->pstI2CPeriph, I2C_PECPosition_Next) ;
                I2C_AcknowledgeConfig(pstI2CDevice->pstI2CPeriph, DISABLE) ;
            }

            /* 读取SR2以清除中断，并开始接收数据 */
            pstI2CDevice->pstI2CPeriph->SR2 ;
        }
        /* ##########################################################################################
         * CASE 3 : RxNE中断。有以下各种情况：
         * 1. 进入RxNE中断，一定只剩下一个数据需要接收并且已经接收完成。设置STOP，并禁止中断，然后读取
         *    数据。
         * ##########################################################################################
         */
        //else if(SET == I2C_GetITStatus(pstI2CDevice->pstI2CPeriph, I2C_IT_RXNE))
        else if((pstI2CDevice->pstI2CPeriph->SR1 & I2C_IT_RXNE & 0x0000FFFF) && 
                (pstI2CDevice->pstI2CPeriph->CR2 & I2C_IT_BUF) && 
                (pstI2CDevice->pstI2CPeriph->CR2 & I2C_IT_EVT))
        {
            /* 产生停止位 */
            I2C_GenerateSTOP(pstI2CDevice->pstI2CPeriph, ENABLE) ;

            /* 最后一个数据读取 */
            *pstI2CDevice->pucBuf = I2C_ReceiveData(pstI2CDevice->pstI2CPeriph) ;

            /* 禁止中断 */
            I2C_ITConfig(pstI2CDevice->pstI2CPeriph, I2C_IT_EVT | I2C_IT_BUF, DISABLE) ;

            /* 收尾 */
            pstI2CDevice->enWorkMode = I2C_MODE_IDLE ;
            pstI2CDevice->pucBuf     = NULL ;
            pstI2CDevice->uiSize     = 0 ;        

            /* 释放读操作完成信号量 */
#ifdef DRV_I2C_USE_UCOS_III
            OSSemPost(&pstI2CDevice->stI2CRDSem, OS_OPT_POST_1, &enErr) ;
#else
            pstI2CDevice->ucI2CRDSem = DRV_I2C_RD_DONE ;
#endif
        }
        /* ##########################################################################################
         * CASE 4 : BTF中断。有以下各种情况：
         * 1. 进入BTF中断，一定是接收2个或者2个以上字节的数据。
         * 2. 如果剩余接收字节数（注意要减去DR和DSR中的2个数据）大于2，读取2个数据以清除BTF中断即可。
         * 3. 如果剩余接收字节数（注意要减去DR和DSR中的2个数据）等于2，需要设置POS和ACK，使得不响应
         *    下个数据，然后读取2个数据以清除BTF中断。
         * 4. 如果剩余接收字节数（注意要减去DR和DSR中的2个数据）等于1，需要设置POS和ACK，使得不响应
         *    本个数据，并使能RxNE中断，然后读取2个数据以清除BTF中断。
         * 5. 如果剩余接收字节数（注意要减去DR和DSR中的2个数据）等于0，需要设置STOP以产生STOP，并禁止
         *    中断，然后读取数据，清除工作区记录的消息。
         * ##########################################################################################
         */
        else if(SET == I2C_GetITStatus(pstI2CDevice->pstI2CPeriph, I2C_IT_BTF))
        {
            switch(pstI2CDevice->uiSize)
            {
                case 0 :   /* 异常情况 */
                case 1 :   /* 异常情况 */
                    {
                        /* 产生停止位 */
                        I2C_GenerateSTOP(pstI2CDevice->pstI2CPeriph, ENABLE) ;
                        
                        /* 禁止中断 */
                        I2C_ITConfig(pstI2CDevice->pstI2CPeriph, I2C_IT_EVT | I2C_IT_BUF, DISABLE) ;

                        I2C_ReceiveData(pstI2CDevice->pstI2CPeriph) ;
                        /* 等待第二个数据从DSR加载到DR */
                        while(RESET == I2C_GetFlagStatus(pstI2CDevice->pstI2CPeriph, I2C_FLAG_RXNE)) ;
                        I2C_ReceiveData(pstI2CDevice->pstI2CPeriph) ;

                        /* 收尾 */
                        pstI2CDevice->enWorkMode = I2C_MODE_IDLE ;
                        pstI2CDevice->pucBuf     = NULL ;
                        pstI2CDevice->uiSize     = 0 ;       

                        /* 释放读操作完成信号量 */
#ifdef DRV_I2C_USE_UCOS_III
                        OSSemPost(&pstI2CDevice->stI2CRDSem, OS_OPT_POST_1, &enErr) ;
#else
                        pstI2CDevice->ucI2CRDSem = DRV_I2C_RD_DONE ;
#endif
             
                    }
                    break ;
                case 2 :   /* 剩余数据等于0 */
                    {
                        /* 产生停止位 */
                        I2C_GenerateSTOP(pstI2CDevice->pstI2CPeriph, ENABLE) ;

                        /* 禁止中断 */
                        I2C_ITConfig(pstI2CDevice->pstI2CPeriph, I2C_IT_EVT | I2C_IT_BUF, DISABLE) ;

                        /* 数据读取 */
                        pstI2CDevice->pucBuf[0]  = I2C_ReceiveData(pstI2CDevice->pstI2CPeriph) ;
                        /* 等待第二个数据从DSR加载到DR */
                        while(RESET == I2C_GetFlagStatus(pstI2CDevice->pstI2CPeriph, I2C_FLAG_RXNE)) ;
                        pstI2CDevice->pucBuf[1]  = I2C_ReceiveData(pstI2CDevice->pstI2CPeriph) ;
                        pstI2CDevice->pucBuf    += 2 ;
                        pstI2CDevice->uiSize    -= 2 ;

                        /* 收尾 */
                        pstI2CDevice->enWorkMode = I2C_MODE_IDLE ;
                        pstI2CDevice->pucBuf     = NULL ;
                        pstI2CDevice->uiSize     = 0 ;        

                        /* 释放读操作完成信号量 */
#ifdef DRV_I2C_USE_UCOS_III
                        OSSemPost(&pstI2CDevice->stI2CRDSem, OS_OPT_POST_1, &enErr) ;
#else
                        pstI2CDevice->ucI2CRDSem = DRV_I2C_RD_DONE ;
#endif
                    }
                    break ;
                case 3 :   /* 剩余数据等于1 */
                    {
                        /* 禁止第一个接收数据的响应 */
                        I2C_PECPositionConfig(pstI2CDevice->pstI2CPeriph, I2C_PECPosition_Current) ;
                        I2C_AcknowledgeConfig(pstI2CDevice->pstI2CPeriph, DISABLE) ;

                        /* 使能BUF(RxNE)中断 */
                        I2C_ITConfig(pstI2CDevice->pstI2CPeriph, I2C_IT_BUF, ENABLE) ;

                        /* 读取2个数据以结束中断。下次中断是RxNE中断，并进入case 2 */
                        pstI2CDevice->pucBuf[0]  = I2C_ReceiveData(pstI2CDevice->pstI2CPeriph) ;
                        /* 等待第二个数据从DSR加载到DR */
                        while(RESET == I2C_GetFlagStatus(pstI2CDevice->pstI2CPeriph, I2C_FLAG_RXNE)) ;
                        pstI2CDevice->pucBuf[1]  = I2C_ReceiveData(pstI2CDevice->pstI2CPeriph) ;
                        pstI2CDevice->pucBuf    += 2 ;
                        pstI2CDevice->uiSize    -= 2 ;
                    }
                    break ;
                case 4 :   /* 剩余数据等于2 */
                    {
                        /* 禁止第二个接收数据的响应 */
                        I2C_PECPositionConfig(pstI2CDevice->pstI2CPeriph, I2C_PECPosition_Next) ;
                        I2C_AcknowledgeConfig(pstI2CDevice->pstI2CPeriph, DISABLE) ;

                        /* 读取2个数据以结束中断。下次中断还是BTF中断，并进入case 2 */
                        pstI2CDevice->pucBuf[0]  = I2C_ReceiveData(pstI2CDevice->pstI2CPeriph) ;
                        /* 等待第二个数据从DSR加载到DR */
                        while(RESET == I2C_GetFlagStatus(pstI2CDevice->pstI2CPeriph, I2C_FLAG_RXNE)) ;
                        pstI2CDevice->pucBuf[1]  = I2C_ReceiveData(pstI2CDevice->pstI2CPeriph) ;
                        pstI2CDevice->pucBuf    += 2 ;
                        pstI2CDevice->uiSize    -= 2 ;
                    }
                    break ;
                default :  /* 剩余数据大于2 */
                    {
                        /* 读取2个数据 */
                        pstI2CDevice->pucBuf[0]  = I2C_ReceiveData(pstI2CDevice->pstI2CPeriph) ;
                        /* 等待第二个数据从DSR加载到DR */
                        while(RESET == I2C_GetFlagStatus(pstI2CDevice->pstI2CPeriph, I2C_FLAG_RXNE)) ;
                        pstI2CDevice->pucBuf[1]  = I2C_ReceiveData(pstI2CDevice->pstI2CPeriph) ;
                        pstI2CDevice->pucBuf    += 2 ;
                        pstI2CDevice->uiSize    -= 2 ;
                    }
                    break ;
            }
        }
        else
        {
            /* 错误处理 */
        }
    }
    else
    {
        /* 错误的工作模式 */
        return I2C_ERROR_INTHANDLER_WORKMODE_ERROR ;
    }
    
    return I2C_ERROR_OK ;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file drv_i2c_stm32f4xx.c. *******/  
