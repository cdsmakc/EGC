
// FireFlyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FireFly.h"
#include "FireFlyDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CODE_SECTION("=========================================");
CODE_SECTION("==  全局变量定义                         ==");
CODE_SECTION("=========================================");

// 通信端口文件句柄
HANDLE                  g_hUART;

// LOG显示Buf
CHAR                    g_acShowLogBuf[16384];
UINT                    g_uiShowLogBufPtr = 0;

// 线程间消息缓存
GTT_COMM_MSG_DLG2COMM_S g_stDlg2CommMsg;
GTT_COMM_MSG_COMM2DLG_S g_stComm2DlgMsg;

// 通告线程结束标记
bool                    g_bNotifyStopThread;
bool                    g_bThreadStopped;

//本地配置信息
FFY_CONFIG_INFO_S       g_stFFYConfigInfo;


// CFireFlyDlg 对话框
CODE_SECTION("=========================================");
CODE_SECTION("==  对话框类成员函数定义                  ==");
CODE_SECTION("=========================================");

CFireFlyDlg::CFireFlyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFireFlyDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFireFlyDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_COMM_PORT_SEL, m_obCBXCOMMPortSel);
    DDX_Control(pDX, IDC_BUTTON_OPEN_CLOSE_COMM, m_obBTNOpenCloseComm);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP1_WEEK_MODE, m_obCBXTimeShowGrp1WeekMode);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP2_WEEK_MODE, m_obCBXTimeShowGrp2WeekMode);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP3_WEEK_MODE, m_obCBXTimeShowGrp3WeekMode);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP4_WEEK_MODE, m_obCBXTimeShowGrp4WeekMode);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP5_WEEK_MODE, m_obCBXTimeShowGrp5WeekMode);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP6_WEEK_MODE, m_obCBXTimeShowGrp6WeekMode);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP1_START_HOUR, m_obCBXTimeShowGrp1StartHour);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP2_START_HOUR, m_obCBXTimeShowGrp2StartHour);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP3_START_HOUR, m_obCBXTimeShowGrp3StartHour);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP4_START_HOUR, m_obCBXTimeShowGrp4StartHour);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP5_START_HOUR, m_obCBXTimeShowGrp5StartHour);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP6_START_HOUR, m_obCBXTimeShowGrp6StartHour);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP1_START_MINUTE, m_obCBXTimeShowGrp1StartMinute);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP2_START_MINUTE, m_obCBXTimeShowGrp2StartMinute);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP3_START_MINUTE, m_obCBXTimeShowGrp3StartMinute);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP4_START_MINUTE, m_obCBXTimeShowGrp4StartMinute);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP5_START_MINUTE, m_obCBXTimeShowGrp5StartMinute);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP6_START_MINUTE, m_obCBXTimeShowGrp6StartMinute);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP1_END_HOUR, m_obCBXTimeShowGrp1EndHour);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP2_END_HOUR, m_obCBXTimeShowGrp2EndHour);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP3_END_HOUR, m_obCBXTimeShowGrp3EndHour);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP4_END_HOUR, m_obCBXTimeShowGrp4EndHour);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP5_END_HOUR, m_obCBXTimeShowGrp5EndHour);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP6_END_HOUR, m_obCBXTimeShowGrp6EndHour);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP1_END_MINUTE, m_obCBXTimeShowGrp1EndMinute);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP2_END_MINUTE, m_obCBXTimeShowGrp2EndMinute);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP3_END_MINUTE, m_obCBXTimeShowGrp3EndMinute);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP4_END_MINUTE, m_obCBXTimeShowGrp4EndMinute);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP5_END_MINUTE, m_obCBXTimeShowGrp5EndMinute);
    DDX_Control(pDX, IDC_COMBO_TIME_SHOW_GRP6_END_MINUTE, m_obCBXTimeShowGrp6EndMinute);
    DDX_Control(pDX, IDC_COMBO_VFD_BRIGHTNESS, m_obCBXVFDBrightness);
    DDX_Control(pDX, IDC_COMBO_EGRO_SHOW_DATE_SECOND, m_obCBXEgroShowDateSeconds);
    DDX_Control(pDX, IDC_COMBO_EGRO_SHOW_TEMP_SECOND, m_obCBXEgroShowTempSeconds);
    DDX_Control(pDX, IDC_COMBO_EGRO_SHOW_HUMD_SECOND, m_obCBXEgroShowHumdSeconds);
    DDX_Control(pDX, IDC_COMBO_TOUCH_SENSITIVITY, m_obCBXTouchSensetivity);
    DDX_Control(pDX, IDC_COMBO_WIFI_UPDATE_INVERVAL, m_obCBXWIFIUpdateTimeInterval);
    DDX_Control(pDX, IDC_EDIT_WIFI_AP1_SSID, m_obEDTAP1SSID);
    DDX_Control(pDX, IDC_EDIT_WIFI_AP1_PASSWORD, m_obEDTAP1Password);
    DDX_Control(pDX, IDC_EDIT_WIFI_AP2_PASSWORD, m_obEDTAP2Password);
    DDX_Control(pDX, IDC_EDIT_WIFI_AP2_SSID, m_obEDTAP2SSID);
    DDX_Control(pDX, IDC_EDIT_WIFI_NTP_SERVER1, m_obEDTNTPServer1);
    DDX_Control(pDX, IDC_EDIT_WIFI_NTP_SERVER2, m_obEDTNTPServer2);
    DDX_Control(pDX, IDC_EDIT_WIFI_NTP_SERVER3, m_obEDTNTPServer3);
    DDX_Control(pDX, IDC_COMBO_FONTS_PROGRAM_DESTINATION, m_obCBXFontsBMPGroup);
    DDX_Control(pDX, IDC_PROGRESS_FONTS_PROGRAM_PROGRESS, m_obPGSFlashWrite);
    DDX_Control(pDX, IDC_COMBO_FONTS_GROUP_SEL, m_obCBXFontsGroupSel);
}

BEGIN_MESSAGE_MAP(CFireFlyDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_MESSAGE(WM_NEW_MSG_TO_DLG, OnProcessNewRecvMessage)
    ON_BN_CLICKED(IDC_BUTTON_OPEN_CLOSE_COMM, &CFireFlyDlg::OnBnClickedButtonOpenCloseComm)
    ON_BN_CLICKED(IDC_BUTTON_GET_PROFILE, &CFireFlyDlg::OnBnClickedButtonGetProfile)
    ON_BN_CLICKED(IDC_BUTTON_STORE_PROFILE, &CFireFlyDlg::OnBnClickedButtonStoreProfile)
	ON_BN_CLICKED(IDC_BUTTON_FONTS_PROGRAM, &CFireFlyDlg::OnBnClickedButtonFontsProgram)
END_MESSAGE_MAP()

/***********************************************************************************************************
 * 对话框类的功能函数定义
 ***********************************************************************************************************/

/*******************************************************************************
- Function    : FUNC_SetAllCtrlState
- Description : 该功能函数使能或禁止对话框中的一系列控件。根据输入的参数决定使能或者禁
                止。
- Input       : bState : 使能或者禁止对话框中的控件。
                    true  : 使能控件
                    false : 禁止控件
- Output      : --
- Return      : --
- Others      :
*******************************************************************************/
VOID CFireFlyDlg::FUNC_SetAllCtrlState(bool bState)
{
    CWnd    *cobCtrl;

    cobCtrl = GetDlgItem(IDC_BUTTON_GET_PROFILE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_BUTTON_STORE_PROFILE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP1);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP2);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP3);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP4);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP5);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP6);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP1_WEEK_MODE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP2_WEEK_MODE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP3_WEEK_MODE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP4_WEEK_MODE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP5_WEEK_MODE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP6_WEEK_MODE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP1_START_HOUR);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP2_START_HOUR);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP3_START_HOUR);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP4_START_HOUR);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP5_START_HOUR);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP6_START_HOUR);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP1_START_MINUTE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP2_START_MINUTE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP3_START_MINUTE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP4_START_MINUTE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP5_START_MINUTE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP6_START_MINUTE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP1_END_HOUR);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP2_END_HOUR);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP3_END_HOUR);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP4_END_HOUR);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP5_END_HOUR);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP6_END_HOUR);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP1_END_MINUTE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP2_END_MINUTE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP3_END_MINUTE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP4_END_MINUTE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP5_END_MINUTE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TIME_SHOW_GRP6_END_MINUTE);
    cobCtrl->EnableWindow(bState);

    cobCtrl = GetDlgItem(IDC_COMBO_VFD_BRIGHTNESS);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_FONTS_GROUP_SEL);
    cobCtrl->EnableWindow(bState);

    cobCtrl = GetDlgItem(IDC_CHECK_USE_EGRO_SHOW);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_EGRO_SHOW_DATE);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_EGRO_SHOW_TEMP);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_EGRO_SHOW_HUMD);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_EGRO_SHOW_DATE_SECOND);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_EGRO_SHOW_TEMP_SECOND);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_EGRO_SHOW_HUMD_SECOND);
    cobCtrl->EnableWindow(bState);

    cobCtrl = GetDlgItem(IDC_CHECK_USE_WIFI);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_EDIT_WIFI_AP1_SSID);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_EDIT_WIFI_AP2_SSID);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_EDIT_WIFI_AP1_PASSWORD);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_EDIT_WIFI_AP2_PASSWORD);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_EDIT_WIFI_NTP_SERVER1);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_EDIT_WIFI_NTP_SERVER2);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_EDIT_WIFI_NTP_SERVER3);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_WIFI_UPDATE_INVERVAL);
    cobCtrl->EnableWindow(bState);

    cobCtrl = GetDlgItem(IDC_CHECK_USE_TOUCH);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_TOUCH_TO_SHOW);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_TOUCH_TO_SWITCH);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_TOUCH_TO_STOP_ALARM);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_TOUCH_SENSITIVITY);
    cobCtrl->EnableWindow(bState);

    cobCtrl = GetDlgItem(IDC_MFCEDITBROWSE_FONTS_FILE_PATH);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_COMBO_FONTS_PROGRAM_DESTINATION);
    cobCtrl->EnableWindow(bState);
    cobCtrl = GetDlgItem(IDC_BUTTON_FONTS_PROGRAM);
    cobCtrl->EnableWindow(bState);

    return;
}

/*******************************************************************************
- Function    : FUNC_SetAllComboBox
- Description : 该功能函数设置所有ComboBox控件的选项。
- Input       : --
- Output      : --
- Return      : --
- Others      :
*******************************************************************************/
VOID CFireFlyDlg::FUNC_SetAllComboBox()
{
    CString  strComboBoxContent;
    UINT     uiLoop;

    // 定时显示的模式ComboBox
    strComboBoxContent.Format(_T("周一至周日"));
    m_obCBXTimeShowGrp1WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp2WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp3WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp4WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp5WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp6WeekMode.AddString(strComboBoxContent);

    strComboBoxContent.Format(_T("周一至周五"));
    m_obCBXTimeShowGrp1WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp2WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp3WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp4WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp5WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp6WeekMode.AddString(strComboBoxContent);

    strComboBoxContent.Format(_T("周六至周日"));
    m_obCBXTimeShowGrp1WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp2WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp3WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp4WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp5WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp6WeekMode.AddString(strComboBoxContent);

    strComboBoxContent.Format(_T("周一"));
    m_obCBXTimeShowGrp1WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp2WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp3WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp4WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp5WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp6WeekMode.AddString(strComboBoxContent);

    strComboBoxContent.Format(_T("周二"));
    m_obCBXTimeShowGrp1WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp2WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp3WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp4WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp5WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp6WeekMode.AddString(strComboBoxContent);

    strComboBoxContent.Format(_T("周三"));
    m_obCBXTimeShowGrp1WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp2WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp3WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp4WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp5WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp6WeekMode.AddString(strComboBoxContent);

    strComboBoxContent.Format(_T("周四"));
    m_obCBXTimeShowGrp1WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp2WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp3WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp4WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp5WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp6WeekMode.AddString(strComboBoxContent);

    strComboBoxContent.Format(_T("周五"));
    m_obCBXTimeShowGrp1WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp2WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp3WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp4WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp5WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp6WeekMode.AddString(strComboBoxContent);

    strComboBoxContent.Format(_T("周六"));
    m_obCBXTimeShowGrp1WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp2WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp3WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp4WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp5WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp6WeekMode.AddString(strComboBoxContent);

    strComboBoxContent.Format(_T("周日"));
    m_obCBXTimeShowGrp1WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp2WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp3WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp4WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp5WeekMode.AddString(strComboBoxContent);
    m_obCBXTimeShowGrp6WeekMode.AddString(strComboBoxContent);

    // 定时显示的起始小时ComboBox
    for (uiLoop = 0; uiLoop <= 23; uiLoop++)
    {
        strComboBoxContent.Format(_T("%d"), uiLoop);
        m_obCBXTimeShowGrp1StartHour.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp2StartHour.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp3StartHour.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp4StartHour.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp5StartHour.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp6StartHour.AddString(strComboBoxContent);
    }

    // 定时显示的起始分钟ComboBox
    for (uiLoop = 0; uiLoop <= 59; uiLoop++)
    {
        strComboBoxContent.Format(_T("%d"), uiLoop);
        m_obCBXTimeShowGrp1StartMinute.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp2StartMinute.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp3StartMinute.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp4StartMinute.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp5StartMinute.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp6StartMinute.AddString(strComboBoxContent);
    }

    // 定时显示的结束小时ComboBox
    for (uiLoop = 0; uiLoop <= 23; uiLoop++)
    {
        strComboBoxContent.Format(_T("%d"), uiLoop);
        m_obCBXTimeShowGrp1EndHour.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp2EndHour.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp3EndHour.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp4EndHour.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp5EndHour.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp6EndHour.AddString(strComboBoxContent);
    }

    // 定时显示的结束分钟ComboBox
    for (uiLoop = 0; uiLoop <= 59; uiLoop++)
    {
        strComboBoxContent.Format(_T("%d"), uiLoop);
        m_obCBXTimeShowGrp1EndMinute.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp2EndMinute.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp3EndMinute.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp4EndMinute.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp5EndMinute.AddString(strComboBoxContent);
        m_obCBXTimeShowGrp6EndMinute.AddString(strComboBoxContent);
    }

    // 亮度调节ComboBox
    m_obCBXVFDBrightness.AddString(_T("关闭"));
    m_obCBXVFDBrightness.AddString(_T("1"));
    m_obCBXVFDBrightness.AddString(_T("2"));
    m_obCBXVFDBrightness.AddString(_T("3"));
    m_obCBXVFDBrightness.AddString(_T("4"));
    m_obCBXVFDBrightness.AddString(_T("5"));
    m_obCBXVFDBrightness.AddString(_T("6"));
    m_obCBXVFDBrightness.AddString(_T("7"));
    m_obCBXVFDBrightness.AddString(_T("8"));
    m_obCBXVFDBrightness.AddString(_T("9"));
    m_obCBXVFDBrightness.AddString(_T("最亮"));

    // 字体设置ComboBox
    m_obCBXFontsGroupSel.AddString(_T("第1套"));
    m_obCBXFontsGroupSel.AddString(_T("第2套"));
    m_obCBXFontsGroupSel.AddString(_T("第3套"));
    m_obCBXFontsGroupSel.AddString(_T("第4套"));
    m_obCBXFontsGroupSel.AddString(_T("第5套"));
    m_obCBXFontsGroupSel.AddString(_T("第6套"));
    m_obCBXFontsGroupSel.AddString(_T("第7套"));
    m_obCBXFontsGroupSel.AddString(_T("第8套"));
    m_obCBXFontsGroupSel.AddString(_T("第9套"));
    m_obCBXFontsGroupSel.AddString(_T("第10套"));

    // 遍历显示日期时，显示日期的时间
    m_obCBXEgroShowDateSeconds.AddString(_T("0秒"));
    m_obCBXEgroShowDateSeconds.AddString(_T("1秒"));
    m_obCBXEgroShowDateSeconds.AddString(_T("2秒"));
    m_obCBXEgroShowDateSeconds.AddString(_T("3秒"));
    m_obCBXEgroShowDateSeconds.AddString(_T("4秒"));
    m_obCBXEgroShowDateSeconds.AddString(_T("5秒"));
    m_obCBXEgroShowDateSeconds.AddString(_T("6秒"));
    m_obCBXEgroShowDateSeconds.AddString(_T("7秒"));
    m_obCBXEgroShowDateSeconds.AddString(_T("8秒"));

    // 遍历显示温度时，显示温度的时间
    m_obCBXEgroShowTempSeconds.AddString(_T("0秒"));
    m_obCBXEgroShowTempSeconds.AddString(_T("1秒"));
    m_obCBXEgroShowTempSeconds.AddString(_T("2秒"));
    m_obCBXEgroShowTempSeconds.AddString(_T("3秒"));
    m_obCBXEgroShowTempSeconds.AddString(_T("4秒"));
    m_obCBXEgroShowTempSeconds.AddString(_T("5秒"));
    m_obCBXEgroShowTempSeconds.AddString(_T("6秒"));
    m_obCBXEgroShowTempSeconds.AddString(_T("7秒"));
    m_obCBXEgroShowTempSeconds.AddString(_T("8秒"));

    // 遍历显示湿度时，显示湿度的时间
    m_obCBXEgroShowHumdSeconds.AddString(_T("0秒"));
    m_obCBXEgroShowHumdSeconds.AddString(_T("1秒"));
    m_obCBXEgroShowHumdSeconds.AddString(_T("2秒"));
    m_obCBXEgroShowHumdSeconds.AddString(_T("3秒"));
    m_obCBXEgroShowHumdSeconds.AddString(_T("4秒"));
    m_obCBXEgroShowHumdSeconds.AddString(_T("5秒"));
    m_obCBXEgroShowHumdSeconds.AddString(_T("6秒"));
    m_obCBXEgroShowHumdSeconds.AddString(_T("7秒"));
    m_obCBXEgroShowHumdSeconds.AddString(_T("8秒"));

    // 触碰灵敏度
    m_obCBXTouchSensetivity.AddString(_T("非常灵敏"));
    m_obCBXTouchSensetivity.AddString(_T("灵敏"));
    m_obCBXTouchSensetivity.AddString(_T("中等"));
    m_obCBXTouchSensetivity.AddString(_T("迟钝"));
    m_obCBXTouchSensetivity.AddString(_T("非常迟钝"));

    // WIFI更新时间间隔
    m_obCBXWIFIUpdateTimeInterval.AddString(_T("每小时"));
    m_obCBXWIFIUpdateTimeInterval.AddString(_T("每4小时"));
    m_obCBXWIFIUpdateTimeInterval.AddString(_T("每12小时"));
    m_obCBXWIFIUpdateTimeInterval.AddString(_T("每天"));
    m_obCBXWIFIUpdateTimeInterval.AddString(_T("每周"));
    m_obCBXWIFIUpdateTimeInterval.AddString(_T("每2周"));
    m_obCBXWIFIUpdateTimeInterval.AddString(_T("每月"));

    // 字体风格组
    m_obCBXFontsBMPGroup.AddString(_T("第1组"));
    m_obCBXFontsBMPGroup.AddString(_T("第2组"));
    m_obCBXFontsBMPGroup.AddString(_T("第3组"));
    m_obCBXFontsBMPGroup.AddString(_T("第4组"));
    m_obCBXFontsBMPGroup.AddString(_T("第5组"));
    m_obCBXFontsBMPGroup.AddString(_T("第6组"));
    m_obCBXFontsBMPGroup.AddString(_T("第7组"));
    m_obCBXFontsBMPGroup.AddString(_T("第8组"));
    m_obCBXFontsBMPGroup.AddString(_T("第9组"));
    m_obCBXFontsBMPGroup.AddString(_T("第10组"));

    return;
}

/***********************************************************************************************************
* 对话框类的消息处理函数
***********************************************************************************************************/

BOOL CFireFlyDlg::OnInitDialog()
{
    CString  strComboBoxContent;

	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
    // 未连接串口的时候，使所有的按钮都失效，避免操作
    FUNC_SetAllCtrlState(false);

    // 为所有的Combo控件添加选项
    FUNC_SetAllComboBox();

    // 遍历串口，将串口号放置在下拉框中
    FFY_COMM_ListComm2ComboBox(&m_obCBXCOMMPortSel);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CFireFlyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CFireFlyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD WINAPI XSleepThread(LPVOID pWaitTime)
{
    XSleep_Structure *sleep = (XSleep_Structure*)pWaitTime;

    Sleep(sleep->duration);
    SetEvent(sleep->evenHandle);

    return 0;
}

void XSleep(int nWaitInMsecs)
{
    XSleep_Structure sleep;
    sleep.duration = nWaitInMsecs;
    sleep.evenHandle = CreateEvent(NULL, TRUE, FALSE, NULL);

    DWORD dwThreadId;
    CreateThread(NULL, 0, &XSleepThread, &sleep, 0, &dwThreadId);

    MSG msg;
    while (::WaitForSingleObject(sleep.evenHandle, 0) == WAIT_TIMEOUT)
    {
        // get and dispatch message
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    CloseHandle(sleep.evenHandle);

    return;
}

/*******************************************************************************
- Function    : OnBnClickedButtonOpenCloseComm
- Description : 打开或者关闭串口。将创建或者销毁线程。
- Input       : --
- Output      : --
- Return      : --
- Others      :
*******************************************************************************/
void CFireFlyDlg::OnBnClickedButtonOpenCloseComm()
{
    CString  strSelComm;

    if (false == m_bCommOpened)
    {
        // 通信端口未打开，则打开通信端口
        m_obCBXCOMMPortSel.GetWindowTextW(strSelComm);

        if (5 == strSelComm.GetLength())
        {
            strSelComm = _T("\\\\.\\") + strSelComm;
        }

        g_hUART = CreateFile(strSelComm,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (INVALID_HANDLE_VALUE == g_hUART)
        {
            // 串口打开失败
            m_bCommOpened = false;
            AfxMessageBox(_T("打开串口失败！"), MB_OK | MB_ICONINFORMATION);

            return;
        }
        else
        {
            // 串口打开成功
            m_bCommOpened = true;
            m_obBTNOpenCloseComm.SetWindowTextW(_T("关闭串口"));
            m_obBTNOpenCloseComm.UpdateWindow();
        }

        // 获取串口默认配置信息
        GetCommState(g_hUART, &m_stUARTInfo);

        // 填写串口配置信息
        m_stUARTInfo.BaudRate = 115200;
        m_stUARTInfo.Parity = NOPARITY;
        m_stUARTInfo.fParity = 0;
        m_stUARTInfo.StopBits = ONESTOPBIT;
        m_stUARTInfo.ByteSize = 8;
        m_stUARTInfo.fOutxCtsFlow = 0;
        m_stUARTInfo.fOutxDsrFlow = 0;
        m_stUARTInfo.fDtrControl = DTR_CONTROL_DISABLE;
        m_stUARTInfo.fDsrSensitivity = 0;
        m_stUARTInfo.fRtsControl = RTS_CONTROL_DISABLE;
        m_stUARTInfo.fOutX = 0;
        m_stUARTInfo.fInX = 0;
        m_stUARTInfo.fErrorChar = 0;
        m_stUARTInfo.fBinary = 1;
        m_stUARTInfo.fNull = 0;
        m_stUARTInfo.fAbortOnError = 0;
        m_stUARTInfo.wReserved = 0;
        m_stUARTInfo.XonLim = 2;
        m_stUARTInfo.XoffLim = 4;
        m_stUARTInfo.XonChar = 0x13;
        m_stUARTInfo.XoffChar = 0x19;
        m_stUARTInfo.EvtChar = 0;

        // 配置串口
        SetCommState(g_hUART, &m_stUARTInfo);
        SetupComm(g_hUART, 8192, 8192);

        // 创建串口处理线程
        g_bNotifyStopThread = false;
        m_pobCommThread = AfxBeginThread((AFX_THREADPROC)FFY_COMM_ProcessCommThread, this, THREAD_PRIORITY_NORMAL, 0, 0, NULL);

        // 串口打开成功，功能按钮有效
        FUNC_SetAllCtrlState(true);
    }
    else
    {
        // 通告关闭串口处理线程
        g_bThreadStopped = false;
        g_bNotifyStopThread = true;

        // 等待线程结束
        //while (true != g_bThreadStopped);

        // 关闭串口
        CloseHandle(g_hUART);

        // 串口关闭成功，功能按钮无效
        FUNC_SetAllCtrlState(false);

        // 设置串口关闭标志
        m_bCommOpened = false;
        m_obBTNOpenCloseComm.SetWindowTextW(_T("打开串口"));
        m_obBTNOpenCloseComm.UpdateWindow();
    }

    return;
}


void CFireFlyDlg::OnBnClickedButtonGetProfile()
{
    // 发送消息
    if (0 != g_stDlg2CommMsg.ucValid)
    {
        // 消息没空间，放弃本次操作
        return;
    }
    else
    {
        g_stDlg2CommMsg.ucMsgDst = FFY_COMM_MSG_DST_F429;
        g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG1;
        g_stDlg2CommMsg.ucMsgLen = 0;
        g_stDlg2CommMsg.ucValid = 1;

        while (1 == g_stDlg2CommMsg.ucValid)
        {
            XSleep(10);
        }
        g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG2;
        g_stDlg2CommMsg.ucValid = 1;

        while (1 == g_stDlg2CommMsg.ucValid)
        {
            XSleep(10);
        }
        g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG3;
        g_stDlg2CommMsg.ucValid = 1;

        while (1 == g_stDlg2CommMsg.ucValid)
        {
            XSleep(10);
        }
        g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG4;
        g_stDlg2CommMsg.ucValid = 1;

        while (1 == g_stDlg2CommMsg.ucValid)
        {
            XSleep(10);
        }
        g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG5;
        g_stDlg2CommMsg.ucValid = 1;

        while (1 == g_stDlg2CommMsg.ucValid)
        {
            XSleep(10);
        }
        g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG6;
        g_stDlg2CommMsg.ucValid = 1;

        while (1 == g_stDlg2CommMsg.ucValid)
        {
            XSleep(10);
        }
        g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG7;
        g_stDlg2CommMsg.ucValid = 1;

        while (1 == g_stDlg2CommMsg.ucValid)
        {
            XSleep(10);
        }
        g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_RD_CONFIG_SEG8;
        g_stDlg2CommMsg.ucValid = 1;

        while (1 == g_stDlg2CommMsg.ucValid)
        {
            XSleep(10);
        }
    }

    return;
}

/*******************************************************************************
- Function    : OnProcessNewRecvMessage
- Description : 处理从通讯模块接收到的消息。
- Input       : --
- Output      : --
- Return      : --
- Others      :
*******************************************************************************/
LRESULT CFireFlyDlg::OnProcessNewRecvMessage(WPARAM wParam, LPARAM lParam)
{
    CWnd    *cobCtrl;
    static UINT     uiSegmentRecvFlag = 0;

    if (0 != g_stComm2DlgMsg.ucValid)
    {
        switch (g_stComm2DlgMsg.ucMsgType)
        {
        case FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG1:
            uiSegmentRecvFlag |= 0x01;
            memcpy(&g_stFFYConfigInfo.aucRawData[FFY_HST_CONFI_INFO_SEGMENT_SIZE * 0],
                g_stComm2DlgMsg.aucMsg,
                g_stComm2DlgMsg.ucMsgLen);
            break;
        case FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG2:
            uiSegmentRecvFlag |= 0x02;
            memcpy(&g_stFFYConfigInfo.aucRawData[FFY_HST_CONFI_INFO_SEGMENT_SIZE * 1],
                g_stComm2DlgMsg.aucMsg,
                g_stComm2DlgMsg.ucMsgLen);
            break;
        case FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG3:
            uiSegmentRecvFlag |= 0x04;
            memcpy(&g_stFFYConfigInfo.aucRawData[FFY_HST_CONFI_INFO_SEGMENT_SIZE * 2],
                g_stComm2DlgMsg.aucMsg,
                g_stComm2DlgMsg.ucMsgLen);
            break;
        case FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG4:
            uiSegmentRecvFlag |= 0x08;
            memcpy(&g_stFFYConfigInfo.aucRawData[FFY_HST_CONFI_INFO_SEGMENT_SIZE * 3],
                g_stComm2DlgMsg.aucMsg,
                g_stComm2DlgMsg.ucMsgLen);
            break;
        case FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG5:
            uiSegmentRecvFlag |= 0x10;
            memcpy(&g_stFFYConfigInfo.aucRawData[FFY_HST_CONFI_INFO_SEGMENT_SIZE * 4],
                g_stComm2DlgMsg.aucMsg,
                g_stComm2DlgMsg.ucMsgLen);
            break;
        case FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG6:
            uiSegmentRecvFlag |= 0x20;
            memcpy(&g_stFFYConfigInfo.aucRawData[FFY_HST_CONFI_INFO_SEGMENT_SIZE * 5],
                g_stComm2DlgMsg.aucMsg,
                g_stComm2DlgMsg.ucMsgLen);
            break;
        case FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG7:
            uiSegmentRecvFlag |= 0x40;
            memcpy(&g_stFFYConfigInfo.aucRawData[FFY_HST_CONFI_INFO_SEGMENT_SIZE * 6],
                g_stComm2DlgMsg.aucMsg,
                g_stComm2DlgMsg.ucMsgLen);
            break;
        case FFY_HST_L2_TYPE_RPL_RD_CONFIG_SEG8:
            uiSegmentRecvFlag |= 0x80;
            memcpy(&g_stFFYConfigInfo.aucRawData[FFY_HST_CONFI_INFO_SEGMENT_SIZE * 7],
                g_stComm2DlgMsg.aucMsg,
                g_stComm2DlgMsg.ucMsgLen);

            // 所有信息接收完毕，将信息显示在窗口
            uiSegmentRecvFlag = 0;
            cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW);
            ((CButton *)cobCtrl)->SetCheck(g_stFFYConfigInfo.uiEnableTimeShow);
            cobCtrl = GetDlgItem(IDC_CHECK_USE_EGRO_SHOW);
            ((CButton *)cobCtrl)->SetCheck(g_stFFYConfigInfo.uiEnableEgroShow);
            cobCtrl = GetDlgItem(IDC_CHECK_USE_TOUCH);
            ((CButton *)cobCtrl)->SetCheck(g_stFFYConfigInfo.uiEnableTouchSense);
            cobCtrl = GetDlgItem(IDC_CHECK_USE_WIFI);
            ((CButton *)cobCtrl)->SetCheck(g_stFFYConfigInfo.uiEnableWIFIUpdateTime);

            cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP1);
            ((CButton *)cobCtrl)->SetCheck(g_stFFYConfigInfo.stTimeShowInfo1.ucValid);
            cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP2);
            ((CButton *)cobCtrl)->SetCheck(g_stFFYConfigInfo.stTimeShowInfo2.ucValid);
            cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP3);
            ((CButton *)cobCtrl)->SetCheck(g_stFFYConfigInfo.stTimeShowInfo3.ucValid);
            cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP4);
            ((CButton *)cobCtrl)->SetCheck(g_stFFYConfigInfo.stTimeShowInfo4.ucValid);
            cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP5);
            ((CButton *)cobCtrl)->SetCheck(g_stFFYConfigInfo.stTimeShowInfo5.ucValid);
            cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP6);
            ((CButton *)cobCtrl)->SetCheck(g_stFFYConfigInfo.stTimeShowInfo6.ucValid);

            m_obCBXVFDBrightness.SetCurSel(g_stFFYConfigInfo.enLCDBrightness);
            m_obCBXFontsGroupSel.SetCurSel((g_stFFYConfigInfo.uiCharGroup > 9) ? 9 : g_stFFYConfigInfo.uiCharGroup);

            if (1 == g_stFFYConfigInfo.stTimeShowInfo1.ucMonday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo1.ucTuesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo1.ucWednesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo1.ucThursday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo1.ucFriday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo1.ucSaturday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo1.ucSunday)
            {
                m_obCBXTimeShowGrp1WeekMode.SetCurSel(0);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo1.ucMonday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo1.ucTuesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo1.ucWednesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo1.ucThursday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo1.ucFriday)
            {
                m_obCBXTimeShowGrp1WeekMode.SetCurSel(1);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo1.ucSaturday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo1.ucSunday)
            {
                m_obCBXTimeShowGrp1WeekMode.SetCurSel(2);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo1.ucMonday)
            {
                m_obCBXTimeShowGrp1WeekMode.SetCurSel(3);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo1.ucTuesday)
            {
                m_obCBXTimeShowGrp1WeekMode.SetCurSel(4);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo1.ucWednesday)
            {
                m_obCBXTimeShowGrp1WeekMode.SetCurSel(5);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo1.ucThursday)
            {
                m_obCBXTimeShowGrp1WeekMode.SetCurSel(6);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo1.ucFriday)
            {
                m_obCBXTimeShowGrp1WeekMode.SetCurSel(7);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo1.ucSaturday)
            {
                m_obCBXTimeShowGrp1WeekMode.SetCurSel(8);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo1.ucSunday)
            {
                m_obCBXTimeShowGrp1WeekMode.SetCurSel(9);
            }

            if (1 == g_stFFYConfigInfo.stTimeShowInfo2.ucMonday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo2.ucTuesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo2.ucWednesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo2.ucThursday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo2.ucFriday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo2.ucSaturday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo2.ucSunday)
            {
                m_obCBXTimeShowGrp2WeekMode.SetCurSel(0);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo2.ucMonday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo2.ucTuesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo2.ucWednesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo2.ucThursday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo2.ucFriday)
            {
                m_obCBXTimeShowGrp2WeekMode.SetCurSel(1);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo2.ucSaturday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo2.ucSunday)
            {
                m_obCBXTimeShowGrp2WeekMode.SetCurSel(2);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo2.ucMonday)
            {
                m_obCBXTimeShowGrp2WeekMode.SetCurSel(3);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo2.ucTuesday)
            {
                m_obCBXTimeShowGrp2WeekMode.SetCurSel(4);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo2.ucWednesday)
            {
                m_obCBXTimeShowGrp2WeekMode.SetCurSel(5);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo2.ucThursday)
            {
                m_obCBXTimeShowGrp2WeekMode.SetCurSel(6);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo2.ucFriday)
            {
                m_obCBXTimeShowGrp2WeekMode.SetCurSel(7);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo2.ucSaturday)
            {
                m_obCBXTimeShowGrp2WeekMode.SetCurSel(8);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo2.ucSunday)
            {
                m_obCBXTimeShowGrp2WeekMode.SetCurSel(9);
            }

            if (1 == g_stFFYConfigInfo.stTimeShowInfo3.ucMonday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo3.ucTuesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo3.ucWednesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo3.ucThursday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo3.ucFriday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo3.ucSaturday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo3.ucSunday)
            {
                m_obCBXTimeShowGrp3WeekMode.SetCurSel(0);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo3.ucMonday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo3.ucTuesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo3.ucWednesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo3.ucThursday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo3.ucFriday)
            {
                m_obCBXTimeShowGrp3WeekMode.SetCurSel(1);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo3.ucSaturday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo3.ucSunday)
            {
                m_obCBXTimeShowGrp3WeekMode.SetCurSel(2);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo3.ucMonday)
            {
                m_obCBXTimeShowGrp3WeekMode.SetCurSel(3);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo3.ucTuesday)
            {
                m_obCBXTimeShowGrp3WeekMode.SetCurSel(4);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo3.ucWednesday)
            {
                m_obCBXTimeShowGrp3WeekMode.SetCurSel(5);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo3.ucThursday)
            {
                m_obCBXTimeShowGrp3WeekMode.SetCurSel(6);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo3.ucFriday)
            {
                m_obCBXTimeShowGrp3WeekMode.SetCurSel(7);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo3.ucSaturday)
            {
                m_obCBXTimeShowGrp3WeekMode.SetCurSel(8);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo3.ucSunday)
            {
                m_obCBXTimeShowGrp3WeekMode.SetCurSel(9);
            }

            if (1 == g_stFFYConfigInfo.stTimeShowInfo4.ucMonday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo4.ucTuesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo4.ucWednesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo4.ucThursday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo4.ucFriday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo4.ucSaturday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo4.ucSunday)
            {
                m_obCBXTimeShowGrp4WeekMode.SetCurSel(0);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo4.ucMonday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo4.ucTuesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo4.ucWednesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo4.ucThursday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo4.ucFriday)
            {
                m_obCBXTimeShowGrp4WeekMode.SetCurSel(1);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo4.ucSaturday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo4.ucSunday)
            {
                m_obCBXTimeShowGrp4WeekMode.SetCurSel(2);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo4.ucMonday)
            {
                m_obCBXTimeShowGrp4WeekMode.SetCurSel(3);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo4.ucTuesday)
            {
                m_obCBXTimeShowGrp4WeekMode.SetCurSel(4);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo4.ucWednesday)
            {
                m_obCBXTimeShowGrp4WeekMode.SetCurSel(5);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo4.ucThursday)
            {
                m_obCBXTimeShowGrp4WeekMode.SetCurSel(6);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo4.ucFriday)
            {
                m_obCBXTimeShowGrp4WeekMode.SetCurSel(7);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo4.ucSaturday)
            {
                m_obCBXTimeShowGrp4WeekMode.SetCurSel(8);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo4.ucSunday)
            {
                m_obCBXTimeShowGrp4WeekMode.SetCurSel(9);
            }

            if (1 == g_stFFYConfigInfo.stTimeShowInfo5.ucMonday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo5.ucTuesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo5.ucWednesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo5.ucThursday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo5.ucFriday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo5.ucSaturday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo5.ucSunday)
            {
                m_obCBXTimeShowGrp5WeekMode.SetCurSel(0);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo5.ucMonday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo5.ucTuesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo5.ucWednesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo5.ucThursday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo5.ucFriday)
            {
                m_obCBXTimeShowGrp5WeekMode.SetCurSel(1);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo5.ucSaturday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo5.ucSunday)
            {
                m_obCBXTimeShowGrp5WeekMode.SetCurSel(2);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo5.ucMonday)
            {
                m_obCBXTimeShowGrp5WeekMode.SetCurSel(3);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo5.ucTuesday)
            {
                m_obCBXTimeShowGrp5WeekMode.SetCurSel(4);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo5.ucWednesday)
            {
                m_obCBXTimeShowGrp5WeekMode.SetCurSel(5);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo5.ucThursday)
            {
                m_obCBXTimeShowGrp5WeekMode.SetCurSel(6);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo5.ucFriday)
            {
                m_obCBXTimeShowGrp5WeekMode.SetCurSel(7);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo5.ucSaturday)
            {
                m_obCBXTimeShowGrp5WeekMode.SetCurSel(8);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo5.ucSunday)
            {
                m_obCBXTimeShowGrp5WeekMode.SetCurSel(9);
            }

            if (1 == g_stFFYConfigInfo.stTimeShowInfo6.ucMonday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo6.ucTuesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo6.ucWednesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo6.ucThursday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo6.ucFriday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo6.ucSaturday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo6.ucSunday)
            {
                m_obCBXTimeShowGrp6WeekMode.SetCurSel(0);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo6.ucMonday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo6.ucTuesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo6.ucWednesday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo6.ucThursday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo6.ucFriday)
            {
                m_obCBXTimeShowGrp6WeekMode.SetCurSel(1);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo6.ucSaturday &&
                1 == g_stFFYConfigInfo.stTimeShowInfo6.ucSunday)
            {
                m_obCBXTimeShowGrp6WeekMode.SetCurSel(2);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo6.ucMonday)
            {
                m_obCBXTimeShowGrp6WeekMode.SetCurSel(3);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo6.ucTuesday)
            {
                m_obCBXTimeShowGrp6WeekMode.SetCurSel(4);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo6.ucWednesday)
            {
                m_obCBXTimeShowGrp6WeekMode.SetCurSel(5);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo6.ucThursday)
            {
                m_obCBXTimeShowGrp6WeekMode.SetCurSel(6);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo6.ucFriday)
            {
                m_obCBXTimeShowGrp6WeekMode.SetCurSel(7);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo6.ucSaturday)
            {
                m_obCBXTimeShowGrp6WeekMode.SetCurSel(8);
            }
            else if (1 == g_stFFYConfigInfo.stTimeShowInfo6.ucSunday)
            {
                m_obCBXTimeShowGrp6WeekMode.SetCurSel(9);
            }

            m_obCBXTimeShowGrp1StartHour.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo1.ucStartHour);
            m_obCBXTimeShowGrp2StartHour.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo2.ucStartHour);
            m_obCBXTimeShowGrp3StartHour.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo3.ucStartHour);
            m_obCBXTimeShowGrp4StartHour.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo4.ucStartHour);
            m_obCBXTimeShowGrp5StartHour.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo5.ucStartHour);
            m_obCBXTimeShowGrp6StartHour.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo6.ucStartHour);

            m_obCBXTimeShowGrp1StartMinute.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo1.ucStartMinute);
            m_obCBXTimeShowGrp2StartMinute.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo2.ucStartMinute);
            m_obCBXTimeShowGrp3StartMinute.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo3.ucStartMinute);
            m_obCBXTimeShowGrp4StartMinute.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo4.ucStartMinute);
            m_obCBXTimeShowGrp5StartMinute.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo5.ucStartMinute);
            m_obCBXTimeShowGrp6StartMinute.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo6.ucStartMinute);

            m_obCBXTimeShowGrp1EndHour.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo1.ucEndHour);
            m_obCBXTimeShowGrp2EndHour.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo2.ucEndHour);
            m_obCBXTimeShowGrp3EndHour.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo3.ucEndHour);
            m_obCBXTimeShowGrp4EndHour.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo4.ucEndHour);
            m_obCBXTimeShowGrp5EndHour.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo5.ucEndHour);
            m_obCBXTimeShowGrp6EndHour.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo6.ucEndHour);

            m_obCBXTimeShowGrp1EndMinute.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo1.ucEndMinute);
            m_obCBXTimeShowGrp2EndMinute.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo2.ucEndMinute);
            m_obCBXTimeShowGrp3EndMinute.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo3.ucEndMinute);
            m_obCBXTimeShowGrp4EndMinute.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo4.ucEndMinute);
            m_obCBXTimeShowGrp5EndMinute.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo5.ucEndMinute);
            m_obCBXTimeShowGrp6EndMinute.SetCurSel(g_stFFYConfigInfo.stTimeShowInfo6.ucEndMinute);

            cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_EGRO_SHOW_DATE);
            ((CButton *)cobCtrl)->SetCheck(g_stFFYConfigInfo.stErgoShowInfo.ucShowDate);
            cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_EGRO_SHOW_TEMP);
            ((CButton *)cobCtrl)->SetCheck(g_stFFYConfigInfo.stErgoShowInfo.ucShowTemp);
            cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_EGRO_SHOW_HUMD);
            ((CButton *)cobCtrl)->SetCheck(g_stFFYConfigInfo.stErgoShowInfo.ucShowHumd);

            m_obCBXEgroShowDateSeconds.SetCurSel(g_stFFYConfigInfo.stErgoShowInfo.ucShowDateSecondsPerMinute);
            m_obCBXEgroShowTempSeconds.SetCurSel(g_stFFYConfigInfo.stErgoShowInfo.ucShowTempSecondsPerMinute);
            m_obCBXEgroShowHumdSeconds.SetCurSel(g_stFFYConfigInfo.stErgoShowInfo.ucShowHumdSecondsPerMinute);

            cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_TOUCH_TO_SHOW);
            ((CButton *)cobCtrl)->SetCheck(g_stFFYConfigInfo.stTouchConfigInfo.ucTouchToShow);
            cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_TOUCH_TO_SWITCH);
            ((CButton *)cobCtrl)->SetCheck(g_stFFYConfigInfo.stTouchConfigInfo.ucTouchToSwitch);
            cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_TOUCH_TO_STOP_ALARM);
            ((CButton *)cobCtrl)->SetCheck(g_stFFYConfigInfo.stTouchConfigInfo.ucTouchToStopAlarm);

            m_obCBXTouchSensetivity.SetCurSel(g_stFFYConfigInfo.stTouchConfigInfo.enTouchSenseLevel);

            m_obCBXWIFIUpdateTimeInterval.SetCurSel(g_stFFYConfigInfo.stWIFIConfigInfo.enUpdateIntervel);
            m_obEDTAP1SSID.SetWindowText(char2wchar(g_stFFYConfigInfo.stWIFIConfigInfo.stAP1Info.acSSID));
            m_obEDTAP2SSID.SetWindowText(char2wchar(g_stFFYConfigInfo.stWIFIConfigInfo.stAP2Info.acSSID));
            m_obEDTAP1Password.SetWindowText(char2wchar(g_stFFYConfigInfo.stWIFIConfigInfo.stAP1Info.acPassword));
            m_obEDTAP2Password.SetWindowText(char2wchar(g_stFFYConfigInfo.stWIFIConfigInfo.stAP2Info.acPassword));
            m_obEDTNTPServer1.SetWindowText(char2wchar(g_stFFYConfigInfo.stWIFIConfigInfo.stNTPServer1.acNTPServer));
            m_obEDTNTPServer2.SetWindowText(char2wchar(g_stFFYConfigInfo.stWIFIConfigInfo.stNTPServer2.acNTPServer));
            m_obEDTNTPServer3.SetWindowText(char2wchar(g_stFFYConfigInfo.stWIFIConfigInfo.stNTPServer3.acNTPServer));

            break;
        default:
            break;
        }
        g_stComm2DlgMsg.ucValid = 0;
    }

    return 0;
}


void CFireFlyDlg::OnBnClickedButtonStoreProfile()
{
    CWnd    *cobCtrl;

    cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW);
    g_stFFYConfigInfo.uiEnableTimeShow = ((CButton *)cobCtrl)->GetCheck();
    cobCtrl = GetDlgItem(IDC_CHECK_USE_EGRO_SHOW);
    g_stFFYConfigInfo.uiEnableEgroShow = ((CButton *)cobCtrl)->GetCheck();
    cobCtrl = GetDlgItem(IDC_CHECK_USE_TOUCH);
    g_stFFYConfigInfo.uiEnableTouchSense = ((CButton *)cobCtrl)->GetCheck();
    cobCtrl = GetDlgItem(IDC_CHECK_USE_WIFI);
    g_stFFYConfigInfo.uiEnableWIFIUpdateTime = ((CButton *)cobCtrl)->GetCheck();

    cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP1);
    g_stFFYConfigInfo.stTimeShowInfo1.ucValid = ((CButton *)cobCtrl)->GetCheck();
    cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP2);
    g_stFFYConfigInfo.stTimeShowInfo2.ucValid = ((CButton *)cobCtrl)->GetCheck();
    cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP3);
    g_stFFYConfigInfo.stTimeShowInfo3.ucValid = ((CButton *)cobCtrl)->GetCheck();
    cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP4);
    g_stFFYConfigInfo.stTimeShowInfo4.ucValid = ((CButton *)cobCtrl)->GetCheck();
    cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP5);
    g_stFFYConfigInfo.stTimeShowInfo5.ucValid = ((CButton *)cobCtrl)->GetCheck();
    cobCtrl = GetDlgItem(IDC_CHECK_USE_TIME_SHOW_GRP6);
    g_stFFYConfigInfo.stTimeShowInfo6.ucValid = ((CButton *)cobCtrl)->GetCheck();

    g_stFFYConfigInfo.enLCDBrightness = (FFY_CONFIG_VFD_BRSS_DEF_E)m_obCBXVFDBrightness.GetCurSel();
    g_stFFYConfigInfo.uiCharGroup = m_obCBXFontsGroupSel.GetCurSel();

    if (0 == m_obCBXTimeShowGrp1WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo1.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSunday = 1;
    }
    else if (1 == m_obCBXTimeShowGrp1WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo1.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSunday = 0;
    }
    else if (2 == m_obCBXTimeShowGrp1WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo1.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSunday = 1;
    }
    else if (3 == m_obCBXTimeShowGrp1WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo1.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSunday = 0;
    }
    else if (4 == m_obCBXTimeShowGrp1WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo1.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSunday = 0;
    }
    else if (5 == m_obCBXTimeShowGrp1WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo1.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSunday = 0;
    }
    else if (6 == m_obCBXTimeShowGrp1WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo1.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSunday = 0;
    }
    else if (7 == m_obCBXTimeShowGrp1WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo1.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSunday = 0;
    }
    else if (8 == m_obCBXTimeShowGrp1WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo1.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSunday = 0;
    }
    else if (9 == m_obCBXTimeShowGrp1WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo1.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo1.ucSunday = 1;
    }

    if (0 == m_obCBXTimeShowGrp2WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo2.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSunday = 1;
    }
    else if (1 == m_obCBXTimeShowGrp2WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo2.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSunday = 0;
    }
    else if (2 == m_obCBXTimeShowGrp2WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo2.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSunday = 1;
    }
    else if (3 == m_obCBXTimeShowGrp2WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo2.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSunday = 0;
    }
    else if (4 == m_obCBXTimeShowGrp2WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo2.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSunday = 0;
    }
    else if (5 == m_obCBXTimeShowGrp2WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo2.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSunday = 0;
    }
    else if (6 == m_obCBXTimeShowGrp2WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo2.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSunday = 0;
    }
    else if (7 == m_obCBXTimeShowGrp2WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo2.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSunday = 0;
    }
    else if (8 == m_obCBXTimeShowGrp2WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo2.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSunday = 0;
    }
    else if (9 == m_obCBXTimeShowGrp2WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo2.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo2.ucSunday = 1;
    }

    if (0 == m_obCBXTimeShowGrp3WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo3.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSunday = 1;
    }
    else if (1 == m_obCBXTimeShowGrp3WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo3.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSunday = 0;
    }
    else if (2 == m_obCBXTimeShowGrp3WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo3.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSunday = 1;
    }
    else if (3 == m_obCBXTimeShowGrp3WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo3.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSunday = 0;
    }
    else if (4 == m_obCBXTimeShowGrp3WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo3.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSunday = 0;
    }
    else if (5 == m_obCBXTimeShowGrp3WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo3.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSunday = 0;
    }
    else if (6 == m_obCBXTimeShowGrp3WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo3.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSunday = 0;
    }
    else if (7 == m_obCBXTimeShowGrp3WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo3.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSunday = 0;
    }
    else if (8 == m_obCBXTimeShowGrp3WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo3.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSunday = 0;
    }
    else if (9 == m_obCBXTimeShowGrp3WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo3.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo3.ucSunday = 1;
    }

    if (0 == m_obCBXTimeShowGrp4WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo4.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSunday = 1;
    }
    else if (1 == m_obCBXTimeShowGrp4WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo4.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSunday = 0;
    }
    else if (2 == m_obCBXTimeShowGrp4WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo4.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSunday = 1;
    }
    else if (3 == m_obCBXTimeShowGrp4WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo4.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSunday = 0;
    }
    else if (4 == m_obCBXTimeShowGrp4WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo4.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSunday = 0;
    }
    else if (5 == m_obCBXTimeShowGrp4WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo4.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSunday = 0;
    }
    else if (6 == m_obCBXTimeShowGrp4WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo4.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSunday = 0;
    }
    else if (7 == m_obCBXTimeShowGrp4WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo4.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSunday = 0;
    }
    else if (8 == m_obCBXTimeShowGrp4WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo4.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSunday = 0;
    }
    else if (9 == m_obCBXTimeShowGrp4WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo4.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo4.ucSunday = 1;
    }

    if (0 == m_obCBXTimeShowGrp5WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo5.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSunday = 1;
    }
    else if (1 == m_obCBXTimeShowGrp5WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo5.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSunday = 0;
    }
    else if (2 == m_obCBXTimeShowGrp5WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo5.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSunday = 1;
    }
    else if (3 == m_obCBXTimeShowGrp5WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo5.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSunday = 0;
    }
    else if (4 == m_obCBXTimeShowGrp5WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo5.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSunday = 0;
    }
    else if (5 == m_obCBXTimeShowGrp5WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo5.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSunday = 0;
    }
    else if (6 == m_obCBXTimeShowGrp5WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo5.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSunday = 0;
    }
    else if (7 == m_obCBXTimeShowGrp5WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo5.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSunday = 0;
    }
    else if (8 == m_obCBXTimeShowGrp5WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo5.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSunday = 0;
    }
    else if (9 == m_obCBXTimeShowGrp5WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo5.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo5.ucSunday = 1;
    }

    if (0 == m_obCBXTimeShowGrp6WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo6.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSunday = 1;
    }
    else if (1 == m_obCBXTimeShowGrp6WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo6.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSunday = 0;
    }
    else if (2 == m_obCBXTimeShowGrp6WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo6.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSunday = 1;
    }
    else if (3 == m_obCBXTimeShowGrp6WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo6.ucMonday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSunday = 0;
    }
    else if (4 == m_obCBXTimeShowGrp6WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo6.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucTuesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSunday = 0;
    }
    else if (5 == m_obCBXTimeShowGrp6WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo6.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucWednesday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSunday = 0;
    }
    else if (6 == m_obCBXTimeShowGrp6WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo6.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucThursday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSunday = 0;
    }
    else if (7 == m_obCBXTimeShowGrp6WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo6.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucFriday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSunday = 0;
    }
    else if (8 == m_obCBXTimeShowGrp6WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo6.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSaturday = 1;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSunday = 0;
    }
    else if (9 == m_obCBXTimeShowGrp6WeekMode.GetCurSel())
    {
        g_stFFYConfigInfo.stTimeShowInfo6.ucMonday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucTuesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucWednesday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucThursday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucFriday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSaturday = 0;
        g_stFFYConfigInfo.stTimeShowInfo6.ucSunday = 1;
    }

    g_stFFYConfigInfo.stTimeShowInfo1.ucStartHour = m_obCBXTimeShowGrp1StartHour.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo2.ucStartHour = m_obCBXTimeShowGrp2StartHour.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo3.ucStartHour = m_obCBXTimeShowGrp3StartHour.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo4.ucStartHour = m_obCBXTimeShowGrp4StartHour.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo5.ucStartHour = m_obCBXTimeShowGrp5StartHour.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo6.ucStartHour = m_obCBXTimeShowGrp6StartHour.GetCurSel();

    g_stFFYConfigInfo.stTimeShowInfo1.ucStartMinute = m_obCBXTimeShowGrp1StartMinute.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo2.ucStartMinute = m_obCBXTimeShowGrp2StartMinute.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo3.ucStartMinute = m_obCBXTimeShowGrp3StartMinute.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo4.ucStartMinute = m_obCBXTimeShowGrp4StartMinute.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo5.ucStartMinute = m_obCBXTimeShowGrp5StartMinute.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo6.ucStartMinute = m_obCBXTimeShowGrp6StartMinute.GetCurSel();

    g_stFFYConfigInfo.stTimeShowInfo1.ucEndHour = m_obCBXTimeShowGrp1EndHour.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo2.ucEndHour = m_obCBXTimeShowGrp2EndHour.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo3.ucEndHour = m_obCBXTimeShowGrp3EndHour.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo4.ucEndHour = m_obCBXTimeShowGrp4EndHour.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo5.ucEndHour = m_obCBXTimeShowGrp5EndHour.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo6.ucEndHour = m_obCBXTimeShowGrp6EndHour.GetCurSel();

    g_stFFYConfigInfo.stTimeShowInfo1.ucEndMinute = m_obCBXTimeShowGrp1EndMinute.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo2.ucEndMinute = m_obCBXTimeShowGrp2EndMinute.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo3.ucEndMinute = m_obCBXTimeShowGrp3EndMinute.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo4.ucEndMinute = m_obCBXTimeShowGrp4EndMinute.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo5.ucEndMinute = m_obCBXTimeShowGrp5EndMinute.GetCurSel();
    g_stFFYConfigInfo.stTimeShowInfo6.ucEndMinute = m_obCBXTimeShowGrp6EndMinute.GetCurSel();

    cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_EGRO_SHOW_DATE);
    g_stFFYConfigInfo.stErgoShowInfo.ucShowDate = ((CButton *)cobCtrl)->GetCheck();
    cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_EGRO_SHOW_TEMP);
    g_stFFYConfigInfo.stErgoShowInfo.ucShowTemp = ((CButton *)cobCtrl)->GetCheck();
    cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_EGRO_SHOW_HUMD);
    g_stFFYConfigInfo.stErgoShowInfo.ucShowHumd = ((CButton *)cobCtrl)->GetCheck();

    g_stFFYConfigInfo.stErgoShowInfo.ucShowDateSecondsPerMinute = m_obCBXEgroShowDateSeconds.GetCurSel();
    g_stFFYConfigInfo.stErgoShowInfo.ucShowTempSecondsPerMinute = m_obCBXEgroShowTempSeconds.GetCurSel();
    g_stFFYConfigInfo.stErgoShowInfo.ucShowHumdSecondsPerMinute = m_obCBXEgroShowHumdSeconds.GetCurSel();

    cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_TOUCH_TO_SHOW);
    g_stFFYConfigInfo.stTouchConfigInfo.ucTouchToShow = ((CButton *)cobCtrl)->GetCheck();
    cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_TOUCH_TO_SWITCH);
    g_stFFYConfigInfo.stTouchConfigInfo.ucTouchToSwitch = ((CButton *)cobCtrl)->GetCheck();
    cobCtrl = GetDlgItem(IDC_CHECK_ENABLE_TOUCH_TO_STOP_ALARM);
    g_stFFYConfigInfo.stTouchConfigInfo.ucTouchToStopAlarm = ((CButton *)cobCtrl)->GetCheck();

    g_stFFYConfigInfo.stTouchConfigInfo.enTouchSenseLevel = (FFY_CONFIG_TOUCH_SENSE_LVL_E)m_obCBXTouchSensetivity.GetCurSel();

    g_stFFYConfigInfo.stWIFIConfigInfo.enUpdateIntervel = (FFY_CONFIG_WIFI_UPDATE_TIME_E)m_obCBXWIFIUpdateTimeInterval.GetCurSel();

    CString strTemp;

    m_obEDTAP1SSID.GetWindowText(strTemp);
    USES_CONVERSION;
    sprintf_s(g_stFFYConfigInfo.stWIFIConfigInfo.stAP1Info.acSSID, "%s", T2A(strTemp));
    if (0 != strlen(g_stFFYConfigInfo.stWIFIConfigInfo.stAP1Info.acSSID))
    {
        g_stFFYConfigInfo.stWIFIConfigInfo.stAP1Info.ucValid = 1;
    }
    else
    {
        g_stFFYConfigInfo.stWIFIConfigInfo.stAP1Info.ucValid = 0;
    }

    m_obEDTAP2SSID.GetWindowText(strTemp);
    sprintf_s(g_stFFYConfigInfo.stWIFIConfigInfo.stAP2Info.acSSID, "%s", T2A(strTemp));
    if (0 != strlen(g_stFFYConfigInfo.stWIFIConfigInfo.stAP1Info.acSSID))
    {
        g_stFFYConfigInfo.stWIFIConfigInfo.stAP2Info.ucValid = 1;
    }
    else
    {
        g_stFFYConfigInfo.stWIFIConfigInfo.stAP2Info.ucValid = 0;
    }

    m_obEDTAP1Password.GetWindowText(strTemp);
    sprintf_s(g_stFFYConfigInfo.stWIFIConfigInfo.stAP1Info.acPassword, "%s", T2A(strTemp));

    m_obEDTAP2Password.GetWindowText(strTemp);
    sprintf_s(g_stFFYConfigInfo.stWIFIConfigInfo.stAP2Info.acPassword, "%s", T2A(strTemp));

    m_obEDTNTPServer1.GetWindowText(strTemp);
    sprintf_s(g_stFFYConfigInfo.stWIFIConfigInfo.stNTPServer1.acNTPServer, "%s", T2A(strTemp));
    if (0 != strlen(g_stFFYConfigInfo.stWIFIConfigInfo.stNTPServer1.acNTPServer))
    {
        g_stFFYConfigInfo.stWIFIConfigInfo.stNTPServer1.ucValid = 1;
    }
    else
    {
        g_stFFYConfigInfo.stWIFIConfigInfo.stNTPServer1.ucValid = 0;
    }

    m_obEDTNTPServer2.GetWindowText(strTemp);
    sprintf_s(g_stFFYConfigInfo.stWIFIConfigInfo.stNTPServer2.acNTPServer, "%s", T2A(strTemp));
    if (0 != strlen(g_stFFYConfigInfo.stWIFIConfigInfo.stNTPServer2.acNTPServer))
    {
        g_stFFYConfigInfo.stWIFIConfigInfo.stNTPServer2.ucValid = 1;
    }
    else
    {
        g_stFFYConfigInfo.stWIFIConfigInfo.stNTPServer2.ucValid = 0;
    }

    m_obEDTNTPServer3.GetWindowText(strTemp);
    sprintf_s(g_stFFYConfigInfo.stWIFIConfigInfo.stNTPServer3.acNTPServer, "%s", T2A(strTemp));
    if (0 != strlen(g_stFFYConfigInfo.stWIFIConfigInfo.stNTPServer3.acNTPServer))
    {
        g_stFFYConfigInfo.stWIFIConfigInfo.stNTPServer3.ucValid = 1;
    }
    else
    {
        g_stFFYConfigInfo.stWIFIConfigInfo.stNTPServer3.ucValid = 0;
    }

    // 发送消息
    if (0 != g_stDlg2CommMsg.ucValid)
    {
        // 消息没空间，放弃本次操作
        return;
    }
    else
    {
        g_stDlg2CommMsg.ucMsgDst = FFY_COMM_MSG_DST_F429;
        g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG1;
        g_stDlg2CommMsg.ucMsgLen = FFY_HST_CONFI_INFO_SEGMENT_SIZE;
        memcpy(g_stDlg2CommMsg.aucMsg, &g_stFFYConfigInfo.aucRawData[FFY_HST_CONFI_INFO_SEGMENT_SIZE * 0], FFY_HST_CONFI_INFO_SEGMENT_SIZE);
        g_stDlg2CommMsg.ucValid = 1;

        while (1 == g_stDlg2CommMsg.ucValid)
        {
            XSleep(10);
        }
        g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG2;
        memcpy(g_stDlg2CommMsg.aucMsg, &g_stFFYConfigInfo.aucRawData[FFY_HST_CONFI_INFO_SEGMENT_SIZE * 1], FFY_HST_CONFI_INFO_SEGMENT_SIZE);
        g_stDlg2CommMsg.ucValid = 1;

        while (1 == g_stDlg2CommMsg.ucValid)
        {
            XSleep(10);
        }
        g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG3;
        memcpy(g_stDlg2CommMsg.aucMsg, &g_stFFYConfigInfo.aucRawData[FFY_HST_CONFI_INFO_SEGMENT_SIZE * 2], FFY_HST_CONFI_INFO_SEGMENT_SIZE);
        g_stDlg2CommMsg.ucValid = 1;

        while (1 == g_stDlg2CommMsg.ucValid)
        {
            XSleep(10);
        }
        g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG4;
        memcpy(g_stDlg2CommMsg.aucMsg, &g_stFFYConfigInfo.aucRawData[FFY_HST_CONFI_INFO_SEGMENT_SIZE * 3], FFY_HST_CONFI_INFO_SEGMENT_SIZE);
        g_stDlg2CommMsg.ucValid = 1;

        while (1 == g_stDlg2CommMsg.ucValid)
        {
            XSleep(10);
        }
        g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG5;
        memcpy(g_stDlg2CommMsg.aucMsg, &g_stFFYConfigInfo.aucRawData[FFY_HST_CONFI_INFO_SEGMENT_SIZE * 4], FFY_HST_CONFI_INFO_SEGMENT_SIZE);
        g_stDlg2CommMsg.ucValid = 1;

        while (1 == g_stDlg2CommMsg.ucValid)
        {
            XSleep(10);
        }
        g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG6;
        memcpy(g_stDlg2CommMsg.aucMsg, &g_stFFYConfigInfo.aucRawData[FFY_HST_CONFI_INFO_SEGMENT_SIZE * 5], FFY_HST_CONFI_INFO_SEGMENT_SIZE);
        g_stDlg2CommMsg.ucValid = 1;

        while (1 == g_stDlg2CommMsg.ucValid)
        {
            XSleep(10);
        }
        g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG7;
        memcpy(g_stDlg2CommMsg.aucMsg, &g_stFFYConfigInfo.aucRawData[FFY_HST_CONFI_INFO_SEGMENT_SIZE * 6], FFY_HST_CONFI_INFO_SEGMENT_SIZE);
        g_stDlg2CommMsg.ucValid = 1;

        while (1 == g_stDlg2CommMsg.ucValid)
        {
            XSleep(10);
        }
        g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_WR_CONFIG_SEG8;
        memcpy(g_stDlg2CommMsg.aucMsg, &g_stFFYConfigInfo.aucRawData[FFY_HST_CONFI_INFO_SEGMENT_SIZE * 7], FFY_HST_CONFI_INFO_SEGMENT_SIZE);
        g_stDlg2CommMsg.ucValid = 1;

        while (1 == g_stDlg2CommMsg.ucValid)
        {
            XSleep(10);
        }
    }
    
    return;
}


void CFireFlyDlg::OnBnClickedButtonFontsProgram()
{
    CString strSelGroup;
    CString strFilePath;
    CString strReport;
    CHAR   *pcFilePath;
    CHAR   *pcFileBuf;
    UINT    uiFlashAddr;
    UINT    uiFlashAddrBase;
    UINT    uiLoopFrame;
    UINT    uiLoopLine;
    UINT    uiLoopPixel;
    UCHAR   aucCtrlBuf[5];
    UCHAR   aucDataBuf[270];
    CHAR   *pcSrcBuf;
    CWnd   *cobCtrl;
    UCHAR   ucRGBByte;

    // 检查是否选择了正确的组
    m_obCBXFontsBMPGroup.GetWindowTextW(strSelGroup);

    if (strSelGroup != _T("第1组") && strSelGroup != _T("第2组") &&
        strSelGroup != _T("第3组") && strSelGroup != _T("第4组") &&
        strSelGroup != _T("第5组") && strSelGroup != _T("第6组") &&
        strSelGroup != _T("第7组") && strSelGroup != _T("第8组") &&
        strSelGroup != _T("第9组") && strSelGroup != _T("第10组"))
    {
        AfxMessageBox(_T("请选择一个组。"));
        return;
    }

    // 检查是否设置了文件路径
    GetDlgItemText(IDC_MFCEDITBROWSE_FONTS_FILE_PATH, strFilePath);

    if (0 == strFilePath.GetLength())
    {
        AfxMessageBox(_T("请设置正确BMP的路径。"));
        return;
    }
    else
    {
        // 打开文件
        USES_CONVERSION;
        pcFilePath = T2A(strFilePath);
        if (NULL != m_pstBMPFile)
        {
            fclose(m_pstBMPFile);
        }

        fopen_s(&m_pstBMPFile, pcFilePath, "rb");

        if (NULL == m_pstBMPFile)
        {
            strReport.Format(_T("无法打开文件：%s。"), pcFilePath);
            AfxMessageBox(strReport);
            return;
        }
    }

    // 检查文件大小是否正确。15幅135*240分辨率的BMP24文件，应该是1468854个字节。
    // 其中文件头是54字节。每行135个像素会被补充到4字节对齐，因此每行额外加上3个字节。合计：
    // 54 + 15 * (135 * 3 * 240 + 1 *3 *240) = 1468854
    fseek(m_pstBMPFile, 0L, SEEK_END);

    if (1468854 != ftell(m_pstBMPFile))
    {
        strReport.Format(_T("BMP文件大小错误。当前文件大小：%s。实际应该是1468854。"), ftell(m_pstBMPFile));
        fclose(m_pstBMPFile);
        return;
    }

    // 申请缓存
    pcFileBuf = (CHAR *)malloc(2 * 1024 * 1024);

    if (NULL == pcFileBuf)
    {
        strReport.Format(_T("申请文件缓存失败。"));
        fclose(m_pstBMPFile);
        return;
    }

    // 将文件整个读入到缓存
    fseek(m_pstBMPFile, 0L, SEEK_SET);

    fread(pcFileBuf,
          1468854,
          1,
          m_pstBMPFile);

    // 关闭文件
    fclose(m_pstBMPFile);

    // 发送数据

    if (_T("第1组") == strSelGroup)
    {
        uiFlashAddrBase = 0x00100000;
    }
    else  if (_T("第2组") == strSelGroup)
    {
        uiFlashAddrBase = 0x00200000;
    }
    else  if (_T("第3组") == strSelGroup)
    {
        uiFlashAddrBase = 0x00300000;
    }
    else  if (_T("第4组") == strSelGroup)
    {
        uiFlashAddrBase = 0x00400000;
    }
    else  if (_T("第5组") == strSelGroup)
    {
        uiFlashAddrBase = 0x00500000;
    }
    else  if (_T("第6组") == strSelGroup)
    {
        uiFlashAddrBase = 0x00600000;
    }
    else  if (_T("第7组") == strSelGroup)
    {
        uiFlashAddrBase = 0x00700000;
    }
    else  if (_T("第8组") == strSelGroup)
    {
        uiFlashAddrBase = 0x00800000;
    }
    else  if (_T("第9组") == strSelGroup)
    {
        uiFlashAddrBase = 0x00900000;
    }
    else  if (_T("第10组") == strSelGroup)
    {
        uiFlashAddrBase = 0x00A00000;
    }

    // 禁止控件
    cobCtrl = GetDlgItem(IDC_MFCEDITBROWSE_FONTS_FILE_PATH);
    cobCtrl->EnableWindow(FALSE);
    cobCtrl = GetDlgItem(IDC_COMBO_FONTS_PROGRAM_DESTINATION);
    cobCtrl->EnableWindow(FALSE);
    cobCtrl = GetDlgItem(IDC_BUTTON_FONTS_PROGRAM);
    cobCtrl->EnableWindow(FALSE);


    for (uiLoopFrame = 0; uiLoopFrame <= 14; uiLoopFrame++)
    {
        // BMP文件中一共有15幅画面,每幅画面占用64KB空间
        uiFlashAddr = uiFlashAddrBase + uiLoopFrame * 65536;

        for (uiLoopLine = 0; uiLoopLine <= 239; uiLoopLine++)
        {
            // 进度条显示
            m_obPGSFlashWrite.SetRange(0, 100);
            m_obPGSFlashWrite.SetPos((UCHAR)(100.0f * ((FLOAT)((uiLoopLine + 1) + uiLoopFrame * 240) / (FLOAT)(15 * 240))));

            // 每幅画面一共有240行，每行一共有（405+3）个字节，转换为（270个字节），第一次发送136个字节，
            // 第二次发送134个字节，这样每行需要发送2次

            // 将BGR24转换为RGB16
            // 计算图像行地址
            pcSrcBuf = pcFileBuf + 54; // 偏移过BMP文件头
            pcSrcBuf = pcSrcBuf + (14 - uiLoopFrame) * 97920; // 偏移过图像
            pcSrcBuf = pcSrcBuf + (239 - uiLoopLine) * 408; // 偏移过行

            for (uiLoopPixel = 0; uiLoopPixel <= 134; uiLoopPixel++)
            {
                // 清空Buf
                aucDataBuf[2 * uiLoopPixel + 0] = 0x00;
                aucDataBuf[2 * uiLoopPixel + 1] = 0x00;

                // 转换B
                ucRGBByte = (UCHAR)(*(pcSrcBuf + (uiLoopPixel * 3) + 0));
                ucRGBByte = (ucRGBByte >> 3);
                aucDataBuf[2 * uiLoopPixel + 1] = 0x1F & ucRGBByte;

                // 转换R
                ucRGBByte = (UCHAR)(*(pcSrcBuf + (uiLoopPixel * 3) + 2));
                aucDataBuf[2 * uiLoopPixel + 0] = 0xF8 & ucRGBByte;

                // 转换G
                aucDataBuf[2 * uiLoopPixel + 0] |= (0x07 & ((*(pcSrcBuf + (uiLoopPixel * 3) + 1)) >> 5));
                aucDataBuf[2 * uiLoopPixel + 1] |= (0xE0 & ((*(pcSrcBuf + (uiLoopPixel * 3) + 1)) << 3));
            }

            // 等待消息空间
            while (1 == g_stDlg2CommMsg.ucValid)
            {
                XSleep(10);
            }

            // 计算Flash地址
            aucCtrlBuf[0] = (uiFlashAddr >> 24);
            aucCtrlBuf[1] = (uiFlashAddr >> 16);
            aucCtrlBuf[2] = (uiFlashAddr >> 8);
            aucCtrlBuf[3] = (uiFlashAddr >> 0);
            aucCtrlBuf[4] = 135;
                
            g_stDlg2CommMsg.ucMsgDst = FFY_COMM_MSG_DST_F429;
            g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_WR_FLASH;
            g_stDlg2CommMsg.ucMsgLen = sizeof(aucCtrlBuf) + 135;

            memcpy(g_stDlg2CommMsg.aucMsg + 0,
                aucCtrlBuf,
                sizeof(aucCtrlBuf));

            memcpy(g_stDlg2CommMsg.aucMsg + sizeof(aucCtrlBuf),
                aucDataBuf, 
                135);

            g_stDlg2CommMsg.ucValid = 1;

            // 等待发送完成
            XSleep(70);
            while (1 == g_stDlg2CommMsg.ucValid)
            {
                XSleep(10);
            }

            // 计算Flash地址
            uiFlashAddr += 135;

            aucCtrlBuf[0] = (uiFlashAddr >> 24);
            aucCtrlBuf[1] = (uiFlashAddr >> 16);
            aucCtrlBuf[2] = (uiFlashAddr >> 8);
            aucCtrlBuf[3] = (uiFlashAddr >> 0);
            aucCtrlBuf[4] = 135;

            g_stDlg2CommMsg.ucMsgDst = FFY_COMM_MSG_DST_F429;
            g_stDlg2CommMsg.ucMsgType = FFY_HST_L2_TYPE_CMD_WR_FLASH;
            g_stDlg2CommMsg.ucMsgLen = sizeof(aucCtrlBuf) + 135;

            memcpy(g_stDlg2CommMsg.aucMsg + 0,
                aucCtrlBuf,
                sizeof(aucCtrlBuf));

            memcpy(g_stDlg2CommMsg.aucMsg + sizeof(aucCtrlBuf),
                aucDataBuf + 135,
                135);

            g_stDlg2CommMsg.ucValid = 1;

            // 等待发送完成
            XSleep(70);
            while (1 == g_stDlg2CommMsg.ucValid)
            {
                XSleep(10);
            }

            // 计算Flash地址
            uiFlashAddr += 135; 
        }
    }

    // 恢复控件
    cobCtrl = GetDlgItem(IDC_MFCEDITBROWSE_FONTS_FILE_PATH);
    cobCtrl->EnableWindow(TRUE);
    cobCtrl = GetDlgItem(IDC_COMBO_FONTS_PROGRAM_DESTINATION);
    cobCtrl->EnableWindow(TRUE);
    cobCtrl = GetDlgItem(IDC_BUTTON_FONTS_PROGRAM);
    cobCtrl->EnableWindow(TRUE);
}
