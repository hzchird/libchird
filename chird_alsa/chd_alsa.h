//2012年11月25日, PM 10:46:01 by mfj
#ifndef _CHIRD_ALSA1_H_
#define _CHIRD_ALSA1_H_

#include <alsa/asoundlib.h>

//操作类型
typedef enum{
	ALSA_STREAM_TYPE_PLAY = 0,			//播放
	ALSA_STREAM_TYPE_SAMPLE = 1,		//录音
}CHIRD_ALSA_STREAM_TYPE_E;

//采集频率
typedef enum{
	CHIRD_ALSA_RATE_8K = 8000,
	CHIRD_ALSA_RATE_16K = 16000,
	CHIRD_ALSA_RATE_44_1K = 44100,
	CHIRD_ALSA_RATE_48K = 48000,
}CHIRD_ALSA_RATE_E;

//通道数
typedef enum{
	CHIRD_ALSA_CHN_SINGLE = 1,		//单通道
	CHIRD_ALSA_CHN_STEREO = 2,		//立体声
}CHIRD_ALSA_CHN_E;

typedef struct chird_alsa_s
{
	int card;						//声卡编号 	ex:pcmC0D1p  card=0, device=1
	int device;						//设备号
	CHIRD_ALSA_STREAM_TYPE_E stream;
	CHIRD_ALSA_RATE_E rate;
	CHIRD_ALSA_CHN_E channel;
}CHIRD_ALSA_S;

/*
	alsa音频初始化	默认为16位小端数据
	返回值:   音频句柄
	dev	  :   设备名， 默认为default		plugin:0:0
	stream:   播放、录音  0为播放，1为录音
	rate  :	  采样频率， 会挑选相近且有效的频率
	channel:  音频通道
*/
void * chird_alsa_init(const CHIRD_ALSA_S *pstalsa);

int chird_alsa_uninit(void * handle);

int chird_alsa_sample(void * handle, void * buffer, unsigned long length);

int chird_alsa_playback(void * handle, void * buffer, unsigned long length);


#endif
