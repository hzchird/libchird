/*-----------------------------------------------------------------*
 * Copyright (C),
 * FileName:            jpeg.h
 * Author:             	meifj
 * Version:             
 * Datetime:           	2011-10-30	create 	by meifj
 						2012-5-8	modification	by meifj
 						2013-5-10	modification	by meifj
 						2013-7-7	modification	by meifj
 						2014-03-10  modification	by meifj 
 													使用libjpeg8，增加编码功能
 * Description:         基于libjpeg 
 * Version information: 优化，代码封装，增加功能
 *-----------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <errno.h>

#include "chird_jpeg.h"

struct my_error_mgr {
	struct jpeg_error_mgr pub;	/* "public" fields */
	jmp_buf setjmp_buf;			/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

void my_emit_message (j_common_ptr cinfo, int msg_level)
{
	my_error_ptr myerr = (my_error_ptr) cinfo->err;  
	if(msg_level == -1) longjmp(myerr->setjmp_buf, 1);  
}

#if 0
int jpeg_decodebyfile(struct jpeg_info * pj, const char * filename)
{
	int line;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	unsigned char * p;
	FILE *fp;

	fp = fopen(filename, "r");
	if(NULL == fp){
		printf("fopen failed : %s : %s\n", filename, strerror(errno));
		return -1;
	}

	cinfo.err = jpeg_std_error(&jerr);  		// 分配并初始化一个JPEG解压对象
	jpeg_create_decompress(&cinfo);   			// 创建解压文件
	jpeg_stdio_src(&cinfo, fp);  				// 指定要解压缩的文件 
	jpeg_read_header(&cinfo, TRUE);     		// 获取图像信息
	jpeg_start_decompress(&cinfo);      		// 开始解压

	pj->width 		= cinfo.output_width;
	pj->height 		= cinfo.output_height;
	pj->pixel	 	= cinfo.output_components;
	pj->pixel_num	= cinfo.output_width * pj->height;
	pj->stride 		= pj->width * cinfo.output_components;
	pj->size 		= pj->height*pj->stride;
	pj->buf 		= malloc(pj->size);

	p = pj->buf;
	for(line = 0; line < pj->height; line++){
    	jpeg_read_scanlines(&cinfo, &p, 1);  	
		p += pj->stride;
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(fp);

	return 0;
}


	
int jpeg_decodebybuffer(struct jpeg_info * pj, unsigned char * buffer, unsigned int size)
{
	int line;
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	unsigned char * p;

	cinfo.err = jpeg_std_error(&jerr.pub);		// 分配并初始化一个JPEG解压对象
	jerr.pub.emit_message = my_emit_message;
	if (setjmp(jerr.setjmp_buf)) {
		jpeg_destroy_decompress(&cinfo);
		return -1;
	}
	
	jpeg_create_decompress(&cinfo);   			// 创建解压文件

	jpeg_mem_src(&cinfo, buffer, size);		// 指定要解压缩的缓存
	jpeg_read_header(&cinfo, TRUE);     		// 获取图像信息
	jpeg_start_decompress(&cinfo);      		// 开始解压

	pj->width 		= cinfo.output_width;
	pj->height 		= cinfo.output_height;
	pj->pixel	 	= cinfo.output_components;
	pj->pixel_num	= cinfo.output_width * pj->height;
	pj->stride 		= pj->width * cinfo.output_components;
	pj->size 		= pj->height*pj->stride;
	pj->buf 		= malloc(pj->size);
//	printf("size = %d, w = %d h = %d\n", pj->size, pj->width, pj->height);
	p = pj->buf;

	for(line = 0; line < pj->height; line++){
    	jpeg_read_scanlines(&cinfo, &p, 1);  	
		p += pj->stride;
	}
	
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	return 0;  
}

int jpeg_encodetofile(struct jpeg_info * pj, 
					int quality, 
					const char * filename)
{
	struct jpeg_compress_struct cinfo; 
	struct jpeg_error_mgr jerr; 

	FILE * outfile;   
	JSAMPIMAGE  buffer; 
	unsigned char *pSrc, *pDst; 
	int band,i,buf_width[3], buf_height[3]; 
	cinfo.err = jpeg_std_error(&jerr); 
	int w ,h;
	
	w = pj->width;
	h = pj->height;
	unsigned char* yData = pj->buf;
	unsigned char* uData = yData + w * h;
	unsigned char* vData = uData + w * h/2;
    
	jpeg_create_compress(&cinfo); 
	if ((outfile = fopen(filename, "wb")) == NULL) { 
		return -1; 
	} 

	jpeg_stdio_dest(&cinfo, outfile); 

	cinfo.image_width = w; 
	cinfo.image_height = h; 
	cinfo.input_components = 3;    

	jpeg_set_defaults(&cinfo); 
	jpeg_set_quality(&cinfo, quality, TRUE ); 

	cinfo.raw_data_in = TRUE; 
	cinfo.jpeg_color_space = JCS_YCbCr; 
	cinfo.comp_info[0].h_samp_factor = 2; 
	cinfo.comp_info[0].v_samp_factor = 2; 

	jpeg_start_compress(&cinfo, TRUE); 

	buffer = (JSAMPIMAGE) (*cinfo.mem->alloc_small) ((j_common_ptr) &cinfo, 
			JPOOL_IMAGE, 3 * sizeof(JSAMPARRAY)); 

	for(band = 0; band < 3; band++) 
	{ 
		buf_width[band] = cinfo.comp_info[band].width_in_blocks * DCTSIZE; 
		buf_height[band] = cinfo.comp_info[band].v_samp_factor * DCTSIZE; 
		buffer[band] = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, 
				JPOOL_IMAGE, buf_width[band], buf_height[band]); 
	} 	 

	unsigned char *rawData[3]; 
	rawData[0] = yData;
	rawData[1] = uData;
	rawData[2] = vData;

	int src_width[3],src_height[3]; 
	for(i = 0; i < 3; i++) 
	{ 
		src_width[i]=(i==0)?w:w/2; 
		src_height[i]=(i==0)?h:h/2; 
	} 

	//max_line一般为16，外循环每次处理16行数据。 
	int max_line = cinfo.max_v_samp_factor * DCTSIZE; 
	int counter;
	for(counter=0; cinfo.next_scanline < cinfo.image_height; counter++) 
	{ 
		//buffer image copy. 
		for(band=0; band <3; band++)  //每个分量分别处理 
		{ 
			int mem_size = src_width[band];//buf_width[band]; 
			pDst = (unsigned char *) buffer[band][0]; 
			pSrc = (unsigned char *) rawData[band] + counter*buf_height[band] * src_width[band];//buf_width[band];  //yuv.data[band]分别表示YUV起始地址 

			for(i=0; i <buf_height[band]; i++)  //处理每行数据 
			{ 
				memcpy(pDst, pSrc, mem_size); 
				pSrc += src_width[band];//buf_width[band]; 
				pDst += buf_width[band]; 
			} 
		} 

		jpeg_write_raw_data(&cinfo, buffer, max_line); 
	} 
	jpeg_finish_compress(&cinfo); 

	fclose(outfile); 
	jpeg_destroy_compress(&cinfo); 

	return 0; 
}


int jpeg_encodetobuffer(struct jpeg_info * pj, int quality, char * yuv420)
{
	struct jpeg_compress_struct cinfo; 
	struct jpeg_error_mgr jerr; 

	JSAMPIMAGE  buffer; 
	unsigned char *pSrc, *pDst; 
	int band,i,buf_width[3], buf_height[3]; 
	cinfo.err = jpeg_std_error(&jerr); 
	int w ,h;
	
	w = pj->width;
	h = pj->height;
	unsigned char* yData = yuv420;
	unsigned char* uData = yData + w * h;
	unsigned char* vData = uData + (w * h / 4);
    
	jpeg_create_compress(&cinfo); 

	pj->buf = malloc(w*h/2);
	jpeg_mem_dest(&cinfo, &pj->buf, &pj->size); 

	cinfo.image_width = w; 
	cinfo.image_height = h; 
	cinfo.input_components = 3;    

	jpeg_set_defaults(&cinfo); 
	jpeg_set_quality(&cinfo, quality, TRUE); 

	cinfo.raw_data_in = TRUE; 
	cinfo.jpeg_color_space = JCS_YCbCr; 
	cinfo.comp_info[0].h_samp_factor = 2; 
	cinfo.comp_info[0].v_samp_factor = 2; 

	jpeg_start_compress(&cinfo, TRUE); 
	
	buffer = (JSAMPIMAGE) (*cinfo.mem->alloc_small) ((j_common_ptr) &cinfo, 
			JPOOL_IMAGE, 3 * sizeof(JSAMPARRAY)); 

	for(band = 0; band < 3; band++) 
	{ 
		buf_width[band] = cinfo.comp_info[band].width_in_blocks * DCTSIZE; 
		buf_height[band] = cinfo.comp_info[band].v_samp_factor * DCTSIZE; 
		buffer[band] = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, 
				JPOOL_IMAGE, buf_width[band], buf_height[band]); 
	} 	 

	unsigned char * rawData[3]; 
	rawData[0] = yData;
	rawData[1] = uData;
	rawData[2] = vData;

	int src_width[3],src_height[3]; 
	for(i = 0; i < 3; i++) 
	{ 
		src_width[i]=(i==0)?w:w/2; 
		src_height[i]=(i==0)?h:h/2; 
	} 

	//max_line一般为16，外循环每次处理16行数据。 
	int max_line = cinfo.max_v_samp_factor * DCTSIZE; 
	int counter;
	for(counter=0; cinfo.next_scanline < cinfo.image_height; counter++) 
	{ 
		//buffer image copy. 
		for(band=0; band <3; band++)  //每个分量分别处理 
		{ 
			int mem_size = src_width[band];//buf_width[band]; 
			pDst = (unsigned char *) buffer[band][0]; 
			pSrc = (unsigned char *) rawData[band] + counter*buf_height[band] * src_width[band];//buf_width[band];  //yuv.data[band]分别表示YUV起始地址 

			for(i=0; i <buf_height[band]; i++)  //处理每行数据 
			{ 
				memcpy(pDst, pSrc, mem_size); 
				pSrc += src_width[band];//buf_width[band]; 
				pDst += buf_width[band]; 
			} 
		} 

		jpeg_write_raw_data(&cinfo, buffer, max_line); 
	} 
	
	jpeg_finish_compress(&cinfo); 
	
	// 这里才可以获得图片大小
	
	jpeg_destroy_compress(&cinfo); 

	return 0; 
}



int jpeg_RGB888toRGB565(struct jpeg_info *pj)
{
	int i;
	unsigned short * sp = (unsigned short *)pj->buf;
	unsigned char * p = pj->buf;
	unsigned short  B;
	unsigned short  G;
	unsigned short  R;

	for(i = 0; i < pj->pixel_num; i++, p += 3){
		R = ((*p >> 3) << 11) 	& 0xF800;
		G = ((*(p+1) >> 2) << 5)& 0x07E0;
		B = (*(p+2) >> 3) 		& 0X001F;
			
		*sp++ = R | G | B;
	}

	pj->pixel = 16;
	pj->stride = pj->width * 2;
	pj->size = pj->stride*pj->height;
	
	return 0;
}

int jpeg_release(struct jpeg_info *pj)
{
	if(pj->buf != NULL){
		free(pj->buf);
	}
	
	memset(pj, 0x00, sizeof(struct jpeg_info));
	
	return 0;
}

int jpeg_showinfo(struct jpeg_info *pj)
{
	printf("Jpeg Information : (%d %d %d)\n", pj->width, pj->height, pj->pixel);
	
	return 0;
}

int jpeg_yuyv_to_yv12(unsigned char * dest,
			  	 const unsigned char * src, 
			  	 int w, int h)
{
    const int yuyv_frm_sz = w * h * 2;
	unsigned char * psrc = (unsigned char *)src;
	int i, r = 0;
	unsigned char * y;
	unsigned char * u;
	unsigned char * v;

	if(w % 2 != 0 && h % 2 != 0) return -1;

	y = dest;
	u = y + (w * h);
	v = u + (w * h / 4);

	//Y
	for(i = 0;i < yuyv_frm_sz; i += 2)
		*y++ = src[i];

	for(r = 0; r < h; r += 2) {	
		psrc = (unsigned char *)src + (r * w * 2);
	    for(i = 0; i < w * 2; i += 4){
			*u++ = psrc[i+1];
			*v++ = psrc[i+3];
	    }
	}
	
	return 0;
}

#endif
int chird_jpeg_decbybuffer(const void * jpegbuf, void * rgb24, int size)
{
	int line;
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	unsigned char * p;
	int stride, h;

	if(NULL == jpegbuf || NULL == rgb24) return -1;
	
	cinfo.err = jpeg_std_error(&jerr.pub);		// 分配并初始化一个JPEG解压对象
	jerr.pub.emit_message = my_emit_message;
	if (setjmp(jerr.setjmp_buf)) {
		jpeg_destroy_decompress(&cinfo);
		return -1;
	}
	
	jpeg_create_decompress(&cinfo);   			// 创建解压文件

	jpeg_mem_src(&cinfo, (unsigned char*)jpegbuf, size);	// 指定要解压缩的缓存
	jpeg_read_header(&cinfo, TRUE);     		// 获取图像信息
	jpeg_start_decompress(&cinfo);      		// 开始解压

	p = rgb24;
	h =  cinfo.output_height; 
	stride = cinfo.output_width * cinfo.output_components;
	for(line = 0; line < h; line++){
    	jpeg_read_scanlines(&cinfo, &p, 1);  	
		p += stride;
	}
	
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	return 0;
}

int chird_jpeg_decbyfile(const char * filename, void * rgb24)
{
	int line;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	unsigned char * p;
	int stride,  h;
	FILE *fp;

	if(NULL == filename || NULL == rgb24) return -1;

	fp = fopen(filename, "r");
	if(NULL == fp){
		printf("fopen failed : %s : %s\n", filename, strerror(errno));
		return -1;
	}

	cinfo.err = jpeg_std_error(&jerr);  		// 分配并初始化一个JPEG解压对象
	jpeg_create_decompress(&cinfo);   			// 创建解压文件
	jpeg_stdio_src(&cinfo, fp);  				// 指定要解压缩的文件 
	jpeg_read_header(&cinfo, TRUE);     		// 获取图像信息
	jpeg_start_decompress(&cinfo);      		// 开始解压

	p = rgb24;
	h =  cinfo.output_height; 
	stride = cinfo.output_width * cinfo.output_components;
	for(line = 0; line < h; line++){
    	jpeg_read_scanlines(&cinfo, &p, 1);  	
		p += stride;
	}
	
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(fp);
	return 0;
}

int chird_jpeg_showbybuffer(const void * jpegbuf, int size)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;

	
	cinfo.err = jpeg_std_error(&jerr.pub);		// 分配并初始化一个JPEG解压对象
	jerr.pub.emit_message = my_emit_message;
	if (setjmp(jerr.setjmp_buf)) {
		jpeg_destroy_decompress(&cinfo);
		return -1;
	}
	
	jpeg_create_decompress(&cinfo);   			// 创建解压文件

	jpeg_mem_src(&cinfo, (unsigned char*)jpegbuf, size);	// 指定要解压缩的缓存
	jpeg_read_header(&cinfo, TRUE);     		// 获取图像信息
	jpeg_start_decompress(&cinfo);      		// 开始解压

	printf("w      = %d\n", cinfo.output_width);
	printf("h      = %d\n", cinfo.output_height);
	printf("pixel  = %d\n", cinfo.output_components);
	printf("size   = %d\n", cinfo.output_width*cinfo.output_height*
								cinfo.output_components);
	
	jpeg_finish_decompress(&cinfo);
	
	jpeg_destroy_decompress(&cinfo);
	
	return 0;
}


int chird_jpeg_showbyfile(const char * filename)
{
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
	FILE *fp;


	fp = fopen(filename, "r");
	if(NULL == fp){
		printf("fopen failed : %s : %s\n", filename, strerror(errno));
		return -1;
	}
	
	cinfo.err = jpeg_std_error(&jerr.pub);		// 分配并初始化一个JPEG解压对象
	jerr.pub.emit_message = my_emit_message;
	if (setjmp(jerr.setjmp_buf)) {
		jpeg_destroy_decompress(&cinfo);
		return -1;
	}
	
	jpeg_create_decompress(&cinfo);   			// 创建解压文件
	jpeg_stdio_src(&cinfo, fp);
	jpeg_read_header(&cinfo, TRUE);     		// 获取图像信息
	jpeg_start_decompress(&cinfo);      		// 开始解压

	printf("w      = %d\n", cinfo.output_width);
	printf("h      = %d\n", cinfo.output_height);
	printf("pixel  = %d\n", cinfo.output_components);
	printf("size   = %d\n", cinfo.output_width*cinfo.output_height*
								cinfo.output_components);
	
	jpeg_finish_decompress(&cinfo);
	
	jpeg_destroy_decompress(&cinfo);

	fclose(fp);
	
	return 0;
}


//jpegbuf只能是由指针
int chird_jpeg_encotobuffer(const char * yv12, 
							void ** jpegbuf, 
							int w, int h, unsigned long * psize,
							int quality)
{
	struct jpeg_compress_struct cinfo; 
	struct jpeg_error_mgr jerr; 

	JSAMPIMAGE  buffer; 
	unsigned char *pSrc, *pDst; 
	int band,i,buf_width[3], buf_height[3]; 
	cinfo.err = jpeg_std_error(&jerr); 

	unsigned char * yData = (unsigned char *)yv12;
	unsigned char * uData = yData + w * h;
	unsigned char * vData = uData + (w * h / 4);
    
	jpeg_create_compress(&cinfo); 

	jpeg_mem_dest(&cinfo, (unsigned char**)jpegbuf, (unsigned long *)psize); 

	cinfo.image_width = w; 
	cinfo.image_height = h; 
	cinfo.input_components = 3;    

	jpeg_set_defaults(&cinfo); 
	jpeg_set_quality(&cinfo, quality, TRUE); 

	cinfo.raw_data_in = TRUE; 
	cinfo.jpeg_color_space = JCS_YCbCr; 
	cinfo.comp_info[0].h_samp_factor = 2; 
	cinfo.comp_info[0].v_samp_factor = 2; 

	jpeg_start_compress(&cinfo, TRUE); 
	
	buffer = (JSAMPIMAGE) (*cinfo.mem->alloc_small) ((j_common_ptr) &cinfo, 
			JPOOL_IMAGE, 3 * sizeof(JSAMPARRAY)); 

	for(band = 0; band < 3; band++) 
	{ 
		buf_width[band] = cinfo.comp_info[band].width_in_blocks * DCTSIZE; 
		buf_height[band] = cinfo.comp_info[band].v_samp_factor * DCTSIZE; 
		buffer[band] = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, 
				JPOOL_IMAGE, buf_width[band], buf_height[band]); 
	} 	 

	unsigned char * rawData[3]; 
	rawData[0] = yData;
	rawData[1] = uData;
	rawData[2] = vData;

	int src_width[3],src_height[3]; 
	for(i = 0; i < 3; i++) 
	{ 
		src_width[i]=(i==0)?w:w/2; 
		src_height[i]=(i==0)?h:h/2; 
	} 

	//max_line一般为16，外循环每次处理16行数据。 
	int max_line = cinfo.max_v_samp_factor * DCTSIZE; 
	int counter;
	for(counter=0; cinfo.next_scanline < cinfo.image_height; counter++) 
	{ 
		//buffer image copy. 
		for(band=0; band <3; band++)  //每个分量分别处理 
		{ 
			int mem_size = src_width[band];//buf_width[band]; 
			pDst = (unsigned char *) buffer[band][0]; 
			pSrc = (unsigned char *) rawData[band] + counter*buf_height[band] * src_width[band];//buf_width[band];  //yuv.data[band]分别表示YUV起始地址 

			for(i=0; i <buf_height[band]; i++)  //处理每行数据 
			{ 
				memcpy(pDst, pSrc, mem_size); 
				pSrc += src_width[band];//buf_width[band]; 
				pDst += buf_width[band]; 
			} 
		} 

		jpeg_write_raw_data(&cinfo, buffer, max_line); 
	} 
	
	jpeg_finish_compress(&cinfo); 
	
	// 这里才可以获得图片大小
	
	jpeg_destroy_compress(&cinfo); 

	return 0; 
}


int chird_jpeg_encotofile(const char * yv12, 
							const char * filename, 
							int w, int h,
							int quality)
{
	struct jpeg_compress_struct cinfo; 
	struct jpeg_error_mgr jerr; 
	

	FILE * outfile;   
	JSAMPIMAGE  buffer; 
	unsigned char *pSrc, *pDst; 
	int band,i,buf_width[3], buf_height[3]; 

	if(NULL == yv12 || NULL == filename) return -1;
	
	cinfo.err = jpeg_std_error(&jerr); 

	unsigned char* yData = (unsigned char *)yv12;
	unsigned char* uData = yData + w * h;
	unsigned char* vData = uData + w * h / 4;
    
	jpeg_create_compress(&cinfo); 
	if ((outfile = fopen(filename, "wb")) == NULL) { 
		return -1; 
	} 

	jpeg_stdio_dest(&cinfo, outfile); 

	cinfo.image_width = w; 
	cinfo.image_height = h; 
	cinfo.input_components = 3;    

	jpeg_set_defaults(&cinfo); 
	jpeg_set_quality(&cinfo, quality, TRUE ); 

	cinfo.raw_data_in = TRUE; 
	cinfo.jpeg_color_space = JCS_YCbCr; 
	cinfo.comp_info[0].h_samp_factor = 2; 
	cinfo.comp_info[0].v_samp_factor = 2; 

	jpeg_start_compress(&cinfo, TRUE); 

	buffer = (JSAMPIMAGE) (*cinfo.mem->alloc_small) ((j_common_ptr) &cinfo, 
			JPOOL_IMAGE, 3 * sizeof(JSAMPARRAY)); 

	for(band = 0; band < 3; band++) 
	{ 
		buf_width[band] = cinfo.comp_info[band].width_in_blocks * DCTSIZE; 
		buf_height[band] = cinfo.comp_info[band].v_samp_factor * DCTSIZE; 
		buffer[band] = (*cinfo.mem->alloc_sarray) ((j_common_ptr) &cinfo, 
				JPOOL_IMAGE, buf_width[band], buf_height[band]); 
	} 	 

	unsigned char *rawData[3]; 
	rawData[0] = yData;
	rawData[1] = uData;
	rawData[2] = vData;

	int src_width[3], src_height[3]; 
	for(i = 0; i < 3; i++) 
	{ 
		src_width[i]=(i==0)?w:w/2; 
		src_height[i]=(i==0)?h:h/2; 
	} 

	//max_line一般为16，外循环每次处理16行数据。 
	int max_line = cinfo.max_v_samp_factor * DCTSIZE; 
	int counter;
	for(counter=0; cinfo.next_scanline < cinfo.image_height; counter++) 
	{ 
		//buffer image copy. 
		for(band=0; band <3; band++)  //每个分量分别处理 
		{ 
			int mem_size = src_width[band];//buf_width[band]; 
			pDst = (unsigned char *) buffer[band][0]; 
			pSrc = (unsigned char *) rawData[band] + counter*buf_height[band] * src_width[band];//buf_width[band];  //yuv.data[band]分别表示YUV起始地址 

			for(i=0; i <buf_height[band]; i++)  //处理每行数据 
			{ 
				memcpy(pDst, pSrc, mem_size); 
				pSrc += src_width[band];//buf_width[band]; 
				pDst += buf_width[band]; 
			} 
		} 

		jpeg_write_raw_data(&cinfo, buffer, max_line); 
	} 
	jpeg_finish_compress(&cinfo); 

	fclose(outfile); 
	
	jpeg_destroy_compress(&cinfo); 
	
	return 0;
}


#if 0	


#define PIC_W 640
#define PIC_H 480
#define QUALITY 70

int main()
{
	FILE * fp;

#if 0
	fb_create(0, 0, 0, 1280, 720, 32);
	char * fb = fb_getmemory(0);
	fb_clear(0, 0xff);
	
	char rgb24[PIC_W*PIC_H*3], buf[PIC_W*PIC_H];
	
	fp = fopen("./test.jpeg", "r");
	fread(buf, 1, PIC_W*PIC_H*3,  fp);	fclose(fp);
	
	chird_jpeg_decbybuffer(buf, rgb24, PIC_W*PIC_H*3);
	display_rgb24torgb32(fb, 1280, rgb24, PIC_W, PIC_H);
	
	chird_jpeg_decbyfile("./test.jpeg", rgb24);
	display_rgb24torgb32(fb + PIC_W*4, 1280, rgb24, PIC_W, PIC_H);
#else

	char yuyv[PIC_W*PIC_H*2];
	char yv12[PIC_W*PIC_H*2];
	char * jpegbuf = malloc(PIC_W*PIC_H);
	unsigned long size;
		
	fp = fopen("./test_yuyv.yuv", "r");
	fread(yuyv, 1, PIC_W*PIC_H*2, fp);
	close(fp);
	
	yuyv_to_yv12(yuyv, yv12, PIC_W, PIC_H);
	
	chird_jpeg_encotofile(yv12, "./test1.jpeg", PIC_W, PIC_H, QUALITY);

	yuyv_to_yv12(yuyv, yv12, PIC_W, PIC_H);
	chird_jpeg_encotobuffer(yv12, (void**)&jpegbuf, PIC_W, PIC_H, &size, QUALITY);
	printf("encoder size = %d\n", size);
	
	fp = fopen("./test2.jpeg", "wb");
	fwrite(jpegbuf, 1, size, fp); fclose(fp);

	
#endif
	
	return 0;
}


int display_rgb24torgb32(char * pfb, int fbw, char * pbuf, int bufw, int bufh)
{
	int i, j;
	for(i = 0; i < bufh; i++){
		for(j = 0; j < bufw; j++){
			pfb[j*4+2] = pbuf[j*3+0];
			pfb[j*4+1] = pbuf[j*3+1];
			pfb[j*4+0] = pbuf[j*3+2];	
		}
		
		pbuf += bufw * 3;
		pfb += fbw * 4;
	}
}

int yuyv_to_yv12(const unsigned char * src, 
				unsigned char * dest,
				int w, int h)
{
    const int yuyv_frm_sz = w * h * 2;
	unsigned char * psrc = (unsigned char *)src;
	int i, r = 0;
	unsigned char * y;
	unsigned char * u;
	unsigned char * v;

	if(w % 2 != 0 && h % 2 != 0) return -1;

	y = dest;
	u = y + (w * h);
	v = u + (w * h / 4);

	//Y
	for(i = 0;i < yuyv_frm_sz; i += 2)
		*y++ = src[i];

	for(r = 0; r < h; r += 2) {	
		psrc = (unsigned char *)src + (r * w * 2);
	    for(i = 0; i < w * 2; i += 4){
			*u++ = psrc[i+1];
			*v++ = psrc[i+3];
	    }
	}
	
	return 0;
}
#endif



