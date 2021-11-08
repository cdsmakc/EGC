/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : pub_lib.h
    Project name : 公共模块。
    Module name  : 
    Date created : 2017年7月12日   13时45分30秒
    Author       : Ning.JianLi
    Description  : 公共模块。包含常用的函数和宏等。
*******************************************************************************/

#ifndef __PUB_LIB_H__
#define __PUB_LIB_H__

#include <stdio.h>
#include <stdarg.h>
#include "pub_type.h"

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

/* 宏定义 */
#ifndef CODE_SECTION
#define CODE_SECTION(Discription)
#endif

#ifndef OVERFLOW
#define OVERFLOW(_SIZE_, _VAL_) ((_SIZE_ - 1) & _VAL_)
#endif

/* 函数声明 */
int scnprintf(char *buf, size_t size, const char *fmt, ...) ;
int vscnprintf(char *buf, size_t size, const char *fmt, va_list args) ;


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __PUB_LIB_H__ */

/******* End of file pub_lib.h. *******/  
