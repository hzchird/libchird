/*
**	********************************************************************************
**                                    chd_type.h
**                           
**
**	File  Name   :  chd_type.h
**	Description  : 
**	Modification : 2011-03-28		MFJ		Create the file
				   2014-05-11		MFJ		Modify & Refresh
**	********************************************************************************
*/

#ifndef CHD_TYPE_H_
#define CHD_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ANSI C 头文件 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <dirent.h>

/* Linux 系统头文件 */
#ifdef __linux__
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/io.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/shm.h>			/* TimYu changed */
#include <sys/mount.h>
#include <sys/sysmacros.h>
#include <sys/poll.h>
#include <syslog.h>

/* Linux网络套接字 */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#endif

typedef short				CHD_SINT16;
typedef unsigned short		CHD_UINT16;
typedef int					CHD_SINT32;
typedef unsigned int		CHD_UINT32;
typedef char				CHD_SINT8;
typedef unsigned char		CHD_UINT8;
typedef float 				CHD_FLOAT;
typedef double				CHD_DOUBLE;

typedef unsigned char 		BOOL;
typedef unsigned char		BYTE;
typedef unsigned short		WORD;
typedef signed int			LONG;

#ifndef SUCCESS
#define SUCCESS         	0
#endif

#ifndef FAILURE
#define FAILURE       		-1
#endif

#ifndef TRUE
#define TRUE 				1
#endif

#ifndef FALSE
#define FLASE 				0
#endif

#ifndef NULL
#define NULL				0
#endif

#ifndef ON
#define ON					1
#endif

#ifndef OFF
#define OFF					0
#endif

/* 最大值 */
#ifndef MAX
#define MAX(a,b)		(((a) > (b)) ? (a) : (b))
#endif

/* 最小值 */
#ifndef MIN
#define MIN(a,b)		(((a) < (b)) ? (a) : (b))
#endif

/* 绝对值  */
#ifndef ABS
#define ABS(x) ((x)>=0?(x):-(x))
#endif

/* 延时函数, 不会被信号及系统调用打断 */
#include <sys/time.h>
#include <sys/select.h>
#define CHD_USLEEP(sec, usec) {\
	struct timeval tv = {sec, usec};	\
	do{}while(select(0, NULL, NULL, NULL, &tv) != 0);	\
}


/* 求数组大小 */
#define TABLE_SIZE(table)	(sizeof(table)/sizeof(table[0]))

/* 位操作 */
#define BITMSK(bit)			(1 << (bit))
/* 位置0 */
#define BITSET0(v, bit)		(v) &= ~ BITMSK(bit)
/* 位置 1*/
#define BITSET1(v, bit)		(v) |= BITMSK(bit)
/* 取低8位 */
#define LOBYTE(v)			((CHD_UINT8)(v))
/* 取高8位 */
#define HIBYTE(v)			((CHD_UINT8)(((CHD_UINT8)(v) >> 8) & 0xFFFF))
/* 取低16位 */
#define LOWORD(v)           ((CHD_UINT32)(v))
/* 取高16位 */
#define HIWORD(v)           ((CHD_UINT32)(((CHD_UINT32)(v) >> 16) & 0xFFFF))
/* 组合成16位 */
#define MAKEWORD(a, b)      ((CHD_UINT16)(((CHD_UINT8)(a)) | ((CHD_UINT16)((CHD_UINT8)(b))) << 8))
/* 组合成32位 */
#define MAKELONG(a, b)      ((CHD_UINT32)(((CHD_UINT16)(a)) | ((CHD_UINT32)((CHD_UINT16)(b))) << 16))

/* 打印 */
#define CHD_PRINT(fmt, arg ...) 		printf(fmt, arg)

/* 错误打错 */
#define CHD_ERR_PRINT(fmt,arg ...) 		fprintf(stderr, fmt, arg)

/* 调试打印行数 */
#define CHD_DEBUG_LINE(i) 				CHD_ERR_PRINT("%d\n", i)


/* 参数错误打印 */
#define CHD_ERR_PARAM(fmt,arg ...) 		CHD_ERR_PRINT("[%s] Parameter error line : %d " fmt,\
													__FUNCTION__, __LINE__, ## arg)

/* 返回检测 */
#define CHD_CHECK_RET(s32ret, funname)do{ \
	if(SUCCESS != s32ret){ \
		CHD_ERR_PRINT("[fun : %s] failed at <%s.c : %d> errno : %s!\n",	\
							funname, __FUNCTION__, __LINE__, strerror(errno));	\
		return FAILURE;\
	}\
}while(0)



#ifdef __cplusplus
}
#endif

#endif

