#include "delay.h"
#include "usart3.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  
#include "timer.h"

#include "oled.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F4开发板
//串口3驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/8/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//串口发送缓存区 	
__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 	//发送缓冲,最大USART3_MAX_SEND_LEN字节
#ifdef USART3_RX_EN   								//如果使能了接收   	  
//串口接收缓存区 	
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				//接收缓冲,最大USART3_MAX_RECV_LEN个字节.
u8 integer;
u8 decimal;
float ad=0;
u8 ad_RX_BUF[3];


//通过判断接收连续2个字符之间的时间差不大于100ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过100ms,则认为不是1次连续数据.也就是超过100ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度	 			//相当于设置好了一次最多接受15个字符
u16 USART3_RX_STA=0;   
int flag=0;	//判断收到a，开始计数
int i=0;		//计数333
int f=0;		//判断是at指令还是ad数据
//ad=0;flag=0;i=0;
	//ad_RX_BUF[0]=0;ad_RX_BUF[1]=0;ad_RX_BUF[2]=0;
void USART3_IRQHandler(void)		//定时器3中断，每10ms中断一次，记录一次数据
{
	u8 res=0;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收到数据
	{	 
 
	res =USART_ReceiveData(USART3);	
	//if((USART3_RX_STA&(1<<6))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据 
																//第六位为0就意味着没有接受完6个字符
	{ 
		//if(USART3_RX_STA<USART3_MAX_RECV_LEN)		//还可以接收数据
		{
			TIM_SetCounter(TIM7,0);//计数器清空        				 
			if(USART3_RX_STA==0)		
				TIM_Cmd(TIM7, ENABLE);  //使能定时器7 	10ms中断一次
			//假如还没收到a就收到数据，那么不会显示的
			if(res=='a'||f == 1)			//收到电压整数标志位	或者不是第一次收到a数据
			{
				f = 1;
				if(res=='a')			//收到电压整数标志位
			{
				i = 0;
				flag = 1;
				ad = (ad_RX_BUF[0]-'0') + (ad_RX_BUF[1]-'0')/10 + (ad_RX_BUF[2]-'0')/100; 
				ad_RX_BUF[0]=0;ad_RX_BUF[1]=0;ad_RX_BUF[2]=0;
			}
				else
			{
				if(flag == 1)
				{
					ad_RX_BUF[i] = res;		//记录接收到的值1=0.1.2，可以
					i++;
					if(i==3)
					{
//						OLED_ShowNum(0,36,ad_RX_BUF[0],1,12);
//						OLED_ShowChar(6,36,'.',12);
//						OLED_ShowNum(6*2,36,ad_RX_BUF[1],1,12);
//						OLED_ShowNum(6*3,36,ad_RX_BUF[2],1,12);
//						OLED_ShowChar(6*4,36,'V',12);
						
						//串口返回的以及发送的，默认都是按照字符的形式发出以及接收
						//我发送1，发出的其实是0x31，也就是十进制的49。。。。。
						//OLED_Clear();
						OLED_Clear();	
						OLED_ShowString(0,0,"Matser: Connect ",12);
						OLED_ShowString(0,12,"AD1:",12);
						OLED_ShowString(0,24,"AD2:",12);
						OLED_ShowString(0,36,"AD3:",12);
						//if(1)
							OLED_ShowString(0+6*(4+5+3),12,"safe!",12);
						OLED_ShowChar(4*6+0,12,ad_RX_BUF[0],12);
						OLED_ShowChar(4*6+6,12,'.',12);
						OLED_ShowChar(4*6+6*2,12,ad_RX_BUF[1],12);
						OLED_ShowChar(4*16+6*3,12,ad_RX_BUF[2],12);
						OLED_ShowChar(4*6+6*4,12,'V',12);
						OLED_Refresh();
						delay_ms(100);
						i=0;
					}
				}
			}
		}
			else
				f = 0;
		}
	}//else 
		//USART3_RX_STA|=1<<15;					//强制标记接收完成，15位全部清零
	} 
}  
		
		//设置好了一次最多接受15个字符
//	if((USART3_RX_STA&(1<<15))==0)//接收完的一批数据,还没有被处理,则不再接收其他数据
//	{ 
//		if(USART3_RX_STA<USART3_MAX_RECV_LEN)		//还可以接收数据
//		{
//			TIM_SetCounter(TIM7,0);//计数器清空        				 
//			if(USART3_RX_STA==0)		
//				TIM_Cmd(TIM7, ENABLE);  //使能定时器7 	10ms中断一次
//			USART3_RX_BUF[USART3_RX_STA++]=res;		//记录接收到的值	 
//		}else 
//		{
//			USART3_RX_STA|=1<<15;					//强制标记接收完成
//		} 
//	}  	

#endif	
//初始化IO 串口3
//bound:波特率	  
void usart3_init(u32 bound)
{  
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
 
	USART_DeInit(USART3);  //复位串口3
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE); //使能GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟
	
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10; //GPIOB11和GPIOB10初始化
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure); //初始化GPIOB11，和GPIOB10
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3); //GPIOB11复用为USART3
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3); //GPIOB10复用为USART3	  
	
	USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART3, &USART_InitStructure); //初始化串口3
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断  
		
	USART_Cmd(USART3, ENABLE);                    //使能串口 
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
	TIM7_Int_Init(100-1,8400-1);	//10ms中断一次
	
  TIM_Cmd(TIM7, DISABLE); //关闭定时器7
	
	USART3_RX_STA=0;				//清零 
}

//串口3,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char* fmt,...)  
{  
	u16 i,j;
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);//此次发送数据的长度
	for(j=0;j<i;j++)//循环发送数据
	{
	  while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);  //等待上次传输完成 
		USART_SendData(USART3,(uint8_t)USART3_TX_BUF[j]); 	 //发送数据到串口3 
	}
	
}


