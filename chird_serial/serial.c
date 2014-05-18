#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/
#include     <unistd.h>     /*Unix标准函数定义*/
#include     <sys/types.h> /**/
#include     <sys/stat.h>   /**/
#include     <fcntl.h>      /*文件控制定义*/
#include     <termio.h>    /*PPSIX终端控制定义*/
#include     <errno.h>      /*错误号定义*/
#include     <string.h>
#include     "serial.h"

int chird_serial_open(const char * Dev)
{
	int fd = open(Dev, O_RDWR);        
	if (-1 == fd){ 						 /*设置数据位数*/
		perror("Can't Open Serial Port");
		return -1;
	}

	return fd;
}



void chird_serial_setspeed(int fd, int speed)
{
	int   i;
	int   status;
	struct termios   Opt;
    int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,
     B38400, B19200, B9600, B4800, B2400, B1200, B300, };
    int name_arr[] = {38400, 19200, 9600, 4800, 2400, 1200, 300,
     38400, 19200, 9600, 4800, 2400, 1200, 300, };

	tcgetattr(fd, &Opt);
	for ( i= 0; i < sizeof(speed_arr) / sizeof(int); i++){
	   if (speed == name_arr[i])
	   {
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
            Opt.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHOK|ECHONL|NOFLSH);
			status = tcsetattr(fd, TCSANOW, &Opt);
			if (status != 0)
				perror("tcsetattr fd1");
		 return;
		}
       
   tcflush(fd,TCIOFLUSH);
   }
}

int chird_serial_setParity(int fd,int databits,int stopbits,int parity)
{
	struct termios options;
	if ( tcgetattr( fd,&options) != 0)					/*tcgetattr函数用于获取与终端相关的参数。参数fd为终端的文件描述符，
														返回的结果保存在termios结构体中,成功反回0,失败反回-1*/
	{
		perror("SetupSerial 1");
		return(0);
	}

	options.c_cflag &= ~CSIZE;
	switch (databits) /*设置数据位数*/
	{
		case 7:
			options.c_cflag |= CS7;
			break;
		case 8:
			options.c_cflag |= CS8;
			break;
		default:
			fprintf(stderr,"Unsupported data size\n");	
		return(0);/*return (FALSE);*/
	}
	switch (parity)
	{
		case 'n':
		case 'N':
			options.c_cflag &= ~PARENB;   /* Clear parity enable */
			options.c_iflag &= ~INPCK;     /* Enable parity checking */
			break;
		case 'o':
		case 'O':
			options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/ 
			options.c_iflag |= INPCK;             /* Disnable parity checking */
			break;
		case 'e':
		case 'E':
			options.c_cflag |= PARENB;     /* Enable parity */
			options.c_cflag &= ~PARODD;   /* 转换为偶效验*/ 
			options.c_iflag |= INPCK;       /* Disnable parity checking */
			break;
		case 'S':
		case 's': /*as no parity*/
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
		break;
		default:
			fprintf(stderr,"Unsupported parity\n");
		return(0);/*return (FALSE);*/
    }
/* 设置停止位*/   
	switch (stopbits)
	{
		case 1:
		   options.c_cflag &= ~CSTOPB;
		   break;
		case 2:
		   options.c_cflag |= CSTOPB;
		   break;
		default:
		   fprintf(stderr,"Unsupported stop bits\n");
		   return(0);/*return (FALSE);*/
	}
/* Set input parity option */
	if (parity != 'n')
		options.c_iflag |= INPCK;
    options.c_cc[VTIME] = 150; // 15 seconds
    options.c_cc[VMIN] = 0;

	tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
	if (tcsetattr(fd,TCSANOW,&options) != 0)
	{
	   perror("SetupSerial 3");
	   return (0);/*return (FALSE);*/
	}
	return(1)/*return (TRUE)*/;
}
//////////////////////////////////////////////////////////////////////////////
int chird_serial_open(char *dev, int speed){

    int fd;

    fd = OpenDev(dev);				/*openDev打开com1的设备文件,打开反回1或没有打开返回0*/

    if (fd>0)						/*如果打开com1*/
		set_speed(fd,speed);		/*设置波特率为19200*/
	else							/*否则输出错误信息"没有打开串口"*/
	{
	   printf("Can't Open Serial Port!\n");
	   exit(0);						/*退出程序*/
	}
    
	if (set_Parity(fd,8,1,'N')== 0)	/*设置打开的文件fd,数据位8位,停止位1位,奇偶校验位,取值为N,E,O,S*/
	{
		printf("Set Parity Error\n");
		exit(1);
	}

	chird_serial_setparam(fd, &speed, 8, 1, 'N');

    return fd;

}     

int chird_serial_setparam(int fd, const int * speed, 
									const int * databits, 
									const int * stopbits, 
									const int * parity)
{
	if(NULL != speed){
		if(chird_serial_setspeed(fd, speed) < 0)
			return -1;
	}

	if(NULL != databits){
	}

	if(NULL != stopbits){
	}

	if(NULL != parity){
		
	}
	
}

int serial_read(int fd, char *buf){

    int cnt;
	memset(buf, 0, 2);	
    cnt = read(fd, buf, 1);
   // printf("read(%d):%c\n", cnt, *buf);
	
    return cnt;
}

int serial_write(int fd, char *buf){

    int cnt;

    cnt = write(fd, buf, 1);
  //  printf("write(%d):%c\n", cnt,*buf);
    
    return cnt;
}

int serial_close(int fd){
    
    close(fd);
}


