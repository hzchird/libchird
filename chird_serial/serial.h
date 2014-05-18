#ifndef _SERIAL_H
#define _SERIAL_H

int serial_init(char *dev, int speed);
    
int serial_read(int fd, char *buf);

int serial_write(int fd, char *buf);

int serial_close(int fd);

#endif
