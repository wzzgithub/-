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


#define    LCD71_11   _IO('A',9)//ʱ����
#define    LCD71_12   _IO('A',20)//ʱ����
#define    LCD72_11   _IO('A',10)//��ʼʱ��
#define    LCD_C_4    _IO('A',11)//�˳�����
#define    LCD_CL_2    _IO('A',12)

#define  LCD41_31  _IO('A',13)//���ò�������ʮλ
#define  LCD41_32  _IO('A',14)//���ò���������λ
#define  LCD41_33  _IO('A',28)//���ò���������λ

#define  LCD41_21  _IO('A',15)//���ò������ʮλ
#define  LCD41_22  _IO('A',16)//���ò�����ȸ�λ
#define  LCD41_23  _IO('A',27)//���ò�����ȸ�λ


#define  LCD41_11       _IO('A',18)//��������
#define  LCD41_12     _IO('A',19)//��������
#define  LCD41_13     _IO('A',26)//��������

#define  LCD61_11       _IO('A',60)//��������
#define  LCD61_12     _IO('A',61)//��������
#define  LCD61_13     _IO('A',62)//��������
#define  LCD61_21       _IO('A',63)//��������
#define  LCD61_22     _IO('A',64)//��������
#define  LCD61_23     _IO('A',65)//��������
char * buf[4];
char buf_sj[4];//�������������

char * buf_s[4] = {"0"};//������������(��Ļ����ʾ)
int buf_set[4];//�������� �Ĵ��� ����
int zhouqi_set[4]; //��������
int kuandu_set[4]; //���ÿ��
int cishu_set[4]; //���ò�������

int clcs_set[4]; //���ò�������

int KEY;
int fd,fp;

char * reverse(char *s)
{
    char temp;
    char *p = s;//pָ��s��ͷ��
    char *q = s;//qָ��s��β��
    while(*q)
        ++q;
    q--;
    while(q > p)//�����ƶ�ָ�룬ֱ��p��q����
    {
        temp = *p;
        *p++ = *q;
        *q-- = temp;
    }
    return s;
}

/*  
 *  ���ܣ�����ת��Ϊ�ַ���
 *  char s[]�������Ǵ洢������ÿһλ
 */
char *my_itoa(int n)
{
    int i = 0,isNegative = 0;
    static char s[100];//Ϊstatic������������ȫ�ֱ���
    if((isNegative = n) < 0) //����Ǹ�������׬Ϊ����
    {
        n = -n;
    }
    do   //�Ӹ�λ��ʼ��Ϊ�ַ���ֱ�����λ�����Ӧ�÷�ת
    {
        s[i++] = n%10 + '0';
        n = n/10;
    }while(n > 0);

    if(isNegative < 0)   //����Ǹ��������ϸ���
    {
		
        s[i++] = '-';
    }
    s[i]='\0';   //�������ַ���������
    
   return reverse(s);
}

int key_4x4()
{
	
int ret = 0;
int state;
int cod;
int a;


struct input_event bufd;//����ֵ�ṹ��
						ret = read(fp,&bufd,sizeof(struct input_event));
						if (ret <= 0)
						{
							printf("read failed!\n");
							return -1;
						}
						cod = bufd.code;
					
			
					
						switch(cod)
						{
							case KEY_1:  //����1
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

struct input_event bufd;//����ֵ�ṹ��
    

    int flag,fl,flag_sj=0;
    
	int i,a;
    char* device;
	int trans_ind;
	device="/dev/ttyS3";//��ˮ��ͨ��
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
                case KEY_1://�������3
                    if(key_state==1)
                    {
                        ioctl(fd,LCD3);
						/*��������  ģ���������de����*/
						/*for(i = 0;i<4;i++)
						{
							printf("\n please input the %d number",i+1);
							scanf("%d\n",&buf_sj[i]);
						}*/
						for(i = 0;i < 4;i++)//��ת�����ַ�����������ָ��
						{
							buf[i] = (char *)malloc(sizeof(char)*4);
							memcpy((*(buf+i)),my_itoa(buf_sj[i]),strlen(my_itoa(buf_sj[i]))+1);
						}
				       sleep(3);
					  ioctl(fd,LCD3_1);//�������3.1
						write(fd,buf,15);
						for(i = 0;i < 4;i++)
						{
							free(buf[i]);
						}
				        
						flag = 2;
                    }
                    break;
                case KEY_4://����2�������4  ����
                    if(key_state==1)
                    {
                        ioctl(fd,LCD4);
                        flag = 3;
                    }
                   break;
			    case KEY_2://����4�������6
                    if(key_state==1)
                    {
                        ioctl(fd,LCD6);
                        flag = 6;
                    }
				    break;
                case KEY_7://����3�������5
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
			    case KEY_2://����4
                    if(key_state==1)
                    {
                        ioctl(fd,LCD2);//���ؽ���2
                        flag = 1;
                    }
				    break;
            }
       }
	   if(flag == 3)//����4
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
				case KEY_1://����һ����������
                    if(key_state==1)
                    {
                        ioctl(fd,LCD4_1);
                        flag = 41;
                    }
				    break;
			    case KEY_2://�����ģ�����
                    if(key_state==1)
                    {
                        ioctl(fd,LCD2);
                        flag = 1;
                    }
				    break;
				case KEY_7://����3  ����ʱ����
                    if(key_state==1)
                    {
                        ioctl(fd,LCD4_3);
                        flag = 43;
                    }
				    break;
			    case KEY_4://���������汾����
                    if(key_state==1)
                    {
                        ioctl(fd,LCD4_2);
                        flag = 42;
                    }
				    break;
			}
	   }
	   if(flag == 41)//����4.1
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
                    if(key_state==1)//��������
                    {
                        ioctl(fd,LCD41_11);
                        flag = 411;
                    }
				    break;
				case KEY_4://������
                    if(key_state==1)
                    {
                        ioctl(fd,LCD41_21);
                        flag = 412;
                    }
				    break;
				case KEY_7:
                    if(key_state==1)//��������
                    {
                        ioctl(fd,LCD41_31);
                        flag = 413;
                    }
				    break;
				case KEY_2://f���ؽ���4
                    if(key_state==1)
                    {
                        ioctl(fd,LCD4);
                        flag = 3;
                    }
				    break;
			}
	   }
	   if(flag == 42)// �汾����  ��ʾ�Ƿ������ɹ�    �� �汾��
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
				case KEY_2://f���ؽ���4
                    if(key_state==1)
                    {
                        ioctl(fd,LCD4);
                        flag = 3;
                    }
				    break;
			}
		   
	   }
	   if(flag == 43)//��ʱ���� �������7
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
				case KEY_1:// ����ʱ����
                    if(key_state==1)
                    {
                        ioctl(fd,LCD71_11);
                        flag = 71; 
                    }
				    break;
				case KEY_4://��ʼʱ��
                    if(key_state==1)
                    {
                        ioctl(fd,LCD72_11);
                        flag = 72; 
                    }
				    break;
				case KEY_2://f���ؽ���4
                    if(key_state==1)
                    {
                        ioctl(fd,LCD4);
                        flag = 3;
                    }
				    break;
			}
	   }
         
    /*   if(flag == 3)//����
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
                        flag = 90;//���ò����ٶȱ�־
                    }
					
				    break;
			    case KEY_7:
                    if(key_state==1)
                    {
                        ioctl(fd,LCD_S_4_3);
                        flag = 93; // ���úӵ���ȱ�־
                    }
				    break;
				case KEY_BACKSPACE:
                    if(key_state==1)
                    {
                        ioctl(fd,LCD_C_4);
                        flag = 3; // ���úӵ���ȱ�־
                    }
				    break;
            }
       }*/
       if(flag == 4)//����5
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
			    case KEY_2://����4���·�ҳ
                    if(key_state==1)
                    {
                        ioctl(fd,LCD5_1);
                        flag = 7;
                    }
				    break;
            }
       }
	   if(flag == 7)//5.1����
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
	   if(flag == 8)//5.2����
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
	   
      /* if(flag == 5)//4.1ҳ��
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
						ioctl(fd,LCD41_11);//��������
						flag = 50;
					}
					break;
				case KEY_4:
					if(key_state==1)
					{
						ioctl(fd,LCD41_21);//���ÿ��
						flag = 60;
					}
					break;
				case KEY_7:
					if(key_state==1)
					{
						ioctl(fd,LCD41_31);//��������
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
		if(flag == 411)//�����������ݴ���411  414  415
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
				zhouqi_set[0] = (buf_set[0]*100+buf_set[1]*10+buf_set[2]);//����������ݱ��������
				
				if(KEY==11)//������� 
				{
					ioctl(fd,LCD_C_4);
					KEY = 0;
					flag = 41;
					printf("zhouqi: %d\n",zhouqi_set[0]);
				}	
			}		
		}
		
	    if(flag == 412)//������   412  416 417
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
				kuandu_set[0] = (buf_set[0]*100+buf_set[1]*10+buf_set[2]);//����������ݱ��������
				
				if(KEY==11)
				{
					ioctl(fd,LCD_C_4);
					KEY = 0;
					flag = 41;
					printf("kuandu_set %d\n",kuandu_set[0]);
				}	
			}	
			
		}
		
		if(flag == 413)//��������   413   418   419
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
				cishu_set[0] = (buf_set[0]*100+buf_set[1]*10+buf_set[2]);//����������ݱ��������
				
				if(KEY==11)
				{
					ioctl(fd,LCD_C_4);
					KEY = 0;
					flag = 41;
					printf("cishu_set %d\n",cishu_set[0]);
				}	
			}	
			
		}
		
       if(flag == 6)//����6
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
	    if(flag == 61)//����6_1
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
		
       
	   if(flag == 71)//��ʱʱ��
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
				clcs_set[0] = (buf_set[0]*100+buf_set[1]*10+buf_set[2]);//����������ݱ��������
				
				if(KEY==11)
				{
					ioctl(fd,LCD_C_4);
					KEY = 0;
					flag = 3;
					printf("clcs_set[0] %d\n",clcs_set[0]);
				}	
			}	
			
		}
		
		
		
		if(flag == 67)// �ӵ����
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
				clcs_set[1] = (buf_set[0]*100+buf_set[1]*10+buf_set[2]);//����������ݱ��������
				
				if(KEY==11)
				{
					ioctl(fd,LCD_C_4);
					KEY = 0;
					flag = 6;
					printf("clcs_set[1] %d\n",clcs_set[1]);
				}
			}	
			
		}
		
		
		if(flag == 64)// �����
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
				clcs_set[1] = (buf_set[0]*100+buf_set[1]*10+buf_set[2]);//����������ݱ��������
				
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
