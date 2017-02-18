#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include "serial.h"

int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio,oldtio;  
    if(tcgetattr(fd,&oldtio)!=0)  
    {  
        perror("error:SetupSerial 3\n");  
        return -1;  
    }  
    bzero(&newtio,sizeof(newtio));  
    //使能串口接收  
    newtio.c_cflag |= CLOCAL | CREAD;  
    newtio.c_cflag &= ~CSIZE;  
      
    newtio.c_lflag &=~ICANON;//原始模式    
                               
    //newtio.c_lflag |=ICANON; //标准模式   
      
    //设置串口数据位  
    switch(nBits)  
    {  
        case 7:  
            newtio.c_cflag |= CS7;  
            break;  
        case 8:  
            newtio.c_cflag |=CS8;  
            break;  
    }  
    //设置奇偶校验位  
    switch(nEvent)  
    {  
        case 'O':  
            newtio.c_cflag |= PARENB;  
            newtio.c_cflag |= PARODD;  
            newtio.c_iflag |= (INPCK | ISTRIP);  
            break;  
        case 'E':  
            newtio.c_iflag |= (INPCK | ISTRIP);  
            newtio.c_cflag |= PARENB;  
            newtio.c_cflag &= ~PARODD;  
            break;  
        case 'N':  
            newtio.c_cflag &=~PARENB;  
            break;  
    }  
    //设置串口波特率  
    switch(nSpeed)  
    {  
        case 2400:  
            cfsetispeed(&newtio,B2400);  
            cfsetospeed(&newtio,B2400);  
            break;  
        case 4800:  
            cfsetispeed(&newtio,B4800);  
            cfsetospeed(&newtio,B4800);  
            break;  
        case 9600:  
            cfsetispeed(&newtio,B9600);  
            cfsetospeed(&newtio,B9600);  
            break;  
        case 115200:  
            cfsetispeed(&newtio,B115200);  
            cfsetospeed(&newtio,B115200);  
            break;  
        case 460800:  
            cfsetispeed(&newtio,B460800);  
            cfsetospeed(&newtio,B460800);  
            break;  
        default:  
            cfsetispeed(&newtio,B9600);  
            cfsetospeed(&newtio,B9600);  
            break;  
    }  
    //设置停止位  
    if(nStop == 1)  
        newtio.c_cflag &= ~CSTOPB;  
    else if(nStop == 2)  
        newtio.c_cflag |= CSTOPB; 
	
    newtio.c_cc[VTIME] = 0;  
    newtio.c_cc[VMIN] = 0;  
    tcflush(fd,TCIFLUSH);  
      
    if(tcsetattr(fd,TCSANOW,&newtio)!=0)  
    {  
        perror("com set error\n");  
        return -1;  
    }  
    return 0;  
}


int open_port(int fd,int comport)
{
	fd = open( "/dev/ttyUSB0", O_RDWR|O_NOCTTY);
	//fd = open( "/dev/ttyS1", O_RDWR|O_NOCTTY|O_NDELAY);
	if(-1==fd)
		{
			perror("Can't Open Serial Port\n");
			return(-1);
		}
	return fd;
}
