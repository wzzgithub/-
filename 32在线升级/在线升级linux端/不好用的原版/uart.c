#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
//#include "gpio.h"
#include <sys/ioctl.h>
int main(void)
{
    int fd,fd1;
    int nread,nwrite,q;
    char ret,re;
    int value=0x01,led=0;
    struct termios options;
    fd = open("/dev/ttyS2",O_RDWR); //打开串口设备文件
    if(fd<0)
        perror("error");
	fd1 = open("/dev/ttyS4",O_RDWR);
	 if(fd1<0)
        perror("error");
    tcgetattr(fd,&options);   //获取串口设备的相关属性
    tcgetattr(fd1,&options);   //获取串口设备的相关属性
    cfsetispeed(&options,B115200); //设置串口的输入波特率为115200
    cfsetospeed(&options,B115200); //设置串口的输出波特率为115200
    options.c_cflag &= ~PARENB;    // 校验位 这个操作是清除它的校验位
    options.c_cflag &= ~CSTOPB;        // 设置停止位,  这个停止位是1位
    options.c_cflag &= ~CSIZE;  // 清除数据位 , 
    options.c_cflag |= CS8;    // 设置数据位 为8位 
    // 以上是设置uart为8N1
    options.c_lflag &= (~ICANON);   // 关闭标准模式
    options.c_lflag &= (~ECHO);
    tcsetattr(fd,TCSANOW,&options); //设置串口设备的相关属性
	tcsetattr(fd1,TCSANOW,&options);
    while(1)
    {//
        led = value;
        nread = read(fd1,&ret,sizeof(ret));
        //从串口设备文件中读数据，并将该数据存入ret内
        if(nread<0)
            perror("read error");
	//	printf("1..:%d\n",sizeof(ret));
		//ret = 'c';
        nwrite = write(fd,&ret,sizeof(ret)); //向GPIO设备文件中写数据
        if(nwrite<0)
            printf("error");
       
		printf("1..:%c\n",ret);
       

    }
	   return 0;
}
