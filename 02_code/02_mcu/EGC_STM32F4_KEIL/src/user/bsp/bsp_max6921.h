/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : bsp_max6921.h
    Project name : MAX6921驱动代码。
    Module name  : 
    Date created : 2018年12月6日   19时19分37秒
    Author       : Ning.JianLi
    Description  : 本文件提供一种MAX6921的驱动方法。
*******************************************************************************/

#ifndef __BSP_MAX6921_H__
#define __BSP_MAX6921_H__

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "stm32f4xx.h"

#include "pub_type.h"
#include "pub_lib.h"


VOID BSP_MAX_Init
(
    VOID
) ;

VOID BSP_MAX_Blank
(
    BOOL_T bEnable
) ;

VOID BSP_MAX_Show
(
    CHAR  cDispChar, 
    UCHAR ucDispPos, 
    UCHAR ucDPShow
) ;



#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __BSP_MAX6921_H__ */

/******* End of file max6921.h. *******/  



