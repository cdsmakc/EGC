/*******************************************************************************
Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
File name    : FireFlyComm.h
Project name : GAirTestTool
Module name  :
Date created : 2020��02��10��   17ʱ15��39��
Author       : Ning.JianLi
Description  : GAirTestTool����ͨ��ģ��ͷ�ļ���
               ��ģ������ṩ���µĽӿڣ�
                 1. �ṩͨ�Ŷ˿ڳ�ʼ���Ľӿڡ�
                 2. �ṩģ���̺߳�����
               ��ģ��ʵ�ֵĹ��ܣ�
                 1. ��ͨ�Ŷ˿ڻ�ȡ���ݣ������н�����L2�������Щ������У�飬Ȼ�����L2�㣬
                    ��ͨ����Ϣ�͵��Ի����̡߳�
                 2. �ӶԻ����̻߳�ȡ��Ϣ�������L2�������Щ���������L2��֮����Ϊ����
                    ���͵�ͨ�Ŷ˿ڡ�
               ע�⣺
                 1. ��ģ����漰L2��Ϣ������L2�����Ӻ�ɾ���������ڲ㲻���κδ�����Ϊ
                    �ڲ���������ҵ��ģ�飬��ͨ���޹أ���ģ�������ͨ�����ҵ��
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
CODE_SECTION("==  �궨��                              ==");
CODE_SECTION("=========================================");

// L2֡ͷ����
#define FFY_COMM_MSG_L2_HEAD_BYTE0               (0xEB)
#define FFY_COMM_MSG_L2_HEAD_BYTE1               (0x90)

// ��ϢĿ�ĵض���
#define FFY_COMM_MSG_DST_F429                    (0x02)
#define FFY_COMM_MSG_DST_HOST                    (0x03)

// ��Ϣ���Ͷ���
#define FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG1       (0x81)     /* �����·�����ȡ������Ϣ��1 (128B) */
#define FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG2       (0x82)     /* �����·�����ȡ������Ϣ��2 (128B) */
#define FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG3       (0x83)     /* �����·�����ȡ������Ϣ��3 (128B) */
#define FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG4       (0x84)     /* �����·�����ȡ������Ϣ��4 (128B) */
#define FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG5       (0x85)     /* �����·�����ȡ������Ϣ��5 (128B) */
#define FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG6       (0x86)     /* �����·�����ȡ������Ϣ��6 (128B) */
#define FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG7       (0x87)     /* �����·�����ȡ������Ϣ��7 (128B) */
#define FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG8       (0x88)     /* �����·�����ȡ������Ϣ��8 (128B) */

#define FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG1       (0x89)     /* �����·���д��������Ϣ��1 (128B) */
#define FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG2       (0x8A)     /* �����·���д��������Ϣ��2 (128B) */
#define FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG3       (0x8B)     /* �����·���д��������Ϣ��3 (128B) */
#define FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG4       (0x8C)     /* �����·���д��������Ϣ��4 (128B) */
#define FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG5       (0x8D)     /* �����·���д��������Ϣ��5 (128B) */
#define FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG6       (0x8E)     /* �����·���д��������Ϣ��6 (128B) */
#define FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG7       (0x8F)     /* �����·���д��������Ϣ��7 (128B) */
#define FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG8       (0x90)     /* �����·���д��������Ϣ��8 (128B) */

#define FFY_HST_L2_TYPE_CMD_UPDATE_TIME          (0x91)     /* �����·�������ʱ�� */
#define FFY_HST_L2_TYPE_CMD_WR_FLASH             (0x92)     /* �����·���дFlash���� */

#define FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG1       (0xC1)     /* F4XX��Ӧ���ر�������Ϣ��1 (128B) */
#define FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG2       (0xC2)     /* F4XX��Ӧ���ر�������Ϣ��2 (128B) */
#define FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG3       (0xC3)     /* F4XX��Ӧ���ر�������Ϣ��3 (128B) */
#define FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG4       (0xC4)     /* F4XX��Ӧ���ر�������Ϣ��4 (128B) */
#define FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG5       (0xC5)     /* F4XX��Ӧ���ر�������Ϣ��5 (128B) */
#define FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG6       (0xC6)     /* F4XX��Ӧ���ر�������Ϣ��6 (128B) */
#define FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG7       (0xC7)     /* F4XX��Ӧ���ر�������Ϣ��7 (128B) */
#define FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG8       (0xC8)     /* F4XX��Ӧ���ر�������Ϣ��8 (128B) */

#define FFY_HST_L2_TYPE_CMD_RT_LOG               (0xC9)     /* F4XX����������������ʵʱ��־ */
#define FFY_HST_L2_TYPE_CMD_RT_STATE             (0xCA)     /* F4XX����������������ʵʱ״̬ */

CODE_SECTION("=========================================");
CODE_SECTION("==  �����������Ͷ���                     ==");
CODE_SECTION("=========================================");

// ���½ṹ�����Ͷ����˶Ի����̵߳������߳�֮����Ϣ�ĸ�ʽ
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
            UCHAR ucMsgLen;    // ��Ϣ����
            UCHAR aucMsg[256]; // ��Ϣָ��
        };
    };
} GTT_COMM_MSG_DLG2COMM_S;

// ���½ṹ�����Ͷ����˴����̵߳��Ի����߳�֮����Ϣ�ĸ�ʽ
typedef struct tagGTT_COMM_MSG_COMM2DLG
{
    union
    {
        UCHAR aucRawData[259];

        struct
        {
            UCHAR ucValid;
            UCHAR ucMsgType;
            UCHAR ucMsgLen;    // ��Ϣ����
            UCHAR aucMsg[256]; // ��Ϣָ��
        };
    };
} GTT_COMM_MSG_COMM2DLG_S;



// ���½ṹ�����Ͷ�������Ϣ��L2��ʽ
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
            UCHAR ucLen; //�ڲ㳤��
        };
    };
}MSG_L2_HEAD_S;

CODE_SECTION("=========================================");
CODE_SECTION("==  ��������                            ==");
CODE_SECTION("=========================================");

wchar_t * char2wchar(const char* cchar);
VOID FFY_COMM_ListComm2ComboBox(CComboBox *pComboBox);
UINT FFY_COMM_ProcessCommThread(LPVOID pParam);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file FireFlyComm.h. *******/
