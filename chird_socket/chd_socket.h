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
//#include <linux/route.h>

#define MAX_NET_CARDS		10
#define SA struct sockaddr

#define IP_ADDR		 0
#define BROADCAST_IP 1
#define NETMASK_IP   2


int chird_net_travelcard();

/* 将网卡名写入buf中，以:隔开，如lo:eth0:wlan0 第一个肯定是lo*/
int chird_net_getcard2buff(void * buf, int * cardnum);


int chird_net_setIP(const char * netcard, char flag, const char * addr, int enable);

/* 	获取本地 IP 地址
	ethname	: 指定网卡名称，本地网卡:eth0  无线网卡: wlan1
	flag	: 0 获取IP地址     1 获取广播地址
	padr	: 地址结构体，存储获取的ip地址 */
int chird_net_getIP(const char * netcard, char flag, struct in_addr * padr);

/* 二进制IP转点分十进制 */
char * chird_net_ntos(const struct in_addr * addr);		// inet_ntop	
/* 点分十进制 转 二进制IP*/
struct in_addr chird_net_ston(const char * ip);	// inet_pton

/*============================================================
*							TCP
*============================================================*/

/*客户端连接函数*/
int chird_tcp_connect(const char * ip, int port, int timeout);

/*服务端监听函数*/
int chird_tcp_listen(int port/*, void (*paccept)(void * param), void * param*/);

/*服务端等待连接函数*/
int chird_tcp_accept(int sockfd, struct sockaddr_in * addr);

/*接收tcp数据函数 timeout 为0时不超时*/
int chird_tcp_recv(int fd, void * buf, int size, int timeout);

/*发送tcp数据函数*/
int chird_tcp_send(int fd, const void * buf, int size);


/*============================================================
*							UDP
*============================================================*/

/* 创建udp套接字		port 端口号，
*  nbcast : 1 创建广播套接字  0 : 创建普通套接字 
   nettype: 网络类型 如eth0 wlan0等
*/
int chird_udp_create(int port, int nbcast, const char * nettype);

/*接收udp数据*/
int chird_udp_recv(int fd,  void * buf, int size, struct sockaddr_in *addr, int timeout);

/*发送udp数据 timeout 为0时不超时*/
int chird_udp_send(int fd,  void * buf, int buf_size, const struct sockaddr_in *addr);

/* 指定IP和端口号发送 */
int chird_udp_send_byips(int fd,  void * buf, int size, const char  * ip, int port);

int chird_udp_send_byip(int fd,  void * buf, int size, struct in_addr inaddr, int port);

#endif
