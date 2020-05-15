#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "led.h"

#include "oled.h"

#include "usmart.h"		
#include "hc05.h" 	 
#include "usart3.h" 	
#include "key.h" 	 
#include "string.h"	 

#include "menu.h"

//ALIENTEK 探索者STM32F407开发板 扩展实验1
//ATK-HC05蓝牙串口模块实验 -库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK

int main(void)
{ 
	init();
 	while(1)
	{	
		//OLED_ShowNum(6*8,48,ad,3,12);
		//不要在循环跟中断中同时加入oled否则一旦显示到一半进入了中断，会产生冲突的，花屏乱码
		menu();
		delay_ms(500);//这里必须加延时，要不然oled会花屏，不知道为啥。。。。。	可能是oled显示到一半，然后就中断了？？
	}											    
}

