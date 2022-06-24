#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "main.h"
#include "encryption.h"
#include "websockets.h"

#define MAX_PICTURE_SIZE 2048

void print_IR_pict_msg(unsigned char *recv_msg, int recv_msg_size){
	//system("clear");
	printf("IR picture string: \n[");
	for(int i = 0; i < recv_msg_size; i++){
		if(i < recv_msg_size - 1)
			printf("%c, ", recv_msg[i]);
		else
			printf("%c]\n", recv_msg[i]);
	}

	printf("\nIR picture byte dump: \n[");
	for(int i = 0; i < recv_msg_size; i++){
		if(i < recv_msg_size - 1)
			printf("%u, ", recv_msg[i]);
		else
			printf("%u]\n", recv_msg[i]);
	}
}

void print_options(){
	system("clear");
	printf("1. Type in target ip\n");
	printf("2. Connect to target\n");
	printf("3. Get picture from IR camera on target\n");
	printf("4. Decompress IR picture\n");
	printf("5. Open interface for arm control\n");
	printf("6. USE CBC\n\n");
	printf("Option (Type in option number): ");
}

void option_1(char **sock_pack){
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

void option_2(char **sock_pack){
	send_msg(2, sock_pack);
}

void option_3(char **sock_pack){
	char recv_msg[MAX_PICTURE_SIZE] = {};
	int recv_msg_size = 0;

	recv_IR_pict(2, sock_pack, &recv_msg[0], &recv_msg_size);

	getchar();
	print_IR_pict_msg(&recv_msg[0], recv_msg_size);
	getchar();

	CBC_decrypt(&recv_msg[0], recv_msg_size);

	getchar();
	print_IR_pict_msg(&recv_msg[0], recv_msg_size);
	getchar();
}

void option_6(){
	unsigned char recv_msg[] = "Hello! I\'m Baymax, your personal healthcare companion.";
	int recv_msg_size = strlen(recv_msg);

	print_IR_pict_msg(&recv_msg[0], recv_msg_size);
	CBC_encrypt(&recv_msg[0], recv_msg_size);
	print_IR_pict_msg(&recv_msg[0], recv_msg_size);
	CBC_decrypt(&recv_msg[0], recv_msg_size);
	print_IR_pict_msg(&recv_msg[0], recv_msg_size);

	getchar();
	getchar();
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
			option_1(sock_pack);
		}else if(opt[0] == '2'){
			option_2(sock_pack);
		}else if(opt[0] == '3'){
			option_3(sock_pack);
		}else if(opt[0] == '4'){
		}else if(opt[0] == '5'){
		}else if(opt[0] == '6'){
			option_6();
		}
	}

	return 0;
}
