/*******************************************************************************
Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
File name    : FireFlyComm.cpp
Project name : GAirTestTool
Module name  :
Date created : 2020年02月10日   17时15分39秒
Author       : Ning.JianLi
Description  : GAirTestTool工具通信模块源文件。
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

#include "stdafx.h"
#include "FireFlyComm.h"

// 通信端口文件句柄
extern HANDLE                  g_hUART;

// LOG显示Buf
extern CHAR                    g_acShowLogBuf[16384];
extern UINT                    g_uiShowLogBufPtr;

// 线程间消息缓存
extern GTT_COMM_MSG_DLG2COMM_S g_stDlg2CommMsg;
extern GTT_COMM_MSG_COMM2DLG_S g_stComm2DlgMsg;

// 通告线程结束标记
extern bool                    g_bNotifyStopThread;
extern bool                    g_bThreadStopped;

/*******************************************************************************
- Function    : char2wchar
- Description : 将char型字符串转换为wchar_t类型字符串。
- Input       : --
- Output      : --
- Return      : --
- Others      :
*******************************************************************************/
wchar_t * char2wchar(const char* cchar)
{
    wchar_t *m_wchar;
    int len = MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), NULL, 0);
    m_wchar = new wchar_t[len + 1];
    MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), m_wchar, len);
    m_wchar[len] = '\0';
    return m_wchar;
}

/*******************************************************************************
- Function    : FFY_COMM_ListComm2ComboBox
- Description : 通过注册表查询系统的串口，并将串口显示在ComboBox控件的选项中。
- Input       : --
- Output      : --
- Return      : --
- Others      :
*******************************************************************************/
VOID FFY_COMM_ListComm2ComboBox(CComboBox *pComboBox)
{
    HKEY  hKey;
    LONG  nRetVal;
    CHAR  acPortName[256];
    CHAR  acCOMMName[256];
    DWORD dwLong;
    DWORD dwSize;

    // 打开注册表项目
    nRetVal = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        _T("Hardware\\DeviceMap\\SerialComm"),
        NULL,
        KEY_WOW64_64KEY | KEY_READ,
        &hKey);

    if (ERROR_SUCCESS == nRetVal)
    {
        int i = 0;

        while (1)
        {
            dwLong = dwSize = sizeof(acPortName);

            nRetVal = RegEnumValue(hKey, i, (LPWSTR)acPortName, &dwLong, NULL, NULL, (PUCHAR)acCOMMName, &dwSize);

            if (nRetVal == ERROR_NO_MORE_ITEMS)
            {
                break;
            }

            CString strCommName;

            strCommName.Format(_T("%s"), acCOMMName);

            pComboBox->AddString(strCommName);
            i++;
        }

        if (pComboBox->GetCount() == 0)
        {
            AfxMessageBox(_T("[HKEY_LOCAL_MACHINE:Hardware\\DeviceMap\\SerialComm]路径下没有串口。"));
        }
        RegCloseKey(hKey);
    }
}

USHORT __FFY_COMM_PCT_CalcChkSum(UCHAR *pucBuf, UINT uiInnerSize)
{
    UINT   uiLoop;
    USHORT usRetVal = 0;

    for (uiLoop = 0; uiLoop < (uiInnerSize + sizeof(MSG_L2_HEAD_S)); uiLoop++)
    {
        // 长度不包含帧头，目的地，类型，长度字节，以及校验和本身，但是校验和要全部包含
        usRetVal += pucBuf[uiLoop];
    }
    
    return usRetVal;
}

USHORT __FFY_COMM_PCT_GetFrameChkSum(UCHAR *pucBuf, UINT uiInnerSize)
{
    USHORT usRetVal;

    usRetVal = pucBuf[sizeof(MSG_L2_HEAD_S) + uiInnerSize] + pucBuf[sizeof(MSG_L2_HEAD_S) + uiInnerSize + 1] * 256;

    return usRetVal;
}

/*******************************************************************************
- Function    : FFY_COMM_ProcessCommThread
- Description : 通过注册表查询系统的串口，并将串口显示在ComboBox控件的选项中。
- Input       : --
- Output      : --
- Return      : --
- Others      :
*******************************************************************************/
UINT FFY_COMM_ProcessCommThread(LPVOID pParam)
{
    UINT                   uiLoop;
    COMMTIMEOUTS           stTimeOut;
    CHAR                   acReadBuf[256];
    CHAR                   acWriteBuf[256];
    CHAR                   acProcessBuf[256];
    DWORD                  dwReadSize;
    DWORD                  dwWriteSize;
    CHAR                   acReadAccumulateBuf[8192];
    UINT                   uiReadAccumulateBufEnd = 0;
    MSG_L2_HEAD_S         *pstMsgL2Head;
    USHORT                 usCalcChkSum, usFrameChkSum;
	CFireFlyDlg           *pDlg = (CFireFlyDlg *)pParam;
    UINT                   uiCopyLen;

    // ****************************************************
    // 配置读写超时
    // ****************************************************

    // 超时参数设置
    stTimeOut.ReadIntervalTimeout = 0;
    stTimeOut.ReadTotalTimeoutMultiplier = 0;
    stTimeOut.ReadTotalTimeoutConstant = 10; // 每次读在10ms内完成
    stTimeOut.WriteTotalTimeoutMultiplier = 0;
    stTimeOut.WriteTotalTimeoutConstant = 0;  // 写不设置超时
    // 超时设置
    SetCommTimeouts(g_hUART, &stTimeOut);

    // 任务循环
    while (1)
    {
        // ****************************************************
        // 线程退出
        // ****************************************************
        if (true == g_bNotifyStopThread)
        {
            break;
        }

        // ****************************************************
        // 读处理
        // ****************************************************

        // 读取部分数据
        ReadFile(g_hUART, acReadBuf, sizeof(acReadBuf), &dwReadSize, NULL);

        // 将数据拷贝到累积Buf中
        memcpy(&acReadAccumulateBuf[uiReadAccumulateBufEnd], acReadBuf, dwReadSize);
        uiReadAccumulateBufEnd += dwReadSize;

        // 循环查找其中可处理的信息
        if (uiReadAccumulateBufEnd >= sizeof(MSG_L2_HEAD_S) + sizeof(USHORT))
        {
            // 总长度可能超过一帧的长度，则可以查找是否存在帧
            for (uiLoop = 0; uiLoop + sizeof(MSG_L2_HEAD_S) + sizeof(USHORT) < uiReadAccumulateBufEnd; uiLoop++)
            {
                pstMsgL2Head = (MSG_L2_HEAD_S *)&acReadAccumulateBuf[uiLoop];

                // 判断帧头能否匹配
                if (pstMsgL2Head->ucHead0 == FFY_COMM_MSG_L2_HEAD_BYTE0 && 
                    pstMsgL2Head->ucHead1 == FFY_COMM_MSG_L2_HEAD_BYTE1 &&
                    pstMsgL2Head->ucDst == FFY_COMM_MSG_DST_HOST)
                {
                    if (uiReadAccumulateBufEnd > uiLoop + pstMsgL2Head->ucLen + sizeof(MSG_L2_HEAD_S) + sizeof(USHORT))
                    {
                        // 有完整的帧，拷贝出来
                        memcpy(acProcessBuf, &acReadAccumulateBuf[uiLoop], pstMsgL2Head->ucLen + sizeof(MSG_L2_HEAD_S) + sizeof(USHORT));

                        // 在处理Buf中分析消息
                        pstMsgL2Head = (MSG_L2_HEAD_S *)acProcessBuf;

                        // 根据长度计算校验和
                        usCalcChkSum = __FFY_COMM_PCT_CalcChkSum((UCHAR *)acProcessBuf, pstMsgL2Head->ucLen);
						usFrameChkSum = __FFY_COMM_PCT_GetFrameChkSum((UCHAR *)acProcessBuf, pstMsgL2Head->ucLen);

                        if (usCalcChkSum == usFrameChkSum)
                        {
                            // 校验和正确，提取出内部的消息，并封装后发送给对话框线程
                            if (g_stComm2DlgMsg.ucValid == 0)
                            {
                                // 空间有效，才将数据复制进去
                                memcpy(g_stComm2DlgMsg.aucMsg, acProcessBuf + sizeof(MSG_L2_HEAD_S), pstMsgL2Head->ucLen);
                                g_stComm2DlgMsg.ucMsgType = pstMsgL2Head->ucType;
                                g_stComm2DlgMsg.ucMsgLen = pstMsgL2Head->ucLen;
                                g_stComm2DlgMsg.ucValid = 1;

                                // 发送消息更新显示
                                SendMessage(pDlg->m_hWnd, WM_NEW_MSG_TO_DLG, FALSE, NULL);
                            }
                        }

                        // 剩下有未处理的数据，拷贝这些数据到累积Buf的顶端
                        uiCopyLen = uiReadAccumulateBufEnd - uiLoop - pstMsgL2Head->ucLen - sizeof(MSG_L2_HEAD_S) - sizeof(USHORT);
                        memmove(acReadAccumulateBuf,
                            acReadAccumulateBuf + uiLoop + pstMsgL2Head->ucLen + sizeof(MSG_L2_HEAD_S) + sizeof(USHORT),
                            uiCopyLen);
                        uiReadAccumulateBufEnd -= (uiLoop + pstMsgL2Head->ucLen + sizeof(MSG_L2_HEAD_S) + sizeof(USHORT));

                        // 因为uiReadAccumulateBufEnd发生了改变，因此要从头开始查找。同时考虑uiLoop接下来会+1
                        uiLoop = 0 - 1;
                    }
                    else if (uiReadAccumulateBufEnd == uiLoop + pstMsgL2Head->ucLen + sizeof(MSG_L2_HEAD_S) + sizeof(USHORT))
                    {
                        // 有完整的帧，拷贝出来
                        memcpy(acProcessBuf, &acReadAccumulateBuf[uiLoop], pstMsgL2Head->ucLen + sizeof(MSG_L2_HEAD_S) + sizeof(USHORT));

                        // 剩下没有数据了，则修改结尾到顶端即可，无需数据拷贝
                        uiReadAccumulateBufEnd = 0;

                        // 在处理Buf中分析消息
                        pstMsgL2Head = (MSG_L2_HEAD_S *)acProcessBuf;

                        // 根据长度计算校验和
                        usCalcChkSum = __FFY_COMM_PCT_CalcChkSum((UCHAR *)acProcessBuf, pstMsgL2Head->ucLen);
                        usFrameChkSum = __FFY_COMM_PCT_GetFrameChkSum((UCHAR *)acProcessBuf, pstMsgL2Head->ucLen);

                        if (usCalcChkSum == usFrameChkSum)
                        {
                            // 校验和正确，提取出内部的消息，并封装后发送给对话框线程
                            if (g_stComm2DlgMsg.ucValid == 0)
                            {
                                // 空间有效，才将数据复制进去
                                memcpy(g_stComm2DlgMsg.aucMsg, acProcessBuf + sizeof(MSG_L2_HEAD_S), pstMsgL2Head->ucLen);
                                g_stComm2DlgMsg.ucMsgType = pstMsgL2Head->ucType;
                                g_stComm2DlgMsg.ucMsgLen = pstMsgL2Head->ucLen;
                                g_stComm2DlgMsg.ucValid = 1;

                                // 发送消息更新显示
                                SendMessage(pDlg->m_hWnd, WM_NEW_MSG_TO_DLG, FALSE, NULL);
                            }
                        }

                        // 因为uiReadAccumulateBufEnd发生了改变，因此要从头开始查找。同时考虑uiLoop接下来会+1
                        uiLoop = 0 - 1;
                    }
                    else
                    {
                        // 结尾长度小于消息中的帧长，可能有2种情况，一种是消息中的帧长有错误（过大），另一种是帧还没有收完
                        // 这两种情况，都不做处理，等到剩下的数据到来。如果正常错误，可能会破坏下一帧，但是在下下帧会恢复
                    }
                }
                else
                {
                    continue;
                }
            }
        }

        // 保护：如果出现了乱码，且Buf过大，则主动清理空间
        if (uiReadAccumulateBufEnd >= 4200)
        {
            memmove(acReadAccumulateBuf, &acReadAccumulateBuf[4200], uiReadAccumulateBufEnd - 4200);
            uiReadAccumulateBufEnd -= 4200;
        }

        // ****************************************************
        // 写处理
        // ****************************************************
        if (0 != g_stDlg2CommMsg.ucValid)
        {
            // 将消息拼接上头部
            pstMsgL2Head = (MSG_L2_HEAD_S *)acWriteBuf;
            pstMsgL2Head->ucHead0 = FFY_COMM_MSG_L2_HEAD_BYTE0;
            pstMsgL2Head->ucHead1 = FFY_COMM_MSG_L2_HEAD_BYTE1;
            pstMsgL2Head->ucDst = g_stDlg2CommMsg.ucMsgDst;
            pstMsgL2Head->ucType = g_stDlg2CommMsg.ucMsgType;
            pstMsgL2Head->ucLen = g_stDlg2CommMsg.ucMsgLen;

            // 将消息拷贝到Buf中
            memcpy(acWriteBuf + sizeof(MSG_L2_HEAD_S), g_stDlg2CommMsg.aucMsg, g_stDlg2CommMsg.ucMsgLen);
            
            // 计算校验和
            usCalcChkSum = __FFY_COMM_PCT_CalcChkSum((UCHAR *)acWriteBuf, g_stDlg2CommMsg.ucMsgLen);

            // 将校验和填写到缓存中
            memcpy(acWriteBuf + sizeof(MSG_L2_HEAD_S) + g_stDlg2CommMsg.ucMsgLen, &usCalcChkSum, sizeof(USHORT));

            // 发送数据
            WriteFile(g_hUART, acWriteBuf, sizeof(MSG_L2_HEAD_S) + g_stDlg2CommMsg.ucMsgLen + sizeof(USHORT), &dwWriteSize, NULL);

            g_stDlg2CommMsg.ucValid = 0;
        }
    }

    // 通告主线程任务结束
    g_bThreadStopped = true;

    return 0;
}

/******* End of file FireFlyComm.cpp. *******/



