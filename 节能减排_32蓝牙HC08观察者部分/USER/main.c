#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "led.h"


#include "oled.h"
#include "bmp.h"

#include "usmart.h"		
#include "hc05.h" 	 
#include "usart3.h" 	
#include "key.h" 	 
#include "string.h"	 

//ALIENTEK 探索者STM32F407开发板 扩展实验1
//ATK-HC05蓝牙串口模块实验 -库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK


void init()
{
	delay_init(168);      //初始化延时函数
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	uart_init(115200);		//初始化串口波特率为115200
	usmart_dev.init(84); 		//初始化USMART	
	OLED_Init();				//初始化OLED	//x:0~127		y:0~63
	OLED_ShowString(0,0,"OLED init success!",12);
	OLED_Refresh();
	delay_ms(1000);
	HC05_Init();				//初始化蓝牙串口
	USART3_RX_STA=0;
	OLED_ShowString(0,12,"HC-08 init success!",12);
	OLED_Refresh();
	delay_ms(1000);
	OLED_ShowString(0,36,"Started!",12);
	OLED_Refresh();
	delay_ms(2000);
	OLED_Clear();	
	//at指令跟uart3定时器会产生冲突，小心使用，最好不要在程序里用at指令，用了之后一定要重启
	//HC08:不需要密码什么的配对似乎。。。并且AT指令不需要\n\t
	//HC05_Set_Cmd("AT+DEFAULT");					//回复默认
	//HC05_Set_Cmd("AT+CONT=1");//先让他不可链接，防止干扰
	//HC05_Set_Cmd("AT+ROLE=M");
	//HC05_Role_Show();
	//HC05_Set_Cmd("AT+CONT=0");
	
	//蓝牙广播模式设置：
	//主机：AT+ROLE=M（设置主机）AT+CONT=1（设置“不可连接”）AT+CINT=16,32（设置连接时间）AT+CLEAR（清除记忆地址）
	//从机：AT+ROLE=S（设置从机）AT+CONT=1（设置“不可连接”）
	//			AT+AINT=3200（设置广播间隔2s）AT+CINT=16,32（设置连接时间20ms~40ms）
	//			AT+MODE=1（设置模式：?查询，0默认，1低功耗，2睡眠）  1.2模式，需要先发送一个数据才可唤醒，故最后设置这个
	//			AT+AVDA=xxx
}
int main(void)
{ 
	init();
 	while(1)
	{	
		//不要在循环跟中断中同时加入oled否则一旦显示到一半进入了中断，会产生冲突的，花屏乱码
		delay_ms(500);//这里必须加延时，要不然oled会花屏，不知道为啥。。。可能是oled显示到一半，然后就中断了？？
	}											    
}

