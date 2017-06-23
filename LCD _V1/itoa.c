#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/***翻转字符串*****/
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
    static char s[100];//必须为static变量，或者是全局变量
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
    
   // printf("s = %s\n",reverse(s));
   return reverse(s);
}

int main(void)
{
    int i=0,j=0;
    int m[5]={12,21,321,432,13};
    char * buf[]={"xxxx","xaaa","xzzz","xccc","xvvv"};
    for(i = 0;i<5;i++)  //吧转换的字符串赋给数组指针
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
