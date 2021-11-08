/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : EGC.h
    Project name : EGC
    Module name  : 
    Date created : 2020年02月11日   12时29分58秒
    Author       : Ning.JianLi
    Description  : FireFly荧光钟主控程序头文件。
*******************************************************************************/

#ifndef __EGC_H__
#define __EGC_H__

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include  <stdarg.h>
#include  <stdio.h>
#include  <stdlib.h>
#include  <math.h>

#include  <cpu.h>
#include  <lib_def.h>
#include  <lib_ascii.h>
#include  <lib_math.h>
#include  <lib_mem.h>
#include  <lib_str.h>

#include  <os.h>

#include "stm32f4xx.h"
#include "pub_type.h"
#include "sys_cfg.h"

#include "EGC_Config.h"
#include "EGC_ErrCode.h"

#include "EGC_ESP.h"
#include "EGC_CTL.h"
#include "EGC_MSR.h"
#include "EGC_HST.h"
#include "EGC_LG1.h"
#include "EGC_LG2.h"


/* 任务属性定义--->各任务栈大小定义(以4字节为单位) */
#define EGC_TASK_STK_SIZE_ESP            (1024u)
#define EGC_TASK_STK_SIZE_CTL            (1024u)
#define EGC_TASK_STK_SIZE_MSR            (1024u)
#define EGC_TASK_STK_SIZE_HST            (1024u)
#define EGC_TASK_STK_SIZE_LG1            (2048u)
#define EGC_TASK_STK_SIZE_LG2            (2048u)

/* 任务属性定义--->各任务栈最大可利用空间定义 */
#define EGC_TASK_STK_SIZE_MAX_USAGE_ESP  (900u)
#define EGC_TASK_STK_SIZE_MAX_USAGE_CTL  (900u)
#define EGC_TASK_STK_SIZE_MAX_USAGE_MSR  (900u)
#define EGC_TASK_STK_SIZE_MAX_USAGE_HST  (900u)
#define EGC_TASK_STK_SIZE_MAX_USAGE_LG1  (1900u)
#define EGC_TASK_STK_SIZE_MAX_USAGE_LG2  (1900u)

/* 任务属性定义--->各任务栈最小剩余值 */
#define EGC_TASK_STK_SIZE_MIN_FREE_ESP   (EGC_TASK_STK_SIZE_ESP - EGC_TASK_STK_SIZE_MAX_USAGE_ESP)
#define EGC_TASK_STK_SIZE_MIN_FREE_CTL   (EGC_TASK_STK_SIZE_CTL - EGC_TASK_STK_SIZE_MAX_USAGE_CTL)
#define EGC_TASK_STK_SIZE_MIN_FREE_MSR   (EGC_TASK_STK_SIZE_MSR - EGC_TASK_STK_SIZE_MAX_USAGE_MSR)
#define EGC_TASK_STK_SIZE_MIN_FREE_HST   (EGC_TASK_STK_SIZE_HST - EGC_TASK_STK_SIZE_MAX_USAGE_HST)
#define EGC_TASK_STK_SIZE_MIN_FREE_LG1   (EGC_TASK_STK_SIZE_LG1 - EGC_TASK_STK_SIZE_MAX_USAGE_LG1)
#define EGC_TASK_STK_SIZE_MIN_FREE_LG2   (EGC_TASK_STK_SIZE_LG2 - EGC_TASK_STK_SIZE_MAX_USAGE_LG2)

/* 任务属性定义--->各任务优先级定义 */
#define EGC_TASK_PRIO_ESP                (6u)
#define EGC_TASK_PRIO_CTL                (7u)
#define EGC_TASK_PRIO_MSR                (8u)
#define EGC_TASK_PRIO_HST                (9u)
#define EGC_TASK_PRIO_LG1                (10u)
#define EGC_TASK_PRIO_LG2                (11u)


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __EGC_H__ */

/******* End of file EGC.h. *******/  
