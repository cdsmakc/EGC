/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : EGC_ErrCode.h
    Project name : EGC
    Module name  : 
    Date created : 2019年3月21日   18时23分40秒
    Author       : Ning.JianLi
    Description  : 本文件包含了项目的错误代码定义。
*******************************************************************************/

#ifndef __EGC_ERRCODE_H__
#define __EGC_ERRCODE_H__

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "pub_lib.h"

/* 错误码格式定义：
 * |--------------------------------------------------------------------------------|
 * | bit 31 ~ 24  | bit 23 ~ 20 | bit 19 ~ 16 |          bit 15 ~ 0                 |
 * |--------------------------------------------------------------------------------|
 * |   block id   |  err level  |  reserved   |            err code                 |
 * |--------------------------------------------------------------------------------|
 */

/* 错误码表模块编号定义 */
#define EGC_ERR_BLOCK_HST                                              (0x01000000)


/* 错误码表模块编号定义 */
#define EGC_ERR_LEVEL_FATAL                                            (0x00300000)
#define EGC_ERR_LEVEL_CRITICAL                                         (0x00200000)
#define EGC_ERR_LEVEL_GENERAL                                          (0x00100000)
#define EGC_ERR_LEVEL_PROMPT                                           (0x00000000)

/* ##############################################################################################
 * DBG模块错误码定义
 * ##############################################################################################
 */
#define EGC_ERR_CODE_HST_NORMAL                                        (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_PROMPT   | 0x00000000)

#define EGC_ERR_CODE_HST_INITVAR_CREATE_HOST_UART_SENDDONE_SEM_FAIL    (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_FATAL    | 0x00000101)
#define EGC_ERR_CODE_HST_INITVAR_CREATE_UART_MEM_FAIL                  (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_FATAL    | 0x00000102)
#define EGC_ERR_CODE_HST_INITVAR_CREATE_LOG_MSG_Q_FAIL                 (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_FATAL    | 0x00000103)
#define EGC_ERR_CODE_HST_INITVAR_CREATE_UART_RECV_MSG_Q_FAIL           (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_FATAL    | 0x00000104)
#define EGC_ERR_CODE_HST_INITVAR_CREATE_LOG_MEM_FAIL                   (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_FATAL    | 0x00000105)

#define EGC_ERR_CODE_HST_REPLYREG_GET_MEM_FAIL                         (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_FATAL    | 0x00000201)
#define EGC_ERR_CODE_HST_REPLYREG_PUT_MEM_FAIL                         (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_FATAL    | 0x00000202)

#define EGC_ERR_CODE_HST_GENLOG_GET_LOG_MEM_FAIL                       (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_CRITICAL | 0x00000401)
#define EGC_ERR_CODE_HST_GENLOG_POST_LOG_MSG_FAIL                      (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_CRITICAL | 0x00000403)

#define EGC_ERR_CODE_HST_UARTSEND_WAIT_SEND_DONE_FAIL                  (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_CRITICAL | 0x00000501)

#define EGC_ERR_CODE_HST_LOG_PEND_LOG_MSG_Q_FAIL                       (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_CRITICAL | 0x00000601)
#define EGC_ERR_CODE_HST_LOG_GET_UART_MEM_FAIL                         (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_CRITICAL | 0x00000602)
#define EGC_ERR_CODE_HST_LOG_PUT_UART_MEM_FAIL                         (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_CRITICAL | 0x00000603)
#define EGC_ERR_CODE_HST_LOG_PUT_LOG_MEM_FAIL                          (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_CRITICAL | 0x00000604)

#define EGC_ERR_CODE_HST_UARTINT_POST_USART_SEM_FAIL                   (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_CRITICAL | 0x00000701)
#define EGC_ERR_CODE_HST_UARTINT_GET_MEM_FAIL                          (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_CRITICAL | 0x00000702)
#define EGC_ERR_CODE_HST_UARTINT_POST_RECV_MSG_FAIL                    (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_CRITICAL | 0x00000703)
#define EGC_ERR_CODE_HST_UARTINT_PUT_MEM_FAIL                          (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_CRITICAL | 0x00000704)
#define EGC_ERR_CODE_HST_UARTINT_UART_UNKNOWN_INT                      (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_CRITICAL | 0x00000705)

#define EGC_ERR_CODE_HST_PROCESSRECVMSG_PEND_UART_RECV_MSG_Q_FAIL      (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_CRITICAL | 0x00000801)
#define EGC_ERR_CODE_HST_PROCESSRECVMSG_MSG_HEAD_ERR                   (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_CRITICAL | 0x00000802)
#define EGC_ERR_CODE_HST_PROCESSRECVMSG_MSG_L3LEN_MAX_ERR              (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_CRITICAL | 0x00000803)
#define EGC_ERR_CODE_HST_PROCESSRECVMSG_MSG_CHKSUM_ERR                 (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_CRITICAL | 0x00000804)
#define EGC_ERR_CODE_HST_PROCESSRECVMSG_MSG_DST_ERR                    (EGC_ERR_BLOCK_HST | EGC_ERR_LEVEL_CRITICAL | 0x00000805)

/* ##############################################################################################
 * MTR模块错误码定义
 * ##############################################################################################
 */

/* ##############################################################################################
 * MSR模块错误码定义
 * ##############################################################################################
 */

/* ##############################################################################################
 * HTG模块错误码定义
 * ##############################################################################################
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __EGC_ERRCODE_H__ */

/******* End of file EGC_ErrCode.h. *******/  
