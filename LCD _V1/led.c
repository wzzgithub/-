#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/irq.h>
#include <linux/gpio.h>

#include <linux/clip.h>
char * buf1[]={"��ӭʹ��"};
char * buf2[]={"1.�������","2.����","3.�豸״̬","4.�ӵ�����"};
char * buf3[]={"���ڲ���������������"};
char * buf3_1[]={"1.���٣� ","2.������","3.ˮ�£� ","4.����"};
char * buf3_2[]={"1.�ӻ����� ","2.ˮ�¹���"," ","4.����"};
char * buf4[]={"1.��������","2.�汾���� ","3.��ʱ���� ","4.����"};
char * buf4_1[]={"1.��������:","2.������:","3.��������:","4.����"};
char * buf4_2[]={"1.�����ɹ��� ","2.�汾�ţ�","","4.����"};
char * buf5[]={"1.ˮ�£�17��c","2.ˮ� 0.5m","3.��λ�� 32/13/42","4.��ҳ"};
char * buf5_1[]={"1.�ӻ�״̬:�ϵ�/�µ�","2.ˮ��״̬:�ϵ�/�µ�","3.©ˮ:��","4.��һҳ"};
char * buf5_2[]={"1.�汾�ţ�","","","4.����"};
char * buf6[]={"1.�ӵ����:  ","2.�������","3.�����ͼ��","4.����"};
char * buf6_1[]={"1"};
char * buf4_3[]={"ʱ����:","��ʼʱ��:","","����"};
#define CONTROL_MODULE  0x44e10000
#define RST  0x9A4
#define SID  0x99C
#define CS  0x9A0
#define SCLK1  0X9AC
#define SCLK2  0X990
#define GND   0x994  //LCD����
#define LED_RUN   0xA4C   
#define LED_COM   0xA48

#define USB_EN         0xAC0

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



void __iomem *control;
int vall = 0;
int cmd = 0;
int major = 500;
int minor = 0;
struct cdev mycdev;
static struct class *lcd_class;         //LED驱动???void __iomem *timter0; //基地址0x48044000

char * buf[4]={0};


#define TIMTER_BASE    0x44E05000 //timer3
#define CM_DPLL        0x44DF4200
#define TCLR         0x38
#define TLDR         0X40
#define IRQ_EN_SET   0x2c
#define IRQ_STS      0x28
#define TTGR         0x44
#define TIMER2       0x04
#define GPIO_TO_PIN(bank, gpio)  (32 * (bank) + (gpio))
int i=7;
int n=7;
int flag;
//-------------------------------------------------
//�ϰ���д����
//-------------------------------------------------
void cmd1(char cmd)
{
	int j;
	gpio_set_value(GPIO_TO_PIN(3,18),1);
	gpio_set_value(GPIO_TO_PIN(3,17), 1);
	for(j=0;j<5;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,21), 0);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,21), 1);
		udelay(10);
	}
	gpio_set_value(GPIO_TO_PIN(3,21), 0);
		udelay(10);
	gpio_set_value(GPIO_TO_PIN(3,17), 0);
	for(j=0;j<3;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,21), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,21), 0);
		udelay(10);
	}
	for(j=0;j<4;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,17),(cmd&(0x01<<i))>>i);
		i--;
            if(i<0)
            {
                i = 7;
			}
		gpio_set_value(GPIO_TO_PIN(3,21), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,21), 0);
		udelay(10);
	}
	gpio_set_value(GPIO_TO_PIN(3,17), 0);
	for(j=0;j<4;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,21), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,21), 0);
		udelay(10);
	}
	for(j=0;j<4;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,17),(cmd&(0x01<<i))>>i);
		i--;
            if(i<0)
            {
                i = 7;
			}
		gpio_set_value(GPIO_TO_PIN(3,21), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,21), 0);
		udelay(10);
	}
	gpio_set_value(GPIO_TO_PIN(3,17), 0);
	for(j=0;j<4;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,21), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,21), 0);
		udelay(10);
	}
	gpio_set_value(GPIO_TO_PIN(3,18),0);
}
//-------------------------------------------------
//�°���д����
//-------------------------------------------------
void cmd2(char cmd)
{
	int j;
	gpio_set_value(GPIO_TO_PIN(3,18),1);
	gpio_set_value(GPIO_TO_PIN(3,17), 1);
	for(j=0;j<5;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,14), 0);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,14), 1);
		udelay(10);
	}
	gpio_set_value(GPIO_TO_PIN(3,14), 0);
		udelay(10);
	gpio_set_value(GPIO_TO_PIN(3,17), 0);
	for(j=0;j<3;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,14), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,14), 0);
		udelay(10);
	}
	for(j=0;j<4;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,17),(cmd&(0x01<<i))>>i);
		i--;
            if(i<0)
            {
                i = 7;
			}
		gpio_set_value(GPIO_TO_PIN(3,14), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,14), 0);
		udelay(10);
	}
	gpio_set_value(GPIO_TO_PIN(3,17), 0);
	for(j=0;j<4;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,14), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,14), 0);
		udelay(10);
	}
	for(j=0;j<4;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,17),(cmd&(0x01<<i))>>i);
		i--;
            if(i<0)
            {
                i = 7;
			}
		gpio_set_value(GPIO_TO_PIN(3,14), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,14), 0);
		udelay(10);
	}
	gpio_set_value(GPIO_TO_PIN(3,17), 0);
	for(j=0;j<4;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,14), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,14), 0);
		udelay(10);
	}
	gpio_set_value(GPIO_TO_PIN(3,18),0);
}

//-------------------------------------------------
//�ϰ���д����
//-------------------------------------------------
void data1(char dat)
{
	int j;
	gpio_set_value(GPIO_TO_PIN(3,18),1);
	gpio_set_value(GPIO_TO_PIN(3,17), 1);
	for(j=0;j<5;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,21), 0);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,21), 1);
		udelay(10);
	}
	gpio_set_value(GPIO_TO_PIN(3,21), 0);
	udelay(10);
	gpio_set_value(GPIO_TO_PIN(3,17), 0);
	gpio_set_value(GPIO_TO_PIN(3,21), 1);
	udelay(10);
		
	gpio_set_value(GPIO_TO_PIN(3,21), 0);
	udelay(10);
	gpio_set_value(GPIO_TO_PIN(3,17), 1);
	gpio_set_value(GPIO_TO_PIN(3,21), 1);
	udelay(10);
	
	gpio_set_value(GPIO_TO_PIN(3,21), 0);
	udelay(10);
	gpio_set_value(GPIO_TO_PIN(3,17), 0);
	gpio_set_value(GPIO_TO_PIN(3,21), 1);
	udelay(10);

    gpio_set_value(GPIO_TO_PIN(3,21), 0);
    udelay(10);
	for(j=0;j<4;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,17),(dat&(0x01<<i))>>i);
		i--;
            if(i<0)
            {
                i = 7;
			}
		gpio_set_value(GPIO_TO_PIN(3,21), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,21), 0);
		udelay(10);
	}
	gpio_set_value(GPIO_TO_PIN(3,17), 0);
	for(j=0;j<4;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,21), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,21), 0);
		udelay(10);
	}
	for(j=0;j<4;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,17),(dat&(0x01<<i))>>i);
		i--;
            if(i<0)
            {
                i = 7;
			}
		gpio_set_value(GPIO_TO_PIN(3,21), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,21), 0);
		udelay(10);
	}
	gpio_set_value(GPIO_TO_PIN(3,17), 0);
	for(j=0;j<4;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,21), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,21), 0);
		udelay(10);
	}
	gpio_set_value(GPIO_TO_PIN(3,18),0);
}

//-------------------------------------------------
// �°���д����
//-------------------------------------------------
void data2(char dat)
{
	int j;
	gpio_set_value(GPIO_TO_PIN(3,18),1);
	gpio_set_value(GPIO_TO_PIN(3,17), 1);
	for(j=0;j<5;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,14), 0);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,14), 1);
		udelay(10);
	}
	gpio_set_value(GPIO_TO_PIN(3,14), 0);
	udelay(10);
	gpio_set_value(GPIO_TO_PIN(3,17), 0);
	gpio_set_value(GPIO_TO_PIN(3,14), 1);
	udelay(10);
		
	gpio_set_value(GPIO_TO_PIN(3,14), 0);
	udelay(10);
	gpio_set_value(GPIO_TO_PIN(3,17), 1);
	gpio_set_value(GPIO_TO_PIN(3,14), 1);
	udelay(10);
	
	gpio_set_value(GPIO_TO_PIN(3,14), 0);
	udelay(10);
	gpio_set_value(GPIO_TO_PIN(3,17), 0);
	gpio_set_value(GPIO_TO_PIN(3,14), 1);
	udelay(10);

    gpio_set_value(GPIO_TO_PIN(3,14), 0);
    udelay(10);
	for(j=0;j<4;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,17),(dat&(0x01<<i))>>i);
		i--;
            if(i<0)
            {
                i = 7;
			}
		gpio_set_value(GPIO_TO_PIN(3,14), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,14), 0);
		udelay(10);
	}
	gpio_set_value(GPIO_TO_PIN(3,17), 0);
	for(j=0;j<4;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,14), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,14), 0);
		udelay(10);
	}
	for(j=0;j<4;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,17),(dat&(0x01<<i))>>i);
		i--;
            if(i<0)
            {
                i = 7;
			}
		gpio_set_value(GPIO_TO_PIN(3,14), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,14), 0);
		udelay(10);
	}
	gpio_set_value(GPIO_TO_PIN(3,17), 0);
	for(j=0;j<4;j++)
	{
		gpio_set_value(GPIO_TO_PIN(3,14), 1);
		udelay(10);
		gpio_set_value(GPIO_TO_PIN(3,14), 0);
		udelay(10);
	}
	gpio_set_value(GPIO_TO_PIN(3,18),0);
}

//-------------------------------------------------
//��ʼ��
//-------------------------------------------------
void init_lcd(void)
{
	cmd1(0x34);	   //--����ָ�����
	cmd2(0x34);	   //--����ָ�����
    cmd1(0x30);	   //--����ָ�����
	cmd2(0x30);	   //--����ָ�����
    cmd1(0x01);	   //--�����ʾ
	cmd2(0x01);	   //--�����ʾ
    cmd1(0x06);	   //ָ��������д����ȡʱ�������ƶ�����
	cmd2(0x06);	   //ָ��������д����ȡʱ�������ƶ�����
}
//-------------------------------------------------
// �趨X,Y ����λ��(�ı�ģʽ)   ^_^
//     ��ʾλ��Ϊ��Y��(һ��Ϊ16��),��col*16��//-------------------------------------------------
void gotoxy(uint row, uint col)    //��col Ϊ����ÿ����1������16��
{
    switch(row)
    {
        case 1: 
            cmd1(0x80+col);
            break;
        case 2:
            cmd1(0x90+col);
            break;
        case 3:
            cmd2(0x80+col);
            break;
        case 4:
            cmd2(0x90+col);
            break;

    }
}
//����
void lcd_cls(void)
{
	mdelay(500);
	cmd1(0x30);
	cmd1(0x01);
	cmd2(0x30);
	cmd2(0x01);
	
}

//-------------------------------------------------
// �ϰ�����ӡ�ַ���
//------------------------------------------------
void sendstr1(char *ptstring)
{
    while((*ptstring)!='\0')
    {
        data1(*ptstring++);
    }
} 
//-------------------------------------------------
// �°�����ӡ�ַ���
//------------------------------------------------
void sendstr2(char *ptstring)
{
    while((*ptstring)!='\0')
    {
        data2(*ptstring++);
    }
}


//     �� �� ˵ ��: ��ʾ5�����Ի���       
//-------------------------------------------------
void  display(int fst, int snd)
{ 
  int x,y,k;
	//   cmd1(0x36);     //����ָ�� ��ͼ��ʾ
       cmd2(0x36);     //����ָ�� ��ͼ��ʾ
    /*  for(y=0;y<32;y++)
      {     if((y%2)==1)
               k=fst;
            else
               k=snd;
            for(x=0;x<12;x++)
            { 
			   cmd1(y+0x80);    //�е�ַ 
               cmd1(x+0x80);        
               data1(k);
               data1(k);       
            }
       }  
*/
     for(y=0;y<32;y++)
      {     if((y%2)==1)
               k=fst;
            else
               k=snd;
            for(x=0;x<12;x++)
            { 
               cmd2(y+0x80);    //�е�ַ  
               cmd2(x+0x80);           
               data2(k);
               data2(k);
            }
       }  
   		//cmd1(0x30);               //����ָ�� ��ͼ�ر� 
		cmd2(0x30);               //����ָ�� ��ͼ�ر�   
} 


/*//�жϴ�����
static irqreturn_t timter4_interrupt(int irqno,void * devid)
{
    int val;
    

    val = ioread32(timter0+IRQ_STS)|(0x00);
    iowrite32(val,(timter0+IRQ_STS));
    return IRQ_HANDLED;
}*/
static int lcd_open(struct inode*inode, struct file *file)
{
    int result;
    //printk("opencmd!\n");  /*ÿ�δ��ļ�ʱ���һ��open*/
   result = gpio_direction_output(GPIO_TO_PIN(3,19), 0);
    if (result != 0)
        printk("gpio_direction(3_19) failed!\n");
    msleep(400);
    result = gpio_direction_output(GPIO_TO_PIN(3,19), 1);
    if (result != 0)
        printk("gpio_direction(3_19) failed!\n");
    msleep(400);
    
	result = gpio_direction_output(GPIO_TO_PIN(3,15), 1);
    if (result != 0)
        printk("gpio_direction(3_15) failed!\n");
	init_lcd();
	mdelay(1000);
	//mdelay(1000);
	//mdelay(1000);
	gotoxy(2,4);
	sendstr1("��ӭʹ�� ");
	mdelay(1000);
	lcd_cls();
		
		
		
		gotoxy(1,0);
		sendstr1(buf2[0]);
		gotoxy(2,0);
        sendstr1(buf2[1]);
        gotoxy(3,0);
		sendstr2(buf2[2]);
		gotoxy(4,0);
		sendstr2(buf2[3]);	
		
		
	
	
    return 0;
}
/*�ͷ�open()�������������Դ*/
static int lcd_release (struct inode *inode, struct file *filp)
{
    int ret = 0;
  
    //printk("led_release\n");
    return ret;
}
static ssize_t lcd_write (struct file *filp, const char __user *user_buf, size_t count, loff_t * arg)
{
    ssize_t ret = 0;
   // printk("beep_write\n");
   //int i;
    ret = copy_from_user(buf,user_buf,15);
     
	/*flag 1-3 �����*/
	if(flag == 1)
	{
		gotoxy(2,7);
		sendstr1(buf[0]);
		cmd1(0x14);	
	}
	if(flag == 2)
	{
		gotoxy(2,8);
		sendstr1(buf[1]);
		cmd1(0x15);
	}
	if(flag == 3)
	{
		gotoxy(2,9);
		sendstr1(buf[2]);
		cmd2(0x16);
	}
	/*5-7�ӵ����*/
	if(flag == 5)
	{
		gotoxy(1,7);
		sendstr1(buf[0]);
		cmd1(0x14);	
	}
	if(flag == 6)
	{
		gotoxy(1,8);
		sendstr1(buf[1]);
		cmd1(0x15);
	}
	if(flag == 7)
	{
		gotoxy(1,9);
		sendstr1(buf[2]);
		cmd2(0x16);
	}

	/*411~~ ��������  420~~������ 430~~��������*/
	if(flag == 411)
	{
		gotoxy(1,7);
		sendstr1(buf[0]);
		cmd1(0x14);
	}
	if(flag == 412)
	{
		gotoxy(1,8);
		sendstr1(buf[1]);
		cmd1(0x15);
	}
	if(flag == 413)
	{
		gotoxy(1,9);
		sendstr1(buf[2]);
		cmd2(0x16);
	}
	
	if(flag == 420)
	{
		
		gotoxy(2,7);
		sendstr1(buf[0]);
		cmd1(0x14);	
	}
	if(flag == 421)
	{
		gotoxy(2,8);
		sendstr1(buf[1]);
		cmd1(0x15);
	}
	if(flag == 422)
	{
		gotoxy(2,9);
		sendstr1(buf[2]);
		cmd2(0x16);
	}
	
	if(flag == 430)
	{
		
		gotoxy(3,7);
		sendstr2(buf[0]);
		cmd2(0x14);	
	}
	if(flag == 431)
	{
		gotoxy(3,8);
		sendstr2(buf[1]);
		cmd2(0x15);
	}
	if(flag == 432)
	{
		gotoxy(3,9);
		sendstr2(buf[2]);
		cmd1(0x16);
	}
	
	if(flag == 71)
	{
		gotoxy(0,7);
		sendstr2(buf[0]);
		cmd2(0x15);
	}
	if(flag == 72)
	{
		gotoxy(0,8);
		sendstr2(buf[1]);
		cmd1(0x15);
	}
	
	if(flag == 4)// ��ʾ��������
    {
	   
		gotoxy(1,6);
		sendstr1(buf[0]);
		//gotoxy(1,11);
		sendstr1(" m/s ");
		gotoxy(2,6);
		sendstr1(buf[1]);
		//gotoxy(2,11);
		sendstr1(" m3/s");
		gotoxy(3,6);
		sendstr2(buf[2]);
		//gotoxy(3,11);
		data2(0xA1);//���϶�
		data2(0xE6);
    } 
    return ret;
                                
}
static long lcd_unlocked_ioctl (struct file *filp, unsigned int cmd, unsigned long arg)//ioctl
{
    long ret = 0; 
    switch(cmd)
    {
        case LCD2:
			lcd_cls();
            gotoxy(1,0);
			sendstr1(buf2[0]);
			gotoxy(2,0);
			sendstr1(buf2[1]);
			gotoxy(3,0);
			sendstr2(buf2[2]);
			gotoxy(4,0);
			sendstr2(buf2[3]);
			
            break;
        case LCD3:
            lcd_cls();
            gotoxy(1,0);
			sendstr1(buf3_1[0]);
			gotoxy(2,0);
			sendstr1(buf3_1[1]);
			gotoxy(3,0);
			sendstr2(buf3_1[2]);
			gotoxy(4,0);
			sendstr2(buf3_1[3]);
			
			break;
		case LCD3_1:
			lcd_cls();
            gotoxy(1,0);
			sendstr1(buf3_1[0]);
			gotoxy(2,0);  
			sendstr1(buf3_1[1]);
			gotoxy(3,0);
			sendstr2(buf3_1[2]);
			gotoxy(4,0);
			sendstr2(buf3_1[3]);
			
			flag = 4;
            break;
		case LCD4:
			lcd_cls();
            gotoxy(1,0);
			sendstr1(buf4[0]);
			gotoxy(2,0);
			sendstr1(buf4[1]);
			gotoxy(3,0);
			sendstr2(buf4[2]);
			gotoxy(4,0);
			sendstr2(buf4[3]);
            break;
		case LCD4_1:
			lcd_cls();
            gotoxy(1,0);
			sendstr1(buf4_1[0]);
			gotoxy(2,0);
			sendstr1(buf4_1[1]);
			gotoxy(3,0);
			sendstr2(buf4_1[2]);
			gotoxy(4,0);
			sendstr2(buf4_1[3]);
            break;
		case LCD4_2:
			lcd_cls();
            gotoxy(1,0);
			sendstr1(buf4_2[0]);
			gotoxy(2,0);
			sendstr1(buf4_2[1]);
			gotoxy(3,0);
			sendstr2(buf4_2[2]);
			gotoxy(4,0);
			sendstr2(buf4_2[3]);
            break;
		case LCD4_3:
			lcd_cls();
            gotoxy(1,0);
			sendstr1(buf4_3[0]);
			gotoxy(2,0);
			sendstr1(buf4_3[1]);
			gotoxy(3,0);
			sendstr2(buf4_3[2]);
			gotoxy(4,0);
			sendstr2(buf4_3[3]);
            break;
		case LCD41_11://��������
			gotoxy(1,7);
			cmd1(0x0F);//��꿪��
			cmd2(0x0C); 
			flag = 411;
			break;
		case LCD41_12://��������
			flag = 412;
			break;
		case LCD41_13://��������
			flag = 413;
			break;
		case LCD41_21://������
			gotoxy(2,7);
			cmd1(0x0F);//��꿪��
			cmd2(0x0C); 
			flag = 420;
			break;
		case LCD41_22://������
			flag = 421;
			break;
		case LCD41_23://������
			flag = 422;
			break;
		case LCD41_31://��������
			gotoxy(3,7);
			cmd1(0x0C);//��꿪��
			cmd2(0x0F); 
			flag = 430;
			break;
		case LCD41_32://��������
			flag = 431;
			break;	
		case LCD41_33://��������
			flag = 432;
			break;
		case LCD71_11://��ʱʱ��
			gotoxy(3,7);
			cmd1(0x0C);//��꿪��
			cmd2(0x0F); 
			flag = 71;
			break;
		case LCD71_12://��ʱʱ��
			flag = 72;
			break;
		case LCD5:
			lcd_cls();
            gotoxy(1,0);
			sendstr1(buf5[0]);
			gotoxy(2,0);
			sendstr1(buf5[1]);
			gotoxy(3,0);
			sendstr2(buf5[2]);
			gotoxy(4,0);
			sendstr2(buf5[3]);
            break;
		case LCD5_1:
			lcd_cls();
            gotoxy(1,0);
			sendstr1(buf5_1[0]);
			gotoxy(2,0);
			sendstr1(buf5_1[1]);
			gotoxy(3,0);
			sendstr2(buf5_1[2]);
			gotoxy(4,0);
			sendstr2(buf5_1[3]);
			break;
		case LCD5_2:
			lcd_cls();
            gotoxy(1,0);
			sendstr1(buf5_2[0]);
			gotoxy(4,0);
			sendstr2(buf5_2[3]);
            break;
		case LCD6:
			lcd_cls();
            gotoxy(1,0);
			sendstr1(buf6[0]);
			gotoxy(2,0);
			sendstr1(buf6[1]);
			gotoxy(3,0);
			sendstr2(buf6[2]);
			gotoxy(4,0);
			sendstr2(buf6[3]);
            break;
		case LCD6_1:
			lcd_cls();
            display(0,1);
            break;
		case LCD61_21://�����
			gotoxy(2,7);
			cmd1(0x0F);//��꿪��
			cmd2(0x0C); 
			flag = 1;
			break;
		case LCD61_22:
			flag = 2;  
            break;
		case LCD61_23:
			flag = 3;  
            break;	
		case LCD61_11://�ӵ����
			gotoxy(1,7);
			cmd1(0x0F); //��꿪��
			cmd2(0x0C); 
			flag = 5;
			break;
		case LCD61_12:
			flag = 6;  
            break;
		case LCD61_13:
			flag = 7;  
            break;
			
		case LCD_C_4://�˳�����
			cmd1(0x0C);
			cmd2(0x0C);
			break;
        default :
            break;
    }
    return ret;
}
static struct file_operations lcd_fops= {
    .owner  =   THIS_MODULE,    /* ����һ���꣬�������ģ��ʱ�Զ�����this_module���� */
    .open   =   lcd_open,
    .write  =  lcd_write,
    .release =   lcd_release,
	.unlocked_ioctl = lcd_unlocked_ioctl,
};

static int __init lcd_init(void)
{
    unsigned int val;
    int ret;
    int result;
    dev_t devno = MKDEV(major,minor);
    ret = register_chrdev_region(devno,1,"cd");
    if(ret < 0)
    {
        printk("failed get devno\n");
        return ret;
    }
    cdev_init(&mycdev,&lcd_fops);
    ret = cdev_add(&mycdev,devno,1);
    if(ret < 0)
    {
        printk("cdev_add failed \n");
        return ret;
    }
    lcd_class= class_create(THIS_MODULE, "lcd_t");
    device_create(lcd_class, NULL, devno, NULL,"lcd"); /* /dev/led_register */
   /*��������ΪIO ģʽ*/
    control = ioremap(CONTROL_MODULE,4);
    val = (ioread32(control + RST) & ~(0xff<<0)) | (0x07<<0);
    iowrite32(val,control+RST);
    val = (ioread32(control + CS) & ~(0xff<<0)) | (0x07<<0);
    iowrite32(val,control+CS);
    val = (ioread32(control + SID) & ~(0xff<<0)) | (0x07<<0);
    iowrite32(val,control+SID);
    val = (ioread32(control + SCLK1) & ~(0xff<<0)) | (0x07<<0);
    iowrite32(val,control+SCLK1);
    val = (ioread32(control + SCLK2) & ~(0xff<<0)) | (0x07<<0);
    iowrite32(val,control+SCLK2);

	val = (ioread32(control + GND) & ~(0xff<<0)) | (0x07<<0);//LCD����
    iowrite32(val,control+GND);
	val = (ioread32(control + LED_COM) & ~(0xff<<0)) | (0x07<<0);//LED_COM
    iowrite32(val,control+ LED_COM);
	val = (ioread32(control + LED_RUN) & ~(0xff<<0)) | (0x07<<0);//LED_RUN
    iowrite32(val,control+LED_RUN);
	
	val = (ioread32(control + LED_RUN) & ~(0xff<<0)) | (0x07<<0);//LED_RUN
    iowrite32(val,control+LED_RUN);
	
	/*������Դ  �� ���ó�ʼ״̬*/
    result = gpio_request(GPIO_TO_PIN(3,19), "rst");//RST
    if (result != 0)
        printk("gpio_request(3_14) failed!\n");
    result = gpio_direction_output(GPIO_TO_PIN(3,19), 1);
    if (result != 0)
        printk("gpio_direction(3_14) failed!\n");
    result = gpio_request(GPIO_TO_PIN(3,18), "cs");//CS
    if (result != 0)
        printk("gpio_request(0_3) failed!\n");
    result = gpio_direction_output(GPIO_TO_PIN(3,18), 0);
    if (result != 0)
        printk("gpio_direction(0_3) failed!\n");
    result = gpio_request(GPIO_TO_PIN(3,17), "sid");//SID
    if (result != 0)
        printk("gpio_request(3_17) failed!\n");
    result = gpio_direction_output(GPIO_TO_PIN(3,17), 0);
    if (result != 0)
        printk("gpio_direction(3_17) failed!\n");
    result = gpio_request(GPIO_TO_PIN(3,21), "sclk1");//SCLK1
    if (result != 0)
        printk("gpio_request(0_18) failed!\n");
    result = gpio_direction_output(GPIO_TO_PIN(3,21), 1);
    if (result != 0)
        printk("gpio_direction(0_18) failed!\n");
	result = gpio_request(GPIO_TO_PIN(3,14), "sclk2");//SCLK2
    if (result != 0)
        printk("gpio_request(0_19) failed!\n");
    result = gpio_direction_output(GPIO_TO_PIN(3,14), 1);
    if (result != 0)
        printk("gpio_direction(0_19) failed!\n");
	result = gpio_request(GPIO_TO_PIN(3,15), "gnd");//LCD_GND
    if (result != 0)
        printk("gpio_request(3_14) failed!\n");
	
	/*result = gpio_request(GPIO_TO_PIN(5,12), "led1");//LED_COM
    if (result != 0)
        printk("gpio_request(5_12) failed!\n");
	result = gpio_direction_output(GPIO_TO_PIN(5,12), 1);
    if (result != 0)
        printk("gpio_direction(5_12) failed!\n");
	result = gpio_request(GPIO_TO_PIN(5,13), "led2");//LED_RUN
    if (result != 0)
        printk("gpio_request(5_13) failed!\n");
	result = gpio_direction_output(GPIO_TO_PIN(5,13), 1);
    if (result != 0)
        printk("gpio_direction(5_13) failed!\n");*/
    printk("led_registerdriver load completed!\n");     
    return 0;
}

static void __exit lcd_exit(void)
{
    dev_t devno = MKDEV(major,minor);
    device_destroy(lcd_class,devno);
    class_destroy(lcd_class);
    
	gpio_free(GPIO_TO_PIN(3,14));
    gpio_free(GPIO_TO_PIN(3,15));
    gpio_free(GPIO_TO_PIN(3,21));
    gpio_free(GPIO_TO_PIN(3,18));
	gpio_free(GPIO_TO_PIN(3,19));
	gpio_free(GPIO_TO_PIN(3,17));
	/*gpio_free(GPIO_TO_PIN(5,12));
	gpio_free(GPIO_TO_PIN(5,13));*/

    cdev_del(&mycdev);
    unregister_chrdev_region(devno,1);

    printk("led_registerdriver unload completed!\n");   
}

module_init(lcd_init);
module_exit(lcd_exit); 
MODULE_LICENSE("GPL");
