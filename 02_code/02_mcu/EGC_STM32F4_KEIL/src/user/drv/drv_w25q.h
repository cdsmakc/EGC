/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : drv_w25q.h
    Project name : 
    Module name  : 
    Date created : 2021年9月18日   9时59分31秒
    Author       : Ning.JianLi
    Description  : W25Q SPI Flash器件驱动程序。
*******************************************************************************/

#ifndef __DRV_W25Q_H__
#define __DRV_W25Q_H__

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "os.h"

#include "pub_type.h"
#include "pub_lib.h"

#pragma anon_unions

CODE_SECTION("==========================") ;
CODE_SECTION("==  移植定义            ==") ;
CODE_SECTION("==========================") ;
           

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块内部宏定义      ==") ;
CODE_SECTION("==========================") ;
/* W25Q Flash器件相关特定参数 */
#define DRV_W25Q_FLASH_PAGE_SIZE                    (256)
#define DRV_W25Q_FLASH_SECTOR_SIZE                  (4096)
#define DRV_W25Q_FLASH_MANUFACTURER_ID              (0xEF)
#define DRV_W25Q_FLASH_MEMORY_TYPE_ID               (0X40)
#define DRV_W25Q_FLASH_CAPACITY_ID                  (0x18)
#define DRV_W25Q_FLASH_JEDEC_ID                     ((DRV_W25Q_FLASH_MANUFACTURER_ID << 16) |\
                                                    (DRV_W25Q_FLASH_MEMORY_TYPE_ID << 16) |\
                                                     DRV_W25Q_FLASH_CAPACITY_ID)

/* W25Q Flash器件指令码 */
#define DRV_W25Q_INS_CODE_WRITE_ENABLE              (0x06)
#define DRV_W25Q_INS_CODE_WRITE_DISABLE             (0x04)
#define DRV_W25Q_INS_CODE_READ_STATUS_REG1          (0x05)
#define DRV_W25Q_INS_CODE_READ_STATUS_REG2          (0x35)
#define DRV_W25Q_INS_CODE_READ_STATUS_REG3          (0x15)
#define DRV_W25Q_INS_CODE_WRITE_STATUS_REG1         (0x01)
#define DRV_W25Q_INS_CODE_WRITE_STATUS_REG2         (0x31)
#define DRV_W25Q_INS_CODE_WRITE_STATUS_REG3         (0x11)
#define DRV_W25Q_INS_CODE_READ_DATA                 (0x03)
#define DRV_W25Q_INS_CODE_PAGE_PROGRAM              (0x02)
#define DRV_W25Q_INS_CODE_SECTOR_ERASE              (0x20)
#define DRV_W25Q_INS_CODE_32K_BLOCK_ERASE           (0x52)
#define DRV_W25Q_INS_CODE_64K_BLOCK_ERASE           (0xD8)
#define DRV_W25Q_INS_CODE_CHIP_ERASE                (0xC7)
#define DRV_W25Q_INS_CODE_POWER_DOWN                (0xB9)
#define DRV_W25Q_INS_CODE_RELEASE_POWER_DOWN_DEV_ID (0xAB)
#define DRV_W25Q_INS_CODE_READ_JEDEC_ID             (0x9F)
#define DRV_W25Q_INS_CODE_ENABLE_RESET              (0x66)
#define DRV_W25Q_INS_CODE_RESET                     (0x99)

/* W25Q填充无效字节定义 */
#define DRV_W25Q_DUMMY_BYTE                         (0x00)

/* W25Q器件打开标记 */
#define DRV_W25Q_DEVICE_OPEN_FLAG                   (0xAABBCCDD)

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块枚举数据类型    ==") ;
CODE_SECTION("==========================") ;
/* W25Q相关操作的错误码 */
typedef enum tagDRV_W25Q_ERROR_CODE
{
    /* 操作正常 */
    W25Q_ERROR_OK                                 =    0u, /* 操作正常 */

    /* WriteEnable函数错误码 */
    W25Q_ERROR_WRITEENABLE_SPI_TRAMS_ERROR        = 1000u, /* SPI传输异常 */

    /* WaitNoBusy函数错误码 */
    W25Q_ERROR_WAITNOBUSY_SPI_SEND_ERROR          = 1500u, /* SPI发送异常 */
    W25Q_ERROR_WAITNOBUSY_SPI_RECV_ERROR          = 1501u, /* SPI接收异常 */

    /* EraseSector函数错误码 */
    W25Q_ERROR_ERASESECTOR_WRITEENABLE_FAIL       = 2000u, /* 写使能失败 */
    W25Q_ERROR_ERASESECTOR_WAITNOBUSY_FAIL        = 2001u, /* 等待就绪失败 */
    W25Q_ERROR_ERASESECTOR_SPI_SEND_ERROR         = 2002u, /* SPI发送失败 */
    W25Q_ERROR_ERASESECTOR_WAITNOBUSY_FAIL2       = 2003u, /* 等待就绪失败2 */

    /* EraseChip函数错误码 */
    W25Q_ERROR_ERASECHIP_WRITEENABLE_FAIL         = 2500u, /* 写使能失败 */
    W25Q_ERROR_ERASECHIP_WAITNOBUSY_FAIL          = 2501u, /* 等待就绪失败 */
    W25Q_ERROR_ERASECHIP_SPI_SEND_ERROR           = 2502u, /* SPI发送失败 */
    W25Q_ERROR_ERASECHIP_WAITNOBUSY_FAIL2         = 2503u, /* 等待就绪失败2 */

    /* ProgramPage函数错误码 */
    W25Q_ERROR_PROGRAMPAGE_WRITEENABLE_FAIL       = 3000u, /* 写使能失败 */
    W25Q_ERROR_PROGRAMPAGE_WAITNOBUSY_FAIL        = 3001u, /* 等待就绪失败 */
    W25Q_ERROR_PROGRAMPAGE_SPI_SEND_ERROR         = 3002u, /* SPI发送失败 */
    W25Q_ERROR_PROGRAMPAGE_SPI_SEND_ERROR2        = 3003u, /* SPI发送失败2 */
    W25Q_ERROR_PROGRAMPAGE_WAITNOBUSY_FAIL2       = 3004u, /* 等待就绪失败2 */

    /* InternalRead函数错误码 */
    W25Q_ERROR_INTERNALREAD_SPI_SEND_ERROR        = 3500u, /* SPI发送失败 */
    W25Q_ERROR_INTERNALREAD_SPI_RECV_ERROR        = 3501u, /* SPI接收失败 */

    /* Open函数错误码 */
    W25Q_ERROR_OPEN_DEVICE_PTR_NULL               = 4000u, /* 设备指针为空 */
    W25Q_ERROR_OPEN_INITINFO_PTR_NULL             = 4001u, /* 设备初始化信息指针为空 */
    W25Q_ERROR_OPEN_DEVICE_ALREADY_OPEN           = 4002u, /* 设备已经打开 */
    W25Q_ERROR_OPEN_INITINFO_SEND_FUNCTION_NULL   = 4003u, /* 设备初始化信息中的发送函数为空 */
    W25Q_ERROR_OPEN_INITINFO_RECV_FUNCTION_NULL   = 4004u, /* 设备初始化信息中的接收函数为空 */
    W25Q_ERROR_OPEN_INITINFO_SECTOR_BUF_NULL      = 4005u, /* 设备初始化信息中的扇区缓存为空 */
    W25Q_ERROR_OPEN_CREATEDEVSEM_FAIL             = 4006u, /* 创建设备信号量失败 */

    /* Close函数错误码 */
    W25Q_ERROR_CLOSE_DEVICE_PTR_NULL              = 4500u, /* 设备指针为空 */
    W25Q_ERROR_CLOSE_DEVICE_ALREADY_CLOSE         = 4501u, /* 设备已经关闭 */
    W25Q_ERROR_CLOSE_DELETEDEVSEM_FAIL            = 4502u, /* 删除设备信号量失败 */

    /* ReadJEDECID函数错误码 */
    W25Q_ERROR_READJEDECID_SPI_SEND_ERROR         = 5000u, /* 读取JEDEC ID SPI发送错误 */
    W25Q_ERROR_READJEDECID_SPI_RECV_ERROR         = 5001u, /* 读取JEDEC ID SPI接收错误 */
    W25Q_ERROR_READJEDECID_DEVICE_PTR_NULL        = 5002u, /* 设备指针为空 */
    W25Q_ERROR_READJEDECID_JEDECID_PTR_NULL       = 5003u, /* JEDEC ID指针为空 */
    W25Q_ERROR_READJEDECID_DEVICE_NOT_OPENED      = 5004u, /* 设备未打开 */

    /* ReadDevID函数错误码 */
    W25Q_ERROR_READDEVID_DEVICE_PTR_NULL          = 5500u, /* 设备指针为空 */
    W25Q_ERROR_READDEVID_DEVID_PTR_NULL           = 5501u, /* DEV ID指针为空 */
    W25Q_ERROR_READDEVID_DEVICE_NOT_OPENED        = 5502u, /* 设备未打开 */
    W25Q_ERROR_READDEVID_SPI_SEND_ERROR           = 5503u, /* 读取DEV ID SPI发送错误 */
    W25Q_ERROR_READDEVID_SPI_RECV_ERROR           = 5504u, /* 读取DEV ID SPI接收错误 */

    /* PowerDown函数错误码 */
    W25Q_ERROR_POWERDOWN_DEVICE_PTR_NULL          = 6000u, /* 设备指针为空 */
    W25Q_ERROR_POWERDOWN_DEVICE_NOT_OPENED        = 6001u, /* 设备未打开 */
    W25Q_ERROR_POWERDOWN_SPI_SEND_ERROR           = 6002u, /* SPI发送错误 */

    /* ReleasePowerDown函数错误码 */
    W25Q_ERROR_RELEASEPOWERDOWN_DEVICE_PTR_NULL   = 6500u, /* 设备指针为空 */
    W25Q_ERROR_RELEASEPOWERDOWN_DEVICE_NOT_OPENED = 6501u, /* 设备未打开 */
    W25Q_ERROR_RELEASEPOWERDOWN_SPI_SEND_ERROR    = 6502u, /* SPI发送错误 */

    /* Read函数错误码 */
    W25Q_ERROR_READ_DEVICE_PTR_NULL               = 7000u, /* 设备指针为空 */
    W25Q_ERROR_READ_DEVICE_NOT_OPENED             = 7001u, /* 设备未打开 */
    W25Q_ERROR_READ_BUF_PTR_NULL                  = 7002u, /* 缓存指针为空 */
    W25Q_ERROR_READ_INTERNALREAD_ERR              = 7003u, /* 内部读取错误 */
    W25Q_ERROR_READ_SIZE_IS_0                     = 7004u, /* 读取长度为0 */
    W25Q_ERROR_READ_PENDDEVSEM_ERR                = 7005u, /* 获取设备信号量出错 */
    W25Q_ERROR_READ_POSTDEVSEM_ERR                = 7006u, /* 释放器件信号量出错 */

    /* Write函数错误码 */
    W25Q_ERROR_WRITE_DEVICE_PTR_NULL              = 7500u, /* 设备指针为空 */
    W25Q_ERROR_WRITE_DEVICE_NOT_OPENED            = 7501u, /* 设备未打开 */
    W25Q_ERROR_WRITE_BUF_PTR_NULL                 = 7502u, /* 缓存指针为空 */
    W25Q_ERROR_WRITE_PENDDEVSEM_ERR               = 7503u, /* 获取器件信号量出错 */
    W25Q_ERROR_WRITE_INTERNALREAD_ERR             = 7504u, /* 内部读取错误 */
    W25Q_ERROR_WRITE_ERASESECTOR_ERR              = 7505u, /* 扇区擦除错误 */
    W25Q_ERROR_WRITE_PROGRAMPAGE_ERR              = 7506u, /* 页编程错误 */
    W25Q_ERROR_WRITE_POSTDEVSEM_ERR               = 7507u  /* 释放器件信号量出错 */
} DRV_W25Q_ERROR_CODE_E ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块结构数据类型    ==") ;
CODE_SECTION("==========================") ;
/* 定义SPI操作函数指针类型 */
/* 说明：
 * pucBuf ：待发送或接收的数据缓冲区；
 * uiSize ：发送或接收的数据量；
 * uiOpMode ：
 *     ************************************************************************
 *     * BIT  *   7   *   6   *   5   *   4   *   3   *   2   *   1   *   0   *
 *     ************************************************************************
 *     * FLAG *       *       *       *       *       * FAST  * STOP  * START *
 *     ************************************************************************
 *     START : 传输前发送SPI的NSS启动传输条件；
 *     STOP  : 传输后发送SPI的NSS停止传输条件；
 *     FAST  : 使用快速的方式传输大量的数据（例如DMA方式）；
 * 返回值：
 *     0    ：操作正常；
 *     其他 ：操作失败；
 */
#define DRV_W25Q_SPI_OP_MODE_NONE                 0x00000000
#define DRV_W25Q_SPI_OP_MODE_WITH_START           0x00000001 
#define DRV_W25Q_SPI_OP_MODE_WITH_STOP            0x00000002 
#define DRV_W25Q_SPI_OP_MODE_WITH_DMA             0x00000004 

/* SPI读写函数类型 */
typedef UINT (*DRV_W25Q_SPI_Send)(UCHAR *pucBuf, UINT uiSize, UINT uiOpMode) ;
typedef UINT (*DRV_W25Q_SPI_Recv)(UCHAR *pucBuf, UINT uiSize, UINT uiOpMode) ;

/* W25Q初始化消息 */
typedef struct tagDRV_W25Q_INIT_INFO
{
    union
    {
        UCHAR aucRawData[12] ;

        struct
        {
            /* DW0 */
            VOID *pfSPISendFunction ;  /* SPI发送函数指针 */

            /* DW1 */
            VOID *pfSPIRecvFunction ;  /* SPI接收函数指针 */

            /* DW2 */
            VOID *pvSectorBuf ;        /* 扇区存储空间指针 */
        } ;
    } ;
} DRV_W25Q_INIT_INFO_S ;

/* W25Q驱动设备 */
typedef struct tagDRV_W25Q_DEVICE
{
    union
    {
        UCHAR aucRawData[16 + sizeof(OS_SEM)] ;

        struct
        {
            /* DW0 */
            UINT               uiDevOpenFlag ;                 /* 设备打开标记 */

            /* DW1 */
            DRV_W25Q_SPI_Send  pfSPISend ;                     /* SPI发送函数指针 */

            /* DW2 */
            DRV_W25Q_SPI_Recv  pfSPIRecv ;                     /* SPI接收函数指针 */

            /* DW3 */
            VOID              *pvSectorBuf ;                   /* 扇区存储空间指针 */

            /* DW4 */
            OS_SEM             stW25QDeviceLock ;              /* 设备锁 */
        } ;
    } ;
} DRV_W25Q_DEVICE_S ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块函数声明        ==") ;
CODE_SECTION("==========================") ;
DRV_W25Q_ERROR_CODE_E DRV_W25Q_Open(DRV_W25Q_DEVICE_S *pstW25QDevice, DRV_W25Q_INIT_INFO_S *pstW25QInitInfo) ;
DRV_W25Q_ERROR_CODE_E DRV_W25Q_Close(DRV_W25Q_DEVICE_S *pstW25QDevice) ;
DRV_W25Q_ERROR_CODE_E DRV_W25Q_ReadJEDECID(DRV_W25Q_DEVICE_S *pstW25QDevice, UINT *puiJEDECID) ;
DRV_W25Q_ERROR_CODE_E DRV_W25Q_ReadDevID(DRV_W25Q_DEVICE_S *pstW25QDevice, UINT *puiDevID) ;
DRV_W25Q_ERROR_CODE_E DRV_W25Q_PowerDown(DRV_W25Q_DEVICE_S *pstW25QDevice) ;
DRV_W25Q_ERROR_CODE_E DRV_W25Q_ReleasePowerDown(DRV_W25Q_DEVICE_S *pstW25QDevice) ;
DRV_W25Q_ERROR_CODE_E DRV_W25Q_Read(DRV_W25Q_DEVICE_S *pstW25QDevice, UCHAR *pucBuf, UINT uiAddr, UINT uiSize) ;
DRV_W25Q_ERROR_CODE_E DRV_W25Q_Write(DRV_W25Q_DEVICE_S *pstW25QDevice, UCHAR *pucBuf, UINT uiAddr, UINT uiSize) ;


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __DRV_W25Q_H__ */

/******* End of file drv_w25q.h. *******/  
