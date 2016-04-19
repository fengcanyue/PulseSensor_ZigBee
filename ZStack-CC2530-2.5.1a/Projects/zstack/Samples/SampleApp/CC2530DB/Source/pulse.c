//包含头文件
//ADC输入引脚为P0_3
#include "iocc2530.h"
#include "stdio.h"
#include "hal_adc.h"
#include "MT_UART.h"

#define false 0
#define true  1


#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)


//#define ADC_REF_AVDD        0x80    //adc参考电压3.3v
//#define ADC_10_BIT          0x20     //adc分辨率10位
//#define ADC_PULSE_SENS      0x04   //adc输入P0_4
//#define ADC_SINGLE_CONVERSION(settings) \
//   do{ ADCCON3 = (settings); }while(0) //adc设置参考电压，分辨率，输入引脚
//#define ADC_START_SINGLE() \
//  do { ADC_START_MODE(); ADCCON1 |= 0x40;  } while (0)//adc手动启动
//#define ADC_SAMPLE_READY()  (ADCCON1 & 0x80) //adc转换结束，为下一次做准备
//#define ADC_START_MODE() \
//  do { ADCCON1 |= 0x30; } while (0) //adc启动方式选择为手动触发


//全局变量
//char T3Flag=0;
unsigned char PulsePin = 4; //p0_4
unsigned char PulseArr[4];
// these variables are volatile because they are used during the interrupt service routine!
 unsigned int BPM;                   // used to hold the pulse rate
 unsigned int Signal;                // holds the incoming raw data
 unsigned int IBI = 600;             // holds the time between beats, must be seeded!心跳间隔 
 unsigned char Pulse = false;     // true when pulse wave is high, false when it's  unsigned char QS = false;        
 unsigned char QS = false;    // becomes true when Arduoino finds a beat.
  
 int rate[10];                    // array to hold last ten IBI values
 unsigned long sampleCounter = 0;          // used to determine pulse timing
 unsigned long lastBeatTime = 0;           // used to find  int Peak =512;     // used to find peak in pulse wave, seeded	  int Trough = 512;                     // used to find trough in pulse wave, seeded	波谷
 int Peak =512; 
 int Trough = 512;  
 int thresh = 512;                // used to find instant moment of heart beat, seeded 	心跳瞬间
 int amp = 100;                   // used to hold amplitude of pulse waveform, seeded 	振幅
 unsigned char firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
 unsigned char secondBeat = false;      // used to seed rate array so we startup with reasonable BPM

 //函数声明
 void T3_init(void);// 16位定时器计数器，2ms触发一次，开启中断
// unsigned int analogRead(unsigned char channel);//读取A/D转换数据，再重新启动
 unsigned char getPulseArr(unsigned int dat);
 void pulse_test(void);
 void sendDataToProcessing(char symbol, unsigned int dat );
__interrupt void T3_ISR(void) ;//定时器1中断函数


//串口初始化
//void UART_init(void)
//{
//  PERCFG &=0xFE;//把这个寄存器的第零位强行清零  1111 1110 
//  //就是把串口0的脚位置配置在备用位置1 即P0_2  P0_3
//  P0SEL  |=0x0C;//让P0_2  P0_3这两个脚工作在片上外设模式,而不是普通IO口       0000 1100
//  U0CSR |=0xC0;
//  U0UCR =0; //串口0 典型的串口配置  校验位 停止位之类的东西
//  U0GCR =11;
//  U0BAUD =216;//就是重官方数据手册中波特率表格中参照115200时的 配置值，前提是系统时钟在32M
//  IEN0 |=0x04; //开接收数据的中断  0000 0100
//  EA=1;
//}

//发字符
//char Putchar(unsigned char dat)
//{
//  UTX0IF=0;//串口发送完成标志位清零
//  U0DBUF=dat;//将ch中存放的数据发送出去dat;
//  while(!UTX0IF);
//  UTX0IF=0;
//  return U0DBUF;
//}
//获取脉搏数组的函数
unsigned char getPulseArr(unsigned int dat)
{
  unsigned char len;
  len=sprintf((char*)PulseArr,"%d",dat);
  return len;
}
//延时函数
//void delay(unsigned int n) //延时us
//{
//  MicroWait(k);	
//  //unsigned int i,j;
//	//for(i=0;i<n;i++)
//		//for(j=0;j<100;j++);
//}
//发送字符串
//void sendDataToProcessing(char symbol, unsigned int dat ){
//  char  buf[5];
//  int len;
//  len=sprintf(buf,"%d",dat);
//  Putchar(symbol);                // symbol prefix tells Processing what type of data is coming
//  UartTX_Send_String(buf, len);
//}
void pulse_test(void)
{
    sendDataToProcessing('S', Signal);     // send Processing the raw Pulse Sensor data
    if (QS == true)
    {          
      // Quantified Self flag is true when arduino finds a heartbeat
      sendDataToProcessing('B',BPM);   // send heart rate with a 'B' prefix
      sendDataToProcessing('Q',IBI);   // send time between beats with a 'Q' prefix
      //printf("%d\n",BPM);
      QS = false; 
    }// reset the Quantified Self flag for next time 
    delay(137);                             //  take a break 19.6ms
  }
void sendDataToProcessing(char symbol, unsigned int dat ){
  char  buf[5],len;
  HalUARTWrite(0,(unsigned char *)&symbol,1);
  len=sprintf(buf,"%d",dat);
  HalUARTWrite(0,(unsigned char *)buf,len);
}
//定时器初始化(CLKCONCMD.TICKSPD为定时器提供滴答时钟，默认为001，即16MHz)
void T3_init()
{     
      T3CTL |= 0x08 ;             //开溢出中断  00001000   
      T3IE = 1;                   //开总中断和T3中断
      T3CTL|=0XE0;               // 11100000 128分频,128/16000000*N=0.002S,N=250              
      T3CTL |= 0X02;            //模计数方式，0x00到T3CC0
      T3CC0=0xF9;              //设置T3CC0为249
      T3CCTL0 |=0x04;          //使用模模式，需要设置通道0的输出比较模式
      T3CTL |=0X10;//启动
      EA = 1; 
}

//unsigned int analogRead(unsigned char channel)
//{
//    unsigned short value; //短整型16位
//    APCFG=channel;  //配置P0_7~P0_0为模拟I/O，1使能，0禁用
//    ADC_SINGLE_CONVERSION(ADC_REF_AVDD | ADC_10_BIT | ADC_PULSE_SENS);    // 使用3.3V参考电压，10位分辨率，AD源为P0_4
//    ADC_START_SINGLE();                                                   //开启单通道ADC
//    while(!ADC_SAMPLE_READY());                 //等待AD转换完成
//    value =  ADCL >> 5;                         //ADCL寄存器低2位无效
//    value |= (((unsigned short)ADCH) << 3);   //ADCH最高位为符号位
//    return value;
//}

// Timer 0中断子程序，每2MS中断一次，读取AD值，计算心率值
#pragma vector = T3_VECTOR//定时器3 
 __interrupt void T3_ISR(void) 

{          
  IRCON = 0x00;                  //中断标志4，清中断标志, 也可由硬件自动完成 
  long N;
  unsigned char i;
// keep a running total of the last 10 IBI values
  unsigned int runningTotal = 0;                  // clear the runningTotal variable    
  EA=0;                                      // disable interrupts while we do this （EA:cpu总中断开关）关中断
	//TL0=T0MS;
	//TH0=T0MS>>8;				//reload 16 bit TIMER0	//重新载入定时器初值
  Signal =HalAdcRead (HAL_ADC_CHANNEL_4, HAL_ADC_RESOLUTION_12);              // read the Pulse Sensor 
  sampleCounter += 2;                         // keep track of the time in mS with this variable
  N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise


    //  find the peak（波峰） and trough（波谷） of the pulse wave
  //if(Signal < thresh && N > (IBI/5)*3){       // avoid dicrotic noise by waiting 3/5 of last IBI
  if(Signal < thresh && Signal < Trough){                  // T is the trough
      Trough = Signal;                         // keep track of lowest point in pulse wave 
  }

  if(Signal > thresh && Signal > Peak){          // thresh condition helps avoid noise
    Peak = Signal;                             // P is the peak
  }                                        // keep track of highest point in pulse wave

  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
  //如果大于250ms
  //if (N > 250){
  //if (N > (IBI/5)*3){// avoid high frequency noise
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) ){        
      Pulse = true;                               // set the Pulse flag when we think there is a pulse
//      blinkPin=0;               // turn on pin 13 LED
      IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
      lastBeatTime = sampleCounter;               // keep track of time for next pulse

      if(secondBeat){                        // if this is the second beat, if secondBeat == TRUE
        secondBeat = false;                  // clear secondBeat flag
        for(i=0; i<=9; i++){             // seed the running total to get a realisitic BPM at startup
          rate[i] = IBI;                      
        }
      }
//首次检测到心跳，只执行一次
      if(firstBeat){                         // if it's the first time we found a beat, if firstBeat == TRUE
        firstBeat = false;                   // clear firstBeat flag
        secondBeat = true;                   // set the second beat flag
        EA=1;                               // enable interrupts again
        return;                              // IBI value is unreliable so discard it
      }   



      for(i=0; i<=8; i++){                // shift data in the rate array（左移）
        rate[i] = rate[i+1];                  // and drop the oldest IBI value 
        runningTotal += rate[i];              // add up the 9 oldest IBI values
      }

      rate[9] = IBI;                          // add the latest IBI to the rate array
      runningTotal += rate[9];                // add the latest IBI to runningTotal
      runningTotal /= 10;                     // average the last 10 IBI values 
      BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
			if(BPM>200)BPM=200;			//限制BPM最高显示值
			if(BPM<30)BPM=30;				//限制BPM最低显示值
      QS = true; 
      
     
      // set Quantified Self flag 
      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }                       
  
  
//重新设置指标
  if (Signal < thresh && Pulse == true){   // when the values are going down, the beat is over
//    blinkPin=1;            // turn off pin 13 LED
    
    Pulse = false;                         // reset the Pulse flag so we can do it again
    amp = Peak - Trough;                           // get amplitude of the pulse wave
    thresh = amp/2 + Trough;                    // set thresh at 50% of the amplitude
    Peak = thresh;                            // reset these for next time
    Trough = thresh;
  }
//超时重置
  if (N > 2500){                           // if 2.5 seconds go by without a beat
    thresh = 512;                          // set thresh default
    Peak = 512;                               // set P default
    Trough = 512;                               // set T default
    lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date        
    firstBeat = true;                      // set these to avoid noise
    secondBeat = false;                    // when we get the heartbeat back
  }

  EA=1;                                   // enable interrupts when youre done!
}// end isr