#include "main.h"
#include "encryption.h"
#include "websockets.h"
#include "tlzw.h"

#define MAX_PICTURE_SIZE 2048

void print_IR_pict_msg(unsigned char *recv_msg, int recv_msg_size){
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
	printf("5. Open interface for arm control\n\n");
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
	//Send some message to target
	send_msg(2, sock_pack);
}

void option_3(char **sock_pack){
	//Buffers
	unsigned char bmp_ir_pict_buff[MAX_PICTURE_SIZE] = {};
	unsigned char decomp_buf[MAX_PICTURE_SIZE];
	uint16_t out_buf[MAX_PICTURE_SIZE];

	//Variables for buffers
	uint16_t recv_msg_size = 0;
	uint16_t out_buf_len = 0;
	uint16_t decomp_buf_len = 0;

	//File variables
	FILE *bmp_file, *lzw_file, *decomp_file;

	//Opening picture files
	bmp_file = fopen("ir_pict.bmp", "w");
	lzw_file = fopen("ir_pict.lzw", "w");
	decomp_file = fopen("ir_pict_decomp.bmp", "w");

	//Receiving and decrypting IR picture
	recv_IR_pict(2, sock_pack, &bmp_ir_pict_buff[0], (int*)&recv_msg_size);
	CBC_decrypt(&bmp_ir_pict_buff[0], (int)recv_msg_size);

	//Compressing and decompressing picture buffer
	lzw_compress(&bmp_ir_pict_buff[0], &recv_msg_size, &out_buf[0], &out_buf_len);
	lzw_decompress(&out_buf[0], out_buf_len, &decomp_buf[0], &decomp_buf_len);

	//Writing normal, compressed and decompressed picture buffers in files
	fwrite(&out_buf[0], sizeof(unsigned char), out_buf_len, lzw_file);
	fwrite(&bmp_ir_pict_buff[0], sizeof(unsigned char), (int)recv_msg_size, decomp_file);
	fwrite(&decomp_buf[0], sizeof(unsigned char), decomp_buf_len, bmp_file);

	//Closing picture files
	fclose(bmp_file);
	fclose(lzw_file);
	fclose(decomp_file);
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
			//option_4(sock_pack);
		}else if(opt[0] == '5'){
			//option_5(sock_pack);
		}
	}

	return 0;
}
