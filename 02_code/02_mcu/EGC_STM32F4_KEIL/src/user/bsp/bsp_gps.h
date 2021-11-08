




typedef struct tagIV18_GPS_WORKAREA
{
    union
    {
        UCHAR aucRawData[173] ;
        
        struct
        {
            /* BYTE0 */

            /* 从GPS转换来的time信息 */ /* 44B */
            struct tm stGPSTime ;
        } ;
    } ;
}IV18_GPS_WORKAREA_S ;
















