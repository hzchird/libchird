#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#include "serial.h"

int fd;


void *pthread_proc(void *arg)
{
	char buf = 'a';
	//int fd = *(int *)arg;
	int i =0;
	while(1){
		
		serial_write(3, &buf);
		printf("Dev(%d) write(%d) is %c\n", fd, i++, buf);
		sleep(1);
	}

}

int main(int argc, char **argv){

    
    char buf = 0;
	int i = 0;
    int speed = 9600;

    fd = serial_init(argv[1], speed);
    printf("111fd=%d\n", fd);


    pthread_t tid;	
    if(pthread_create(&tid, NULL, pthread_proc, NULL) < 0){  // 创建线程1
		perror("failed");
		return -1;
    }


    while(1){
		
    	serial_read(fd, &buf);
		printf("read(%d) is %c\n", i++, buf);
		sleep(1);

	}
	
    serial_close(fd);
	
}
