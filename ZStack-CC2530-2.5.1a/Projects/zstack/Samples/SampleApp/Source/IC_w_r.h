#ifndef __IC_W_R_H__
#define __IC_W_R_H__

extern void PcdReset(void);//复位
extern void M500PcdConfigISOType(unsigned char type);//工作方式
extern char PcdRequest(unsigned char req_code,unsigned char *pTagType);//寻卡
extern char PcdAnticoll(unsigned char *pSnr);//读卡号
extern char PcdSelect(unsigned char *pSnr); //选卡
extern char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr);//校验密码
extern char PcdWrite(unsigned char addr,unsigned char *pData);//写块区
extern char PcdRead(unsigned char addr,unsigned char *pData);//读块区
extern char PcdHalt(void);//命令卡片进入休眠状态
#endif 