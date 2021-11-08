/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : EGC_CTL.h
    Project name : EGC
    Module name  : 
    Date created : 2020年02月12日    16时50分47秒
    Author       : Ning.JianLi
    Description  : Ctrl模块头文件。
*******************************************************************************/

#ifndef __EGC_CTL_H__
#define __EGC_CTL_H__

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "pub_type.h"
#include "pub_lib.h"

#pragma anon_unions

CODE_SECTION("==========================") ;
CODE_SECTION("==  简单宏定义          ==") ;
CODE_SECTION("==========================") ;
#define EGC_CTL_TASK_DELAY                 (100u)          /* 任务循环时间 */

#define EGC_CTL_FORCE_SHOW_PERIOD          (60000u)        /* 触碰显示时的显示时间 */
#define EGC_CTL_TOUCH_SWITCH_PERIOD        (5000u)         /* 触碰切换显示的时间 */


CODE_SECTION("==========================") ;
CODE_SECTION("==  复杂数据类型定义    ==") ;
CODE_SECTION("==========================") ;
/* 以下结构体类型定义了用户配置信息 */
typedef struct tagEGC_CTL_USER_CFG_INFO
{
    union
    {
        UCHAR aucRawData[512] ;

        struct
        {
            /* 第一组WIFI信息 */
            CHAR acWIFI1SSID[40] ;
            CHAR acWIFI1Password[40] ;

            /* 第二组WIFI信息 */
            CHAR acWIFI2SSID[40] ;
            CHAR acWIFI2Password[40] ;
        } ;
    } ;
}EGC_CTL_USER_CFG_INFO_S ;

/* 以下枚举类型定义了显示类型 */
typedef enum tagEGC_CTL_SHOW_TYPE
{
    SHOW_TYPE_TIME  = 0 ,
    SHOW_TYPE_DATE  = 1 ,
    SHOW_TYPE_TEMP  = 2 ,
    SHOW_TYPE_HUMD  = 3
}EGC_CTL_SHOW_TYPE_E ;

/* 以下结构体类型定义了CTL模块的工作区 */
typedef struct tagEGC_CTL_WORKAREA
{
    union
    {
        UCHAR aucRawData[36] ;
        
        struct
        {
            /* DW0 */ /* FLAG ONLY */
            UINT                      uiByte0Rsv0     : 8 ;

            UINT                      uiForceShow     : 1 ;
            UINT                      uiTouchShow     : 1 ;
            UINT                      uiByte1Rsv0     : 6 ;

            UINT                      uiByte2Rsv0     : 8 ;

            UINT                      uiByte3Rsv0     : 8 ;

            /* DW1 */ /* 仅用作错误码 */
            UINT                      uiErrCode ;

            /* DW2 */
            EGC_CTL_SHOW_TYPE_E       enShowType      : 8 ;
            EGC_CTL_SHOW_TYPE_E       enTouchShowType : 8 ;
            UCHAR                     ucRsv2 ;
            UCHAR                     ucRsv3 ;

            /* DW3 */
            USHORT                    usForceShowDelay ;
            USHORT                    usTouchSwitchDelay ;

            /* DW4 */
            RTC_TimeTypeDef           stPreviousTime ;     /* 上一次处理时的时间，用以判断是否需要更新显示 */

            /* DW5 */
            RTC_DateTypeDef           stPreviousDate ;     /* 上一次处理时的日期，用以判断是否需要更新显示 */

            /* DW6 */
            FLOAT                     fPreviousTemp ;      /* 上一次处理时的温度，用以判断是否需要更新显示 */

            /* DW7 */
            FLOAT                     fPreviousHumi ;      /* 上一次处理时的湿度，用以判断是否需要更新显示 */

            /* DW8 */
            EGC_CTL_SHOW_TYPE_E       enPreviousShow : 8 ; /* 上一次显示类型，用以判断是否需要更新显示 */
            UINT                      uiRsv0         : 24 ;
        } ;
    } ;
}EGC_CTL_WORKAREA_S ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  函数声明            ==") ;
CODE_SECTION("==========================") ;
VOID EGC_CTL_Task(VOID) ;
VOID EGC_CTL_SPIIntHandler(VOID) ;
VOID EGC_CTL_SPIDMAIntHandler(VOID) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __EGC_CTL_H__ */

/******* End of file EGC_CTL.h. *******/  
