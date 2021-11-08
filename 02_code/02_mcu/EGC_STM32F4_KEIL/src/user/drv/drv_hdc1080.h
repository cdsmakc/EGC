/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : drv_hdc1080.c
    Project name : 
    Module name  : 
    Date created : 2021年9月15日   8时54分44秒
    Author       : Ning.JianLi
    Description  : 提供HDC1080温湿度传感器的接口文件。
*******************************************************************************/

#ifndef __DRV_HDC1080_H__
#define __DRV_HDC1080_H__

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */


#pragma anon_unions

#include "pub_type.h"
#include "pub_lib.h"

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块内部宏定义      ==") ;
CODE_SECTION("==========================") ;

 /* HDC1080总线地址定义 */
#define DRV_HDC1080_BUS_ADDR                0x40

/* HDC1080寄存器地址定义 */
#define DRV_HDC1080_REG_ADDR_TEMP_RES       0x00
#define DRV_HDC1080_REG_ADDR_HUMI_RES       0x01
#define DRV_HDC1080_REG_ADDR_CONFIG         0x02
#define DRV_HDC1080_REG_ADDR_MANU_ID        0xFE
#define DRV_HDC1080_REG_ADDR_DEV_ID         0xFF

/* 正确的Manufacturer ID和Device ID定义 */
#define DRV_HDC1080_MAN_ID                  0x5449
#define DRV_HDC1080_DEV_ID                  0x1050

/* 从触发测量到读取结果的时间间隔 */
#define DRV_HDC1080_MEASURE_TIME_MS         (800u)

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块枚举数据类型    ==") ;
CODE_SECTION("==========================") ;

/* 以下枚举类型定义了驱动函数的错误码 */
typedef enum tagDRV_HDC_ERROR_CODE
{
    /* 操作正常 */
    HDC1080_ERROR_OK                               =    0u, /* 操作正常 */

    /* Write函数错误码 */
    HDC1080_ERROR_WRITE_ERROR                      = 2000u, /* 写入失败 */

    /* Read函数错误码 */
    HDC1080_ERROR_READ_ERROR                       = 3000u, /* 读取失败 */

    /* ReadWithDelay函数错误码 */
    HDC1080_ERROR_READWITHDELAY_ERROR              = 4000u, /* 延时读取失败 */

    /* Init函数错误码 */
    HDC1080_ERROR_INIT_FIRST_I2C_WRITE_FAIL        = 5000u, /* 第一次I2C写入失败 */
    HDC1080_ERROR_INIT_SECOND_I2C_WRITE_FAIL       = 5001u, /* 第二次I2C写入失败 */

    /* GetManufacturerID函数错误码 */
    HDC1080_ERROR_GETMANUFACTURERID_I2C_READ_FAIL  = 6000u, /* I2C读取失败 */

    /* GetDeviceID函数错误码 */
    HDC1080_ERROR_GETDEVICEID_I2C_READ_FAIL        = 7000u, /* I2C读取失败 */

    /* Measure函数错误码 */
    HDC1080_ERROR_MEASURE_I2C_READ_FAIL            = 8000u  /* I2C读取失败 */
} DRV_HDC_ERROR_CODE_E ;


CODE_SECTION("==========================") ;
CODE_SECTION("==  模块结构数据类型    ==") ;
CODE_SECTION("==========================") ;

/* HDC1080配置寄存器定义 */
#pragma pack(1)   
typedef struct tagDRV_HDC1080_CONFIG
{
    union
    {
        UCHAR aucRawData[2] ;

        struct
        {
            /* BYTE0-HIGH */
            UCHAR ucHRes                : 2 ;
            UCHAR ucTRes                : 1 ;
            UCHAR ucBTST                : 1 ;
            UCHAR ucMode                : 1 ;
            UCHAR ucHeat                : 1 ;
            UCHAR ucRSV0                : 1 ;
            UCHAR ucRST                 : 1 ;

            /* BYTE1-LOW */
            UCHAR ucRSV1 ;
        } ;
    } ;
} DRV_HDC1080_CONFIG_S ;
#pragma pack()   


CODE_SECTION("==========================") ;
CODE_SECTION("==  模块函数声明        ==") ;
CODE_SECTION("==========================") ;

DRV_HDC_ERROR_CODE_E DRV_HDC_Init(VOID) ;
DRV_HDC_ERROR_CODE_E DRV_HDC_GetManufacturerID(USHORT *pusManufacturerID) ;
DRV_HDC_ERROR_CODE_E DRV_HDC_GetDeviceID(USHORT *pusDeviceID) ;
DRV_HDC_ERROR_CODE_E DRV_HDC_Measure(FLOAT *pfTemp, FLOAT *pfHumi) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __DRV_HDC1080_H__ */

/******* End of file drv_hdc1080.h. *******/  
