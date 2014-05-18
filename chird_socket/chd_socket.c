
#include "chd_socket.h"

#define  LISTEN_QUE		10

int chird_net_travelcard()
{
	int i;
	int if_cnt;
	struct ifreq if_buf[MAX_NET_CARDS]; 
	struct ifconf ifc;
	int fd;

    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){   
         printf("socket create failed...Get Local Ip!/n");   
         return -1;   
    }   
		
    ifc.ifc_len = sizeof(if_buf);
    ifc.ifc_buf = (char *)if_buf;

	if(!ioctl(fd, SIOCGIFCONF, (char *)&ifc)){
		if_cnt = ifc.ifc_len / sizeof(struct ifreq);
		for(i = 0; i < if_cnt; i++){
			printf("%s\n", if_buf[i].ifr_name);
		}
	}	

	close(fd);
	
	return 0;
}

int chird_net_getcard2buff(void * buf, int * cardnum)
{
	int i;
	int if_cnt;
	struct ifreq if_buf[MAX_NET_CARDS]; 
	struct ifconf ifc;
	int fd;

	if(NULL == buf || cardnum == NULL)
		return -1;

    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1){   
         printf("socket create failed...Get Local Ip!/n");   
         return -1;   
    }   

    ifc.ifc_len = sizeof(if_buf);
    ifc.ifc_buf = (char *)if_buf;

	* cardnum = 0;

	
	sprintf(buf, "lo");
	if(!ioctl(fd, SIOCGIFCONF, (char *)&ifc)){
		if_cnt = ifc.ifc_len / sizeof(struct ifreq);
		for(i = 0; i < if_cnt; i++){
	        if (strcmp("lo", if_buf[i].ifr_name) == 0)
                continue;
		
			strcat(buf, ":");
			strcat(buf, if_buf[i].ifr_name);

			*cardnum += 1;
		}
	}	

	close(fd);
	return 0;
}

/* 查看网卡是否在系统中 在返回0，不在返回-1 */
int chird_net_search(int fd, const char * netcard)
{
	int i;
	int if_cnt;
	struct ifreq if_buf[MAX_NET_CARDS]; 
	struct ifconf ifc;		

    ifc.ifc_len = sizeof(if_buf);
    ifc.ifc_buf = (char *)if_buf;

	if(!ioctl(fd, SIOCGIFCONF, (char *)&ifc)){
		if_cnt = ifc.ifc_len / sizeof(struct ifreq);
		for(i = 0; i < if_cnt; i++){
			if(strcmp(netcard, if_buf[i].ifr_name) == 0){
				return 0;
			}
		}
	}

	printf("ERROR : No such netcard\n");
	
	return -1;
	
}

#if 0

int chird_net_setgateway(const char * netcard,  const char * gwip)
{
	struct rtentry rm;
	struct sockaddr_in sin;
	int fd;
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1){
		perror("Not create network socket connection \n");
		return -1;
	}
	
  	memset(&sin,   0,   sizeof(sin));
    sin.sin_family   	  = AF_INET;
    sin.sin_addr.s_addr   = inet_addr(gwip);
    memcpy(&rm.rt_gateway, &sin, sizeof(sin));

    rm.rt_dev = (char *)netcard;
    rm.rt_flags = RTF_UP | RTF_GATEWAY ;
    if(ioctl(fd, SIOCADDRT, &rm ) < 0){
        perror("gateway ioctl error\n");
        return (-1);
    }

	close(fd);

	return 0;
}
#endif

/* flag: 
		0 获取IP地址 
		1 获取广播地址
		2 子网掩码
*/
int chird_net_setIP(const char * netcard, char flag, const char * addr, int enable)
{
	struct sockaddr_in sin;
	struct ifreq ifr;
	int fd;
	int type;
	int ret;

	bzero(&ifr, sizeof(struct ifreq)); 
	  
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1){
		perror("Not create network socket connection\n");
		return -1;
	}

	strncpy(ifr.ifr_name, netcard, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = 0;
	
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(addr);
	memcpy(&ifr.ifr_addr, &sin, sizeof(sin));

	switch(flag){
	default:
	case 0: type = SIOCSIFADDR; 	break;
	case 1: type = SIOCSIFBRDADDR; 	break;
	case 2: type = SIOCSIFNETMASK;  break;
	}

	if (ioctl(fd, type, &ifr) < 0){
		perror("Not setup interface ");
		return -1;
	}

	if(enable){
		ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
	}

	if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0){
		perror("SIOCSIFFLAGS");
		return -1;
	} 
	
	return 0; 

}



/* flag: 
		0 获取IP地址 
		1 获取广播地址
		2 子网掩码
*/
int chird_net_getIP(const char * netcard, char flag, struct in_addr * padr)
{   
    int fd;    
    struct sockaddr_in * sin;   
    struct ifreq ifr_ip;
	int type = 0;
	
    if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){   
         printf("socket create failed...Get Local Ip!\n");   
         return -1;   
    }   

	/* 找到相应的网卡 */
	if(chird_net_search(fd, netcard) == 0){

		memset(&ifr_ip, 0, sizeof(struct ifreq));
	    strncpy(ifr_ip.ifr_name, netcard, IFNAMSIZ);  
		ifr_ip.ifr_name[IFNAMSIZ - 1] = 0;

		switch(flag){
		default:
		case 0: type = SIOCGIFADDR; 	break;
		case 1: type = SIOCGIFBRDADDR; 	break;
		case 2: type = SIOCGIFNETMASK;  break;
		}

	    if(ioctl(fd, type, &ifr_ip) < 0 ){    
			close(fd);
	        return -1;      
	    }        
		
	    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;      
		(*padr).s_addr = sin->sin_addr.s_addr;

	  	close(fd);

		return 0;

	}
	else{
		return -1;
	}

    return 0;  

}  


/* 二进制IP转点分十进制，网络地址转换成字符串 */
char * chird_net_ntos(const struct in_addr * addr)		
{
	static char ip[20];
	if(NULL == inet_ntop(AF_INET, (void*)addr, ip, 20)){
		perror("chird_ntos");
		return NULL;
	}
	
	return ip;
}

/*点分十进制转二进制， 把字符串	转换成符合Ipv4或Ipv6的网络地址*/
struct in_addr chird_net_ston(const char * ip)
{
	struct in_addr addr = {0};
	
	if(inet_pton(AF_INET, ip, (void*)&addr) < 0){
		perror("chird_ston");
		return addr;
	}
	
	return addr;
}


/* ===============================================================================
*					TCP				
* =============================================================================== */

int chird_tcp_connect(const char * ip, int port, int timeout) 
{
	int fd, ret = 1, error=-1;
	socklen_t len = sizeof(struct sockaddr_in);
	struct sockaddr_in serv = {0};
	struct timeval tm;
	fd_set set;
	unsigned long ul = 1;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(fd < 0) {
		perror("socket");
		return -1;
	}

	if(timeout == 0){
		serv.sin_family = AF_INET;
		serv.sin_addr = chird_net_ston(ip);
		serv.sin_port = htons(port);
		if(connect(fd, (SA*)&serv, len) == -1) {	
			close(fd);
			return -1;
		}
	}
	else{
		ioctl(fd, FIONBIO, &ul); 
		serv.sin_family = AF_INET;
		serv.sin_addr = chird_net_ston(ip);
		serv.sin_port = htons(port);
		if(connect(fd, (SA*)&serv, len) == -1) {

			tm.tv_sec = timeout;
			tm.tv_usec = 0;
			FD_ZERO(&set);
			FD_SET(fd, &set);

			if(select(fd+1, NULL, &set, NULL, &tm) > 0){
				getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);

				ret = (error == 0) ? 1 : 0;
			} 
			else ret = 0;
		}
		
		ul = 0;
		ioctl(fd, FIONBIO, &ul); //设置为阻塞模式
		
		if(!ret){
			close(fd);
			return -1;
		}
	}
	
	return fd;
	
}

int chird_tcp_listen(int port)
{
	const int on = 1;
	struct sockaddr_in staddr;
	int sockfd;
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		perror("socket");
		return -1;
	}
	/* 端口重入 */
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0){
		close(sockfd);
		perror("setsockopt");
		return -1;
	}

	/* 绑定本地IP及端口 */
	bzero(&staddr, sizeof(staddr));
	staddr.sin_family = AF_INET;
	staddr.sin_port = htons(port);		
	staddr.sin_addr.s_addr = INADDR_ANY;
	if(bind(sockfd, (SA*)&staddr, sizeof(SA)) < 0){
		close(sockfd);
		perror("bind");
		return -1;
	}

	/* 监听端口 */
	if(listen(sockfd, LISTEN_QUE) < 0){
		close(sockfd);
		perror("listen");
		return -1;
	}
	
	return sockfd;

}


int chird_tcp_accept(int sockfd, struct sockaddr_in * addr)
{
	int fd;
	unsigned int len = sizeof(struct sockaddr_in);
	struct sockaddr_in staddr;
	
	if(NULL == addr){
		fd = accept(sockfd, (SA*)&staddr, &len);
	}
	else{
		fd = accept(sockfd, (SA*)addr, &len);
	}
	if(fd < 0){
		perror("accept");
		return -1;
	}

	return fd;
	
}

int chird_tcp_recv(int fd, void * buf, int size, int timeout)
{
	int rn, remain = size;
	struct timeval tv;
	fd_set readfds;
	int ret;
	
	if(timeout == 0){
		while(remain != 0){
			if((rn = recv(fd, buf + size - remain, remain , MSG_WAITALL)) <= 0){
				return rn;
			}

			remain -= rn;
		}	
	}
	else{
		tv.tv_sec  = timeout;
		tv.tv_usec = 0;
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		ret = select(fd + 1, &readfds, NULL, NULL, &tv);
		if(ret > 0){
			while(remain != 0){
				if((rn = recv(fd, buf + size - remain, remain , MSG_WAITALL)) <= 0){
					return rn;
				}

				remain -= rn;
			}
		}
		else if(ret == 0){
			printf("chird_tcp_recv : time out...\n");
			return 0;
		}
		else{
			printf("chird_tcp_recv : select failed\n");
			return -1;
		}
	}
	
	return size;
}

int chird_tcp_send(int fd, const void * buf, int size)
{
	return send(fd, buf, size, 0);
}

/* ===============================================================================
*					UDP				
* =============================================================================== */


/* 创建udp套接字
*  port 端口号 0不绑定  >0绑定
*  nbcast : 1 创建广播套接字  0 : 创建普通套接字 */
int chird_udp_create(int port, int nbcast, const char * nettype)
{
	const int on = 1;

	struct sockaddr_in staddr;
	int sockfd;

//	if(nbcast == 1 && port == 0){
//		printf(" when nbcaste set 1, port must assign!!\n");
//		return -1;
//	}
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0) {
		perror("socket");
		return -1;
	}

	/* 端口重入 */
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0){
		close(sockfd);
		perror("setsockopt");
		return -1;
	}

	if(nbcast){
		if(NULL != nettype){
			if(chird_net_getIP(nettype, 1, &staddr.sin_addr) < 0){
				close(sockfd);
				return -1;
			}
		}
		else{
			if(chird_net_getIP("eth0",  1, &staddr.sin_addr) < 0){
				close(sockfd);
				return -1;
			}
		}
	}
	else{
		staddr.sin_addr.s_addr = htonl(INADDR_ANY);
	}

	if(port){
		/* 绑定本地IP及端口 */
		bzero(&staddr, sizeof(staddr));
		staddr.sin_family = AF_INET;
		staddr.sin_port = htons(port);
		
		if(bind(sockfd, (SA*)&staddr, sizeof(SA)) < 0){
			close(sockfd);
			perror("bind");
			return -1;
		}
	}
	
	if(nbcast){
		/* 设置套接字属性: 广播*/
		if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0){
			perror("setsockopt");
			return -1;
		}
	}
	
	return sockfd;
}


int chird_udp_recv(int fd, void * buf, int size, struct sockaddr_in * addr, int timeout)
{
	struct sockaddr_in staddr;
	socklen_t len = sizeof(struct sockaddr_in);
	int ret;
	
	struct timeval tv;
	fd_set readfds;

	if(timeout == 0){
		if(NULL == addr){
			return recvfrom(fd, (void *)buf, size, 0, (SA*)&staddr, &len);
		}
		else{
			return recvfrom(fd, (void *)buf, size, 0, (SA*)addr, &len);
		}	
	}
	else{
		tv.tv_sec  = timeout;
		tv.tv_usec = 0;
		FD_ZERO(&readfds);
		FD_SET(fd, &readfds);
		ret = select(fd + 1, &readfds, NULL, NULL, &tv);
		if( ret > 0){
			if(NULL == addr){
				return recvfrom(fd, (void *)buf, size, 0, (SA*)&staddr, &len);
			}
			else{
				return recvfrom(fd, (void *)buf, size, 0, (SA*)addr, &len);
			}	
		}
		else if(ret == 0){
			printf("chird_udp_recv : time out...\n");
			return 0;
		}
		else{
			printf("chird_udp_recv : select failed\n");
			return -1;
		}
	}

	return 0;
}


int chird_udp_send(int fd,  void * buf, int size, const struct sockaddr_in * addr)
{
	return sendto(fd, (void *)buf, size, 0, (SA*)addr, sizeof(struct sockaddr_in));
}

int chird_udp_send_byip(int fd,  void * buf, int size, struct in_addr inaddr, int port)
{
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = inaddr;
	
	return sendto(fd, (void *)buf, size, 0, (SA*)&addr, sizeof(struct sockaddr_in));
}

int chird_udp_send_byips(int fd,  void * buf, int size, const char  * ip, int port)
{
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr = chird_net_ston(ip);
	
	return sendto(fd, (void *)buf, size, 0, (SA*)&addr, sizeof(struct sockaddr_in));
}



#if 0
int Get_ControlParem(int argc, char **argv)
{
	int ch;
	int flag;
	
	while((ch = getopt(argc, argv, "sr")) != -1){
		switch(ch){
		case 's':
			flag = 1;	break;	
		case 'r':
			flag = 2;	break;
		default:
			printf("other option : %c\n", ch);
		}
	}	

	return flag;
}

int main(int argc, char **argv)
{
	char *re_ch;
	struct sockaddr_in addr = {0};
	int fd;
	char buf[20];
	char send_s[20]="1234567890123456789";
	int len, rn;
	int flag = Get_ControlParem(argc, argv);
#if 0
	if(1 == flag){
		fd = chird_udp_create(17000, 1);
	//	addr.sin_family = AF_INET;
	//	addr.sin_port = htons(17000);
	//	addr.sin_addr.s_addr = chird_ston("192.168.1.255");
		while(1){
		//	chird_udp_send(fd, send_s, 20, &addr);
			chird_udp_send1(fd, send_s, 20, "192.168.1.255", 17000);
			printf("send : %s\n", send_s);
			sleep(1);
		}
	}else if(2 == flag){
		fd = chird_udp_create(17000, 0);
		while(1){
			chird_udp_recv(fd, buf, 20, &addr, 0);
			char *ch;
			ch = chird_ntos((struct in_addr *)&addr.sin_addr);
			printf("recvfrom:[%s] %s\n",ch, buf);
		}

	}
#else
	if(1 == flag){
		int listen_fd;
		int date_fd;
		listen_fd = chird_tcp_listen(17001);

		printf("socket server start...\n");
		if((date_fd = chird_tcp_accept(listen_fd, &addr)) < 0){
			printf("accept is failed");
			return -1;
		}
		
		char * ch = chird_ntos((struct in_addr *)&addr.sin_addr);
		
		while(1){
			rn = chird_tcp_recv(date_fd, buf, 20, 1);	
			if(rn <= 0){
				printf("remote lost..\n");
				break;
			}
				
			printf("recv from [%s]: %s (%d)\n", ch, buf, rn);
		}
		
	}
	else if(2 == flag){

		if((fd = chird_tcp_connect("192.168.1.201", 17001)) < 0 ){
			printf("connect is failed");
			return -1;
		}
		
		while(1){
			fgets(buf, 20, stdin);
			rn = chird_tcp_send(fd, buf, 20);
			if(rn <= 0 ){
				printf("send server lost..\n");
				break;
			}
			printf("send size:%d %s\n", strlen(buf), buf);	
		}
	}
	close(fd);
#endif
	return 0;
}
#endif

