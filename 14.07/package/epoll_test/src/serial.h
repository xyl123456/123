#ifndef __SERIAL_H__
#define __SERIAL_H__

extern int open_port(int fd,int comport);
extern int set_opt(int fd,int nSpeed, int nBits, char nEvent, int nStop);

#endif
