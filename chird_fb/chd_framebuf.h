/*-----------------------------------------------------------------*
 * Copyright (C),
 * FileName:            frambuffer.c
 * Author:             	meifj
 * Version:             
 * Datetime:           	2011-10-30	create 	by meifj
 						2012-5-8	modification	by meifj
 						2013-5-10	modification	by meifj
 						2013-7-7	modification	by meifj
 * Description:         基于frambuffer for linux显示 
 * Version information: 优化，代码封装，增加功能
 *-----------------------------------------------------------------*/
#ifndef _CHIRD_FRAME_BUF_H
#define _CHIRD_FRAME_BUF_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

/* 创建显存 */
int chird_fb_create(int index, int x, int y, int w, int h, int pixel);

/* 销毁显存 */
int chird_fb_destroy(int index);

/* 显存清空 */
int chird_fb_clear(int index, int color);

/* 打印屏幕固定信息*/
int chird_fb_showscreenfix(int idx);

/* 打印屏幕可变信息 */
int chird_fb_showscreenvar(int idx);

#endif

