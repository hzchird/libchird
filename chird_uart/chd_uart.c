/*-----------------------------------------------------------------*
 * Copyright (C),		Hangzhou chird Technology Ltd
 * FileName:            chird_uart.c
 * Author:             	meifj
 * Version:             
 * Datetime:            
 * Description:         create by meifj 2014-03-23	串口协议的封装
 * Version information: 
 *------------------------------------------------------------*/

#include "chd_uart.h"

//桢格式 : HEAD1 HEAD2 LENGTH TYPE PARAM CHECK LAST
//接收到完整的一桢后frame中存放的是TYPE PARAM CHECK LAST

static unsigned char send_frame[10] = {FIRST_CHAR, SECOND_CHAR};
unsigned char framedata[10] = {0};
unsigned char frame_finish = 0;

void uart_comm_handler(int8 ch)
{
	static int8 cnt = 0;
	static int8 start = 0;
	static int8 len;

	if(frame_finish == 0 && start == 0){
		if(ch == FIRST_CHAR) cnt = 1;		//第一个字符为FIRST_CHAR
		else if(ch == SECOND_CHAR && cnt == 1) cnt = 2;	  //第二个字符为SECOND_CHAR
		else if(cnt == 2){
			start = 1;		// 开接接收数据
			len = ch-1;		// 赋值长度，减去自己的长度
			cnt = 0;		// 计数置0
		}
		else cnt = 0;		// 任何错误的接收，都将cnt置0
		
	}
	else if(start == 1){
		
		frame[cnt++] = ch;
		// 接收到指定长度字符
		if(cnt == len){
			if(frame[cnt - 1] == LAST_CHAR){
			//	uart_putchar(UART4, 0x88);
				frame_finish = 1; 
			}
			len = 0;
			start = 0;
			cnt = 0;
		}
	}
	else{
		start = 0;
	}
		
}

void chird_uart_send_key(unsigned char idx)
{
	send_frame[2] = CHIRD_UART_KEY_LEN;
	send_frame[3] = idx + CHIRD_UART_KEY1; 
	send_frame[4] = send_frame[2] + send_frame[3];
	send_frame[5] = LAST_CHAR;

	chird_uart_send(send_frame, CHIRD_UART_KEY_LEN + 2);	
}

void chird_uart_send_slide(unsigned char idx, unsigned char param)
{
	send_frame[2] = CHIRD_UART_SLIDE_LEN;
	send_frame[3] = idx + CHIRD_UART_SLIDE1; 
	send_frame[4] = param;
	send_frame[5] = send_frame[2] + send_frame[3] + send_frame[4];
	send_frame[6] = LAST_CHAR;

	chird_uart_send(send_frame, CHIRD_UART_SLIDE_LEN + 2);	
}

void chird_uart_send_led(unsigned char idx, unsigned char sw)
{
	send_frame[2] = CHIRD_UART_LED_LEN;
	send_frame[3] = idx + CHIRD_UART_LED1; 
	send_frame[4] = sw;
	send_frame[5] = send_frame[2] + send_frame[3] + send_frame[4];
	send_frame[6] = LAST_CHAR;

	chird_uart_send(send_frame, CHIRD_UART_LED_LEN + 2);
}

void chird_uart_send_param(unsigned char idx, unsigned char param1, unsigned char param2)
{

	send_frame[2] = CHIRD_UART_PARAM_LEN;
	send_frame[3] = idx + CHIRD_UART_PARAM1; 
	send_frame[4] = param1;
	send_frame[5] = param2;
	send_frame[6] = send_frame[2] + send_frame[3] + send_frame[4] + send_frame[5];
	send_frame[7] = LAST_CHAR;

	chird_uart_send(send_frame, CHIRD_UART_PARAM_LEN + 2);
}


int chird_uart_Process()
{
	/* ****** 带Linux操作系统的，可以在此接收信号量 ****** */


	/* ****** 带Linux操作系统的，可以在此接收信号量 ****** */
	
	if(frame_finish == 1){

		// 类型判断 
		switch(framedata[0]){
		//滚动条
		case CHIRD_UART_SLIDE1:
			break;
		case CHIRD_UART_SLIDE2:
			break;
		case CHIRD_UART_SLIDE3:
			break;

		//按键
		case CHIRD_UART_KEY1  :
			break;
		case CHIRD_UART_KEY2  :
			break;
		case CHIRD_UART_KEY3  :
			break;
		case CHIRD_UART_KEY4  :
			break;
		case CHIRD_UART_KEY5  :
			break;
		case CHIRD_UART_KEY6  :
			break;
		case CHIRD_UART_KEY7  :
			break;
		case CHIRD_UART_KEY8  :
			break;
		case CHIRD_UART_KEY9  :
			break;
		case CHIRD_UART_KEY10 :
			break;
		case CHIRD_UART_KEY11 :
			break;
		case CHIRD_UART_KEY12 :
			break;

		// LED灯、状态
		case CHIRD_UART_LED1  : 
			break;
		case CHIRD_UART_LED2  : 
			break;
		case CHIRD_UART_LED3  : 
			break;
		case CHIRD_UART_LED4  : 
			break;
		case CHIRD_UART_LED5  : 
			break;
		case CHIRD_UART_LED6  : 
			break;
		case CHIRD_UART_LED7  : 
			break;
		case CHIRD_UART_LED8  : 
			break;

		// 参数
		case CHIRD_UART_PARAM1: 
			break;
		case CHIRD_UART_PARAM2: 
			break;
		case CHIRD_UART_PARAM3: 
			break;
		case CHIRD_UART_PARAM4: 
			break;
		case CHIRD_UART_PARAM5: 
			break;
		case CHIRD_UART_PARAM6: 
			break;
		}
	}
	
	return 0;
}

void chird_uart_send(unsigned char * framedata, int len)
{
	/* ****** 往串口中写入数据 ****** */


	/* ****** 往串口中写入数据 ****** */
	
}

#if 0
int main()
{
	int i;
	unsigned char v = 0x5+CHIRD_UART_SLIDE1+255;
	unsigned char a[] = {FIRST_CHAR, SECOND_CHAR, 5, CHIRD_UART_SLIDE1, 255, v, LAST_CHAR};
	
	for(i = 0 ; i < 7 ; i++){
		chird_uart_handler(a[i]);
	}

	chird_uart_Process();

	chird_uart_send_key(1);
	
}
#endif
