#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/***��ת�ַ���*****/
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
    static char s[100];//����Ϊstatic������������ȫ�ֱ���
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
    
   // printf("s = %s\n",reverse(s));
   return reverse(s);
}

int main(void)
{
    int i=0,j=0;
    int m[5]={12,21,321,432,13};
    char * buf[]={"xxxx","xaaa","xzzz","xccc","xvvv"};
    for(i = 0;i<5;i++)  //��ת�����ַ�����������ָ��
    {
        buf[i] = (char *)malloc(sizeof(char)*4);
           // buf[0]  = my_itoa(m[0]);
        //buf=my_itoa(m[i]);
         memcpy((*(buf+i)),my_itoa(m[i]),strlen(my_itoa(m[i]))+1);
    }
        
        printf("str =%s ,",*(buf+0));
        printf("str =%s ,",*(buf+1));
        printf("str =%s ,",*(buf+2));
        printf("str =%s ,",*(buf+3));
        printf("str =%s ,",*(buf+4));
        printf("\n");

        return 0;
}
