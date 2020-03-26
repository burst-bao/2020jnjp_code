#include <msp430.h>

#define uint unsigned int //定义常用数据类型替代码
#define uchar unsigned char
#define ulong unsigned long int
//#define CPU_F ((double)8000000)
//#define CPU_F ((double)32000)
#define CPU_F ((double)1048576)  //MCLK=SMCLK 默认上电频率
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))

void HC05_init()
{
  P3SEL = BIT3+BIT4;                        // P3.4,5 = USCI_A0 TXD/RXD

  UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
  UCA0CTL1 |= UCSSEL_1;                     // CLK = ACLK
  UCA0BR0 = 0x03;                           // 32kHz/9600=3.41 (see User's Guide)
  UCA0BR1 = 0x00;                           //
  UCA0MCTL = UCBRS_3+UCBRF_0;               // Modulation UCBRSx=3, UCBRFx=0
  UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
  UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

// P1.0 = 0  //灭灯  = 1,亮灯
void LED_init()
{
    P1DIR |= BIT0;                            //设置P1.0为输出
    //P1DIR |= 0x01;
    P1OUT &= ~BIT0;                       // P1.0 = 0，灭灯
}

void ADC_init()
{
      P6SEL = 0x0F;                             // Enable A/D channel inputs
      //ADC12ON:    ADC12 On/enable
      //ADC12MSC:   ADC12 Multiple SampleConversion
      //ADC12SHT0_8:ADC12 Sample Hold 0 Select Bit: 8
      ADC12CTL0 = ADC12ON+ADC12MSC+ADC12SHT0_8; // Turn on ADC12, extend sampling time
                                                // to avoid overflow of results
      //ADC12SHP:       ADC12 Sample/Hold Pulse Mode
      //ADC12CONSEQ_3:  ADC12 Conversion Sequence Select: 3
      ADC12CTL1 = ADC12SHP+ADC12CONSEQ_3;       // Use sampling timer, repeated sequence
      ADC12MCTL0 = ADC12INCH_0;                 // ref+=AVcc, channel = A0
      ADC12MCTL1 = ADC12INCH_1;                 // ref+=AVcc, channel = A1
      ADC12MCTL2 = ADC12INCH_2;                 // ref+=AVcc, channel = A2
      ADC12MCTL3 = ADC12INCH_3+ADC12EOS;        // ref+=AVcc, channel = A3, end seq.
      ADC12IE = 0x08;                           // Enable ADC12IFG.3
      ADC12CTL0 |= ADC12ENC;                    // Enable conversions
      ADC12CTL0 |= ADC12SC;                     // Start convn - software trigger
}

#define   Num_of_Results   8

volatile unsigned int A0results[Num_of_Results];
volatile unsigned int A1results[Num_of_Results];
volatile unsigned int A2results[Num_of_Results];
volatile unsigned int A3results[Num_of_Results];
unsigned long int sum;
uint ad;
uchar table[14]={"0123456789V.ab"};
int qian,bai,shi,ge;

int main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer
  int i=10;
  HC05_init();
  LED_init();
  while(i--)        //先发送一段数据激活
  {
      UCA0TXBUF = 0xFF;
      delay_ms(10);
  }
  i = 10;
  delay_ms(100);    //延时之后，发送该发送的数据
  ADC_init();

  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, Enable interrupts
  __no_operation();                         // For debugger

}

//USCI A0 Receive/Transmit中断服务函数，必须要有，就算是空的
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
  switch(__even_in_range(UCA0IV,4))
  {
  case 0:break;                             // Vector 0 - no interrupt
  case 2:                                   // Vector 2 - RXIFG
    //while (!(UCA0IFG&UCTXIFG));             // USCI_A0 TX buffer ready?
    //UCA0TXBUF = UCA0RXBUF;                  // TX -> RXed character
    //P1OUT ^= BIT0;
    break;
  case 4:break;                             // Vector 4 - TXIFG
  default: break;
  }
}

//#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
//#elif defined(__GNUC__)
//void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12ISR (void)
//#else
//#error Compiler not supported!
//#endif
{
  static unsigned int index = 0;
  //int i=10;
  switch(__even_in_range(ADC12IV,34))
  {
  case  0: break;                           // Vector  0:  No interrupt
  case  2: break;                           // Vector  2:  ADC overflow
  case  4: break;                           // Vector  4:  ADC timing overflow
  case  6: break;                           // Vector  6:  ADC12IFG0
  case  8: break;                           // Vector  8:  ADC12IFG1
  case 10: break;                           // Vector 10:  ADC12IFG2
  case 12:                                  // Vector 12:  ADC12IFG3
      if(index < 8)
      {
          A0results[index] = ADC12MEM0;           // Move A0 results, IFG is cleared
          A1results[index] = ADC12MEM1;           // Move A1 results, IFG is cleared
          A2results[index] = ADC12MEM2;           // Move A2 results, IFG is cleared
          A3results[index] = ADC12MEM3;           // Move A3 results, IFG is cleared
          index++;                                // Increment results index, modulo; Set Breakpoint1 here
      }
      else if(index == 8)
      {
          for(index=0;index<8;index++)
              sum += A0results[index];
          sum /= 8;     //八次采样滤波
          ad = (sum*3.3*100)/4095;  //两位小数，转化成三位数

          //qian=ad/1000;
          bai=(ad/100)%10;
          shi=(ad/10)%10;
          ge=ad%10;
          __no_operation();
          /*
          while(i--)        //先发送一段数据激活
          {
              UCA0TXBUF = 0xFF;
              delay_ms(10);
          }
          i = 10;
          delay_ms(100);    //延时之后，发送该发送的数据
          */
          UCA0TXBUF = 'A';  //AT+AVDA=xxx（设置广播数据）
          delay_ms(10);
          UCA0TXBUF = 'T';
          delay_ms(10);
          UCA0TXBUF = '+';
          delay_ms(10);
          UCA0TXBUF = 'A';
          delay_ms(10);
          UCA0TXBUF = 'V';//a开头
          delay_ms(10);
          UCA0TXBUF = 'D'; //整数部分  为啥我数字发送出去，那一边收到的是字符。。。按照字符的形式默认发送吗。。
          delay_ms(10);
          UCA0TXBUF = 'A';
          delay_ms(10);
          UCA0TXBUF = '=';
          delay_ms(10);
          UCA0TXBUF = table[12];//a开头
          delay_ms(10);
          UCA0TXBUF = table[bai]; //整数部分  为啥我数字发送出去，那一边收到的是字符。。。按照字符的形式默认发送吗。。
          delay_ms(10);
          UCA0TXBUF = table[shi];
          delay_ms(10);
          UCA0TXBUF = table[ge];
          delay_ms(10);
          UCA0TXBUF = table[2];    //最后一位，AD通道校验位, table[2]='2'
          delay_ms(100);
          __no_operation();
          /*
          UCA0TXBUF = table[12];//a开头
          delay_ms(20);
          UCA0TXBUF = table[bai]; //整数部分  为啥我数字发送出去，那一边收到的是字符。。。按照字符的形式默认发送吗。。
          delay_ms(20);
          UCA0TXBUF = table[shi];
          delay_ms(20);
          UCA0TXBUF = table[ge];
          delay_ms(100);
          */
          //通过判断接收连续2个字符之间的时间差不大于100ms来决定是不是一次连续的数据.
          //如果2个字符接收间隔超过100ms,则认为不是1次连续数据.也就是超过100ms没有接收到
          //任何数据,则表示此次接收完毕.

          index = 0;
          sum = 0;
      }
        break;
  case 14: break;                           // Vector 14:  ADC12IFG4
  case 16: break;                           // Vector 16:  ADC12IFG5
  case 18: break;                           // Vector 18:  ADC12IFG6
  case 20: break;                           // Vector 20:  ADC12IFG7
  case 22: break;                           // Vector 22:  ADC12IFG8
  case 24: break;                           // Vector 24:  ADC12IFG9
  case 26: break;                           // Vector 26:  ADC12IFG10
  case 28: break;                           // Vector 28:  ADC12IFG11
  case 30: break;                           // Vector 30:  ADC12IFG12
  case 32: break;                           // Vector 32:  ADC12IFG13
  case 34: break;                           // Vector 34:  ADC12IFG14
  default: break;
  }
}
