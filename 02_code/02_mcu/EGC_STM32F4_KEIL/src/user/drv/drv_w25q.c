/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : drv_w25q.c
    Project name : 
    Module name  : 
    Date created : 2021年9月18日   9时59分31秒
    Author       : Ning.JianLi
    Description  : W25Q SPI Flash器件驱动程序。
*******************************************************************************/

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include <string.h>

#include "pub_type.h"
#include "pub_lib.h"

#include "drv_w25q.h"

/*******************************************************************************
- Function    : __DRV_W25Q_WriteEnable
- Description : 本函数通过设置状态寄存器中的WEL位，来使得器件可以进行擦除/编程等
                操作。
- Input       : pstW25QDevice :
                    W25Q设备指针。
- Output      : NULL 
- Return      : DRV_W25Q_ERROR_CODE_E ：错误码。
                    W25Q_ERROR_OK ：操作正常。
                    其他            ：查看具体定义。
- Others      :
*******************************************************************************/
DRV_W25Q_ERROR_CODE_E __DRV_W25Q_WriteEnable(DRV_W25Q_DEVICE_S *pstW25QDevice)
{
    UCHAR ucTmp ;
    UINT  uiFuncCallRetVal ;

    ucTmp = DRV_W25Q_INS_CODE_WRITE_ENABLE ;
    
    uiFuncCallRetVal = pstW25QDevice->pfSPISend(&ucTmp, sizeof(ucTmp), DRV_W25Q_SPI_OP_MODE_WITH_START | DRV_W25Q_SPI_OP_MODE_WITH_STOP) ;

    if(0 == uiFuncCallRetVal)
    {
        return W25Q_ERROR_OK ;
    }
    else
    {
        return W25Q_ERROR_WRITEENABLE_SPI_TRAMS_ERROR ;
    }
}

/*******************************************************************************
- Function    : __DRV_W25Q_WaitNoBusy
- Description : 本函数读循环取状态寄存器中的BUSY位，直到此位为0，表示器件处于就
                绪状态。
- Input       : pstW25QDevice :
                    W25Q设备指针。
- Output      : NULL 
- Return      : DRV_W25Q_ERROR_CODE_E ：错误码。
                    W25Q_ERROR_OK ：操作正常。
                    其他            ：查看具体定义。
- Others      :
*******************************************************************************/
DRV_W25Q_ERROR_CODE_E __DRV_W25Q_WaitNoBusy(DRV_W25Q_DEVICE_S *pstW25QDevice)
{
    UCHAR  ucTmp ;
    UINT   uiFuncCallRetVal ;

    while(1)
    {
        ucTmp = DRV_W25Q_INS_CODE_READ_STATUS_REG1 ;

        uiFuncCallRetVal = pstW25QDevice->pfSPISend(&ucTmp, sizeof(ucTmp), DRV_W25Q_SPI_OP_MODE_WITH_START) ;

        if(0 != uiFuncCallRetVal)
        {
            return W25Q_ERROR_WAITNOBUSY_SPI_SEND_ERROR ;
        }

        ucTmp = 0 ;

        uiFuncCallRetVal = pstW25QDevice->pfSPIRecv(&ucTmp, sizeof(ucTmp), DRV_W25Q_SPI_OP_MODE_WITH_STOP) ;

        if(0 != uiFuncCallRetVal)
        {
            return W25Q_ERROR_WAITNOBUSY_SPI_RECV_ERROR ;
        }

        if(0x00 == (ucTmp & 0x01))
        {
            /* BUSY撤销，循环可退出 */
            break ; 
        }
    }

    return W25Q_ERROR_OK ;
}

/*******************************************************************************
- Function    : __DRV_W25Q_EraseSector
- Description : 本函数擦除一个指定的扇区。函数将会等待，直到擦除完成。
- Input       : pstW25QDevice :
                    W25Q设备指针。
                uiSectorAddr : 
                    指定扇区的地址。
- Output      : NULL 
- Return      : DRV_W25Q_ERROR_CODE_E ：错误码。
                    W25Q_ERROR_OK ：操作正常。
                    其他            ：查看具体定义。
- Others      :
*******************************************************************************/
DRV_W25Q_ERROR_CODE_E __DRV_W25Q_EraseSector(DRV_W25Q_DEVICE_S *pstW25QDevice, UINT uiSectorAddr)
{
    UCHAR                 aucTmp[4] ;    
    DRV_W25Q_ERROR_CODE_E enFuncCallRetVal ;
    UINT                  uiFuncCallRetVal ;

    /* 写使能并等待非繁忙状态 */
    enFuncCallRetVal = __DRV_W25Q_WriteEnable(pstW25QDevice) ;

    if(W25Q_ERROR_OK != enFuncCallRetVal)
    {
        return W25Q_ERROR_ERASESECTOR_WRITEENABLE_FAIL ;
    }
    
    enFuncCallRetVal = __DRV_W25Q_WaitNoBusy(pstW25QDevice) ;

    if(W25Q_ERROR_OK != enFuncCallRetVal)
    {
        return W25Q_ERROR_ERASESECTOR_WAITNOBUSY_FAIL ;
    }

    /* 发送扇区擦出指令 */
    aucTmp[0] = DRV_W25Q_INS_CODE_SECTOR_ERASE ;
    aucTmp[1] = (UCHAR)(uiSectorAddr >> 16) ;
    aucTmp[2] = (UCHAR)(uiSectorAddr >> 8) ;
    aucTmp[3] = (UCHAR)(uiSectorAddr >> 0) ;

    uiFuncCallRetVal = pstW25QDevice->pfSPISend(aucTmp, sizeof(aucTmp), DRV_W25Q_SPI_OP_MODE_WITH_START | DRV_W25Q_SPI_OP_MODE_WITH_STOP) ;

    if(0 != uiFuncCallRetVal)
    {
        return W25Q_ERROR_ERASESECTOR_SPI_SEND_ERROR ;
    }

    enFuncCallRetVal = __DRV_W25Q_WaitNoBusy(pstW25QDevice) ;

    if(W25Q_ERROR_OK != enFuncCallRetVal)
    {
        return W25Q_ERROR_ERASESECTOR_WAITNOBUSY_FAIL2 ;
    }

    return W25Q_ERROR_OK ;
}

/*******************************************************************************
- Function    : __DRV_W25Q_EraseChip
- Description : 本函数擦除整个W25Q芯片。函数将会等待，直到擦除完成。
- Input       : pstW25QDevice :
                    W25Q设备指针。
- Output      : NULL 
- Return      : DRV_W25Q_ERROR_CODE_E ：错误码。
                    W25Q_ERROR_OK ：操作正常。
                    其他            ：查看具体定义。
- Others      :
*******************************************************************************/
DRV_W25Q_ERROR_CODE_E __DRV_W25Q_EraseChip(DRV_W25Q_DEVICE_S *pstW25QDevice)
{
    UCHAR                 ucTmp ;    
    DRV_W25Q_ERROR_CODE_E enFuncCallRetVal ;
    UINT                  uiFuncCallRetVal ;

    /* 写使能并等待非繁忙状态 */
    enFuncCallRetVal = __DRV_W25Q_WriteEnable(pstW25QDevice) ;

    if(W25Q_ERROR_OK != enFuncCallRetVal)
    {
        return W25Q_ERROR_ERASECHIP_WRITEENABLE_FAIL ;
    }
    
    enFuncCallRetVal = __DRV_W25Q_WaitNoBusy(pstW25QDevice) ;

    if(W25Q_ERROR_OK != enFuncCallRetVal)
    {
        return W25Q_ERROR_ERASECHIP_WAITNOBUSY_FAIL ;
    }

    /* 发送指令 */
    ucTmp = DRV_W25Q_INS_CODE_CHIP_ERASE ;

    uiFuncCallRetVal = pstW25QDevice->pfSPISend(&ucTmp, sizeof(ucTmp), DRV_W25Q_SPI_OP_MODE_WITH_START | DRV_W25Q_SPI_OP_MODE_WITH_STOP) ;

    if(0 != uiFuncCallRetVal)
    {
        return W25Q_ERROR_ERASECHIP_SPI_SEND_ERROR ;
    }

    enFuncCallRetVal = __DRV_W25Q_WaitNoBusy(pstW25QDevice) ;

    if(W25Q_ERROR_OK != enFuncCallRetVal)
    {
        return W25Q_ERROR_ERASECHIP_WAITNOBUSY_FAIL2 ;
    }

    return W25Q_ERROR_OK ;
}


/*******************************************************************************
- Function    : __DRV_W25Q_ProgramPage
- Description : 本函数对特定地址的页进行编程。函数将会等待，直到编程完成。
- Input       : pstW25QDevice :
                    W25Q设备指针。
                pucBuf  : 
                    指向待编程的数据缓存。
                uiAddr  : 
                    需编程的Flash存储区地址。
                uiSize  : 
                    指示编程大小。必须大于1，小于等于256
- Output      : NULL 
- Return      : DRV_W25Q_ERROR_CODE_E ：错误码。
                    W25Q_ERROR_OK ：操作正常。
                    其他            ：查看具体定义。
- Others      :
*******************************************************************************/
DRV_W25Q_ERROR_CODE_E __DRV_W25Q_ProgramPage(DRV_W25Q_DEVICE_S *pstW25QDevice, UCHAR *pucBuf, UINT uiAddr, UINT uiSize)
{
    UCHAR                 aucTmp[4] ;    
    DRV_W25Q_ERROR_CODE_E enFuncCallRetVal ;
    UINT                  uiFuncCallRetVal ;

    /* 写使能并等待非繁忙状态 */
    enFuncCallRetVal = __DRV_W25Q_WriteEnable(pstW25QDevice) ;

    if(W25Q_ERROR_OK != enFuncCallRetVal)
    {
        return W25Q_ERROR_PROGRAMPAGE_WRITEENABLE_FAIL ;
    }
    
    enFuncCallRetVal = __DRV_W25Q_WaitNoBusy(pstW25QDevice) ;

    if(W25Q_ERROR_OK != enFuncCallRetVal)
    {
        return W25Q_ERROR_PROGRAMPAGE_WAITNOBUSY_FAIL ;
    }

    /* 准备指令及地址 */
    aucTmp[0] = DRV_W25Q_INS_CODE_PAGE_PROGRAM ;
    aucTmp[1] = (UCHAR)(uiAddr >> 16) ;
    aucTmp[2] = (UCHAR)(uiAddr >> 8) ;
    aucTmp[3] = (UCHAR)(uiAddr >> 0) ;

    uiFuncCallRetVal = pstW25QDevice->pfSPISend(aucTmp, sizeof(aucTmp), DRV_W25Q_SPI_OP_MODE_WITH_START) ;

    if(0 != uiFuncCallRetVal)
    {
        return W25Q_ERROR_PROGRAMPAGE_SPI_SEND_ERROR ;
    }

    /* 发送数据 */
    uiFuncCallRetVal = pstW25QDevice->pfSPISend(pucBuf, (uiSize > 256)? 256 : uiSize, DRV_W25Q_SPI_OP_MODE_WITH_STOP | DRV_W25Q_SPI_OP_MODE_WITH_DMA) ;

    if(0 != uiFuncCallRetVal)
    {
        return W25Q_ERROR_PROGRAMPAGE_SPI_SEND_ERROR2 ;
    }
    
    enFuncCallRetVal = __DRV_W25Q_WaitNoBusy(pstW25QDevice) ;

    if(W25Q_ERROR_OK != enFuncCallRetVal)
    {
        return W25Q_ERROR_PROGRAMPAGE_WAITNOBUSY_FAIL2 ;
    }

    return W25Q_ERROR_OK ;
}

/*******************************************************************************
- Function    : __DRV_W25Q_InternalRead
- Description : 本函数执行W25Q Flash器件的读取操作。
- Input       : pstW25QDevice :
                    W25Q设备指针。
                pucBuf  : 
                    指向用来保存读取数据的缓存。
                uiAddr  : 
                    从存储器中读取数据的起始地址。
                uiSize  : 
                    读取的字节数。必须大于1
- Output      : NULL 
- Return      : DRV_W25Q_ERROR_CODE_E ：错误码。
                    W25Q_ERROR_OK ：操作正常。
                    其他            ：查看具体定义。
- Others      :
*******************************************************************************/
DRV_W25Q_ERROR_CODE_E __DRV_W25Q_InternalRead(DRV_W25Q_DEVICE_S *pstW25QDevice, UCHAR *pucBuf, UINT uiAddr, UINT uiSize)
{
    UINT                  uiDMATransSize ;
    UCHAR                 aucTmp[4] ;    
    UINT                  uiFuncCallRetVal ;

    /* 准备指令及地址 */
    aucTmp[0] = DRV_W25Q_INS_CODE_READ_DATA ;
    aucTmp[1] = (UCHAR)(uiAddr >> 16) ;
    aucTmp[2] = (UCHAR)(uiAddr >> 8) ;
    aucTmp[3] = (UCHAR)(uiAddr >> 0) ;

    uiFuncCallRetVal = pstW25QDevice->pfSPISend(aucTmp, sizeof(aucTmp), DRV_W25Q_SPI_OP_MODE_WITH_START) ;

    if(0 != uiFuncCallRetVal)
    {
        return W25Q_ERROR_INTERNALREAD_SPI_SEND_ERROR ;
    }

    /* 因为DMA存在65535的数据量上限，因此数据量较大时需要分多次传输 */
    uiDMATransSize = (uiSize > 64000)? 64000 : uiSize ;

    /* DMA传输有上限，这里每次传输不超过64000个字节 */
    while(0 != uiSize)
    {
        uiDMATransSize = (uiSize > 64000)? 64000 : uiSize ;

        if(uiDMATransSize != uiSize)
        {
            /* 需要多次传输，不发送SPI结束 */
            uiFuncCallRetVal = pstW25QDevice->pfSPIRecv(pucBuf, uiDMATransSize, DRV_W25Q_SPI_OP_MODE_WITH_DMA) ;
        }
        else
        {
            /* 需要单次传输，发送SPI结束 */
            uiFuncCallRetVal = pstW25QDevice->pfSPIRecv(pucBuf, uiDMATransSize, DRV_W25Q_SPI_OP_MODE_WITH_STOP | DRV_W25Q_SPI_OP_MODE_WITH_DMA) ;
        }

        /* 剩余数据计算 */
        pucBuf += uiDMATransSize ;
        uiSize -= uiDMATransSize ;

        if(0 != uiFuncCallRetVal)
        {
            return W25Q_ERROR_INTERNALREAD_SPI_RECV_ERROR ;
        }
    }

    return W25Q_ERROR_OK ;
}

/*******************************************************************************
- Function    : DRV_W25Q_Open
- Description : 本函数创建一个W25Q控制设备。
- Input       : pstW25QInitInfo ：
                    W25Q控制设备初始化信息。
- Output      : pstW25QDevice :
                    W25Q设备指针。 
- Return      : DRV_W25Q_ERROR_CODE_E ：错误码。
                    W25Q_ERROR_OK ：操作正常。
                    其他            ：查看具体定义。
- Others      :
*******************************************************************************/
DRV_W25Q_ERROR_CODE_E DRV_W25Q_Open(DRV_W25Q_DEVICE_S *pstW25QDevice, DRV_W25Q_INIT_INFO_S *pstW25QInitInfo) 
{
    OS_ERR enErr ;
    
    /* 参数检查 */
    if(NULL == pstW25QDevice)
    {
        return W25Q_ERROR_OPEN_DEVICE_PTR_NULL ;
    }

    if(NULL == pstW25QInitInfo)
    {
        return W25Q_ERROR_OPEN_INITINFO_PTR_NULL ;
    }

    if(DRV_W25Q_DEVICE_OPEN_FLAG == pstW25QDevice->uiDevOpenFlag)
    {
        return W25Q_ERROR_OPEN_DEVICE_ALREADY_OPEN ;
    }

    if(NULL == pstW25QInitInfo->pfSPISendFunction)
    {
        return W25Q_ERROR_OPEN_INITINFO_SEND_FUNCTION_NULL ;
    }

    if(NULL == pstW25QInitInfo->pfSPIRecvFunction)
    {
        return W25Q_ERROR_OPEN_INITINFO_RECV_FUNCTION_NULL ;
    }

    if(NULL == pstW25QInitInfo->pvSectorBuf)
    {
        return W25Q_ERROR_OPEN_INITINFO_SECTOR_BUF_NULL ;
    }

    /* 创建W25Q访问锁 */
    OSSemCreate((OS_SEM      *)&(pstW25QDevice->stW25QDeviceLock), 
                (CPU_CHAR    *)"Semaphore-W25Q", 
                (OS_SEM_CTR   )1, 
                (OS_ERR      *)&enErr) ;

    if(OS_ERR_NONE != enErr)
    {
        return W25Q_ERROR_OPEN_CREATEDEVSEM_FAIL ;
    }

    pstW25QDevice->pfSPISend     = (DRV_W25Q_SPI_Send)(pstW25QInitInfo->pfSPISendFunction) ;
    pstW25QDevice->pfSPIRecv     = (DRV_W25Q_SPI_Recv)(pstW25QInitInfo->pfSPIRecvFunction) ;
    pstW25QDevice->pvSectorBuf   = pstW25QInitInfo->pvSectorBuf ;
    pstW25QDevice->uiDevOpenFlag = DRV_W25Q_DEVICE_OPEN_FLAG ;

    return W25Q_ERROR_OK ;
}

/*******************************************************************************
- Function    : DRV_W25Q_Close
- Description : 本函数关闭一个已经创建的W25Q控制设备。
- Input       : pstW25QDevice :
                    W25Q设备指针。
- Output      : NULL
- Return      : DRV_W25Q_ERROR_CODE_E ：错误码。
                    W25Q_ERROR_OK ：操作正常。
                    其他            ：查看具体定义。
- Others      :
*******************************************************************************/
DRV_W25Q_ERROR_CODE_E DRV_W25Q_Close(DRV_W25Q_DEVICE_S *pstW25QDevice)
{
    OS_ERR enErr ;
    
    /* 参数检查 */
    if(NULL == pstW25QDevice)
    {
        return W25Q_ERROR_CLOSE_DEVICE_PTR_NULL ;
    }

    if(DRV_W25Q_DEVICE_OPEN_FLAG != pstW25QDevice->uiDevOpenFlag)
    {
        return W25Q_ERROR_CLOSE_DEVICE_ALREADY_CLOSE ;
    }

    /* 删除W25Q访问锁 */
    OSSemDel((OS_SEM  *)&(pstW25QDevice->stW25QDeviceLock), 
             (OS_OPT   )OS_OPT_DEL_ALWAYS, 
             (OS_ERR  *)&enErr) ;

    if(OS_ERR_NONE != enErr)
    {
        return W25Q_ERROR_CLOSE_DELETEDEVSEM_FAIL ;
    }             
    
    pstW25QDevice->uiDevOpenFlag = ~(DRV_W25Q_DEVICE_OPEN_FLAG) ;

    return W25Q_ERROR_OK ;
}

/*******************************************************************************
- Function    : DRV_W25Q_ReadJEDECID
- Description : 本函数获取W25Q存储器的JEDEC ID。JEDEC ID包含三个部分，分别是制造
                商ID，存储器类型，容量信息等。这三个信息通过一个UINT类型的值返回，
                最低字节为容量信息，次低字节为存储器类型信息，次高字节为制造商ID。
- Input       : pstW25QDevice :
                    W25Q设备指针。
- Output      : puiJEDECID :
                    JEDEC ID指针。
- Return      : DRV_W25Q_ERROR_CODE_E ：错误码。
                    W25Q_ERROR_OK ：操作正常。
                    其他            ：查看具体定义。
- Others      :
*******************************************************************************/
DRV_W25Q_ERROR_CODE_E DRV_W25Q_ReadJEDECID(DRV_W25Q_DEVICE_S *pstW25QDevice, UINT *puiJEDECID)
{
    UCHAR aucTmp[3] ;    
    UINT  uiFuncCallRetVal ;
    UINT  uiManuID ;
    UINT  uiMemTypeID ;
    UINT  uiCapID ;

    /* 参数检查 */
    if(NULL == pstW25QDevice)
    {
        return W25Q_ERROR_READJEDECID_DEVICE_PTR_NULL ;
    }

    if(NULL ==puiJEDECID)
    {
        return W25Q_ERROR_READJEDECID_JEDECID_PTR_NULL ;
    }

    if(DRV_W25Q_DEVICE_OPEN_FLAG != pstW25QDevice->uiDevOpenFlag)
    {
        return W25Q_ERROR_READJEDECID_DEVICE_NOT_OPENED ;
    }

    /* 设定命令 */
    aucTmp[0] = DRV_W25Q_INS_CODE_READ_JEDEC_ID ;

    uiFuncCallRetVal = pstW25QDevice->pfSPISend(aucTmp, sizeof(aucTmp[0]), DRV_W25Q_SPI_OP_MODE_WITH_START) ;

    if(0 != uiFuncCallRetVal)
    {
        return W25Q_ERROR_READJEDECID_SPI_SEND_ERROR ;
    }

    uiFuncCallRetVal = pstW25QDevice->pfSPIRecv(aucTmp, sizeof(aucTmp), DRV_W25Q_SPI_OP_MODE_WITH_STOP) ;

    if(0 != uiFuncCallRetVal)
    {
        return W25Q_ERROR_READJEDECID_SPI_RECV_ERROR ;
    }

    uiManuID    = aucTmp[0] ;
    uiMemTypeID = aucTmp[1] ;
    uiCapID     = aucTmp[2] ;

    *puiJEDECID = ((uiManuID << 16) | (uiMemTypeID << 8) | uiCapID) ;

    return W25Q_ERROR_OK ;
}

/*******************************************************************************
- Function    : DRV_W25Q_ReadDevID
- Description : 本函数获取W25Q存储器的DEVICE ID。DEVICE ID的长度是1个字节，在返
                回值的最低字节。
- Input       : pstW25QDevice :
                    W25Q设备指针。
- Output      : puiDevID :
                    DEV ID指针。
- Return      : DRV_W25Q_ERROR_CODE_E ：错误码。
                    W25Q_ERROR_OK ：操作正常。
                    其他            ：查看具体定义。
- Others      :
*******************************************************************************/
DRV_W25Q_ERROR_CODE_E DRV_W25Q_ReadDevID(DRV_W25Q_DEVICE_S *pstW25QDevice, UINT *puiDevID)
{
    UCHAR aucTmp[4] ;    
    UINT  uiFuncCallRetVal ;

    /* 参数检查 */
    if(NULL == pstW25QDevice)
    {
        return W25Q_ERROR_READDEVID_DEVICE_PTR_NULL ;
    }

    if(NULL ==puiDevID)
    {
        return W25Q_ERROR_READDEVID_DEVID_PTR_NULL ;
    }

    if(DRV_W25Q_DEVICE_OPEN_FLAG != pstW25QDevice->uiDevOpenFlag)
    {
        return W25Q_ERROR_READDEVID_DEVICE_NOT_OPENED ;
    }

    /* 设定命令 */
    aucTmp[0] = DRV_W25Q_INS_CODE_RELEASE_POWER_DOWN_DEV_ID ;

    uiFuncCallRetVal = pstW25QDevice->pfSPISend(aucTmp, sizeof(aucTmp[0]), DRV_W25Q_SPI_OP_MODE_WITH_START) ;

    if(0 != uiFuncCallRetVal)
    {
        return W25Q_ERROR_READDEVID_SPI_SEND_ERROR ;
    }

    uiFuncCallRetVal = pstW25QDevice->pfSPIRecv(aucTmp, sizeof(aucTmp), DRV_W25Q_SPI_OP_MODE_WITH_STOP) ;

    if(0 != uiFuncCallRetVal)
    {
        return W25Q_ERROR_READDEVID_SPI_RECV_ERROR ;
    }

    *puiDevID = (UINT)aucTmp[3] ;
    
    return W25Q_ERROR_OK ;
}

/*******************************************************************************
- Function    : DRV_W25Q_PowerDown
- Description : 本函数将器件置于Power Down模式下。在该模式下，器件消耗非常低的
                电流，并且仅接受Release Power Down指令。
- Input       : pstW25QDevice :
                    W25Q设备指针。
- Output      : NULL 
- Return      : DRV_W25Q_ERROR_CODE_E ：错误码。
                    W25Q_ERROR_OK ：操作正常。
                    其他            ：查看具体定义。
- Others      : 注意，从Power Down模式释放，需要消耗tRES1时间才能接受新的指令。
                该时间对于W25Q128FV而言是3us。
*******************************************************************************/
DRV_W25Q_ERROR_CODE_E DRV_W25Q_PowerDown(DRV_W25Q_DEVICE_S *pstW25QDevice)
{
    UCHAR ucTmp ;
    UINT  uiFuncCallRetVal ;

    /* 参数检查 */
    if(NULL == pstW25QDevice)
    {
        return W25Q_ERROR_POWERDOWN_DEVICE_PTR_NULL ;
    }

    if(DRV_W25Q_DEVICE_OPEN_FLAG != pstW25QDevice->uiDevOpenFlag)
    {
        return W25Q_ERROR_POWERDOWN_DEVICE_NOT_OPENED ;
    }

    /* 设定命令 */
    ucTmp = DRV_W25Q_INS_CODE_POWER_DOWN ;

    uiFuncCallRetVal = pstW25QDevice->pfSPISend(&ucTmp, sizeof(ucTmp), DRV_W25Q_SPI_OP_MODE_WITH_START | DRV_W25Q_SPI_OP_MODE_WITH_STOP) ;

    if(0 != uiFuncCallRetVal)
    {
        return W25Q_ERROR_POWERDOWN_SPI_SEND_ERROR ;
    }

    return W25Q_ERROR_OK ;
}

/*******************************************************************************
- Function    : DRV_W25Q_ReleasePowerDown
- Description : 本函数将器件从Power Down模式下唤醒，从而使得器件进入正常工作模式。
- Input       : pstW25QDevice :
                    W25Q设备指针。
- Output      : NULL 
- Return      : DRV_W25Q_ERROR_CODE_E ：错误码。
                    W25Q_ERROR_OK ：操作正常。
                    其他            ：查看具体定义。
- Others      : 注意，从Power Down模式释放，需要消耗tRES1时间才能接受新的指令。
                该时间对于W25Q128FV而言是3us。
*******************************************************************************/
DRV_W25Q_ERROR_CODE_E DRV_W25Q_ReleasePowerDown(DRV_W25Q_DEVICE_S *pstW25QDevice)
{
    UCHAR ucTmp ;
    UINT  uiFuncCallRetVal ;

    /* 参数检查 */
    if(NULL == pstW25QDevice)
    {
        return W25Q_ERROR_RELEASEPOWERDOWN_DEVICE_PTR_NULL ;
    }

    if(DRV_W25Q_DEVICE_OPEN_FLAG != pstW25QDevice->uiDevOpenFlag)
    {
        return W25Q_ERROR_RELEASEPOWERDOWN_DEVICE_NOT_OPENED ;
    }

    /* 设定命令 */
    ucTmp = DRV_W25Q_INS_CODE_RELEASE_POWER_DOWN_DEV_ID ;

    uiFuncCallRetVal = pstW25QDevice->pfSPISend(&ucTmp, sizeof(ucTmp), DRV_W25Q_SPI_OP_MODE_WITH_START | DRV_W25Q_SPI_OP_MODE_WITH_STOP) ;

    if(0 != uiFuncCallRetVal)
    {
        return W25Q_ERROR_RELEASEPOWERDOWN_SPI_SEND_ERROR ;
    }

    return W25Q_ERROR_OK ;
}


/*******************************************************************************
- Function    : DRV_W25Q_Read
- Description : 本函数执行W25Q Flash器件的读取操作。
- Input       : pstW25QDevice :
                    W25Q设备指针。
                pucBuf  : 
                    指向用来保存读取数据的缓存。
                uiAddr  : 
                    从存储器中读取数据的起始地址。
                uiSize  : 
                    读取的字节数。必须大于1。
- Output      : NULL 
- Return      : DRV_W25Q_ERROR_CODE_E ：错误码。
                    W25Q_ERROR_OK ：操作正常。
                    其他            ：查看具体定义。
- Others      :
*******************************************************************************/
DRV_W25Q_ERROR_CODE_E DRV_W25Q_Read(DRV_W25Q_DEVICE_S *pstW25QDevice, UCHAR *pucBuf, UINT uiAddr, UINT uiSize)
{
    OS_ERR                enErr ;
    DRV_W25Q_ERROR_CODE_E enFuncCallRetVal ;

    if(NULL == pstW25QDevice)
    {
        return W25Q_ERROR_READ_DEVICE_PTR_NULL ;
    }

    if(DRV_W25Q_DEVICE_OPEN_FLAG != pstW25QDevice->uiDevOpenFlag)
    {
        return W25Q_ERROR_READ_DEVICE_NOT_OPENED ;
    }

    if(NULL == pucBuf)
    {
        return W25Q_ERROR_READ_BUF_PTR_NULL ;
    }

    if(0 == uiSize)
    {
        return W25Q_ERROR_READ_SIZE_IS_0 ;
    }

    /* W25Q加锁 */
    OSSemPend((OS_SEM   *)&(pstW25QDevice->stW25QDeviceLock), 
              (OS_TICK   )0, 
              (OS_OPT    )OS_OPT_PEND_BLOCKING, 
              (CPU_TS   *)NULL, 
              (OS_ERR   *)&enErr) ;  

    if(OS_ERR_NONE != enErr)
    {
        return W25Q_ERROR_READ_PENDDEVSEM_ERR ;
    }

    enFuncCallRetVal = __DRV_W25Q_InternalRead(pstW25QDevice, pucBuf, uiAddr, uiSize) ;

    if(W25Q_ERROR_OK != enFuncCallRetVal)
    {
        return W25Q_ERROR_READ_INTERNALREAD_ERR ;
    }

    /* W25Q解锁 */
    OSSemPost((OS_SEM  *)&(pstW25QDevice->stW25QDeviceLock), 
              (OS_OPT   )OS_OPT_POST_1, 
              (OS_ERR  *)&enErr) ;  

    if(OS_ERR_NONE != enErr)
    {
        return W25Q_ERROR_READ_POSTDEVSEM_ERR ;
    }

    return W25Q_ERROR_OK ;
}


/*******************************************************************************
- Function    : DRV_W25Q_Write
- Description : 本函数执行W25Q Flash器件的写入操作。
- Input       : pstW25QDevice :
                    W25Q设备指针。
                pucBuf  : 
                    指向待编程的数据缓存。
                uiAddr  : 
                    需编程的Flash存储区地址。
                uiSize  : 
                    指示编程大小。必须大于1
- Output      : NULL 
- Return      : DRV_W25Q_ERROR_CODE_E ：错误码。
                    W25Q_ERROR_OK ：操作正常。
                    其他            ：查看具体定义。
- Others      :
*******************************************************************************/
DRV_W25Q_ERROR_CODE_E DRV_W25Q_Write(DRV_W25Q_DEVICE_S *pstW25QDevice, UCHAR *pucBuf, UINT uiAddr, UINT uiSize)
{
    OS_ERR                enErr ;
    DRV_W25Q_ERROR_CODE_E enFuncCallRetVal ;
    UINT                  uiFirstSectorAddr ;
    UINT                  uiLastSectorAddr ;
    UINT                  uiFirstSectorWrSize ;
    UINT                  uiLastSectorWrSize ;
    UINT                  uiLoop ;
    UINT                  uiLoopPage ;

    if(NULL == pstW25QDevice)
    {
        return W25Q_ERROR_WRITE_DEVICE_PTR_NULL ;
    }

    if(DRV_W25Q_DEVICE_OPEN_FLAG != pstW25QDevice->uiDevOpenFlag)
    {
        return W25Q_ERROR_WRITE_DEVICE_NOT_OPENED ;
    }

    if(NULL == pucBuf)
    {
        return W25Q_ERROR_WRITE_BUF_PTR_NULL ;
    }

    /* W25Q加锁 */
    OSSemPend((OS_SEM   *)&(pstW25QDevice->stW25QDeviceLock), 
              (OS_TICK   )0, 
              (OS_OPT    )OS_OPT_PEND_BLOCKING, 
              (CPU_TS   *)NULL, 
              (OS_ERR   *)&enErr) ;

    if(OS_ERR_NONE != enErr)
    {
        return W25Q_ERROR_WRITE_PENDDEVSEM_ERR ;
    }
    
    /* 计算首个扇区和最后一个扇区的物理地址 */
    uiFirstSectorAddr = uiAddr & 0xFFF000 ;                /* 扇区4KB，对应12位地址 */
    uiLastSectorAddr  = (uiAddr + uiSize - 1) & 0xFFF000 ; /* 扇区4KB，对应12位地址 */

    /* 逐扇区操作 */
    for(uiLoop = uiFirstSectorAddr ; uiLoop <= uiLastSectorAddr ; uiLoop = uiLoop + DRV_W25Q_FLASH_SECTOR_SIZE)
    { 
        /* 读取扇区中的所有数据 */
        enFuncCallRetVal = __DRV_W25Q_InternalRead(pstW25QDevice, 
                                                   (UCHAR *)pstW25QDevice->pvSectorBuf, 
                                                   uiLoop, 
                                                   DRV_W25Q_FLASH_SECTOR_SIZE) ;

        if(W25Q_ERROR_OK != enFuncCallRetVal)
        {
            return W25Q_ERROR_WRITE_INTERNALREAD_ERR ;
        }

        if(W25Q_ERROR_OK != enFuncCallRetVal)
        {
            return W25Q_ERROR_WRITE_ERASESECTOR_ERR ;
        }

        /* 第一个扇区处理 */
        if(uiLoop == uiFirstSectorAddr)
        {
            /* 首个扇区，计算本扇区的写入数据量 */
            if(DRV_W25Q_FLASH_SECTOR_SIZE - (uiAddr & 0x000FFF) >= uiSize)
            {
                /* 首个扇区的剩余空间大于或等于总写入数据量，则写入数据大小就为总数据量，此时是单扇区操作 */
                uiFirstSectorWrSize = uiSize ;
            }
            else
            {
                /* 首个扇区的剩余空间小于总写入数据量，则本扇区剩余空间就是本扇区的写入数据量 */
                uiFirstSectorWrSize = DRV_W25Q_FLASH_SECTOR_SIZE - (uiAddr & 0x000FFF) ;
            }

            /* 需要写入首个扇区的数据拷贝到缓存 */
            memcpy(((UCHAR *)pstW25QDevice->pvSectorBuf) + uiAddr - uiFirstSectorAddr,
                    pucBuf,
                    uiFirstSectorWrSize) ;
            
            pucBuf += uiFirstSectorWrSize ;
        }
        else if(uiLoop == uiLastSectorAddr)
        {
            /* 最后一个扇区，计算本扇区的写入数据量 */
            uiLastSectorWrSize = (uiSize - uiFirstSectorWrSize) % DRV_W25Q_FLASH_SECTOR_SIZE ;

            /* 注意：如果计算出来的最后一个扇区写入大小为0B的话，实际应该是一整个扇区，需要修正 */
            if(0 == uiLastSectorWrSize)
            {
                uiLastSectorWrSize = DRV_W25Q_FLASH_SECTOR_SIZE ;
            }

            /* 需要写入首个扇区的数据拷贝到缓存 */
            memcpy((UCHAR *)pstW25QDevice->pvSectorBuf,
                   pucBuf,
                   uiLastSectorWrSize) ;
        }
        else
        {
            /* 中间扇区 */
            memcpy((UCHAR *)pstW25QDevice->pvSectorBuf,
                   pucBuf,
                   DRV_W25Q_FLASH_SECTOR_SIZE) ;

            /* 调整缓存指针 */
            pucBuf += DRV_W25Q_FLASH_SECTOR_SIZE ;
        }

        /* 擦除扇区 */
        enFuncCallRetVal = __DRV_W25Q_EraseSector(pstW25QDevice, uiLoop) ;
        
        /* 向扇区写入数据，写整个扇区 */
        for(uiLoopPage = 0 ; uiLoopPage <= 15 ; uiLoopPage++)
        {
            enFuncCallRetVal = __DRV_W25Q_ProgramPage(pstW25QDevice,
                                                      ((UCHAR *)pstW25QDevice->pvSectorBuf) + (uiLoopPage * DRV_W25Q_FLASH_PAGE_SIZE),
                                                      uiLoop + (uiLoopPage * DRV_W25Q_FLASH_PAGE_SIZE),
                                                      DRV_W25Q_FLASH_PAGE_SIZE) ;
            if(W25Q_ERROR_OK != enFuncCallRetVal)
            {
                return W25Q_ERROR_WRITE_PROGRAMPAGE_ERR ;
            }
        }      
    }

    /* W25Q解锁 */
    OSSemPost((OS_SEM  *)&(pstW25QDevice->stW25QDeviceLock), 
              (OS_OPT   )OS_OPT_POST_1, 
              (OS_ERR  *)&enErr) ;

    if(OS_ERR_NONE != enErr)
    {
        return W25Q_ERROR_WRITE_POSTDEVSEM_ERR ;
    }

    return W25Q_ERROR_OK ;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file drv_w25q.c. *******/  
