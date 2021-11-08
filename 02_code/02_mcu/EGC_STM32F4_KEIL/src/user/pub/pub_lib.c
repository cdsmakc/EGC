/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : pub_lib.c
    Project name : 公共模块。
    Module name  : 
    Date created : 2017年7月12日   13时44分40秒
    Author       : Ning.JianLi
    Description  : 公共模块。包含常用的函数和宏等。
*******************************************************************************/

#include "pub_lib.h"

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

int scnprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list ap ;
    int len ;

    va_start(ap, fmt) ;
    len = vsnprintf(buf, size, fmt, ap) ;
    va_end(ap) ;

    return (len >= size)? (size - 1) : len ;
}

int vscnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
    int len ;

    len = vsnprintf(buf, size, fmt, args) ;

    return (len >= size)? (size - 1) : len ;
}



#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file pub_lib.c. *******/  
