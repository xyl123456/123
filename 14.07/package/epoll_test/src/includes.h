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

#define MAXBUFLEN  2048
#define TRUE 0
#define FALSE -1
#define msleep(x)  usleep((x)*1000)

typedef struct Server_env {
	int port;
	int serial_fd; //´®¿ÚÃèÊö·û
}server_env_t;

typedef struct E2s_env_t {
	int socket_fd;
	int serial_fd;
	int timeout;
}e2s_env_t;

typedef struct Serial {
	char dev[11];
	int speed;
	int databits;
	int stopbits;
	char parity;
}serial_t;

typedef	struct	_CONFIG_INFO
{
	char		ipaddr[40];
	int		TCPBasePort;
	int  		BaudRate[6];
	int		DataBits[6];
	int	   	StopBits[6];
	char   	Parity[6];
	int		TimeOut;
	
}CONFIG_INFO;

