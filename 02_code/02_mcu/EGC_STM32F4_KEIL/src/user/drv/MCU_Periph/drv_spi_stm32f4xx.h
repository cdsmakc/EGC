/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : drv_spi_stm32f4xx.h
    Project name : STM32F4XX版本SPI外设驱动。
    Module name  : 
    Date created : 2020年4月16日   16时41分3秒
    Author       : Ning.JianLi
    Description  : STM32F4XX版本SPI外设驱动头文件。
*******************************************************************************/

/* ##############################################################################################
 * ## 版本说明                                                                                     ##
 * ##############################################################################################
 * REV1.0    Ning.JianLi   初始版本。
 * REV1.1    Ning.JianLi   增加 DRV_SPI_WRITE_STORE_BACK_DATA 宏，并添加相关代码，使得执行SPI写入
 *                         时，能够保存返回的数据。
 *                         这是因为，在使用ADS128S022芯片时，第一个发送的数据，所对应的返回数据中
 *                         包含有部分有效数据。
 * REV1.2    Ning.JianLi   修正代码20210830No01。修正错误。函数DRV_SPI_Open和函数DRV_SPI_Close中，
 *                         对初始化信息DMA通道的判断有问题。当编译宏DRV_SPI_USE_DMA被定义，但是初
 *                         始化信息中bUseDMA设置BOOL_FALSE时，仍然会检查pstSPIDMARxChannel和
 *                         pstSPIDMATxChannel，实际上这两个信息在此种情况下用不到，用户也无需配置。
 */

/* ##############################################################################################
 * 如何使用本模块？
 * ##############################################################################################
 * 1. 如果需要使用UCOS-III操作系统，定义宏DRV_SPI_USE_UCOS_III。该定义使得操作函数内部的等待变为
 *    非阻塞式的。
 * 2. 如果需要使用DMA实现较快的、大数据量的、无CPU介入的传输操作，定义宏DRV_SPI_USE_DMA。并且，在
 *    使用函数DRV_SPI_Open创建SPI设备时，需要指定使用DMA功能。
 * 3. 使用函数DRV_SPI_Open创建SPI设备。其配置参数可以是动态的，使用完毕后即可释放。SPI器件参数需要
 *    是静态的，在使用函数DRV_SPI_Close关闭设备之前，不能被释放，并且不能被外部访问。
 * 4. 使用函数DRV_SPI_Close关闭设备。
 * 5. 使用函数DRV_SPI_Write进行SPI写入操作。注意最后一个参数，必须指定一种操作模式。
 * 6. 使用函数DRV_SPI_Read进行SPI读取操作。注意最后一个参数，必须指定一种操作模式。
 * 7. 如果需要SPI外设中断方式传输，需要定义新的中断处理函数，并在函数中调用DRV_SPI_GeneralIntHandler,
 *    并将SPI设备作为参数传入。新的中断处理函数需要关联到cstartup.s文件中的SPI中断服务。
 * 8. 如果需要SPI外设DMA方式传输，需要定义新的中断处理函数，并在函数中调用DRV_SPI_GeneralDMAIntHandler,
 *    并将SPI设备作为参数传入。新的中断处理函数需要关联到cstartup.s文件中的中断服务，包括发送流和
 *    接收流。
 */

#ifndef __DRV_SPI_STM32F4XX_H__
#define __DRV_SPI_STM32F4XX_H__

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#pragma anon_unions
//#pragma pack(1)

#include "stm32f4xx.h"
#include "pub_type.h"
#include "pub_lib.h"

CODE_SECTION("==========================") ;
CODE_SECTION("==  移植相关定义        ==") ;
CODE_SECTION("==========================") ;

/* 以下宏定义确定是否在UC/OS-III系统中使用本模块.如果在该系统中使用本模块,应该定义该宏 */
#define DRV_SPI_USE_UCOS_III

/* 以下宏定义确定本模块是否支持DMA方式传输.如果定义该宏,创建SPI设备时需要额外的DMA相关信息 */
#define DRV_SPI_USE_DMA

#ifdef DRV_SPI_USE_UCOS_III
#include "os.h"
#endif

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块内部宏定义      ==") ;
CODE_SECTION("==========================") ;

/* SPI设备已经打开特征值 */
#define DRV_SPI_OPEN_FLAG                (0x5A)

/* 以下宏定义，定义了SPI的工作模式 */
#define DRV_SPI_MODE_0                   0                  /* SPI工作方式0 */
#define DRV_SPI_MODE_1                   1                  /* SPI工作方式1 */
#define DRV_SPI_MODE_2                   2                  /* SPI工作方式2 */
#define DRV_SPI_MODE_3                   3                  /* SPI工作方式3 */

/* 以下宏定义，定时SPI读写操作时，的操作模式 */
#define DRV_SPI_READ_WRITE_MODE_QUERY    0x00000001         /* 查询方式操作 */
#define DRV_SPI_READ_WRITE_MODE_INT      0x00000002         /* 中断方式操作 */
#ifdef DRV_SPI_USE_DMA
#define DRV_SPI_READ_WRITE_MODE_DMA      0x00000004         /* DMA方式操作 */
#endif
#define DRV_SPI_READ_WRITE_WITH_START    0x00000008         /* 读写前产生Start */
#define DRV_SPI_READ_WRITE_WITH_STOP     0x00000010         /* 读写后产生Stop */
#define DRV_SPI_WRITE_STORE_BACK_DATA    0x00000020         /* 写入时保存返回的数据 */

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块枚举数据类型    ==") ;
CODE_SECTION("==========================") ;

/* 以下枚举类型定义了SPI器件模块操作函数的错误码 */
typedef enum tagDRV_SPI_ERROR_CODE
{
    /* 操作正常 */
    SPI_ERROR_OK                                   =    0u, /* 操作正常 */
    
    /* SPI设备打开函数错误码 */
    SPI_ERROR_OPEN_SPI_DEVICE_PTR_NULL             = 2000u, /* SPI设备指针为空 */
    SPI_ERROR_OPEN_CONFIG_INFO_PTR_NULL            = 2001u, /* 配置信息指针为空 */
    SPI_ERROR_OPEN_SCK_PIN_GROUP_ERROR             = 2002u, /* SCK引脚所在GPIO Group定义错误 */
    SPI_ERROR_OPEN_NSS_PIN_GROUP_ERROR             = 2003u, /* NSS引脚所在GPIO Group定义错误 */
    SPI_ERROR_OPEN_MOSI_PIN_GROUP_ERROR            = 2004u, /* MOSI引脚所在GPIO Group定义错误 */
    SPI_ERROR_OPEN_MISO_PIN_GROUP_ERROR            = 2005u, /* MISO引脚所在GPIO Group定义错误 */
    SPI_ERROR_OPEN_SCK_PIN_ERROR                   = 2006u, /* SCK引脚ID定义错误 */
    SPI_ERROR_OPEN_NSS_PIN_ERROR                   = 2007u, /* NSS引脚ID定义错误 */
    SPI_ERROR_OPEN_MOSI_PIN_ERROR                  = 2008u, /* MOSI引脚ID定义错误 */
    SPI_ERROR_OPEN_MISO_PIN_ERROR                  = 2009u, /* MISO引脚ID定义错误 */
    SPI_ERROR_OPEN_SCK_AF_ERROR                    = 2010u, /* SCK引脚复用功能定义错误 */
    SPI_ERROR_OPEN_MISO_AF_ERROR                   = 2011u, /* MISO引脚复用功能定义错误 */
    SPI_ERROR_OPEN_MOSI_AF_ERROR                   = 2012u, /* MOSI引脚复用功能定义错误 */
    SPI_ERROR_OPEN_INVALID_SPI_PERIPH              = 2013u, /* SPI外设定义错误 */
    SPI_ERROR_OPEN_INVALID_SPI_PRESCALER           = 2014u, /* SPI预分频系数错误 */
    SPI_ERROR_OPEN_INVALID_SPI_MODE                = 2015u, /* SPI模式设定错误 */
#ifdef DRV_SPI_USE_DMA
    SPI_ERROR_OPEN_INVALID_DMA_RX_STREAM           = 2080u, /* DMA RX流指定错误 */
    SPI_ERROR_OPEN_INVALID_DMA_TX_STREAM           = 2081u, /* DMA TX流指定错误 */
    SPI_ERROR_OPEN_INVALID_DMA_RX_CHANNEL          = 2082u, /* DMA RX通道指定错误 */
    SPI_ERROR_OPEN_INVALID_DMA_TX_CHANNEL          = 2083u, /* DMA TX通道指定错误 */
#endif 

    /* SPI设备关闭函数错误码 */
    SPI_ERROR_CLOSE_SPI_DEVICE_PTR_NULL            = 3000u, /* SPI设备指针为空 */
    SPI_ERROR_CLOSE_CONFIG_INFO_PTR_NULL           = 3001u, /* 配置信息指针为空 */
    SPI_ERROR_CLOSE_SCK_PIN_GROUP_ERROR            = 3002u, /* SCK引脚所在GPIO Group定义错误 */
    SPI_ERROR_CLOSE_NSS_PIN_GROUP_ERROR            = 3003u, /* NSS引脚所在GPIO Group定义错误 */
    SPI_ERROR_CLOSE_MOSI_PIN_GROUP_ERROR           = 3004u, /* MOSI引脚所在GPIO Group定义错误 */
    SPI_ERROR_CLOSE_MISO_PIN_GROUP_ERROR           = 3005u, /* MISO引脚所在GPIO Group定义错误 */
    SPI_ERROR_CLOSE_SCK_PIN_ERROR                  = 3006u, /* SCK引脚ID定义错误 */
    SPI_ERROR_CLOSE_NSS_PIN_ERROR                  = 3007u, /* NSS引脚ID定义错误 */
    SPI_ERROR_CLOSE_MOSI_PIN_ERROR                 = 3008u, /* MOSI引脚ID定义错误 */
    SPI_ERROR_CLOSE_MISO_PIN_ERROR                 = 3009u, /* MISO引脚ID定义错误 */
    SPI_ERROR_CLOSE_INVALID_SPI_PERIPH             = 3010u, /* SPI外设定义错误 */
    SPI_ERROR_CLOSE_SPI_NOT_OPENED                 = 3011u, /* SPI设备没有被打开，或者已经关闭 */
#ifdef DRV_SPI_USE_DMA
    SPI_ERROR_CLOSE_INVALID_DMA_RX_STREAM          = 3080u, /* DMA RX流指定错误 */
    SPI_ERROR_CLOSE_INVALID_DMA_TX_STREAM          = 3081u, /* DMA TX流指定错误 */
#endif 

    /* SPI设备启动传输函数错误码 */
    SPI_ERROR_START_SPI_NOT_OPENED                 = 4000u, /* SPI设备没有被打开，或者已经关闭 */

    /* SPI设备停止传输函数错误码 */
    SPI_ERROR_STOP_SPI_NOT_OPENED                  = 5000u, /* SPI设备没有被打开，或者已经关闭 */
    SPI_ERROR_STOP_ALREADY_STOP                    = 5001u, /* SPI设备传输已经停止 */

    /* SPI设备写入函数错误码 */
    SPI_ERROR_WRITE_SPI_NOT_OPENED                 = 6000u, /* SPI设备没有被打开，或者已经关闭 */
    SPI_ERROR_WRITE_SPI_DEVICE_P_NULL              = 6001u, /* SPI设备指定错误,该设备未定义,为空 */
    SPI_ERROR_WRITE_BUF_P_NULL                     = 6002u, /* 写缓存指定错误,为空 */
    SPI_ERROR_WRITE_DMA_NOT_CONFIGED               = 6003u, /* 指定了DMA方式传输，但配置时未使能DMA */
    SPI_ERROR_WRITE_BACK_BUF_P_NULL                = 6004u, /* 指定写入需要保存返回数据时，给与的返回数据缓存指针为空 */
#ifdef DRV_SPI_USE_DMA
    SPI_ERROR_WRITE_RECORD_BACK_DATA_WHEN_DMA      = 6005u, /* DMA写入时不可保存返回数据 */
#endif

    /* SPI设备读取函数错误码 */
    SPI_ERROR_READ_SPI_NOT_OPENED                  = 7000u, /* SPI设备没有被打开，或者已经关闭 */
    SPI_ERROR_READ_SPI_DEVICE_P_NULL               = 7001u, /* SPI设备指定错误,该设备未定义,为空 */
    SPI_ERROR_READ_BUF_P_NULL                      = 7002u, /* 读缓存指定错误,为空 */
    SPI_ERROR_READ_DMA_NOT_CONFIGED                = 7003u  /* 指定了DMA方式传输，但配置时未使能DMA */
} DRV_SPI_ERROR_CODE_E ;

/* 以下枚举类型定义了SPI的操作模式 */
typedef enum tagDRV_SPI_CURRENT_OPERATION
{
    SPI_CURRENT_OPERATION_WR                       =    0u, /* 写入 */
    SPI_CURRENT_OPERATION_RD                       =    1u  /* 读取 */
} DRV_SPI_CURRENT_OPERATION_E ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块结构数据类型    ==") ;
CODE_SECTION("==========================") ;
/* 以下结构类型定义了打开或关闭SPI设备时的配置信息 */
typedef struct tagDRV_SPI_INIT_INFO
{
    union
    {
#ifdef DRV_SPI_USE_DMA
        UCHAR ucRawData[73] ;
#else
        UCHAR ucRawData[48] ;
#endif

        struct
        {
            struct
            {
                GPIO_TypeDef       *pstSCKGPIOGroup ;       /* 指定SCK引脚所在GPIO Group， GPIOx，            x = A,B,C,...K */
                GPIO_TypeDef       *pstNSSGPIOGroup ;       /* 指定NSS引脚所在GPIO Group， GPIOx，            x = A,B,C,...K */
                GPIO_TypeDef       *pstMOSIGPIOGroup ;      /* 指定MOSI引脚所在GPIO Group，GPIOx，            x = A,B,C,...K */
                GPIO_TypeDef       *pstMISOGPIOGroup ;      /* 指定MISO引脚所在GPIO Group，GPIOx，            x = A,B,C,...K */

                UINT                uiSCKPinID ;            /* 指定SCK引脚编号，           GPIO_Pin_x，       x = 0...15 */
                UINT                uiNSSPinID ;            /* 指定NSS引脚编号，           GPIO_Pin_x，       x = 0...15 */
                UINT                uiMOSIPinID ;           /* 指定MOSI引脚编号，          GPIO_Pin_x，       x = 0...15 */
                UINT                uiMISOPinID ;           /* 指定MISO引脚编号，          GPIO_Pin_x，       x = 0...15 */

                UCHAR               ucSCKPinAFDef ;         /* 指定SCK引脚的复用功能 */
                UCHAR               ucMOSIPinAFDef ;        /* 指定MOSI引脚的复用功能 */
                UCHAR               ucMISOPinAFDef ;        /* 指定MISO引脚的复用功能 */
                UCHAR               ucRsv0 ;
            } ; /* 36B */

            struct 
            {
                SPI_TypeDef        *pstSPIPeriph ;          /* SPI外设指针，               SPIx，             x = 1,2,3,...6 */
                USHORT              usSPIPrescaler ;        /* 指定SPI预分频系数，参考SPI_BaudRate_Prescaler */
                UCHAR               ucSPIMode ;             /* 指定SPI的模式，             DRV_SPI_MODE_x，   x =        0...3 */
                UCHAR               ucRsv1 ;
                UINT                uiSPIIrqNumber ;        /* SPI中断序号 */
            } ; /* 12B */

#ifdef DRV_SPI_USE_DMA
            BOOL_T                  bUseDMA ;
            struct 
            {
                DMA_Stream_TypeDef *pstSPIDMATxStream ;     /* 指定TX DMA使用的流 */
                DMA_Stream_TypeDef *pstSPIDMARxStream ;     /* 指定RX DMA使用的流 */
                UINT                uiSPIDMATxChannel ;     /* 指定TX DMA使用的通道 */
                UINT                uiSPIDMARxChannel ;     /* 指定RX DMA使用的通道 */
                UINT                uiSPIDMATxIrqNum ;      /* 指定TX DMA中断编号 */
                UINT                uiSPIDMARxIrqNum ;      /* 指定RX DMA中断编号 */
            } ; /* 25B */
#endif
        } ;
    } ;
} DRV_SPI_INIT_INFO_S ;

/* 以下结构类型定义了SPI设备 */
typedef struct tagDRV_SPI_DEVICE
{
    union
    {
#if defined(DRV_SPI_USE_UCOS_III) && defined(DRV_SPI_USE_DMA)
        UCHAR aucRawData[38 + sizeof(OS_SEM) + 16] ;
#elif defined(DRV_SPI_USE_UCOS_III)
        UCHAR aucRawData[38 + sizeof(OS_SEM)] ;
#elif defined(DRV_SPI_USE_DMA)
        UCHAR aucRawData[38 + 1 + 16] ;
#else
        UCHAR aucRawData[38 + 1] ;
#endif

    struct
        {
            /* 部件创建标记 */
            UCHAR                        ucOpenFlag ;           /* 设备打开标记 */
            UCHAR                        ucSPIStart ;
            
            /* GPIO相关信息，NSS引脚由软件驱动，需要记录 */
            GPIO_TypeDef                *pstNSSGPIOGroup ;      /* 指定NSS引脚所在GPIO Group，GPIOx，x = A,B,C,...K */
            UINT                         uiNSSPinID ;           /* 指定NSS引脚编号，GPIO_Pin_x，x =         0...15 */

            /* 控制位记录 */
            UINT                         uiMode ;

            /* SPI外设相关信息 */
            SPI_TypeDef                 *pstSPIPeriph ;         /* SPI外设指针，SPIx，x = 1,2,3,...6 */

            /* 当前SPI传输操作 */
            DRV_SPI_CURRENT_OPERATION_E  enCurrentOperation ;   /* 当前SPI正在进行的操作 */

            /* 缓冲区指针 */
            UCHAR                       *pucWriteBuf ;          /* SPI写入操作的源数据缓存 */
            UCHAR                       *pucReadBuf ;           /* SPI读取操作的目的数据缓存 */
            UCHAR                       *pucBackBuf ;           /* SPI写入操作的返回数据缓存 */
            UINT                         uiSize ;               /* SPI读写操作的数据量 */
            
#ifndef DRV_SPI_USE_UCOS_III
            UCHAR                        ucOperationDone ;      /* 操作完成标记 */
#else 
            OS_SEM                       stSemOperationDone ;   /* 操作完成信号量 */
#endif

#ifdef DRV_SPI_USE_DMA
            BOOL_T                       bUseDMA ;
            DMA_Stream_TypeDef          *pstSPIDMATxStream ;    /* 指定TX DMA使用的流 */
            DMA_Stream_TypeDef          *pstSPIDMARxStream ;    /* 指定RX DMA使用的流 */
            UINT                         uiSPIDMATxStreamTCIF ; /* 指定TX DMA传输完成中断标记 */
            UINT                         uiSPIDMARxStreamTCIF ; /* 指定RX DMA传输完成中断标记 */
#endif
        } ;
    } ;
}DRV_SPI_DEVICE_S ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块函数声明        ==") ;
CODE_SECTION("==========================") ;
DRV_SPI_ERROR_CODE_E DRV_SPI_Open(DRV_SPI_DEVICE_S *pstSPIDevice, DRV_SPI_INIT_INFO_S *pstSPIInitInfo) ;
DRV_SPI_ERROR_CODE_E DRV_SPI_Close(DRV_SPI_DEVICE_S *pstSPIDevice, DRV_SPI_INIT_INFO_S *pstSPIInitInfo) ;
DRV_SPI_ERROR_CODE_E DRV_SPI_Write(DRV_SPI_DEVICE_S *pstSPIDevice, UCHAR *pucWriteBuf, UCHAR *pucBackBuf, UINT uiSize, UINT uiMode) ;
DRV_SPI_ERROR_CODE_E DRV_SPI_Read(DRV_SPI_DEVICE_S *pstSPIDevice, UCHAR *pucReadBuf, UINT uiSize, UINT uiMode) ;
VOID DRV_SPI_GeneralIntHandler(DRV_SPI_DEVICE_S *pstSPIDevice) ;
#ifdef DRV_SPI_USE_DMA
VOID DRV_SPI_GeneralDMAIntHandler(DRV_SPI_DEVICE_S *pstSPIDevice) ;
#endif
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __DRV_SPI_STM32F4XX_H__ */

/******* End of file drv_spi_stm32f4xx.h. *******/  
