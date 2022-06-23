#include<stdio.h>
#include<string.h>
#include "encryption.h"

char KEY[10] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50};

void encrypt(char *msg, int msglen){
	for(int i = 0; i < msglen; i++){
		msg[i] ^= KEY[i % 9];
	}
}

void decrypt(char *msg, int msglen){
	for(int i = 0; i < msglen; i++){
		msg[i] ^= KEY[i % 9];
	}
}

void print_message(char *msg, int msglen){
	printf("Message: [");
	for(int i = 0; i < msglen; i++){
		if(i < msglen - 1)
			printf("%c, ", msg[i]);
		else
			printf("%c]\n", msg[i]);
	}

	printf("Bytes: [");
	for(int i = 0; i < msglen; i++){
		if(i < msglen - 1)
			printf("%d, ", msg[i]);
		else
			printf("%d]\n", msg[i]);
	}
}
