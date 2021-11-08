
// FireFlyDlg.h : ͷ�ļ�
//

#pragma once
#pragma pack(1)

#include "afxwin.h"
#include "resource.h"
#include "typedef.h"
#include "FireFlyComm.h"
#include "afxcmn.h"


struct XSleep_Structure
{
    int    duration;
    HANDLE evenHandle;
};

/* ������Ϣ�ֶδ�С */
#define FFY_HST_CONFI_INFO_SEGMENT_SIZE          (128u)


/* ����ö�����Ͷ�����VFD��������Ϣ */
typedef enum tagFFY_CONFIG_VFD_BRSS_DEF
{
    BRIGHTNESS_LEVEL_OFF = 0,
    BRIGHTNESS_LEVEL_1,
    BRIGHTNESS_LEVEL_2,
    BRIGHTNESS_LEVEL_3,
    BRIGHTNESS_LEVEL_4,
    BRIGHTNESS_LEVEL_5,
    BRIGHTNESS_LEVEL_6,
    BRIGHTNESS_LEVEL_7,
    BRIGHTNESS_LEVEL_8,
    BRIGHTNESS_LEVEL_9,
    BRIGHTNESS_LEVEL_MAX
}FFY_CONFIG_VFD_BRSS_DEF_E;

/* ����ö�����Ͷ����˴����������� */
typedef enum tagFFY_CONFIG_TOUCH_SENSE_LVL
{
    TOUCH_SENSE_LEVEL_VERY_HIGH = 0,
    TOUCH_SENSE_LEVEL_HIGH,
    TOUCH_SENSE_LEVEL_MEDIUM,
    TOUCH_SENSE_LEVEL_LOW,
    TOUCH_SENSE_LEVEL_VERY_LOW
}FFY_CONFIG_TOUCH_SENSE_LVL_E;

/* ����ö�����Ͷ�����WIFI��ȡ����ʱ��ĸ������� */
typedef enum tagFFY_CONFIG_WIFI_UPDATE_TIME
{
    UPDATE_TIME_1_HOUR = 0,        /* ÿ1Сʱ�������ȡһ��ʱ�� */
    UPDATE_TIME_4_HOUR,              /* ÿ4Сʱ�������ȡһ��ʱ�� */
    UPDATE_TIME_12_HOUR,             /* ÿ12Сʱ�������ȡһ��ʱ�� */
    UPDATE_TIME_1_DAY,               /* ÿ1��������ȡһ��ʱ�� */
    UPDATE_TIME_1_WEEK,              /* ÿ1�ܴ������ȡһ��ʱ�� */
    UPDATE_TIME_2_WEEK,              /* ÿ2�ܴ������ȡһ��ʱ�� */
    UPDATE_TIME_1_MONTH               /* ÿ1�´������ȡһ��ʱ�� */
}FFY_CONFIG_WIFI_UPDATE_TIME_E;



/* ���½ṹ�嶨���˶�ʱ��ʾ��ģʽ��ʱ����Ϣ */
typedef struct  tagFFY_CONFIG_TIME_SHOW_INFO
{
    union
    {
        UCHAR aucRawData[8];

        struct
        {
            /* byte 0 */
            UCHAR ucValid : 1; /* ���Ʊ���������ʾ��Ϣ�Ƿ���Ч��0����Ч��1�����ձ�����Ϣ��ʱ����ʾ�� */
            UCHAR ucMonday : 1; /* ������һ�Ƿ���ʾ��0��������һ��ʾ��1����һ�����趨�ġ�ʱ/�֡�ʱ�����ʾ */
            UCHAR ucTuesday : 1; /* �����ܶ��Ƿ���ʾ��0�������ܶ���ʾ��1���ܶ������趨�ġ�ʱ/�֡�ʱ�����ʾ */
            UCHAR ucWednesday : 1; /* ���������Ƿ���ʾ��0������������ʾ��1�����������趨�ġ�ʱ/�֡�ʱ�����ʾ */
            UCHAR ucThursday : 1; /* ���������Ƿ���ʾ��0������������ʾ��1�����İ����趨�ġ�ʱ/�֡�ʱ�����ʾ */
            UCHAR ucFriday : 1; /* ���������Ƿ���ʾ��0������������ʾ��1�����尴���趨�ġ�ʱ/�֡�ʱ�����ʾ */
            UCHAR ucSaturday : 1; /* ���������Ƿ���ʾ��0������������ʾ��1�����������趨�ġ�ʱ/�֡�ʱ�����ʾ */
            UCHAR ucSunday : 1; /* ���������Ƿ���ʾ��0������������ʾ��1�����հ����趨�ġ�ʱ/�֡�ʱ�����ʾ */

            /* byte 1, 2, 3, 4 */
            UCHAR ucStartHour;                     /* ��ʾʱ��ε���ʼСʱ */
            UCHAR ucStartMinute;                   /* ��ʾʱ��ε���ʼ���� */
            UCHAR ucEndHour;                       /* ��ʾʱ��εĽ���Сʱ */
            UCHAR ucEndMinute;                     /* ��ʾʱ��εĽ������� */

            /* byte 5, 6, 7 */
            UCHAR ucRsv0;
            UCHAR ucRsv1;
            UCHAR ucRsv2;

        };
    };
} FFY_CONFIG_TIME_SHOW_INFO_S;

/* ���½ṹ�嶨���˱�����ʾ��ģʽ�ͱ���ʱ����Ϣ */
typedef struct tagFFY_CONFIG_ERGO_SHOW_INFO
{
    union
    {
        UCHAR aucRawData[4];

        struct
        {
            /* byte 0 */
            UCHAR ucShowDate : 1; /* �����Ƿ���������ʾ���ڡ�0������ʾ���ڣ�1��������ʾ���ڡ� */
            UCHAR ucShowTemp : 1; /* �����Ƿ���������ʾ�¶ȡ�0������ʾ�¶ȣ�1��������ʾ�¶ȡ� */
            UCHAR ucShowHumd : 1; /* �����Ƿ���������ʾʪ�ȡ�0������ʾʪ�ȣ�1��������ʾʪ�ȡ� */
            UCHAR ucRsv0 : 5;

            /* byte 1 */
            UCHAR ucShowDateSecondsPerMinute : 4; /* ���������ʾ���ڣ�����ÿ������ʾ���ڵ������� */
            UCHAR ucShowTempSecondsPerMinute : 4; /* ���������ʾ�¶ȣ�����ÿ������ʾ�¶ȵ������� */

            /* byte 2 */
            UCHAR ucShowHumdSecondsPerMinute : 4; /* ���������ʾ�¶ȣ�����ÿ������ʾ�¶ȵ������� */
            UCHAR ucRsv1 : 4;

            /* byte 3 */
            UCHAR ucRsv2;
        };
    };
} FFY_CONFIG_ERGO_SHOW_INFO_S;

/* ���½ṹ�嶨���˴�����֪������Ϣ */
typedef struct tagFFY_CONFIG_TOUCH_CFG_INFO
{
    union
    {
        UCHAR aucRawData[8];

        struct
        {
            /* DW0 */
            UCHAR                        ucTouchToShow : 1; /* �����ڷ���ʾʱ���Ƿ�����������ʾ��0����ֹ��������ʾ��1������������ʾ */
            UCHAR                        ucTouchToSwitch : 1; /* ��������ʾʱ���Ƿ����������л���ʾ��0����ֹ�������л���ʾ��1�����������л���ʾ */
            UCHAR                        ucTouchToStopAlarm : 1; /* ���õ���������ʱ���Ƿ���������ֹͣ���塣0����ֹ������ֹͣ���壻1����������ֹͣ���� */
            UCHAR                        ucRsv0 : 5;

            UCHAR                        ucRsv1;
            UCHAR                        ucRsv2;
            UCHAR                        ucRsv3;

            /* DW1 */
            FFY_CONFIG_TOUCH_SENSE_LVL_E enTouchSenseLevel : 8;
            UINT                         uiRsv4 : 24;
        };
    };
}FFY_CONFIG_TOUCH_CFG_INFO_S;

/* ���½ṹ�嶨����WIFI���ӵ�AP�������Ϣ */
typedef struct tagFFY_CONFIG_AP_INFO
{
    union
    {
        UCHAR aucRawData[80];

        struct
        {
            /* BYTE 0 */
            UCHAR ucValid : 1;
            UCHAR ucRsv0 : 7;

            /* BYTE 01 ~ 40 */ /* SSID */
            CHAR  acSSID[40];

            /* BYTE 41 ~ 79 */ /* passsword */
            CHAR  acPassword[39];
        };
    };
} FFY_CONFIG_AP_INFO_S;

/* ���½ṹ�����Ͷ�����NTP�������������Ϣ */
typedef struct tagFFY_CONFIG_NTP_SERVER_INFO
{
    union
    {
        UCHAR aucRawData[128];

        struct
        {
            /* BYTE 0 */
            UCHAR           ucValid : 1;
            UCHAR           ucRsv0 : 7;

            /* BYTE 1 ~ 127 */
            CHAR            acNTPServer[127];
        };
    };
} FFY_CONFIG_NTP_SERVER_INFO_S;

/* һ�½ṹ�����Ͷ�����WIFI��ȡ����ʱ������������Ϣ */
typedef struct tagFFY_CONFIG_WIFI_CFG_INFO
{
    union
    {
        UCHAR aucRawData[548];

        struct
        {
            /* DW  0 ~ DW 19 */ /* AP1������Ϣ */
            FFY_CONFIG_AP_INFO_S          stAP1Info;
            /* DW 20 ~ DW 39 */ /* AP2������Ϣ */
            FFY_CONFIG_AP_INFO_S          stAP2Info;
            /* DW 40 ~ DW 71 */ /* NTP������1������Ϣ */
            FFY_CONFIG_NTP_SERVER_INFO_S  stNTPServer1;
            /* DW 72 ~ DW103 */ /* NTP������2������Ϣ */
            FFY_CONFIG_NTP_SERVER_INFO_S  stNTPServer2;
            /* DW104 ~ DW135 */ /* NTP������2������Ϣ */
            FFY_CONFIG_NTP_SERVER_INFO_S  stNTPServer3;
            /* DW136 */
            FFY_CONFIG_WIFI_UPDATE_TIME_E enUpdateIntervel;
        };
    };
}FFY_CONFIG_WIFI_CFG_INFO_S;


/* ���½ṹ�嶨����FFY�Ĺ���������Ϣ */
typedef struct tagFFY_CONFIG_INFO
{
    union
    {
        UCHAR aucRawData[1024];

        struct
        {
            /* DW0 */ /* ��������� */
            UINT                         uiEnableTimeShow : 1;
            UINT                         uiEnableEgroShow : 1;
            UINT                         uiEnableTouchSense : 1;
            UINT                         uiEnableWIFIUpdateTime : 1;
            UINT                         uiByte0Rsv0 : 4;

            UINT                         uiByte1Rsv0 : 8;

            UINT                         uiByte2Rsv0 : 8;

            UINT                         uiFormatFlag : 8;

            /* DW1 ~ DW7 */
            FFY_CONFIG_VFD_BRSS_DEF_E    enLCDBrightness : 8; /* VFD���ȵȼ����� */
            UINT                         uiCharGroup     : 8; /* �趨��ʾ���ַ��� */
            UINT                         uiRsv0          : 16;
            UCHAR                        aucRsv1[24];

            /* DW8 ~ DW15 */ /* ��ʱ��ʾ��Ϣ */
            FFY_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo1;
            FFY_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo2;
            FFY_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo3;
            FFY_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo4;
            FFY_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo5;
            FFY_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo6;

            /* DW16 */ /* ������ʾ��Ϣ */
            FFY_CONFIG_ERGO_SHOW_INFO_S  stErgoShowInfo;

            /* DW17 */ /* ������ʾ������Ϣ */
            FFY_CONFIG_TOUCH_CFG_INFO_S  stTouchConfigInfo;

            /* DW18 ~ DW154 */ /* WIFI��ȡʱ������������Ϣ */
            FFY_CONFIG_WIFI_CFG_INFO_S   stWIFIConfigInfo;
        };
    };
} FFY_CONFIG_INFO_S;


// CFireFlyDlg �Ի���
class CFireFlyDlg : public CDialogEx
{
// ����
public:
	CFireFlyDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FIREFLY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
    afx_msg LRESULT OnProcessNewRecvMessage(WPARAM wParam, LPARAM lParam);

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private :
    bool          m_bCommOpened;           //��ʾͨ�Ŷ˿��Ƿ�򿪳ɹ�
    DCB           m_stUARTInfo;            // ����������Ϣ
    FILE         *m_pstBMPFile;

    CComboBox     m_obCBXCOMMPortSel;
    CButton       m_obBTNOpenCloseComm;

    CComboBox     m_obCBXTimeShowGrp1WeekMode;
    CComboBox     m_obCBXTimeShowGrp2WeekMode;
    CComboBox     m_obCBXTimeShowGrp3WeekMode;
    CComboBox     m_obCBXTimeShowGrp4WeekMode;
    CComboBox     m_obCBXTimeShowGrp5WeekMode;
    CComboBox     m_obCBXTimeShowGrp6WeekMode;

    CComboBox     m_obCBXTimeShowGrp1StartHour;
    CComboBox     m_obCBXTimeShowGrp2StartHour;
    CComboBox     m_obCBXTimeShowGrp3StartHour;
    CComboBox     m_obCBXTimeShowGrp4StartHour;
    CComboBox     m_obCBXTimeShowGrp5StartHour;
    CComboBox     m_obCBXTimeShowGrp6StartHour;

    CComboBox     m_obCBXTimeShowGrp1StartMinute;
    CComboBox     m_obCBXTimeShowGrp2StartMinute;
    CComboBox     m_obCBXTimeShowGrp3StartMinute;
    CComboBox     m_obCBXTimeShowGrp4StartMinute;
    CComboBox     m_obCBXTimeShowGrp5StartMinute;
    CComboBox     m_obCBXTimeShowGrp6StartMinute;

    CComboBox     m_obCBXTimeShowGrp1EndHour;
    CComboBox     m_obCBXTimeShowGrp2EndHour;
    CComboBox     m_obCBXTimeShowGrp3EndHour;
    CComboBox     m_obCBXTimeShowGrp4EndHour;
    CComboBox     m_obCBXTimeShowGrp5EndHour;
    CComboBox     m_obCBXTimeShowGrp6EndHour;

    CComboBox     m_obCBXTimeShowGrp1EndMinute;
    CComboBox     m_obCBXTimeShowGrp2EndMinute;
    CComboBox     m_obCBXTimeShowGrp3EndMinute;
    CComboBox     m_obCBXTimeShowGrp4EndMinute;
    CComboBox     m_obCBXTimeShowGrp5EndMinute;
    CComboBox     m_obCBXTimeShowGrp6EndMinute;

    CComboBox     m_obCBXVFDBrightness;
    CComboBox     m_obCBXEgroShowDateSeconds;
    CComboBox     m_obCBXEgroShowTempSeconds;
    CComboBox     m_obCBXEgroShowHumdSeconds;
    CComboBox     m_obCBXTouchSensetivity;
    CComboBox     m_obCBXWIFIUpdateTimeInterval;

    CComboBox     m_obCBXFontsBMPGroup;

    CEdit         m_obEDTAP1SSID;
    CEdit         m_obEDTAP1Password;
    CEdit         m_obEDTAP2SSID;
    CEdit         m_obEDTAP2Password;
    CEdit         m_obEDTNTPServer1;
    CEdit         m_obEDTNTPServer2;
    CEdit         m_obEDTNTPServer3;

    CProgressCtrl m_obPGSFlashWrite;

    CWinThread   *m_pobCommThread;

public:
    afx_msg void OnBnClickedButtonOpenCloseComm();

private:
    VOID         FUNC_SetAllCtrlState(bool bState);
    VOID         FUNC_SetAllComboBox();


public:
    afx_msg void OnBnClickedButtonGetProfile();

    afx_msg void OnBnClickedButtonStoreProfile();
	afx_msg void OnBnClickedButtonFontsProgram();
private:
    
    
public:
    CComboBox m_obCBXFontsGroupSel;
};
