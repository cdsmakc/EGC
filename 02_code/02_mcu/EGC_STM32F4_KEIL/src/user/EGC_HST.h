/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : EGC_HST.h
    Project name : EGC
    Module name  : 
    Date created : 2020年2月12日   17时2分50秒
    Author       : Ning.JianLi
    Description  : Host模块头文件。
*******************************************************************************/

#ifndef __EGC_HST_H__
#define __EGC_HST_H__

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "pub_type.h"
#include "pub_lib.h"

#pragma anon_unions

CODE_SECTION("==========================") ;
CODE_SECTION("==  HST模块TASK工作参数 ==") ;
CODE_SECTION("==========================") ;
#define EGC_HST_TASK_CYCLE_GAP_IN_MS             (10u)   /* Task循环时间，以毫秒为单位 */


CODE_SECTION("==========================") ;
CODE_SECTION("==  HST模块常量定义     ==") ;
CODE_SECTION("==========================") ;

/* 与主机交互的帧头定义 */
#define EGC_HST_L2_MAX_SIZE                      (256u)                        /* 链路层最大长度 */
#define EGC_HST_L2_HEAD1                         (0xEB)
#define EGC_HST_L2_HEAD2                         (0x90)
#define EGC_HST_L2_L7LEN_MAX_SIZE                (EGC_HST_L2_MAX_SIZE - 5 - 2) /* 负载最大长度 */

/* 目的地类型 */
#define EGC_HST_L2_DST_F429                      (0x02)
#define EGC_HST_L2_DST_HOST                      (0x03)

/* 消息类型定义 */
#define EGC_HST_L2_TYPE_CMD_RD_CONFIG_SEG1       (0x81)     /* 主机下发，读取配置信息段1 (128B) */
#define EGC_HST_L2_TYPE_CMD_RD_CONFIG_SEG2       (0x82)     /* 主机下发，读取配置信息段2 (128B) */
#define EGC_HST_L2_TYPE_CMD_RD_CONFIG_SEG3       (0x83)     /* 主机下发，读取配置信息段3 (128B) */
#define EGC_HST_L2_TYPE_CMD_RD_CONFIG_SEG4       (0x84)     /* 主机下发，读取配置信息段4 (128B) */
#define EGC_HST_L2_TYPE_CMD_RD_CONFIG_SEG5       (0x85)     /* 主机下发，读取配置信息段5 (128B) */
#define EGC_HST_L2_TYPE_CMD_RD_CONFIG_SEG6       (0x86)     /* 主机下发，读取配置信息段6 (128B) */
#define EGC_HST_L2_TYPE_CMD_RD_CONFIG_SEG7       (0x87)     /* 主机下发，读取配置信息段7 (128B) */
#define EGC_HST_L2_TYPE_CMD_RD_CONFIG_SEG8       (0x88)     /* 主机下发，读取配置信息段8 (128B) */

#define EGC_HST_L2_TYPE_CMD_WR_CONFIG_SEG1       (0x89)     /* 主机下发，写入配置信息段1 (128B) */
#define EGC_HST_L2_TYPE_CMD_WR_CONFIG_SEG2       (0x8A)     /* 主机下发，写入配置信息段2 (128B) */
#define EGC_HST_L2_TYPE_CMD_WR_CONFIG_SEG3       (0x8B)     /* 主机下发，写入配置信息段3 (128B) */
#define EGC_HST_L2_TYPE_CMD_WR_CONFIG_SEG4       (0x8C)     /* 主机下发，写入配置信息段4 (128B) */
#define EGC_HST_L2_TYPE_CMD_WR_CONFIG_SEG5       (0x8D)     /* 主机下发，写入配置信息段5 (128B) */
#define EGC_HST_L2_TYPE_CMD_WR_CONFIG_SEG6       (0x8E)     /* 主机下发，写入配置信息段6 (128B) */
#define EGC_HST_L2_TYPE_CMD_WR_CONFIG_SEG7       (0x8F)     /* 主机下发，写入配置信息段7 (128B) */
#define EGC_HST_L2_TYPE_CMD_WR_CONFIG_SEG8       (0x90)     /* 主机下发，写入配置信息段8 (128B) */

#define EGC_HST_L2_TYPE_CMD_UPDATE_TIME          (0x91)     /* 主机下发，更新时间 */
#define EGC_HST_L2_TYPE_CMD_WR_FLASH             (0x92)     /* 主机下发，写Flash操作 */

#define EGC_HST_L2_TYPE_RPL_RD_CONFIG_SEG1       (0xC1)     /* F4XX响应，回报配置信息段1 (128B) */
#define EGC_HST_L2_TYPE_RPL_RD_CONFIG_SEG2       (0xC2)     /* F4XX响应，回报配置信息段2 (128B) */
#define EGC_HST_L2_TYPE_RPL_RD_CONFIG_SEG3       (0xC3)     /* F4XX响应，回报配置信息段3 (128B) */
#define EGC_HST_L2_TYPE_RPL_RD_CONFIG_SEG4       (0xC4)     /* F4XX响应，回报配置信息段4 (128B) */
#define EGC_HST_L2_TYPE_RPL_RD_CONFIG_SEG5       (0xC5)     /* F4XX响应，回报配置信息段5 (128B) */
#define EGC_HST_L2_TYPE_RPL_RD_CONFIG_SEG6       (0xC6)     /* F4XX响应，回报配置信息段6 (128B) */
#define EGC_HST_L2_TYPE_RPL_RD_CONFIG_SEG7       (0xC7)     /* F4XX响应，回报配置信息段7 (128B) */
#define EGC_HST_L2_TYPE_RPL_RD_CONFIG_SEG8       (0xC8)     /* F4XX响应，回报配置信息段8 (128B) */

#define EGC_HST_L2_TYPE_CMD_RT_LOG               (0xC9)     /* F4XX发出，向主机发送实时日志 */
#define EGC_HST_L2_TYPE_CMD_RT_STATE             (0xCA)     /* F4XX发出，向主机发出实时状态 */

/* 配置信息分段大小 */
#define EGC_HST_CONFI_INFO_SEGMENT_SIZE          (128u)

CODE_SECTION("==========================") ;
CODE_SECTION("==  复杂数据类型定义    ==") ;
CODE_SECTION("==========================") ;
/* 以下枚举类型定义了配置信息的各个段 */
typedef enum tagEGC_HST_CONFIG_INFO_SEGMENT
{
    CONFIG_INFO_SEGMENT_1 = 0 ,
    CONFIG_INFO_SEGMENT_2 ,
    CONFIG_INFO_SEGMENT_3 ,
    CONFIG_INFO_SEGMENT_4 ,
    CONFIG_INFO_SEGMENT_5 ,
    CONFIG_INFO_SEGMENT_6 ,
    CONFIG_INFO_SEGMENT_7 ,
    CONFIG_INFO_SEGMENT_8
} EGC_HST_CONFIG_INFO_SEGMENT_E ;

/* 以下结构体类型定义了429的通信帧的帧头部 */
#pragma pack(1)
typedef struct tagEGC_HST_MSG_L2_HEAD
{
    union
    {
        UCHAR aucRawData[5] ;

        struct
        {
            UCHAR ucHead1 ;
            UCHAR ucHead2 ;
            UCHAR ucDst ;
            UCHAR ucType ;
            UCHAR ucL7Len ;
        } ;
    } ;
} EGC_HST_MSG_L2_HEAD_S ;
#pragma pack()

/* 以下结构体定义了HST模块工作区 */
typedef struct tagEGC_HST_WORKAREA
{
    union
    {
        UCHAR aucRawData[8] ;

        struct
        {
            /* DW0 */ /* 仅用作标记 */
            UINT                       uiTaskRun             : 1 ;  /* 模块任务启动标记，外部模块读写，内部模块只读 */
            UINT                       uiInitDone            : 1 ;  /* 初始化完成标记，外部模块只读，内部模块读写 */
            UINT                       uiByte0Rsv0           : 6 ;  /* 字节0的保留部分 */

            UINT                       uiConfigUpdateByHost  : 1 ;  /* 配置信息被上位机修改。内部模块置位，外部模块清除 */
            UINT                       uiByte1Rsv0           : 7 ;  /* 字节1的保留部分 */
            
            UINT                       uiByte2Rsv0           : 8 ;  /* 字节2的保留部分 */
            
            UINT                       uiByte3RSV0           : 8 ;  /* 字节3的保留部分 */

            /* DW1 */ /* 仅用作错误码 */
            UINT                       uiErrCode ;
        } ;
    } ;
} EGC_HST_WORKAREA_S ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  模块函数声明        ==") ;
CODE_SECTION("==========================") ;
VOID EGC_HST_Task(VOID) ;
VOID EGC_HST_UARTInterruptHandler(VOID) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __EGC_HST_H__ */

/******* End of file EGC_HST.h. *******/  
