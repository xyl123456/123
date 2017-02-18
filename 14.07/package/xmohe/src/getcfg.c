#include <unistd.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <termios.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "getcfg.h"
#include "list.h"

//获取目标字符串后的数字
int GetValue( char* CFGBuffer, int Buflen, char *pKeyName )
{
	int   i1, i2, len1, len2;
	char pStr[20];

	len1 = strlen( pKeyName );

	for( i1=0; i1<Buflen; i1++ )
	{
		if( strncmp( &CFGBuffer[i1], pKeyName, len1 ) == 0 )
		{
			i1 += len1;
			break;
		}
	}
	if( i1==Buflen )    return  -1;
	int Flg=0;
	for( i2=0; i1<Buflen; i1++)
	{
		if( (CFGBuffer[i1]==0x27) &&(Flg==0) )//0x27 表示的是‘
		{
			Flg = 1;
			continue;
		}
		if( (CFGBuffer[i1]==0x27)&&(Flg>0) )
			break;
		pStr[i2] = CFGBuffer[i1];
		i2++;
	}
	pStr[i2] = '\0';
	return atoi(pStr);
}

/*获取目标后边的字符串，函数返回字符指针*/
void  GetString( char* CFGBuffer, int Buflen, char *pKeyName )
{
	int   i1, i2, len1, len2;
	int   len_temp;
	char pStr[25];

	len1 = strlen( pKeyName );

	for( i1=0; i1<Buflen; i1++ )
	{     //字符相同放回0
		if( strncmp( &CFGBuffer[i1], pKeyName, len1 ) == 0 )
		{
			i1 += len1;
			len_temp = i1;
			break;
		}
	}
	if( len_temp==Buflen )   
		{
		memset(ipaddr,0,sizeof(ipaddr));
		return  ;
		}
	if(i1==Buflen)
		{
		memset(ipaddr,0,sizeof(ipaddr));
		return  ;
		}
	int Flg=0;
	for( i2=0; len_temp<Buflen; len_temp++)
	{
		if( (CFGBuffer[len_temp]==0x27) &&(Flg==0) )//0x27 表示的是‘,表示第一次'
		{
			Flg = 1;
			continue;
		}
		else if( (CFGBuffer[len_temp]==0x27)&&(Flg>0) )
			break;
		else
			{
			pStr[i2] = CFGBuffer[len_temp];
			i2++;
			}
	}
	pStr[i2] = '\0';
	int num_i,num_m;
	for(num_i = 0,num_m=0;num_i < i2;num_i++)
		{
		if(pStr[num_i]!=' ')
			{
		ipaddr[num_m] = pStr[num_i];
		num_m++;
			}
		else
			{
			continue;
			}
		}
	//printf("get the ipaddr is%s\n",get_string);
}


/*获取目标后边的字符串，函数返回字符指针*/
void  GetDNS_String( char* CFGBuffer, int Buflen, char *pKeyName )
{
	int   i1, i2, len1, len2;
	int   len_temp;
	char pStr[25];

	len1 = strlen( pKeyName );

	for( i1=0; i1<Buflen; i1++ )
	{     //字符相同放回0
		if( strncmp( &CFGBuffer[i1], pKeyName, len1 ) == 0 )
		{
			i1 += len1;
			len_temp = i1;
			break;
		}
	}
	if( len_temp==Buflen )  
		{
		memset(dns_addr,0,sizeof(dns_addr));
		return  ;
		}
	if(i1==Buflen)
		{
		memset(dns_addr,0,sizeof(ipaddr));
		return  ;
		}
	int Flg=0;
	for( i2=0; len_temp<Buflen; len_temp++)
	{
		if( (CFGBuffer[len_temp]==0x27) &&(Flg==0) )//0x27 表示的是‘,表示第一次'
		{
			Flg = 1;
			continue;
		}
		else if( (CFGBuffer[len_temp]==0x27)&&(Flg>0) )
			break;
		else
			{
			pStr[i2] = CFGBuffer[len_temp];
			i2++;
			}
	}
	pStr[i2] = '\0';
	int num_i,num_m;
	for(num_i = 0,num_m=0;num_i < i2;num_i++)
		{
		if(pStr[num_i]!=' ')
			{
	        dns_addr[num_m] = pStr[num_i];
		num_m++;
			}
		else
			{
			continue;
			}
		}
	//printf("get the ipaddr is%s\n",get_string);
}
/*判断pKeyName后面是否为pItemName
*/
int GetCFGValue( char* CFGBuffer, int Buflen, char *pKeyName, char *pItemName )
{
	int   i1, i2, len1, len2,sizelen,bufsize;

	len1 = strlen( pKeyName );
	len2 = strlen( pItemName );

	for( i1=0; i1<Buflen; i1++ )
	{
		if( strncmp( &CFGBuffer[i1], pKeyName, len1 ) == 0 )
		{
			i1 += len1;
			sizelen = i1;
			break;
		}
	}
	if( i1==Buflen )    return  -1;
	int num;
	for(num= sizelen; num<Buflen;num++ )
	{
		if( strncmp( &CFGBuffer[num], pItemName, len2 )== 0 )
		{
			num+= len2;
			bufsize=num;
			break;
		}
	}
	if( (bufsize==Buflen )||(bufsize>(sizelen+10)))   //到文件末尾或者间隔大于10字符
		{
		sizelen = 0;
		bufsize = 0;
		return -1;
		}
	return 0;
}


void GetConfigValue( char* FileName)
{
	FILE*	fp;
    	char    	Buffer[1000], str[100];
	char 		rs232_buf;
	int		nBytes;
	int		baud;
	int 		tmp_port;
	
	fp = fopen( FileName, "rt" );
	if( fp!=NULL )
	{
		fread( Buffer, 1000, 1, fp );//读入FP
		nBytes = strlen( Buffer );
		fclose( fp );
		if( nBytes > 0 )
		{
		//注意串口0就是rs232
		if (0 ==  GetCFGValue( Buffer, nBytes, "rs232_cfg", "'1'")) 
			{
				rs232_buf='1';
				baud=GetValue(Buffer,nBytes,"rs232_baud");
				printf("I get the RS232,the baud is %d\n",baud);
				
			} 
		else {
				rs232_buf='0';
				baud=0;
				printf("the RS232 is not ready\n");
			}
			// get serial parameters
	
			RS232_BUF=rs232_buf;
			RS232_BAUD=baud;
			tmp_port=GetValue(Buffer,nBytes,"client_port");
			if(tmp_port>0)
			client_port=tmp_port;
			else
				{
				client_port=0;
				tmp_port=0;
				}
			tmp_port=GetValue(Buffer,nBytes,"ser_port");
			if(tmp_port>0)
			ser_port=tmp_port;//获取远程端口
			else
				{
				ser_port=0;
				tmp_port=0;
				}
			GetString( Buffer,nBytes, "ser_ip" );//获取IP地址
			//GetDNS_String( Buffer,nBytes, "dns_addr" );//获取DNS地址
		}
	}
}

/*
int iscorrectcfg(CONFIG_INFO* pConfigInfo)
{
	int i;
	
	if (pConfigInfo->TCPBasePort < 1000 || pConfigInfo->TCPBasePort > 20000) 
		pConfigInfo->TCPBasePort = 1234;

	for (i = 0; i < 6; i++) {
		if (pConfigInfo->BaudRate[i] < 300 || pConfigInfo->BaudRate[i] > 230400)
			pConfigInfo->BaudRate[i] = 57600;
		if (pConfigInfo->DataBits[i] < 5 || pConfigInfo->DataBits[i] > 8)
			pConfigInfo->DataBits[i] = 8;
		if (pConfigInfo->Parity[i] != 'N' ||pConfigInfo->Parity[i] != 'n' ||\
				pConfigInfo->Parity[i] != 'O' ||pConfigInfo->Parity[i] != 'o' ||\
				pConfigInfo->Parity[i] != 'E' ||pConfigInfo->Parity[i] != 'e' ||\
				pConfigInfo->Parity[i] != 'S' ||pConfigInfo->Parity[i] != 's' ) {
			pConfigInfo->Parity[i] = 'n';

		}
		if (pConfigInfo->StopBits[i] != 0 || pConfigInfo->StopBits[i] != 1 || pConfigInfo->StopBits[i] != 2)
			 pConfigInfo->StopBits[i] = 1;
		printf("BaudRate[%d] = %d\n", i, pConfigInfo->BaudRate[i]);
		printf("DataBits[%d] = %d\n", i, pConfigInfo->DataBits[i]);
		printf("Parity[%d] = %c\n", i, pConfigInfo->Parity[i]);
		printf("StopBits[%d] = %d\n", i, pConfigInfo->StopBits[i]);
	} 

	return 0;
}
*/

//串口数据处理函数
void serialdata_handle(char buff[],int len)
{
	int send_fd;
	int k;
	if(NULL==head){
		return;
		}
	Node * p = head->pNext;
	while(NULL!=p){
		send_fd = p->fd_data;
		k=write(send_fd,buff,len);
		//printf("write %d is %s\n",k,buff);
		p = p->pNext;
		}
	free(p);
	return;
}



void handle_sigchld(int sig)
{
	while(waitpid(-1,NULL,WNOHANG)>0)
		;
}
/*
int ComRd(int portNo,char buf[],int maxCnt,int Timeout)
{
	int actualRead = 0;
	int pos=0;
	fd_set rfds;
	struct timeval tv;
 	int retval;
	FD_ZERO(&rfds);
	FD_SET(portNo,&rfds);
	tv.tv_sec= Timeout/1000;
	tv.tv_usec= (Timeout%1000)*1000;
	while(FD_ISSET(portNo,&rfds)){
		FD_ZERO(&rfds);
		FD_SET(portNo,&rfds);
		retval = select(portNo+1,&rfds,NULL,NULL,&tv);
		if(retval == -1){
			break;
		}
		else if(retval>0){
		actualRead= read(portNo, buf, maxCnt);
		pos=pos+actualRead;
		FD_ZERO(&rfds);
		FD_SET(portNo,&rfds);
		
		}
		else{
			
		}
	}
	return actualRead;
}
*/

