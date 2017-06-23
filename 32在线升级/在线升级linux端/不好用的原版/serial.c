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
    tcgetattr(fd,&Opt);   //��ȡ�����豸���������
    //tcgetattr(fd1,&options);   //��ȡ�����豸���������
    cfsetispeed(&Opt,B9600); //���ô��ڵ����벨����Ϊ115200
   // cfsetospeed(&Opt,B115200); //���ô��ڵ����������Ϊ115200
    
    Opt.c_cflag &= ~PARENB;    // У��λ ����������������У��λ
    Opt.c_cflag &= ~CSTOPB;        // ����ֹͣλ,  ���ֹͣλ��1λ
    Opt.c_cflag &= ~CSIZE;  // �������λ , 
    Opt.c_cflag |= CS8;    // ��������λ Ϊ8λ 
     // ����������uartΪ8N1
    Opt.c_lflag &= (~ICANON);   // �رձ�׼ģʽ
    Opt.c_lflag &= (~ECHO);
    Opt.c_oflag &= ~(ONLCR | OCRNL | ONOCR | ONLRET);
    tcsetattr(fd,TCSANOW,&Opt); //���ô����豸���������
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

