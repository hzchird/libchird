
#ifndef __CHIRD_VIDEO_H__
#define __CHIRD_VIDEO_H__

// c语言不可以调用C++的函数，C++能否调用C的函数
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>
#include <linux/videodev2.h>

#define debug  printf

#define FORMAT_MJPEG		V4L2_PIX_FMT_MJPEG
#define FORMAT_YUYV 		V4L2_PIX_FMT_YUYV

typedef struct chird_video
{
	char * devname;
	int  framerate;			//桢率, 实际返回最接近的桢率
	int  width;				//宽度, 实际返回最接近的宽度
	int  height;			//高度, 实际返回最接近的高度
	int  format;
	void (*pfun)(const char * videodata, int length, void * param);
	void * param;	
	int framecount;			//需要分配的桢空间
}CHIRD_VIDEO_S;	


//功能，到/dev下面寻找有没有video设备，如果有返回设备名，如果没有返回NULL
//valididx 获取第几个有效视频设备, 一般valididx选0即可
char * chird_video_devscan(int valididx);

//功能: 视频初始化, 返回视频句柄
void * chird_video_init(CHIRD_VIDEO_S * pst);

//功能: 视频反初始化
int chird_video_uninit(void * handle);

//功能: 开启视频采集
int chird_video_start(void * handle);

//功能: 关闭视频采集
int chird_video_end(void * handle);

//功能: 获取视频参数
int chird_video_showparam(void * handle);

//功用: 获取桢数
int chird_video_getframecount(void * handle);

//功用: 清空桢数
int chird_video_clearframecount(void * handle);

// 查询视频信息
int chird_video_query_info(void * handle);

// 扫描视频支持格式
int chird_video_scan_support(void * handle);


	
#ifdef __cplusplus
}
#endif

#endif


