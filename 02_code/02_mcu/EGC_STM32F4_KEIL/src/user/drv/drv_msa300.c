/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : drv_msa300.c
    Project name : 
    Module name  : 
    Date created : 2021年9月15日   11时44分45秒
    Author       : Ning.JianLi
    Description  : MSA300接口源文件。
*******************************************************************************/

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "drv_msa300.h"

#include "pub_type.h"
#include "pub_lib.h"

CODE_SECTION("=======================================") ;
CODE_SECTION("==  移植：修改以下代码以实现I2C访问  ==") ;
CODE_SECTION("=======================================") ;

#include "drv_i2c_stm32f4xx.h"

/* I2C驱动设备 */
extern DRV_I2C_DEVICE_S g_stI2CDevice ;

/*******************************************************************************
- Function    : __DRV_MSA_I2C_Write
- Description : 本函数实现针对HDC1080器件的I2C写入。
- Input       : ucRegAddr ：寄存器地址；
                pucBuf    ：数据指针；
                ucWrSize  ：写入字节数。
- Output      : NULL
- Return      : DRV_MSA_ERROR_CODE_E ：错误码：
                MSA_ERROR_OK          ：写入正常。
                MSA_ERROR_WRITE_ERROR ：写入失败。
- Others      : 
*******************************************************************************/
DRV_MSA_ERROR_CODE_E __DRV_MSA_I2C_Write(UCHAR ucRegAddr, UCHAR *pucBuf, UCHAR ucWrSize)
{
    DRV_I2C_ERROR_CODE_E enI2COpRetVal ;
    
    enI2COpRetVal = DRV_I2C_Write(&g_stI2CDevice, DRV_MSA300_BUS_ADDR, ucRegAddr, pucBuf, ucWrSize) ;

    if(I2C_ERROR_OK != enI2COpRetVal)
    {
        return MSA_ERROR_WRITE_ERROR ;
    }
    else
    {
        return MSA_ERROR_OK ;
    }
}


/*******************************************************************************
- Function    : __DRV_MSA_I2C_Read
- Description : 本函数实现针对HDC1080器件的I2C读取。
- Input       : ucRegAddr ：寄存器地址；
                ucWrSize  ：读取字节数。
- Output      : pucBuf    ：数据指针；
- Return      : DRV_MSA_ERROR_CODE_E ：错误码：
                MSA_ERROR_OK         ：读取正常。
                MSA_ERROR_READ_ERROR ：读取失败。
- Others      : 
*******************************************************************************/
DRV_MSA_ERROR_CODE_E __DRV_MSA_I2C_Read(UCHAR ucRegAddr, UCHAR *pucBuf, UCHAR ucRdSize)
{
    DRV_I2C_ERROR_CODE_E enI2COpRetVal ;

    enI2COpRetVal = DRV_I2C_Read(&g_stI2CDevice, DRV_MSA300_BUS_ADDR, ucRegAddr, pucBuf, ucRdSize, 0) ;

    if(I2C_ERROR_OK != enI2COpRetVal)
    {
        return MSA_ERROR_READ_ERROR ;
    }
    else
    {
        return MSA_ERROR_OK ;
    }
}

CODE_SECTION("=======================================") ;
CODE_SECTION("==  移植：修改结束                   ==") ;
CODE_SECTION("=======================================") ;

CODE_SECTION("=======================================") ;
CODE_SECTION("==  内部函数                         ==") ;
CODE_SECTION("=======================================") ;

/*******************************************************************************
- Function    : __DRV_MSA_TailLeadZeros
- Description : 本函数检查一个输入数据,从最低位到最高位,遇到的第一个bit '1'之前,
                一共有多少个bit '0'.
- Input       : uiData : 要检查的数据.
- Output      : NULL 
- Return      : UCHAR  : 从低位到高位,第一个出现的bit '1'之前,一共出现的0的个数.
- Others      : 特殊情况 : 如果uiData为0,则返回为0.
*******************************************************************************/
UCHAR __DRV_MSA_TailLeadZeros(UINT uiData)
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
- Function    : __DRV_MSA_GPIOConfig
- Description : 本函数初始化MSA300芯片使用到的STM32引脚。MSA300一共使用3个引脚，
                分别是I2C_SDA，I2C_SCL，INT0。前两者在I2C做初始化，这里仅初始化
                INT0使用的引脚。INT0引脚作为中断输入。
- Input       : pstExtIntGPIOGroup : 指定INT0功能引脚的GPIO外设编号。
                uiExtIntGPIOIndex  : 指定INT0功能引脚的GPIO编号。
- Output      : NULL
- Return      : DRV_MSA_ERROR_CODE_E ：错误码：
                MSA_ERROR_OK         ：读取正常。
                其他值                  ：查看定义。
- Others      :
*******************************************************************************/
DRV_MSA_ERROR_CODE_E __DRV_MSA_GPIOConfig(GPIO_TypeDef *pstExtIntGPIOGroup, UINT uiExtIntGPIOIndex)
{
    GPIO_InitTypeDef stGPIOInitInfo ;
    UINT             uiAHBAPBPeriphClockCtrl = 0 ;

    if(0 == IS_GPIO_ALL_PERIPH(pstExtIntGPIOGroup))
    {
        return MSA_ERROR_GPIOCONFIG_INVALID_GPIO_GROUP ;
    }

    if(0 == IS_GPIO_PIN(uiExtIntGPIOIndex))
    {
        return MSA_ERROR_GPIOCONFIG_INVALID_GPIO_INDEX ;
    }

    /* 使能GPIO时钟 */
    switch((UINT)pstExtIntGPIOGroup)
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

    /* 使能GPIO时钟 */
    RCC_AHB1PeriphClockCmd(uiAHBAPBPeriphClockCtrl, ENABLE) ;
    
    /* 配置GPIO引脚 */
    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_IN ;
    stGPIOInitInfo.GPIO_OType = GPIO_OType_PP ;
    stGPIOInitInfo.GPIO_PuPd  = GPIO_PuPd_NOPULL ;
    stGPIOInitInfo.GPIO_Pin   = uiExtIntGPIOIndex ;
    stGPIOInitInfo.GPIO_Speed = GPIO_Speed_50MHz ;

    GPIO_Init(pstExtIntGPIOGroup, &stGPIOInitInfo) ;

    return MSA_ERROR_OK ;
}

/*******************************************************************************
- Function    : __DRV_MSA_EXTIConfig
- Description : 本函数初始化MSA300芯片INT0引脚作为中断输入时，STM32的EXTI相关的
                配置。函数先配置EXTI的中断源和中断线，中断触发属性，然后在NVIC
                中允许该中断。
- Input       : pstExtIntGPIOGroup : 指定INT0功能引脚的GPIO外设编号。
                uiEXTILine         : 指定INT0功能引脚的外部中断线编号。
- Output      : NULL
- Return      : DRV_MSA_ERROR_CODE_E : 错误码。
                    MSA_ERROR_OK     : 读取正常。
                    其他值              : 查看定义。
- Others      :
*******************************************************************************/
DRV_MSA_ERROR_CODE_E __DRV_MSA_EXTIConfig(GPIO_TypeDef *pstExtIntGPIOGroup, UINT uiEXTILine)
{
    EXTI_InitTypeDef stEXTIInitInfo ;
    NVIC_InitTypeDef stNVICInitInfo ;

    if(0 == IS_GPIO_ALL_PERIPH(pstExtIntGPIOGroup))
    {
        return MSA_ERROR_EXTICONFIG_INVALID_GPIO_GROUP ;
    }
    
    if(0 == IS_GET_EXTI_LINE(uiEXTILine))
    {
        return MSA_ERROR_EXTICONFIG_INVALID_EXTI_LINE ;
    }
    else if(uiEXTILine > EXTI_Line15)
    {
        return MSA_ERROR_EXTICONFIG_INVALID_EXTI_LINE ;
    }

    /* 中断配置 */
    stEXTIInitInfo.EXTI_Line    = uiEXTILine ;
    stEXTIInitInfo.EXTI_LineCmd = ENABLE ;
    stEXTIInitInfo.EXTI_Mode    = EXTI_Mode_Interrupt ;
    stEXTIInitInfo.EXTI_Trigger = EXTI_Trigger_Rising ;

    EXTI_Init(&stEXTIInitInfo) ;

    switch(uiEXTILine)
    {
        case EXTI_Line0 :
            stNVICInitInfo.NVIC_IRQChannel = EXTI0_IRQn ;
            break ;
        case EXTI_Line1 :
            stNVICInitInfo.NVIC_IRQChannel = EXTI1_IRQn ;
            break ;
        case EXTI_Line2 :
            stNVICInitInfo.NVIC_IRQChannel = EXTI2_IRQn ;
            break ;
        case EXTI_Line3 :
            stNVICInitInfo.NVIC_IRQChannel = EXTI3_IRQn ;
            break ;
        case EXTI_Line4 :
            stNVICInitInfo.NVIC_IRQChannel = EXTI4_IRQn ;
            break ;
        case EXTI_Line5 :
        case EXTI_Line6 :
        case EXTI_Line7 :
        case EXTI_Line8 :
        case EXTI_Line9 :
            stNVICInitInfo.NVIC_IRQChannel = EXTI9_5_IRQn ;
            break ;
        case EXTI_Line10 :
        case EXTI_Line11 :
        case EXTI_Line12 :
        case EXTI_Line13 :
        case EXTI_Line14 :
        case EXTI_Line15 :
            stNVICInitInfo.NVIC_IRQChannel = EXTI15_10_IRQn ;
            break ;
    }

    stNVICInitInfo.NVIC_IRQChannelCmd = ENABLE ;

    NVIC_Init(&stNVICInitInfo);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);
    SYSCFG_EXTILineConfig(((UINT)pstExtIntGPIOGroup - (UINT)GPIOA)/((UINT)GPIOB - (UINT)GPIOA), 
                          __DRV_MSA_TailLeadZeros(uiEXTILine)) ;

    return MSA_ERROR_OK ;
}

/*******************************************************************************
- Function    : __DRV_MSA_GetInterruptFlag
- Description : 本函数获取MSA300芯片内部的中断状态。
- Input       : enBit    : 指定查询的中断状态位。
- Output      : pbStatus : 返回状态。
                    BOOL_FALSE : 由enBit指定的中断未发生。
                    BOOL_TRUE  : 由enBit指定的中断发生。
- Return      : DRV_MSA_ERROR_CODE_E : 错误码。
                    MSA_ERROR_OK     : 读取正常。
                    其他值              : 查看定义。
- Others      :
*******************************************************************************/
DRV_MSA_ERROR_CODE_E __DRV_MSA_GetInterruptFlag(DRV_MSA300_INT_BIT_E enBit, BOOL_T *pbStatus)
{
    UCHAR                ucDataTmp ;
    DRV_MSA_ERROR_CODE_E enCallRetVal ;

    /* 读取数值 */
    if(INT_BIT_NEW_DATA != enBit)
    {
        enCallRetVal = __DRV_MSA_I2C_Read(DRV_MSA300_REG_ADDR_MOTION_INTERRUPT, &ucDataTmp, sizeof(ucDataTmp));
    }
    else
    {
        enCallRetVal = __DRV_MSA_I2C_Read(DRV_MSA300_REG_ADDR_DATA_INTERRUPT, &ucDataTmp, sizeof(ucDataTmp));

        ucDataTmp = (ucDataTmp << 7) ;
    }

    if(MSA_ERROR_OK != enCallRetVal)
    {
        return MSA_ERROR_GETINTERRUPTFLAG_I2C_READ_FAIL ;
    }

    /* 返回状态 */
    if(0 != (ucDataTmp & enBit))
    {
        *pbStatus = BOOL_TRUE ;
    }
    else
    {
        *pbStatus = BOOL_FALSE ;
    }

    return MSA_ERROR_OK ;
}

/*******************************************************************************
- Function    : DRV_MSA_ReadAcceleration
- Description : 本函数读取MSA300的加速度测量值。
- Input       : NULL
- Output      : pfXAxis :
                    用于保存X轴加速度值的指针；
                pfYAxis ：
                    用于保存Y轴加速度值的指针；
                pfZAxis ：
                    用于保存Z轴加速度值的指针；
- Return      : DRV_MSA_ERROR_CODE_E : 错误码。
                    MSA_ERROR_OK     : 读取正常。
                    其他值              : 查看定义。
- Others      : 
*******************************************************************************/
DRV_MSA_ERROR_CODE_E DRV_MSA_ReadAcceleration(FLOAT *pfXAxis, FLOAT *pfYAxis, FLOAT *pfZAxis)
{
    signed CHAR          acTmp[2] ;
    DRV_MSA_ERROR_CODE_E enCallRetVal ;

    /* X-Axis */
    enCallRetVal = __DRV_MSA_I2C_Read(DRV_MSA300_REG_ADDR_X_AXIS, (UCHAR *)acTmp, sizeof(acTmp));

    if(MSA_ERROR_OK != enCallRetVal)
    {
        return MSA_ERROR_READACCELERATION_READ_X_FAIL ;
    }

    *pfXAxis = ((FLOAT)acTmp[1]) * 0.0156f ;

    /* Y-Axis */
    enCallRetVal = __DRV_MSA_I2C_Read(DRV_MSA300_REG_ADDR_Y_AXIS, (UCHAR *)acTmp, sizeof(acTmp));

    if(MSA_ERROR_OK != enCallRetVal)
    {
        return MSA_ERROR_READACCELERATION_READ_Y_FAIL ;
    }

    *pfYAxis = ((FLOAT)acTmp[1]) * 0.0156f ;

    /* Z-Axis */
    enCallRetVal = __DRV_MSA_I2C_Read(DRV_MSA300_REG_ADDR_Z_AXIS, (UCHAR *)acTmp, sizeof(acTmp));

    if(MSA_ERROR_OK != enCallRetVal)
    {
        return MSA_ERROR_READACCELERATION_READ_Z_FAIL ;
    }

    *pfZAxis = ((FLOAT)acTmp[1]) * 0.0156f ;

    return MSA_ERROR_OK ;
}

/*******************************************************************************
- Function    : DRV_MSA_SensitivitySet
- Description : 本函数初始化MSA300芯片Active中断产生的阈值。
- Input       : ucSense : 触碰敏感度（加速度）g值。以3.81mg为单位。
- Output      : NULL
- Return      : DRV_MSA_ERROR_CODE_E : 错误码。
                    MSA_ERROR_OK     : 读取正常。
                    其他值              : 查看定义。
- Others      : 
*******************************************************************************/
DRV_MSA_ERROR_CODE_E DRV_MSA_SensitivitySet(UCHAR ucSense)
{
    DRV_MSA_ERROR_CODE_E enCallRetVal ;
    
    enCallRetVal = __DRV_MSA_I2C_Write(DRV_MSA300_REG_ADDR_ACTIVE_TH, &ucSense, sizeof(ucSense)) ;

    if(MSA_ERROR_OK != enCallRetVal)
    {
        return MSA_ERROR_SENSITIVITYSET_I2C_WRITE_FAIL ;
    }
    
    return MSA_ERROR_OK ;
} ;

/*******************************************************************************
- Function    : DRV_MSA_Init
- Description : 本函数初始化MSA300芯片。
                1、工作模式配置为normal模式，连续工作；
                2、设置分辨率为14bit，量程为+/-2g；
                3、设置Active初始灵敏度；
                4、配置INT0中断，配置该中断的相关电气属性，并将Active中断映射到其上；
                5、配置中断锁定时间为1s，避免频繁中断。
- Input       : pstExtIntGPIOGroup : 外部中断引脚的GPIO外设；
                uiExtIntGPIOIndex  : 外部中断引脚的GPIO；
                uiEXTILine         : 外部中断线指定。
- Output      : NULL
- Return      : DRV_MSA_ERROR_CODE_E : 错误码。
                    MSA_ERROR_OK     : 读取正常。
                    其他值              : 查看定义。
- Others      :
*******************************************************************************/
DRV_MSA_ERROR_CODE_E DRV_MSA_Init(GPIO_TypeDef *pstExtIntGPIOGroup, UINT uiExtIntGPIOIndex, UINT uiEXTILine)
{
    DRV_MSA300_REG_POWER_BW_S         stMSA300RegPowerModeBandwidth ; 
    DRV_MSA300_REG_RESOLUTION_RANGE_S stMSA300RegResolutionRange ;
    DRV_MSA300_REG_INT_SET0_S         stMSA300RegIntSet0 ;
    DRV_MSA300_REG_INT_MAP0_S         stMSA300RegIntMap0 ;
    DRV_MSA300_REG_INT_MAP3_S         stMSA300RegIntMap3 ;
    DRV_MSA300_REG_INT_LATCH_S        stMSA300RegIntLatch ;
    DRV_MSA_ERROR_CODE_E              enCallRetVal ;

    /* power mode 配置，此处配置为连续工作模式 */
    enCallRetVal = __DRV_MSA_I2C_Read(DRV_MSA300_REG_ADDR_POWER_MODE_BANDWIDTH,
                                      &stMSA300RegPowerModeBandwidth.ucRawData,
                                      sizeof(DRV_MSA300_REG_POWER_BW_S)) ;

    if(MSA_ERROR_OK != enCallRetVal)
    {
        return MSA_ERROR_INIT_READ_POWER_BW_FAIL ;
    }

    stMSA300RegPowerModeBandwidth.enPWRMode = POWER_MODE_DEF_NORMAL ; /* normal mode */

    enCallRetVal = __DRV_MSA_I2C_Write(DRV_MSA300_REG_ADDR_POWER_MODE_BANDWIDTH, 
                                       &stMSA300RegPowerModeBandwidth.ucRawData,
                                       sizeof(DRV_MSA300_REG_POWER_BW_S)) ;

    if(MSA_ERROR_OK != enCallRetVal)
    {
        return MSA_ERROR_INIT_WRITE_POWER_BW_FAIL ;
    }

    /* 分辨率和量程设置 */
    stMSA300RegResolutionRange.ucRawData    = 0x00 ;
    stMSA300RegResolutionRange.enRange      = RANGE_DEF_4G ;           /* +/-2G */
    stMSA300RegResolutionRange.enResolution = RESOLUTION_DEF_14BIT ;   /* 14bit */

    enCallRetVal = __DRV_MSA_I2C_Write(DRV_MSA300_REG_ADDR_RESOLUTION_RANGE,
                                       &stMSA300RegResolutionRange.ucRawData,
                                       sizeof(DRV_MSA300_REG_RESOLUTION_RANGE_S)) ;

    if(MSA_ERROR_OK != enCallRetVal)
    {
        return MSA_ERROR_INIT_WRITE_RESOLUTION_FAIL ;
    }

    /* ACTIVE门限设置 */
    enCallRetVal = DRV_MSA_SensitivitySet(20) ;

    if(MSA_ERROR_OK != enCallRetVal)
    {
        return MSA_ERROR_INIT_SET_SENSITIVITY_FAIL ;
    }

    /* ACTIVE中断配置 */
    stMSA300RegIntSet0.ucRawData      = 0 ;
    stMSA300RegIntSet0.ucActiveIntEnX = 1 ;
    stMSA300RegIntSet0.ucActiveIntEnY = 1 ;
    stMSA300RegIntSet0.ucActiveIntEnZ = 1 ;

    enCallRetVal = __DRV_MSA_I2C_Write(DRV_MSA300_REG_ADDR_INT_SET_0, 
                                       &stMSA300RegIntSet0.ucRawData,
                                       sizeof(DRV_MSA300_REG_INT_SET0_S)) ;

    if(MSA_ERROR_OK != enCallRetVal)
    {
        return MSA_ERROR_INIT_WRITE_INT_SET_FAIL ;
    }


    /* ACTIVE中断映射到INT0 */
    stMSA300RegIntMap0.ucRawData    = 0 ;
    stMSA300RegIntMap0.ucInt1Active = 1 ;

    enCallRetVal = __DRV_MSA_I2C_Write(DRV_MSA300_REG_ADDR_INT_MAP_0, 
                                       &stMSA300RegIntMap0.ucRawData,
                                       sizeof(DRV_MSA300_REG_INT_MAP0_S)) ;

    if(MSA_ERROR_OK != enCallRetVal)
    {
        return MSA_ERROR_INIT_WRITE_INT_MAP0_FAIL ;
    }

    /* INT0中断配置为推挽模式，高电平有效 */
    stMSA300RegIntMap3.ucRawData       = 0 ;
    stMSA300RegIntMap3.ucInt1OpenDrain = 0 ;
    stMSA300RegIntMap3.ucInt1Level     = 1 ;

    enCallRetVal = __DRV_MSA_I2C_Write(DRV_MSA300_REG_ADDR_INT_MAP_3, 
                                       &stMSA300RegIntMap3.ucRawData,
                                       sizeof(DRV_MSA300_REG_INT_MAP3_S)) ;

    if(MSA_ERROR_OK != enCallRetVal)
    {
        return MSA_ERROR_INIT_WRITE_INT_MAP3_FAIL ;
    }
                         
    /* 中断信号锁定时间为1s */
    stMSA300RegIntLatch.ucRawData  = 0 ;
    stMSA300RegIntLatch.enLatchInt = LATCH_INT_DEF_1S ;

    enCallRetVal = __DRV_MSA_I2C_Write(DRV_MSA300_REG_ADDR_INT_LATCH, 
                                       &stMSA300RegIntLatch.ucRawData,
                                       sizeof(DRV_MSA300_REG_INT_LATCH_S)) ;

    if(MSA_ERROR_OK != enCallRetVal)
    {
        return MSA_ERROR_INIT_WRITE_INT_LATCH_FAIL ;
    }

    /* GPIO中断设置 */
    enCallRetVal = __DRV_MSA_GPIOConfig(pstExtIntGPIOGroup, uiExtIntGPIOIndex) ;

    if(MSA_ERROR_OK != enCallRetVal)
    {
        return MSA_ERROR_INIT_CONFIG_GPIO_FAIL ;
    }

    enCallRetVal =__DRV_MSA_EXTIConfig(pstExtIntGPIOGroup, uiEXTILine) ;

    if(MSA_ERROR_OK != enCallRetVal)
    {
        return MSA_ERROR_INIT_CONFIG_EXTI_FAIL ;
    }
    
    return MSA_ERROR_OK ;
}

/*******************************************************************************
- Function    : EXTI0_IRQHandler
- Description : 外部中断的中断处理函数。函数清除外部中断。
- Input       : uiEXTILine         : 指定INT0功能引脚的外部中断线编号。
- Output      : NULL
- Return      : DRV_MSA_ERROR_CODE_E : 错误码。
                    MSA_ERROR_OK     : 读取正常。
                    其他值              : 查看定义。
- Others      :
*******************************************************************************/
DRV_MSA_ERROR_CODE_E DRV_MSA_IRQHandler(UINT uiEXTILine)
{
    if(0 == IS_GET_EXTI_LINE(uiEXTILine))
    {
        return MSA_ERROR_INIT_INVALID_EXTI_LINE ;
    }
    else if(uiEXTILine > EXTI_Line15)
    {
        return MSA_ERROR_INIT_INVALID_EXTI_LINE ;
    }

    /* 清除外部中断 */
    EXTI_ClearITPendingBit(uiEXTILine) ;

    return MSA_ERROR_OK ;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file drv_msa300.c. *******/  

