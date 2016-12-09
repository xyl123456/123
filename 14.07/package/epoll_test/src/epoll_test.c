#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <pthread.h>
#include "serial.h"
#include "getcfg.h"


int listenfd=0;//listen socket file
int connfd=0;//connected socket file
int ret =0;
int i;
int file_num;
int k;
int read_cont=0;
int serial_num;
int ip_port_num[1]={0};
char buf[128]={0};
char serial_buf[128];
char RS232_BUF[5];
int RS232_BAUD[5];
uint32_t len = 0;
int iRecvLen, iSendLen;
pthread_t Client_ID;

//INFODATA info_data;
struct sockaddr_in servaddr={0};
struct sockaddr_in cliaddr={0};
int epoll_instance,nfound,sockfd;

int serial_fd[4];

/*struct Infodata {
	int socfd;
	char * string;
	}INFODATA;
*/
/*function prototype*/
/*
void setnonblocking(int sock)
{
	int opts;
	opts=fcntl(sock,F_GETFL);
	if(opts<0)
	{
		perror("fcntl GETFL");
		exit(1);
	}
	opts=opts|O_NONBLOCK;
	if(fcntl(sock,F_SETFL,opts)<0)
	{
		perror("fcntl SETFL");
		exit(1);
	}
}
*/
void* ClientThread(void *recv_id)
{
	int revi = *(int *)recv_id;
		
		/* 接收客户端发来的数据并显示出来 */
		
		if (ret <= 0)
		{
			close(revi);
			pthread_exit(NULL);
		}
		else
		{
			buf[ret]='\0';
			//写入选中的串口中
			//write(*recv_id,buf,ret);
			write(serial_fd[2],buf,ret);
			printf("rcv socket:%s\n",buf);
			
			
		}

}


int main()
{
     //串口初始化
  //GetConfigValue("config.txt" , RS232_BUF,RS232_BAUD, ip_port_num);
  GetConfigValue("/etc/config/rs232" , RS232_BUF,RS232_BAUD, ip_port_num);
  printf("ipport is is %d\n",ip_port_num[0]);

   /*声明epoll_event结构体的变量，ev用于注册事件，events
   数组用于回传要处理的事件*/
   struct epoll_event ev,events[20];//数组是用于回传接收的事件
   /*生成用于处理accept的epoll专用
   的文件描述符，指定生成描述符的最大范围为256*/
   epoll_instance=epoll_create(256);//size is unused nowadays

   listenfd = socket(AF_INET,SOCK_STREAM,0);
   if(listenfd <0)
   {
      perror("error opening socket");
      return -1;
   }
   //setnonblocking(listenfd);//把用于监听的socket设置成非阻塞方式
   ev.data.fd=listenfd;//设置与要处理的事件相关的文件描述符
   ev.events=EPOLLIN|EPOLLET;//设置要处理的事件类型
   epoll_ctl(epoll_instance,EPOLL_CTL_ADD,listenfd,&ev);//注册epoll事件

   memset(&servaddr,0,sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = INADDR_ANY;
   servaddr.sin_port = htons((uint16_t )ip_port_num[0]);
      
   ret = bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
   if(ret <0)
   {
      perror("Error on binding");
      return -1;
   } 
   ret = listen(listenfd,5);//backlog
   if(ret !=0)
   {
       perror("Error on listening");
       return -1;
   }
//串口设备文件
  for (file_num= 0;file_num<5;file_num++)
  	{
  	if('1'==*(RS232_BUF+file_num))
  		{
  		 if(( serial_fd[file_num] = open_port(serial_fd[file_num], file_num))<0)
 			{
 			perror("open_port error");
			return;
 			}
   		if((k=set_opt(serial_fd[file_num],RS232_BAUD[file_num],8,'N',1))<0)
			{
			perror("set_opt error\n");
			return;
			}
		if(serial_fd[file_num]>2)
			{
			ev.data.fd=serial_fd[file_num];//设置与要处理的事件相关的文件描述符
   			ev.events=EPOLLIN|EPOLLET;//设置要处理的事件类型
   			epoll_ctl(epoll_instance,EPOLL_CTL_ADD,serial_fd[file_num],&ev);//注册epoll事件
			printf("s is %d,serial_fd is %d\n",file_num,serial_fd[file_num]);	
			}
  		}
	}

   
   while(1)
   {

   	  nfound=epoll_wait(epoll_instance,events,20,1000);//等待epoll事件的发生
   	  if(nfound==0)
   	  {
   	  	//printf(".");
   	  	fflush(stdout);
   	  	continue;
   	  }
    for(i=0;i<nfound;i++)
   	  {
   	
   	  	if(events[i].data.fd==listenfd)/*监听事件*/
   	  	{
   	  		  len = sizeof(cliaddr);
   	  		  connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&len);
   	  		  printf("connection from host %s,port %d,sockfd %d\n",
              inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port),connfd);
   	  		  //setnonblocking(connfd);
   	  		  
   	  		  ev.data.fd=connfd;//设置用于读操作的文件描述符
   	  		  ev.events=EPOLLIN|EPOLLET;//设置用于注册的读操作事件
   	  		  epoll_ctl(epoll_instance,EPOLL_CTL_ADD,connfd,&ev);//注册ev事件
			  printf("connect is %d\n",connfd);
			//info_data.socfd=connfd;
			//printf("connect is %d\n,info_data.socfd");
			//添加系统信息	
   	  	}
   	  	else  if(events[i].events&EPOLLIN)/*读事件*/
   	  	{
   	  		if(events[i].data.fd==serial_fd[0])
   	  		{
   	  		
   	  			memset(serial_buf,0,sizeof(serial_buf));
   	  			if((read_cont=read(serial_fd[0],serial_buf,128))>0)
						{
						serial_buf[read_cont]='\0';
						printf("rev %d from serial is : %s\n",read_cont,serial_buf);
						}
					else
						{
						ev.data.fd=serial_fd[0];
   	  		    			ev.events=EPOLLIN|EPOLLET;
   	  		    			epoll_ctl(epoll_instance,EPOLL_CTL_DEL,serial_fd[0],&ev);
   	  		    			close(serial_fd[0]);
   	  		   			printf("del uart client 0 \n");
						continue;
						}
   	  		}

		 else  if(events[i].data.fd==serial_fd[1])
   	  		{
   	  			memset(serial_buf,0,sizeof(serial_buf));
   	  			if((read_cont=read(serial_fd[1],serial_buf,128))>0)
						{
						serial_buf[read_cont]='\0';
						printf("rev %d from serial is : %s\n",read_cont,serial_buf);
						}
					else
						{
						ev.data.fd=serial_fd[1];
   	  		    			ev.events=EPOLLIN|EPOLLET;
   	  		    			epoll_ctl(epoll_instance,EPOLL_CTL_DEL,serial_fd[1],&ev);
   	  		    			close(serial_fd[1]);
   	  		   			printf("del uart client 1 \n");
						continue;
						}
   	  		}
		else if(events[i].data.fd==serial_fd[2])
   	  		{
   	  			memset(serial_buf,0,sizeof(serial_buf));
   	  			if((read_cont=read(serial_fd[2],serial_buf,128))>0)
						{
						serial_buf[read_cont]='\0';
						printf("rev %d from serial is : %s\n",read_cont,serial_buf);
						}
					else
						{
						ev.data.fd=serial_fd[2];
   	  		    			ev.events=EPOLLIN|EPOLLET;
   	  		    			epoll_ctl(epoll_instance,EPOLL_CTL_DEL,serial_fd[2],&ev);
   	  		    			close(serial_fd[2]);
   	  		   			printf("del uart client 2 \n");
						continue;
						}
   	  		}
		else if(events[i].data.fd==serial_fd[3])
   	  		{
   	  			memset(serial_buf,0,sizeof(serial_buf));
   	  			if((read_cont=read(serial_fd[3],serial_buf,128))>0)
						{
						serial_buf[read_cont]='\0';
						printf("rev %d from serial is : %s\n",read_cont,serial_buf);
						}
					else
						{
						ev.data.fd=serial_fd[3];
   	  		    			ev.events=EPOLLIN|EPOLLET;
   	  		    			epoll_ctl(epoll_instance,EPOLL_CTL_DEL,serial_fd[3],&ev);
   	  		    			close(serial_fd[3]);
   	  		   			printf("del uart client 3 \n");
						continue;
						}
		
   	  		}  	
		else if(events[i].data.fd==serial_fd[4])
   	  		{
   	  			memset(serial_buf,0,sizeof(serial_buf));
   	  			if((read_cont=read(serial_fd[4],serial_buf,128))>0)
						{
						serial_buf[read_cont]='\0';
						printf("rev %d from serial is : %s\n",read_cont,serial_buf);
						}
					else
						{
						ev.data.fd=serial_fd[4];
   	  		    			ev.events=EPOLLIN|EPOLLET;
   	  		    			epoll_ctl(epoll_instance,EPOLL_CTL_DEL,serial_fd[4],&ev);
   	  		    			close(serial_fd[4]);
   	  		   			printf("del uart client 4 \n");
						continue;
						}
		
   	  		}		
		//判断是否是串口数据
		else
   	  		sockfd=events[i].data.fd;	
   	  		memset(buf,0,sizeof(buf));
			ret=read(sockfd,buf,sizeof(buf)-1);
   	  		if(ret==0)
   	  		  {
   	  		    ev.data.fd=sockfd;
   	  		    ev.events=EPOLLIN|EPOLLET;
   	  		    epoll_ctl(epoll_instance,EPOLL_CTL_DEL,sockfd,&ev);
   	  		    close(sockfd);
   	  		    printf("del client\n");
   	  		    continue;
   	  		  }
			  //处理数据函数，可用线程
			  int *temp=(int *)malloc(sizeof(int));
			  *temp=sockfd;
			  pthread_create(&Client_ID,NULL,&ClientThread,temp);
   	  		  //write(sockfd,buf,ret);
			  //printf ("rec data is :%s\n",buf);
   	  		  //printf("write back to client.....\n");
		
   	     }
   	} 
    }
     close(sockfd);
     return 0;
}
