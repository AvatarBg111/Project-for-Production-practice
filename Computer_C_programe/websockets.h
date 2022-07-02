#ifndef _WEBSOCKETS_H
#define _WEBSOCKETS_H

int check_ip(char*);
int send_msg(int, char**);
int send_arm_controls(int, char**, int, int);
int recv_IR_pict(int, char**, char*, int*);

#endif	//_WEBSOCKETS_H
