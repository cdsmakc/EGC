/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : EGC_MSR.h
    Project name : EGC
    Module name  : 
    Date created : 2020年02月12日    9时46分54秒
    Author       : Ning.JianLi
    Description  : Measure模块头文件。
*******************************************************************************/

#ifndef __EGC_MSR_H__
#define __EGC_MSR_H__

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "pub_type.h"
#include "pub_lib.h"

#include "EGC_Config.h"

CODE_SECTION("==========================") ;
CODE_SECTION("==  简单宏定义          ==") ;
CODE_SECTION("==========================") ;
#define EGC_MSR_TASK_DELAY_S              (1u)             


CODE_SECTION("==========================") ;
CODE_SECTION("==  复杂数据类型定义    ==") ;
CODE_SECTION("==========================") ;
/* 以下结构体类型定义了MSR模块的工作区 */
#pragma pack(4)   
typedef struct tagEGC_MSR_WORKAREA
{
    union
    {
        UCHAR aucRawData[32] ;
        
        struct
        {
            /* DW0 */ /* FLAG ONLY */
            UINT                         uiTaskRun             : 1 ; /* 任务启动标记。外部模块读写，内部模块只读 */
            UINT                         uiInitDone            : 1 ;  /* 初始化完成标记，外部模块只读，内部模块读写 */
            UINT                         uiTouchOccur          : 1 ;
            UINT                         uiByte0RSV0           : 5 ;
            
            UINT                         uiByte1RSV0           : 8 ;
            
            UINT                         uiByte2RSV0           : 8 ;
            
            UINT                         uiByte3RSV0           : 8 ;

            /* DW1 */ /* Error code only */
            UINT                         uiErrCode ;

            /* DW2 */
            FLOAT                        fTemp ;

            /* DW3 */
            FLOAT                        fHumi ;

            /* DW4 */
            EGC_CONFIG_TOUCH_SENSE_LVL_E enTouchSenseLevel ;

            /* DW5 */
            FLOAT                        fXAxisAcc ;

            /* DW6 */
            FLOAT                        fYAxisAcc ;

            /* DW7 */
            FLOAT                        fZAxisAcc ;
        } ;
    } ;
}EGC_MSR_WORKAREA_S ;
#pragma pack()  
CODE_SECTION("==========================") ;
CODE_SECTION("==  函数声明            ==") ;
CODE_SECTION("==========================") ;

VOID EGC_MSR_Task(VOID) ;
VOID EGC_MSR_MSA300EXTIIntHandler(VOID) ;
VOID EGC_MSR_I2CIntHandler(VOID) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __EGC_MSR_H__ */

/******* End of file EGC_MSR.h. *******/  
