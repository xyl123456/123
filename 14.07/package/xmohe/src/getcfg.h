#ifndef __GETCFG_H_
#define __GETCFG_H_


extern char ipaddr[16];//���ڴ�Ż�ȡ��Ip��ַ
extern char dns_addr[50];//���ڴ�Ŵ��Զ��������ַ
extern int ser_port;//Զ�̶˿�
extern int client_port;//���ض˿�
extern char RS232_BUF;//���ڶ˿�
extern int RS232_BAUD;//���ڲ�����
extern void  GetConfigValue( char* FileName);
extern void serialdata_handle(char buff[],int len);
extern void handle_sigchld(int sig);

extern int ComRd(int portNo,char buf[],int maxCnt,int Timeout);

#endif