#include "encryption.h"

#define KEYSIZE 10

unsigned char KEY[KEYSIZE] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50};
unsigned char IV[KEYSIZE] = {219, 128, 87, 73, 139, 125, 159, 132, 91, 193};

void _encrypt(unsigned char *msg, int msglen){
	for(int i = 0; i < msglen; i++){
		msg[i] = (unsigned char)(msg[i] ^ KEY[i % 9]);
	}
}

void _decrypt(unsigned char *msg, int msglen){
	for(int i = 0; i < msglen; i++){
		msg[i] = (unsigned char)(msg[i] ^ KEY[i % 9]);
	}
}

void print_message(unsigned char *msg, int msglen){
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

void CBC_xor(unsigned char *msg, unsigned char *xor_vector, int block_size){
	for(int i = 0; i < block_size; i++)
		msg[i] = (unsigned char)(msg[i] ^ xor_vector[i]);
}

void CBC_block_encr(unsigned char *msg, int block_size){
	_encrypt(msg, block_size);
}

void CBC_block_decr(unsigned char *msg, int block_size){
	_decrypt(msg, block_size);
}

void CBC_encrypt(unsigned char *msg, int msglen){
	int block_idx = 0;
	int block_size = 0;

	for(int i = 0; i < msglen; i++){
		if((msglen - block_idx) > KEYSIZE){
			block_size = KEYSIZE;
		}else{
			block_size = msglen - block_idx;
		}

		if(i == 0){
			CBC_xor(&msg[0], &IV[0], block_size);
			CBC_block_encr(&msg[0], block_size);
		}else{
			CBC_xor(&msg[block_idx], &msg[block_idx - KEYSIZE], block_size);
			CBC_block_encr(&msg[block_idx], block_size);
		}

		block_idx += block_size;
		i = block_idx;
	}
}

void CBC_decrypt(unsigned char *msg, int msglen){
	int block_idx = 0;
	int block_size = 0;
	unsigned char curr_block_vector[KEYSIZE] = {};
	unsigned char last_block_vector[KEYSIZE] = {};

	for(int i = 0; i < msglen; i++){
		if((msglen - block_idx) > KEYSIZE){
			block_size = KEYSIZE;
		}else{
			block_size = msglen - block_idx;
		}

		if(i == 0){
			for(int n = 0; n < block_size; n++)
				last_block_vector[n] = msg[block_idx + n];
			CBC_block_decr(&msg[0], block_size);
			CBC_xor(&msg[0], &IV[0], block_size);
		}else{
			for(int n = 0; n < block_size; n++)
				curr_block_vector[n] = last_block_vector[n];
			for(int n = 0; n < block_size; n++)
				last_block_vector[n] = msg[block_idx + n];
			CBC_block_decr(&msg[block_idx], block_size);
			CBC_xor(&msg[block_idx], &curr_block_vector[0], block_size);
		}

		block_idx += KEYSIZE;
		i = block_idx;
	}
}



