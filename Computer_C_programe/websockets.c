#include<sys/socket.h>
#include<sys/types.h>
#include<signal.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<stdarg.h>
#include<errno.h>
#include<fcntl.h>
#include<sys/time.h>
#include<sys/ioctl.h>
#include<netdb.h>
#include "websockets.h"

#define SERVER_PORT 8080
#define MAXLINE 4096
#define SA struct sockaddr

int check_ip(char *target_ip_str){
	struct sockaddr_in servaddr;

	if(inet_pton(AF_INET, target_ip_str, &servaddr.sin_addr) <= 0){
		printf("Could not turn string into readable ip!");
		return -1;
	}
	return 0;
}

//int main(int argc, char **argv){
int send_msg(int argc, char **argv){
	//Initialize function parameters
	int sockfd, n = 1;
	int sendbytes;
	struct sockaddr_in servaddr;
	char sendline[MAXLINE];
	char recvline[MAXLINE];

	//Checking received parameters
	if(argc != 2){
		system("clear");
		printf("Invalid count of arguments!\n");
		return -1;
		//exit(0);
	}

	//Creating socket entity
	if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		system("clear");
		printf("Could not create socket struct!\n");
		return -1;
		//exit(0);
	}

	//Initializing socket entity parameters
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);

	//Turning target ip string into special struct
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
		system("clear");
		printf("Could not turn port into readable port!\n");
		return -1;
		//exit(0);
	}

	//Creating connection
	if(connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0){
		system("clear");
		printf("Could not connect!\n");
		return -1;
		//exit(0);
	}

	//Write message to target
	sprintf(sendline, "Hello from computer programe!");
	sendbytes = strlen(sendline);

	if(write(sockfd, sendline, sendbytes) != sendbytes){
		system("clear");
		printf("Could not send message!\n");
		return -1;
		//exit(0);
	}

	//Reading and printing response
	memset(recvline, 0, MAXLINE);
	int packet_num = 0;

	printf("Waiting for response...\n");
	while(n > 0){
		n = read(sockfd, recvline, MAXLINE);
		printf("Packet %d: %s\n", ++packet_num, recvline);
		if(n < 0){
			printf("Error while reading response!\n");
		}
		memset(recvline, 0, 50);
		if(n < MAXLINE)
			break;
	}
	printf("\nResponse printed.\n");

	if(n < 0){
		system("clear");
		printf("Error while reading response!\n");
		return -1;
		//exit(0);
	}

	close(sockfd);

	return 0;
}
