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

#define SERVER_PORT 8080
#define MAXLINE 4096
#define SA struct sockaddr

int main(int argc, char **argv){
	int sockfd, n = 1;
	int sendbytes;
	struct sockaddr_in servaddr;
	char sendline[MAXLINE];
	char recvline[MAXLINE];

	if(argc != 2){
		system("clear");
		printf("Invalid count of arguments!\n");
		exit(0);
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		system("clear");
		printf("Could not create socket struct!\n");
		exit(0);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);

	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0){
		system("clear");
		printf("Could not turn port into readable port!\n");
		exit(0);
	}

	if(connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0){
		system("clear");
		printf("Could not connect!\n");
		exit(0);
	}

	sprintf(sendline, "Hello from computer programe!");
	sendbytes = strlen(sendline);

	if(write(sockfd, sendline, sendbytes) != sendbytes){
		system("clear");
		printf("Could not send message!\n");
		exit(0);
	}

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
		exit(0);
	}

	printf("Hello, World!\n");
	return 0;
}

