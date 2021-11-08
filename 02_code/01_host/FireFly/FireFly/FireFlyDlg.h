
// FireFlyDlg.h : 头文件
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

/* 配置信息分段大小 */
#define FFY_HST_CONFI_INFO_SEGMENT_SIZE          (128u)


/* 以下枚举类型定义了VFD的亮度信息 */
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

/* 以下枚举类型定义了触碰的灵敏度 */
typedef enum tagFFY_CONFIG_TOUCH_SENSE_LVL
{
    TOUCH_SENSE_LEVEL_VERY_HIGH = 0,
    TOUCH_SENSE_LEVEL_HIGH,
    TOUCH_SENSE_LEVEL_MEDIUM,
    TOUCH_SENSE_LEVEL_LOW,
    TOUCH_SENSE_LEVEL_VERY_LOW
}FFY_CONFIG_TOUCH_SENSE_LVL_E;

/* 以下枚举类型定义了WIFI获取网络时间的更新周期 */
typedef enum tagFFY_CONFIG_WIFI_UPDATE_TIME
{
    UPDATE_TIME_1_HOUR = 0,        /* 每1小时从网络获取一次时间 */
    UPDATE_TIME_4_HOUR,              /* 每4小时从网络获取一次时间 */
    UPDATE_TIME_12_HOUR,             /* 每12小时从网络获取一次时间 */
    UPDATE_TIME_1_DAY,               /* 每1天从网络获取一次时间 */
    UPDATE_TIME_1_WEEK,              /* 每1周从网络获取一次时间 */
    UPDATE_TIME_2_WEEK,              /* 每2周从网络获取一次时间 */
    UPDATE_TIME_1_MONTH               /* 每1月从网络获取一次时间 */
}FFY_CONFIG_WIFI_UPDATE_TIME_E;



/* 以下结构体定义了定时显示的模式和时间信息 */
typedef struct  tagFFY_CONFIG_TIME_SHOW_INFO
{
    union
    {
        UCHAR aucRawData[8];

        struct
        {
            /* byte 0 */
            UCHAR ucValid : 1; /* 控制本条周期显示信息是否有效。0：无效；1：按照本条信息的时间显示。 */
            UCHAR ucMonday : 1; /* 控制周一是否显示。0：不在周一显示；1：周一按照设定的“时/分”时间段显示 */
            UCHAR ucTuesday : 1; /* 控制周二是否显示。0：不在周二显示；1：周二按照设定的“时/分”时间段显示 */
            UCHAR ucWednesday : 1; /* 控制周三是否显示。0：不在周三显示；1：周三按照设定的“时/分”时间段显示 */
            UCHAR ucThursday : 1; /* 控制周四是否显示。0：不在周四显示；1：周四按照设定的“时/分”时间段显示 */
            UCHAR ucFriday : 1; /* 控制周五是否显示。0：不在周五显示；1：周五按照设定的“时/分”时间段显示 */
            UCHAR ucSaturday : 1; /* 控制周六是否显示。0：不在周六显示；1：周六按照设定的“时/分”时间段显示 */
            UCHAR ucSunday : 1; /* 控制周日是否显示。0：不在周日显示；1：周日按照设定的“时/分”时间段显示 */

            /* byte 1, 2, 3, 4 */
            UCHAR ucStartHour;                     /* 显示时间段的起始小时 */
            UCHAR ucStartMinute;                   /* 显示时间段的起始分钟 */
            UCHAR ucEndHour;                       /* 显示时间段的结束小时 */
            UCHAR ucEndMinute;                     /* 显示时间段的结束分钟 */

            /* byte 5, 6, 7 */
            UCHAR ucRsv0;
            UCHAR ucRsv1;
            UCHAR ucRsv2;

        };
    };
} FFY_CONFIG_TIME_SHOW_INFO_S;

/* 以下结构体定义了遍历显示的模式和遍历时间信息 */
typedef struct tagFFY_CONFIG_ERGO_SHOW_INFO
{
    union
    {
        UCHAR aucRawData[4];

        struct
        {
            /* byte 0 */
            UCHAR ucShowDate : 1; /* 控制是否周期性显示日期。0：不显示日期；1：周期显示日期。 */
            UCHAR ucShowTemp : 1; /* 控制是否周期性显示温度。0：不显示温度；1：周期显示温度。 */
            UCHAR ucShowHumd : 1; /* 控制是否周期性显示湿度。0：不显示湿度；1：周期显示湿度。 */
            UCHAR ucRsv0 : 5;

            /* byte 1 */
            UCHAR ucShowDateSecondsPerMinute : 4; /* 如果周期显示日期，设置每分钟显示日期的秒数。 */
            UCHAR ucShowTempSecondsPerMinute : 4; /* 如果周期显示温度，设置每分钟显示温度的秒数。 */

            /* byte 2 */
            UCHAR ucShowHumdSecondsPerMinute : 4; /* 如果周期显示温度，设置每分钟显示温度的秒数。 */
            UCHAR ucRsv1 : 4;

            /* byte 3 */
            UCHAR ucRsv2;
        };
    };
} FFY_CONFIG_ERGO_SHOW_INFO_S;

/* 以下结构体定义了触碰感知配置信息 */
typedef struct tagFFY_CONFIG_TOUCH_CFG_INFO
{
    union
    {
        UCHAR aucRawData[8];

        struct
        {
            /* DW0 */
            UCHAR                        ucTouchToShow : 1; /* 配置在非显示时，是否允许触摸以显示。0：禁止触摸以显示；1：允许触摸以显示 */
            UCHAR                        ucTouchToSwitch : 1; /* 配置在显示时，是否允许触摸以切换显示。0：禁止触摸以切换显示；1：允许触摸以切换显示 */
            UCHAR                        ucTouchToStopAlarm : 1; /* 配置当闹铃闹响时，是否允许触摸以停止闹铃。0：禁止触摸以停止闹铃；1：允许触摸以停止闹铃 */
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

/* 以下结构体定义了WIFI连接的AP的相关信息 */
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

/* 以下结构体类型定义了NTP服务器的相关信息 */
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

/* 一下结构体类型定义了WIFI获取网络时间的相关配置信息 */
typedef struct tagFFY_CONFIG_WIFI_CFG_INFO
{
    union
    {
        UCHAR aucRawData[548];

        struct
        {
            /* DW  0 ~ DW 19 */ /* AP1配置信息 */
            FFY_CONFIG_AP_INFO_S          stAP1Info;
            /* DW 20 ~ DW 39 */ /* AP2配置信息 */
            FFY_CONFIG_AP_INFO_S          stAP2Info;
            /* DW 40 ~ DW 71 */ /* NTP服务器1配置信息 */
            FFY_CONFIG_NTP_SERVER_INFO_S  stNTPServer1;
            /* DW 72 ~ DW103 */ /* NTP服务器2配置信息 */
            FFY_CONFIG_NTP_SERVER_INFO_S  stNTPServer2;
            /* DW104 ~ DW135 */ /* NTP服务器2配置信息 */
            FFY_CONFIG_NTP_SERVER_INFO_S  stNTPServer3;
            /* DW136 */
            FFY_CONFIG_WIFI_UPDATE_TIME_E enUpdateIntervel;
        };
    };
}FFY_CONFIG_WIFI_CFG_INFO_S;


/* 以下结构体定义了FFY的工作配置信息 */
typedef struct tagFFY_CONFIG_INFO
{
    union
    {
        UCHAR aucRawData[1024];

        struct
        {
            /* DW0 */ /* 仅用作标记 */
            UINT                         uiEnableTimeShow : 1;
            UINT                         uiEnableEgroShow : 1;
            UINT                         uiEnableTouchSense : 1;
            UINT                         uiEnableWIFIUpdateTime : 1;
            UINT                         uiByte0Rsv0 : 4;

            UINT                         uiByte1Rsv0 : 8;

            UINT                         uiByte2Rsv0 : 8;

            UINT                         uiFormatFlag : 8;

            /* DW1 ~ DW7 */
            FFY_CONFIG_VFD_BRSS_DEF_E    enLCDBrightness : 8; /* VFD亮度等级设置 */
            UINT                         uiCharGroup     : 8; /* 设定显示的字符组 */
            UINT                         uiRsv0          : 16;
            UCHAR                        aucRsv1[24];

            /* DW8 ~ DW15 */ /* 定时显示信息 */
            FFY_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo1;
            FFY_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo2;
            FFY_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo3;
            FFY_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo4;
            FFY_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo5;
            FFY_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo6;

            /* DW16 */ /* 遍历显示信息 */
            FFY_CONFIG_ERGO_SHOW_INFO_S  stErgoShowInfo;

            /* DW17 */ /* 触碰显示配置信息 */
            FFY_CONFIG_TOUCH_CFG_INFO_S  stTouchConfigInfo;

            /* DW18 ~ DW154 */ /* WIFI获取时间的相关配置信息 */
            FFY_CONFIG_WIFI_CFG_INFO_S   stWIFIConfigInfo;
        };
    };
} FFY_CONFIG_INFO_S;


// CFireFlyDlg 对话框
class CFireFlyDlg : public CDialogEx
{
// 构造
public:
	CFireFlyDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FIREFLY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
    afx_msg LRESULT OnProcessNewRecvMessage(WPARAM wParam, LPARAM lParam);

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private :
    bool          m_bCommOpened;           //表示通信端口是否打开成功
    DCB           m_stUARTInfo;            // 串口配置信息
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
