/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : drv_i2c_stm32f4xx.h
    Project name : 
    Module name  : 
    Date created : 2021年9月14日   10时37分29秒
    Author       : Ning.JianLi
    Description  : 基于STM32F4xx微控制器的I2C外设驱动程序头文件。
*******************************************************************************/

/* ##############################################################################################
 * ## 版本说明                                                                                     ##
 * ##############################################################################################
 * REV1.0    Ning.JianLi   初始版本。
 */

/* ##############################################################################################
 * 如何使用本模块？
 * ##############################################################################################
 * 1. 如果需要使用UCOS-III操作系统，定义宏DRV_SPI_USE_UCOS_III。该定义使得操作函数内部的等待变为
 *    非阻塞式的。
 * 2. 使用函数DRV_I2C_Open创建I2C驱动设备。其配置参数可以是动态的，使用完毕后即可释放。I2C设备参
 *    数需要是静态的，在使用函数DRV_I2C_Close关闭设备之前，不能被释放，并且不能被外部访问。
 * 3. 使用函数DRV_I2C_Close关闭设备。
 * 4. 使用函数DRV_I2C_Write进行I2C写入操作。
 * 5. 使用函数DRV_I2C_Read进行SPI读取操作。参数ucAddGapMs用于指定在读操作的第一相和第二相之间插入
 *    的延时（以毫秒为单位）。某些器件可能需要在两相之间有一个延时。该功能仅在使能uCOS-III时有效。
 * 6. 用户需要定义一个中断处理函数。该函数内直接调用DRV_I2C_GeneralIntHandler以处理中断。用户需要
 *    将该函数链接到中断向量表。
 */

#ifndef __DRV_I2C_STM32F4XX_H__
#define __DRV_I2C_STM32F4XX_H__

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#pragma anon_unions

#include "stm32f4xx.h"
#include "pub_type.h"
#include "pub_lib.h"

CODE_SECTION("==========================") ;
CODE_SECTION("==  移植相关定义        ==") ;
CODE_SECTION("==========================") ;

/* 以下宏定义确定是否在UC/OS-III系统中使用本模块.如果在该系统中使用本模块,应该定义该宏 */
#define DRV_I2C_USE_UCOS_III

#ifdef DRV_I2C_USE_UCOS_III
#include "os.h"
#endif

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块内部宏定义      ==") ;
CODE_SECTION("==========================") ;

/* I2C设备已经打开特征值 */
#define DRV_I2C_OPEN_FLAG                        (0x5B)

#ifndef DRV_I2C_USE_UCOS_III
/* I2C总线占用标记 */
#define DRV_I2C_BUS_UNOCCUPY                     (0x00) /* 总线被闲置 */
#define DRV_I2C_BUS_OCCUPY                       (0xFF) /* 总线被占用 */

/* I2C写操作完成标记 */
#define DRV_I2C_WR_DONE                          (0x00) /* 写操作完成 */
#define DRV_I2C_WR_PEND                          (0xFF) /* 写操作挂起 */

/* I2C读操作完成标记 */
#define DRV_I2C_RD_DONE                          (0x00) /* 读操作完成 */
#define DRV_I2C_RD_PEND                          (0xFF) /* 读操作挂起 */
#endif

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块枚举数据类型    ==") ;
CODE_SECTION("==========================") ;

/* 以下枚举类型定义了I2C器件模块操作函数的错误码 */
typedef enum tagDRV_I2C_ERROR_CODE
{
    /* 操作正常 */
    I2C_ERROR_OK                                   =    0u, /* 操作正常 */

    /* I2C设备打开函数错误码 */
    I2C_ERROR_OPEN_I2C_DEVICE_PTR_NULL             = 2000u, /* I2C设备指针为空 */
    I2C_ERROR_OPEN_CONFIG_INFO_PTR_NULL            = 2001u, /* 配置信息指针为空 */
    I2C_ERROR_OPEN_SCL_PIN_GROUP_ERROR             = 2002u, /* I2C-SCL引脚所在GPIO Group定义错误 */
    I2C_ERROR_OPEN_SDA_PIN_GROUP_ERROR             = 2003u, /* I2C-SDA引脚所在GPIO Group定义错误 */
    I2C_ERROR_OPEN_SCL_PIN_ERROR                   = 2004u, /* I2C-SCL引脚ID定义错误 */
    I2C_ERROR_OPEN_SDA_PIN_ERROR                   = 2005u, /* I2C-SDA引脚ID定义错误 */
    I2C_ERROR_OPEN_SCL_AF_ERROR                    = 2006u, /* I2C-SCL引脚复用功能定义错误 */
    I2C_ERROR_OPEN_SDA_AF_ERROR                    = 2007u, /* I2C-SDA引脚复用功能定义错误 */
    I2C_ERROR_OPEN_INVALID_I2C_PERIPH              = 2008u, /* I2C外设定义错误 */
    I2C_ERROR_OPEN_INVALID_I2C_CLOCK_SPEED         = 2009u, /* I2C时钟速率定义错误 */
    I2C_ERROR_OPEN_INVALID_I2C_DUTY_CYCLE          = 2010u, /* I2C Duty-Cycle定义错误 */
#ifdef DRV_I2C_USE_UCOS_III
    I2C_ERROR_OPEN_CREATE_SEM_I2C_ACC              = 2011u, /* 创建I2C总线资源信号量出错 */
    I2C_ERROR_OPEN_CREATE_SEM_I2C_WR               = 2012u, /* 创建I2C写操作信号量出错 */
    I2C_ERROR_OPEN_CREATE_SEM_I2C_RD               = 2013u, /* 创建I2C读操作信号量出错 */
#endif
    I2C_ERROR_OPEN_DEVICE_ALREADY_CREATED          = 2014u, /* I2C驱动设备已经被创建 */

    /* I2C设备读操作第一相函数错误码 */
    I2C_ERROR_READPHASE1_DEVICE_PTR_NULL           = 3000u, /* I2C设备指针为空 */
    I2C_ERROR_READPHASE1_DEVICE_NOT_OPENED         = 3001u, /* I2C设备未创建 */
    I2C_ERROR_READPHASE1_DEVICE_NOT_IDLE           = 3002u, /* I2C设备非空闲状态 */

    /* I2C设备读操作第二相函数错误码 */
    I2C_ERROR_READPHASE2_DEVICE_PTR_NULL           = 4000u, /* I2C设备指针为空 */
    I2C_ERROR_READPHASE2_BUFFER_PTR_NULL           = 4001u, /* I2C缓存指针为空 */
    I2C_ERROR_READPHASE2_READ_SIZE_ZERO            = 4002u, /* I2C读取数据量为0 */
    I2C_ERROR_READPHASE2_DEVICE_NOT_OPENED         = 4003u, /* I2C设备未创建 */
    I2C_ERROR_READPHASE2_DEVICE_NOT_IDLE           = 4004u, /* I2C设备非空闲状态 */

    /* I2C设备写操作函数错误码 */
    I2C_ERROR_WRITE_DEVICE_PTR_NULL                = 5000u, /* I2C设备指针为空 */
    I2C_ERROR_WRITE_BUFFER_PTR_NULL                = 5001u, /* I2C缓存指针为空 */
    I2C_ERROR_WRITE_READ_SIZE_ZERO                 = 5002u, /* I2C读取数据量为0 */
    I2C_ERROR_WRITE_DEVICE_NOT_OPENED              = 5003u, /* I2C设备未创建 */
    I2C_ERROR_WRITE_DEVICE_NOT_IDLE                = 5004u, /* I2C设备非空闲状态 */

    /* I2C设备读操作函数错误码 */
    I2C_ERROR_READ_DEVICE_PTR_NULL                 = 6000u, /* I2C设备指针为空 */
    I2C_ERROR_READ_BUFFER_PTR_NULL                 = 6001u, /* I2C缓存指针为空 */
    I2C_ERROR_READ_READ_SIZE_ZERO                  = 6002u, /* I2C读取数据量为0 */
    I2C_ERROR_READ_DEVICE_NOT_OPENED               = 6003u, /* I2C设备未创建 */
    I2C_ERROR_READ_DEVICE_NOT_IDLE                 = 6004u, /* I2C设备非空闲状态 */

    /* I2C总线复位函数错误码 */
    I2C_ERROR_RESETBUS_DEVICE_PTR_NULL             = 7000u, /* I2C设备指针为空 */
    I2C_ERROR_RESETBUS_DEVICE_NOT_OPENED           = 7001u, /* I2C设备未创建 */

    /* I2C中断处理函数错误码 */
    I2C_ERROR_INTHANDLER_DEVICE_PTR_NULL           = 8000u, /* I2C设备指针为空 */
    I2C_ERROR_INTHANDLER_DEVICE_NOT_OPENED         = 8001u, /* I2C设备未创建 */
    I2C_ERROR_INTHANDLER_WORKMODE_ERROR            = 8002u, /* I2C工作模式错误 */

    /* I2C设备关闭函数错误码 */
    I2C_ERROR_CLOSE_DEVICE_PTR_NULL                = 9000u, /* I2C设备指针为空 */
    I2C_ERROR_CLOSE_DEVICE_NOT_OPENED              = 9001u, /* I2C设备未打开 */
    I2C_ERROR_CLOSE_SCL_PIN_GROUP_ERROR            = 9002u, /* I2C-SCL引脚所在GPIO Group定义错误 */
    I2C_ERROR_CLOSE_SDA_PIN_GROUP_ERROR            = 9003u, /* I2C-SDA引脚所在GPIO Group定义错误 */
    I2C_ERROR_CLOSE_SCL_PIN_ERROR                  = 9004u, /* I2C-SCL引脚ID定义错误 */
    I2C_ERROR_CLOSE_SDA_PIN_ERROR                  = 9005u, /* I2C-SDA引脚ID定义错误 */
    I2C_ERROR_CLOSE_SCL_AF_ERROR                   = 9006u, /* I2C-SCL引脚复用功能定义错误 */
    I2C_ERROR_CLOSE_SDA_AF_ERROR                   = 9007u, /* I2C-SDA引脚复用功能定义错误 */
    I2C_ERROR_CLOSE_INVALID_I2C_PERIPH             = 9008u  /* I2C外设定义错误 */
} DRV_I2C_ERROR_CODE_E ;


CODE_SECTION("==========================") ;
CODE_SECTION("==  模块结构数据类型    ==") ;
CODE_SECTION("==========================") ;
/* I2C配置信息 */
typedef struct tagDRV_I2C_INIT_INFO
{
    union
    {
        UCHAR aucRawData[20] ;

        struct
        {
            struct
            {
                GPIO_TypeDef       *pstSCLGPIOGroup ;       /* 指定SCL引脚所在GPIO Group -- GPIOx ------- x = A,B,C,...K */
                GPIO_TypeDef       *pstSDAGPIOGroup ;       /* 指定SDA引脚所在GPIO Group -- GPIOx ------- x = A,B,C,...K */

                UINT                uiSCLPinID ;            /* 指定SCL引脚编号 ------------ GPIO_Pin_x -- x = 0...15 */
                UINT                uiSDAPinID ;            /* 指定SDA引脚编号 ------------ GPIO_Pin_x -- x = 0...15 */

                UCHAR               ucSCLPinAFDef ;         /* 指定SCL引脚的复用功能     */
                UCHAR               ucSDAPinAFDef ;         /* 指定SDA引脚的复用功能     */
            } stGPIOInfo ;

            struct
            {
                I2C_TypeDef        *pstI2CPeriph ;          /* I2C外设指针 ---------------- I2Cx -------- x = 1,2,3 */
                UINT                uiI2CClockSpeed ;       /* I2C速率，必须不大于400,000 */
                USHORT              usI2CDutyCycle ;        /* I2C时钟占空比，I2C_DutyCycle_2或I2C_DutyCycle_16_9 */
            } stI2CInfo ;
        } ;
    } ;
} DRV_I2C_INIT_INFO_S ;

/* I2C传输模式定义 */
typedef enum tagDRV_I2C_WORK_MODE
{
    I2C_MODE_IDLE = 0 ,   /* 就绪模式，未发生操作 */
    I2C_MODE_RD_P1 ,      /* 读模式，阶段1，写寄存器地址 */
    I2C_MODE_RD_P2 ,      /* 读模式，阶段2，读数据 */
    I2C_MODE_WR           /* 写模式 */
} DRV_I2C_WORK_MODE_E ;

/* I2C设备定义 */
typedef struct tagDRV_I2C_DEVICE
{
    union
    {
#ifdef DRV_I2C_USE_UCOS_III
        UCHAR aucRawData[4 + 20 + 3 * sizeof(OS_SEM) + 16] ;
#else
        UCHAR aucRawData[4 + 20 + 4 + 16] ;
#endif
        struct
        {
            UCHAR                   ucOpenFlag ;      /* 驱动打开标记 */
            UCHAR                   ucDevAddr ;       /* 设备地址 */
            UCHAR                   ucRegAddr ;       /* 寄存器地址 */
            UCHAR                   ucRsv0 ;          /* 保留 */
        
            /* GPIO信息登记 */
            struct
            {
                GPIO_TypeDef       *pstSCLGPIOGroup ; /* SCL引脚GPIO Group */
                GPIO_TypeDef       *pstSDAGPIOGroup ; /* SDA引脚GPIO Group */
                UINT                uiSCLPinID ;      /* SCL引脚GPIO Index */
                UINT                uiSDAPinID ;      /* SDA引脚GPIO Index */
                UCHAR               ucSCLPinAFDef ;   /* SCL引脚GPIO AF定义 */
                UCHAR               ucSDAPinAFDef ;   /* SDA引脚GPIO AF定义 */
                UCHAR               ucRsv1 ;          /* 保留 */
                UCHAR               ucRsv2 ;          /* 保留 */
            } ; /* 20B */

#ifdef DRV_I2C_USE_UCOS_III
            OS_SEM                  stI2CACCSem ;     /* 该信号量用于给I2C的访问加锁 */
            OS_SEM                  stI2CWRSem ;      /* 该信号量用于给写操作加锁 */
            OS_SEM                  stI2CRDSem ;      /* 该信号量用于给读操作加锁 */
#else
            UCHAR                   ucI2CACCSem ;     /* 该标记用于给I2C的访问加锁 */
            UCHAR                   ucI2CWRSem ;      /* 该标记用于给写操作加锁 */
            UCHAR                   ucI2CRDSem ;      /* 该标记用于给读操作加锁 */
            UCHAR                   ucRsv3 ;          /* 保留 */
#endif

            I2C_TypeDef            *pstI2CPeriph ;    /* I2C外设   */
            DRV_I2C_WORK_MODE_E     enWorkMode ;      /* 记录I2C的工作模式 */
            UCHAR                  *pucBuf ;          /* 读写缓存 */
            UINT                    uiSize ;          /* 读写数据量，随读写的进行而改变 */
        } ;
    } ;
} DRV_I2C_DEVICE_S ;


CODE_SECTION("==========================") ;
CODE_SECTION("==  模块函数声明        ==") ;
CODE_SECTION("==========================") ;

DRV_I2C_ERROR_CODE_E DRV_I2C_Open(DRV_I2C_DEVICE_S *pstI2CDevice, DRV_I2C_INIT_INFO_S *pstI2CInitInfo) ;
DRV_I2C_ERROR_CODE_E DRV_I2C_Write(DRV_I2C_DEVICE_S *pstI2CDevice, UCHAR ucDevAddr, UCHAR ucRegAddr, UCHAR *pucBuf, UINT uiSize) ;
DRV_I2C_ERROR_CODE_E DRV_I2C_Read(DRV_I2C_DEVICE_S *pstI2CDevice, UCHAR ucDevAddr, UCHAR ucRegAddr, UCHAR *pucBuf, UINT uiSize, USHORT usAddGapMs) ;
DRV_I2C_ERROR_CODE_E DRV_I2C_Close(DRV_I2C_DEVICE_S *pstI2CDevice) ;
DRV_I2C_ERROR_CODE_E DRV_I2C_ResetBus(DRV_I2C_DEVICE_S *pstI2CDevice) ;
DRV_I2C_ERROR_CODE_E DRV_I2C_GeneralIntHandler(DRV_I2C_DEVICE_S *pstI2CDevice) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __DRV_I2C_STM32F4XX_H__ */

/******* End of file drv_i2c_stm32f4xx.h. *******/  
