
/* 接收Buffer及指针 */
CHAR      g_acGPSRecvBuf[128] ;
UCHAR     g_ucGPSRecvPtr ;


UINT __IV18_GPS_GPRMCParse(CHAR pcGPRMCInfo, struct tm *pstGPSTime)
{
    CHAR *pcCharStart ;
    CHAR acBuf[10] ;
    UINT uiRetval ;  

    /* 搜索第一个间隔符 */
    pcCharStart = strchr(pcGPRMCInfo, ',') ;
    pcCharStart++ ;
    
    /* hhmmss.sss */
    /* 转换时 */
    acBuf[0] = *pcCharStart ;
    acBuf[1] = *(pcCharStart + 1) ;
    acBuf[2] = '\0' ;
    
    pstGPSTime->tm_hour = atoi(acBuf) ;

    /* 转换分 */
    acBuf[0] = *(pcCharStart + 2) ;
    acBuf[1] = *(pcCharStart + 3) ;
    acBuf[2] = '\0' ;

    pstGPSTime->tm_min = atoi(acBuf) ;

    /* 转换秒 */
    acBuf[0] = *(pcCharStart + 4) ;
    acBuf[1] = *(pcCharStart + 5) ;
    acBuf[2] = '\0' ;

    pstGPSTime->tm_sec = atoi(acBuf) ;

    /* 搜索第二个间隔符 */
    pcCharStart = strchr(pcCharStart, ',') ;
    pcCharStart++ ;

    /* 解析定位状态 */
    if('A' == (*pcCharStart))
    {
        /* 定位有效 */
        uiRetval = 1 ;
    }
    else
    {
        /* 定位无效 */
        uiRetval = 0 ;
    }

    /* 搜索第九个间隔符 */
    pcCharStart = strchr(pcCharStart, ',') ; /* 3 */
    pcCharStart++ ;
    pcCharStart = strchr(pcCharStart, ',') ; /* 4 */
    pcCharStart++ ;
    pcCharStart = strchr(pcCharStart, ',') ; /* 5 */
    pcCharStart++ ;
    pcCharStart = strchr(pcCharStart, ',') ; /* 6 */
    pcCharStart++ ;
    pcCharStart = strchr(pcCharStart, ',') ; /* 7 */
    pcCharStart++ ;
    pcCharStart = strchr(pcCharStart, ',') ; /* 8 */
    pcCharStart++ ;
    pcCharStart = strchr(pcCharStart, ',') ; /* 9 */
    pcCharStart++ ;

    /* 解析年月日 */
    /* ddmmyy */
    /* 转换日 */
    acBuf[0] = *(pcCharStart + 0) ;
    acBuf[1] = *(pcCharStart + 1) ;
    acBuf[2] = '\0' ;

    pstGPSTime->tm_mday = atoi(acBuf) ;
    
    /* 转换月 */
    acBuf[0] = *(pcCharStart + 2) ;
    acBuf[1] = *(pcCharStart + 3) ;
    acBuf[2] = '\0' ;

    pstGPSTime->tm_mon = atoi(acBuf) ;

    /* 转换年 */
    acBuf[0] = *(pcCharStart + 4) ;
    acBuf[1] = *(pcCharStart + 5) ;
    acBuf[2] = '\0' ;

    pstGPSTime->tm_year = atoi(acBuf) ;

    /* 其他struct tm参数填写 */
    pstGPSTime->tm_isdst = 0 ;

    return uiRetval ;
}

VOID USART3_IRQHandler(VOID)
{
    UCHAR ucTmp ;
    UINT  uiRTCTime ;
    
    if(SET == USART_GetITStatus(USART1, USART_IT_RXNE))
    {
        ucTmp = USART_ReceiveData(USART1) ;

        if('$' == ucTmp)
        {
            g_ucGPSRecvPtr = 0 ;
        }
        else
        {
            if(120 == g_ucGPSRecvPtr)
            {
                /* 太长，接收肯定有误，保护指针 */
                g_ucGPSRecvPtr = 0 ;
            }
        }

        /* 接收的字符写入缓存，并累加指针 */
        g_acGPSRecvBuf[g_ucGPSRecvPtr++] = ucTmp ;
        g_acGPSRecvBuf[127] = '\0' ;
        
        /* 解析 */
        if('\r' == ucTmp)
        {
            if(0 = strncmp(g_acGPSRecvBuf, "$GPRMC", strlen("$GPRMC")))
            {
                /* 匹配OK，可以进行解析 */
                if(1 == __IV18_GPS_GPRMCParse(g_acGPSRecvBuf, &g_stGPSTime))
                {
                    /* 定位有效 */
                    uiRTCTime = mktime(&g_stGPSTime) ;

                    /* 将时间刷新到RTC中 */
                    RTC_SetCounter(uiRTCTime) ;
                }
            }
        }
    }

    return ;
}




VOID IV18_GPS_Init(VOID)
{
    GPIO_InitTypeDef  stGPIOInitInfo ;
    USART_InitTypeDef stUSARTCfgInfo ;

    /* 使能GPIO时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE) ;
    
    /* GPS使用PB11/UART3接收 */
    stGPIOInitInfo.GPIO_Pin   = GPIO_Pin_11 ;
    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_IN_FLOATING ;
    stGPIOInitInfo.GPIO_Speed = GPIO_Speed_10MHz ;

    GPIO_Init(GPIOB, &stGPIOInitInfo) ;

    /* 使用PB2控制GPS模块上下电 */
    stGPIOInitInfo.GPIO_Pin   = GPIO_Pin_2 ;
    stGPIOInitInfo.GPIO_Mode  = GPIO_Mode_Out_PP ;
    stGPIOInitInfo.GPIO_Speed = GPIO_Speed_10MHz ;

    GPIO_Init(GPIOB, &stGPIOInitInfo) ;

    /* 设置引脚为低电平，默认不使能GPS模块 */
    GPIO_ResetBits(GPIOB, GPIO_Pin_2) ;

    /* 配置UART3 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE) ;

    stUSARTCfgInfo.USART_BaudRate            = 9600 ;
    stUSARTCfgInfo.USART_HardwareFlowControl = USART_HardwareFlowControl_None ;
    stUSARTCfgInfo.USART_Mode                = USART_Mode_Rx ;                   /* 仅接收模式 */
    stUSARTCfgInfo.USART_Parity              = USART_Parity_No ;
    stUSARTCfgInfo.USART_StopBits            = USART_StopBits_1 ;
    stUSARTCfgInfo.USART_WordLength          = USART_WordLength_8b ;

    return ;
}

VOID IV18_GPS_AdjustTime(VOID)
{
    /* 启动GPS */
    GPIO_SetBits(GPIOB, GPIO_Pin_2) ;

    /*  */
}















