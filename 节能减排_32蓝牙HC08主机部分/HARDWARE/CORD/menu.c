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
	delay_init(168);      //��ʼ����ʱ����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	usmart_dev.init(84); 		//��ʼ��USMART	
	OLED_Init();				//��ʼ��OLED	//x:0~127		y:0~63
	OLED_ShowString(0,0,"OLED init success!",12);
	OLED_Refresh();
	delay_ms(500);
	HC05_Init();				//��ʼ����������
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
	
	//������ԣ�
	//��ȫ�� AT+DEFAULT
	//������AT+ROLE=M������������AT+CONT=0�����á������ӡ���AT+CLEAR����������ַ��
	//�ӻ���AT+ROLE=S�����ôӻ���AT+CONT=0�����á������ӡ���
	
	//�����㲥ģʽ���ã�
	//������AT+ROLE=M������������AT+CONT=1�����á��������ӡ���AT+CINT=16,32����������ʱ�䣩AT+CLEAR����������ַ��
	//�ӻ���AT+ROLE=S�����ôӻ���AT+CONT=1�����á��������ӡ���
	//			AT+MODE=1������ģʽ��?��ѯ��0Ĭ�ϣ�1�͹��ģ�2˯�ߣ�  1.2ģʽ����Ҫ�ȷ���һ�����ݲſɻ���
	//			AT+AINT=3200�����ù㲥���2s��AT+CINT=16,32����������ʱ��20ms~40ms��
}

//���˵�
void menu()
{
	int i=0;
	GPIO_SetBits(GPIOD,GPIO_Pin_14);
	GPIO_SetBits(GPIOC,GPIO_Pin_8);
	GPIO_SetBits(GPIOC,GPIO_Pin_7);
	//ÿ�ν������˵���ֹͣ���ڽ��գ����˵���ʾ���֮���ٴ�
	USART_Cmd(USART3, DISABLE);                    //�رմ���
	OLED_Clear();	
	//���ڷ��ص��Լ����͵ģ�Ĭ�϶��ǰ����ַ�����ʽ�����Լ�����
	//�ҷ���1����������ʵ��0x31��Ҳ����ʮ���Ƶ�49�����������������Ҫ�����ַ�����ʾ
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
	
	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���
}

//��ʾATK-HC05ģ�������״̬
void HC05_Role_Show(void)
{
	if(HC05_Get_Role()==1)	
	{
		OLED_ShowString(0,12,"ROLE:Master ",12);	//����
		OLED_Refresh();
	}
	else		 		
	{
		OLED_ShowString(0,12,"ROLE:Slave ",12);		//�ӻ�
		OLED_Refresh();
	}
}
//��ʾATK-HC05ģ�������״̬
void HC05_Sta_Show(void)
{												 
	if(HC05_LED)	//���ӳɹ�
	{
		OLED_ShowString(0,24,"STA:Connected ",12);//6*12 ��ABC��
		OLED_Refresh();
	}
	else //δ����	
	{
		OLED_ShowString(0,24,"STA:Disconnect ",12);//6*12 ��ABC��
		OLED_Refresh();
	}
}	

