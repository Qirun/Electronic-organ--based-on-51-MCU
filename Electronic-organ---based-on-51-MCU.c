#include <reg51.h>
#define uchar unsigned char
#define uint unsigned int
sbit speak=P1^0;
sbit k1=P2^0;
sbit k2=P2^1;
sbit k3=P2^2;
sbit k4=P2^3;
sbit k5=P2^4;
sbit k6=P2^5;
sbit k7=P2^6;
sbit k8=P2^7;
sbit b1=P3^3;
sbit b2=P3^6;

char temp;
uchar code table[]={0xd7,0x32,0x52,0xd4,0x58,0x18,0xd3,0xc7,0x11};
uchar s=0;//用来控制数组内的数
uint code note[]={64580,64684,64777,64820,64898,64968,65030,65058};//此数组只有一个八度


unsigned char timer0h, timer0l, time;
// 频率-半周期数据表 高八位  四个八度的28个频率数据
code unsigned char FREQH[] = {
    0xF2, 0xF3, 0xF5, 0xF5, 0xF6, 0xF7, 0xF8,    //低音1234567
    0xF9, 0xF9, 0xFA, 0xFA, 0xFB, 0xFB, 0xFC, 0xFC,//1,2,3,4,5,6,7,i
    0xFC, 0xFD, 0xFD, 0xFD, 0xFD, 0xFE,            //高音 234567
    0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF};   //超高音 1234567
// 频率-半周期数据表 低八位

code unsigned char FREQL[] = {
    0x42, 0xC1, 0x17, 0xB6, 0xD0, 0xD1, 0xB6,    //低音1234567
    0x21, 0xE1, 0x8C, 0xD8, 0x68, 0xE9, 0x5B, 0x8F, //1,2,3,4,5,6,7,i
    0xEE, 0x44, 0x6B, 0xB4, 0xF4, 0x2D,             //高音 234567
    0x47, 0x77, 0xA2, 0xB6, 0xDA, 0xFA, 0x16};   //超高音 1234567

code unsigned char BB[]={6,1,2,  3,2,2,  3,2,1,  2,2,1,  3,2,2,  2,2,1,  1,2,1,  2,2,2,  2,2,1,  1,2,1,   6,1,1,  1,2,1,  
						 6,1,2,  3,2,2,  3,2,1,  2,2,1,  3,2,2,  2,2,1,  1,2,1,  2,2,2,  2,2,1,  1,2,1,   6,1,1,  1,2,1,  
						 6,1,2,  3,2,2,  3,2,1,  2,2,1,  3,2,2,  2,2,1,  1,2,1,  2,2,2,  2,2,1,  1,2,1,   6,1,1,  1,2,1,};
						 //the next episode 主旋律

void t0int() interrupt 1         //T0中断程序，控制发音的音调
{
    TR0 = 0;                     
    speak= !speak;          	 
    TH0 = timer0h;               //下次的中断时间, 这个时间, 控制音调高低
    TL0 = timer0l;
    TR0 = 1;                     
}

void int0() interrupt 3
{
	TH1=note[s]/256;
	TL1=note[s]%256;
	temp=!temp;
}

void delays(unsigned char t)     //延时程序，控制发音的时间长度
{
    unsigned char t1;
    unsigned long t2;
    for(t1 = 0; t1 < t; t1++)    //共延时t个半拍
      for(t2 = 0; t2 < 8000; t2++); 
    TR0 = 0;                        //关闭T0, 停止发音
}


void song()                      //演奏一个音符
{
    TH0 = timer0h;               //控制音调
    TL0 = timer0l;
    TR0 = 1;                     
    delays(time);                 //控制时间长度
}
//--------------------------------------




void delay(uchar t)
{
  unsigned char i;
  while(t--)
   {
    for(i=0;i<120;i++);
   }
}


uint scan()//键盘扫描函数
{	
	uchar t;
	b1=1;b2=1;
	do
	{
		if(b1==0)  //防抖动
		{	
			delay(5);
			if(b1==0)
			{
				t=1;
			}
		}
		if(b2==0)
		{
			delay(5);
			if(b2==0)
			{
				t=2;
			}
		}
	}  
	while(k1!=0&&k2!=0);
	return t;
}

void init()
{
	TMOD=0x10;
	TH1=note[0]/256;
	TL1=note[0]%256;
	TR1=1;
	ET1=1;
	EA=1;	
}

void mode1()
{
	unsigned char k, i;
    TMOD = 1;                   
    ET0 = 1;                     
    EA = 1;                      
    while(1)
	{
	    i = 0;
	    time = 1; 
	    while(time) 
		{
	        k = BB[i] + 7 * BB[i + 1] - 1;//第i个是音符, 第i+1个是第几个八度
	        timer0h = FREQH[k];      //从数据表中读出频率数值
	        timer0l = FREQL[k];      //定时的时间长度
	        time = BB[i + 2];   
	        i += 3;
	        song();     
		}   
	} 
}

void mode2()
{
	init();
	while(1)
	{
		if(k1==0)
		{
			delay(2);
			s=0;
			while(k1==0)
			{
				P0=table[s];
				speak=temp;	
			}
			P0=table[8];
		}
		if(k2==0)
		{
			
			delay(2);
			s=1;
			while(k2==0)
			{
				P0=table[s];
				speak=temp;	
			}
			P0=table[8];
		}
		if(k3==0)
		{
			
			delay(2);
			s=2;
			while(k3==0)
			{
				P0=table[s];
				speak=temp;	
			}
			P0=table[8];
		}
		if(k4==0)
		{
			delay(2);
			s=3;
			while(k4==0)
			{
				P0=table[s];
				speak=temp;	
			}
			P0=table[8];
		}
		if(k5==0)
		{
			
			delay(2);
			s=4;
			while(k5==0)
			{
				P0=table[s];
				speak=temp;	
			}
			P0=table[8];
		}
		if(k6==0)
		{
			
			delay(2);
			s=5;
			while(k6==0)
			{
				P0=table[s];
				speak=temp;	
			}
			P0=table[8];
		}
		if(k7==0)
		{
			
			delay(2);
			s=6;
			while(k7==0)
			{
				P0=table[s];
				speak=temp;	
			}
			P0=table[8];
		}
		if(k8==0)
		{
			
			delay(2);
			s=7;
			while(k8==0)
			{
				P0=table[s];
				speak=temp;	
			}
			P0=table[8];
		}
	}				
}

void main(void)
{
   while(1)
   {
   		if(b1==0)
		{
			mode1();
		}
		else if(b2==0)
		{
			mode2();
		}
   }
}
