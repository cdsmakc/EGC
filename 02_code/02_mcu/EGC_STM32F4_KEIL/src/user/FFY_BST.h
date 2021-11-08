/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : iv18_bst.h
    Project name : IV18
    Module name  : 
    Date created : 10 January 2020   14时30分6秒
    Author       : Ning.JianLi
    Description  : Boost模块的头文件。
*******************************************************************************/

#ifndef __IV18_BST_H__
#define __IV18_BST_H__

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "stm32f10x.h"
#include "os.h"
#include "pub.h"

#pragma anon_unions

CODE_SECTION("==========================") ;
CODE_SECTION("==  简单宏定义          ==") ;
CODE_SECTION("==========================") ;

#define IV18_BST_TASK_DELAY                 (5u)            /* 任务循环时间 */

CODE_SECTION("==========================") ;
CODE_SECTION("==  复杂数据类型定义    ==") ;
CODE_SECTION("==========================") ;
/* 以下结构体类型定义了BST模块的工作区 */
typedef struct tagIV18_BST_WORKAREA
{
    union
    {
        UCHAR aucRawData[8] ;
        
        struct
        {
            /* DW0 */ /* FLAG ONLY */
            UINT  uiRsv0 ;

            /* DW1 */
            UCHAR ucBrightness ;
            UCHAR ucRsv1 ;
            UCHAR ucRsv2 ;
            UCHAR ucRsv3 ;
        } ;
    } ;
}IV18_BST_WORKAREA_S ;

CODE_SECTION("==========================") ;
CODE_SECTION("==  函数声明            ==") ;
CODE_SECTION("==========================") ;

VOID IV18_BST_Task(VOID) ;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __IV18_BST_H__ */

/******* End of file iv18_bst.h. *******/  
