/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : EGC_CTL.c
    Project name : EGC
    Module name  : 
    Date created : 2020年02月12日    16时50分27秒
    Author       : Ning.JianLi
    Description  : Ctrl模块源文件。
*******************************************************************************/

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "pub_type.h"
#include "pub_lib.h"

CODE_SECTION("==========================") ;
CODE_SECTION("==  HST模块包含文件     ==") ;
CODE_SECTION("==========================") ;
/* <标准库> */
#include <string.h>

/* <第三方> */
#include "os.h"
#include "stm32f4xx.h"

/* <项目内部> */
#include "drv_spi_stm32f4xx.h"
#include "drv_w25q.h"

#include "EGC_Boards.h"
#include "EGC_Config.h"

#include "EGC_CTL.h"
#include "EGC_HST.h"
#include "EGC_LG1.h"
#include "EGC_LG2.h"
#include "EGC_MSR.h"
#include "EGC_ESP.h"

CODE_SECTION("==========================") ;
CODE_SECTION("==  CTL模块全局变量     ==") ;
CODE_SECTION("==========================") ;
/* 全局工作区 */
EGC_CTL_WORKAREA_S           g_stCTLWA ;

/* SPI设备 */
DRV_SPI_DEVICE_S             g_stW25QSPIDevice ;

/* W25Q设备 */
DRV_W25Q_DEVICE_S            g_stW25QDevice ;

/* 用户工作信息 */
EGC_CONFIG_INFO_S            g_stEGCConfigInfo ;

/* 外部变量 */
extern EGC_LG1_WORKAREA_S    g_stLG1WA ;
extern EGC_LG2_WORKAREA_S    g_stLG2WA ;
extern EGC_MSR_WORKAREA_S    g_stMSRWA ; 
extern EGC_ESP_WORKAREA_S    g_stESPWA ;
extern EGC_HST_WORKAREA_S    g_stHSTWA ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  CTL模块内部函数     ==") ;
CODE_SECTION("==========================") ;
/*******************************************************************************
- Function    : __EGC_CTL_INIT_CONFIGGPIO_GetPinGroup
- Description : 本函数输入一个EGC_Boards.h文件中定义的引脚(EGC_BOARDS_GPIO_Pxx)，
                返回该引脚所在的GPIO组(GPIOx)。
- Input       : uiPin         ：EGC_BOARDS_GPIO_Pxx
- Output      : ppstGPIOGroup ：用于保存GPIOx的指针
- Return      : VOID
- Others      :
*******************************************************************************/
STATIC VOID __EGC_CTL_INIT_CONFIGGPIO_GetPinGroup(UINT uiPin, GPIO_TypeDef **ppstGPIOGroup)
{
    switch(EGC_BOARDS_GPIO_GROUP_MASK & uiPin)
    {
        case EGC_BOARDS_GPIO_GROUP_A :
            *ppstGPIOGroup = GPIOA ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_B :
            *ppstGPIOGroup = GPIOB ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_C :
            *ppstGPIOGroup = GPIOC ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_D :
            *ppstGPIOGroup = GPIOD ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_E :
            *ppstGPIOGroup = GPIOE ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_F :
            *ppstGPIOGroup = GPIOF ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_G :
            *ppstGPIOGroup = GPIOG ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_H :
            *ppstGPIOGroup = GPIOH ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_I :
            *ppstGPIOGroup = GPIOI ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_J :
            *ppstGPIOGroup = GPIOJ ;
            break ;
        case EGC_BOARDS_GPIO_GROUP_K :
            *ppstGPIOGroup = GPIOK ;
            break ;            
        default:
            *ppstGPIOGroup = NULL ;
    }

    return ;
}


/*******************************************************************************
- Function    : __EGC_CTL_INIT_CONFIGGPIO_GetPinIndex
- Description : 本函数输入一个EGC_Boards.h文件中定义的引脚(EGC_BOARDS_GPIO_Pxx)，
                返回该引脚的GPIO序号(GPIO_Pin_x)。
- Input       : uiPin    ：EGC_BOARDS_GPIO_Pxx
- Output      : puiIndex ：用于保存GPIO_Pin_x的指针
- Return      : VOID
- Others      :
*******************************************************************************/
STATIC VOID __EGC_CTL_INIT_CONFIGGPIO_GetPinIndex(UINT uiPin, UINT *puiIndex)
{
    switch(EGC_BOARDS_GPIO_BIT_MASK & uiPin)
    {
        case EGC_BOARDS_GPIO_BIT_0 :
            *puiIndex = GPIO_Pin_0 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_1 :
            *puiIndex = GPIO_Pin_1 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_2 :
            *puiIndex = GPIO_Pin_2 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_3 :
            *puiIndex = GPIO_Pin_3 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_4 :
            *puiIndex = GPIO_Pin_4 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_5 :
            *puiIndex = GPIO_Pin_5 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_6 :
            *puiIndex = GPIO_Pin_6 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_7 :
            *puiIndex = GPIO_Pin_7 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_8 :
            *puiIndex = GPIO_Pin_8 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_9 :
            *puiIndex = GPIO_Pin_9 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_10 :
            *puiIndex = GPIO_Pin_10 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_11 :
            *puiIndex = GPIO_Pin_11 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_12 :
            *puiIndex = GPIO_Pin_12 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_13 :
            *puiIndex = GPIO_Pin_13 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_14 :
            *puiIndex = GPIO_Pin_14 ;
            break ;
        case EGC_BOARDS_GPIO_BIT_15 :
            *puiIndex = GPIO_Pin_15 ;
            break ;
        default:
            *puiIndex = NULL ;
    }

    return ;
}


/*******************************************************************************
- Function    : __EGC_CTL_W25Q_SPISend
- Description : 本函数实现面向W25Q SPI的发送操作。
- Input       : pucBuf ：
                    待发送的数据。
                uiSize :
                    待发送的数据量。
                uiOpMode :
                    发送模式设定。
                    DRV_W25Q_SPI_OP_MODE_NONE ：
                        不指定任何模式。
                    DRV_W25Q_SPI_OP_MODE_WITH_START ：
                        数据传输前发送SPI Start。
                    DRV_W25Q_SPI_OP_MODE_WITH_STOP ：
                        数据传输后发送SPI Stop。
                    DRV_W25Q_SPI_OP_MODE_WITH_DMA ：
                        数据使用DMA模式发送。
- Output      : NULL
- Return      : 0 ：
                    操作正常。
                其他值：
                    操作失败。
- Others      :
*******************************************************************************/
UINT __EGC_CTL_W25Q_SPISend(UCHAR *pucBuf, UINT uiSize, UINT uiOpMode)
{
    DRV_SPI_ERROR_CODE_E enSPIErr ;
    UINT                 uiMode = 0 ;

    if(DRV_W25Q_SPI_OP_MODE_NONE == uiOpMode)
    {
        uiMode = DRV_SPI_READ_WRITE_MODE_INT ;
    }
    else
    {
        if(0 != (DRV_W25Q_SPI_OP_MODE_WITH_START & uiOpMode))
        {
            /* 发出起始条件 */
            uiMode |= DRV_SPI_READ_WRITE_WITH_START ; 
        }

        if(0 != (DRV_W25Q_SPI_OP_MODE_WITH_STOP & uiOpMode))
        {
            /* 发出停止条件 */
            uiMode |= DRV_SPI_READ_WRITE_WITH_STOP ;
        }

        if(0 != (DRV_W25Q_SPI_OP_MODE_WITH_DMA & uiOpMode))
        {
            /* DMA方式访问 */
            uiMode |= DRV_SPI_READ_WRITE_MODE_DMA ;
        }
        else
        {
            /* INT方式访问 */
            uiMode |= DRV_SPI_READ_WRITE_MODE_INT ;
        }
    }
    
    enSPIErr = DRV_SPI_Write(&g_stW25QSPIDevice, pucBuf, NULL, uiSize, uiMode) ;

    if(SPI_ERROR_OK == enSPIErr)
    {
        return 0 ;
    }
    else
    {
        return 1 ;
    }
}

/*******************************************************************************
- Function    : __EGC_CTL_W25Q_SPIRecv
- Description : 本函数实现面向W25Q SPI的接收操作。
- Input       : uiSize :
                    待发送的数据量。
                uiOpMode :
                    发送模式设定。
                    DRV_W25Q_SPI_OP_MODE_NONE ：
                        不指定任何模式。
                    DRV_W25Q_SPI_OP_MODE_WITH_START ：
                        数据传输前发送SPI Start。
                    DRV_W25Q_SPI_OP_MODE_WITH_STOP ：
                        数据传输后发送SPI Stop。
                    DRV_W25Q_SPI_OP_MODE_WITH_DMA ：
                        数据使用DMA模式发送。
- Output      : pucBuf ：
                    接收数据的缓存。
- Return      : 0 ：
                    操作正常。
                其他值：
                    操作失败。
- Others      :
*******************************************************************************/
UINT __EGC_CTL_W25Q_SPIRecv(UCHAR *pucBuf, UINT uiSize, UINT uiOpMode)
{
    DRV_SPI_ERROR_CODE_E enSPIErr ;
    UINT                 uiMode = 0 ;

    if(DRV_W25Q_SPI_OP_MODE_NONE == uiOpMode)
    {
        uiMode = DRV_SPI_READ_WRITE_MODE_INT ;
    }
    else
    {
        if(0 != (DRV_W25Q_SPI_OP_MODE_WITH_START & uiOpMode))
        {
            /* 发出起始条件 */
            uiMode |= DRV_SPI_READ_WRITE_WITH_START ; 
        }

        if(0 != (DRV_W25Q_SPI_OP_MODE_WITH_STOP & uiOpMode))
        {
            /* 发出停止条件 */
            uiMode |= DRV_SPI_READ_WRITE_WITH_STOP ;
        }

        if(0 != (DRV_W25Q_SPI_OP_MODE_WITH_DMA & uiOpMode))
        {
            /* DMA方式访问 */
            uiMode |= DRV_SPI_READ_WRITE_MODE_DMA ;
        }
        else
        {
            /* INT方式访问 */
            uiMode |= DRV_SPI_READ_WRITE_MODE_INT ;
        }
    }
    
    enSPIErr = DRV_SPI_Read(&g_stW25QSPIDevice, pucBuf, uiSize, uiMode) ;

    if(SPI_ERROR_OK == enSPIErr)
    {
        return 0 ;
    }
    else
    {
        return 1 ;
    }
}


/*******************************************************************************
- Function    : __EGC_CTL_FormatUserConfig
- Description : 本函数格式化用户配置信息。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_CTL_FormatUserConfig(VOID)
{
    /* 复位所有数据 */
    memset((VOID     *)&g_stEGCConfigInfo.aucRawData[0], 
           (INT       )0, 
           (size_t    )sizeof(g_stEGCConfigInfo.aucRawData));
    
    /* 标记位格式化 */
    g_stEGCConfigInfo.uiEnableTimeShow       = 0 ;            /* 默认禁止定时显示，显示一直打开 */
    g_stEGCConfigInfo.uiEnableEgroShow       = 0 ;            /* 默认禁止遍历显示，一直显示时间 */
    g_stEGCConfigInfo.uiEnableTouchSense     = 1 ;            /* 默认禁止触摸感知 */
    g_stEGCConfigInfo.uiEnableWIFIUpdateTime = 0 ;            /* 默认禁止WIFI更新时间 */

    /* 亮度格式化 */
    g_stEGCConfigInfo.enLCDBrightness = BRIGHTNESS_LEVEL_MAX ; /* 默认亮度等级MAX，最亮 */

    /* 字符组设定 */
    g_stEGCConfigInfo.uiCharGroup = 1 ;

    /* 定时显示信息格式化 */
    g_stEGCConfigInfo.stTimeShowInfo1.ucValid       = 0 ;     /* 禁止本条目 */
    g_stEGCConfigInfo.stTimeShowInfo1.ucMonday      = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo1.ucTuesday     = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo1.ucWednesday   = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo1.ucThursday    = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo1.ucFriday      = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo1.ucSaturday    = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo1.ucSunday      = 1 ;
    
    g_stEGCConfigInfo.stTimeShowInfo1.ucStartHour   = 0 ;
    g_stEGCConfigInfo.stTimeShowInfo1.ucStartMinute = 0 ;
    g_stEGCConfigInfo.stTimeShowInfo1.ucEndHour     = 23 ;
    g_stEGCConfigInfo.stTimeShowInfo1.ucEndMinute   = 59 ;

    g_stEGCConfigInfo.stTimeShowInfo2.ucValid       = 0 ;     /* 禁止本条目 */
    g_stEGCConfigInfo.stTimeShowInfo2.ucMonday      = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo2.ucTuesday     = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo2.ucWednesday   = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo2.ucThursday    = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo2.ucFriday      = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo2.ucSaturday    = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo2.ucSunday      = 1 ;
    
    g_stEGCConfigInfo.stTimeShowInfo2.ucStartHour   = 0 ;
    g_stEGCConfigInfo.stTimeShowInfo2.ucStartMinute = 0 ;
    g_stEGCConfigInfo.stTimeShowInfo2.ucEndHour     = 23 ;
    g_stEGCConfigInfo.stTimeShowInfo2.ucEndMinute   = 59 ;

    g_stEGCConfigInfo.stTimeShowInfo3.ucValid       = 0 ;       /* 禁止本条目 */
    g_stEGCConfigInfo.stTimeShowInfo3.ucMonday      = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo3.ucTuesday     = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo3.ucWednesday   = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo3.ucThursday    = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo3.ucFriday      = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo3.ucSaturday    = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo3.ucSunday      = 1 ;
    
    g_stEGCConfigInfo.stTimeShowInfo3.ucStartHour   = 0 ;
    g_stEGCConfigInfo.stTimeShowInfo3.ucStartMinute = 0 ;
    g_stEGCConfigInfo.stTimeShowInfo3.ucEndHour     = 23 ;
    g_stEGCConfigInfo.stTimeShowInfo3.ucEndMinute   = 59 ;

    g_stEGCConfigInfo.stTimeShowInfo4.ucValid       = 0 ;       /* 禁止本条目 */
    g_stEGCConfigInfo.stTimeShowInfo4.ucMonday      = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo4.ucTuesday     = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo4.ucWednesday   = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo4.ucThursday    = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo4.ucFriday      = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo4.ucSaturday    = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo4.ucSunday      = 1 ;
    
    g_stEGCConfigInfo.stTimeShowInfo4.ucStartHour   = 0 ;
    g_stEGCConfigInfo.stTimeShowInfo4.ucStartMinute = 0 ;
    g_stEGCConfigInfo.stTimeShowInfo4.ucEndHour     = 23 ;
    g_stEGCConfigInfo.stTimeShowInfo4.ucEndMinute   = 59 ;

    g_stEGCConfigInfo.stTimeShowInfo5.ucValid       = 0 ;       /* 禁止本条目 */
    g_stEGCConfigInfo.stTimeShowInfo5.ucMonday      = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo5.ucTuesday     = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo5.ucWednesday   = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo5.ucThursday    = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo5.ucFriday      = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo5.ucSaturday    = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo5.ucSunday      = 1 ;
    
    g_stEGCConfigInfo.stTimeShowInfo5.ucStartHour   = 0 ;
    g_stEGCConfigInfo.stTimeShowInfo5.ucStartMinute = 0 ;
    g_stEGCConfigInfo.stTimeShowInfo5.ucEndHour     = 23 ;
    g_stEGCConfigInfo.stTimeShowInfo5.ucEndMinute   = 59 ;

    g_stEGCConfigInfo.stTimeShowInfo6.ucValid       = 0 ;       /* 禁止本条目 */
    g_stEGCConfigInfo.stTimeShowInfo6.ucMonday      = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo6.ucTuesday     = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo6.ucWednesday   = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo6.ucThursday    = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo6.ucFriday      = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo6.ucSaturday    = 1 ;
    g_stEGCConfigInfo.stTimeShowInfo6.ucSunday      = 1 ;
    
    g_stEGCConfigInfo.stTimeShowInfo6.ucStartHour   = 0 ;
    g_stEGCConfigInfo.stTimeShowInfo6.ucStartMinute = 0 ;
    g_stEGCConfigInfo.stTimeShowInfo6.ucEndHour     = 23 ;
    g_stEGCConfigInfo.stTimeShowInfo6.ucEndMinute   = 59 ;

    /* 遍历显示信息格式化 */
    g_stEGCConfigInfo.stErgoShowInfo.ucShowDate     = 0 ;
    g_stEGCConfigInfo.stErgoShowInfo.ucShowTemp     = 0 ;
    g_stEGCConfigInfo.stErgoShowInfo.ucShowHumd     = 0 ;

    g_stEGCConfigInfo.stErgoShowInfo.ucShowDateSecondsPerMinute = 5 ;
    g_stEGCConfigInfo.stErgoShowInfo.ucShowTempSecondsPerMinute = 5 ;
    g_stEGCConfigInfo.stErgoShowInfo.ucShowHumdSecondsPerMinute = 5 ;

    /* 触摸功能格式化 */
    g_stEGCConfigInfo.stTouchConfigInfo.ucTouchToShow      = 0 ;
    g_stEGCConfigInfo.stTouchConfigInfo.ucTouchToSwitch    = 1 ;
    g_stEGCConfigInfo.stTouchConfigInfo.ucTouchToStopAlarm = 0 ;

    g_stEGCConfigInfo.stTouchConfigInfo.enTouchSenseLevel  = TOUCH_SENSE_LEVEL_VERY_HIGH ;

    /* WIFI功能格式化 */
    g_stEGCConfigInfo.stWIFIConfigInfo.stAP1Info.ucValid = 0 ;
    g_stEGCConfigInfo.stWIFIConfigInfo.stAP2Info.ucValid = 0 ;
    g_stEGCConfigInfo.stWIFIConfigInfo.stNTPServer1.ucValid = 0 ;
    g_stEGCConfigInfo.stWIFIConfigInfo.stNTPServer2.ucValid = 0 ;
    g_stEGCConfigInfo.stWIFIConfigInfo.stNTPServer3.ucValid = 0 ;

    memset((VOID     *)&g_stEGCConfigInfo.stWIFIConfigInfo.stAP1Info.acSSID[0], 
           (INT       )0, 
           (size_t    )sizeof(g_stEGCConfigInfo.stWIFIConfigInfo.stAP1Info.acSSID));

    memset((VOID     *)&g_stEGCConfigInfo.stWIFIConfigInfo.stAP1Info.acPassword[0], 
           (INT       )0, 
           (size_t    )sizeof(g_stEGCConfigInfo.stWIFIConfigInfo.stAP1Info.acPassword));

    memset((VOID     *)&g_stEGCConfigInfo.stWIFIConfigInfo.stAP2Info.acSSID[0], 
           (INT       )0, 
           (size_t    )sizeof(g_stEGCConfigInfo.stWIFIConfigInfo.stAP2Info.acSSID));

    memset((VOID     *)&g_stEGCConfigInfo.stWIFIConfigInfo.stAP2Info.acPassword[0], 
           (INT       )0, 
           (size_t    )sizeof(g_stEGCConfigInfo.stWIFIConfigInfo.stAP2Info.acPassword));

    memset((VOID     *)&g_stEGCConfigInfo.stWIFIConfigInfo.stNTPServer1.acNTPServer[0], 
           (INT       )0, 
           (size_t    )sizeof(g_stEGCConfigInfo.stWIFIConfigInfo.stNTPServer1.acNTPServer));

    memset((VOID     *)&g_stEGCConfigInfo.stWIFIConfigInfo.stNTPServer2.acNTPServer[0], 
           (INT       )0, 
           (size_t    )sizeof(g_stEGCConfigInfo.stWIFIConfigInfo.stNTPServer2.acNTPServer));

    memset((VOID     *)&g_stEGCConfigInfo.stWIFIConfigInfo.stNTPServer3.acNTPServer[0], 
           (INT       )0, 
           (size_t    )sizeof(g_stEGCConfigInfo.stWIFIConfigInfo.stNTPServer3.acNTPServer));

    g_stEGCConfigInfo.stWIFIConfigInfo.enUpdateIntervel = UPDATE_TIME_4_HOUR ;

    /* 格式化标记置位 */
    g_stEGCConfigInfo.uiFormatFlag = EGC_CONFIG_USER_CONFIG_FORMAT_FLAG ;

    return ;
}

/*******************************************************************************
- Function    : __EGC_CTL_GetUserConfig
- Description : 本函数读取用户配置信息。如果配置信息没有被格式化，还将格式化这些
                配置信息。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_CTL_GetUserConfig(VOID)
{
    UINT uiCharGroupBaseAddr ;
    
    /* 读取全局配置信息 */
    DRV_W25Q_Read(&g_stW25QDevice,
                   g_stEGCConfigInfo.aucRawData, 
                   EGC_CONFIG_W25Q_ADDR_CONFIG,
                   sizeof(EGC_CONFIG_INFO_S)) ;

    /* 检查全局配置信息是否被格式化，如果格式化了，则格式化配置信息为初始状态 */
    if(EGC_CONFIG_USER_CONFIG_FORMAT_FLAG != g_stEGCConfigInfo.uiFormatFlag)
    {
        __EGC_CTL_FormatUserConfig() ;

        DRV_W25Q_Write(&g_stW25QDevice,
                        g_stEGCConfigInfo.aucRawData, 
                        0x000000,
                        sizeof(EGC_CONFIG_INFO_S)) ;
    }

    /* 读取字符组 */
    switch(g_stEGCConfigInfo.uiCharGroup)
    {
        case 0 :
            uiCharGroupBaseAddr = EGC_CONFIG_W25Q_ADDR_CHAR_GROUP1 ;
            break ;
        case 1 :
            uiCharGroupBaseAddr = EGC_CONFIG_W25Q_ADDR_CHAR_GROUP2 ;
            break ;
        case 2 :
            uiCharGroupBaseAddr = EGC_CONFIG_W25Q_ADDR_CHAR_GROUP3 ;
            break ;
        case 3 :
            uiCharGroupBaseAddr = EGC_CONFIG_W25Q_ADDR_CHAR_GROUP4 ;
            break ;
        case 4 :
            uiCharGroupBaseAddr = EGC_CONFIG_W25Q_ADDR_CHAR_GROUP5 ;
            break ;
        case 5 :
            uiCharGroupBaseAddr = EGC_CONFIG_W25Q_ADDR_CHAR_GROUP6 ;
            break ; 
        case 6 :
            uiCharGroupBaseAddr = EGC_CONFIG_W25Q_ADDR_CHAR_GROUP7 ;
            break ;
        case 7 :
            uiCharGroupBaseAddr = EGC_CONFIG_W25Q_ADDR_CHAR_GROUP8 ;
            break ;            
        case 8 :
            uiCharGroupBaseAddr = EGC_CONFIG_W25Q_ADDR_CHAR_GROUP9 ;
            break ;
        case 9 :
            uiCharGroupBaseAddr = EGC_CONFIG_W25Q_ADDR_CHAR_GROUP10 ;
            break ;
        default:
            uiCharGroupBaseAddr = EGC_CONFIG_W25Q_ADDR_CHAR_GROUP1 ;
            break ;
    }

    DRV_W25Q_Read(&g_stW25QDevice,
                   (UCHAR *)EGC_CONFIG_SDRAM_ADDR_CHAR_GROUP_BUF, 
                   uiCharGroupBaseAddr,
                   EGC_CONFIG_W25Q_SIZE_CHAR_GROUP_BASE) ;
    
    return ;
}

/*******************************************************************************
- Function    : __EGC_CTL_Init
- Description : 本函数初始化CTL模块。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_CTL_Init(VOID)
{
    DRV_SPI_INIT_INFO_S           stSPIInitInfo ;
    volatile DRV_SPI_ERROR_CODE_E enSPIRetval ;
    DRV_W25Q_INIT_INFO_S          stW25QInitInfo ;

    /* 初始化SPI */
    __EGC_CTL_INIT_CONFIGGPIO_GetPinGroup(EGC_BOARDS_GPIO_W25Q_NSS,  &stSPIInitInfo.pstNSSGPIOGroup) ;
    __EGC_CTL_INIT_CONFIGGPIO_GetPinGroup(EGC_BOARDS_GPIO_W25Q_SCK,  &stSPIInitInfo.pstSCKGPIOGroup) ;
    __EGC_CTL_INIT_CONFIGGPIO_GetPinGroup(EGC_BOARDS_GPIO_W25Q_MOSI, &stSPIInitInfo.pstMOSIGPIOGroup) ;
    __EGC_CTL_INIT_CONFIGGPIO_GetPinGroup(EGC_BOARDS_GPIO_W25Q_MISO, &stSPIInitInfo.pstMISOGPIOGroup) ;

    __EGC_CTL_INIT_CONFIGGPIO_GetPinIndex(EGC_BOARDS_GPIO_W25Q_NSS,  &stSPIInitInfo.uiNSSPinID) ;
    __EGC_CTL_INIT_CONFIGGPIO_GetPinIndex(EGC_BOARDS_GPIO_W25Q_SCK,  &stSPIInitInfo.uiSCKPinID) ;
    __EGC_CTL_INIT_CONFIGGPIO_GetPinIndex(EGC_BOARDS_GPIO_W25Q_MOSI, &stSPIInitInfo.uiMOSIPinID) ;
    __EGC_CTL_INIT_CONFIGGPIO_GetPinIndex(EGC_BOARDS_GPIO_W25Q_MISO, &stSPIInitInfo.uiMISOPinID) ;
    
    stSPIInitInfo.ucSCKPinAFDef     = EGC_BOARDS_PERIPH_SPI_W25Q_AF_DEF ;
    stSPIInitInfo.ucMOSIPinAFDef    = EGC_BOARDS_PERIPH_SPI_W25Q_AF_DEF ;
    stSPIInitInfo.ucMISOPinAFDef    = EGC_BOARDS_PERIPH_SPI_W25Q_AF_DEF ;

    stSPIInitInfo.pstSPIPeriph      = EGC_BOARDS_PERIPH_SPI_W25Q ;
    stSPIInitInfo.usSPIPrescaler    = SPI_BaudRatePrescaler_16 ;
    stSPIInitInfo.ucSPIMode         = DRV_SPI_MODE_3 ;
    stSPIInitInfo.uiSPIIrqNumber    = EGC_BOARDS_PERIPH_SPI_W25Q_INTN ;
    stSPIInitInfo.bUseDMA           = BOOL_TRUE ;

    stSPIInitInfo.pstSPIDMATxStream = EGC_BOARDS_PERIPH_SPI_W25Q_DMA_TX_STREAM ;
    stSPIInitInfo.pstSPIDMARxStream = EGC_BOARDS_PERIPH_SPI_W25Q_DMA_RX_STREAM ;
    stSPIInitInfo.uiSPIDMATxChannel = EGC_BOARDS_PERIPH_SPI_W25Q_DMA_TX_CHANNEL ;
    stSPIInitInfo.uiSPIDMARxChannel = EGC_BOARDS_PERIPH_SPI_W25Q_DMA_RX_CHANNEL ;
    stSPIInitInfo.uiSPIDMATxIrqNum  = EGC_BOARDS_PERIPH_SPI_W25Q_DMA_TX_IRQ_NUM ;
    stSPIInitInfo.uiSPIDMARxIrqNum  = EGC_BOARDS_PERIPH_SPI_W25Q_DMA_RX_IRQ_NUM ;

    enSPIRetval = DRV_SPI_Open(&g_stW25QSPIDevice, &stSPIInitInfo) ;
    
    /* SPI及W25Q Flash初始化 */
    stW25QInitInfo.pfSPIRecvFunction = __EGC_CTL_W25Q_SPIRecv ;
    stW25QInitInfo.pfSPISendFunction = __EGC_CTL_W25Q_SPISend ;
    stW25QInitInfo.pvSectorBuf       = (VOID *)EGC_CONFIG_SDRAM_ADDR_W25Q_SECTOR_BUF ;
    
    DRV_W25Q_Open(&g_stW25QDevice, &stW25QInitInfo) ;

    /* 获取用户配置信息 */
    __EGC_CTL_GetUserConfig() ;

    /* 初始显示类型 */
    g_stCTLWA.enShowType = SHOW_TYPE_TIME ;

    return ;
}


/*******************************************************************************
- Function    : __EGC_CTL_ConvertChar2BufAddr
- Description : 本函数转换一个输入字符，并返回该字符对应的LCD图形的指针。
- Input       : cChar :
                    输入字符。
- Output      : NULL
- Return      : UCHAR * ：
                    指向字符图形的指针。
- Others      :
*******************************************************************************/
UCHAR *__EGC_CTL_ConvertChar2BufAddr(CHAR cChar)
{
    EGC_CONFIG_CHAR_GROUP_S *pstCharGroup ;

    pstCharGroup = (EGC_CONFIG_CHAR_GROUP_S *)EGC_CONFIG_SDRAM_ADDR_CHAR_GROUP_BUF ;

    if(' ' == cChar)
    {
        return pstCharGroup->aucCharSpace ;
    }
    else if('%' == cChar)
    {
        return pstCharGroup->aucCharPercentSign ;
    }
    else if('C' == cChar)
    {
        return pstCharGroup->aucCharCelsiusSign ;
    }
    else if(':' == cChar)
    {
        return pstCharGroup->aucCharColonSign ;
    }
    else if('/' == cChar)
    {
        return pstCharGroup->aucCharStrokeSign ;
    }
    else if('0' <= cChar && '9' >= cChar)
    {
        switch(cChar)
        {
            case '0' :
                return pstCharGroup->aucChar0 ;
            case '1' :
                return pstCharGroup->aucChar1 ;
            case '2' :
                return pstCharGroup->aucChar2 ;
            case '3' :
                return pstCharGroup->aucChar3 ;
            case '4' :
                return pstCharGroup->aucChar4 ;
            case '5' :
                return pstCharGroup->aucChar5 ;
            case '6' :
                return pstCharGroup->aucChar6 ;
            case '7' :
                return pstCharGroup->aucChar7 ;
            case '8' :
                return pstCharGroup->aucChar8 ;
            case '9' :
                return pstCharGroup->aucChar9 ;
            default :
                return pstCharGroup->aucCharSpace ; 
        }
    }

    return pstCharGroup->aucCharSpace ; 
}

/*******************************************************************************
- Function    : __EGC_CTL_UpdateShow
- Description : 本函数刷新显示内容。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_CTL_UpdateShow(VOID)
{
    CHAR                acString[16] ;
    BOOL_T              bShow = BOOL_FALSE ;
    UCHAR               ucDateStartSecond = 60 ;
    UCHAR               ucTempStartSecond = 60 ;
    UCHAR               ucHumdStartSecond = 60 ;
    USHORT              ucStartMinute ;
    USHORT              ucEndMinute ;
    USHORT              ucCurrMinute ;
    EGC_CTL_SHOW_TYPE_E enShowReference ;
    RTC_TimeTypeDef     stCurrentTime ;
    RTC_DateTypeDef     stCurrentDate ;

    /**********************************************************************************************
     * 阶段1：根据时间判断当前是否显示。
     *        阶段1.1：根据重力加速度判断当前设备是否水平放置，如果没有水平放置则不显示。
     *********************************************************************************************/
    /* 获取当前时间 */
    RTC_GetTime(RTC_Format_BIN, &stCurrentTime) ;
    RTC_GetDate(RTC_Format_BIN, &stCurrentDate) ;
    ucCurrMinute = (stCurrentTime.RTC_Hours * 60) + stCurrentTime.RTC_Minutes ;

    /* 检查是否强制显示 */
    if(1 == g_stCTLWA.uiForceShow)
    {
        g_stLG1WA.uiHide = 0 ;
        g_stLG2WA.uiHide = 0 ;
    }
    else if(-0.8f < g_stMSRWA.fZAxisAcc)
    {
        /* Z向加速度不够大，认为设备没有被水平放置，不显示 */
        g_stLG1WA.uiHide = 1 ;
        g_stLG2WA.uiHide = 1 ;
    }
    else
    {
        /* 检查时间是否坐落在一个指定的显示时间段内，并且判断是否使能了分段显示 */
        if(1 == g_stEGCConfigInfo.uiEnableTimeShow)
        {
            /* 使能了定时显示，则逐条判断当前时间是否位于显示时间段之内 */
            if(1 == g_stEGCConfigInfo.stTimeShowInfo1.ucValid)
            {
                ucStartMinute = (g_stEGCConfigInfo.stTimeShowInfo1.ucStartHour * 60) + g_stEGCConfigInfo.stTimeShowInfo1.ucStartMinute ;
                ucEndMinute   = (g_stEGCConfigInfo.stTimeShowInfo1.ucEndHour * 60) + g_stEGCConfigInfo.stTimeShowInfo1.ucEndMinute ;
                
                if(((1 == g_stEGCConfigInfo.stTimeShowInfo1.ucMonday    && RTC_Weekday_Monday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo1.ucTuesday   && RTC_Weekday_Tuesday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo1.ucWednesday && RTC_Weekday_Wednesday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo1.ucThursday  && RTC_Weekday_Thursday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo1.ucFriday    && RTC_Weekday_Friday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo1.ucSaturday  && RTC_Weekday_Saturday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo1.ucSunday    && RTC_Weekday_Sunday == stCurrentDate.RTC_WeekDay)) &&
                    ucCurrMinute >= ucStartMinute && ucCurrMinute < ucEndMinute)
                {
                    /* 位于时间段内，要允许显示 */
                    bShow = BOOL_TRUE ;
                }
            }

            if(1 == g_stEGCConfigInfo.stTimeShowInfo2.ucValid)
            {
                ucStartMinute = (g_stEGCConfigInfo.stTimeShowInfo2.ucStartHour * 60) + g_stEGCConfigInfo.stTimeShowInfo2.ucStartMinute ;
                ucEndMinute   = (g_stEGCConfigInfo.stTimeShowInfo2.ucEndHour * 60) + g_stEGCConfigInfo.stTimeShowInfo2.ucEndMinute ;
                
                if(((1 == g_stEGCConfigInfo.stTimeShowInfo2.ucMonday    && RTC_Weekday_Monday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo2.ucTuesday   && RTC_Weekday_Tuesday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo2.ucWednesday && RTC_Weekday_Wednesday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo2.ucThursday  && RTC_Weekday_Thursday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo2.ucFriday    && RTC_Weekday_Friday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo2.ucSaturday  && RTC_Weekday_Saturday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo2.ucSunday    && RTC_Weekday_Sunday == stCurrentDate.RTC_WeekDay)) &&
                    ucCurrMinute >= ucStartMinute && ucCurrMinute < ucEndMinute)
                {
                    /* 位于时间段内，要允许显示 */
                    bShow = BOOL_TRUE ;
                }
            }

            if(1 == g_stEGCConfigInfo.stTimeShowInfo3.ucValid)
            {
                ucStartMinute = (g_stEGCConfigInfo.stTimeShowInfo3.ucStartHour * 60) + g_stEGCConfigInfo.stTimeShowInfo3.ucStartMinute ;
                ucEndMinute   = (g_stEGCConfigInfo.stTimeShowInfo3.ucEndHour * 60) + g_stEGCConfigInfo.stTimeShowInfo3.ucEndMinute ;
                
                if(((1 == g_stEGCConfigInfo.stTimeShowInfo3.ucMonday    && RTC_Weekday_Monday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo3.ucTuesday   && RTC_Weekday_Tuesday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo3.ucWednesday && RTC_Weekday_Wednesday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo3.ucThursday  && RTC_Weekday_Thursday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo3.ucFriday    && RTC_Weekday_Friday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo3.ucSaturday  && RTC_Weekday_Saturday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo3.ucSunday    && RTC_Weekday_Sunday == stCurrentDate.RTC_WeekDay)) &&
                    ucCurrMinute >= ucStartMinute && ucCurrMinute < ucEndMinute)
                {
                    /* 位于时间段内，要允许显示 */
                    bShow = BOOL_TRUE ;
                }
            }

            /* 使能了定时显示，则逐条判断当前时间是否位于显示时间段之内 */
            if(1 == g_stEGCConfigInfo.stTimeShowInfo4.ucValid)
            {
                ucStartMinute = (g_stEGCConfigInfo.stTimeShowInfo4.ucStartHour * 60) + g_stEGCConfigInfo.stTimeShowInfo4.ucStartMinute ;
                ucEndMinute   = (g_stEGCConfigInfo.stTimeShowInfo4.ucEndHour * 60) + g_stEGCConfigInfo.stTimeShowInfo4.ucEndMinute ;
                
                if(((1 == g_stEGCConfigInfo.stTimeShowInfo4.ucMonday    && RTC_Weekday_Monday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo4.ucTuesday   && RTC_Weekday_Tuesday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo4.ucWednesday && RTC_Weekday_Wednesday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo4.ucThursday  && RTC_Weekday_Thursday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo4.ucFriday    && RTC_Weekday_Friday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo4.ucSaturday  && RTC_Weekday_Saturday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo4.ucSunday    && RTC_Weekday_Sunday == stCurrentDate.RTC_WeekDay)) &&
                    ucCurrMinute >= ucStartMinute && ucCurrMinute < ucEndMinute)
                {
                    /* 位于时间段内，要允许显示 */
                    bShow = BOOL_TRUE ;
                }
            }

            /* 使能了定时显示，则逐条判断当前时间是否位于显示时间段之内 */
            if(1 == g_stEGCConfigInfo.stTimeShowInfo5.ucValid)
            {
                ucStartMinute = (g_stEGCConfigInfo.stTimeShowInfo5.ucStartHour * 60) + g_stEGCConfigInfo.stTimeShowInfo5.ucStartMinute ;
                ucEndMinute   = (g_stEGCConfigInfo.stTimeShowInfo5.ucEndHour * 60) + g_stEGCConfigInfo.stTimeShowInfo5.ucEndMinute ;
                
                if(((1 == g_stEGCConfigInfo.stTimeShowInfo5.ucMonday    && RTC_Weekday_Monday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo5.ucTuesday   && RTC_Weekday_Tuesday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo5.ucWednesday && RTC_Weekday_Wednesday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo5.ucThursday  && RTC_Weekday_Thursday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo5.ucFriday    && RTC_Weekday_Friday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo5.ucSaturday  && RTC_Weekday_Saturday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo5.ucSunday    && RTC_Weekday_Sunday == stCurrentDate.RTC_WeekDay)) &&
                    ucCurrMinute >= ucStartMinute && ucCurrMinute < ucEndMinute)
                {
                    /* 位于时间段内，要允许显示 */
                    bShow = BOOL_TRUE ;
                }
            }

            /* 使能了定时显示，则逐条判断当前时间是否位于显示时间段之内 */
            if(1 == g_stEGCConfigInfo.stTimeShowInfo6.ucValid)
            {
                ucStartMinute = (g_stEGCConfigInfo.stTimeShowInfo6.ucStartHour * 60) + g_stEGCConfigInfo.stTimeShowInfo6.ucStartMinute ;
                ucEndMinute   = (g_stEGCConfigInfo.stTimeShowInfo6.ucEndHour * 60) + g_stEGCConfigInfo.stTimeShowInfo6.ucEndMinute ;
                
                if(((1 == g_stEGCConfigInfo.stTimeShowInfo6.ucMonday    && RTC_Weekday_Monday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo6.ucTuesday   && RTC_Weekday_Tuesday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo6.ucWednesday && RTC_Weekday_Wednesday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo6.ucThursday  && RTC_Weekday_Thursday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo6.ucFriday    && RTC_Weekday_Friday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo6.ucSaturday  && RTC_Weekday_Saturday == stCurrentDate.RTC_WeekDay) ||
                    (1 == g_stEGCConfigInfo.stTimeShowInfo6.ucSunday    && RTC_Weekday_Sunday == stCurrentDate.RTC_WeekDay)) &&
                    ucCurrMinute >= ucStartMinute && ucCurrMinute < ucEndMinute)
                {
                    /* 位于时间段内，要允许显示 */
                    bShow = BOOL_TRUE ;
                }
            }            

            if(BOOL_TRUE == bShow)
            {
                /* 显示 */
                g_stLG1WA.uiHide = 0 ;
                g_stLG2WA.uiHide = 0 ;
            }
            else
            {
                /* 不显示 */
                g_stLG1WA.uiHide = 1 ;
                g_stLG2WA.uiHide = 1 ;
            }
        }
        else
        {
            /* 没有使能定时显示，则一直显示 */
            g_stLG1WA.uiHide = 0 ;
            g_stLG2WA.uiHide = 0 ;
        }
    }

    /**********************************************************************************************
     * 阶段2：遍历显示控制。
     *        如果遍历显示打开，还可能进一步控制如下3个子项目：
     *        1、显示日期；
     *        2、显示温度；
     *        3、显示湿度；
     *        每分钟为一个显示周期。日期、温度、湿度放在分钟的最后显示，每项目最长5秒，因此时间显示
     *        最短为45秒。
     *********************************************************************************************/
    if(1 == g_stEGCConfigInfo.uiEnableEgroShow)
    {
        if(1 == g_stEGCConfigInfo.stErgoShowInfo.ucShowHumd)
        {
            ucHumdStartSecond = ucHumdStartSecond - g_stEGCConfigInfo.stErgoShowInfo.ucShowHumdSecondsPerMinute ;
            ucTempStartSecond = ucHumdStartSecond ;
            ucDateStartSecond = ucHumdStartSecond ;
        }

        if(1 == g_stEGCConfigInfo.stErgoShowInfo.ucShowTemp)
        {
            ucTempStartSecond = ucTempStartSecond - g_stEGCConfigInfo.stErgoShowInfo.ucShowTempSecondsPerMinute ;
            ucDateStartSecond = ucTempStartSecond ;
        }

        if(1 == g_stEGCConfigInfo.stErgoShowInfo.ucShowDate)
        {
            ucDateStartSecond = ucDateStartSecond - g_stEGCConfigInfo.stErgoShowInfo.ucShowDateSecondsPerMinute ;
        }

        if(stCurrentTime.RTC_Seconds >= ucHumdStartSecond)
        {
            /* 在此期间显示湿度 */
            g_stCTLWA.enShowType = SHOW_TYPE_HUMD ;
        }
        else if(stCurrentTime.RTC_Seconds >= ucTempStartSecond)
        {
            /* 在此期间显示日期 */
            g_stCTLWA.enShowType = SHOW_TYPE_TEMP ;
        }
        else if(stCurrentTime.RTC_Seconds >= ucDateStartSecond)
        {
            /* 在此期间显示日期 */
            g_stCTLWA.enShowType = SHOW_TYPE_DATE ;
        }
        else
        {
            g_stCTLWA.enShowType = SHOW_TYPE_TIME ;
        }
    }
    /**********************************************************************************************
     * 阶段3：判断当前显示哪个信息，并显示相应的信息。
     *********************************************************************************************/
    if(1 == g_stCTLWA.uiTouchShow)
    {
        /* 根据触摸切换显示 */
        enShowReference = g_stCTLWA.enTouchShowType ;
    }
    else
    {
        enShowReference = g_stCTLWA.enShowType ;
    }

    switch(enShowReference)
    {
        case SHOW_TYPE_DATE :
            if(SHOW_TYPE_DATE          != g_stCTLWA.enPreviousShow || 
               stCurrentDate.RTC_Year  != g_stCTLWA.stPreviousDate.RTC_Year ||
               stCurrentDate.RTC_Month != g_stCTLWA.stPreviousDate.RTC_Month ||
               stCurrentDate.RTC_Date  != g_stCTLWA.stPreviousDate.RTC_Date)
            {
                /* 两种情况刷新显示：1. 上次没有在显示日期，这次要显示日期；2. 日期发生改变。 */
                /* 显示日期 */
                sprintf(acString, "%02d/%02d/%02d", stCurrentDate.RTC_Year, stCurrentDate.RTC_Month, stCurrentDate.RTC_Date) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD1_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[0]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD2_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[1]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD3_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[2]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD4_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[3]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD5_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[4]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD6_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[5]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD7_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[6]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD8_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[7]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                /* 设定显示 */
                g_stLG1WA.uiLCD1UpdateView   = 1 ;
                g_stLG1WA.uiLCD2UpdateView   = 1 ;
                g_stLG1WA.uiLCD3UpdateView   = 1 ;
                g_stLG1WA.uiLCD4UpdateView   = 1 ;
                g_stLG2WA.uiLCD5UpdateView   = 1 ;
                g_stLG2WA.uiLCD6UpdateView   = 1 ;
                g_stLG2WA.uiLCD7UpdateView   = 1 ;
                g_stLG2WA.uiLCD8UpdateView   = 1 ;
                g_stLG1WA.uiLCD1UseSecondBuf = 0 ;
                g_stLG1WA.uiLCD2UseSecondBuf = 0 ;
                g_stLG1WA.uiLCD3UseSecondBuf = 0 ;
                g_stLG1WA.uiLCD4UseSecondBuf = 0 ;
                g_stLG2WA.uiLCD5UseSecondBuf = 0 ;
                g_stLG2WA.uiLCD6UseSecondBuf = 0 ;
                g_stLG2WA.uiLCD7UseSecondBuf = 0 ;
                g_stLG2WA.uiLCD8UseSecondBuf = 0 ; 
            }
            
            break ;
        case SHOW_TYPE_TEMP :
            if(SHOW_TYPE_TEMP != g_stCTLWA.enPreviousShow || g_stMSRWA.fTemp  != g_stCTLWA.fPreviousTemp)
            {
                /* 两种情况刷新显示：1. 上次没有在显示温度，这次要显示温度；2. 温度发生改变。 */
                /* 显示温度 */
                if(0 > g_stMSRWA.fTemp)
                {
                    /* 有负号 */
                    sprintf(acString, "     %02dC", (UINT)g_stMSRWA.fTemp) ;
                }
                else
                {
                    /* 无负号 */
                    sprintf(acString, "     %02dC", (UINT)g_stMSRWA.fTemp) ;
                }

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD1_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[0]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD2_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[1]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD3_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[2]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD4_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[3]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD5_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[4]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD6_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[5]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD7_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[6]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD8_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[7]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                /* 设定显示 */
                g_stLG1WA.uiLCD1UpdateView   = 1 ;
                g_stLG1WA.uiLCD2UpdateView   = 1 ;
                g_stLG1WA.uiLCD3UpdateView   = 1 ;
                g_stLG1WA.uiLCD4UpdateView   = 1 ;
                g_stLG2WA.uiLCD5UpdateView   = 1 ;
                g_stLG2WA.uiLCD6UpdateView   = 1 ;
                g_stLG2WA.uiLCD7UpdateView   = 1 ;
                g_stLG2WA.uiLCD8UpdateView   = 1 ;
                g_stLG1WA.uiLCD1UseSecondBuf = 0 ;
                g_stLG1WA.uiLCD2UseSecondBuf = 0 ;
                g_stLG1WA.uiLCD3UseSecondBuf = 0 ;
                g_stLG1WA.uiLCD4UseSecondBuf = 0 ;
                g_stLG2WA.uiLCD5UseSecondBuf = 0 ;
                g_stLG2WA.uiLCD6UseSecondBuf = 0 ;
                g_stLG2WA.uiLCD7UseSecondBuf = 0 ;
                g_stLG2WA.uiLCD8UseSecondBuf = 0 ; 
            }

            break ;
        case SHOW_TYPE_HUMD :
            if(SHOW_TYPE_HUMD != g_stCTLWA.enPreviousShow || g_stMSRWA.fHumi != g_stCTLWA.fPreviousHumi)
            {
                /* 两种情况刷新显示：1. 上次没有在显示湿度，这次要显示湿度；2. 湿度发生改变。 */
                /* 显示湿度 */
                sprintf(acString, "     %02d%%", (UINT)g_stMSRWA.fHumi) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD1_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[0]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD2_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[1]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD3_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[2]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD4_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[3]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD5_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[4]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD6_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[5]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD7_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[6]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD8_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[7]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                /* 设定显示 */
                g_stLG1WA.uiLCD1UpdateView   = 1 ;
                g_stLG1WA.uiLCD2UpdateView   = 1 ;
                g_stLG1WA.uiLCD3UpdateView   = 1 ;
                g_stLG1WA.uiLCD4UpdateView   = 1 ;
                g_stLG2WA.uiLCD5UpdateView   = 1 ;
                g_stLG2WA.uiLCD6UpdateView   = 1 ;
                g_stLG2WA.uiLCD7UpdateView   = 1 ;
                g_stLG2WA.uiLCD8UpdateView   = 1 ;
                g_stLG1WA.uiLCD1UseSecondBuf = 0 ;
                g_stLG1WA.uiLCD2UseSecondBuf = 0 ;
                g_stLG1WA.uiLCD3UseSecondBuf = 0 ;
                g_stLG1WA.uiLCD4UseSecondBuf = 0 ;
                g_stLG2WA.uiLCD5UseSecondBuf = 0 ;
                g_stLG2WA.uiLCD6UseSecondBuf = 0 ;
                g_stLG2WA.uiLCD7UseSecondBuf = 0 ;
                g_stLG2WA.uiLCD8UseSecondBuf = 0 ; 
            }

            break ;
        case SHOW_TYPE_TIME :
        default :
            if(SHOW_TYPE_TIME            != g_stCTLWA.enPreviousShow || 
               stCurrentTime.RTC_Hours   != g_stCTLWA.stPreviousTime.RTC_Hours ||
               stCurrentTime.RTC_Minutes != g_stCTLWA.stPreviousTime.RTC_Minutes ||
               stCurrentTime.RTC_Seconds != g_stCTLWA.stPreviousTime.RTC_Seconds)
            {
                /* 两种情况刷新显示：1. 上次没有在显示时间，这次要显示时间；2. 时间发生改变。 */
                /* 显示时间 */
                sprintf(acString, "%02d:%02d:%02d", stCurrentTime.RTC_Hours, stCurrentTime.RTC_Minutes, stCurrentTime.RTC_Seconds) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD1_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[0]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD2_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[1]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD3_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[2]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD4_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[3]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD5_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[4]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD6_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[5]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD7_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[6]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                memcpy((VOID *)EGC_CONFIG_SDRAM_ADDR_LCD8_BUF1,
                       (VOID *)__EGC_CTL_ConvertChar2BufAddr(acString[7]),
                       EGC_LG1_LCD_FRAME_SIZE) ;

                /* 设定显示 */
                g_stLG1WA.uiLCD1UpdateView   = 1 ;
                g_stLG1WA.uiLCD2UpdateView   = 1 ;
                g_stLG1WA.uiLCD3UpdateView   = 1 ;
                g_stLG1WA.uiLCD4UpdateView   = 1 ;
                g_stLG2WA.uiLCD5UpdateView   = 1 ;
                g_stLG2WA.uiLCD6UpdateView   = 1 ;
                g_stLG2WA.uiLCD7UpdateView   = 1 ;
                g_stLG2WA.uiLCD8UpdateView   = 1 ;
                g_stLG1WA.uiLCD1UseSecondBuf = 0 ;
                g_stLG1WA.uiLCD2UseSecondBuf = 0 ;
                g_stLG1WA.uiLCD3UseSecondBuf = 0 ;
                g_stLG1WA.uiLCD4UseSecondBuf = 0 ;
                g_stLG2WA.uiLCD5UseSecondBuf = 0 ;
                g_stLG2WA.uiLCD6UseSecondBuf = 0 ;
                g_stLG2WA.uiLCD7UseSecondBuf = 0 ;
                g_stLG2WA.uiLCD8UseSecondBuf = 0 ; 
            }

            break ;
    }

    /* 记录信息，以决定下次是否更新显示 */
    g_stCTLWA.stPreviousTime = stCurrentTime ;
    g_stCTLWA.stPreviousDate = stCurrentDate ;
    g_stCTLWA.fPreviousTemp  = g_stMSRWA.fTemp ;
    g_stCTLWA.fPreviousHumi  = g_stMSRWA.fHumi ;
    g_stCTLWA.enPreviousShow = enShowReference ;

    return ;
}

/*******************************************************************************
- Function    : __EGC_CTL_ProcTouch
- Description : 本函数处理触摸事件。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_CTL_ProcTouch(VOID)
{
    if(1 == g_stMSRWA.uiTouchOccur)
    {
        g_stMSRWA.uiTouchOccur = 0 ;

        /* 第一种可能，如果在非显示时间段，则强制显示一分钟 */
        if(1 == g_stLG1WA.uiHide && 1 == g_stEGCConfigInfo.uiEnableTouchSense && 1 == g_stEGCConfigInfo.stTouchConfigInfo.ucTouchToShow)
        {
            g_stCTLWA.usForceShowDelay = EGC_CTL_FORCE_SHOW_PERIOD ;
        }

        /* 第二种可能，如果正在显示，则强制切换下一个画面，并维持一小段时间 */
        if(0 == g_stLG1WA.uiHide && 1 == g_stEGCConfigInfo.uiEnableTouchSense && 1 == g_stEGCConfigInfo.stTouchConfigInfo.ucTouchToSwitch)
        {
            if(0 != g_stCTLWA.usTouchSwitchDelay)
            {
                /* 上一次的触摸切换还未结束，所以需要切换下一个视图 */    
                switch(g_stCTLWA.enTouchShowType)
                {
                    case SHOW_TYPE_TIME :
                        g_stCTLWA.enTouchShowType = SHOW_TYPE_DATE ;
                        break ;
                    case SHOW_TYPE_DATE :
                        g_stCTLWA.enTouchShowType = SHOW_TYPE_TEMP ;
                        break ;
                    case SHOW_TYPE_TEMP :
                        g_stCTLWA.enTouchShowType = SHOW_TYPE_HUMD ;
                        break ;
                    case SHOW_TYPE_HUMD :
                    default :
                        g_stCTLWA.enTouchShowType = SHOW_TYPE_TIME ;
                        break ;
                }                
            }
            else
            {
                /* 一次新的触摸，切换到显示日期 */
                g_stCTLWA.enTouchShowType = SHOW_TYPE_DATE ;
            }

            g_stCTLWA.usTouchSwitchDelay = EGC_CTL_TOUCH_SWITCH_PERIOD ;
        }
    }

    g_stMSRWA.enTouchSenseLevel = g_stEGCConfigInfo.stTouchConfigInfo.enTouchSenseLevel ;

    return ;
}

/*******************************************************************************
- Function    : __EGC_CTL_UpdateWIFI
- Description : 本函数更新WIFI模块的工作信息。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID __EGC_CTL_UpdateWIFI(VOID)
{
    g_stESPWA.uiWIFIUpdateEnable = g_stEGCConfigInfo.uiEnableWIFIUpdateTime ;
    
    g_stESPWA.uiAP1Valid         = g_stEGCConfigInfo.stWIFIConfigInfo.stAP1Info.ucValid ;
    g_stESPWA.uiAP2Valid         = g_stEGCConfigInfo.stWIFIConfigInfo.stAP2Info.ucValid ;
    
    g_stESPWA.uiNTPServer1Valid  = g_stEGCConfigInfo.stWIFIConfigInfo.stNTPServer1.ucValid ;
    g_stESPWA.uiNTPServer2Valid  = g_stEGCConfigInfo.stWIFIConfigInfo.stNTPServer2.ucValid ;
    g_stESPWA.uiNTPServer3Valid  = g_stEGCConfigInfo.stWIFIConfigInfo.stNTPServer3.ucValid ;
    
    g_stESPWA.enUpdateInterval   = g_stEGCConfigInfo.stWIFIConfigInfo.enUpdateIntervel ;
    
    g_stESPWA.pcAP1SSID          = g_stEGCConfigInfo.stWIFIConfigInfo.stAP1Info.acSSID ;
    g_stESPWA.pcAP1Password      = g_stEGCConfigInfo.stWIFIConfigInfo.stAP1Info.acPassword ;
    
    g_stESPWA.pcAP2SSID          = g_stEGCConfigInfo.stWIFIConfigInfo.stAP2Info.acSSID ;
    g_stESPWA.pcAP2Password      = g_stEGCConfigInfo.stWIFIConfigInfo.stAP2Info.acPassword ;
    
    g_stESPWA.pcNTPServer1       = g_stEGCConfigInfo.stWIFIConfigInfo.stNTPServer1.acNTPServer ;
    g_stESPWA.pcNTPServer2       = g_stEGCConfigInfo.stWIFIConfigInfo.stNTPServer2.acNTPServer ;
    g_stESPWA.pcNTPServer3       = g_stEGCConfigInfo.stWIFIConfigInfo.stNTPServer3.acNTPServer ;

    return ;
}

/*******************************************************************************
- Function    : __EGC_CTL_DriveCount
- Description : 本函数驱动模块内定时任务。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __EGC_CTL_DriveCount(VOID)
{
    if(g_stCTLWA.usForceShowDelay >= EGC_CTL_TASK_DELAY)
    {
        g_stCTLWA.usForceShowDelay -= EGC_CTL_TASK_DELAY ;
        g_stCTLWA.uiForceShow       = 1 ;
    }
    else
    {
        g_stCTLWA.usForceShowDelay = 0 ;
        g_stCTLWA.uiForceShow      = 0 ;
    }

    if(g_stCTLWA.usTouchSwitchDelay >= EGC_CTL_TASK_DELAY)
    {
        g_stCTLWA.usTouchSwitchDelay -= EGC_CTL_TASK_DELAY ;
        g_stCTLWA.uiTouchShow       = 1 ;
    }
    else
    {
        g_stCTLWA.usTouchSwitchDelay = 0 ;
        g_stCTLWA.uiTouchShow      = 0 ;
    }
    
    return ;
}

CODE_SECTION("==========================") ;
CODE_SECTION("==  CTL模块任务函数     ==") ;
CODE_SECTION("==========================") ;
/*******************************************************************************
- Function    : EGC_CTL_Task
- Description : 本函数为CTL模块的任务函数。
- Input       : VOID
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID EGC_CTL_Task(VOID)
{
    OS_ERR enErr ;

    /* 模块初始化 */
    __EGC_CTL_Init() ;

    /* 等待HOST模块初始化完毕 */
    while(1 != g_stHSTWA.uiInitDone || 1 != g_stLG1WA.uiInitDone || 1 != g_stLG2WA.uiInitDone || 1 != g_stESPWA.uiInitDone || 1 != g_stMSRWA.uiInitDone)
    {
        OSTimeDlyHMSM((CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT32U   )10u,
                      (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                      (OS_ERR      *)&enErr);              
    }

    /* 启动其他模块 */
    g_stHSTWA.uiTaskRun = 1 ; /* 启动HST(Host         )模块工作 */
    g_stLG1WA.uiTaskRun = 1 ; /* 启动LG1(LCD Group 1)模块工作 */
    g_stLG2WA.uiTaskRun = 1 ; /* 启动LG2(LCD Group 2)模块工作 */
    g_stESPWA.uiTaskRun = 1 ; /* 启动ESP(WIFI         )模块工作 */
    g_stMSRWA.uiTaskRun = 1 ; /* 启动MSR(Measure    )模块工作 */

    while(DEF_TRUE)
    {
        /**********************************************************************************************************
         * 1、通用设置 
         **********************************************************************************************************/
        g_stLG1WA.enBrightnessLevel = g_stEGCConfigInfo.enLCDBrightness ; /* 亮度调节 */

        /**********************************************************************************************************
         * 2、显示内容刷新 
         **********************************************************************************************************/
        __EGC_CTL_UpdateShow() ;

        /**********************************************************************************************************
         * 3、检查是否有触碰事件发生，并处理触碰事件
         **********************************************************************************************************/
        __EGC_CTL_ProcTouch() ;

        /**********************************************************************************************************
         * 4、更新WIFI工作信息
         **********************************************************************************************************/
        __EGC_CTL_UpdateWIFI() ;

        /**********************************************************************************************************
         * 5、检查配置信息是否被修改。如果被修改了，要重新加载配置信息和字体文件
         **********************************************************************************************************/
         if(1 == g_stHSTWA.uiConfigUpdateByHost)
         {
            g_stHSTWA.uiConfigUpdateByHost = 0 ;

            __EGC_CTL_GetUserConfig() ;
         }

        /* 周期性任务 */
        __EGC_CTL_DriveCount() ;

        /* 任务延时 */
        OSTimeDlyHMSM((CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT16U   )0u, 
                      (CPU_INT32U   )EGC_CTL_TASK_DELAY,
                      (OS_OPT       )OS_OPT_TIME_HMSM_STRICT,
                      (OS_ERR      *)&enErr);       
    }
}


/*******************************************************************************
- Function    : EGC_CTL_SPIIntHandler
- Description : 本函数为CTL模块所使用的SPI外设的中断服务函数。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
volatile UINT uiSPIIntCtr = 0 ;
volatile UINT uiSPIDMAIntCtr = 0 ;
VOID EGC_CTL_SPIIntHandler(VOID)
{
    uiSPIIntCtr++ ;
    DRV_SPI_GeneralIntHandler(&g_stW25QSPIDevice) ;
    
    return ;
}


/*******************************************************************************
- Function    : EGC_CTL_SPIDMAIntHandler
- Description : 本函数为CTL模块所使用的SPI外设的DMA中断服务函数。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID EGC_CTL_SPIDMAIntHandler(VOID)
{
    uiSPIDMAIntCtr++;
    DRV_SPI_GeneralDMAIntHandler(&g_stW25QSPIDevice) ;

    return ;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file EGC_CTL.c. *******/  
