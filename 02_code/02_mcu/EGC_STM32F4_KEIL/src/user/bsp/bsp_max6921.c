/*******************************************************************************
 Copyright SAKC Corporation. 2016. All rights reserved.
--------------------------------------------------------------------------------
    File name    : bsp_max6921.c
    Project name : MAX6921驱动代码
    Module name  : 
    Date created : 2018年12月6日   18时51分7秒
    Author       : Ning.JianLi
    Description  : 本文件提供一种MAX6921的驱动方法。
*******************************************************************************/

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

#include "bsp_max6921.h"

/* 能够显示的字符
 * 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
 *  -,   ,   ,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,   ,   ,   ,   ,   ,   ,   ,
 *
 * 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
 *  A,  B,  C,  D,  E,  F,  G,  H,  I,  J,  K,  L,          O,  P,  Q,  R,  S,  T,
 * 
 * 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,
 *  U,                                              a,  b,  c,  d,  e,      g,  h,
 *
 *105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122
 *  i,  j,   ,  l,   ,   ,  o,  p,  q,  r,  s,   ,  u,   ,   ,   ,   ,
 */

UCHAR g_ucChar2Seg[128] = 
{
/*     0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
/*    16    17    18    19    20    21    22    23    24    25    26    27    28    29    30    31 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
/*    32    33    34    35    36    37    38    39    40    41    42    43    44    45    46    47 */
/*                                                                                   -             */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 
/*    48    49    50    51    52    53    54    55    56    57    58    59    60    61    62    63 */
/*     0     1     2     3     4     5     6     7     8     9                                     */
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
/*    64    65    66    67    68    69    70    71    72    73    74    75    76    77    78    79 */
/*           A     B     C     D     E     F     G     H     I     J           L                 O */
    0x00, 0x77, 0x7F, 0x39, 0x3F, 0x79, 0x71, 0x7D, 0x76, 0x06, 0x0E, 0x00, 0x38, 0x00, 0x00, 0x3F, 
/*    80    81    82    83    84    85    86    87    88    89    90    91    92    93    94    95 */
/*     P     Q     R     S     T     U                                                             */
    0x73, 0xBF, 0x77, 0x6D, 0x07, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
/*    96    97    98    99   100   101   102   103   104   105   106   107   108   109   110   111 */
/*           a     b     c     d     e           g     h     i     j           l                 o */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
/*   112   113   114   115   116   117   118   119   120   121   122   123   124   125   126   127 */
/*     p     q     r                 u                                                             */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x4C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
} ;


/*******************************************************************************
- Function    : __BSP_MAX_GPIOConfig
- Description : 本函数配置STM32驱动MAX6921的引脚。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      : 
*******************************************************************************/
VOID __BSP_MAX_GPIOConfig(VOID)
{
    GPIO_InitTypeDef stGPIOInitInfo ;
    
    /* 使能GPIO时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE) ;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE) ;
    
    /* MAX6921电路相关配置 */
    /*
     * PE5 : BLANK，需要配置为数字输出
     * PE4 : LOAD，需要配置为数字输出
     * PE2 : CLK，需要配置为数字输出
     * PE3 : DATA，需要配置为数字输出
     * PA1 : EN，需要配置为数字输出
     */
    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_OUT ;
    stGPIOInitInfo.GPIO_Speed = GPIO_Speed_50MHz ;
    stGPIOInitInfo.GPIO_OType = GPIO_OType_PP ;
    stGPIOInitInfo.GPIO_PuPd  = GPIO_PuPd_NOPULL ;

    stGPIOInitInfo.GPIO_Pin   = GPIO_Pin_2 ;
    GPIO_Init(GPIOE, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin   = GPIO_Pin_3 ;
    GPIO_Init(GPIOE, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin   = GPIO_Pin_4 ;
    GPIO_Init(GPIOE, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin   = GPIO_Pin_5 ;
    GPIO_Init(GPIOE, &stGPIOInitInfo) ;

    stGPIOInitInfo.GPIO_Pin   = GPIO_Pin_1 ;
    stGPIOInitInfo.GPIO_OType = GPIO_OType_OD ;
    
    GPIO_Init(GPIOA, &stGPIOInitInfo) ;
    
    return ;
}

/*******************************************************************************
- Function    : __BSP_MAX_Send
- Description : 本函数设置MAX6921的显示引脚。
- Input       : ucPosition : 设置需要显示的位。合法值0到7。
                             0为最高(左侧)位，7为最低(右侧)位。
                ucData     : 需要显示的值。从最高位到最低位依次为DP，G，F，E，D,
                             C，B，A。对应位为1时显示该段点亮，0时熄灭。
- Output      : NULL
- Return      : void
- Others      : 注意本函数的时序是否满足MAX6921的时序参数，必要的情况下需要调整。
                CLK周期              : 至少200 ns
                CLK高电平持续时间         : 至少90  ns
                CLK低电平持续时间         : 至少90  ns
                CLK上升沿与LOAD上升沿间隔 : 至少100 ns
                LOAD高电平维持时间        : 至少55  ns
                MAX6921分配 : 
                19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
                SF SB G5 G8 G6 G4 DP SD SE SG SC G2 G7 G3 SA G9 G1
                19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
                NC P9  a P1  f P3  b P5  g P8  e P7  c P6  d P4 dp P2 NC NC
*******************************************************************************/
VOID __BSP_MAX_Send(UCHAR ucPosition, UCHAR ucData)
{
    UINT uiData ;
    UINT uiLoop ;

    /* 数据拼接，先拼接位，再拼接段 */
    switch(ucPosition)
    {
        case 7 : uiData = 0x00008 ; break ;
        case 6 : uiData = 0x00100 ; break ;
        case 5 : uiData = 0x00040 ; break ;
        case 4 : uiData = 0x04000 ; break ;
        case 3 : uiData = 0x20000 ; break ;
        case 2 : uiData = 0x08000 ; break ;
        case 1 : uiData = 0x00080 ; break ;
        case 0 : uiData = 0x10000 ; break ;
    }
    /* SEG A拼接 */
    if(0x01 == (ucData & 0x01))
    {
        uiData |= 0x00020 ;
    }
    /* SEG B拼接 */
    if(0x02 == (ucData & 0x02))
    {
        uiData |= 0x40000 ;
    }
    /* SEG C拼接 */
    if(0x04 == (ucData & 0x04))
    {
        uiData |= 0x00200 ;
    }
    /* SEG D拼接 */
    if(0x08 == (ucData & 0x08))
    {
        uiData |= 0x01000 ;
    }
    /* SEG E拼接 */
    if(0x10 == (ucData & 0x10))
    {
        uiData |= 0x00800 ;
    }
    /* SEG F拼接 */
    if(0x20 == (ucData & 0x20))
    {
        uiData |= 0x80000 ;
    }
    /* SEG G拼接 */
    if(0x40 == (ucData & 0x40))
    {
        uiData |= 0x00400 ;
    }
    /* SEG DP拼接 */
    if(0x80 == (ucData & 0x80))
    {
        uiData |= 0x02000 ;
    }
    
    /* 先将CLK和LOAD引脚设置为低电平 */
    GPIO_ResetBits(GPIOE, GPIO_Pin_2) ;
    GPIO_ResetBits(GPIOE, GPIO_Pin_4) ;

    /* 移位数据进入MAX6921 */
    for(uiLoop = 0 ; uiLoop <= 19 ; uiLoop++)
    {
        /* 将数据放到MAX6921的DATA数据线上 */
        if(1 == (0x01 & (uiData >> (19 - uiLoop))))
        {
            GPIO_SetBits(GPIOE, GPIO_Pin_3) ;
        }
        else /* 0 == (uiData >> (19 - uiLoop)) */
        {
            GPIO_ResetBits(GPIOE, GPIO_Pin_3) ;
        }

        /* 给出时钟 */
        GPIO_SetBits(GPIOE, GPIO_Pin_2) ;
        /* 撤销时钟 */
        GPIO_ResetBits(GPIOE, GPIO_Pin_2) ;
    }

    /* 给出LOAD信号，加载数据 */
    GPIO_SetBits(GPIOE, GPIO_Pin_4) ;

    /* 撤销LOAD信号，完成数据加载 */
    GPIO_ResetBits(GPIOE, GPIO_Pin_4) ;

    return ;
}


/*******************************************************************************
- Function    : BSP_MAX_Init
- Description : 本函数初始化MAX6921功能。
- Input       : NULL
- Output      : NULL
- Return      : VOID
- Others      :
*******************************************************************************/
VOID BSP_MAX_Init(VOID)
{
    /* 初始化GPIO */
    __BSP_MAX_GPIOConfig() ;

    return ;
}

/*******************************************************************************
- Function    : BSP_MAX_Blank
- Description : 本函数设置MAX6921芯片的BLANK引脚，控制其是否驱动VFD显示。
- Input       : BOOL_FALSE : 禁止BLANK功能，MAX6921正常显示；
                BOOL_TRUE  : 允许BLANK功能，MAX6921不显示。
- Output      : NULL
- Return      : void
- Others      :
*******************************************************************************/
VOID BSP_MAX_Blank(BOOL_T bEnable)
{
    if(BOOL_FALSE == bEnable)
    {
        /* 复位BLANK，允许显示 */
        GPIO_ResetBits(GPIOE, GPIO_Pin_5) ;
        /* 复位灯丝电压门控，允许显示 */
        GPIO_ResetBits(GPIOA, GPIO_Pin_1) ;
    }
    else if(BOOL_TRUE == bEnable)
    {
        /* 置位BLANK，禁止显示 */
        GPIO_SetBits(GPIOE, GPIO_Pin_5) ;
        /* 置位灯丝电压门控，禁止显示 */
        GPIO_SetBits(GPIOA, GPIO_Pin_1) ;
    }

    return ;
}

/*******************************************************************************
- Function    : BSP_MAX_Show
- Description : 本函数设置显示值。
- Input       : cDispChar : 需要显示的字符。
                ucDispPos : 指向需要显示的位置。
                ucDPShow  : 指示是否显示DP。8bit分别对应8位管。
- Output      : NULL
- Return      : void
- Others      :
*******************************************************************************/
VOID BSP_MAX_Show(CHAR cDispChar, UCHAR ucDispPos, UCHAR ucDPShow)
{
    UCHAR ucShowData ;

    /* 参数限制 */
    cDispChar &= 0x7F ;
    ucDispPos  = (ucDispPos > 7)? 7 : ucDispPos ;
    

    ucShowData = g_ucChar2Seg[cDispChar] ;

    /* 判断是否显示DP */
    if(0x01 & (ucDPShow >> ucDispPos))
    {
        ucShowData |= 0x80 ;
    }
        
    /* 显示当前位 */
    __BSP_MAX_Send(ucDispPos, ucShowData) ;

    return ;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
/******* End of file bsp_max6921.c. *******/  

