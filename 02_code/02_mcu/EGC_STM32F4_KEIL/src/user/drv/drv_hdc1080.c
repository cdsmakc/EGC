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

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "pub_type.h"
#include "pub_lib.h"
#include "drv_hdc1080.h"

CODE_SECTION("=======================================") ;
CODE_SECTION("==  移植：修改以下代码以实现I2C访问  ==") ;
CODE_SECTION("=======================================") ;

#include "drv_i2c_stm32f4xx.h"

/* I2C驱动设备 */
extern DRV_I2C_DEVICE_S g_stI2CDevice ;

/*******************************************************************************
- Function    : __DRV_HDC_I2C_Write
- Description : 本函数实现针对HDC1080器件的I2C写入。
- Input       : ucRegAddr ：寄存器地址；
                pucBuf    ：数据指针；
                ucWrSize  ：写入字节数。
- Output      : NULL
- Return      : DRV_HDC_ERROR_CODE_E ：错误码。
                    HDC1080_ERROR_OK          ：写入正常。
                    HDC1080_ERROR_WRITE_ERROR ：写入失败。
- Others      : 
*******************************************************************************/
DRV_HDC_ERROR_CODE_E __DRV_HDC_I2C_Write(UCHAR ucRegAddr, UCHAR *pucBuf, UCHAR ucWrSize)
{
    DRV_I2C_ERROR_CODE_E enI2COpRetVal ;
    
    enI2COpRetVal = DRV_I2C_Write(&g_stI2CDevice, DRV_HDC1080_BUS_ADDR, ucRegAddr, pucBuf, ucWrSize) ;

    if(I2C_ERROR_OK != enI2COpRetVal)
    {
        return HDC1080_ERROR_WRITE_ERROR ;
    }
    else
    {
        return HDC1080_ERROR_OK ;
    }
}


/*******************************************************************************
- Function    : __DRV_HDC_I2C_Read
- Description : 本函数实现针对HDC1080器件的I2C读取。
- Input       : ucRegAddr ：寄存器地址；
                ucWrSize  ：读取字节数。
- Output      : pucBuf    ：数据指针；
- Return      : DRV_HDC_ERROR_CODE_E ：错误码。
                    HDC1080_ERROR_OK         ：读取正常。
                    HDC1080_ERROR_READ_ERROR ：读取失败。
- Others      : 
*******************************************************************************/
DRV_HDC_ERROR_CODE_E __DRV_HDC_I2C_Read(UCHAR ucRegAddr, UCHAR *pucBuf, UCHAR ucRdSize)
{
    DRV_I2C_ERROR_CODE_E enI2COpRetVal ;

    enI2COpRetVal = DRV_I2C_Read(&g_stI2CDevice, DRV_HDC1080_BUS_ADDR, ucRegAddr, pucBuf, ucRdSize, 0) ;

    if(I2C_ERROR_OK != enI2COpRetVal)
    {
        return HDC1080_ERROR_READ_ERROR ;
    }
    else
    {
        return HDC1080_ERROR_OK ;
    }
}


/*******************************************************************************
- Function    : __DRV_HDC_I2C_ReadWithDelay
- Description : 本函数实现针对HDC1080器件的I2C读取，并且在读操作的两相之间插入一
                个延时。
- Input       : ucRegAddr ：寄存器地址；
                ucWrSize  ：读取字节数。
- Output      : pucBuf    ：数据指针；
- Return      : DRV_HDC_ERROR_CODE_E ：错误码。
                    HDC1080_ERROR_OK                  ：读取正常。
                    HDC1080_ERROR_READWITHDELAY_ERROR ：读取失败。
- Others      : 
*******************************************************************************/
DRV_HDC_ERROR_CODE_E __DRV_HDC_I2C_ReadWithDelay(UCHAR ucRegAddr, UCHAR *pucBuf, UCHAR ucRdSize)
{
    DRV_I2C_ERROR_CODE_E enI2COpRetVal ;

    enI2COpRetVal = DRV_I2C_Read(&g_stI2CDevice, DRV_HDC1080_BUS_ADDR, ucRegAddr, pucBuf, ucRdSize, DRV_HDC1080_MEASURE_TIME_MS) ;

    if(I2C_ERROR_OK != enI2COpRetVal)
    {
        return HDC1080_ERROR_READWITHDELAY_ERROR ;
    }
    else
    {
        return HDC1080_ERROR_OK ;
    }
}


CODE_SECTION("=======================================") ;
CODE_SECTION("==  移植：修改结束                   ==") ;
CODE_SECTION("=======================================") ;

CODE_SECTION("=======================================") ;
CODE_SECTION("==  内部函数                         ==") ;
CODE_SECTION("=======================================") ;

/*******************************************************************************
- Function    : DRV_HDC_Init
- Description : 本函数初始化HDC1080的相关控制接口，和芯片初始化配置。
- Input       : NULL
- Output      : NULL
- Return      : DRV_HDC_ERROR_CODE_E ：错误码。
                    HDC1080_ERROR_OK ：读取正常。
                    其他值              ：查看定义。
- Others      : 
*******************************************************************************/
DRV_HDC_ERROR_CODE_E DRV_HDC_Init(VOID)
{
    DRV_HDC_ERROR_CODE_E  enCallRetVal ;
    DRV_HDC1080_CONFIG_S  stHDC1080ConfigInfo ;

    /* 模式配置 */
    stHDC1080ConfigInfo.aucRawData[0] = 0 ;
    stHDC1080ConfigInfo.aucRawData[1] = 0 ;

    stHDC1080ConfigInfo.ucRST         = 1 ; /* 复位 */

    enCallRetVal = __DRV_HDC_I2C_Write(DRV_HDC1080_REG_ADDR_CONFIG, stHDC1080ConfigInfo.aucRawData, sizeof(DRV_HDC1080_CONFIG_S)) ;

    if(HDC1080_ERROR_OK != enCallRetVal)
    {
        return HDC1080_ERROR_INIT_FIRST_I2C_WRITE_FAIL ;
    }

    stHDC1080ConfigInfo.ucRST         = 0 ; /* 取消复位 */
    stHDC1080ConfigInfo.ucMode        = 1 ; /* 顺序测量温湿度 */
    stHDC1080ConfigInfo.ucTRes        = 1 ; /* 温度使用11位测量 */
    stHDC1080ConfigInfo.ucHRes        = 2 ; /* 湿度使用8位测量 */

    enCallRetVal = __DRV_HDC_I2C_Write(DRV_HDC1080_REG_ADDR_CONFIG, stHDC1080ConfigInfo.aucRawData, sizeof(DRV_HDC1080_CONFIG_S)) ;

    if(HDC1080_ERROR_OK != enCallRetVal)
    {
        return HDC1080_ERROR_INIT_SECOND_I2C_WRITE_FAIL ;
    }
    
    return HDC1080_ERROR_OK ;
}

/*******************************************************************************
- Function    : DRV_HDC_GetManufacturerID
- Description : 本函数返回HDC1080的制造商信息。正确的返回值应为0x5449(即字符TI)。
                本函数可用来测试芯片访问是否正常。
- Input       : NULL
- Output      : pusManufacturerID ：制造商ID。正确值为0x5449。
- Return      : DRV_HDC_ERROR_CODE_E ：错误码。
                    HDC1080_ERROR_OK ：读取正常。
                    其他值              ：查看定义。
- Others      : 
*******************************************************************************/
DRV_HDC_ERROR_CODE_E DRV_HDC_GetManufacturerID(USHORT *pusManufacturerID)
{
    DRV_HDC_ERROR_CODE_E  enCallRetVal ;
    UCHAR                 aucReadBuf[2] ;
    USHORT                usTmp ;

    /* 从对应地址读取2字节 */
    enCallRetVal = __DRV_HDC_I2C_Read(DRV_HDC1080_REG_ADDR_MANU_ID, aucReadBuf, sizeof(aucReadBuf)) ;

    if(HDC1080_ERROR_OK != enCallRetVal)
    {
        return HDC1080_ERROR_GETMANUFACTURERID_I2C_READ_FAIL ;
    }

    usTmp              = aucReadBuf[0] ;
    usTmp              = (usTmp << 8) | aucReadBuf[1] ;
    *pusManufacturerID = usTmp ;

    return HDC1080_ERROR_OK ;
}

/*******************************************************************************
- Function    : DRV_HDC_GetDeviceID
- Description : 本函数返回HDC1080的器件标识符信息。正确的返回值应为0x1050。
                本函数可用来测试芯片访问是否正常。
- Input       : NULL
- Output      : pusDeviceID : 器件标识符。正确值为0x1050。
- Return      : DRV_HDC_ERROR_CODE_E ：错误码。
                    HDC1080_ERROR_OK ：读取正常。
                    其他值              ：查看定义。
- Others      : 
*******************************************************************************/
DRV_HDC_ERROR_CODE_E DRV_HDC_GetDeviceID(USHORT *pusDeviceID)
{
    DRV_HDC_ERROR_CODE_E  enCallRetVal ;
    UCHAR                 aucReadBuf[2] ;
    USHORT                usTmp ;

    /* 从对应地址读取2字节 */
    enCallRetVal = __DRV_HDC_I2C_Read(DRV_HDC1080_REG_ADDR_DEV_ID, aucReadBuf, sizeof(aucReadBuf)) ;

    if(HDC1080_ERROR_OK != enCallRetVal)
    {
        return HDC1080_ERROR_GETDEVICEID_I2C_READ_FAIL ;
    }
    
    usTmp        = aucReadBuf[0] ;
    usTmp        = (usTmp << 8) | aucReadBuf[1] ;
    *pusDeviceID = usTmp ;

    return HDC1080_ERROR_OK ;
}

/*******************************************************************************
- Function    : DRV_HDC_Measure*
- Description : 本函数读取一次HDC1080的测量值，然后计算温度值和湿度值。
- Input       : NULL
- Output      : pfTemp : 指向保存温度值变量的指针。
                pfHumi : 指向保存湿度值变量的指针。
- Return      : DRV_HDC_ERROR_CODE_E ：错误码。
                    HDC1080_ERROR_OK ：读取正常。
                    其他值              ：查看定义
- Others      : 
*******************************************************************************/
DRV_HDC_ERROR_CODE_E DRV_HDC_Measure(FLOAT *pfTemp, FLOAT *pfHumi)
{
    DRV_HDC_ERROR_CODE_E  enCallRetVal ;
    UCHAR                 aucI2CReadBuf[4] ;
    USHORT                usTempRaw ;
    USHORT                usHumiRaw ;

    /* 读取测量值 */
    enCallRetVal = __DRV_HDC_I2C_ReadWithDelay(DRV_HDC1080_REG_ADDR_TEMP_RES, aucI2CReadBuf, sizeof(aucI2CReadBuf)) ;

    if(HDC1080_ERROR_OK != enCallRetVal)
    {
        return HDC1080_ERROR_MEASURE_I2C_READ_FAIL ;
    }

    /* 整理原始值 */
    usTempRaw = aucI2CReadBuf[0] ;
    usTempRaw = (usTempRaw << 8) | aucI2CReadBuf[1] ;
    usHumiRaw = aucI2CReadBuf[2] ;
    usHumiRaw = (usHumiRaw << 8) | aucI2CReadBuf[3] ;

    /* 计算温度和湿度 */
    *pfTemp = (usTempRaw / 65536.0) * 165.0 - 40.0;
    *pfHumi = (usHumiRaw / 65536.0) * 100.0 ;

    return HDC1080_ERROR_OK ;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file drv_hdc1080.c. *******/  
