//2012年11月25日, PM 10:46:01 by mfj

#include <stdio.h>
#include "chd_alsa.h"

#define CHIRD_ALSA_MAGIC 0x11223355
#define ALSA_NUM		32

#define alsa_error(stream, err, handle)do{		\
	fprintf(stderr, "alsa error : "stream"(%s)\n", snd_strerror(err));	\
	snd_pcm_close(handle);	\
	return (void *)NULL;	\
}while(0)

#define alsa_mixer_error(stream, err, handle)do{		\
	fprintf(stderr, "alsa mixer error : "stream"(%s)\n", snd_strerror(err));	\
	snd_mixer_close(handle);	\
	return -1;	\
}while(0)

typedef struct chird_alsa_info
{
	int  magic;			// 幻数
	snd_pcm_t * handle;
	char devname[20];
	snd_pcm_stream_t stream;
	int access;
	int rate;					//采集频率
	int channel;				//通道数
	int byteperframe;			//每桢的字节大小
}CHIRD_ALSA_INFO_S;

static void * __alsa_init_hw_param(snd_pcm_t * handle, int rate, int access, int channel)
{
	int err;
	snd_pcm_hw_params_t * hw_params;
	unsigned int exact_rate = 0;
	int bits = SND_PCM_FORMAT_S16_LE;

	/* 2 allocate the snd_pcm_hw_params_t structure on the stack */
	if((err = snd_pcm_hw_params_malloc(&hw_params)) < 0){
		alsa_error("cannot allocate hardware parameters", err, handle);
	}

	/* 3 Init hwparams with full configureation space */
	if((err = snd_pcm_hw_params_any(handle, hw_params)) < 0){
		alsa_error("cannot initialize hardware parameters structure", err, handle);
	}

	/* 4 set access type */
	if((err = snd_pcm_hw_params_set_access(handle, hw_params, access)) < 0){
		alsa_error("cannot set access type", err, handle);
	}

	/* 5 set sample format */
	if((err = snd_pcm_hw_params_set_format(handle, hw_params, bits)) < 0){
		alsa_error("cannot set sample format", err, handle);
	}

	/* 6 set sample rate, if the exact rate is not supported by the hardware, 
		use nearest possible rate */
	exact_rate = rate;
	if((err = snd_pcm_hw_params_set_rate_near(handle, hw_params, &exact_rate, 0)) < 0){
		alsa_error("cannot set sample rate", err, handle);	
	}

	if(exact_rate != rate){
		fprintf(stderr, "The rate %d Hz is supported by your hardware. ==> using %d Hz \n",bits, exact_rate);
	}
	
	/* 7 set number of channels */
	if((err = snd_pcm_hw_params_set_channels(handle, hw_params, channel)) < 0){
		alsa_error("cannot set channel count", err, handle);
	}	

	/* 8 apply HW parameter settings to PCM device and prepare device */
	if((err = snd_pcm_hw_params(handle, hw_params)) < 0){
		alsa_error("cannot set params", err, handle);	
	}

	/* 9 free parameters structure spsace*/
	snd_pcm_hw_params_free(hw_params);
	
	/* 10 prepare audio */
	if((err = snd_pcm_prepare(handle)) < 0){
		alsa_error("cannot prepare audio interface for use", err, handle);	
	}
	return handle;
	
}

/*
	alsa音频初始化	默认为16位小端数据
	返回值:   音频句柄
	dev	  :   设备名， 默认为default		plugin:0:0
	stream:   播放、录音  0为播放，1为录音
	rate  :	  采样频率， 会挑选相近且有效的频率
	channel:  音频通道
*/
//void * chird_alsa_init(const char * dev, int stream, int rate, int channel)
void * chird_alsa_init(const CHIRD_ALSA_S * pst)
{
	int err;
	CHIRD_ALSA_INFO_S * pstalsa;
	
	if((NULL == pst)
		&& (pst->stream != ALSA_STREAM_TYPE_PLAY  || pst->stream != ALSA_STREAM_TYPE_SAMPLE)
		&& (pst->rate != CHIRD_ALSA_RATE_8K       || pst->rate != CHIRD_ALSA_RATE_16K
			|| pst->rate != CHIRD_ALSA_RATE_44_1K || pst->rate != CHIRD_ALSA_RATE_48K)
		&& (pst->channel != CHIRD_ALSA_CHN_SINGLE || pst->channel != CHIRD_ALSA_CHN_STEREO)){
		printf("chird_alsa_init param failed\n");
		return NULL;
	}
		

	pstalsa = (CHIRD_ALSA_INFO_S *)calloc(1, sizeof(CHIRD_ALSA_INFO_S));
	if(NULL == pstalsa){
		perror("callco failed");
		return NULL;
	}

	sprintf(pstalsa->devname, "plughw:%d,%d",  pst->card, pst->device);\

	pstalsa->stream = (pst->stream == 0) ? SND_PCM_STREAM_PLAYBACK : SND_PCM_STREAM_CAPTURE;
	pstalsa->access = SND_PCM_ACCESS_RW_INTERLEAVED;	//SND_PCM_ACCESS_RW_NONINTERLEAVED
	pstalsa->rate = pst->rate;
	pstalsa->channel = pst->channel;
	pstalsa->byteperframe = pst->channel * 2;
	pstalsa->magic = CHIRD_ALSA_MAGIC;

	/* 1 Open Pcm, the last parameter of this function is the mode */
	if((err = snd_pcm_open(&pstalsa->handle, pstalsa->devname, pstalsa->stream, 0)) < 0){
		alsa_error("cannot open audio devce", err, pstalsa->handle);
		return (void *)NULL;
	}

	if(pstalsa->handle  != __alsa_init_hw_param(pstalsa->handle, 
												pstalsa->rate, 
												pstalsa->access, 
												pstalsa->channel)){
		
		return NULL;
	}



	printf("\t ----------- chird_alsa_init successful  -----------\n");
	return (void*)pstalsa;
	
}

int chird_alsa_uninit(void * handle)
{
	CHIRD_ALSA_INFO_S * pstalsa = (CHIRD_ALSA_INFO_S *)handle;

	if(NULL == pstalsa || pstalsa->magic != CHIRD_ALSA_MAGIC){
		printf("chird_alsa : handle wrong!..\n");
		return -1;
	}
		
	snd_pcm_close(pstalsa->handle);
	
	free(handle);
	
	printf("\t ----------- chird_alsa_uninit successful  -----------\n");
	
	return 0;
}

//读写时以一个采样频率为周期， 
//如频率设置为44.1KHZ, 那么读写一秒钟的默认
int chird_alsa_sample(void * handle, void * buffer, unsigned long length)
{
	int rn;
	CHIRD_ALSA_INFO_S * pstalsa = (CHIRD_ALSA_INFO_S *)handle;

	if(NULL == pstalsa || pstalsa->magic != CHIRD_ALSA_MAGIC){
		printf("chird_alsa : handle wrong!..\n");
		return -1;
	}
	
	length /= pstalsa->byteperframe;		//对外部而言，读取长度，内部采集则以桢为单位
	
	if((rn = snd_pcm_readi(pstalsa->handle, buffer, length)) != length){				
		fprintf(stderr, "read from audio interface failed(%s)\n", snd_strerror(rn));
		return -1;
	}

	return rn * pstalsa->byteperframe;
}

int chird_alsa_playback(void * handle, void * buffer, unsigned long length)
{
	int pn;
	CHIRD_ALSA_INFO_S * pstalsa = (CHIRD_ALSA_INFO_S *)handle;

	if(NULL == pstalsa || pstalsa->magic != CHIRD_ALSA_MAGIC){
		printf("chird_alsa : handle wrong!..\n");
		return -1;
	}
	
	length /= pstalsa->byteperframe;	//对外部而言，播放长度，内部采集则以桢为单位
	
	while((pn = snd_pcm_writei(pstalsa->handle, buffer, length)) != length){
		if(pn == -EPIPE){
			//完成硬件参数设置，使设备准备好   
			snd_pcm_prepare(pstalsa->handle); 
		}
		else if(pn < 0){
			fprintf(stderr, "write to audio interface failed(%s)\n", snd_strerror(pn));
			return -1;
		}
	}

	return pn * pstalsa->byteperframe;
}

#if 0
int main()
{
	CHIRD_ALSA_S stalsa;
	void * record, * play;
	char buf[4096];


	//
	stalsa.card = 0;
	stalsa.device = 0;
	stalsa.stream = ALSA_STREAM_TYPE_PLAY;
	stalsa.rate = CHIRD_ALSA_RATE_48K;
	stalsa.channel = CHIRD_ALSA_CHN_STEREO;
	play = chird_alsa_init(&stalsa);

	stalsa.card = 2;
	stalsa.device = 0;
	stalsa.stream = ALSA_STREAM_TYPE_SAMPLE;
	record = chird_alsa_init(&stalsa);

	while(1){
		//采集
//		printf("sample..\n");
		chird_alsa_sample(record, buf, 4096);
		//播放
//		printf("play..\n");
		chird_alsa_playback(play, buf, 4096);
	}

	
	chird_alsa_uninit(record);
	chird_alsa_uninit(play);

	return 0;
}
#endif
