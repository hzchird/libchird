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
 
#include "chd_framebuf.h"

#define FB_LAYER_NUM 5
#if 0
struct fb_bitfield g_r = {10, 5, 0};
struct fb_bitfield g_g = {5,  5, 0};
struct fb_bitfield g_b = {0,  5, 0};
struct fb_bitfield g_t = {15, 1, 0};
#endif

struct fb_info
{
	char   name[10];				//显存设备名
	char   bcreate;					//是否创建
	int    fd;						//显存描述符
	void * start;					//显存起始地址

	unsigned int doublefb;			//是否双缓冲
	unsigned int colorkey;			//透明色
	
	unsigned int width;				//宽
	unsigned int height;			//高
	unsigned int stride;			//跨度
	unsigned int bits_per_pixel;	//象素大小
	unsigned int size;				//桢大小

	int reserve;					//保留
};

static struct fb_info g_fb[FB_LAYER_NUM] = {
	{.name = "/dev/fb0"},
	{.name = "/dev/fb1"},
	{.name = "/dev/fb2"},
	{.name = "/dev/fb3"},
	{.name = "/dev/fb4"},
};

void __fb_printvar(const struct fb_var_screeninfo * p)
{
	printf(" ---------- fb var screen information -----------\n");
	printf("\txres      = %d\n", p->xres);
	printf("\tyres      = %d\n", p->yres);
	printf("\txres_vir  = %d\n", p->xres_virtual);
	printf("\tyres_vir  = %d\n", p->yres_virtual);
	printf("\txoffset   = %d\n", p->xoffset);
	printf("\tyoffset   = %d\n", p->yoffset);
	printf("\tbit_per_p = %d\n", p->bits_per_pixel);
	printf("\tgrayscale = %d\n", p->grayscale);
	printf("\twidth     = %d\n", p->width);
	printf("\theight    = %d\n", p->height);
	printf("\tnonstd    = %d\n", p->nonstd);
	printf("\t---------------------\n");
	printf("\tred      (%2d %2d %2d)\n", p->red.offset, p->red.length, p->red.msb_right);
	printf("\tgreen    (%2d %2d %2d)\n", p->green.offset, p->green.length, p->green.msb_right);
	printf("\tblue     (%2d %2d %2d)\n", p->blue.offset, p->blue.length, p->blue.msb_right);
	printf("\ttransp   (%2d %2d %2d)\n", p->transp.offset, p->transp.length, p->transp.msb_right);
	printf("\t---------------------\n");
	printf("--------------------------------------------------\n");
}

int chird_fb_showscreenfix(int idx)
{
	struct fb_fix_screeninfo finfo;
	struct fb_info * pfb = &g_fb[idx];
	if(!pfb->bcreate){
		int fd;
		fd = open(pfb->name, O_RDONLY);
		if(fd < 0){
			perror("open fb failed");
			return -1;
		}
		if(ioctl(fd, FBIOGET_FSCREENINFO, &finfo)) {
			perror("get fbinfo failed");
			close(pfb->fd);
			return -1;
		}
		__fb_printfix(&finfo);
		
		close(fd);
	}
	else{
		if(ioctl(pfb->fd, FBIOGET_FSCREENINFO, &finfo)) {
			perror("get fbinfo failed");
			close(pfb->fd);
			return -1;
		}
		__fb_printfix(&finfo);
	}

	return 0;
}

int chird_fb_create(int idx, int x, int y, int w, int h, int pixel)
{
	struct fb_var_screeninfo vinfo;
	struct fb_info * pfb = &g_fb[idx];

	//已创建过，先销毁再重新创建
	if(pfb->bcreate == 1){
		chird_fb_destroy(idx);
	}
	
	pfb->fd = open(pfb[idx].name, O_RDWR);
	if(g_fb[idx].fd < 0){
		perror("error");
		return -1;
	}

	if(ioctl(pfb->fd, FBIOGET_VSCREENINFO, &vinfo)) {
		perror("get fbinfo failed");
		close(pfb->fd);
		return -1;
	}

	vinfo.xoffset = x;
	vinfo.yoffset = y;	
	vinfo.xres = w;
	vinfo.yres = h;
	vinfo.bits_per_pixel = pixel;
    vinfo.activate = FB_ACTIVATE_FORCE;
    vinfo.yres_virtual = vinfo.yres;
#if 0
	vinfo.red    = g_r;
	vinfo.green  = g_g;
	vinfo.blue   = g_b;
	vinfo.transp = g_t;
#endif
	if(ioctl(pfb->fd, FBIOPUT_VSCREENINFO, &vinfo)){
		perror("put fbinfo failed");
		close(pfb->fd);
		return -1;
	}

	pfb->colorkey= 0;
	pfb->width  = vinfo.xres;
	pfb->height = vinfo.yres;
	pfb->bits_per_pixel  = pixel/8;
	pfb->stride = pfb->bits_per_pixel * pfb->width;
	pfb->size   = pfb->stride * pfb->height;
	pfb->start  = (void *)mmap(0, pfb->size, PROT_READ | PROT_WRITE, MAP_SHARED, pfb->fd, 0);
	if(NULL == pfb->start){
		close(pfb->fd);
		return -1;
	}
	
	printf("Framebuffer(%s) info : addr = %#x (%d %d %d)\n",pfb->name, (int)pfb->start, vinfo.xres, vinfo.yres,vinfo.bits_per_pixel);
	
	//自动填充为0
//	memset(pfb->start, initcolor, pfb->size);

	pfb->bcreate = 1;
	
	return 0;
}


int chird_fb_destroy(int idx)
{
	struct fb_info * pfb = &g_fb[idx];
	if(!pfb->bcreate) return -1;
	
	munmap(pfb->start, pfb->size);

	close(pfb->fd);

	memset(pfb, 0, sizeof(struct fb_info));
	
	return 0;
}

int chird_fb_clear(int idx, int color)
{
	struct fb_info * pfb = &g_fb[idx];
	if(!pfb->bcreate) return -1;
	
	memset(pfb->start, color, pfb->size);

	return 0;
}

void __fb_printfix(const struct fb_fix_screeninfo * p)
{
	printf(" ---------- fb fix screen information -----------\n");
	printf("\tmem_start = %#x\n", (int)p->smem_start);
	printf("\tmem_len   = %d\n", p->smem_len);
	printf("\ttype      = %d\n", p->type);
	printf("\ttype_aux  = %d\n", p->type_aux);
	printf("\tvisual    = %d\n", p->visual);
	printf("\txpanstep  = %d\n", p->xpanstep);
	printf("\typanstep  = %d\n", p->ypanstep);
	printf("\tywrapstep = %d\n", p->ywrapstep);
	printf("\tline_len  = %d\n", p->line_length);
	printf("\tmmio_start= %d\n", (int)p->mmio_start);
	printf("\tmmio_len  = %d\n", p->mmio_len);
	printf("\taccel     = %d\n", p->accel);
	printf("--------------------------------------------------\n");
}





int chird_fb_showscreenvar(int idx)
{
	struct fb_var_screeninfo vinfo;
	struct fb_info * pfb = &g_fb[idx];
	if(!pfb->bcreate){
		int fd;
		fd = open(pfb->name, O_RDONLY);
		if(fd < 0){
			perror("open fb failed");
			return -1;
		}
		if(ioctl(fd, FBIOGET_VSCREENINFO, &vinfo)) {
			perror("get fbinfo failed");
			close(pfb->fd);
			return -1;
		}
		__fb_printvar(&vinfo);
		
		close(fd);
	}
	else{
		if(ioctl(pfb->fd, FBIOGET_VSCREENINFO, &vinfo)) {
			perror("get fbinfo failed");
			close(pfb->fd);
			return -1;
		}
		__fb_printvar(&vinfo);
	}
	return 0;
}

int chird_fb_setcolorkey(int idx, unsigned int color)
{
	struct fb_info * pfb = &g_fb[idx];
	if(!pfb->bcreate) return -1;

	pfb->colorkey = color;
	
	return 0;
}

int chird_fb_memcpy(int idx, int offset, const void * buffer, int size)
{
	struct fb_info * pfb = &g_fb[idx];
	if(!pfb->bcreate) return -1;
	
	memcpy(g_fb[idx].start + offset, buffer, size);
	
	return 0;
}

int chird_fb_disprect(int idx, const void * buffer, int x, int y, int w, int h)
{
	int i;
	struct fb_info * pfb = &g_fb[idx];
	if(!pfb->bcreate) return -1;
	
	int xoffset  = x * pfb->bits_per_pixel;		//x偏移量
	int rect_w   = w * pfb->bits_per_pixel;		//矩形宽
	int	rect_h   = h;							//矩形高

	char * fbmem = (char *)pfb->start + ((y * pfb->stride) + xoffset);	//显存起始地址
	char * buf   = (char *)buffer;										//矩形起始地址

	//若矩形宽度大于fb边界,做限制处理
	if(rect_w + xoffset > pfb->stride){
			rect_w = pfb->stride - xoffset;
			fprintf(stderr, "Warning : %s : Exceeds the maximum width\n", __FUNCTION__);
	}
	
	if(h + y > pfb->height){
			rect_h = pfb->height - y;
			fprintf(stderr, "Warning : %s : Exceeds the maximum height\n", __FUNCTION__);
	}
	
	for(i = 0; i < rect_h; i++){
		memcpy(fbmem, buf, rect_w);
		fbmem += pfb->stride;
		buf   += rect_w;
	}

	return 0;
	
}

char * fb_getmemory(int idx)
{
	struct fb_info * pfb = &g_fb[idx];
	if(!pfb->bcreate) return (char *)NULL;
	
	return (char *)pfb->start;
}

#if 0
int main(int argc, char ** argv)
{
	if(argc != 6){
		printf("usage : < x y w h> <color>\n");
		return -1;
	}
	
	char *buffer;
	int x,y,w,h,c;

	x = atoi(argv[1]);
	y = atoi(argv[2]);
	w = atoi(argv[3]);
	h = atoi(argv[4]);
	c = atoi(argv[5]);

	buffer = malloc(w*h*2);
	
	fb_showscreenfix(0);
	fb_showscreenvar(0);
	
	fb_create(0, 0, 0,480,272,16);
	
	memset(buffer, c, w*h*2);
	fb_disprect(0, buffer, x,y,w,h);
		
	fb_destroy(0);
	free(buffer);
	
	return 0;
}
#endif

