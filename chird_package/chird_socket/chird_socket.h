#ifndef _CHIRD_SOCKET_H_
#define _CHIRD_SOCKET_H_

#ifdef SOLARIS
#include <sys/sockio.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <net/if_arp.h>


#define SA struct sockaddr

#define	 NETCARD_TYPE "eth0"		//网卡类型: eth0 本地网卡，wlan1 无线网卡


/* 	获取本地 IP 地址
	ethname	: 指定网卡名称，本地网卡:eth0  无线网卡: wlan1
	flag	: 0 获取IP地址     1 获取广播地址
	padr	: 地址结构体，存储获取的ip地址 */
int chird_IP_Get(const char *ethname, char flag, struct in_addr *padr);

/* 二进制IP转点分十进制 */
char * chird_ntos(const struct in_addr * addr);		// inet_ntop	
/* 点分十进制 转 二进制IP*/
struct in_addr chird_ston(const char * ip);	// inet_pton

/*============================================================
*							TCP
*============================================================*/

#if 0
typedef struct {
	
}CHIRD_TCP_INFO_S;
#endif

/*客户端连接函数*/
int chird_tcp_connect(const char * ip , int port) ;

/*服务端监听函数*/
int chird_tcp_listen(int port/*, void (*paccept)(void * param), void * param*/);

/*服务端等待连接函数*/
int chird_tcp_accept(int sockfd, struct sockaddr_in * addr);

/*接收tcp数据函数 timeout 为0时不超时*/
int chird_tcp_recv(int fd, char * buf, int size, int timeout);

/*发送tcp数据函数*/
int chird_tcp_send(int fd, const char * buf, int size);


/*============================================================
*							UDP
*============================================================*/

/* 创建udp套接字		port 端口号，
*  nbcast : 1 创建广播套接字  0 : 创建普通套接字 */
int chird_udp_create(int port, int nbcast);

/*接收udp数据*/
int chird_udp_recv(int fd, char *buf, int size, struct sockaddr_in *addr, int timeout);

/*发送udp数据 timeout 为0时不超时*/
int chird_udp_send(int fd, char *buf, int buf_size, const struct sockaddr_in *addr);

/* 指定IP和端口号发送 */
int chird_udp_send_byip(int fd, char *buf, int size, const char  * ip, int port);

#endif
