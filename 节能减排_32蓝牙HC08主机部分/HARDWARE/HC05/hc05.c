#include "delay.h" 			 
#include "usart.h" 			 
#include "usart3.h" 			 
#include "hc05.h" 
#include "led.h" 
#include "string.h"	 
#include "math.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//ATK-HC05����ģ����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/10/26
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//********************************************************************************
//���޸���Ϣ							  
////////////////////////////////////////////////////////////////////////////////// 	   

//��ʼ��ATK-HC05ģ��
//����ֵ:0,�ɹ�;1,ʧ��.
u8 HC05_Init(void)
{
	u8 retry=10,t;		
	u8 temp=0;
	
	GPIO_InitTypeDef GPIO_InitStructure;
 
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��GPIOC,GPIOFʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //LED��Ӧ����state
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC0
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//KEY/en��Ӧ����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOC, &GPIO_InitStructure); //�����趨������ʼ��PF6

	GPIO_SetBits(GPIOC,GPIO_Pin_1);
	
	usart3_init(9600);	//��ʼ������3Ϊ:9600,������.͸����ʱ��ʹ��
	
//	while(retry--)
//	{
//		HC05_KEY=1;					//KEY�ø�,����ATģʽ
//		delay_ms(10);
//		//u3_printf("AT");		//����AT����ָ��
//		u3_printf("AT");		//����AT����
//		HC05_KEY=0;					//KEY����,�˳�ATģʽ
//		for(t=0;t<10;t++) 			//��ȴ�50ms,������HC05ģ��Ļ�Ӧ
//		{
//			if(USART3_RX_STA&0X8000)break;
//			delay_ms(5);
//		}		
//		if(USART3_RX_STA&0X8000)	//���յ�һ��������
//		{
//			temp=USART3_RX_STA&0X7FFF;	//�õ����ݳ���
//			USART3_RX_STA=0;			 
//			if(temp==4&&USART3_RX_BUF[0]=='O'&&USART3_RX_BUF[1]=='K')
//			{
//				temp=0;//���յ�OK��Ӧ
//				break;
//			}
//		}			    		
//	}		    
//	if(retry==0)temp=1;	//���ʧ��
	return temp;	 
}	 
//��ȡATK-HC05ģ��Ľ�ɫ
//����ֵ:0,�ӻ�;1,����;0XFF,��ȡʧ��.							  
u8 HC05_Get_Role(void)
{	 		    
	u8 retry=0X0F;
	u8 temp,t;
	while(retry--)
	{
		HC05_KEY=1;					//KEY�ø�,����ATģʽ
		delay_ms(10);
		u3_printf("AT+ROLE=?");	//��ѯ��ɫ
		for(t=0;t<20;t++) 			//��ȴ�200ms,������HC05ģ��Ļ�Ӧ
		{
			delay_ms(10);
			if(USART3_RX_STA&0X8000)break;	//һ�����յ����ݾ��˳�
		}		
		HC05_KEY=0;					//KEY����,�˳�ATģʽ
		if(USART3_RX_STA&0X8000)	//���յ�һ��������	1000000000000000	USART3_RX_STA��־λ:���յ������ݳ���
		{
			temp=USART3_RX_STA&0X7FFF;	//�õ����ݳ���  0111111111111111
			USART3_RX_STA=0;
			if(temp==6&&USART3_RX_BUF[0]=='M')	//�õ�����ģʽֵ  Master 6���ַ�
				temp = 1; //����
			else if(temp==5&&USART3_RX_BUF[0]=='S')
				temp = 0; //�ӻ�
		}
	}
	if(retry==0)temp=0XFF;//��ѯʧ��.		
	return temp;
} 							   
//ATK-HC05��������
//�˺�����������ATK-HC05,�����ڽ�����OKӦ���ATָ��
//atstr:ATָ�.����:"AT+RESET"/"AT+UART=9600,0,0"/"AT+ROLE=0"���ַ���
//����ֵ:0,���óɹ�;����,����ʧ��.							  
u8 HC05_Set_Cmd(u8* atstr)
{	 		    
	u8 retry=0X0F;
	u8 temp,t;
	while(retry--)
	{
		HC05_KEY=1;					//KEY�ø�,����ATģʽ
		delay_ms(10);
		u3_printf("%s",atstr);	//����AT�ַ���
		HC05_KEY=0;					//KEY����,�˳�ATģʽ
		for(t=0;t<20;t++) 			//��ȴ�100ms,������HC05ģ��Ļ�Ӧ
		{
			if(USART3_RX_STA&0X8000)break;
			delay_ms(5);
		}		
		if(USART3_RX_STA&0X8000)	//���յ�һ��������
		{
			temp=USART3_RX_STA&0X7FFF;	//�õ����ݳ���
			USART3_RX_STA=0;			 
			if(temp==4&&USART3_RX_BUF[0]=='O')//���յ���ȷ��Ӧ���� �յ�OK
			{			
				temp=0;
				break;			 
			}
		}		
	}
	if(retry==0)temp=0XFF;//����ʧ��.
	return temp;
} 
///////////////////////////////////////////////////////////////////////////////////////////////////
//ͨ���ú���,��������USMART,���Խ��ڴ���3�ϵ�ATK-HC05ģ��
//str:���.(����ע�ⲻ����Ҫ������س���)
void HC05_CFG_CMD(u8 *str)
{					  
	u8 temp;
	u8 t;		  
	HC05_KEY=1;						//KEY�ø�,����ATģʽ
	delay_ms(10);
	u3_printf("%s\r\n",(char*)str); //����ָ��
	for(t=0;t<50;t++) 				//��ȴ�500ms,������HC05ģ��Ļ�Ӧ
	{
		if(USART3_RX_STA&0X8000)break;
		delay_ms(10);
	}									    
	HC05_KEY=0;						//KEY����,�˳�ATģʽ
	if(USART3_RX_STA&0X8000)		//���յ�һ��������
	{
		temp=USART3_RX_STA&0X7FFF;	//�õ����ݳ���
		USART3_RX_STA=0;
		USART3_RX_BUF[temp]=0;		//�ӽ�����		 
		printf("\r\n%s",USART3_RX_BUF);//���ͻ�Ӧ���ݵ�����1
	} 				 
}














