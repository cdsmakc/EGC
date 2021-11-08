/*******************************************************************************
Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
File name    : FireFlyComm.h
Project name : GAirTestTool
Module name  :
Date created : 2020年02月10日   17时15分39秒
Author       : Ning.JianLi
Description  : GAirTestTool工具通信模块头文件。
               本模块对外提供如下的接口：
                 1. 提供通信端口初始化的接口。
                 2. 提供模块线程函数。
               本模块实现的功能：
                 1. 从通信端口获取数据，并从中解析出L2层包。这些包将被校验，然后剥除L2层，
                    并通过消息送到对话框线程。
                 2. 从对话框线程获取消息，并添加L2层包。这些包在添加了L2层之后，作为数据
                    发送到通信端口。
               注意：
                 1. 本模块仅涉及L2信息，处理L2层的添加和删除，对于内层不做任何处理。因为
                    内层数据属于业务模块，和通信无关，本模块仅处理通信相关业务。
*******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#pragma once

#include "stdafx.h"
#include "typedef.h"
#include "FireFlyDlg.h"

#ifndef CODE_SECTION
#define CODE_SECTION(Discription)
#endif

CODE_SECTION("=========================================");
CODE_SECTION("==  宏定义                              ==");
CODE_SECTION("=========================================");

// L2帧头定义
#define FFY_COMM_MSG_L2_HEAD_BYTE0               (0xEB)
#define FFY_COMM_MSG_L2_HEAD_BYTE1               (0x90)

// 消息目的地定义
#define FFY_COMM_MSG_DST_F429                    (0x02)
#define FFY_COMM_MSG_DST_HOST                    (0x03)

// 消息类型定义
#define FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG1       (0x81)     /* 主机下发，读取配置信息段1 (128B) */
#define FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG2       (0x82)     /* 主机下发，读取配置信息段2 (128B) */
#define FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG3       (0x83)     /* 主机下发，读取配置信息段3 (128B) */
#define FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG4       (0x84)     /* 主机下发，读取配置信息段4 (128B) */
#define FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG5       (0x85)     /* 主机下发，读取配置信息段5 (128B) */
#define FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG6       (0x86)     /* 主机下发，读取配置信息段6 (128B) */
#define FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG7       (0x87)     /* 主机下发，读取配置信息段7 (128B) */
#define FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG8       (0x88)     /* 主机下发，读取配置信息段8 (128B) */

#define FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG1       (0x89)     /* 主机下发，写入配置信息段1 (128B) */
#define FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG2       (0x8A)     /* 主机下发，写入配置信息段2 (128B) */
#define FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG3       (0x8B)     /* 主机下发，写入配置信息段3 (128B) */
#define FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG4       (0x8C)     /* 主机下发，写入配置信息段4 (128B) */
#define FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG5       (0x8D)     /* 主机下发，写入配置信息段5 (128B) */
#define FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG6       (0x8E)     /* 主机下发，写入配置信息段6 (128B) */
#define FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG7       (0x8F)     /* 主机下发，写入配置信息段7 (128B) */
#define FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG8       (0x90)     /* 主机下发，写入配置信息段8 (128B) */

#define FFY_HST_L2_TYPE_CMD_UPDATE_TIME          (0x91)     /* 主机下发，更新时间 */
#define FFY_HST_L2_TYPE_CMD_WR_FLASH             (0x92)     /* 主机下发，写Flash操作 */

#define FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG1       (0xC1)     /* F4XX响应，回报配置信息段1 (128B) */
#define FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG2       (0xC2)     /* F4XX响应，回报配置信息段2 (128B) */
#define FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG3       (0xC3)     /* F4XX响应，回报配置信息段3 (128B) */
#define FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG4       (0xC4)     /* F4XX响应，回报配置信息段4 (128B) */
#define FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG5       (0xC5)     /* F4XX响应，回报配置信息段5 (128B) */
#define FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG6       (0xC6)     /* F4XX响应，回报配置信息段6 (128B) */
#define FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG7       (0xC7)     /* F4XX响应，回报配置信息段7 (128B) */
#define FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG8       (0xC8)     /* F4XX响应，回报配置信息段8 (128B) */

#define FFY_HST_L2_TYPE_CMD_RT_LOG               (0xC9)     /* F4XX发出，向主机发送实时日志 */
#define FFY_HST_L2_TYPE_CMD_RT_STATE             (0xCA)     /* F4XX发出，向主机发出实时状态 */

CODE_SECTION("=========================================");
CODE_SECTION("==  复杂数据类型定义                     ==");
CODE_SECTION("=========================================");

// 以下结构体类型定义了对话框线程到处理线程之间消息的格式
typedef struct tagGTT_COMM_MSG_DLG2COMM
{
    union
    {
        UCHAR aucRawData[259];

        struct
        {
            UCHAR ucValid;
            UCHAR ucMsgDst;
            UCHAR ucMsgType;
            UCHAR ucMsgLen;    // 消息长度
            UCHAR aucMsg[256]; // 消息指针
        };
    };
} GTT_COMM_MSG_DLG2COMM_S;

// 以下结构体类型定义了处理线程到对话框线程之间消息的格式
typedef struct tagGTT_COMM_MSG_COMM2DLG
{
    union
    {
        UCHAR aucRawData[259];

        struct
        {
            UCHAR ucValid;
            UCHAR ucMsgType;
            UCHAR ucMsgLen;    // 消息长度
            UCHAR aucMsg[256]; // 消息指针
        };
    };
} GTT_COMM_MSG_COMM2DLG_S;



// 以下结构体类型定义了消息的L2格式
typedef struct tagMSG_L2_HEAD
{
    union
    {
        UCHAR aucRawData[5];

        struct
        {
            UCHAR ucHead0;
            UCHAR ucHead1;
            UCHAR ucDst;
            UCHAR ucType;
            UCHAR ucLen; //内层长度
        };
    };
}MSG_L2_HEAD_S;

CODE_SECTION("=========================================");
CODE_SECTION("==  函数声明                            ==");
CODE_SECTION("=========================================");

wchar_t * char2wchar(const char* cchar);
VOID FFY_COMM_ListComm2ComboBox(CComboBox *pComboBox);
UINT FFY_COMM_ProcessCommThread(LPVOID pParam);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file FireFlyComm.h. *******/
