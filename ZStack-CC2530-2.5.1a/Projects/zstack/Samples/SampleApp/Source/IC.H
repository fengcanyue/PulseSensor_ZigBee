#ifndef __IC_H__
#define __IC_H__



//extern   unsigned char qq[4];
//extern unsigned char IC_Test(void);
extern void IC_Init(void);
extern void writeTimeToIC(unsigned char addr);
extern void readTimeFromIC(unsigned char addr);
#endif