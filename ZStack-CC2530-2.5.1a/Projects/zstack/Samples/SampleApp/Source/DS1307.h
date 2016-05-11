#ifndef DS1307_H
#define DS1307_H
#define DS1307_I2C_ADDRESS 0x68
extern unsigned char second;
extern unsigned char minute;
extern unsigned char hour; 
extern unsigned char dayOfWeek;// day of week, 1 = Monday
extern unsigned char dayOfMonth;
extern unsigned char month;
extern unsigned int year;
extern unsigned char decToBcd(unsigned char val);
extern  int bcdToDec(unsigned char val);
extern void getTime(void);
extern void setTime(unsigned char _second,
    unsigned char _minute,
    unsigned char _hour,
    unsigned char _dayOfWeek,
    unsigned char _dayOfMonth,
    unsigned char _month,
    unsigned char _year);
#endif