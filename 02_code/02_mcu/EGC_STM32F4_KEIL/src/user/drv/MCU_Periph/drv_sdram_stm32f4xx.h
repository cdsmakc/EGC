/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : drv_sdram_stm32f4xx.h
    Project name : 
    Module name  : 
    Date created : 2021年9月7日   15时30分2秒
    Author       : Ning.JianLi
    Description  : 基于STM32F4XX系列MCU的SDRAM驱动。
*******************************************************************************/

#ifndef __DRV_SDRAM_STM32F4XX_H__
#define __DRV_SDRAM_STM32F4XX_H__

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "stm32f4xx.h"
#include "stm32f4xx_fmc.h"
#include "pub_type.h"
#include "pub_lib.h"

#pragma anon_unions

CODE_SECTION("===============================") ;
CODE_SECTION("==  编译配置                 ==") ;
CODE_SECTION("===============================") ;
/* 以下宏确定是否使用DQM信号。 */
#define DRV_SDR_USE_DQM_SIGNAL

/* 确定SDRAM器件型号 */
/* 支持如下系列器件：
 * SAMSUNG : K4S281632F-TC75
 * Micron  : MT48LC16M16A2-6A
 */
#define DRV_SDR_DEV_K4S281632F_TC75
//#define DRV_SDR_DEV_MT48LC16M16A2_6A

CODE_SECTION("===============================") ;
CODE_SECTION("==  SDRAM信号所在GPIO组定义  ==") ;
CODE_SECTION("===============================") ;
#define DRV_SDR_A0_PIN_GROUP_ID                GPIOF                        /* A0引脚序号定义 */
#define DRV_SDR_A1_PIN_GROUP_ID                GPIOF                        /* A1引脚序号定义 */
#define DRV_SDR_A2_PIN_GROUP_ID                GPIOF                        /* A2引脚序号定义 */
#define DRV_SDR_A3_PIN_GROUP_ID                GPIOF                        /* A3引脚序号定义 */
#define DRV_SDR_A4_PIN_GROUP_ID                GPIOF                        /* A4引脚序号定义 */
#define DRV_SDR_A5_PIN_GROUP_ID                GPIOF                        /* A5引脚序号定义 */
#define DRV_SDR_A6_PIN_GROUP_ID                GPIOF                        /* A6引脚序号定义 */
#define DRV_SDR_A7_PIN_GROUP_ID                GPIOF                        /* A7引脚序号定义 */
#define DRV_SDR_A8_PIN_GROUP_ID                GPIOF                        /* A8引脚序号定义 */
#define DRV_SDR_A9_PIN_GROUP_ID                GPIOF                        /* A9引脚序号定义 */
#define DRV_SDR_A10_PIN_GROUP_ID               GPIOG                        /* A10引脚序号定义 */
#define DRV_SDR_A11_PIN_GROUP_ID               GPIOG                        /* A11引脚序号定义 */
#ifdef DRV_SDR_DEV_MT48LC16M16A2_6A
#define DRV_SDR_A12_PIN_GROUP_ID               GPIOG                        /* A12引脚序号定义 */
#endif

#define DRV_SDR_BA0_PIN_GROUP_ID               GPIOG                        /* BA0引脚序号定义 */
#define DRV_SDR_BA1_PIN_GROUP_ID               GPIOG                        /* BA1引脚序号定义 */

#define DRV_SDR_D0_PIN_GROUP_ID                GPIOD                        /* D0引脚序号定义 */
#define DRV_SDR_D1_PIN_GROUP_ID                GPIOD                        /* D1引脚序号定义 */
#define DRV_SDR_D2_PIN_GROUP_ID                GPIOD                        /* D2引脚序号定义 */
#define DRV_SDR_D3_PIN_GROUP_ID                GPIOD                        /* D3引脚序号定义 */
#define DRV_SDR_D4_PIN_GROUP_ID                GPIOE                        /* D4引脚序号定义 */
#define DRV_SDR_D5_PIN_GROUP_ID                GPIOE                        /* D5引脚序号定义 */
#define DRV_SDR_D6_PIN_GROUP_ID                GPIOE                        /* D6引脚序号定义 */
#define DRV_SDR_D7_PIN_GROUP_ID                GPIOE                        /* D7引脚序号定义 */
#define DRV_SDR_D8_PIN_GROUP_ID                GPIOE                        /* D8引脚序号定义 */
#define DRV_SDR_D9_PIN_GROUP_ID                GPIOE                        /* D9引脚序号定义 */
#define DRV_SDR_D10_PIN_GROUP_ID               GPIOE                        /* D10引脚序号定义 */
#define DRV_SDR_D11_PIN_GROUP_ID               GPIOE                        /* D11引脚序号定义 */
#define DRV_SDR_D12_PIN_GROUP_ID               GPIOE                        /* D12引脚序号定义 */
#define DRV_SDR_D13_PIN_GROUP_ID               GPIOD                        /* D13引脚序号定义 */
#define DRV_SDR_D14_PIN_GROUP_ID               GPIOD                        /* D14引脚序号定义 */
#define DRV_SDR_D15_PIN_GROUP_ID               GPIOD                        /* D15引脚序号定义 */

#define DRV_SDR_CK_PIN_GROUP_ID                GPIOG                        /* CK引脚序号定义 */
#define DRV_SDR_CKE_PIN_GROUP_ID               GPIOH                        /* CKE引脚序号定义 */
#define DRV_SDR_CS_PIN_GROUP_ID                GPIOH                        /* CS引脚序号定义 */
#define DRV_SDR_RAS_PIN_GROUP_ID               GPIOF                        /* RAS引脚序号定义 */
#define DRV_SDR_CAS_PIN_GROUP_ID               GPIOG                        /* CAS引脚序号定义 */
#define DRV_SDR_WE_PIN_GROUP_ID                GPIOC                        /* WE引脚序号定义 */

#ifdef DRV_SDR_USE_DQM_SIGNAL
#define DRV_SDR_DQM0_PIN_GROUP_ID              GPIOE                        /* DQM0引脚序号定义 */
#define DRV_SDR_DQM1_PIN_GROUP_ID              GPIOE                        /* DQM1引脚序号定义 */
#endif

CODE_SECTION("===============================") ;
CODE_SECTION("==  SDRAM信号定义            ==") ;
CODE_SECTION("===============================") ;
#define DRV_SDR_A0_GPIO_PIN_ID                 GPIO_Pin_0                   /* A0引脚定义 */
#define DRV_SDR_A1_GPIO_PIN_ID                 GPIO_Pin_1                   /* A1引脚定义 */
#define DRV_SDR_A2_GPIO_PIN_ID                 GPIO_Pin_2                   /* A2引脚定义 */
#define DRV_SDR_A3_GPIO_PIN_ID                 GPIO_Pin_3                   /* A3引脚定义 */
#define DRV_SDR_A4_GPIO_PIN_ID                 GPIO_Pin_4                   /* A4引脚定义 */
#define DRV_SDR_A5_GPIO_PIN_ID                 GPIO_Pin_5                   /* A5引脚定义 */
#define DRV_SDR_A6_GPIO_PIN_ID                 GPIO_Pin_12                  /* A6引脚定义 */
#define DRV_SDR_A7_GPIO_PIN_ID                 GPIO_Pin_13                  /* A7引脚定义 */
#define DRV_SDR_A8_GPIO_PIN_ID                 GPIO_Pin_14                  /* A8引脚定义 */
#define DRV_SDR_A9_GPIO_PIN_ID                 GPIO_Pin_15                  /* A9引脚定义 */
#define DRV_SDR_A10_GPIO_PIN_ID                GPIO_Pin_0                   /* A10引脚定义 */
#define DRV_SDR_A11_GPIO_PIN_ID                GPIO_Pin_1                   /* A11引脚定义 */
#ifdef DRV_SDR_DEV_MT48LC16M16A2_6A
#define DRV_SDR_A12_GPIO_PIN_ID                GPIO_Pin_2                   /* A12引脚定义 */
#endif

#define DRV_SDR_BA0_GPIO_PIN_ID                GPIO_Pin_4                   /* BA0引脚定义 */
#define DRV_SDR_BA1_GPIO_PIN_ID                GPIO_Pin_5                   /* BA1引脚定义 */

#define DRV_SDR_D0_GPIO_PIN_ID                 GPIO_Pin_14                  /* D0引脚定义 */
#define DRV_SDR_D1_GPIO_PIN_ID                 GPIO_Pin_15                  /* D1引脚定义 */
#define DRV_SDR_D2_GPIO_PIN_ID                 GPIO_Pin_0                   /* D2引脚定义 */
#define DRV_SDR_D3_GPIO_PIN_ID                 GPIO_Pin_1                   /* D3引脚定义 */
#define DRV_SDR_D4_GPIO_PIN_ID                 GPIO_Pin_7                   /* D4引脚定义 */
#define DRV_SDR_D5_GPIO_PIN_ID                 GPIO_Pin_8                   /* D5引脚定义 */
#define DRV_SDR_D6_GPIO_PIN_ID                 GPIO_Pin_9                   /* D6引脚定义 */
#define DRV_SDR_D7_GPIO_PIN_ID                 GPIO_Pin_10                  /* D7引脚定义 */
#define DRV_SDR_D8_GPIO_PIN_ID                 GPIO_Pin_11                  /* D8引脚定义 */
#define DRV_SDR_D9_GPIO_PIN_ID                 GPIO_Pin_12                  /* D9引脚定义 */
#define DRV_SDR_D10_GPIO_PIN_ID                GPIO_Pin_13                  /* D10引脚定义 */
#define DRV_SDR_D11_GPIO_PIN_ID                GPIO_Pin_14                  /* D11引脚定义 */
#define DRV_SDR_D12_GPIO_PIN_ID                GPIO_Pin_15                  /* D12引脚定义 */
#define DRV_SDR_D13_GPIO_PIN_ID                GPIO_Pin_8                   /* D13引脚定义 */
#define DRV_SDR_D14_GPIO_PIN_ID                GPIO_Pin_9                   /* D14引脚定义 */
#define DRV_SDR_D15_GPIO_PIN_ID                GPIO_Pin_10                  /* D15引脚定义 */

#define DRV_SDR_CK_GPIO_PIN_ID                 GPIO_Pin_8                   /* CK引脚定义 */
#define DRV_SDR_CKE_GPIO_PIN_ID                GPIO_Pin_2                   /* CKE引脚定义 */
#define DRV_SDR_CS_GPIO_PIN_ID                 GPIO_Pin_3                   /* CS引脚定义 */
#define DRV_SDR_RAS_GPIO_PIN_ID                GPIO_Pin_11                  /* RAS引脚定义 */
#define DRV_SDR_CAS_GPIO_PIN_ID                GPIO_Pin_15                  /* CAS引脚定义 */
#define DRV_SDR_WE_GPIO_PIN_ID                 GPIO_Pin_0                   /* WE引脚定义 */

#ifdef DRV_SDR_USE_DQM_SIGNAL
#define DRV_SDR_DQM0_GPIO_PIN_ID               GPIO_Pin_0                   /* DQM0引脚定义 */
#define DRV_SDR_DQM1_GPIO_PIN_ID               GPIO_Pin_1                   /* DQM1引脚定义 */
#endif

CODE_SECTION("===============================") ;
CODE_SECTION("==  SDRAM参数定义            ==") ;
CODE_SECTION("===============================") ;

/* 模式寄存器定义 */
#define DRV_SDR_MR_SEG_BURST_LEN               (0x1)                        /* 突发长度设置 */
#define DRV_SDR_MR_SEG_BURST_TYPE              (0x0)                        /* 突发类型设置 */
#define DRV_SDR_MR_SEG_CAS                     (0x30)                       /* CAS值设置 */
#define DRV_SDR_MR_SEG_OP_MODE                 (0x0)                        /* 操作模式设置 */
#define DRV_SDR_MR_SEG_WRITE_BURST_MODE        (0x200)                      /* 写突发模式设置 */

/* SDRAM型号为K4S281632F-TC75 */
#ifdef DRV_SDR_DEV_K4S281632F_TC75
#define DRV_SDR_PARAM_T_MRD                    (2)                          /* tMRD = 2  CK (-7) */
#define DRV_SDR_PARAM_T_XSR                    (7)                          /* tXSR = 70 ns (-7) */
#define DRV_SDR_PARAM_T_RAS                    (4)                          /* tRAS = 42 ns (-7) */
#define DRV_SDR_PARAM_T_RC                     (7)                          /* tRC  = 63 ns (-7) */
#define DRV_SDR_PARAM_T_WR                     (2)                          /* tWR  = 2  CK (-7) */
#define DRV_SDR_PARAM_T_RP                     (2)                          /* tRP  = 15 ns (-7) */
#define DRV_SDR_PARAM_T_RCD                    (2)                          /* tRCD = 15 ns (-7) */
#define DRV_SDR_PARAM_COL_BIT_NUM              FMC_ColumnBits_Number_9b     /* 列宽度      9  位 */
#define DRV_SDR_PARAM_ROW_BIT_NUM              FMC_RowBits_Number_12b       /* 行宽度      12 位 */
#define DRV_SDR_PARAM_DATA_BIT_NUM             FMC_SDMemory_Width_16b       /* 数据位宽     16 位 */
#define DRV_SDR_PARAM_BANK_NUM                 FMC_InternalBank_Number_4    /* Bank数量   4 */
#define DRV_SDR_PARAM_CAS                      FMC_CAS_Latency_3            /* CAS值     3  CK */
#define DRV_SDR_PARAM_REFRESH_INTERVAL         (1313)                       /* 分布式刷新间隔，单位为CK */

#define DRV_SDR_FMC_BANK_POSITION              FMC_Bank1_SDRAM              /* SDRAM位于FMC中的位置 */
#define DRV_SDR_FMC_WRITE_PROTECTION           FMC_Write_Protection_Disable /* 是否使能写保护 */
#define DRV_SDR_FMC_SDCLK_PERIOD               FMC_SDClock_Period_2         /* SDCLK时钟为AHB时钟2分频 */
#define DRV_SDR_FMC_READ_BURST                 FMC_Read_Burst_Disable       /* 禁止突发访问 */
#define DRV_SDR_FMC_READ_PIPE_DELAY            FMC_ReadPipe_Delay_1
#endif
/* SDRAM型号为MT48LC16M16A2-7 */
#ifdef DRV_SDR_DEV_MT48LC16M16A2_6A
#define DRV_SDR_PARAM_T_MRD                    (2)                          /* tMRD = 2  CK (-7) */
#define DRV_SDR_PARAM_T_XSR                    (7)                          /* tXSR = 70 ns (-7) */
#define DRV_SDR_PARAM_T_RAS                    (4)                          /* tRAS = 42 ns (-7) */
#define DRV_SDR_PARAM_T_RC                     (7)                          /* tRC  = 63 ns (-7) */
#define DRV_SDR_PARAM_T_WR                     (2)                          /* tWR  = 2  CK (-7) */
#define DRV_SDR_PARAM_T_RP                     (2)                          /* tRP  = 15 ns (-7) */
#define DRV_SDR_PARAM_T_RCD                    (2)                          /* tRCD = 15 ns (-7) */
#define DRV_SDR_PARAM_COL_BIT_NUM              FMC_ColumnBits_Number_9b     /* 列宽度      9  位 */
#define DRV_SDR_PARAM_ROW_BIT_NUM              FMC_RowBits_Number_13b       /* 行宽度      13 位 */
#define DRV_SDR_PARAM_DATA_BIT_NUM             FMC_SDMemory_Width_16b       /* 数据位宽     16 位 */
#define DRV_SDR_PARAM_BANK_NUM                 FMC_InternalBank_Number_4    /* Bank数量   4 */
#define DRV_SDR_PARAM_CAS                      FMC_CAS_Latency_3            /* CAS值     3  CK */
#define DRV_SDR_PARAM_REFRESH_INTERVAL         (657)                        /* 分布式刷新间隔，单位为CK */

#define DRV_SDR_FMC_BANK_POSITION              FMC_Bank1_SDRAM              /* SDRAM位于FMC中的位置 */
#define DRV_SDR_FMC_WRITE_PROTECTION           FMC_Write_Protection_Disable /* 是否使能写保护 */
#define DRV_SDR_FMC_SDCLK_PERIOD               FMC_SDClock_Period_2         /* SDCLK时钟为AHB时钟2分频 */
#define DRV_SDR_FMC_READ_BURST                 FMC_Read_Burst_Disable       /* 禁止突发访问 */
#define DRV_SDR_FMC_READ_PIPE_DELAY            FMC_ReadPipe_Delay_1
#endif

#define DRV_SDR_FMC_CMD_TARGET                 FMC_Command_Target_bank1     /* 命令的目标Bank */


CODE_SECTION("===============================") ;
CODE_SECTION("==  SDRAM模块函数声明        ==") ;
CODE_SECTION("===============================") ;
VOID DRV_SDR_Init(VOID) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __DRV_SDRAM_STM32F4XX_H__ */

/******* End of file drv_sdram_stm32f4xx.h. *******/  
