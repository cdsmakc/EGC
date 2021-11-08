/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : EGC_LG1.h
    Project name : EGC
    Module name  : 
    Date created : 2021年09月06日    15时26分4秒
    Author       : Ning.JianLi
    Description  : LG1(LCD Group1)模块头文件。
*******************************************************************************/

#ifndef __EGC_LG1_H__
#define __EGC_LG1_H__

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "pub_type.h"
#include "pub_lib.h"

#include "EGC_Config.h"

CODE_SECTION("==========================") ;
CODE_SECTION("==  LG1模块任务参数定义 ==") ;
CODE_SECTION("==========================") ;
/* 任务循环周期 */
#define EGC_LG1_TASK_CYCLE_PERIOD_MS             (100u)

/* 显示RAM */
#define EGC_LG1_LCD_FRAME_SIZE                   (135 * 240 * 2) /* 64800 */

CODE_SECTION("==========================") ;
CODE_SECTION("==  ST7792寄存器定义    ==") ;
CODE_SECTION("==========================") ;
/* Table 1 */
#define EGC_LG1_ST7789V_CMD_TB1_NOP              (0x00)
#define EGC_LG1_ST7789V_CMD_TB1_SWRESET          (0x01)
#define EGC_LG1_ST7789V_CMD_TB1_RDDID            (0x04)
#define EGC_LG1_ST7789V_CMD_TB1_RDDST            (0x09)
#define EGC_LG1_ST7789V_CMD_TB1_RDDPM            (0x0A)
#define EGC_LG1_ST7789V_CMD_TB1_RDDMADCTL        (0x0B)
#define EGC_LG1_ST7789V_CMD_TB1_RDDCOLMOD        (0x0C)
#define EGC_LG1_ST7789V_CMD_TB1_RDDIM            (0x0D)
#define EGC_LG1_ST7789V_CMD_TB1_RDDSM            (0x0E)
#define EGC_LG1_ST7789V_CMD_TB1_RDDSDR           (0x0F)
#define EGC_LG1_ST7789V_CMD_TB1_SLPIN            (0x10)
#define EGC_LG1_ST7789V_CMD_TB1_SLPOUT           (0x11)
#define EGC_LG1_ST7789V_CMD_TB1_PTLON            (0x12)
#define EGC_LG1_ST7789V_CMD_TB1_NORON            (0x13)
#define EGC_LG1_ST7789V_CMD_TB1_INVOFF           (0x20)
#define EGC_LG1_ST7789V_CMD_TB1_INVON            (0x21)
#define EGC_LG1_ST7789V_CMD_TB1_GAMSET           (0x26)
#define EGC_LG1_ST7789V_CMD_TB1_DISPOFF          (0x28)
#define EGC_LG1_ST7789V_CMD_TB1_DISPON           (0x29)
#define EGC_LG1_ST7789V_CMD_TB1_CASET            (0x2A)
#define EGC_LG1_ST7789V_CMD_TB1_RASET            (0x2B)
#define EGC_LG1_ST7789V_CMD_TB1_RAMWR            (0x2C)
#define EGC_LG1_ST7789V_CMD_TB1_RAMRD            (0x2E)
#define EGC_LG1_ST7789V_CMD_TB1_PTLAR            (0x30)
#define EGC_LG1_ST7789V_CMD_TB1_VSCRDEF          (0x33)
#define EGC_LG1_ST7789V_CMD_TB1_TEOFF            (0x34)
#define EGC_LG1_ST7789V_CMD_TB1_TEON             (0x35)
#define EGC_LG1_ST7789V_CMD_TB1_MADCTL           (0x36)
#define EGC_LG1_ST7789V_CMD_TB1_VSCSAD           (0x37)
#define EGC_LG1_ST7789V_CMD_TB1_IDMOFF           (0x38)
#define EGC_LG1_ST7789V_CMD_TB1_IDMON            (0x39)
#define EGC_LG1_ST7789V_CMD_TB1_COLMOD           (0x3A)
#define EGC_LG1_ST7789V_CMD_TB1_WRMEMC           (0x3C)
#define EGC_LG1_ST7789V_CMD_TB1_RDMEMC           (0x3E)
#define EGC_LG1_ST7789V_CMD_TB1_STE              (0x44)
#define EGC_LG1_ST7789V_CMD_TB1_GSCAN            (0x45)
#define EGC_LG1_ST7789V_CMD_TB1_WRDISBV          (0x51)
#define EGC_LG1_ST7789V_CMD_TB1_RDDISBV          (0x52)
#define EGC_LG1_ST7789V_CMD_TB1_WRCTRLD          (0x53)
#define EGC_LG1_ST7789V_CMD_TB1_RDCTRLD          (0x54)
#define EGC_LG1_ST7789V_CMD_TB1_WRCACE           (0x55)
#define EGC_LG1_ST7789V_CMD_TB1_RDCABC           (0x56)
#define EGC_LG1_ST7789V_CMD_TB1_WRCABCMB         (0x5E)
#define EGC_LG1_ST7789V_CMD_TB1_RDCABCMB         (0x5F)
#define EGC_LG1_ST7789V_CMD_TB1_RDABCSDR         (0x68)
#define EGC_LG1_ST7789V_CMD_TB1_RDID1            (0xDA)
#define EGC_LG1_ST7789V_CMD_TB1_RDID2            (0xDB)
#define EGC_LG1_ST7789V_CMD_TB1_RDID3            (0xDC)

/* Table 2 */
#define EGC_LG1_ST7789V_CMD_TB2_RAMCTRL          (0xB0)
#define EGC_LG1_ST7789V_CMD_TB2_RGBCTRL          (0xB1)
#define EGC_LG1_ST7789V_CMD_TB2_PORCTRL          (0xB2)
#define EGC_LG1_ST7789V_CMD_TB2_FRCTRL1          (0xB3)
#define EGC_LG1_ST7789V_CMD_TB2_PARCTRL          (0xB5)
#define EGC_LG1_ST7789V_CMD_TB2_GCTRL            (0xB7)
#define EGC_LG1_ST7789V_CMD_TB2_GTADJ            (0xB8)
#define EGC_LG1_ST7789V_CMD_TB2_DGMEN            (0xBA)
#define EGC_LG1_ST7789V_CMD_TB2_VCOMS            (0xBB)
#define EGC_LG1_ST7789V_CMD_TB2_LCMCTRL          (0xC0)
#define EGC_LG1_ST7789V_CMD_TB2_IDSET            (0xC1)
#define EGC_LG1_ST7789V_CMD_TB2_VDVVRHEN         (0xC2)
#define EGC_LG1_ST7789V_CMD_TB2_VRHS             (0xC3)
#define EGC_LG1_ST7789V_CMD_TB2_VDVS             (0xC4)
#define EGC_LG1_ST7789V_CMD_TB2_VCMOFSET         (0xC5)
#define EGC_LG1_ST7789V_CMD_TB2_FRCTRL2          (0xC6)
#define EGC_LG1_ST7789V_CMD_TB2_CABCCTRL         (0xC7)
#define EGC_LG1_ST7789V_CMD_TB2_REGSEL1          (0xC8)
#define EGC_LG1_ST7789V_CMD_TB2_REGSEL2          (0xCA)
#define EGC_LG1_ST7789V_CMD_TB2_PWMFRSEL         (0xCC)
#define EGC_LG1_ST7789V_CMD_TB2_PWCTRL1          (0xD0)
#define EGC_LG1_ST7789V_CMD_TB2_VAPVANEN         (0xD2)
#define EGC_LG1_ST7789V_CMD_TB2_CMD2EN           (0xDF)
#define EGC_LG1_ST7789V_CMD_TB2_PVGAMCTRL        (0xE0)
#define EGC_LG1_ST7789V_CMD_TB2_NVGAMCTRL        (0xE1)
#define EGC_LG1_ST7789V_CMD_TB2_DGMLUTR          (0xE2)
#define EGC_LG1_ST7789V_CMD_TB2_DGMLUTB          (0xE3)
#define EGC_LG1_ST7789V_CMD_TB2_GATECTRL         (0xE4)
#define EGC_LG1_ST7789V_CMD_TB2_SPI2EN           (0xE7)
#define EGC_LG1_ST7789V_CMD_TB2_PWCTRL2          (0xE8)
#define EGC_LG1_ST7789V_CMD_TB2_EQCTRL           (0xE9)
#define EGC_LG1_ST7789V_CMD_TB2_PROMCTRL         (0xEC)
#define EGC_LG1_ST7789V_CMD_TB2_PROMEN           (0xFA)
#define EGC_LG1_ST7789V_CMD_TB2_NVMSET           (0xFC)
#define EGC_LG1_ST7789V_CMD_TB2_PROMACT          (0xFE)


CODE_SECTION("==========================") ;
CODE_SECTION("==  LG1模块枚举类型定义 ==") ;
CODE_SECTION("==========================") ;
/* LCD编号定义 */
typedef enum tagEGC_LG1_LCD_DEF
{
    LG1_LCD1 = 1 ,
    LG1_LCD2 ,
    LG1_LCD3 ,
    LG1_LCD4
} EGC_LG1_LCD_DEF_E ;

/* CS信号状态定义 */
typedef enum tagEGC_LG1_SPI_CS_CMD
{
    LG1_SPI_CS_CMD_START = 0 ,
    LG1_SPI_CS_CMD_STOP
} EGC_LG1_SPI_CS_CMD_E ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  LG1模块工作区定义   ==") ;
CODE_SECTION("==========================") ;
/* LG1模块工作区 */
typedef struct tagEGC_LG1_WORKAREA 
{
    union
    {
        UCHAR aucRawData[12] ;
        
        struct
        {
            /* DW0 */ /* 仅用作标记 */
            UINT                       uiTaskRun             : 1 ;  /* 模块任务启动标记，外部模块读写，内部模块只读 */
            UINT                       uiInitDone            : 1 ;  /* 初始化完成标记，外部模块只读，内部模块读写 */
            UINT                       uiHide                : 1 ;  /* 关闭所有显示。外部模块置位/复位，内部模块只读 */
            UINT                       uiByte0Rsv0           : 5 ;  /* 字节0的保留部分 */

            UINT                       uiLCD1UpdateView      : 1 ;  /* 更新LCD1的显示，外部模块置位，内部模块复位 */
            UINT                       uiLCD2UpdateView      : 1 ;  /* 更新LCD2的显示，外部模块置位，内部模块复位 */
            UINT                       uiLCD3UpdateView      : 1 ;  /* 更新LCD3的显示，外部模块置位，内部模块复位 */
            UINT                       uiLCD4UpdateView      : 1 ;  /* 更新LCD4的显示，外部模块置位，内部模块复位 */
            UINT                       uiLCD1UseSecondBuf    : 1 ;  /* LCD1使用第二缓存进行显示，外部模块读写，内部模块只读 */
            UINT                       uiLCD2UseSecondBuf    : 1 ;  /* LCD2使用第二缓存进行显示，外部模块读写，内部模块只读 */
            UINT                       uiLCD3UseSecondBuf    : 1 ;  /* LCD3使用第二缓存进行显示，外部模块读写，内部模块只读 */
            UINT                       uiLCD4UseSecondBuf    : 1 ;  /* LCD4使用第二缓存进行显示，外部模块读写，内部模块只读 */
            
            UINT                       uiByte2Rsv0           : 8 ;  /* 字节2的保留部分 */
            UINT                       uiByte3RSV0           : 8 ;  /* 字节3的保留部分 */

            /* DW1 */  /* 仅用作错误码 */
            UINT                       uiErrCode ;

            /* DW2 */
            EGC_CONFIG_LCD_BRSS_DEF_E  enBrightnessLevel ;          /* 亮度调节 */
        } ; 
    } ;
} EGC_LG1_WORKAREA_S ;


CODE_SECTION("==========================") ;
CODE_SECTION("==  LG1模块函数声明     ==") ;
CODE_SECTION("==========================") ;
VOID EGC_LG1_Task(VOID) ;
VOID EGC_LG1_SPIIntHandler(VOID) ;
VOID EGC_LG1_SPIDMAIntHandler(VOID) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __EGC_LG1_H__ */

/******* End of file EGC_LG1.h. *******/  

