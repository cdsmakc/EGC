/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : drv_sdram_stm32f4xx.c
    Project name : 
    Module name  : 
    Date created : 2021年9月7日   15时30分2秒
    Author       : Ning.JianLi
    Description  : 基于STM32F4XX系列MCU的SDRAM驱动。
*******************************************************************************/

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "drv_sdram_stm32f4xx.h"

/*******************************************************************************
- Function    : __TailLeadZeros
- Description : 本函数检查一个输入数据,从最低位到最高位,遇到的第一个bit '1'之前,
                一共有多少个bit '0'.
- Input       : uiData : 要检查的数据.
- Output      : NULL 
- Return      : UCHAR  : 从低位到高位,第一个出现的bit '1'之前,一共出现的0的个数.
- Others      : 特殊情况 : 如果uiData为0,则返回为0.
*******************************************************************************/
STATIC UCHAR __TailLeadZeros(UINT uiData)
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
- Function    : __DRV_SDR_GPIOConfig
- Description : 配置FMC控制SDRAM的相关引脚。这些引脚包括地址线A，BA，数据线DQ，
                时钟线CK和控制线RAS/CAS/WE/CKE/NCS等。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __DRV_SDR_GPIOConfig(VOID)
{
    GPIO_InitTypeDef stGPIOInitInfo ;
    
    /* 使能所有的GPIO时钟，因为SDRAM和LTDC的引脚比较多，分布在多个BANK中 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE) ;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE) ;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE) ;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE) ;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE) ;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE) ;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE) ;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE) ;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE) ;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOJ, ENABLE) ;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOK, ENABLE) ;

    /* 公共部分 */
    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_AF ;      /* 复用功能 */
    stGPIOInitInfo.GPIO_Speed = GPIO_Speed_100MHz ; /* 速度 */
    stGPIOInitInfo.GPIO_PuPd  = GPIO_PuPd_NOPULL ;  /* 上下拉模式 */
    stGPIOInitInfo.GPIO_OType = GPIO_OType_PP ;     /* 输出模式 */

    /* GPIO配置 */
CODE_SECTION("===============================") ;
CODE_SECTION("==  Address 地址线配置       ==") ;
CODE_SECTION("===============================") ;
    /* Address 地址线配置 */
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_A0_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_A0_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_A1_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_A1_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_A2_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_A2_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_A3_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_A3_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_A4_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_A4_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_A5_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_A5_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_A6_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_A6_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_A7_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_A7_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_A8_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_A8_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_A9_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_A9_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_A10_GPIO_PIN_ID ; GPIO_Init(DRV_SDR_A10_PIN_GROUP_ID, &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_A11_GPIO_PIN_ID ; GPIO_Init(DRV_SDR_A11_PIN_GROUP_ID, &stGPIOInitInfo) ;
#ifdef DRV_SDR_DEV_MT48LC16M16A2_6A
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_A12_GPIO_PIN_ID ; GPIO_Init(DRV_SDR_A12_PIN_GROUP_ID, &stGPIOInitInfo) ;
#endif

    /* 复用配置 */
    GPIO_PinAFConfig(DRV_SDR_A0_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_A0_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_A1_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_A1_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_A2_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_A2_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_A3_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_A3_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_A4_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_A4_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_A5_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_A5_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_A6_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_A6_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_A7_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_A7_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_A8_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_A8_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_A9_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_A9_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_A10_PIN_GROUP_ID, __TailLeadZeros(DRV_SDR_A10_GPIO_PIN_ID), GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_A11_PIN_GROUP_ID, __TailLeadZeros(DRV_SDR_A11_GPIO_PIN_ID), GPIO_AF_FMC) ;
#ifdef DRV_SDR_DEV_MT48LC16M16A2_6A
    GPIO_PinAFConfig(DRV_SDR_A12_PIN_GROUP_ID, __TailLeadZeros(DRV_SDR_A12_GPIO_PIN_ID), GPIO_AF_FMC) ;
#endif
    
CODE_SECTION("===============================") ;
CODE_SECTION("==  Bank    地址线配置       ==") ;
CODE_SECTION("===============================") ;
    /* Bank 地址线配置 */
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_BA0_GPIO_PIN_ID ; GPIO_Init(DRV_SDR_BA0_PIN_GROUP_ID, &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_BA1_GPIO_PIN_ID ; GPIO_Init(DRV_SDR_BA1_PIN_GROUP_ID, &stGPIOInitInfo) ;

    /* 复用配置 */
    GPIO_PinAFConfig(DRV_SDR_BA0_PIN_GROUP_ID, __TailLeadZeros(DRV_SDR_BA0_GPIO_PIN_ID), GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_BA1_PIN_GROUP_ID, __TailLeadZeros(DRV_SDR_BA1_GPIO_PIN_ID), GPIO_AF_FMC) ;

CODE_SECTION("===============================") ;
CODE_SECTION("==  Data    数据线配置       ==") ;
CODE_SECTION("===============================") ;
    /* Data 数据线配置 */
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_D0_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_D0_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_D1_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_D1_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_D2_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_D2_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_D3_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_D3_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_D4_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_D4_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_D5_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_D5_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_D6_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_D6_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_D7_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_D7_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_D8_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_D8_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_D9_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_D9_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_D10_GPIO_PIN_ID ; GPIO_Init(DRV_SDR_D10_PIN_GROUP_ID, &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_D11_GPIO_PIN_ID ; GPIO_Init(DRV_SDR_D11_PIN_GROUP_ID, &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_D12_GPIO_PIN_ID ; GPIO_Init(DRV_SDR_D12_PIN_GROUP_ID, &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_D13_GPIO_PIN_ID ; GPIO_Init(DRV_SDR_D13_PIN_GROUP_ID, &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_D14_GPIO_PIN_ID ; GPIO_Init(DRV_SDR_D14_PIN_GROUP_ID, &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_D15_GPIO_PIN_ID ; GPIO_Init(DRV_SDR_D15_PIN_GROUP_ID, &stGPIOInitInfo) ;

    /* 复用配置 */
    GPIO_PinAFConfig(DRV_SDR_D0_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_D0_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_D1_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_D1_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_D2_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_D2_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_D3_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_D3_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_D4_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_D4_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_D5_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_D5_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_D6_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_D6_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_D7_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_D7_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_D8_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_D8_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_D9_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_D9_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_D10_PIN_GROUP_ID, __TailLeadZeros(DRV_SDR_D10_GPIO_PIN_ID), GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_D11_PIN_GROUP_ID, __TailLeadZeros(DRV_SDR_D11_GPIO_PIN_ID), GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_D12_PIN_GROUP_ID, __TailLeadZeros(DRV_SDR_D12_GPIO_PIN_ID), GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_D13_PIN_GROUP_ID, __TailLeadZeros(DRV_SDR_D13_GPIO_PIN_ID), GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_D14_PIN_GROUP_ID, __TailLeadZeros(DRV_SDR_D14_GPIO_PIN_ID), GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_D15_PIN_GROUP_ID, __TailLeadZeros(DRV_SDR_D15_GPIO_PIN_ID), GPIO_AF_FMC) ;

CODE_SECTION("===============================") ;
CODE_SECTION("==  控制线配置               ==") ;
CODE_SECTION("===============================") ;
    /* 控制线配置 */
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_CK_GPIO_PIN_ID   ; GPIO_Init(DRV_SDR_CK_PIN_GROUP_ID  , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_CKE_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_CKE_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_RAS_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_RAS_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_CAS_GPIO_PIN_ID  ; GPIO_Init(DRV_SDR_CAS_PIN_GROUP_ID , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_WE_GPIO_PIN_ID   ; GPIO_Init(DRV_SDR_WE_PIN_GROUP_ID  , &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_CS_GPIO_PIN_ID   ; GPIO_Init(DRV_SDR_CS_PIN_GROUP_ID  , &stGPIOInitInfo) ;
#ifdef DRV_SDR_USE_DQM_SIGNAL
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_DQM0_GPIO_PIN_ID ; GPIO_Init(DRV_SDR_DQM0_PIN_GROUP_ID, &stGPIOInitInfo) ;
    stGPIOInitInfo.GPIO_Pin   = DRV_SDR_DQM1_GPIO_PIN_ID ; GPIO_Init(DRV_SDR_DQM1_PIN_GROUP_ID, &stGPIOInitInfo) ;
#endif

    /* 复用配置 */
    GPIO_PinAFConfig(DRV_SDR_CK_PIN_GROUP_ID  , __TailLeadZeros(DRV_SDR_CK_GPIO_PIN_ID)  , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_CKE_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_CKE_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_RAS_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_RAS_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_CAS_PIN_GROUP_ID , __TailLeadZeros(DRV_SDR_CAS_GPIO_PIN_ID) , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_WE_PIN_GROUP_ID  , __TailLeadZeros(DRV_SDR_WE_GPIO_PIN_ID)  , GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_CS_PIN_GROUP_ID  , __TailLeadZeros(DRV_SDR_CS_GPIO_PIN_ID)  , GPIO_AF_FMC) ;
#ifdef DRV_SDR_USE_DQM_SIGNAL
    GPIO_PinAFConfig(DRV_SDR_DQM0_PIN_GROUP_ID, __TailLeadZeros(DRV_SDR_DQM0_GPIO_PIN_ID), GPIO_AF_FMC) ;
    GPIO_PinAFConfig(DRV_SDR_DQM1_PIN_GROUP_ID, __TailLeadZeros(DRV_SDR_DQM1_GPIO_PIN_ID), GPIO_AF_FMC) ;
#endif

    return ;
}


/*******************************************************************************
- Function    : DRV_SDR_Init
- Description : 配置FMC控制SDRAM。函数分为以下步骤：
                1、配置GPIO引脚；
                2、使能FMC时钟；
                3、配置SDRAM时序以及工作相关参数；
                4、启动SDRAM上电初始化序列；
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID DRV_SDR_Init(VOID)
{
    UINT                       uiLoop ;
    FMC_SDRAMInitTypeDef       stSDRAMInitInfo ;
    FMC_SDRAMTimingInitTypeDef stSDRAMTimingInitInfo ;
    FMC_SDRAMCommandTypeDef    stSDRAMCmdInfo ;

    /* GPIO引脚配置 */
    __DRV_SDR_GPIOConfig() ;

    /* 使能FMC时钟 */
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FMC, ENABLE);
    
    /* SDRAM基本设置 */
    stSDRAMInitInfo.FMC_SDRAMTimingStruct          = &stSDRAMTimingInitInfo ;
    stSDRAMInitInfo.FMC_Bank                       = DRV_SDR_FMC_BANK_POSITION ;
    stSDRAMInitInfo.FMC_ColumnBitsNumber           = DRV_SDR_PARAM_COL_BIT_NUM ;
    stSDRAMInitInfo.FMC_RowBitsNumber              = DRV_SDR_PARAM_ROW_BIT_NUM ;
    stSDRAMInitInfo.FMC_SDMemoryDataWidth          = DRV_SDR_PARAM_DATA_BIT_NUM ;
    stSDRAMInitInfo.FMC_InternalBankNumber         = DRV_SDR_PARAM_BANK_NUM ;
    stSDRAMInitInfo.FMC_CASLatency                 = DRV_SDR_PARAM_CAS ;
    stSDRAMInitInfo.FMC_WriteProtection            = DRV_SDR_FMC_WRITE_PROTECTION ;
    stSDRAMInitInfo.FMC_SDClockPeriod              = DRV_SDR_FMC_SDCLK_PERIOD ;
    stSDRAMInitInfo.FMC_ReadBurst                  = DRV_SDR_FMC_READ_BURST ;
    stSDRAMInitInfo.FMC_ReadPipeDelay              = DRV_SDR_FMC_READ_PIPE_DELAY ;

    /* 时序设置 */
    stSDRAMTimingInitInfo.FMC_LoadToActiveDelay    = DRV_SDR_PARAM_T_MRD ;
    stSDRAMTimingInitInfo.FMC_ExitSelfRefreshDelay = DRV_SDR_PARAM_T_XSR ;
    stSDRAMTimingInitInfo.FMC_SelfRefreshTime      = DRV_SDR_PARAM_T_RAS ;
    stSDRAMTimingInitInfo.FMC_RowCycleDelay        = DRV_SDR_PARAM_T_RC ;
    stSDRAMTimingInitInfo.FMC_WriteRecoveryTime    = DRV_SDR_PARAM_T_WR ;
    stSDRAMTimingInitInfo.FMC_RPDelay              = DRV_SDR_PARAM_T_RP ;
    stSDRAMTimingInitInfo.FMC_RCDDelay             = DRV_SDR_PARAM_T_RCD ;

    /* 初始化SDRAM */
    FMC_SDRAMInit(&stSDRAMInitInfo) ;

    /* SDRAM供给时钟 */
    stSDRAMCmdInfo.FMC_CommandMode                 = FMC_Command_Mode_CLK_Enabled ;
    stSDRAMCmdInfo.FMC_CommandTarget               = DRV_SDR_FMC_CMD_TARGET ;
    stSDRAMCmdInfo.FMC_ModeRegisterDefinition      = 0 ;
    stSDRAMCmdInfo.FMC_AutoRefreshNumber           = 1 ;

    /* 等待SDRAM空闲 */
    while (FMC_GetFlagStatus(DRV_SDR_FMC_BANK_POSITION, FMC_FLAG_Busy) != RESET);

    FMC_SDRAMCmdConfig(&stSDRAMCmdInfo);

    /* 等待 */
    uiLoop = 1000000 ;

    while(uiLoop--) ;

    /* 所有BANK充电 */
    stSDRAMCmdInfo.FMC_CommandMode                 = FMC_Command_Mode_PALL ;
    stSDRAMCmdInfo.FMC_CommandTarget               = DRV_SDR_FMC_CMD_TARGET ;
    stSDRAMCmdInfo.FMC_ModeRegisterDefinition      = 0 ;
    stSDRAMCmdInfo.FMC_AutoRefreshNumber           = 1 ;
    /* 等待SDRAM空闲 */
    while (FMC_GetFlagStatus(DRV_SDR_FMC_BANK_POSITION, FMC_FLAG_Busy) != RESET);

    FMC_SDRAMCmdConfig(&stSDRAMCmdInfo);

    /* 自动刷新 */
    stSDRAMCmdInfo.FMC_CommandMode                 = FMC_Command_Mode_AutoRefresh ;
    stSDRAMCmdInfo.FMC_CommandTarget               = DRV_SDR_FMC_CMD_TARGET ;
    stSDRAMCmdInfo.FMC_ModeRegisterDefinition      = 0 ;
    stSDRAMCmdInfo.FMC_AutoRefreshNumber           = 4 ;
    /* 等待SDRAM空闲 */
    while (FMC_GetFlagStatus(DRV_SDR_FMC_BANK_POSITION, FMC_FLAG_Busy) != RESET);

    FMC_SDRAMCmdConfig(&stSDRAMCmdInfo);

    while (FMC_GetFlagStatus(DRV_SDR_FMC_BANK_POSITION, FMC_FLAG_Busy) != RESET);

    FMC_SDRAMCmdConfig(&stSDRAMCmdInfo);

    /* 加载模式寄存器 */
    stSDRAMCmdInfo.FMC_CommandMode                 = FMC_Command_Mode_LoadMode ;
    stSDRAMCmdInfo.FMC_CommandTarget               = DRV_SDR_FMC_CMD_TARGET ;
    stSDRAMCmdInfo.FMC_ModeRegisterDefinition      = DRV_SDR_MR_SEG_BURST_LEN | 
                                                     DRV_SDR_MR_SEG_BURST_TYPE |
                                                     DRV_SDR_MR_SEG_CAS |
                                                     DRV_SDR_MR_SEG_OP_MODE |
                                                     DRV_SDR_MR_SEG_WRITE_BURST_MODE ;
    stSDRAMCmdInfo.FMC_AutoRefreshNumber           = 1 ;

    /* 等待SDRAM空闲 */
    while (FMC_GetFlagStatus(DRV_SDR_FMC_BANK_POSITION, FMC_FLAG_Busy) != RESET);

    FMC_SDRAMCmdConfig(&stSDRAMCmdInfo);

    /* 设置SDRAM刷新周期 */
    FMC_SetRefreshCount(DRV_SDR_PARAM_REFRESH_INTERVAL) ;

    /* 等待SDRAM空闲 */
    while (FMC_GetFlagStatus(DRV_SDR_FMC_BANK_POSITION, FMC_FLAG_Busy) != RESET);

    return ;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file drv_sdram_stm32f4xx.c. *******/  
