#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include "serial.h"

int put_char(int fd, unsigned char c)
{
        int ret = 0;

        ret = write(fd, &c, 1);
        if (ret < 0)
                perror("write() in put_char()");

        return ret;
}

char get_char(int fd)
{
        int len = 0;
        char c;

        len = read(fd, &c, 1);
        if (len < 0)
                return 0;

        return c;
}
int set_uart(int fd)
{
    struct termios Opt;
    tcgetattr(fd,&Opt);   //获取串口设备的相关属性
    //tcgetattr(fd1,&options);   //获取串口设备的相关属性
    cfsetispeed(&Opt,B9600); //设置串口的输入波特率为115200
   // cfsetospeed(&Opt,B115200); //设置串口的输出波特率为115200
    
    Opt.c_cflag &= ~PARENB;    // 校验位 这个操作是清除它的校验位
    Opt.c_cflag &= ~CSTOPB;        // 设置停止位,  这个停止位是1位
    Opt.c_cflag &= ~CSIZE;  // 清除数据位 , 
    Opt.c_cflag |= CS8;    // 设置数据位 为8位 
     // 以上是设置uart为8N1
    Opt.c_lflag &= (~ICANON);   // 关闭标准模式
    Opt.c_lflag &= (~ECHO);
    Opt.c_oflag &= ~(ONLCR | OCRNL | ONOCR | ONLRET);
    tcsetattr(fd,TCSANOW,&Opt); //设置串口设备的相关属性
    //tcsetattr(fd1,TCSANOW,&options);
    return 0;
}


int set_serial(int fd)
{
        struct termios opt;

        tcgetattr(fd, &opt);
        tcflush(fd, TCIOFLUSH);

        cfsetispeed(&opt, B115200);
        cfsetospeed(&opt, B115200);

        opt.c_cflag |= CS8;
        opt.c_cflag &= ~PARENB;
        opt.c_iflag &= ~INPCK;
        opt.c_cflag &= ~CSTOPB;

        if (tcsetattr(fd, TCSANOW, &opt) == -1) {
                perror("tcsetattr()");
                return -1;
        }

        return 0;
}

void close_serial(int fd)
{
        close(fd);
}

unsigned int get_file_size(const char *path)
{
        unsigned long filesize = -1;
        struct stat statbuff;
        if(stat(path, &statbuff) < 0){
                return filesize;
        }else{
                filesize = statbuff.st_size;
        }
#if 0
        FILE *fp;
        fp = fopen(path, "r");
        if (fp == NULL) 
                return filesize;
        fseek(fp, 0L, SEEK_END);
        filesize = ftell(fp);
        fclose(fp);
#endif
        return filesize;
}

