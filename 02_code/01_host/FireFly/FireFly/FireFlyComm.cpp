/*******************************************************************************
Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
File name    : FireFlyComm.cpp
Project name : GAirTestTool
Module name  :
Date created : 2020��02��10��   17ʱ15��39��
Author       : Ning.JianLi
Description  : GAirTestTool����ͨ��ģ��Դ�ļ���
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

#include "stdafx.h"
#include "FireFlyComm.h"

// ͨ�Ŷ˿��ļ����
extern HANDLE                  g_hUART;

// LOG��ʾBuf
extern CHAR                    g_acShowLogBuf[16384];
extern UINT                    g_uiShowLogBufPtr;

// �̼߳���Ϣ����
extern GTT_COMM_MSG_DLG2COMM_S g_stDlg2CommMsg;
extern GTT_COMM_MSG_COMM2DLG_S g_stComm2DlgMsg;

// ͨ���߳̽������
extern bool                    g_bNotifyStopThread;
extern bool                    g_bThreadStopped;

/*******************************************************************************
- Function    : char2wchar
- Description : ��char���ַ���ת��Ϊwchar_t�����ַ�����
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
- Description : ͨ��ע����ѯϵͳ�Ĵ��ڣ�����������ʾ��ComboBox�ؼ���ѡ���С�
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

    // ��ע�����Ŀ
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
            AfxMessageBox(_T("[HKEY_LOCAL_MACHINE:Hardware\\DeviceMap\\SerialComm]·����û�д��ڡ�"));
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
        // ���Ȳ�����֡ͷ��Ŀ�ĵأ����ͣ������ֽڣ��Լ�У��ͱ�������У���Ҫȫ������
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
- Description : ͨ��ע����ѯϵͳ�Ĵ��ڣ�����������ʾ��ComboBox�ؼ���ѡ���С�
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
    // ���ö�д��ʱ
    // ****************************************************

    // ��ʱ��������
    stTimeOut.ReadIntervalTimeout = 0;
    stTimeOut.ReadTotalTimeoutMultiplier = 0;
    stTimeOut.ReadTotalTimeoutConstant = 10; // ÿ�ζ���10ms�����
    stTimeOut.WriteTotalTimeoutMultiplier = 0;
    stTimeOut.WriteTotalTimeoutConstant = 0;  // д�����ó�ʱ
    // ��ʱ����
    SetCommTimeouts(g_hUART, &stTimeOut);

    // ����ѭ��
    while (1)
    {
        // ****************************************************
        // �߳��˳�
        // ****************************************************
        if (true == g_bNotifyStopThread)
        {
            break;
        }

        // ****************************************************
        // ������
        // ****************************************************

        // ��ȡ��������
        ReadFile(g_hUART, acReadBuf, sizeof(acReadBuf), &dwReadSize, NULL);

        // �����ݿ������ۻ�Buf��
        memcpy(&acReadAccumulateBuf[uiReadAccumulateBufEnd], acReadBuf, dwReadSize);
        uiReadAccumulateBufEnd += dwReadSize;

        // ѭ���������пɴ������Ϣ
        if (uiReadAccumulateBufEnd >= sizeof(MSG_L2_HEAD_S) + sizeof(USHORT))
        {
            // �ܳ��ȿ��ܳ���һ֡�ĳ��ȣ�����Բ����Ƿ����֡
            for (uiLoop = 0; uiLoop + sizeof(MSG_L2_HEAD_S) + sizeof(USHORT) < uiReadAccumulateBufEnd; uiLoop++)
            {
                pstMsgL2Head = (MSG_L2_HEAD_S *)&acReadAccumulateBuf[uiLoop];

                // �ж�֡ͷ�ܷ�ƥ��
                if (pstMsgL2Head->ucHead0 == FFY_COMM_MSG_L2_HEAD_BYTE0 && 
                    pstMsgL2Head->ucHead1 == FFY_COMM_MSG_L2_HEAD_BYTE1 &&
                    pstMsgL2Head->ucDst == FFY_COMM_MSG_DST_HOST)
                {
                    if (uiReadAccumulateBufEnd > uiLoop + pstMsgL2Head->ucLen + sizeof(MSG_L2_HEAD_S) + sizeof(USHORT))
                    {
                        // ��������֡����������
                        memcpy(acProcessBuf, &acReadAccumulateBuf[uiLoop], pstMsgL2Head->ucLen + sizeof(MSG_L2_HEAD_S) + sizeof(USHORT));

                        // �ڴ���Buf�з�����Ϣ
                        pstMsgL2Head = (MSG_L2_HEAD_S *)acProcessBuf;

                        // ���ݳ��ȼ���У���
                        usCalcChkSum = __FFY_COMM_PCT_CalcChkSum((UCHAR *)acProcessBuf, pstMsgL2Head->ucLen);
						usFrameChkSum = __FFY_COMM_PCT_GetFrameChkSum((UCHAR *)acProcessBuf, pstMsgL2Head->ucLen);

                        if (usCalcChkSum == usFrameChkSum)
                        {
                            // У�����ȷ����ȡ���ڲ�����Ϣ������װ���͸��Ի����߳�
                            if (g_stComm2DlgMsg.ucValid == 0)
                            {
                                // �ռ���Ч���Ž����ݸ��ƽ�ȥ
                                memcpy(g_stComm2DlgMsg.aucMsg, acProcessBuf + sizeof(MSG_L2_HEAD_S), pstMsgL2Head->ucLen);
                                g_stComm2DlgMsg.ucMsgType = pstMsgL2Head->ucType;
                                g_stComm2DlgMsg.ucMsgLen = pstMsgL2Head->ucLen;
                                g_stComm2DlgMsg.ucValid = 1;

                                // ������Ϣ������ʾ
                                SendMessage(pDlg->m_hWnd, WM_NEW_MSG_TO_DLG, FALSE, NULL);
                            }
                        }

                        // ʣ����δ��������ݣ�������Щ���ݵ��ۻ�Buf�Ķ���
                        uiCopyLen = uiReadAccumulateBufEnd - uiLoop - pstMsgL2Head->ucLen - sizeof(MSG_L2_HEAD_S) - sizeof(USHORT);
                        memmove(acReadAccumulateBuf,
                            acReadAccumulateBuf + uiLoop + pstMsgL2Head->ucLen + sizeof(MSG_L2_HEAD_S) + sizeof(USHORT),
                            uiCopyLen);
                        uiReadAccumulateBufEnd -= (uiLoop + pstMsgL2Head->ucLen + sizeof(MSG_L2_HEAD_S) + sizeof(USHORT));

                        // ��ΪuiReadAccumulateBufEnd�����˸ı䣬���Ҫ��ͷ��ʼ���ҡ�ͬʱ����uiLoop��������+1
                        uiLoop = 0 - 1;
                    }
                    else if (uiReadAccumulateBufEnd == uiLoop + pstMsgL2Head->ucLen + sizeof(MSG_L2_HEAD_S) + sizeof(USHORT))
                    {
                        // ��������֡����������
                        memcpy(acProcessBuf, &acReadAccumulateBuf[uiLoop], pstMsgL2Head->ucLen + sizeof(MSG_L2_HEAD_S) + sizeof(USHORT));

                        // ʣ��û�������ˣ����޸Ľ�β�����˼��ɣ��������ݿ���
                        uiReadAccumulateBufEnd = 0;

                        // �ڴ���Buf�з�����Ϣ
                        pstMsgL2Head = (MSG_L2_HEAD_S *)acProcessBuf;

                        // ���ݳ��ȼ���У���
                        usCalcChkSum = __FFY_COMM_PCT_CalcChkSum((UCHAR *)acProcessBuf, pstMsgL2Head->ucLen);
                        usFrameChkSum = __FFY_COMM_PCT_GetFrameChkSum((UCHAR *)acProcessBuf, pstMsgL2Head->ucLen);

                        if (usCalcChkSum == usFrameChkSum)
                        {
                            // У�����ȷ����ȡ���ڲ�����Ϣ������װ���͸��Ի����߳�
                            if (g_stComm2DlgMsg.ucValid == 0)
                            {
                                // �ռ���Ч���Ž����ݸ��ƽ�ȥ
                                memcpy(g_stComm2DlgMsg.aucMsg, acProcessBuf + sizeof(MSG_L2_HEAD_S), pstMsgL2Head->ucLen);
                                g_stComm2DlgMsg.ucMsgType = pstMsgL2Head->ucType;
                                g_stComm2DlgMsg.ucMsgLen = pstMsgL2Head->ucLen;
                                g_stComm2DlgMsg.ucValid = 1;

                                // ������Ϣ������ʾ
                                SendMessage(pDlg->m_hWnd, WM_NEW_MSG_TO_DLG, FALSE, NULL);
                            }
                        }

                        // ��ΪuiReadAccumulateBufEnd�����˸ı䣬���Ҫ��ͷ��ʼ���ҡ�ͬʱ����uiLoop��������+1
                        uiLoop = 0 - 1;
                    }
                    else
                    {
                        // ��β����С����Ϣ�е�֡����������2�������һ������Ϣ�е�֡���д��󣨹��󣩣���һ����֡��û������
                        // ����������������������ȵ�ʣ�µ����ݵ���������������󣬿��ܻ��ƻ���һ֡������������֡��ָ�
                    }
                }
                else
                {
                    continue;
                }
            }
        }

        // ������������������룬��Buf��������������ռ�
        if (uiReadAccumulateBufEnd >= 4200)
        {
            memmove(acReadAccumulateBuf, &acReadAccumulateBuf[4200], uiReadAccumulateBufEnd - 4200);
            uiReadAccumulateBufEnd -= 4200;
        }

        // ****************************************************
        // д����
        // ****************************************************
        if (0 != g_stDlg2CommMsg.ucValid)
        {
            // ����Ϣƴ����ͷ��
            pstMsgL2Head = (MSG_L2_HEAD_S *)acWriteBuf;
            pstMsgL2Head->ucHead0 = FFY_COMM_MSG_L2_HEAD_BYTE0;
            pstMsgL2Head->ucHead1 = FFY_COMM_MSG_L2_HEAD_BYTE1;
            pstMsgL2Head->ucDst = g_stDlg2CommMsg.ucMsgDst;
            pstMsgL2Head->ucType = g_stDlg2CommMsg.ucMsgType;
            pstMsgL2Head->ucLen = g_stDlg2CommMsg.ucMsgLen;

            // ����Ϣ������Buf��
            memcpy(acWriteBuf + sizeof(MSG_L2_HEAD_S), g_stDlg2CommMsg.aucMsg, g_stDlg2CommMsg.ucMsgLen);
            
            // ����У���
            usCalcChkSum = __FFY_COMM_PCT_CalcChkSum((UCHAR *)acWriteBuf, g_stDlg2CommMsg.ucMsgLen);

            // ��У�����д��������
            memcpy(acWriteBuf + sizeof(MSG_L2_HEAD_S) + g_stDlg2CommMsg.ucMsgLen, &usCalcChkSum, sizeof(USHORT));

            // ��������
            WriteFile(g_hUART, acWriteBuf, sizeof(MSG_L2_HEAD_S) + g_stDlg2CommMsg.ucMsgLen + sizeof(USHORT), &dwWriteSize, NULL);

            g_stDlg2CommMsg.ucValid = 0;
        }
    }

    // ͨ�����߳��������
    g_bThreadStopped = true;

    return 0;
}

/******* End of file FireFlyComm.cpp. *******/



