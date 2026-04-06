#include<lpc21xx.h>
#include "project.h"
//data types
typedef  unsigned char u8;
typedef unsigned int u32;
typedef float u32f;
typedef int s32;
void uart0_init(u32 baud)
{
        int pclk,result=0,a[]={15,60,30};
        PINSEL0|=0x5;//p0.0 0 1 as txdo and  p0.1 0 1 as rxdo
        pclk=a[VPBDIV]*1000000;
        result=pclk/(16*baud);
        U0LCR=0x83;
        U0DLM=(result>>8)&0xff;
        U0DLL=result&0xff;
        U0LCR=0x3;
}

#define THRE (U0LSR>>5&1)
void uart0_tx(u8 data)
{
        U0THR=data;
        while(THRE==0);
}
#define RBR (U0LSR&1)
u8 uart0_rx()
{
  while(RBR==0);
        return U0RBR;
}
void uart0_string(u8 *ptr)
{
        while(*ptr!=0)
        {
                uart0_tx(*ptr);
                ptr++;
        }
}
void uart0_bin(int num)
{
int pos,temp;
        for(pos=7;pos>=0;pos--)
        {
                temp=(num>>pos&1);
                uart0_tx(temp+48);
        }
}
void uart0_rx_string(char *ptr,int len)
{


}
void uart0_integer(int num)
{
int a[10],i=0;
        if(num < 0)
        {
                uart0_tx('-');
                num=-num;
        }
        if(num==0)
        {
        uart0_tx('0');
                return;
        }
        while(num>0)
        {
                a[i]=(num%10+48);
                num/=10;
                i++;
        }
        for(i=i-1;i>=0;i--)
        uart0_tx(a[i]);
}

