/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : drv_uart_stm32f4xx.h
    Project name : STM32F4XX版本UART外设驱动。
    Module name  : 
    Date created : 2020年9月30日   13时48分51秒
    Author       : Ning.JianLi
    Description  : STM32F4XX版本UART外设驱动头文件。
*******************************************************************************/

/* ##############################################################################################
 * ## 版本说明                                                                                     ##
 * ##############################################################################################
 * REV1.0 2020/9/30    Ning.JianLi   初始版本。
 * REV1.1 2020/12/02   Ning.JianLi
 *                                   write函数中以下语句：
 *                                   if(pstUARTDevice->usTXBufRdPtr >= pstUARTDevice->usTXBufWrPtr)
 *                                   当2个指针相同，且指针趋近于缓存尾部时，会导致写指针溢出。应当取
 *                                   消等于的可能性。等于时走else分支；
 * REV1.2 2021/7/25    Ning.JianLi   
 *                                   修改1 ：
 *                                   read函数中如下语句：
 *                                   pstUARTDevice->usRXBufRdPtr += (uiSize - uiCopySize) ;
 *                                   明显的逻辑错误。指针应该增加第二次拷贝的内容长度。
 *                                   修改2 ：
 *                                   read函数中如下语句：
 *                                   pstUARTDevice->usRXBufRdPtr += ui2ndCopySize ;
 *                                   仅仅考虑了数据源buf的指针变化，还需要考虑拷贝目的buf的指针变化。
 *                                   在下方增加：
 *                                   pucData                     += ui2ndCopySize ;
 *                                   修改3 ：
 *                                   增加UART外设中断号判断。用户无需设置，初始化程序自动判断中断号。
 */

/* ##############################################################################################
 * 如何使用本模块？
 * ##############################################################################################
 * 1. 如果需要使用UCOS-III操作系统，定义宏DRV_UART_USE_UCOS_III。该定义使得操作函数内部的等待变为
 *    非阻塞式的。
 * 2. 使用函数DRV_UART_Open创建UART设备。其配置参数可以是动态的，使用完毕后即可释放。UART器件参数
 *    需要是静态的，在使用函数DRV_UART_Close关闭设备之前，不能被释放，并且不能被外部访问。
 * 3. 使用函数DRV_UART_Close关闭设备。
 * 4. 使用函数DRV_UART_Write进行UART写入操作。注意最后一个参数，必须指定一种操作模式。
 * 5. 使用函数DRV_UART_Read进行UART读取操作。注意最后一个参数，必须指定一种操作模式。
 * 6. 需要定义新的中断处理函数，并在函数中调用DRV_UART_GeneralIntHandler,并将UART设备作为参数传入。
 *    新的中断处理函数需要关联到cstartup.s文件中的UART中断服务。
 */

#ifndef __DRV_UART_STM32F4XX_H__
#define __DRV_UART_STM32F4XX_H__

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
#define DRV_UART_USE_UCOS_III

#ifdef DRV_UART_USE_UCOS_III
#include "os.h"
#endif

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块内部宏定义      ==") ;
CODE_SECTION("==========================") ;

/* UART设备已经打开特征值 */
#define DRV_UART_OPEN_FLAG               (0x5B)

/* 以下宏定义，定义UART读写操作时，的操作模式 */
#define DRV_UART_WRITE_MODE_NONE         0x00000000  /* 写入操作不指定任何模式 */
#define DRV_UART_WRITE_MODE_WAIT_DONE    0x00000001  /* 阻塞写入，UART写入操作，直到数据完全传输完毕之后才会返回；
                                                      * 不定义这种模式，数据在拷贝到缓存后会立即返回。
                                                      */
#define DRV_UART_READ_MODE_NONE          0x00000000  /* 读取操作不指定任何模式 */

/* 以下宏用于检查指定缓存的容量是否合法 */
#define IS_UART_BUFFER_SIZE(SIZE) (((SIZE) == 256) || \
                                   ((SIZE) == 512) || \
                                   ((SIZE) == 1024) || \
                                   ((SIZE) == 2048) || \
                                   ((SIZE) == 4096))

/* BEGIN : 增加UART外设中断号判断。用户无需设置，初始化程序自动判断中断号。 */
/* UART外设中断编号 */
#define DRV_UART_PERIPH_IRQ_NUMBER_USART1        (37u)
#define DRV_UART_PERIPH_IRQ_NUMBER_USART2        (38u)
#define DRV_UART_PERIPH_IRQ_NUMBER_USART3        (39u)
#define DRV_UART_PERIPH_IRQ_NUMBER_UART4         (52u)
#define DRV_UART_PERIPH_IRQ_NUMBER_UART5         (53u)
#define DRV_UART_PERIPH_IRQ_NUMBER_USART6        (71u)
#define DRV_UART_PERIPH_IRQ_NUMBER_UART7         (82u)
#define DRV_UART_PERIPH_IRQ_NUMBER_UART8         (83u)
/* END   : 增加UART外设中断号判断。用户无需设置，初始化程序自动判断中断号。 */

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块枚举数据类型    ==") ;
CODE_SECTION("==========================") ;

/* 以下枚举类型定义了UART模块操作函数的错误码 */
typedef enum tagDRV_UART_ERROR_CODE
{
    /* 操作正常 */
    UART_ERROR_OK                                   =    0u, /* 操作正常 */
    
    /* UART设备打开函数错误码 */
    UART_ERROR_OPEN_UART_DEVICE_PTR_NULL            = 2000u, /* UART设备指针为空 */
    UART_ERROR_OPEN_CONFIG_INFO_PTR_NULL            = 2001u, /* UART配置信息指针为空 */
    UART_ERROR_OPEN_TX_PIN_GROUP_ERROR              = 2002u, /* TX引脚所在GPIO Group定义错误 */
    UART_ERROR_OPEN_RX_PIN_GROUP_ERROR              = 2003u, /* RX引脚所在GPIO Group定义错误 */
    UART_ERROR_OPEN_TX_PIN_ERROR                    = 2004u, /* TX引脚ID定义错误 */
    UART_ERROR_OPEN_RX_PIN_ERROR                    = 2005u, /* RX引脚ID定义错误 */
    UART_ERROR_OPEN_TX_AF_ERROR                     = 2006u, /* TX引脚复用功能定义错误 */
    UART_ERROR_OPEN_RX_AF_ERROR                     = 2007u, /* RX引脚复用功能定义错误 */
    UART_ERROR_OPEN_INVALID_UART_PERIPH             = 2008u, /* UART外设定义错误 */
    UART_ERROR_OPEN_BAUDRATE_ERROR                  = 2009u, /* 波特率配置错误 */
    UART_ERROR_OPEN_TX_BUFFER_SIZE_ERROR            = 2010u, /* 发送缓存大小指定错误 */
    UART_ERROR_OPEN_RX_BUFFER_SIZE_ERROR            = 2011u, /* 接收缓存大小指定错误 */
    UART_ERROR_OPEN_ALLOC_TX_BUFFER_SIZE_ERROR      = 2012u, /* 分配发送缓存失败 */
    UART_ERROR_OPEN_ALLOC_RX_BUFFER_SIZE_ERROR      = 2013u, /* 分配接收缓存失败 */

    /* UART设备关闭函数错误码 */
    UART_ERROR_CLOSE_UART_DEVICE_PTR_NULL           = 3000u, /* UART设备指针为空 */
    UART_ERROR_CLOSE_CONFIG_INFO_PTR_NULL           = 3001u, /* 配置信息指针为空 */
    UART_ERROR_CLOSE_TX_PIN_GROUP_ERROR             = 3002u, /* TX引脚所在GPIO Group定义错误 */
    UART_ERROR_CLOSE_RX_PIN_GROUP_ERROR             = 3003u, /* RX引脚所在GPIO Group定义错误 */
    UART_ERROR_CLOSE_TX_PIN_ERROR                   = 3004u, /* TX引脚ID定义错误 */
    UART_ERROR_CLOSE_RX_PIN_ERROR                   = 3005u, /* RX引脚ID定义错误 */
    UART_ERROR_CLOSE_INVALID_UART_PERIPH            = 3006u, /* UART外设定义错误 */
    UART_ERROR_CLOSE_UART_NOT_OPENED                = 3007u, /* UART设备没有被打开，或者已经关闭 */

    /* UART设备写入错误码 */
    UART_ERROR_WRITE_UART_DEVICE_P_NULL             = 4000u, /* UART设备指定错误,该设备未定义,为空 */
    UART_ERROR_WRITE_BUF_P_NULL                     = 4001u, /* 写缓存指定错误,为空 */
    UART_ERROR_WRITE_UART_NOT_OPENED                = 4002u, /* UART设备没有被打开，或者已经关闭 */
    UART_ERROR_WRITE_MODE_ERROR                     = 4003u, /* 模式指定错误 */
    UART_ERROR_WRITE_BUF_NO_SPACE                   = 4004u, /* 写入时，缓存无空间 */

    /* UART设备读取错误码 */
    UART_ERROR_READ_UART_DEVICE_P_NULL              = 5000u, /* UART设备指定错误,该设备未定义,为空 */
    UART_ERROR_READ_BUF_P_NULL                      = 5001u, /* 读缓存指定错误,为空 */
    UART_ERROR_READ_READ_SIZE_P_NULL                = 5002u, /* 返回数据量指针为空 */
    UART_ERROR_READ_MODE_ERROR                      = 5003u, /* 模式指定错误 */
    UART_ERROR_READ_UART_NOT_OPENED                 = 5004u  /* UART设备没有被打开，或者已经关闭 */
} DRV_UART_ERROR_CODE_E ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块结构数据类型    ==") ;
CODE_SECTION("==========================") ;
/* 以下结构类型定义了打开或关闭UART设备时的配置信息 */
typedef struct tagDRV_UART_INIT_INFO
{
    union
    {
		/* BEGIN : 增加UART外设中断号判断。用户无需设置，初始化程序自动判断中断号。 */
        /* UCHAR ucRawData[40] ; */
		UCHAR ucRawData[36] ;
		/* END   : 增加UART外设中断号判断。用户无需设置，初始化程序自动判断中断号。 */

        struct
        {
            /* GPIO相关信息 */
            struct
            {
                GPIO_TypeDef              *pstTXGPIOGroup ;   /* 指定TX引脚所在GPIO Group，GPIOx，x = A,B,C,... */
                GPIO_TypeDef              *pstRXGPIOGroup ;   /* 指定RX引脚所在GPIO Group，GPIOx，x = A,B,C,... */

                UINT                       uiTXPinID ;        /* 指定TX引脚编号，GPIO_Pin_x，x =         0...15 */
                UINT                       uiRXPinID ;        /* 指定RX引脚编号，GPIO_Pin_x，x =         0...15 */

                UCHAR                      ucTXPinAFDef ;     /* 指定TX引脚的复用功能 */
                UCHAR                      ucRXPinAFDef ;     /* 指定RX引脚的复用功能 */
                UCHAR                      ucRsv0 ;
                UCHAR                      ucRsv1 ;
            } ; /* 20B */

            /* UART相关信息 */
            struct 
            {
                USART_TypeDef             *pstUARTPeriph ;    /* UART外设指针，可能为：
                                                               * UARTx，x = 4,5,7,8
                                                               * USARTx，x = 1,2,3,6
                                                               */
                UINT                       uiBaudrate ;
				/* BEGIN : 增加UART外设中断号判断。用户无需设置，初始化程序自动判断中断号。 */
                /* UINT                       uiUARTIrqNumber ; */  /* UART中断序号 */
				/* END   : 增加UART外设中断号判断。用户无需设置，初始化程序自动判断中断号。 */

                UINT                       uiTXBufSize ;      /* 256,512,1024,2048,4096 */
                UINT                       uiRXBufSize ;      /* 256,512,1024,2048,4096 */
            } ; /* 16B */
        } ;
    } ;
} DRV_UART_INIT_INFO_S ;

/* 以下结构类型定义了UART设备 */
typedef struct tagDRV_UART_DEVICE
{
    union
    {
#ifndef DRV_UART_USE_UCOS_III
        UCHAR aucRawData[26 + 2] ;
#else
        UCHAR aucRawData[26 + sizeof(OS_SEM)] ;
#endif


        struct
        {
            /* 设备创建标记 */
            UCHAR                          ucOpenFlag ;

            /* UART设备定义 */
            USART_TypeDef                 *pstUARTPeriph ;    /* UART外设指针，可能为：
                                                               * UARTx，x = 4,5,7,8
                                                               * USARTx，x = 1,2,3,6
                                                               */
            /* 读写缓存 */
            UCHAR                         *pucTXBuf ;         /* 写缓存 */
            USHORT                         usTXBufWrPtr ;     /* 写缓存写入指针 */
            USHORT                         usTXBufRdPtr ;     /* 写缓存读取指针 */
            UINT                           uiTXBufSize ;
            
            UCHAR                         *pucRXBuf ;         /* 读缓存 */
            USHORT                         usRXBufWrPtr ;     /* 读缓存写入指针 */
            USHORT                         usRXBufRdPtr ;     /* 读缓存读取指针 */
            UINT                           uiRXBufSize ;

#ifndef DRV_UART_USE_UCOS_III
            UCHAR                          ucTXDone ;         /* UART发送完成标记 */
            UCHAR                          ucRXDone ;         /* UART接收完成标记 */
#else 
            OS_SEM                         stSemTxDone ;      /* 操作完成信号量 */
#endif
        } ;
    } ;
}DRV_UART_DEVICE_S ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块函数声明        ==") ;
CODE_SECTION("==========================") ;
DRV_UART_ERROR_CODE_E DRV_UART_Open(DRV_UART_DEVICE_S *pstUARTDevice, DRV_UART_INIT_INFO_S *pstUARTInitInfo) ;
DRV_UART_ERROR_CODE_E DRV_UART_Close(DRV_UART_DEVICE_S *pstUARTDevice, DRV_UART_INIT_INFO_S *pstUARTInitInfo) ;
DRV_UART_ERROR_CODE_E DRV_UART_Write(DRV_UART_DEVICE_S *pstUARTDevice, UCHAR *pucData, UINT uiSize, UINT uiMode) ;
DRV_UART_ERROR_CODE_E DRV_UART_Read(DRV_UART_DEVICE_S *pstUARTDevice, UCHAR *pucData, UINT uiSize, UINT *puiReadSize, UINT uiMode) ;
VOID DRV_UART_GeneralIntHandler(DRV_UART_DEVICE_S *pstUARTDevice) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __DRV_UART_STM32F4XX_H__ */

/******* End of file drv_uart_stm32f4xx.h. *******/  
