#ifndef __PULSE_H__
#define __PULSE_H__

extern unsigned int BPM; 
extern unsigned char PulseArr[4];
extern void T3_init(void);// 16λ��ʱ����������2ms����һ�Σ������ж�
extern unsigned int analogRead(unsigned char channel);//��ȡA/Dת�����ݣ�����������
extern unsigned char getPulseArr(unsigned int dat);


#endif