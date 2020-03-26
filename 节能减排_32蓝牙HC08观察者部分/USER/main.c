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

//ALIENTEK ̽����STM32F407������ ��չʵ��1
//ATK-HC05��������ģ��ʵ�� -�⺯���汾
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK


void init()
{
	delay_init(168);      //��ʼ����ʱ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	usmart_dev.init(84); 		//��ʼ��USMART	
	OLED_Init();				//��ʼ��OLED	//x:0~127		y:0~63
	OLED_ShowString(0,0,"OLED init success!",12);
	OLED_Refresh();
	delay_ms(1000);
	HC05_Init();				//��ʼ����������
	USART3_RX_STA=0;
	OLED_ShowString(0,12,"HC-08 init success!",12);
	OLED_Refresh();
	delay_ms(1000);
	OLED_ShowString(0,36,"Started!",12);
	OLED_Refresh();
	delay_ms(2000);
	OLED_Clear();	
	//atָ���uart3��ʱ���������ͻ��С��ʹ�ã���ò�Ҫ�ڳ�������atָ�����֮��һ��Ҫ����
	//HC08:����Ҫ����ʲô������ƺ�����������ATָ���Ҫ\n\t
	//HC05_Set_Cmd("AT+DEFAULT");					//�ظ�Ĭ��
	//HC05_Set_Cmd("AT+CONT=1");//�������������ӣ���ֹ����
	//HC05_Set_Cmd("AT+ROLE=M");
	//HC05_Role_Show();
	//HC05_Set_Cmd("AT+CONT=0");
	
	//�����㲥ģʽ���ã�
	//������AT+ROLE=M������������AT+CONT=1�����á��������ӡ���AT+CINT=16,32����������ʱ�䣩AT+CLEAR����������ַ��
	//�ӻ���AT+ROLE=S�����ôӻ���AT+CONT=1�����á��������ӡ���
	//			AT+AINT=3200�����ù㲥���2s��AT+CINT=16,32����������ʱ��20ms~40ms��
	//			AT+MODE=1������ģʽ��?��ѯ��0Ĭ�ϣ�1�͹��ģ�2˯�ߣ�  1.2ģʽ����Ҫ�ȷ���һ�����ݲſɻ��ѣ�������������
	//			AT+AVDA=xxx
}
int main(void)
{ 
	init();
 	while(1)
	{	
		//��Ҫ��ѭ�����ж���ͬʱ����oled����һ����ʾ��һ��������жϣ��������ͻ�ģ���������
		delay_ms(500);//����������ʱ��Ҫ��Ȼoled�Ứ������֪��Ϊɶ������������oled��ʾ��һ�룬Ȼ����ж��ˣ���
	}											    
}

