#ifndef __IC_W_R_H__
#define __IC_W_R_H__

extern void PcdReset(void);//��λ
extern void M500PcdConfigISOType(unsigned char type);//������ʽ
extern char PcdRequest(unsigned char req_code,unsigned char *pTagType);//Ѱ��
extern char PcdAnticoll(unsigned char *pSnr);//������
extern char PcdSelect(unsigned char *pSnr); //ѡ��
extern char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr);//У������
extern char PcdWrite(unsigned char addr,unsigned char *pData);//д����
extern char PcdRead(unsigned char addr,unsigned char *pData);//������
extern char PcdHalt(void);//���Ƭ��������״̬
#endif 