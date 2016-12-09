#ifndef __LIST_H_
#define __LIST_H_
#include<stdbool.h>
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

#define  true     1
#define  false    0

typedef struct node{
	int fd_data;
	struct node* pNext;
}Node;

Node *head ;
extern bool createNodelist();
extern bool  addNode(Node * node);
extern bool deleteNode(int index);

#endif