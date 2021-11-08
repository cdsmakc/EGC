/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : EGC_Config.h
    Project name : EGC
    Module name  : 
    Date created : 2020年2月12日    14时30分6秒
    Author       : Ning.JianLi
    Description  : EGC工程的配置信息定义。
*******************************************************************************/

#ifndef __EGC_CONFIG_H__
#define __EGC_CONFIG_H__

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#pragma anon_unions
#pragma pack(1)

CODE_SECTION("==========================") ;
CODE_SECTION("==  简单宏定义          ==") ;
CODE_SECTION("==========================") ;
/* 用户配置信息格式化标记定义 */
#define EGC_CONFIG_USER_CONFIG_FORMAT_FLAG         (0x5C) /* 用户配置信息格式化标记 */

/* W25Q空间地址划分 */
#define EGC_CONFIG_W25Q_ADDR_BASE                  (0x00000000)
#define EGC_CONFIG_W25Q_ADDR_CONFIG                (0x00000000)
#define EGC_CONFIG_W25Q_SIZE_CONFIG                (0x00000400)
#define EGC_CONFIG_W25Q_ADDR_CHAR_GROUP_BASE       (0x00100000)
#define EGC_CONFIG_W25Q_SIZE_CHAR_GROUP_BASE       (0x00100000)
#define EGC_CONFIG_W25Q_ADDR_CHAR_GROUP1           (0x00100000)
#define EGC_CONFIG_W25Q_SIZE_CHAR_GROUP1           (0x00100000)
#define EGC_CONFIG_W25Q_ADDR_CHAR_GROUP2           (0x00200000)
#define EGC_CONFIG_W25Q_SIZE_CHAR_GROUP2           (0x00100000)
#define EGC_CONFIG_W25Q_ADDR_CHAR_GROUP3           (0x00300000)
#define EGC_CONFIG_W25Q_SIZE_CHAR_GROUP3           (0x00100000)
#define EGC_CONFIG_W25Q_ADDR_CHAR_GROUP4           (0x00400000)
#define EGC_CONFIG_W25Q_SIZE_CHAR_GROUP4           (0x00100000)
#define EGC_CONFIG_W25Q_ADDR_CHAR_GROUP5           (0x00500000)
#define EGC_CONFIG_W25Q_SIZE_CHAR_GROUP5           (0x00100000)
#define EGC_CONFIG_W25Q_ADDR_CHAR_GROUP6           (0x00600000)
#define EGC_CONFIG_W25Q_SIZE_CHAR_GROUP6           (0x00100000)
#define EGC_CONFIG_W25Q_ADDR_CHAR_GROUP7           (0x00700000)
#define EGC_CONFIG_W25Q_SIZE_CHAR_GROUP7           (0x00100000)
#define EGC_CONFIG_W25Q_ADDR_CHAR_GROUP8           (0x00800000)
#define EGC_CONFIG_W25Q_SIZE_CHAR_GROUP8           (0x00100000)
#define EGC_CONFIG_W25Q_ADDR_CHAR_GROUP9           (0x00900000)
#define EGC_CONFIG_W25Q_SIZE_CHAR_GROUP9           (0x00100000)
#define EGC_CONFIG_W25Q_ADDR_CHAR_GROUP10          (0x00A00000)
#define EGC_CONFIG_W25Q_SIZE_CHAR_GROUP10          (0x00100000)

/* SDRAM空间地址分配 */
#define EGC_CONFIG_SDRAM_ADDR_BASE                 (0xC0000000)
#define EGC_CONFIG_SDRAM_ADDR_LCD1_BUF1            (0xC0000000)
#define EGC_CONFIG_SDRAM_ADDR_LCD1_BUF2            (0xC0010000)
#define EGC_CONFIG_SDRAM_ADDR_LCD2_BUF1            (0xC0020000)
#define EGC_CONFIG_SDRAM_ADDR_LCD2_BUF2            (0xC0030000)
#define EGC_CONFIG_SDRAM_ADDR_LCD3_BUF1            (0xC0040000)
#define EGC_CONFIG_SDRAM_ADDR_LCD3_BUF2            (0xC0050000)
#define EGC_CONFIG_SDRAM_ADDR_LCD4_BUF1            (0xC0060000)
#define EGC_CONFIG_SDRAM_ADDR_LCD4_BUF2            (0xC0070000)
#define EGC_CONFIG_SDRAM_ADDR_LCD5_BUF1            (0xC0080000)
#define EGC_CONFIG_SDRAM_ADDR_LCD5_BUF2            (0xC0090000)
#define EGC_CONFIG_SDRAM_ADDR_LCD6_BUF1            (0xC00A0000)
#define EGC_CONFIG_SDRAM_ADDR_LCD6_BUF2            (0xC00B0000)
#define EGC_CONFIG_SDRAM_ADDR_LCD7_BUF1            (0xC00C0000)
#define EGC_CONFIG_SDRAM_ADDR_LCD7_BUF2            (0xC00D0000)
#define EGC_CONFIG_SDRAM_ADDR_LCD8_BUF1            (0xC00E0000)
#define EGC_CONFIG_SDRAM_ADDR_LCD8_BUF2            (0xC00F0000)
#define EGC_CONFIG_SDRAM_ADDR_W25Q_SECTOR_BUF      (0xC0100000)
#define EGC_CONFIG_SDRAM_ADDR_HOST_UART_RECV_BUF   (0xC0180000)
#define EGC_CONFIG_SDRAM_ADDR_CHAR_GROUP_BUF       (0xC0200000)

CODE_SECTION("==========================") ;
CODE_SECTION("==  枚举类型定义        ==") ;
CODE_SECTION("==========================") ;
/* 以下枚举类型定义了LCD的亮度信息 */
typedef enum tagEGC_CONFIG_LCD_BRSS_DEF
{
    BRIGHTNESS_LEVEL_OFF = 0 ,
    BRIGHTNESS_LEVEL_1 ,
    BRIGHTNESS_LEVEL_2 ,
    BRIGHTNESS_LEVEL_3 ,
    BRIGHTNESS_LEVEL_4 ,
    BRIGHTNESS_LEVEL_5 ,
    BRIGHTNESS_LEVEL_6 ,
    BRIGHTNESS_LEVEL_7 ,
    BRIGHTNESS_LEVEL_8 ,
    BRIGHTNESS_LEVEL_9 ,
    BRIGHTNESS_LEVEL_MAX
}EGC_CONFIG_LCD_BRSS_DEF_E ;
    
/* 以下枚举类型定义了触碰的灵敏度 */
typedef enum tagEGC_CONFIG_TOUCH_SENSE_LVL
{
    TOUCH_SENSE_LEVEL_VERY_HIGH = 0 ,
    TOUCH_SENSE_LEVEL_HIGH ,
    TOUCH_SENSE_LEVEL_MEDIUM ,
    TOUCH_SENSE_LEVEL_LOW ,
    TOUCH_SENSE_LEVEL_VERY_LOW
}EGC_CONFIG_TOUCH_SENSE_LVL_E ;

/* 以下枚举类型定义了WIFI获取网络时间的更新周期 */
typedef enum tagEGC_CONFIG_WIFI_UPDATE_TIME
{
    UPDATE_TIME_1_HOUR   = 0 ,        /* 每1小时从网络获取一次时间 */
    UPDATE_TIME_4_HOUR ,              /* 每4小时从网络获取一次时间 */
    UPDATE_TIME_12_HOUR ,             /* 每12小时从网络获取一次时间 */
    UPDATE_TIME_1_DAY ,               /* 每1天从网络获取一次时间 */
    UPDATE_TIME_1_WEEK ,              /* 每1周从网络获取一次时间 */
    UPDATE_TIME_2_WEEK ,              /* 每2周从网络获取一次时间 */
    UPDATE_TIME_1_MONTH               /* 每1月从网络获取一次时间 */
}EGC_CONFIG_WIFI_UPDATE_TIME_E ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  结构类型定义        ==") ;
CODE_SECTION("==========================") ;

/* 以下结构体定义了字符组的数据构成 */
typedef struct tagEGC_CONFIG_CHAR_GROUP
{
    union
    {
        UCHAR aucRawData[1048576] ;

        struct
        {
            UCHAR aucCharSpace[65536] ;              /* 有效字节64800 */
            UCHAR aucCharPercentSign[65536] ;        /* 有效字节64800 */
            UCHAR aucCharCelsiusSign[65536] ;        /* 有效字节64800 */
            UCHAR aucCharColonSign[65536] ;          /* 有效字节64800 */
            UCHAR aucCharStrokeSign[65536] ;         /* 有效字节64800 */
            UCHAR aucChar0[65536] ;                  /* 有效字节64800 */
            UCHAR aucChar1[65536] ;                  /* 有效字节64800 */
            UCHAR aucChar2[65536] ;                  /* 有效字节64800 */
            UCHAR aucChar3[65536] ;                  /* 有效字节64800 */
            UCHAR aucChar4[65536] ;                  /* 有效字节64800 */
            UCHAR aucChar5[65536] ;                  /* 有效字节64800 */
            UCHAR aucChar6[65536] ;                  /* 有效字节64800 */
            UCHAR aucChar7[65536] ;                  /* 有效字节64800 */
            UCHAR aucChar8[65536] ;                  /* 有效字节64800 */
            UCHAR aucChar9[65536] ;                  /* 有效字节64800 */
        } ;
    } ;
}EGC_CONFIG_CHAR_GROUP_S ;

/* 以下结构体定义了定时显示的模式和时间信息 */
typedef struct tagEGC_CONFIG_TIME_SHOW_INFO
{
    union
    {
        UCHAR aucRawData[8] ;

        struct
        {
            /* byte 0 */
            UCHAR ucValid                     : 1 ; /* 控制本条周期显示信息是否有效。0：无效；1：按照本条信息的时间显示。 */
            UCHAR ucMonday                    : 1 ; /* 控制周一是否显示。0：不在周一显示；1：周一按照设定的“时/分”时间段显示 */
            UCHAR ucTuesday                   : 1 ; /* 控制周二是否显示。0：不在周二显示；1：周二按照设定的“时/分”时间段显示 */
            UCHAR ucWednesday                 : 1 ; /* 控制周三是否显示。0：不在周三显示；1：周三按照设定的“时/分”时间段显示 */
            UCHAR ucThursday                  : 1 ; /* 控制周四是否显示。0：不在周四显示；1：周四按照设定的“时/分”时间段显示 */
            UCHAR ucFriday                    : 1 ; /* 控制周五是否显示。0：不在周五显示；1：周五按照设定的“时/分”时间段显示 */
            UCHAR ucSaturday                  : 1 ; /* 控制周六是否显示。0：不在周六显示；1：周六按照设定的“时/分”时间段显示 */
            UCHAR ucSunday                    : 1 ; /* 控制周日是否显示。0：不在周日显示；1：周日按照设定的“时/分”时间段显示 */

            /* byte 1, 2, 3, 4 */
            UCHAR ucStartHour ;                     /* 显示时间段的起始小时 */
            UCHAR ucStartMinute ;                   /* 显示时间段的起始分钟 */
            UCHAR ucEndHour ;                       /* 显示时间段的结束小时 */
            UCHAR ucEndMinute ;                     /* 显示时间段的结束分钟 */

            /* byte 5, 6, 7 */
            UCHAR ucRsv0 ;
            UCHAR ucRsv1 ;
            UCHAR ucRsv2 ;
            
        } ;
    } ;
} EGC_CONFIG_TIME_SHOW_INFO_S ;

/* 以下结构体定义了遍历显示的模式和遍历时间信息 */
typedef struct tagEGC_CONFIG_ERGO_SHOW_INFO
{
    union
    {
        UCHAR aucRawData[4] ;

        struct
        {
            /* byte 0 */
            UCHAR ucShowDate                  : 1 ; /* 控制是否周期性显示日期。0：不显示日期；1：周期显示日期。 */
            UCHAR ucShowTemp                  : 1 ; /* 控制是否周期性显示温度。0：不显示温度；1：周期显示温度。 */
            UCHAR ucShowHumd                  : 1 ; /* 控制是否周期性显示湿度。0：不显示湿度；1：周期显示湿度。 */
            UCHAR ucRsv0                      : 5 ;

            /* byte 1 */
            UCHAR ucShowDateSecondsPerMinute  : 4 ; /* 如果周期显示日期，设置每分钟显示日期的秒数。 */
            UCHAR ucShowTempSecondsPerMinute  : 4 ; /* 如果周期显示温度，设置每分钟显示温度的秒数。 */

            /* byte 2 */
            UCHAR ucShowHumdSecondsPerMinute  : 4 ; /* 如果周期显示温度，设置每分钟显示温度的秒数。 */
            UCHAR ucRsv1                      : 4 ;

            /* byte 3 */
            UCHAR ucRsv2 ;
        } ;
    } ;
} EGC_CONFIG_ERGO_SHOW_INFO_S ;

/* 以下结构体定义了触碰感知配置信息 */
typedef struct tagEGC_CONFIG_TOUCH_CFG_INFO
{
    union
    {
        UCHAR aucRawData[4] ;

        struct
        {
            /* DW0 */
            UCHAR                        ucTouchToShow      : 1 ; /* 配置在非显示时，是否允许触摸以显示。0：禁止触摸以显示；1：允许触摸以显示 */
            UCHAR                        ucTouchToSwitch    : 1 ; /* 配置在显示时，是否允许触摸以切换显示。0：禁止触摸以切换显示；1：允许触摸以切换显示 */
            UCHAR                        ucTouchToStopAlarm : 1 ; /* 配置当闹铃闹响时，是否允许触摸以停止闹铃。0：禁止触摸以停止闹铃；1：允许触摸以停止闹铃 */
            UCHAR                        ucRsv0             : 5 ;

            UCHAR                        ucRsv1;
            UCHAR                        ucRsv2;
            UCHAR                        ucRsv3;

            /* DW1 */
            EGC_CONFIG_TOUCH_SENSE_LVL_E enTouchSenseLevel  : 8 ;
            UINT                         uiRsv4             : 24 ;
        } ;
    } ;
}EGC_CONFIG_TOUCH_CFG_INFO_S ;

/* 以下结构体定义了WIFI连接的AP的相关信息 */
typedef struct tagEGC_CONFIG_AP_INFO
{
    union
    {
        UCHAR aucRawData[80] ;

        struct
        {
            /* BYTE 0 */
            UCHAR ucValid                     : 1 ;
            UCHAR ucRsv0                      : 7 ;
            
            /* BYTE 01 ~ 40 */ /* SSID */
            CHAR  acSSID[40] ;

            /* BYTE 41 ~ 79 */ /* passsword */
            CHAR  acPassword[39] ;
        } ;
    } ;
} EGC_CONFIG_AP_INFO_S ;

/* 以下结构体类型定义了NTP服务器的相关信息 */
typedef struct tagEGC_CONFIG_NTP_SERVER_INFO
{
    union
    {
        UCHAR aucRawData[128] ;

        struct
        {
            /* BYTE 0 */
            UCHAR           ucValid        : 1 ;
            UCHAR           ucRsv0         : 7 ;

            /* BYTE 1 ~ 127 */
            CHAR            acNTPServer[127] ;
        } ;
    } ;
} EGC_CONFIG_NTP_SERVER_INFO_S ;

/* 一下结构体类型定义了WIFI获取网络时间的相关配置信息 */
typedef struct tagEGC_CONFIG_WIFI_CFG_INFO
{
    union
    {
        UCHAR aucRawData[548] ;

        struct
        {
            /* DW  0 ~ DW 19 */ /* AP1配置信息 */
            EGC_CONFIG_AP_INFO_S          stAP1Info ;
            /* DW 20 ~ DW 39 */ /* AP2配置信息 */
            EGC_CONFIG_AP_INFO_S          stAP2Info ;
            /* DW 40 ~ DW 71 */ /* NTP服务器1配置信息 */
            EGC_CONFIG_NTP_SERVER_INFO_S  stNTPServer1 ;
            /* DW 72 ~ DW103 */ /* NTP服务器2配置信息 */
            EGC_CONFIG_NTP_SERVER_INFO_S  stNTPServer2 ;
            /* DW104 ~ DW135 */ /* NTP服务器2配置信息 */
            EGC_CONFIG_NTP_SERVER_INFO_S  stNTPServer3 ;
            /* DW136 */
            EGC_CONFIG_WIFI_UPDATE_TIME_E enUpdateIntervel ;
        } ;
    } ;
}EGC_CONFIG_WIFI_CFG_INFO_S ;


/* 以下结构体定义了EGC的工作配置信息 */
typedef struct tagEGC_CONFIG_INFO
{
    union
    {
        UCHAR aucRawData[1024];

        struct
        {
            /* DW0 */ /* 仅用作标记 */
            UINT                         uiEnableTimeShow          : 1 ;
            UINT                         uiEnableEgroShow          : 1 ;
            UINT                         uiEnableTouchSense        : 1 ;
            UINT                         uiEnableWIFIUpdateTime    : 1 ;
            UINT                         uiByte0Rsv0               : 4 ;

            UINT                         uiByte1Rsv0               : 8 ;

            UINT                         uiByte2Rsv0               : 8 ;

            UINT                         uiFormatFlag              : 8 ;

            /* DW1 ~ DW7 */
            EGC_CONFIG_LCD_BRSS_DEF_E    enLCDBrightness           : 8 ; /* LCD亮度等级设置 */
            UINT                         uiCharGroup               : 8 ; /* 设定显示的字符组 */
            UINT                         uiRsv0                    : 16;
            UCHAR                        aucRsv1[24];

            /* DW8 ~ DW15 */ /* 定时显示信息 */
            EGC_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo1 ;
            EGC_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo2 ;
            EGC_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo3 ;
            EGC_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo4 ;
            EGC_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo5 ;
            EGC_CONFIG_TIME_SHOW_INFO_S  stTimeShowInfo6 ;

            /* DW16 */ /* 遍历显示信息 */
            EGC_CONFIG_ERGO_SHOW_INFO_S  stErgoShowInfo ;

            /* DW17 */ /* 触碰显示配置信息 */
            EGC_CONFIG_TOUCH_CFG_INFO_S  stTouchConfigInfo ;

            /* DW18 ~ DW154 */ /* WIFI获取时间的相关配置信息 */
            EGC_CONFIG_WIFI_CFG_INFO_S   stWIFIConfigInfo ;
        } ;
    } ;
} EGC_CONFIG_INFO_S ;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __EGC_CONFIG_H__ */

/******* End of file EGC_Config.h. *******/  

