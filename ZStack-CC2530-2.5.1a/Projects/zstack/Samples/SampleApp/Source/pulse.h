#ifndef __PULSE_H__
#define __PULSE_H__

extern unsigned int BPM; 
extern unsigned char PulseArr[4];
extern void T3_init(void);// 16位定时器计数器，2ms触发一次，开启中断
extern unsigned int analogRead(unsigned char channel);//读取A/D转换数据，再重新启动
extern unsigned char getPulseArr(unsigned int dat);


#endif