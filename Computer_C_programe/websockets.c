//Includes
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

//Definitions
#define SERVER_PORT 8080
#define MAXLINE 2048
#define MAX_READ_LEN 1024
#define SA struct sockaddr

//Function for checking if input ip is right
int check_ip(char *target_ip_str){
	struct sockaddr_in servaddr;

	if(inet_pton(AF_INET, target_ip_str, &servaddr.sin_addr) <= 0){
		printf("Could not turn string into readable ip!");
		return -1;
	}
	return 0;
}

//Function for sending some message to ESP32 to see if the connection is established
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

//Function for sending controls for arm on ESP32
int send_arm_controls(int argc, char **argv, int arm_movement, int angle){
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
	if(arm_movement == 1)
		sprintf(sendline, "%c%c00", 0b0100, (unsigned char)angle);
	else if(arm_movement == 2)
		sprintf(sendline, "%c%c00", 0b0010, (unsigned char)angle);
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

//Function for requesting and receiving picture from IR camera on ESP32
int recv_IR_pict(int argc, char **argv, char *recv_msg, int *recv_msg_size){
	//Initialize function parameters
	int sockfd, n = 1;
	int sendbytes;
	struct sockaddr_in servaddr;
	char sendline[MAXLINE] = {};
	char recvline[MAXLINE] = {};
	*recv_msg_size = 0;

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

	//Write command to target
	sprintf(sendline, "%c000", 0b0001);
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
		//printf("Packet %d: len - %d\n", ++packet_num, n);

		for(int i = *recv_msg_size; i < n + *recv_msg_size; i++){
			recv_msg[i] = recvline[i - *recv_msg_size];
		}
		*recv_msg_size += n;

		if(n < 0){
			printf("Error while reading response!\n");
		}
		memset(recvline, 0, n);
		if(n < MAX_READ_LEN)
			break;
	}

	if(n < 0){
		system("clear");
		printf("Error while reading response!\n");
		return -1;
		//exit(0);
	}

	close(sockfd);

	return 0;
}
