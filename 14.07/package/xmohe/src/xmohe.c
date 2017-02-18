#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <termios.h>
#include <errno.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <pthread.h>
#include<stdbool.h>
#include<signal.h>
#include<sys/wait.h>
#include "serial.h"
#include "getcfg.h"
#include "list.h"



int sockread_len;//�ͻ��˶�ȡ���ֽ���

char ipaddr[16];//���ڴ�Ż�ȡ��Ip��ַ
char dns_addr[50];//���ڴ�Ŵ��Զ��������ַ
int ser_port;//Զ�̶˿�
int client_port;//���ض˿�
char RS232_BUF;//���ڶ˿�
int RS232_BAUD;//���ڲ�����

int serial_fd;//����FD
int recv_cnt=0;
bool uart_flag=0;
bool read_flag=0;

int serial_readtmp=0;

int read_tmp=0;
int send_tmp=0;


int clidenFd;

bool connect_error=false;

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
void Sleep_ms(int ms)  
{  
    struct timeval delay;  
    delay.tv_sec = 0;  
    delay.tv_usec = ms * 1000; // 20 ms  
    select(0, NULL, NULL, NULL, &delay);  
}  
void* SerialThread(){
	int read_cont;//��ȡ�����ڵ��ֽ���
    	char serial_buf[128];//���ڴ�Ŵ��ڵ�����
    	int serial_read_cnt=0;	
	int read_flag=0;
	int serial_cnt=0;
	memset(serial_buf,0,sizeof(serial_buf));
	while(1){
				Sleep_ms(500);
				if((read_cont=read(serial_fd,serial_buf,sizeof(serial_buf)))>0)
					{
				        //serialdata_handle(serial_buf,read_cont);
				        int write_cnd=0;
				        write_cnd= write(clidenFd,serial_buf,read_cont);
					printf("rev count is %d\n",read_cont);
					memset(serial_buf,0,sizeof(serial_buf));
					read_cont=0;
					}
				Sleep_ms(500);
						
		}
}

int main()
{
  pthread_t Serial_ID;
  signal(SIGCHLD,handle_sigchld);
  createNodelist();  //�����������ڴ洢FD
  //���ڳ�ʼ����ȡ·��ֵ���ô����Լ�Զ�����Ӷ˿�
  GetConfigValue("/etc/config/rs232" );
  //GetConfigValue("/home/dave/test/xmohe/src/rs232" );
  printf("client ipport is %d\n",client_port);//���ض˿�
 // printf("server ipport is %d\n",ser_port);//Զ�̶˿�
 // printf("the server ip is %s\n",ipaddr);
   /*����epoll_event�ṹ��ı�����ev����ע���¼���events
   �������ڻش�Ҫ������¼�*/
   struct epoll_event ev,events[20];//���������ڻش����յ��¼�
   /*�������ڴ���accept��epollר��
   ���ļ���������ָ�����������������ΧΪ256*/
  int epoll_instance=epoll_create(64);//size is unused nowadays

  int  listenfd = socket(AF_INET,SOCK_STREAM,0);
   if(listenfd <0)
   {
      perror("error opening socket");
      return -1;
   }
   setnonblocking(listenfd);//�����ڼ�����socket���óɷ�������ʽ
   ev.data.fd=listenfd;//������Ҫ������¼���ص��ļ�������
   ev.events=EPOLLIN|EPOLLET;//����Ҫ������¼�����
   epoll_ctl(epoll_instance,EPOLL_CTL_ADD,listenfd,&ev);//ע��epoll�¼�
/*
���ط��������ã��󶨶˿ڣ�����ǿͻ���ֱ�Ӷ�ȡԶ�̵�ip
*/
   struct sockaddr_in client_addr;
   memset(&client_addr,0,sizeof(client_addr));
   client_addr.sin_family = AF_INET;
   client_addr.sin_addr.s_addr = INADDR_ANY;
   //client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
   client_addr.sin_port = htons((uint16_t )client_port);//���ض˿�
   //client_addr.sin_port = htons(1234);//���ض˿�
   long opt = 1;
   socklen_t sizeInt_len = sizeof(long);
  if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(char *)&opt,sizeInt_len)==-1)
  	{       
  	perror("setsockopt");       
	return -1;    
	}
  int ret = bind(listenfd,(struct sockaddr*)&client_addr,sizeof(client_addr));
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

//�����豸�ļ�
	if('1'==RS232_BUF)
		{
		  if(( serial_fd = open_port(serial_fd, 0))<0)
 			{
 			perror("open_port error");
			return;
 			}
		int k;
   		if(( k=set_opt(serial_fd,RS232_BAUD,8,0,1))<0)
			{
			perror("set_opt error\n");
			return;
			}
		if(serial_fd>2)
			{
			/*
			int *temp=(int *)malloc(sizeof(int));
			*temp=serial_fd;
			pthread_create(&Serial_ID,NULL,&SerialThread,NULL);
			*/
			ev.data.fd=serial_fd;
   			ev.events=EPOLLIN|EPOLLET;
   			epoll_ctl(epoll_instance,EPOLL_CTL_ADD,serial_fd,&ev);
			}
		}
   while(1)
   {
    int nfound=epoll_wait(epoll_instance,events,20,1000);//�ȴ�epoll�¼��ķ���,�����¼���
   	  if(nfound==0)
   	  {
   	  	//printf(".");
   	  	fflush(stdout);
		continue;
   	  }
	int n_poll;
    for(n_poll=0;n_poll<nfound;n_poll++)
   	  {
   	  	if(events[n_poll].data.fd==listenfd)/*���ض˿����µ�����*/
   	  	{
   	  		struct sockaddr_in cliaddr;//���ӵ����صĿͻ���
   	  		uint32_t  len = sizeof(cliaddr);
   	  		int connfd=accept(listenfd,(struct sockaddr*)&cliaddr,&len);
   	  		printf("connection from host %s,port %d,sockfd is %d\n",
             		inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port),connfd);
   	  		setnonblocking(connfd);
   	  		  
   	  		  ev.data.fd=connfd;//�������ڶ��������ļ�������
   	  		  ev.events=EPOLLIN|EPOLLET;//��������ע��Ķ������¼�
   	  		  epoll_ctl(epoll_instance,EPOLL_CTL_ADD,connfd,&ev);//ע��ev�¼�
			  printf("add %d to epoll\n",connfd);
			  
			clidenFd=connfd;
			//info_data.socfd=connfd;
			//printf("connect is %d\n,info_data.socfd");
			//���ϵͳ��Ϣ
			//��ӵ�����
			Node * clientnode=(Node*)malloc(sizeof(Node));
			clientnode->fd_data=connfd;
			clientnode->pNext=NULL;
			addNode(clientnode);
   	  	}
		else if(events[n_poll].events&EPOLLIN)/*���¼�*/
			if(events[n_poll].data.fd==serial_fd)
				{
				Sleep_ms(500);
				int read_cont;//��ȡ�����ڵ��ֽ���
    				char serial_buf[128];//���ڴ�Ŵ��ڵ�����
				memset(serial_buf,0,sizeof(serial_buf));
				read_cont=read(serial_fd,serial_buf,sizeof(serial_buf));
				        //serialdata_handle(serial_buf,read_cont);
				        int write_cnd=0;
				        write_cnd= write(clidenFd,serial_buf,read_cont);
					printf("rev count is %d\n",read_cont);
					memset(serial_buf,0,sizeof(serial_buf));
					read_cont=0;
				}
   	  		else{ 		
					//�ж��Ƿ���SOCKET�¼�
					int sockfd=events[n_poll].data.fd;
					char cliend_buf[128];//�������ݻ������SOCKET����
					recv_cnt=0;
   	  				memset(cliend_buf,0,sizeof(cliend_buf));
					sockread_len=read(sockfd,cliend_buf,sizeof(cliend_buf));
   	  				if(sockread_len<=0)
   	  		 			 {
   	  		   	  			ev.data.fd=sockfd;
   	  		  	  			ev.events=EPOLLIN|EPOLLET;
   	  		   	 			epoll_ctl(epoll_instance,EPOLL_CTL_DEL,sockfd,&ev);
							if(deleteNode(sockfd))
				 			printf("delete the list\n");
   	  		   	  			close(sockfd);
   	  		  	  			printf("del client\n");
							memset(cliend_buf,0,sizeof(cliend_buf));
   	  		 			 }
					else{	
				 		 //�������ݺ�
				  			recv_cnt=write(serial_fd,cliend_buf,sockread_len);
						 	printf("rcv socket is %d\n",sockfd);
							printf("rcv socket:%s the counut is %d\n",cliend_buf,recv_cnt);
							memset(cliend_buf,0,sizeof(cliend_buf));
							recv_cnt=0;
   	  		 			 }
   	     		}
   		} 
    	}
   pthread_join(Serial_ID,NULL);
   return 0;
}
