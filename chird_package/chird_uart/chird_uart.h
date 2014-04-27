/*-----------------------------------------------------------------*
 * Copyright (C),		Hangzhou chird Technology Ltd
 * FileName:            chird_uart.h
 * Author:             	meifj
 * Version:             
 * Datetime:            
 * Description:         create by meifj 2014-03-23  串口协议的封装
 * Version information: 
 *------------------------------------------------------------*/
#ifndef __CHIRD_UART_H
#define __CHIRD_UART_H

#define FIRST_CHAR	0xA5	// 协议头1
#define SECOND_CHAR	0x5A	// 协议头2
#define LAST_CHAR	0xAA	// 协议头3

/* 在串口接收中断中调用chird_uart_handler
	若完整接收到一桢数据那么frame_finish 置1
*/

//#define chird_uart_send 

#define CHIRD_UART_SLIDE_LEN 	0x05
#define CHIRD_UART_SLIDE1  		0xF1		// 滑动条
#define CHIRD_UART_SLIDE2  		0xF2
#define CHIRD_UART_SLIDE3  		0xF3

#define CHIRD_UART_KEY_LEN		0x04
#define CHIRD_UART_KEY1 		0XB1		// 按钮
#define CHIRD_UART_KEY2 		0XB2
#define CHIRD_UART_KEY3 		0XB3
#define CHIRD_UART_KEY4 		0XB4
#define CHIRD_UART_KEY5 		0XB5
#define CHIRD_UART_KEY6 		0XB6
#define CHIRD_UART_KEY7 		0XB7
#define CHIRD_UART_KEY8 		0XB8
#define CHIRD_UART_KEY9 		0XB9
#define CHIRD_UART_KEY10 		0XBA
#define CHIRD_UART_KEY11 		0XBB
#define CHIRD_UART_KEY12 		0XBC

#define CHIRD_UART_LED_LEN    	0x05		// LED灯
#define CHIRD_UART_LED1  		0xE1
#define CHIRD_UART_LED2  		0xE2
#define CHIRD_UART_LED3  		0xE3
#define CHIRD_UART_LED4  		0xE4
#define CHIRD_UART_LED5  		0xE5
#define CHIRD_UART_LED6  		0xE6
#define CHIRD_UART_LED7  		0xE7
#define CHIRD_UART_LED8  		0xE8

#define CHIRD_UART_PARAM_LEN	0x06		// 参数
#define CHIRD_UART_PARAM1 		0xC1
#define CHIRD_UART_PARAM2 		0xC2
#define CHIRD_UART_PARAM3 		0xC3
#define CHIRD_UART_PARAM4 		0xC4
#define CHIRD_UART_PARAM5 		0xC5
#define CHIRD_UART_PARAM6 		0xC6


extern unsigned char framedata[10];
extern unsigned char frame_finish;

void chird_uart_handler(unsigned char ch);

//滑动条数据发送
void chird_uart_send_slide(unsigned char idx, unsigned char param);

// 按键发送 
void chird_uart_send_key(unsigned char idx);

//LED数据/状态  idx 索引  sw 1:亮, 0灭
void chird_uart_send_led(unsigned char idx, unsigned char sw);

//参数发送  高8位 param1, 低八位param2
void chird_uart_send_param(unsigned char idx, unsigned char param1, unsigned char param2);

//数据处理
int chird_uart_Process();

#endif

