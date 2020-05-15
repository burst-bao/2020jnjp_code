#include "oled.h"
#include "stdlib.h"
//#include "oledfont.h" 
//#include "bmp.h"
#include "delay.h"

#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "led.h"

#include "usmart.h"		
#include "hc05.h" 	 
#include "usart3.h" 
#include "key.h" 	 
#include "string.h"

#include "menu.h"

void init()
{
	int i,j;
	delay_init(168);      //初始化延时函数
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	uart_init(115200);		//初始化串口波特率为115200
	usmart_dev.init(84); 		//初始化USMART	
	OLED_Init();				//初始化OLED	//x:0~127		y:0~63
	OLED_ShowString(0,0,"OLED init success!",12);
	OLED_Refresh();
	delay_ms(500);
	HC05_Init();				//初始化蓝牙串口
	//USART3_RX_STA=0;
	OLED_ShowString(0,12,"HC-08 init success!",12);
	OLED_Refresh();
	delay_ms(500);
	LED_Init();
	OLED_ShowString(0,24,"LED init success!",12);
	OLED_Refresh();
	delay_ms(500);
	OLED_ShowString(0,36,"Started!",12);
	OLED_Refresh();
	delay_ms(1000);
	OLED_Clear();	
	
	for(i=0;i<3;i++)
		ad_RX_BUF[i]=0;
		for(j=0;i<3;i++)
			ad_BUF[i][j]=0;
	
	//蓝牙配对：
	//先全部 AT+DEFAULT
	//主机：AT+ROLE=M（设置主机）AT+CONT=0（设置“可连接”）AT+CLEAR（清除记忆地址）
	//从机：AT+ROLE=S（设置从机）AT+CONT=0（设置“可连接”）
	
	//蓝牙广播模式设置：
	//主机：AT+ROLE=M（设置主机）AT+CONT=1（设置“不可连接”）AT+CINT=16,32（设置连接时间）AT+CLEAR（清除记忆地址）
	//从机：AT+ROLE=S（设置从机）AT+CONT=1（设置“不可连接”）
	//			AT+MODE=1（设置模式：?查询，0默认，1低功耗，2睡眠）  1.2模式，需要先发送一个数据才可唤醒
	//			AT+AINT=3200（设置广播间隔2s）AT+CINT=16,32（设置连接时间20ms~40ms）
}

//主菜单
void menu()
{
	int i=0;
	GPIO_SetBits(GPIOD,GPIO_Pin_14);
	GPIO_SetBits(GPIOC,GPIO_Pin_8);
	GPIO_SetBits(GPIOC,GPIO_Pin_7);
	//每次进入主菜单先停止串口接收，主菜单显示完毕之后再打开
	USART_Cmd(USART3, DISABLE);                    //关闭串口
	OLED_Clear();	
	//串口返回的以及发送的，默认都是按照字符的形式发出以及接收
	//我发送1，发出的其实是0x31，也就是十进制的49。。。。。因此我们要按照字符来显示
	if(connected)
		OLED_ShowString(0,0,"Matser: Connect ",12);
	else
		OLED_ShowString(0,0,"Matser:",12);
	
	OLED_ShowString(0,12,"AD1:",12);
	OLED_ShowString(0,24,"AD2:",12);
	OLED_ShowString(0,36,"AD3:",12);
	
	for(i=0;i<3;i++)
	{
		OLED_ShowChar(4*6+6*0,12+12*i,ad_BUF[i][0],12);
		OLED_ShowChar(4*6+6*1,12+12*i,'.',12);
		OLED_ShowChar(4*6+6*2,12+12*i,ad_BUF[i][1],12);
		OLED_ShowChar(4*6+6*3,12+12*i,ad_BUF[i][2],12);
		OLED_ShowChar(4*6+6*4,12+12*i,'V',12);
	
		if(ad_123[i]<0.5)
		{
			OLED_ShowString(0+6*(4+5+3),12+i*12,"safe!",12);
			//GPIO_ResetBits(GPIOC,GPIO_Pin_7);
		}
		else if(ad_123[i]>=1 && ad_123[i]<3)
		{
			OLED_ShowString(0+6*(4+5+3),12+i*12,"normal!",12);
			//GPIO_ResetBits(GPIOC,GPIO_Pin_8);
		}
		else if(ad_123[i]>=3)
		{
			OLED_ShowString(0+6*(4+5+3),12+i*12,"warning!",12);
			//GPIO_ResetBits(GPIOD,GPIO_Pin_14);
		}
	}
	
	if(ad_123[1]<0.5)
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_7);
	}
	else if(ad_123[1]>=1 && ad_123[1]<3)
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_8);
	}
	else if(ad_123[1]>=3)
	{
		GPIO_ResetBits(GPIOD,GPIO_Pin_14);
	}
	
	if(ad_123[0]<0.5 && ad_123[1]<0.5 && ad_123[2]<0.5)
	{
		OLED_ShowString(0,48,"all safe!!",12);
		//GPIO_ResetBits(GPIOC,GPIO_Pin_7);
	}
	else
	{
		OLED_ShowString(0,48,"warniing!!",12);
		//GPIO_ResetBits(GPIOD,GPIO_Pin_14);
	}
	OLED_Refresh();
	
	USART_Cmd(USART3, ENABLE);                    //使能串口
}

//显示ATK-HC05模块的主从状态
void HC05_Role_Show(void)
{
	if(HC05_Get_Role()==1)	
	{
		OLED_ShowString(0,12,"ROLE:Master ",12);	//主机
		OLED_Refresh();
	}
	else		 		
	{
		OLED_ShowString(0,12,"ROLE:Slave ",12);		//从机
		OLED_Refresh();
	}
}
//显示ATK-HC05模块的连接状态
void HC05_Sta_Show(void)
{												 
	if(HC05_LED)	//连接成功
	{
		OLED_ShowString(0,24,"STA:Connected ",12);//6*12 “ABC”
		OLED_Refresh();
	}
	else //未连接	
	{
		OLED_ShowString(0,24,"STA:Disconnect ",12);//6*12 “ABC”
		OLED_Refresh();
	}
}	

