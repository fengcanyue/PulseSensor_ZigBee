#include "ioCC2530.h"
#include "iic.h"

#define TRUE 1
#define FALSE 0

#define SCL P1_3
#define SDA P2_0
#define set_sda_in()  P2DIR &= ~(1<<0)
#define set_sda_out() P2DIR |= 1<<0
#define set_scl_out() P1DIR |= 1<<3
	
#define delay_TIME 500
unsigned char error; /*������ʾ,ȫ�ֱ���*/

//��������
void delay(void);
void WriteSDA1(void);//SDA output 1 
void WriteSDA0(void);
void WriteSCL0(void);
void WriteSCL1(void);
void ReadSDA(void);
void I2C_Start_1(void);
void I2C_Stop_1(void);
void SEND_0_1(void) ;
void SEND_1_1(void);
char Check_Acknowledge_1(void);
void Write_Acknowledge_1_1(void);
void WriteI2CByte_1(char b);
unsigned char ReadI2CByte_1(void);
//nop=1����������=12��ʱ������=12*1/32us;
//4.7usԼ12.5��nop������13��nop
void delay(void) 
{
  asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop"); asm("nop"); asm("nop"); asm("nop");
  asm("nop"); asm("nop");asm("nop");
}

void WriteSDA1(void)//SDA output 1
{
  set_sda_out();//P2DIR |= 0x10;
  SDA = 1;
}
    
void WriteSDA0(void)
{
  set_sda_out();//P2DIR |= 0x10;
  SDA = 0;
}

void WriteSCL1(void)
{
  set_scl_out();//P2DIR |= 0x08;
  SCL = 1;
}

void WriteSCL0(void)
{
  set_scl_out();//P2DIR |= 0x08;
  SCL = 0;
}

void ReadSDA(void)
{
  set_sda_in();//P2DIR &= 0xEF;
}

    void I2C_Start_1(void)
    {
        /*����I2C���ߵĺ�������SCLΪ�ߵ�ƽʱʹSDA����һ��������*/
        WriteSDA1();
        WriteSCL1();
        delay();
        WriteSDA0();
        delay();
        WriteSCL0();//Ϊ��ʹ���ͺ�����ͷ����дSCL=0��delay
        delay();
    }

    void I2C_Stop_1(void)
    {
        /*��ֹI2C���ߣ���SCLΪ�ߵ�ƽʱʹSDA����һ��������*/
        //WriteSCL0();
        //delay();
        WriteSDA0();
        delay();
        WriteSCL1();
        delay();
        WriteSDA1();
        delay();
       // WriteSCL0();
        //delay();
    }

    void SEND_0_1(void)   /* SEND ACK */
    {
        /*����0����SCLΪ�ߵ�ƽʱʹSDA�ź�Ϊ��*/
        
        WriteSDA0();
        //WriteSCL1();
        delay();
        //WriteSCL0();
        WriteSCL1();
        delay();
        WriteSCL0();
        delay();
    }

    void SEND_1_1(void)
    {
        /*����1����SCLΪ�ߵ�ƽʱʹSDA�ź�Ϊ��*/
       //WriteSCL0();
       //delay();
        WriteSDA1();
        //WriteSCL1();
        delay();
        WriteSCL1();
        delay();
        WriteSCL0();
        delay();
    }
    
   char Check_Acknowledge_1(void)
    {
        /*������һ���ֽں�����豸��Ӧ���ź�*/
        //WriteSCL0();
        //delay();
        ReadSDA();
        //WriteSCL1();
        delay();
        WriteSCL1();
        delay();
        F0=SDA;
        delay();
        WriteSCL0();
        delay();
        if(F0==1)
            return FALSE;
        return TRUE;
    }
    
    void Write_Acknowledge_1_1(void)
    {
        //WriteSCL0();
        //delay();
        WriteSDA1();
        delay();
        WriteSCL1();   
        delay();
	WriteSCL0();   
        delay();
    }
    
    void Write_Acknowledge_1(void)
    {
       // WriteSCL0();  
        //delay();
        
	WriteSDA0();   
        delay();
	WriteSCL1();   
        delay();
	WriteSCL0();   
        delay();
    }

    void WriteI2CByte_1(char b)
    {
        /*��I2C����дһ���ֽ�*/
        char i;
        for(i=0;i<8;i++)
	{
          if((b<<i)&0x80)
	  {
             SEND_1_1();
          }
          else
          {
             SEND_0_1();
          }
        }
    }

    unsigned char ReadI2CByte_1(void)
    {
        /*��I2C���߶�һ���ֽ�*/
        unsigned char b=0,i;
        //WriteSDA1();

        for(i=0;i<8;i++)
        {   
            WriteSCL0();
            delay();
            WriteSCL1(); 
            delay();

            ReadSDA();
            F0=SDA;

            if(F0==1)
            {
              b=b<<1;
              b=b|0x01;
            }
            else
              b=b<<1;
        }
        WriteSCL0();
        return b; 
    }
