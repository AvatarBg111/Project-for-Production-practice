#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "main.h"
#include "encryption.h"
#include "websockets.h"

char some_msg[] = "Hello, World!";

void print_options(){
	system("clear");
	printf("1. Type in target ip\n");
	printf("2. Connect to target\n");
	printf("3. Open interface for arm control\n");
	printf("4. Get picture from IR camera on target\n");
	printf("5. Encrypt message\n");
	printf("6. Decrypt message\n");
	printf("7. Decompress IR picture\n\n");
	printf("Option (Type in option number): ");
}

void option_2(char **sock_pack){
	send_msg(2, sock_pack);
}

void option_5(char *msg, int msglen){
	char message[msglen];

	for(int i = 0; i < msglen; i++)
		message[i] = msg[i];
	encrypt(&message[0], msglen);

	system("clear");
	getchar();
	print_message(&message[0], msglen);
	getchar();
}

void option_6(char *msg, int msglen){
	char message[msglen];

	for(int i = 0; i < msglen; i++)
		message[i] = msg[i];
	encrypt(&message[0], msglen);

	system("clear");
	getchar();
	print_message(&message[0], msglen);
	getchar();
}

void set_target(char **sock_pack){
	int TARGET_STR_LEN = 30;
	char target[TARGET_STR_LEN];

	system("clear");
	printf("Type in target ip: ");
	while(1){
		scanf("%s", target);

		if(!check_ip(&target[0])){
			break;
		}else{
			memset(target, 0, TARGET_STR_LEN);
			printf("\nTry again: ");
		}
	}

	if(sock_pack[1] != NULL){
		free(sock_pack[1]);
	}
	sock_pack[1] = (char*)calloc(strlen(target), sizeof(char));
	strcpy(sock_pack[1], target);

	system("clear");
}

int main(){
	//Defining parameters
	char opt[10] = {};
	char **sock_pack;

	//Zeroing parameters
	sock_pack = (char**)calloc(2, sizeof(char*));
	sock_pack[0] = NULL;
	sock_pack[1] = NULL;

	//Main cycle with menu
	while(1){
		print_options();
		scanf("%s", opt);

		if(opt[1] == 't'){
			break;
		}else if(opt[0] == '1'){
			set_target(sock_pack);
		}else if(opt[0] == '2'){
			option_2(sock_pack);
		}else if(opt[0] == '3'){
		}else if(opt[0] == '4'){
		}else if(opt[0] == '5'){
			option_5(&some_msg[0], strlen(some_msg));
		}else if(opt[0] == '6'){
		}
	}

	return 0;
}
