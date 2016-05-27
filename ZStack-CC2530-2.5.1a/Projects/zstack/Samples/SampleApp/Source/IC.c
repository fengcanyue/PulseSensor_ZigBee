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
//  ar = PcdRequest(0x52,buf);//Ѱ��
//  if(ar != 0x26)
//      ar = PcdRequest(0x52,buf);
//  if(ar != 0x26)
//      find = 0xaa;
//   if((ar == 0x26)&&(find == 0xaa))
//   {
//   if(PcdAnticoll(qq) == 0x26);//����ײ
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
  status = PcdRequest(PICC_REQALL, g_ucTempbuf);   //Ѱ��������ȫ����
////////////////////////////////////////////////////////////////////////////////
         if (status != MI_OK)
         { 
	      return;
         }
/////////////////////////////////////////////////////////////////////
        
         status = PcdAnticoll(Card_ID);//����ײ
//////////////////////////////////////////////////////////////////////
         if (status != MI_OK)
         { 
             return;    
         }
/////////////////////////////////////////////////////////////////////
         status = PcdSelect(g_ucTempbuf);//ѡ��
         if (status != MI_OK)
         {    
            return;    
         }
////////////////////////////////////////////////////////////////////////////         
         status = PcdAuthState(PICC_AUTHENT1A, addr, DefaultKey, g_ucTempbuf);//У������
         if (status != MI_OK)
         {    
            return;    
         }
/////////////////////////////////////////////////////////////////////////////         
         getTime();//��ȡDS1307�е�ʱ��
         data[0]=second;
         data[1]=minute;
         data[2]=hour;
/////////////////////////////////////////////////////////////////////////////         
         status = PcdWrite(addr, data);//д����
         if (status != MI_OK)
         {    
            return;    
         }
          //HalLedBlink( HAL_LED_1, 1, 50, 50 );
          PcdHalt();   //���Ƭ��������״̬
                     
}

void readTimeFromIC(unsigned char addr)
{
  unsigned char status;
  status = PcdRequest(PICC_REQIDL, g_ucTempbuf);   //Ѱ��������δ��������״̬,��PICC_REQIDL����ȡ�濨���ȴ��뿪�������÷�Χ��ֱ���ٴν���
////////////////////////////////////////////////////////////////////////////////
         if (status != MI_OK)
         { 
	      return;
         }
/////////////////////////////////////////////////////////////////////
        
         status = PcdAnticoll(g_ucTempbuf);//����ײ
//////////////////////////////////////////////////////////////////////
         if (status != MI_OK)
         { 
             return;    
         }
/////////////////////////////////////////////////////////////////////
         status = PcdSelect(g_ucTempbuf);//ѡ��
         if (status != MI_OK)
         {    
            return;    
         }
////////////////////////////////////////////////////////////////////////////         
         status = PcdAuthState(PICC_AUTHENT1A, addr, DefaultKey, g_ucTempbuf);//У������
         if (status != MI_OK)
         {    
            return;    
         }
///////////////////////////////////////////////////////////////////////////         
         status = PcdRead(addr, g_ucTempbuf);//������
         if (status != MI_OK)
         {    
           return;   
         }
         PcdHalt();   //���Ƭ��������״̬
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