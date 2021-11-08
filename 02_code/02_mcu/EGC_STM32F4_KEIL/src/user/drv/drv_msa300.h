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

#ifndef __DRV_MSA300_H__
#define __DRV_MSA300_H__

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#pragma anon_unions
#pragma pack(1)

#include "stm32f4xx.h"

#include "pub_type.h"
#include "pub_lib.h"


CODE_SECTION("==========================") ;
CODE_SECTION("==  模块内部宏定义      ==") ;
CODE_SECTION("==========================") ;

/* MSA300总线地址定义 */
#define DRV_MSA300_BUS_ADDR                       0x27

/* MSA300寄存器地址定义 */
#define DRV_MSA300_REG_ADDR_SOFT_RESET            0x00 /* 复位地址 */
#define DRV_MSA300_REG_ADDR_PART_ID               0x01 /* 芯片ID */
#define DRV_MSA300_REG_ADDR_X_AXIS                0x02 /* X轴数据 */
#define DRV_MSA300_REG_ADDR_X_AXIS_LSB            0x02 /* X轴数据低位 */
#define DRV_MSA300_REG_ADDR_X_AXIS_MSB            0x03 /* X轴数据高位 */
#define DRV_MSA300_REG_ADDR_Y_AXIS                0x04 /* Y轴数据 */
#define DRV_MSA300_REG_ADDR_Y_AXIS_LSB            0x04 /* Y轴数据低位 */
#define DRV_MSA300_REG_ADDR_Y_AXIS_MSB            0x05 /* Y轴数据高位 */
#define DRV_MSA300_REG_ADDR_Z_AXIS                0x06 /* Z轴数据 */
#define DRV_MSA300_REG_ADDR_Z_AXIS_LSB            0x06 /* Z轴数据低位 */
#define DRV_MSA300_REG_ADDR_Z_AXIS_MSB            0x07 /* Z轴数据高位 */
#define DRV_MSA300_REG_ADDR_MOTION_INTERRUPT      0x09 /* 中断状态位 */
#define DRV_MSA300_REG_ADDR_DATA_INTERRUPT        0x0A /*  */
#define DRV_MSA300_REG_ADDR_TAPE_ACTIVE_STATUS    0x0B /*  */
#define DRV_MSA300_REG_ADDR_ORIENTATION_STATUS    0x0C /*  */
#define DRV_MSA300_REG_ADDR_RESOLUTION_RANGE      0x0F /*  */
#define DRV_MSA300_REG_ADDR_IDR                   0x10 /*  */
#define DRV_MSA300_REG_ADDR_POWER_MODE_BANDWIDTH  0x11 /*  */
#define DRV_MSA300_REG_ADDR_SWAP_POLARITY         0x12 /*  */
#define DRV_MSA300_REG_ADDR_INT_SET_0             0x16 /*  */
#define DRV_MSA300_REG_ADDR_INT_SET_1             0x17 /*  */
#define DRV_MSA300_REG_ADDR_INT_MAP_0             0x19 /*  */
#define DRV_MSA300_REG_ADDR_INT_MAP_1             0x1A /*  */
#define DRV_MSA300_REG_ADDR_INT_MAP_2             0x1B /*  */
#define DRV_MSA300_REG_ADDR_INT_MAP_3             0x20 /*  */
#define DRV_MSA300_REG_ADDR_INT_LATCH             0x21 /*  */
#define DRV_MSA300_REG_ADDR_FREEFALL_DUR          0x22 /*  */
#define DRV_MSA300_REG_ADDR_FREEFALL_TH           0x23 /*  */
#define DRV_MSA300_REG_ADDR_FREEFALL_HY           0x24 /*  */
#define DRV_MSA300_REG_ADDR_ACTIVE_DUR            0x27 /*  */
#define DRV_MSA300_REG_ADDR_ACTIVE_TH             0x28 /*  */
#define DRV_MSA300_REG_ADDR_TAP_DUR               0x2A /*  */
#define DRV_MSA300_REG_ADDR_TAP_TH                0x2B /*  */
#define DRV_MSA300_REG_ADDR_ORIENT_HY             0x2C /*  */
#define DRV_MSA300_REG_ADDR_Z_BLOCK               0x2D /*  */
#define DRV_MSA300_REG_ADDR_OFFSET_COMPENSATION   0x38 /* 修正值寄存器 */
#define DRV_MSA300_REG_ADDR_OFFSET_COMP_OFF_X     0x38 /* X轴修正值寄存器 */
#define DRV_MSA300_REG_ADDR_OFFSET_COMP_OFF_Y     0x39 /* Y轴修正值寄存器 */
#define DRV_MSA300_REG_ADDR_OFFSET_COMP_OFF_Z     0x3A /* Z轴修正值寄存器 */

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块枚举数据类型    ==") ;
CODE_SECTION("==========================") ;

/* 寄存器地址0x0F，名称Resolution/Range 
 * 本寄存器定义了量化位宽和加速度值测量范围
 */
typedef enum tagDRV_MSA300_RESOLUTION_DEF
{
    RESOLUTION_DEF_14BIT     = 0, /* 14 bit 量化位宽 */
    RESOLUTION_DEF_12BIT     = 1, /* 12 bit 量化位宽 */
    RESOLUTION_DEF_10BIT     = 2, /* 10 bit 量化位宽 */
    RESOLUTION_DEF_8BIT      = 3  /* 8  bit 量化位宽 */
}DRV_MSA300_RESOLUTION_DEF_E ;

typedef enum tagDRV_MSA300_RANGE_DEF
{
    RANGE_DEF_4G             = 0, /* +/- 2  g 测量范围 */
    RANGE_DEF_8G             = 1, /* +/- 4  g 测量范围 */
    RANGE_DEF_16G            = 2, /* +/- 8  g 测量范围 */
    RANGE_DEF_32G            = 3  /* +/- 16 g 测量范围 */
}DRV_MSA300_RANGE_DEF_E ;

/* 以下枚举类型定义了驱动函数的错误码 */
typedef enum tagDRV_MSA_ERROR_CODE
{
    /* 操作正常 */
    MSA_ERROR_OK                               =    0u, /* 操作正常 */

    /* Write函数错误码 */
    MSA_ERROR_WRITE_ERROR                      = 2000u, /* 写入失败 */

    /* Read函数错误码 */
    MSA_ERROR_READ_ERROR                       = 3000u, /* 读取失败 */

    /* GPIOConfig函数错误码 */
    MSA_ERROR_GPIOCONFIG_INVALID_GPIO_GROUP    = 4000u, /* 指定了错误的GPIO Group编号 */
    MSA_ERROR_GPIOCONFIG_INVALID_GPIO_INDEX    = 4001u, /* 指定了错误的GPIO Index编号 */

    /* ETXIConfig函数错误码 */
    MSA_ERROR_EXTICONFIG_INVALID_EXTI_LINE     = 5000u, /* 指定了错误的EXTI Line编号 */
    MSA_ERROR_EXTICONFIG_INVALID_GPIO_GROUP    = 5001u, /* 指定了错误的GPIO Group编号 */

    /* GetInterruptFlag函数错误码 */
    MSA_ERROR_GETINTERRUPTFLAG_I2C_READ_FAIL   = 6000u, /* I2C读取失败 */

    /* ReadAcceleration函数错误码 */
    MSA_ERROR_READACCELERATION_READ_X_FAIL     = 7000u, /* 读取X轴加速度失败 */
    MSA_ERROR_READACCELERATION_READ_Y_FAIL     = 7001u, /* 读取Y轴加速度失败 */
    MSA_ERROR_READACCELERATION_READ_Z_FAIL     = 7002u, /* 读取Z轴加速度失败 */

    /* SensitivitySet函数错误码 */   
    MSA_ERROR_SENSITIVITYSET_I2C_WRITE_FAIL    = 8000u, /* I2C写入失败 */

    /* Init函数错误码 */
    MSA_ERROR_INIT_READ_POWER_BW_FAIL          = 9000u, /* 读取寄存器失败 */
    MSA_ERROR_INIT_WRITE_POWER_BW_FAIL         = 9001u, /* 写入寄存器失败 */
    MSA_ERROR_INIT_WRITE_RESOLUTION_FAIL       = 9002u, /* 写入寄存器失败 */
    MSA_ERROR_INIT_SET_SENSITIVITY_FAIL        = 9003u, /* 配置灵敏度失败 */
    MSA_ERROR_INIT_WRITE_INT_SET_FAIL          = 9004u, /* 写入寄存器失败 */
    MSA_ERROR_INIT_WRITE_INT_MAP0_FAIL         = 9005u, /* 写入寄存器失败 */
    MSA_ERROR_INIT_WRITE_INT_MAP3_FAIL         = 9006u, /* 写入寄存器失败 */
    MSA_ERROR_INIT_WRITE_INT_LATCH_FAIL        = 9007u, /* 写入寄存器失败 */
    MSA_ERROR_INIT_CONFIG_GPIO_FAIL            = 9008u, /* 配置GPIO失败 */
    MSA_ERROR_INIT_CONFIG_EXTI_FAIL            = 9009u, /* 配置EXTI失败 */

    /* IRQHandler函数错误码 */
    MSA_ERROR_INIT_INVALID_EXTI_LINE           = 10000u /* 指定了错误的EXTI Line编号 */
} DRV_MSA_ERROR_CODE_E ;


CODE_SECTION("==========================") ;
CODE_SECTION("==  模块结构数据类型    ==") ;
CODE_SECTION("==========================") ;

typedef struct tagDRV_MSA300_REG_RESOLUTION_RANGE
{
    union
    {
        UCHAR ucRawData ;

        struct
        {
            DRV_MSA300_RANGE_DEF_E      enRange      : 2 ;
            DRV_MSA300_RESOLUTION_DEF_E enResolution : 2 ;
            UCHAR                       ucRSV0       : 4 ;
        } ;
    } ;
}DRV_MSA300_REG_RESOLUTION_RANGE_S ;

/* 寄存器地址0x11，名称Power Mode / Bandwidth 
 * 本寄存器定义了电源模式和带宽范围
 */
typedef enum tagDRV_MSA300_POWER_MODE_DEF
{
    POWER_MODE_DEF_NORMAL    = 0, /* normal mode */
    POWER_MODE_DEF_LOW_POWER = 1, /* low power mode */
    POWER_MODE_DEF_SUSPEND   = 2  /* suspend mode */
}DRV_MSA300_POWER_MODE_DEF_E ;

typedef enum tagDRV_MSA300_LOW_POWER_BW_DEF
{
    LOW_POWER_BW_DEF_1R95    = 0, /* 1.95  Hz */
    LOW_POWER_BW_DEF_3R9     = 3, /* 3.9   Hz */
    LOW_POWER_BW_DEF_7R81    = 4, /* 7.81  Hz */
    LOW_POWER_BW_DEF_15R63   = 5, /* 15.63 Hz */
    LOW_POWER_BW_DEF_31R25   = 6, /* 31.25 Hz */
    LOW_POWER_BW_DEF_62R5    = 7, /* 62.5  Hz */
    LOW_POWER_BW_DEF_125     = 8, /* 125   Hz */
    LOW_POWER_BW_DEF_250     = 9, /* 250   Hz */
    LOW_POWER_BW_DEF_500     = 10 /* 500   Hz */
}DRV_MSA300_LOW_POWER_BW_DEF_E ;

typedef struct tagDRV_MSA300_REG_POWER_BW
{
    union
    {
        UCHAR ucRawData ;

        struct
        {
            UCHAR                         ucRSV0            : 1 ;
            DRV_MSA300_LOW_POWER_BW_DEF_E enLowPowerBW      : 4 ;
            UCHAR                         ucRSV1            : 1 ;
            DRV_MSA300_POWER_MODE_DEF_E   enPWRMode         : 2 ;
        } ;
    } ;
}DRV_MSA300_REG_POWER_BW_S ;

/* 寄存器地址0x16，名称Int_Set_0 
 * 本寄存器定义了中断使能位
 */
typedef struct tagDRV_MSA300_REG_INT_SET0
{
    union
    {
        UCHAR ucRawData ;

        struct
        {
            UCHAR ucActiveIntEnX    : 1 ;
            UCHAR ucActiveIntEnY    : 1 ;
            UCHAR ucActiveIntEnZ    : 1 ;
            UCHAR ucRSV0            : 1 ;
            UCHAR ucDoubleTapIntEn  : 1 ;
            UCHAR ucSingleTapIntEn  : 1 ;
            UCHAR ucOrientIntEn     : 1 ;
            UCHAR ucRSV1            : 1 ;
        } ;
    } ;
}DRV_MSA300_REG_INT_SET0_S ;

/* 寄存器地址0x19，名称Int_Map_0 
 * 本寄存器定义了INT0引脚的中断映射
 */
typedef struct tagDRV_MSA300_REG_INT_MAP0
{
    union
    {
        UCHAR ucRawData ;

        struct
        {
            UCHAR ucInt1Freefall    : 1 ;
            UCHAR ucRSV0            : 1 ;
            UCHAR ucInt1Active      : 1 ;
            UCHAR ucRSV1            : 1 ;
            UCHAR ucInt1DoubleTap   : 1 ;
            UCHAR ucInt1SingleTap   : 1 ;
            UCHAR ucInt1Orient      : 1 ;
            UCHAR ucRSV2            : 1 ;
        } ;
    } ;
}DRV_MSA300_REG_INT_MAP0_S ;

/* 寄存器地址0x19，名称Int_Map_3
 * 本寄存器定义了INT0和INT1引脚的电气属性
 */
typedef struct tagDRV_MSA300_REG_INT_MAP3
{
    union
    {
        UCHAR ucRawData ;

        struct
        {
            UCHAR ucInt1Level       : 1 ;
            UCHAR ucInt1OpenDrain   : 1 ;
            UCHAR ucInt2Level       : 1 ;
            UCHAR ucInt2OpenDrain   : 1 ;
            UCHAR ucRSV0            : 1 ;
        } ;
    } ;
}DRV_MSA300_REG_INT_MAP3_S ;

/* 寄存器地址0x21，名称Int_Latch
 * 本寄存器定义了中断锁定时间，和中断复位
 */
typedef enum tagDRV_MSA300_LATCH_INT_DEF
{
    LATCH_INT_DEF_NON_LATCHED = 0,  /* non-latched */
    LATCH_INT_DEF_250MS       = 1, /* 250 ms */
    LATCH_INT_DEF_500MS       = 2,   /* 500 ms */
    LATCH_INT_DEF_1S          = 3,  /* 1   s */
    LATCH_INT_DEF_2S          = 4,  /* 2   s */
    LATCH_INT_DEF_4S          = 5,  /* 4   s */
    LATCH_INT_DEF_8S          = 6,  /* 8   s */
    LATCH_INT_DEF_LATCHED     = 7,  /* latched */
    LATCH_INT_DEF_1MS         = 9,  /* 1   ms */
    LATCH_INT_DEF_2MS         = 11, /* 2   ms */
    LATCH_INT_DEF_25MS        = 12, /* 25  ms */
    LATCH_INT_DEF_50MS        = 13, /* 50  ms */
    LATCH_INT_DEF_100MS       = 14  /* 100 ms */
}DRV_MSA300_LATCH_INT_DEF_E ;

typedef struct tagDRV_MSA300_REG_INT_LATCH
{
    union
    {
        UCHAR ucRawData ;

        struct
        {
            DRV_MSA300_LATCH_INT_DEF_E enLatchInt : 4 ;
            UCHAR                      ucRSV0     : 3 ;
            UCHAR                      ucResetInt : 1 ;
        } ;
    } ;
}DRV_MSA300_REG_INT_LATCH_S ;

typedef enum tagDRV_MSA300_INT_BIT
{
    INT_BIT_FREEFALL    = 0x01 ,
    INT_BIT_ACTIVE      = 0x04 ,
    INT_BIT_DOUBLE_TAP  = 0x10 ,
    INT_BIT_SINGLE_TAP  = 0x20 ,
    INT_BIT_ORIENT      = 0x40 ,
    INT_BIT_NEW_DATA    = 0x80 ,
} DRV_MSA300_INT_BIT_E ;

typedef struct tagDRV_MSA300_WORKAREA
{
    union
    {
        UCHAR aucRawData[4] ;

        struct
        {
            /* DW0 */
            UINT uiActiveFlag        : 1 ; /* Active 中断标记 */
            UINT uiRSV0              : 31 ;
        } ;
    } ;
}DRV_MSA300_WORKAREA_S ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块函数声明        ==") ;
CODE_SECTION("==========================") ;

DRV_MSA_ERROR_CODE_E DRV_MSA_ReadAcceleration(FLOAT *pfXAxis, FLOAT *pfYAxis, FLOAT *pfZAxis) ;
DRV_MSA_ERROR_CODE_E DRV_MSA_SensitivitySet(UCHAR ucSense) ;
DRV_MSA_ERROR_CODE_E DRV_MSA_Init(GPIO_TypeDef *pstExtIntGPIOGroup, UINT uiExtIntGPIOIndex, UINT uiEXTILine) ;
DRV_MSA_ERROR_CODE_E DRV_MSA_IRQHandler(UINT uiEXTILine) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __DRV_MSA300_H__ */

/******* End of file drv_msa300.h. *******/  
