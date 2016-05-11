#include "iic.h"
#include"OnBoard.h"
unsigned char second;
unsigned char second1;
unsigned char minute;
unsigned char hour; 
unsigned char dayOfWeek;// day of week, 1 = Monday
unsigned char dayOfMonth;
unsigned char month;
unsigned int year;
char acktemp;
//函数声明
unsigned char decToBcd(unsigned char val);
unsigned int bcdToDec(unsigned char val);
void setTime(unsigned char second,
    unsigned char minute,
    unsigned char hour,
    unsigned char dayOfWeek,
    unsigned char dayOfMonth,
    unsigned char month,
    unsigned char year);
void getTime(void);
unsigned char decToBcd(unsigned char val)
{
  return ( (val/10*16) + (val%10) );
}

unsigned int bcdToDec(unsigned char val)
{
  return ( (val/16*10) + (val%16) );
}


void setTime(unsigned char second,
    unsigned char minute,
    unsigned char hour,
    unsigned char dayOfWeek,
    unsigned char dayOfMonth,
    unsigned char month,
    unsigned char year)
{
  //HAL_DISABLE_INTERRUPTS();//读数时要关中断
  I2C_Start_1();
  WriteI2CByte_1(0xd0);
  acktemp=Check_Acknowledge_1();
  WriteI2CByte_1(0x00);
  acktemp=Check_Acknowledge_1();
  
  
  WriteI2CByte_1(decToBcd(second));
  acktemp=Check_Acknowledge_1();	
  WriteI2CByte_1(decToBcd(minute));
  acktemp=Check_Acknowledge_1();
  WriteI2CByte_1(decToBcd(hour));
  acktemp=Check_Acknowledge_1();
  WriteI2CByte_1(decToBcd(dayOfWeek));
  acktemp=Check_Acknowledge_1();
  WriteI2CByte_1(decToBcd(dayOfMonth));
  acktemp=Check_Acknowledge_1();
  WriteI2CByte_1(decToBcd(month));
  acktemp=Check_Acknowledge_1();
  WriteI2CByte_1(decToBcd(year));
  acktemp=Check_Acknowledge_1();
  I2C_Stop_1();
  //HAL_ENABLE_INTERRUPTS();
  
}
void getTime(void)
{
  //HAL_DISABLE_INTERRUPTS();//读数时要关中断
  I2C_Start_1();
  WriteI2CByte_1(0xd0);
  acktemp=Check_Acknowledge_1();				
  WriteI2CByte_1(0);
  acktemp=Check_Acknowledge_1();		
  I2C_Stop_1();

  
  I2C_Start_1();
  WriteI2CByte_1(0xd1);
  acktemp=Check_Acknowledge_1();
  
  second1 = ReadI2CByte_1()&0x7f;	Write_Acknowledge_1(); //&0x7f; 
  second=bcdToDec(second1);
  minute = bcdToDec(ReadI2CByte_1());	Write_Acknowledge_1();  
  hour	 =bcdToDec(ReadI2CByte_1()&0x3f);Write_Acknowledge_1() ;//& 0x3f;// Need to change this if 12 hour am/pm
  dayOfWeek  = bcdToDec(ReadI2CByte_1());Write_Acknowledge_1();  
  dayOfMonth = bcdToDec(ReadI2CByte_1());Write_Acknowledge_1();  
  month      = bcdToDec(ReadI2CByte_1());Write_Acknowledge_1();  
  year	   = bcdToDec(ReadI2CByte_1());	//Write_Acknowledge_1_1();  
  I2C_Stop_1();
  //HAL_ENABLE_INTERRUPTS();
}