#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <unistd.h>
#include <errno.h>

#include 	 "ymodem.h"

#define    LCD2    _IO('A',2)
#define    LCD3    _IO('A',3)
#define    LCD3_1  _IO('A',31)
#define    LCD3_2  _IO('A',32)
#define    LCD4    _IO('A',4)
#define    LCD4_1  _IO('A',41)
#define    LCD4_2  _IO('A',42)
#define    LCD4_3  _IO('A',43)
#define    LCD5    _IO('A',5)
#define    LCD5_1  _IO('A',51)
#define    LCD5_2  _IO('A',52)
#define    LCD6    _IO('A',6)
#define    LCD6_1  _IO('A',55)


#define    LCD71_11   _IO('A',9)//时间间隔
#define    LCD71_12   _IO('A',20)//时间间隔
#define    LCD72_11   _IO('A',10)//开始时间
#define    LCD_C_4    _IO('A',11)//退出设置
#define    LCD_CL_2    _IO('A',12)

#define  LCD41_31  _IO('A',13)//设置测量次数十位
#define  LCD41_32  _IO('A',14)//设置测量次数个位
#define  LCD41_33  _IO('A',28)//设置测量次数个位

#define  LCD41_21  _IO('A',15)//设置测量宽度十位
#define  LCD41_22  _IO('A',16)//设置测量宽度个位
#define  LCD41_23  _IO('A',27)//设置测量宽度个位


#define  LCD41_11       _IO('A',18)//设置周期
#define  LCD41_12     _IO('A',19)//设置周期
#define  LCD41_13     _IO('A',26)//设置周期

#define  LCD61_11       _IO('A',60)//设置周期
#define  LCD61_12     _IO('A',61)//设置周期
#define  LCD61_13     _IO('A',62)//设置周期
#define  LCD61_21       _IO('A',63)//设置周期
#define  LCD61_22     _IO('A',64)//设置周期
#define  LCD61_23     _IO('A',65)//设置周期
char * buf[4];
char buf_sj[4];//保存测量的数据

char * buf_s[4] = {"0"};//测量次数数据(屏幕端显示)
int buf_set[4];//设置数据 的处理 处理
int zhouqi_set[4]; //设置周期
int kuandu_set[4]; //设置宽度
int cishu_set[4]; //设置测量次数

int clcs_set[4]; //设置测量次数

int KEY;
int fd,fp;

char * reverse(char *s)
{
    char temp;
    char *p = s;//p指向s的头部
    char *q = s;//q指向s的尾部
    while(*q)
        ++q;
    q--;
    while(q > p)//交换移动指针，直到p和q交叉
    {
        temp = *p;
        *p++ = *q;
        *q-- = temp;
    }
    return s;
}

/*  
 *  功能：整数转换为字符串
 *  char s[]的作用是存储整数的每一位
 */
char *my_itoa(int n)
{
    int i = 0,isNegative = 0;
    static char s[100];//为static变量，或者是全局变量
    if((isNegative = n) < 0) //如果是负数，先赚为正数
    {
        n = -n;
    }
    do   //从各位开始变为字符，直到最高位，最后应该反转
    {
        s[i++] = n%10 + '0';
        n = n/10;
    }while(n > 0);

    if(isNegative < 0)   //如果是负数，补上负号
    {
		
        s[i++] = '-';
    }
    s[i]='\0';   //最后加上字符串结束符
    
   return reverse(s);
}

int key_4x4()
{
	
int ret = 0;
int state;
int cod;
int a;


struct input_event bufd;//按键值结构体
						ret = read(fp,&bufd,sizeof(struct input_event));
						if (ret <= 0)
						{
							printf("read failed!\n");
							return -1;
						}
						cod = bufd.code;
					
			
					
						switch(cod)
						{
							case KEY_1:  //按键1
								//if(key_state==1)
									KEY = 1;
								break;
							case KEY_4:
								//if(key_state==1)
									KEY = 2;
								break;

							case KEY_7:
								//if(key_state==1)
									KEY = 3;
								break;
							case KEY_2:
								//if(key_state==1)
									KEY = 4;
								break;
							case KEY_5:
								//if(key_state==1)
									KEY = 5;
								break;
							case KEY_8:
								//if(key_state==1)
									KEY = 6;
								break;
							case KEY_3:
								//if(key_state==1)
									KEY = 7;
								break;
							case KEY_6:
								//if(key_state==1)
									KEY = 8;
								break;
							case KEY_9:
								//if(key_state==1)
									KEY = 9;
								break;
							case KEY_TAB:
									KEY = 0;
								break;
							case KEY_LEFTSHIFT:
									KEY = 11;
							default:
								break;
						}
				
						return a;
}




int lcd_show()
{
	
}

int main(int argc, char *argv[])
{
    int ret = 0;
int rett = 0;
int key_state;
int code;

struct input_event bufd;//按键值结构体
    

    int flag,fl,flag_sj=0;
    
	int i,a;
    char* device;
	int trans_ind;
	device="/dev/ttyS3";//与水下通信
    trans_ind = test_data32(device);
	if(trans_ind)
	{
		printf("trans_ind:0x%x\n",trans_ind);
		printf("sorry! get error in transmission\n");
	}
	printf("test_data:%x\n",test_data[5]);
	for(a = 0;a<4;a++)
		buf_sj[a]=test_data[a];
    fd = open("/dev/lcd",O_RDWR);
    if(fd < 0)
    {
        perror("open");
        exit(-1);
    }
   fp = open("/dev/input/event2",O_RDWR);
    if(fp < 0)
    {
        perror("open");
        exit(-1);
    }
    printf("fd=%d\n",fd);
	
	
	flag = 1;
   while(1)
   {
       if(flag == 1)
       {
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            code = bufd.code;
            key_state = bufd.value;
            switch(code) 
            {
                case KEY_1://进入界面3
                    if(key_state==1)
                    {
                        ioctl(fd,LCD3);
						/*输入数据  模拟测量传回de数据*/
						/*for(i = 0;i<4;i++)
						{
							printf("\n please input the %d number",i+1);
							scanf("%d\n",&buf_sj[i]);
						}*/
						for(i = 0;i < 4;i++)//吧转换的字符串赋给数组指针
						{
							buf[i] = (char *)malloc(sizeof(char)*4);
							memcpy((*(buf+i)),my_itoa(buf_sj[i]),strlen(my_itoa(buf_sj[i]))+1);
						}
				       sleep(3);
					  ioctl(fd,LCD3_1);//进入界面3.1
						write(fd,buf,15);
						for(i = 0;i < 4;i++)
						{
							free(buf[i]);
						}
				        
						flag = 2;
                    }
                    break;
                case KEY_4://按键2进入界面4  设置
                    if(key_state==1)
                    {
                        ioctl(fd,LCD4);
                        flag = 3;
                    }
                   break;
			    case KEY_2://按键4进入界面6
                    if(key_state==1)
                    {
                        ioctl(fd,LCD6);
                        flag = 6;
                    }
				    break;
                case KEY_7://按键3进入界面5
                    if(key_state==1)
                    {
                        ioctl(fd,LCD5);
                        flag = 4;
                    }
				    break;
            }      
       }
       if(flag == 2)
       {
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            code = bufd.code;
            key_state = bufd.value;
            switch(code) 
            {
			    case KEY_2://按键4
                    if(key_state==1)
                    {
                        ioctl(fd,LCD2);//返回界面2
                        flag = 1;
                    }
				    break;
            }
       }
	   if(flag == 3)//界面4
	   {
		   rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            code = bufd.code;
            key_state = bufd.value;
            switch(code) 
            {
				case KEY_1://按键一：测量设置
                    if(key_state==1)
                    {
                        ioctl(fd,LCD4_1);
                        flag = 41;
                    }
				    break;
			    case KEY_2://按键四：返回
                    if(key_state==1)
                    {
                        ioctl(fd,LCD2);
                        flag = 1;
                    }
				    break;
				case KEY_7://按键3  ：定时设置
                    if(key_state==1)
                    {
                        ioctl(fd,LCD4_3);
                        flag = 43;
                    }
				    break;
			    case KEY_4://按键二：版本升级
                    if(key_state==1)
                    {
                        ioctl(fd,LCD4_2);
                        flag = 42;
                    }
				    break;
			}
	   }
	   if(flag == 41)//界面4.1
	   {
		   rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            code = bufd.code;
            key_state = bufd.value;
            switch(code) 
            {
				case KEY_1:
                    if(key_state==1)//脉冲周期
                    {
                        ioctl(fd,LCD41_11);
                        flag = 411;
                    }
				    break;
				case KEY_4://脉冲宽度
                    if(key_state==1)
                    {
                        ioctl(fd,LCD41_21);
                        flag = 412;
                    }
				    break;
				case KEY_7:
                    if(key_state==1)//测量次数
                    {
                        ioctl(fd,LCD41_31);
                        flag = 413;
                    }
				    break;
				case KEY_2://f返回界面4
                    if(key_state==1)
                    {
                        ioctl(fd,LCD4);
                        flag = 3;
                    }
				    break;
			}
	   }
	   if(flag == 42)// 版本升级  显示是否升级成功    与 版本号
	   {
		  rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            code = bufd.code;
            key_state = bufd.value;
            switch(code) 
			{
				case KEY_2://f返回界面4
                    if(key_state==1)
                    {
                        ioctl(fd,LCD4);
                        flag = 3;
                    }
				    break;
			}
		   
	   }
	   if(flag == 43)//定时设置 进入界面7
	   {
		   rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            code = bufd.code;
            key_state = bufd.value;
            switch(code) 
            {
				case KEY_1:// 设置时间间隔
                    if(key_state==1)
                    {
                        ioctl(fd,LCD71_11);
                        flag = 71; 
                    }
				    break;
				case KEY_4://开始时间
                    if(key_state==1)
                    {
                        ioctl(fd,LCD72_11);
                        flag = 72; 
                    }
				    break;
				case KEY_2://f返回界面4
                    if(key_state==1)
                    {
                        ioctl(fd,LCD4);
                        flag = 3;
                    }
				    break;
			}
	   }
         
    /*   if(flag == 3)//界面
       {
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            code = bufd.code;
            key_state = bufd.value;
            switch(code) 
            {
			   
			   case KEY_1:
                    if(key_state==1)
                    {
                        ioctl(fd,LCD4_1);
                        flag = 5;
                    }
				    break;
			    case KEY_2:
                    if(key_state==1)
                    {
                        ioctl(fd,LCD2);
                        flag = 1;
                    }
				    break;
				case KEY_4:
                    if(key_state==1)
                    {
                        ioctl(fd,LCD_S_4_2);
                        flag = 90;//设置测量速度标志
                    }
					
				    break;
			    case KEY_7:
                    if(key_state==1)
                    {
                        ioctl(fd,LCD_S_4_3);
                        flag = 93; // 设置河道宽度标志
                    }
				    break;
				case KEY_BACKSPACE:
                    if(key_state==1)
                    {
                        ioctl(fd,LCD_C_4);
                        flag = 3; // 设置河道宽度标志
                    }
				    break;
            }
       }*/
       if(flag == 4)//界面5
       {
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            code = bufd.code;
            key_state = bufd.value;
            switch(code) 
            {
			    case KEY_2://按键4向下翻页
                    if(key_state==1)
                    {
                        ioctl(fd,LCD5_1);
                        flag = 7;
                    }
				    break;
            }
       }
	   if(flag == 7)//5.1界面
       {
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            code = bufd.code;
            key_state = bufd.value;
            switch(code) 
            {
			    case KEY_2:
                    if(key_state==1)
                    {
                        ioctl(fd,LCD5_2);
                        flag = 8;
                    }
				    break;
            }
       }
	   if(flag == 8)//5.2界面
       {
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            code = bufd.code;
            key_state = bufd.value;
            switch(code) 
            {
			    case KEY_2:
                    if(key_state==1)
                    {
                        ioctl(fd,LCD2);
                        flag = 1;
                    }
				    break;
            }
       }
	   
      /* if(flag == 5)//4.1页面
       {
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            code = bufd.code;
            key_state = bufd.value;
            switch(code) 
            {
			    case KEY_1:
					if(key_state==1)
					{
						ioctl(fd,LCD41_11);//设置周期
						flag = 50;
					}
					break;
				case KEY_4:
					if(key_state==1)
					{
						ioctl(fd,LCD41_21);//设置宽度
						flag = 60;
					}
					break;
				case KEY_7:
					if(key_state==1)
					{
						ioctl(fd,LCD41_31);//测量次数
						flag = 70;
					}
					break;
				case KEY_2:
                    if(key_state==1)
                    {   
                        ioctl(fd,LCD4);
                        flag = 3;
                    }
				    break;
			}
        }
		*/
		if(flag == 411)//脉冲周期数据传输411  414  415
	   {
		   rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            key_state = bufd.value;
			if(key_state == 1)
			{
				key_4x4();
				key_4x4();
				if(KEY != 11)
				buf_set[0]=KEY;
			
				buf_s[0] = (char *)malloc(sizeof(char)*4);
				memcpy((*(buf_s+0)),my_itoa(buf_set[0]),strlen(my_itoa(buf_set[0]))+1);
			
				write(fd,buf_s,15);
		
				free(buf_s[0]);
		   
				flag = 414;    
			}			
		  
		}
		if(flag == 414)
		{
			
			ioctl(fd,LCD41_12);
			
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            key_state = bufd.value;
			if(key_state == 1)
			{
				key_4x4();
				key_4x4();
				if(KEY != 11)
				buf_set[1]=KEY;
			
				buf_s[1] = (char *)malloc(sizeof(char)*4);
				memcpy((*(buf_s+1)),my_itoa(buf_set[1]),strlen(my_itoa(buf_set[1]))+1);
			
				write(fd,buf_s,15);
		
				free(buf_s[1]);
		   
				flag = 415;    
			}	
			
		}
		if(flag == 415)
		{
			
			ioctl(fd,LCD41_13);
			
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            key_state = bufd.value;
			if(key_state == 1)
			{
				key_4x4();
				key_4x4();
				if(KEY != 11)
				buf_set[2]=KEY;
			
				buf_s[2] = (char *)malloc(sizeof(char)*4);
				memcpy((*(buf_s+2)),my_itoa(buf_set[2]),strlen(my_itoa(buf_set[2]))+1);
			
				write(fd,buf_s,15);
		
				free(buf_s[2]);
				zhouqi_set[0] = (buf_set[0]*100+buf_set[1]*10+buf_set[2]);//把输入的数据保存进数组
				
				if(KEY==11)//输入完成 
				{
					ioctl(fd,LCD_C_4);
					KEY = 0;
					flag = 41;
					printf("zhouqi: %d\n",zhouqi_set[0]);
				}	
			}		
		}
		
	    if(flag == 412)//脉冲宽度   412  416 417
	   {
		   rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            key_state = bufd.value;
			if(key_state == 1)
			{
				key_4x4();
				key_4x4();
				if(KEY != 11)
				buf_set[0]=KEY;
			
				buf_s[0] = (char *)malloc(sizeof(char)*4);
				memcpy((*(buf_s+0)),my_itoa(buf_set[0]),strlen(my_itoa(buf_set[0]))+1);
			
				write(fd,buf_s,15);
		
				free(buf_s[0]);
		   
				flag = 416;    
			}			
		  
		}
		if(flag == 416)
		{
			
			ioctl(fd,LCD41_22);
			
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            key_state = bufd.value;
			if(key_state == 1)
			{
				key_4x4();
				key_4x4();
				if(KEY != 11)
				buf_set[1]=KEY;
			
				buf_s[1] = (char *)malloc(sizeof(char)*4);
				memcpy((*(buf_s+1)),my_itoa(buf_set[1]),strlen(my_itoa(buf_set[1]))+1);
			
				write(fd,buf_s,15);
		
				free(buf_s[1]);
		   
				flag = 417;    
			}	
			
		}
		if(flag == 417)
		{
			
			ioctl(fd,LCD41_23);
			
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            key_state = bufd.value;
			if(key_state == 1)
			{
				key_4x4();
				key_4x4();
				if(KEY != 11)
				buf_set[2]=KEY;
			
				buf_s[2] = (char *)malloc(sizeof(char)*4);
				memcpy((*(buf_s+2)),my_itoa(buf_set[2]),strlen(my_itoa(buf_set[2]))+1);
			
				write(fd,buf_s,15);
		
				free(buf_s[2]);
				kuandu_set[0] = (buf_set[0]*100+buf_set[1]*10+buf_set[2]);//把输入的数据保存进数组
				
				if(KEY==11)
				{
					ioctl(fd,LCD_C_4);
					KEY = 0;
					flag = 41;
					printf("kuandu_set %d\n",kuandu_set[0]);
				}	
			}	
			
		}
		
		if(flag == 413)//测量次数   413   418   419
	   {
		   rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            key_state = bufd.value;
			if(key_state == 1)
			{
				key_4x4();
				key_4x4();
				if(KEY != 11)
				buf_set[0]=KEY;
			
				buf_s[0] = (char *)malloc(sizeof(char)*4);
				memcpy((*(buf_s+0)),my_itoa(buf_set[0]),strlen(my_itoa(buf_set[0]))+1);
			
				write(fd,buf_s,15);
		
				free(buf_s[0]);
		   
				flag = 418;    
			}			
		  
		}  
		if(flag == 418)
		{
			
			ioctl(fd,LCD41_32);
			
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            key_state = bufd.value;
			if(key_state == 1)
			{
				key_4x4();
				key_4x4();
				if(KEY != 11)
				buf_set[1]=KEY;
			
				buf_s[1] = (char *)malloc(sizeof(char)*4);
				memcpy((*(buf_s+1)),my_itoa(buf_set[1]),strlen(my_itoa(buf_set[1]))+1);
			
				write(fd,buf_s,15);
		
				free(buf_s[1]);
		   
				flag = 419;    
			}	
			
		}
		if(flag == 419)
		{
			
			ioctl(fd,LCD41_33);
			
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            key_state = bufd.value;
			if(key_state == 1)
			{
				key_4x4();
				key_4x4();
				if(KEY != 11)
				buf_set[2]=KEY;
			
				buf_s[2] = (char *)malloc(sizeof(char)*4);
				memcpy((*(buf_s+2)),my_itoa(buf_set[2]),strlen(my_itoa(buf_set[2]))+1);
			
				write(fd,buf_s,15);
		
				free(buf_s[2]);
				cishu_set[0] = (buf_set[0]*100+buf_set[1]*10+buf_set[2]);//把输入的数据保存进数组
				
				if(KEY==11)
				{
					ioctl(fd,LCD_C_4);
					KEY = 0;
					flag = 41;
					printf("cishu_set %d\n",cishu_set[0]);
				}	
			}	
			
		}
		
       if(flag == 6)//界面6
       {
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            code = bufd.code;
            key_state = bufd.value;
            switch(code) 
            {
				case KEY_1:
                    if(key_state==1)
                    {
                        ioctl(fd,LCD61_11);
                        flag = 67;
                    }
				    break;
			    case KEY_4:
                    if(key_state==1)
                    {
                        ioctl(fd,LCD61_21);
                        flag = 64;
                    }
				    break;
				case KEY_7:
                    if(key_state==1)
                    {
                        ioctl(fd,LCD6_1);
                        flag = 61;
                    }
				    break;
			    case KEY_2:
                    if(key_state==1)
                    {
                        ioctl(fd,LCD2);
                        flag = 1;
                    }
				    break;
            }
       }
	    if(flag == 61)//界面6_1
		{
			rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            code = bufd.code;
            key_state = bufd.value;
            switch(code) 
			{
				case KEY_2:
                    if(key_state==1)
                    {
                        ioctl(fd,LCD2);
                        flag = 6;
                    }
				    break;
			}
		}
		
       
	   if(flag == 71)//定时时间
	   {
		   rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            key_state = bufd.value;
			if(key_state == 1)
			{
				key_4x4();
				key_4x4();
				if(KEY != 11)
				buf_set[0]=KEY;
			
				buf_s[0] = (char *)malloc(sizeof(char)*4);
				memcpy((*(buf_s+0)),my_itoa(buf_set[0]),strlen(my_itoa(buf_set[0]))+1);
			
				write(fd,buf_s,15);
		
				free(buf_s[0]);
		   
				flag = 73;    
			}			
		  
		}
		if(flag == 73)
		{
			
			ioctl(fd,61);
			
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            key_state = bufd.value;
			if(key_state == 1)
			{
				key_4x4();
				key_4x4();
				if(KEY != 11)
				buf_set[2]=KEY;
			
				buf_s[2] = (char *)malloc(sizeof(char)*4);
				memcpy((*(buf_s+2)),my_itoa(buf_set[2]),strlen(my_itoa(buf_set[2]))+1);
			
				write(fd,buf_s,15);
		
				free(buf_s[2]);
				clcs_set[0] = (buf_set[0]*100+buf_set[1]*10+buf_set[2]);//把输入的数据保存进数组
				
				if(KEY==11)
				{
					ioctl(fd,LCD_C_4);
					KEY = 0;
					flag = 3;
					printf("clcs_set[0] %d\n",clcs_set[0]);
				}	
			}	
			
		}
		
		
		
		if(flag == 67)// 河道宽度
	   {
		   rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            key_state = bufd.value;
			if(key_state == 1)
			{
				key_4x4();
				key_4x4();
				if(KEY != 11)
				buf_set[0]=KEY;
			
				buf_s[0] = (char *)malloc(sizeof(char)*4);
				memcpy((*(buf_s+0)),my_itoa(buf_set[0]),strlen(my_itoa(buf_set[0]))+1);
			
				write(fd,buf_s,15);
		
				free(buf_s[0]);
		   
				flag = 65;    
			}			
		  
		}
		if(flag == 65)
		{
			
			ioctl(fd,LCD61_12);
			
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            key_state = bufd.value;
			if(key_state == 1)
			{
				key_4x4();
				key_4x4();
				if(KEY != 11)
				buf_set[1]=KEY;
			
				buf_s[1] = (char *)malloc(sizeof(char)*4);
				memcpy((*(buf_s+1)),my_itoa(buf_set[1]),strlen(my_itoa(buf_set[1]))+1);
			
				write(fd,buf_s,15);
		
				free(buf_s[1]);
		   
				flag = 66;    
			}	
			
		}
		if(flag == 66)
		{
			
			ioctl(fd,LCD61_13);
			
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            key_state = bufd.value;
			if(key_state == 1)
			{
				key_4x4();
				key_4x4();
				if(KEY != 11)
				buf_set[2]=KEY;
			
				buf_s[2] = (char *)malloc(sizeof(char)*4);
				memcpy((*(buf_s+2)),my_itoa(buf_set[2]),strlen(my_itoa(buf_set[2]))+1);
			
				write(fd,buf_s,15);
		
				free(buf_s[2]);
				clcs_set[1] = (buf_set[0]*100+buf_set[1]*10+buf_set[2]);//把输入的数据保存进数组
				
				if(KEY==11)
				{
					ioctl(fd,LCD_C_4);
					KEY = 0;
					flag = 6;
					printf("clcs_set[1] %d\n",clcs_set[1]);
				}
			}	
			
		}
		
		
		if(flag == 64)// 截面积
	   {
		   rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            key_state = bufd.value;
			if(key_state == 1)
			{
				key_4x4();
				key_4x4();
				if(KEY != 11)
				buf_set[0]=KEY;
			
				buf_s[0] = (char *)malloc(sizeof(char)*4);
				memcpy((*(buf_s+0)),my_itoa(buf_set[0]),strlen(my_itoa(buf_set[0]))+1);
			
				write(fd,buf_s,15);
		
				free(buf_s[0]);
		   
				flag = 68;    
			}			
		  
		}
		if(flag == 68)
		{
			
			ioctl(fd,LCD61_22);
			
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            key_state = bufd.value;
			if(key_state == 1)
			{
				key_4x4();
				key_4x4();
				if(KEY != 11)
				buf_set[1]=KEY;
			
				buf_s[1] = (char *)malloc(sizeof(char)*4);
				memcpy((*(buf_s+1)),my_itoa(buf_set[1]),strlen(my_itoa(buf_set[1]))+1);
			
				write(fd,buf_s,15);
		
				free(buf_s[1]);
		   
				flag = 69;    
			}	
			
		}
		if(flag == 69)
		{
			
			ioctl(fd,LCD61_23);
			
            rett = read(fp,&bufd,sizeof(struct input_event));
            if (rett <= 0)
		    {
                printf("read failed!\n");
                return -1;
            }
            key_state = bufd.value;
			if(key_state == 1)
			{
				key_4x4();
				key_4x4();
				if(KEY != 11)
				buf_set[2]=KEY;
			
				buf_s[2] = (char *)malloc(sizeof(char)*4);
				memcpy((*(buf_s+2)),my_itoa(buf_set[2]),strlen(my_itoa(buf_set[2]))+1);
			
				write(fd,buf_s,15);
		
				free(buf_s[2]);
				clcs_set[1] = (buf_set[0]*100+buf_set[1]*10+buf_set[2]);//把输入的数据保存进数组
				
				if(KEY==11)
				{
					ioctl(fd,LCD_C_4);
					KEY = 0;
					flag = 6;
					printf("clcs_set[1] %d\n",clcs_set[1]);
				}
			}	
			
		}
  }

					
    close(fd);
    return 0;
}
