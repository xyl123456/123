#ifndef __GETCFG_H_
#define __GETCFG_H_


extern char ipaddr[16];//用于存放获取的Ip地址
extern char dns_addr[50];//用于存放存放远程域名地址
extern int ser_port;//远程端口
extern int client_port;//本地端口
extern char RS232_BUF;//串口端口
extern int RS232_BAUD;//串口波特率
extern void  GetConfigValue( char* FileName);
extern void serialdata_handle(char buff[],int len);
extern void handle_sigchld(int sig);

extern int ComRd(int portNo,char buf[],int maxCnt,int Timeout);

#endif