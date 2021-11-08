/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : EGC.c
    Project name : EGC
    Module name  : 
    Date created : 2020年02月11日   12时29分58秒
    Author       : Ning.JianLi
    Description  : FireFly荧光钟主控程序源文件。
*******************************************************************************/

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "EGC.h"

#include "drv_sdram_stm32f4xx.h"

/* 任务控制块与栈定义 */
static  OS_TCB   g_stEGCESPTaskTCB ;
static  CPU_STK  g_uiEGCESPTaskStk[EGC_TASK_STK_SIZE_ESP] ;

static  OS_TCB   g_stEGCCTLTaskTCB ;
static  CPU_STK  g_uiEGCCTLTaskStk[EGC_TASK_STK_SIZE_CTL] ;

static  OS_TCB   g_stEGCMSRTaskTCB ;
static  CPU_STK  g_uiEGCMSRTaskStk[EGC_TASK_STK_SIZE_MSR] ;

static  OS_TCB   g_stEGCHSTTaskTCB ;
static  CPU_STK  g_uiEGCHSTTaskStk[EGC_TASK_STK_SIZE_HST] ;

static  OS_TCB   g_stEGCLG1TaskTCB ;
static  CPU_STK  g_uiEGCLG1TaskStk[EGC_TASK_STK_SIZE_LG1] ;

static  OS_TCB   g_stEGCLG2TaskTCB ;
static  CPU_STK  g_uiEGCLG2TaskStk[EGC_TASK_STK_SIZE_LG2] ;


INT main(VOID)
{
    OS_ERR  enErr;

    /* 禁止所有中断 */
    CPU_IntDis() ;

    /* uC/OS-III操作系统初始化 */
    OSInit(&enErr) ;
    
    /* 系统配置 */ 
    SYS_SystemConfig() ;

    /* CPU初始化 */
    CPU_Init() ;

    /* SysTick定时器初始化 */
    SYS_SystemTickInit() ;
    
    /* 内存管理单元初始化 */
    Mem_Init() ;

    /* 数学运算单元初始化 */
    Math_Init() ;

    /* 初始化SDRAM */
    DRV_SDR_Init() ;

    /* 任务2：WIFI校时 */
    OSTaskCreate((OS_TCB     *)&g_stEGCESPTaskTCB,
                 (CPU_CHAR   *)"Task_ESP32",
                 (OS_TASK_PTR ) EGC_ESP_Task,
                 (void       *) 0,
                 (OS_PRIO     ) EGC_TASK_PRIO_ESP,
                 (CPU_STK    *)&g_uiEGCESPTaskStk[0],
                 (CPU_STK_SIZE) EGC_TASK_STK_SIZE_MIN_FREE_ESP,
                 (CPU_STK_SIZE) EGC_TASK_STK_SIZE_ESP,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&enErr);

    /* 任务3：控制与调度 */
    OSTaskCreate((OS_TCB     *)&g_stEGCCTLTaskTCB,
                 (CPU_CHAR   *)"Task_Ctrl",
                 (OS_TASK_PTR ) EGC_CTL_Task,
                 (void       *) 0,
                 (OS_PRIO     ) EGC_TASK_PRIO_CTL,
                 (CPU_STK    *)&g_uiEGCCTLTaskStk[0],
                 (CPU_STK_SIZE) EGC_TASK_STK_SIZE_MIN_FREE_CTL,
                 (CPU_STK_SIZE) EGC_TASK_STK_SIZE_CTL,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&enErr);

    /* 任务4：测量 */
    OSTaskCreate((OS_TCB     *)&g_stEGCMSRTaskTCB,
                 (CPU_CHAR   *)"Task_Measure",
                 (OS_TASK_PTR ) EGC_MSR_Task,
                 (void       *) 0,
                 (OS_PRIO     ) EGC_TASK_PRIO_MSR,
                 (CPU_STK    *)&g_uiEGCMSRTaskStk[0],
                 (CPU_STK_SIZE) EGC_TASK_STK_SIZE_MIN_FREE_MSR,
                 (CPU_STK_SIZE) EGC_TASK_STK_SIZE_MSR,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&enErr);

    /* 任务5：主机接口 */
    OSTaskCreate((OS_TCB     *)&g_stEGCHSTTaskTCB,
                 (CPU_CHAR   *)"Task_Host",
                 (OS_TASK_PTR ) EGC_HST_Task,
                 (void       *) 0,
                 (OS_PRIO     ) EGC_TASK_PRIO_HST,
                 (CPU_STK    *)&g_uiEGCHSTTaskStk[0],
                 (CPU_STK_SIZE) EGC_TASK_STK_SIZE_MIN_FREE_HST,
                 (CPU_STK_SIZE) EGC_TASK_STK_SIZE_HST,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&enErr);

    /* 任务6：LCD组1显示控制 */
    OSTaskCreate((OS_TCB     *)&g_stEGCLG1TaskTCB,
                 (CPU_CHAR   *)"Task_LCD_Group1",
                 (OS_TASK_PTR ) EGC_LG1_Task,
                 (void       *) 0,
                 (OS_PRIO     ) EGC_TASK_PRIO_LG1,
                 (CPU_STK    *)&g_uiEGCLG1TaskStk[0],
                 (CPU_STK_SIZE) EGC_TASK_STK_SIZE_MIN_FREE_LG1,
                 (CPU_STK_SIZE) EGC_TASK_STK_SIZE_LG1,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&enErr);

    /* 任务7：LCD组2显示控制 */
    OSTaskCreate((OS_TCB     *)&g_stEGCLG2TaskTCB,
                 (CPU_CHAR   *)"Task_LCD_Group2",
                 (OS_TASK_PTR ) EGC_LG2_Task,
                 (void       *) 0,
                 (OS_PRIO     ) EGC_TASK_PRIO_LG2,
                 (CPU_STK    *)&g_uiEGCLG2TaskStk[0],
                 (CPU_STK_SIZE) EGC_TASK_STK_SIZE_MIN_FREE_LG2,
                 (CPU_STK_SIZE) EGC_TASK_STK_SIZE_LG2,
                 (OS_MSG_QTY  ) 5u,
                 (OS_TICK     ) 0u,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&enErr);

    OSStart(&enErr);

    return (0u) ;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file EGC.c. *******/  
