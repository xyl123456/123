#ifndef __GETCFG_H_
#define __GETCFG_H_


extern char ipaddr[16];//用于存放获取的Ip地址
extern char dns_addr[50];//用于存放存放远程域名地址
extern int  GetConfigValue( char* FileName,char *RS232,int BAD[],int IPPORT[]);
extern void serialdata_handle(char buff[],int len);
extern void handle_sigchld(int sig);

#endif