/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : EGC_ESP.h
    Project name : EGC
    Module name  : 
    Date created : 2021年08月18日    15时26分4秒
    Author       : Ning.JianLi
    Description  : ESP模块头文件。
*******************************************************************************/

#ifndef __EGC_ESP_H__
#define __EGC_ESP_H__

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "pub_type.h"
#include "pub_lib.h"


#include "EGC_Config.h"

#pragma anon_unions
#pragma pack(1)

CODE_SECTION("==========================") ;
CODE_SECTION("==  ESP模块任务参数定义 ==") ;
CODE_SECTION("==========================") ;


CODE_SECTION("==========================") ;
CODE_SECTION("==  其他复杂数据类型定义==") ;
CODE_SECTION("==========================") ;
/* ESP模块工作区定义 */
typedef struct tagEGC_ESP_WORKAREA
{
    union
    {
        UCHAR aucRawData[40] ;
        
        struct
        {
            /* DW0 */ /* FLAG ONLY */
            UINT                          uiTaskRun           : 1 ;  /* 模块任务启动标记，外部模块读写，内部模块只读 */
            UINT                          uiInitDone          : 1 ;  /* 初始化完成标记，外部模块只读，内部模块读写 */
            UINT                          uiWIFIUpdateEnable  : 1 ;  /* 使能WIFI时间更新，外部模块置位，内部模块读并复位 */
            UINT                          uiAP1Valid          : 1 ;
            UINT                          uiAP2Valid          : 1 ;
            UINT                          uiNTPServer1Valid   : 1 ;
            UINT                          uiNTPServer2Valid   : 1 ;
            UINT                          uiNTPServer3Valid   : 1 ;

            UINT                          uiByte1Rsv0         : 8 ;
            UINT                          uiByte2Rsv0         : 8 ;
            UINT                          uiByte3Rsv0         : 8 ;

            /* DW1 */ /* 仅用作错误码 */
            UINT                          uiErrCode ;
            
            /* DW2 */
            EGC_CONFIG_WIFI_UPDATE_TIME_E enUpdateInterval ;

            /* DW3 */
            CHAR *                        pcAP1SSID ;

            /* DW4 */
            CHAR *                        pcAP1Password ;

            /* DW5 */
            CHAR *                        pcAP2SSID ;

            /* DW6 */
            CHAR *                        pcAP2Password ;

            /* DW7 */
            CHAR *                        pcNTPServer1 ;

            /* DW8 */
            CHAR *                        pcNTPServer2 ;

            /* DW9 */
            CHAR *                        pcNTPServer3 ;
        } ;
    } ;
}EGC_ESP_WORKAREA_S ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  函数声明            ==") ;
CODE_SECTION("==========================") ;

VOID EGC_ESP_Task(VOID) ;
VOID EGC_ESP_ESP32UARTIntHandler(VOID) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __EGC_ESP_H__ */

/******* End of file EGC_ESP.h. *******/  
