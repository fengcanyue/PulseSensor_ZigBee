//#include "variable.h"
//#include"delay.h"
//#include "KEY.h"
//#include "UART.h"
#include "IC_w_r.h"

#include "MT_UART.h"
#include "DS1307.h"
#include "rc522.h"
#include "IC.h"
#include "hal_led.h"

  //uchar qq[4];
  //uchar buf[4];

void Initial(void);
//uchar IC_Test(void);
void IC_Init(void);
void writeTimeToIC(unsigned char addr);
void readTimeFromIC(unsigned char addr);


//uchar IC_Test(void)
//{
//  //uint i;
//  uchar find=0xaa;
//  uchar ar;   
//  ar = PcdRequest(0x52,buf);//寻卡
//  if(ar != 0x26)
//      ar = PcdRequest(0x52,buf);
//  if(ar != 0x26)
//      find = 0xaa;
//   if((ar == 0x26)&&(find == 0xaa))
//   {
//   if(PcdAnticoll(qq) == 0x26);//防冲撞
//   {
//  // HalUARTWrite(0,qq,4);
//   find = 0x00;
//   return 1;
//   }
//   }
//   return 0;
// 
//}

unsigned char  data[17] = {0};
unsigned char g_ucTempbuf[20]={0};
unsigned char  DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
unsigned char Card_ID[4]={0};
void writeTimeToIC(unsigned char addr)
{
  unsigned char status;
  status = PcdRequest(PICC_REQALL, g_ucTempbuf);   //寻天线区内全部卡
////////////////////////////////////////////////////////////////////////////////
         if (status != MI_OK)
         { 
	      return;
         }
/////////////////////////////////////////////////////////////////////
        
         status = PcdAnticoll(Card_ID);//防冲撞
//////////////////////////////////////////////////////////////////////
         if (status != MI_OK)
         { 
             return;    
         }
/////////////////////////////////////////////////////////////////////
         status = PcdSelect(g_ucTempbuf);//选卡
         if (status != MI_OK)
         {    
            return;    
         }
////////////////////////////////////////////////////////////////////////////         
         status = PcdAuthState(PICC_AUTHENT1A, addr, DefaultKey, g_ucTempbuf);//校验密码
         if (status != MI_OK)
         {    
            return;    
         }
/////////////////////////////////////////////////////////////////////////////         
         getTime();//获取DS1307中的时间
         data[0]=second;
         data[1]=minute;
         data[2]=hour;
/////////////////////////////////////////////////////////////////////////////         
         status = PcdWrite(addr, data);//写块区
         if (status != MI_OK)
         {    
            return;    
         }
          //HalLedBlink( HAL_LED_1, 1, 50, 50 );
          PcdHalt();   //命令卡片进入休眠状态
                     
}

void readTimeFromIC(unsigned char addr)
{
  unsigned char status;
  status = PcdRequest(PICC_REQIDL, g_ucTempbuf);   //寻天线区内未进入休眠状态,（PICC_REQIDL）读取玩卡后会等待离开天线作用范围，直到再次进入
////////////////////////////////////////////////////////////////////////////////
         if (status != MI_OK)
         { 
	      return;
         }
/////////////////////////////////////////////////////////////////////
        
         status = PcdAnticoll(g_ucTempbuf);//防冲撞
//////////////////////////////////////////////////////////////////////
         if (status != MI_OK)
         { 
             return;    
         }
/////////////////////////////////////////////////////////////////////
         status = PcdSelect(g_ucTempbuf);//选卡
         if (status != MI_OK)
         {    
            return;    
         }
////////////////////////////////////////////////////////////////////////////         
         status = PcdAuthState(PICC_AUTHENT1A, addr, DefaultKey, g_ucTempbuf);//校验密码
         if (status != MI_OK)
         {    
            return;    
         }
///////////////////////////////////////////////////////////////////////////         
         status = PcdRead(addr, g_ucTempbuf);//读块区
         if (status != MI_OK)
         {    
           return;   
         }
         PcdHalt();   //命令卡片进入休眠状态
         //HalLedBlink( HAL_LED_1, 1, 50, 250 );
         HalUARTWrite(0,g_ucTempbuf,16);
         HalUARTWrite(0,"\n",1);
}

void IC_Init(void)
{
  Initial();
  PcdReset();
  M500PcdConfigISOType('A');//
}