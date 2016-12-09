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
#include "getcfg.h"

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
		if( (CFGBuffer[i1]==0x27) &&(Flg==0) )
		{
			Flg = 1;
			continue;
		}
		if( (CFGBuffer[i1]==0x27)&&(Flg>0) )
			break;
		pStr[i2] = CFGBuffer[i1];
		i2++;
	}
	pStr[i2] = 0;
	return atoi(pStr);
}
//获取IP地址
int GetCFGValue( char* CFGBuffer, int Buflen, char *pKeyName, char *pItemName )
{
	int   i1, i2, len1, len2,sizelen;

	len1 = strlen( pKeyName );
	len2 = strlen( pItemName );

	for( i1=0; i1<Buflen; i1++ )
	{
		if( strncmp( &CFGBuffer[i1], pKeyName, len1 ) == 0 )
		{
			i1 += len1;
			break;
		}
	}
	if( i1==Buflen )    return  -1;
	sizelen = i1;
	for( ; i1<Buflen; i1++ )
	{
		if( strncmp( &CFGBuffer[i1], pItemName, len2 )== 0 )
		{
			i1+= len2;
			break;
		}
	}
	if( (i1==Buflen )||(i1>(sizelen+10)))   
		{
		sizelen = 0;
		return -1;
		}
	
	return 0;
}


int GetConfigValue( char* FileName,char *RS232,int BAD[],int IPPORT[])
{
	FILE*	fp;
	char  	ValueStr[80];
    	char    	Buffer[1000], str[100];
	char 		rs232_buf[5];
	int		nBytes;
	int		i;
	int		baud[5];
	fp = fopen( FileName, "rt" );
	if( fp!=NULL )
	{
		fread( Buffer, 1000, 1, fp );//读入FP
		nBytes = strlen( Buffer );
		fclose( fp );
		if( nBytes > 0 )
		{
		if (0 ==  GetCFGValue( Buffer, nBytes, "rs232_0", "'1'")) {
				rs232_buf[0]='1';
				//sprintf(pConfigInfo->ipaddr, "ifconfig eth0 %s", ValueStr);
				//baud[0]=GetValue(Buffer,nBytes,"baud_0");
				baud[0]=0;
				printf("I get the RS232_0,the baud is %d\n",baud[0]);
				
			} else {
				rs232_buf[0]='0';
				baud[0]=0;
				//strcpy( pConfigInfo->ipaddr , "ifconfig eth0 192.168.1.110");
				printf("the RS232_1 is not ready\n");
			}
			if (0 ==  GetCFGValue( Buffer, nBytes, "rs232_1", "'1'")) {
				rs232_buf[1]='1';
				//sprintf(pConfigInfo->ipaddr, "ifconfig eth0 %s", ValueStr);
				baud[1]=GetValue(Buffer,nBytes,"baud_1");
				printf("I get the RS232_1,the baud is %d\n",baud[1]);
				
			} else {
				rs232_buf[1]='0';
				baud[1]=0;
				//strcpy( pConfigInfo->ipaddr , "ifconfig eth0 192.168.1.110");
				printf("the RS232_1 is not ready\n");
			}
			// get tcpserver parameters: base port
			if (0 ==  GetCFGValue( Buffer, nBytes, "rs232_2", "'1'")) {
				rs232_buf[2]='1';
				baud[2]=GetValue(Buffer,nBytes,"baud_2");
				//sprintf(pConfigInfo->ipaddr, "ifconfig eth0 %s", ValueStr);
				printf("I get the RS232_2,the baud is %d\n",baud[2]);
			} else {
				rs232_buf[2]='0';
				baud[2]=0;
				//strcpy( pConfigInfo->ipaddr , "ifconfig eth0 192.168.1.110");
				printf("the RS232_2 is not ready\n");
			}
			if (0 ==  GetCFGValue( Buffer, nBytes, "rs232_3", "'1'")) {
				rs232_buf[3]='1';
				baud[3]=GetValue(Buffer,nBytes,"baud_3");
				//sprintf(pConfigInfo->ipaddr, "ifconfig eth0 %s", ValueStr);
				printf("I get the RS232_3,the baud is %d\n",baud[3]);
			} else {
				rs232_buf[3]='0';
				baud[3]=0;
				//strcpy( pConfigInfo->ipaddr , "ifconfig eth0 192.168.1.110");
				printf("the RS232_3 is not ready\n");
			}
			if (0 ==  GetCFGValue( Buffer, nBytes, "rs232_4", "'1'")) {
				rs232_buf[4]='1';
				baud[4]=GetValue(Buffer,nBytes,"baud_4");
				//sprintf(pConfigInfo->ipaddr, "ifconfig eth0 %s", ValueStr);
				printf("I get the RS232_4,the baud is %d\n",baud[4]);
			} else {
				rs232_buf[4]='0';
				baud[4]=0;
				//strcpy( pConfigInfo->ipaddr , "ifconfig eth0 192.168.1.110");
				printf("the RS232_4 is not ready\n");
			}
			// get serial parameters
			int i;
			for (i=0;i<5;i++)
				{
				RS232[i]=rs232_buf[i];
				BAD[i]=baud[i];
				}
			printf("the buf is %s\n",RS232);

			IPPORT[0]=GetValue(Buffer,nBytes,"ipport");
			//printf("the port is %d\n",IPPORT[0]);
			/*
			for( i=0; i<1; i++ )
			{     
				sprintf( str, "ttys%d=", i );
				if (0 ==  GetCFGValue( Buffer, nBytes, "[RS232]", str, ValueStr ) )
				{
					sscanf( ValueStr, ("%d-%d-%d-%c"), &pConfigInfo->BaudRate[i], &pConfigInfo->DataBits[i],
						      &pConfigInfo->StopBits[i], &pConfigInfo->Parity[i] );
				}
				printf("BaudRate[%d] = %d\n", i, pConfigInfo->BaudRate[i]);
				printf("DataBits[%d] = %d\n", i, pConfigInfo->DataBits[i]);
				printf("Parity[%d] = %c\n", i, pConfigInfo->Parity[i]);
				printf("StopBits[%d] = %d\n", i, pConfigInfo->StopBits[i]);	
			}
			*/
		}
	}
	return 0;
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
