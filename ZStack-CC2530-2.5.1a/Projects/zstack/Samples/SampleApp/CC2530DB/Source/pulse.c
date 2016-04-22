//����ͷ�ļ�
//ADC��������ΪP0_3
#include "iocc2530.h"
#include "stdio.h"
#include "hal_adc.h"
#include "MT_UART.h"

#define false 0
#define true  1


#define HI_UINT16(a) (((a) >> 8) & 0xFF)
#define LO_UINT16(a) ((a) & 0xFF)


//#define ADC_REF_AVDD        0x80    //adc�ο���ѹ3.3v
//#define ADC_10_BIT          0x20     //adc�ֱ���10λ
//#define ADC_PULSE_SENS      0x04   //adc����P0_4
//#define ADC_SINGLE_CONVERSION(settings) \
//   do{ ADCCON3 = (settings); }while(0) //adc���òο���ѹ���ֱ��ʣ���������
//#define ADC_START_SINGLE() \
//  do { ADC_START_MODE(); ADCCON1 |= 0x40;  } while (0)//adc�ֶ�����
//#define ADC_SAMPLE_READY()  (ADCCON1 & 0x80) //adcת��������Ϊ��һ����׼��
//#define ADC_START_MODE() \
//  do { ADCCON1 |= 0x30; } while (0) //adc������ʽѡ��Ϊ�ֶ�����


//ȫ�ֱ���
//char T3Flag=0;
unsigned char PulsePin = 4; //p0_4
unsigned char PulseArr[4];
// these variables are volatile because they are used during the interrupt service routine!
 unsigned int BPM;                   // used to hold the pulse rate
 unsigned int Signal;                // holds the incoming raw data
 unsigned int IBI = 600;             // holds the time between beats, must be seeded!������� 
 unsigned char Pulse = false;     // true when pulse wave is high, false when it's  unsigned char QS = false;        
 unsigned char QS = false;    // becomes true when Arduoino finds a beat.
  
 int rate[10];                    // array to hold last ten IBI values
 unsigned long sampleCounter = 0;          // used to determine pulse timing
 unsigned long lastBeatTime = 0;           // used to find  int Peak =512;     // used to find peak in pulse wave, seeded	  int Trough = 512;                     // used to find trough in pulse wave, seeded	����
 int Peak =512; 
 int Trough = 512;  
 int thresh = 512;                // used to find instant moment of heart beat, seeded 	����˲��
 int amp = 100;                   // used to hold amplitude of pulse waveform, seeded 	���
 unsigned char firstBeat = true;        // used to seed rate array so we startup with reasonable BPM
 unsigned char secondBeat = false;      // used to seed rate array so we startup with reasonable BPM

 //��������
 void T3_init(void);// 16λ��ʱ����������2ms����һ�Σ������ж�
// unsigned int analogRead(unsigned char channel);//��ȡA/Dת�����ݣ�����������
 unsigned char getPulseArr(unsigned int dat);
 void pulse_test(void);
 void sendDataToProcessing(char symbol, unsigned int dat );
__interrupt void T3_ISR(void) ;//��ʱ��1�жϺ���


//���ڳ�ʼ��
//void UART_init(void)
//{
//  PERCFG &=0xFE;//������Ĵ����ĵ���λǿ������  1111 1110 
//  //���ǰѴ���0�Ľ�λ�������ڱ���λ��1 ��P0_2  P0_3
//  P0SEL  |=0x0C;//��P0_2  P0_3�������Ź�����Ƭ������ģʽ,��������ͨIO��       0000 1100
//  U0CSR |=0xC0;
//  U0UCR =0; //����0 ���͵Ĵ�������  У��λ ֹͣλ֮��Ķ���
//  U0GCR =11;
//  U0BAUD =216;//�����عٷ������ֲ��в����ʱ���в���115200ʱ�� ����ֵ��ǰ����ϵͳʱ����32M
//  IEN0 |=0x04; //���������ݵ��ж�  0000 0100
//  EA=1;
//}

//���ַ�
//char Putchar(unsigned char dat)
//{
//  UTX0IF=0;//���ڷ�����ɱ�־λ����
//  U0DBUF=dat;//��ch�д�ŵ����ݷ��ͳ�ȥdat;
//  while(!UTX0IF);
//  UTX0IF=0;
//  return U0DBUF;
//}
//��ȡ��������ĺ���
unsigned char getPulseArr(unsigned int dat)
{
  unsigned char len;
  len=sprintf((char*)PulseArr,"%d",dat);
  return len;
}
//��ʱ����
//void delay(unsigned int n) //��ʱus
//{
//  MicroWait(k);	
//  //unsigned int i,j;
//	//for(i=0;i<n;i++)
//		//for(j=0;j<100;j++);
//}
//�����ַ���
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
//��ʱ����ʼ��(CLKCONCMD.TICKSPDΪ��ʱ���ṩ�δ�ʱ�ӣ�Ĭ��Ϊ001����16MHz)
void T3_init()
{     
      T3CTL |= 0x08 ;             //������ж�  00001000   
      T3IE = 1;                   //�����жϺ�T3�ж�
      T3CTL|=0XE0;               // 11100000 128��Ƶ,128/16000000*N=0.002S,N=250              
      T3CTL |= 0X02;            //ģ������ʽ��0x00��T3CC0
      T3CC0=0xF9;              //����T3CC0Ϊ249
      T3CCTL0 |=0x04;          //ʹ��ģģʽ����Ҫ����ͨ��0������Ƚ�ģʽ
      T3CTL |=0X10;//����
      EA = 1; 
}

//unsigned int analogRead(unsigned char channel)
//{
//    unsigned short value; //������16λ
//    APCFG=channel;  //����P0_7~P0_0Ϊģ��I/O��1ʹ�ܣ�0����
//    ADC_SINGLE_CONVERSION(ADC_REF_AVDD | ADC_10_BIT | ADC_PULSE_SENS);    // ʹ��3.3V�ο���ѹ��10λ�ֱ��ʣ�ADԴΪP0_4
//    ADC_START_SINGLE();                                                   //������ͨ��ADC
//    while(!ADC_SAMPLE_READY());                 //�ȴ�ADת�����
//    value =  ADCL >> 5;                         //ADCL�Ĵ�����2λ��Ч
//    value |= (((unsigned short)ADCH) << 3);   //ADCH���λΪ����λ
//    return value;
//}

// Timer 0�ж��ӳ���ÿ2MS�ж�һ�Σ���ȡADֵ����������ֵ
#pragma vector = T3_VECTOR//��ʱ��3 
 __interrupt void T3_ISR(void) 

{          
  IRCON = 0x00;                  //�жϱ�־4�����жϱ�־, Ҳ����Ӳ���Զ���� 
  long N;
  unsigned char i;
// keep a running total of the last 10 IBI values
  unsigned int runningTotal = 0;                  // clear the runningTotal variable    
  EA=0;                                      // disable interrupts while we do this ��EA:cpu���жϿ��أ����ж�
	//TL0=T0MS;
	//TH0=T0MS>>8;				//reload 16 bit TIMER0	//�������붨ʱ����ֵ
  Signal =HalAdcRead (HAL_ADC_CHANNEL_4, HAL_ADC_RESOLUTION_12);              // read the Pulse Sensor 
  sampleCounter += 2;                         // keep track of the time in mS with this variable
  N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise


    //  find the peak�����壩 and trough�����ȣ� of the pulse wave
  //if(Signal < thresh && N > (IBI/5)*3){       // avoid dicrotic noise by waiting 3/5 of last IBI
  if(Signal < thresh && Signal < Trough){                  // T is the trough
      Trough = Signal;                         // keep track of lowest point in pulse wave 
  }

  if(Signal > thresh && Signal > Peak){          // thresh condition helps avoid noise
    Peak = Signal;                             // P is the peak
  }                                        // keep track of highest point in pulse wave

  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
  //�������250ms
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
//�״μ�⵽������ִֻ��һ��
      if(firstBeat){                         // if it's the first time we found a beat, if firstBeat == TRUE
        firstBeat = false;                   // clear firstBeat flag
        secondBeat = true;                   // set the second beat flag
        EA=1;                               // enable interrupts again
        return;                              // IBI value is unreliable so discard it
      }   



      for(i=0; i<=8; i++){                // shift data in the rate array�����ƣ�
        rate[i] = rate[i+1];                  // and drop the oldest IBI value 
        runningTotal += rate[i];              // add up the 9 oldest IBI values
      }

      rate[9] = IBI;                          // add the latest IBI to the rate array
      runningTotal += rate[9];                // add the latest IBI to runningTotal
      runningTotal /= 10;                     // average the last 10 IBI values 
      BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
			if(BPM>200)BPM=200;			//����BPM�����ʾֵ
			if(BPM<30)BPM=30;				//����BPM�����ʾֵ
      QS = true; 
      
     
      // set Quantified Self flag 
      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }                       
  
  
//��������ָ��
  if (Signal < thresh && Pulse == true){   // when the values are going down, the beat is over
//    blinkPin=1;            // turn off pin 13 LED
    
    Pulse = false;                         // reset the Pulse flag so we can do it again
    amp = Peak - Trough;                           // get amplitude of the pulse wave
    thresh = amp/2 + Trough;                    // set thresh at 50% of the amplitude
    Peak = thresh;                            // reset these for next time
    Trough = thresh;
  }
//��ʱ����
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