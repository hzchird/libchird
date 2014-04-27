/*---------------------------------------------------------*
 * Copyright (C),
 * FileName:    jpeg.h
 * Author:      meifj
 * Version:             
* Datetime:     2011-10-30	create 	by meifj
				2012-5-8	modification	by meifj
				2013-5-10	modification	by meifj
				2013-7-7	modification	by meifj
				2014-03-10  modification	by meifj 
							使用libjpeg8，增加编码功能
 * Description:         基于libjpeg 
 * Version information: 优化，代码封装，增加功能
 *-----------------------------------------------------------*/
#ifndef _CHIRD_JPEG_H_
#define _CHIRD_JPEG_H_

#include <stdio.h>
#include <jpeglib.h>

typedef struct jpeg_info{
	unsigned int width;
	unsigned int height;
	unsigned int stride;
	unsigned int pixel;
	unsigned int pixel_num;
	unsigned char  * buf;
	unsigned long size;
}CHIRD_JPEG_ENC_S;

typedef struct jpeg_
{
	unsigned int   w;
	unsigned int   h;
	unsigned char * buf;
}CHIRD_JPEG_DEC_S;


	
/*	
	jpeg图片解码，解压后的数据在jp->buf中
	图片信息会自动填充到pj中
	解码完后需要调用jpeg_release()函数
*/
#if 0
int jpeg_decodebyfile(struct jpeg_info * pj, const char * filename);
int jpeg_decodebybuffer(struct jpeg_info * pj, unsigned char * buffer, unsigned int size);
int jpeg_showinfo(struct jpeg_info *pj);
/*
	jpeg图片编码，对YUV420(YV12)可编
	编码前需要对pj的width和height进行填充
	编码后的数据在jp->buf中，大小在jp->size中
	编码完后需要调用jpeg_release()函数
*/
int jpeg_encodetofile(struct jpeg_info * pj, int quality, const char * filename);
int jpeg_encodetobuffer(struct jpeg_info * pj, int quality, char * yuv420);
#endif

int chird_jpeg_decbybuffer(const void * jpegbuf, void * rgb24, int size);
int chird_jpeg_decbyfile(const char * filename, void * rgb24);

int chird_jpeg_showbybuffer(const void * jpegbuf, int size);
int chird_jpeg_showbyfile(const char * filename);



//int chird_jpeg_encotobuffer(const char * yv12, 
//							void * jpegbuf, 
//							int w, int h,  unsigned long * psize,
//							int quality);

int chird_jpeg_encotofile(const char * yv12, 
							const char * filename, 
							int w, int h,
							int quality);



/*
	颜色转换，测试用
*/
int jpeg_yuyv_to_yv12(unsigned char * dest,const unsigned char * src, int w, int h);

#endif
