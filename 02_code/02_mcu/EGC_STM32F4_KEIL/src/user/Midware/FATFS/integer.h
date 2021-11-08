/*-------------------------------------------*/
/* Integer type definitions for FatFs module */
/*-------------------------------------------*/

#ifndef _FF_INTEGER
#define _FF_INTEGER

#ifdef _WIN32	/* Development platform */

#include <windows.h>
#include <tchar.h>

#else			/* Embedded platform */

/* This type MUST be 8-bit */
typedef unsigned char	BYTE;

/* These types MUST be 16-bit */
#ifndef SHORT
typedef short			SHORT;
#endif
typedef unsigned short	WORD;
typedef unsigned short	WCHAR;

/* These types MUST be 16-bit or 32-bit */
#ifndef INT
typedef int				INT;
#endif
#ifndef UINT
typedef unsigned int	UINT;
#endif

/* These types MUST be 32-bit */
#ifndef LONG
typedef long			LONG;
#endif
#ifndef DWORD
typedef unsigned long	DWORD;
#endif

/* These types MUST be 64-bit */
//typedef long long		      	LONG;
//typedef unsigned long long ULONG;
//typedef unsigned long	long DWORD;
#endif

#endif
