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

//ALIENTEK ̽����STM32F407������ ��չʵ��1
//ATK-HC05��������ģ��ʵ�� -�⺯���汾
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK

int main(void)
{ 
	init();
 	while(1)
	{	
		//OLED_ShowNum(6*8,48,ad,3,12);
		//��Ҫ��ѭ�����ж���ͬʱ����oled����һ����ʾ��һ��������жϣ��������ͻ�ģ���������
		menu();
		delay_ms(500);//����������ʱ��Ҫ��Ȼoled�Ứ������֪��Ϊɶ����������	������oled��ʾ��һ�룬Ȼ����ж��ˣ���
	}											    
}

