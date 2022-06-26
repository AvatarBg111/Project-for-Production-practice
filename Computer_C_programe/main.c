#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "main.h"
#include "encryption.h"
#include "websockets.h"
#include "lzw.h"

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
	printf("3. Send random message\n");
	printf("4. Get picture from IR camera on target\n");
	printf("5. Decompress IR picture\n");
	printf("6. Open interface for arm control\n");
	printf("7. USE CBC\n");
	printf("8. USE LZW\n\n");
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
	/*char recv_msg[MAX_PICTURE_SIZE] = {};
	int recv_msg_size = 0;

	recv_IR_pict(2, sock_pack, &recv_msg[0], &recv_msg_size);

	getchar();
	print_IR_pict_msg(&recv_msg[0], recv_msg_size);
	getchar();

	CBC_decrypt(&recv_msg[0], recv_msg_size);

	getchar();
	print_IR_pict_msg(&recv_msg[0], recv_msg_size);
	getchar();*/
}

void option_4(char **sock_pack){
	char bmp_ir_pict_buff[MAX_PICTURE_SIZE] = {};
	int recv_msg_size = 0;
	FILE *bmp_file, *lzw_file;

	bmp_file = fopen("ir_pict.bmp", "w");
	lzw_file = fopen("ir_pict.lzw", "w");

	recv_IR_pict(2, sock_pack, &bmp_ir_pict_buff[0], &recv_msg_size);
	CBC_decrypt(&bmp_ir_pict_buff[0], recv_msg_size);

	//  //
	uint16_t out_buf[recv_msg_size];
	unsigned char decomp_buf[recv_msg_size];
	uint16_t out_buf_len = 0;
	uint16_t decomp_buf_len = 0;
	//  //

	print_IR_pict_msg(&bmp_ir_pict_buff[0], recv_msg_size);
	lzw_compress(&bmp_ir_pict_buff[0], recv_msg_size, &out_buf[0], &out_buf_len);
	fwrite(&out_buf[0], sizeof(char), out_buf_len, lzw_file);

	lzw_decompress(&out_buf[0], out_buf_len, &decomp_buf[0], &decomp_buf_len);
	//option_8(&bmp_ir_pict_buff[0], recv_msg_size);

	fwrite(&decomp_buf[0], sizeof(char), decomp_buf_len, bmp_file);
	fclose(bmp_file);
	fclose(lzw_file);
}

void option_7(){
	unsigned char msg[] = "Hello! I\'m Baymax, your personal healthcare companion.";
	int msg_size = strlen(msg);

	print_IR_pict_msg(&msg[0], msg_size);
	CBC_encrypt(&msg[0], msg_size);
	print_IR_pict_msg(&msg[0], msg_size);
	CBC_decrypt(&msg[0], msg_size);
	print_IR_pict_msg(&msg[0], msg_size);

	getchar();
	getchar();
}

void option_8(char *string, int string_len){
	uint16_t out_buf[string_len];
	unsigned char decomp_buf[string_len];
	uint16_t out_buf_len = 0;
	uint16_t decomp_buf_len = 0;

	//system("clear");
	lzw_compress(&string[0], string_len, &out_buf[0], &out_buf_len);
	printf("Compressed string(%x):\n[", out_buf_len);
	for(int i = 0; i < out_buf_len; i++){
		if(i < out_buf_len - 1)
			printf("%u, ", out_buf[i]);
		else
			printf("%u]\n", out_buf[i]);
	}

	lzw_decompress(&out_buf[0], out_buf_len, &decomp_buf[0], &decomp_buf_len);
	/*printf("\n\n\nDecompressed string(%x):\n[", decomp_buf_len);
	for(int i = 0; i < decomp_buf_len; i++){
		if(i < out_buf_len - 1)
			printf("%u, ", decomp_buf[i]);
		else
			printf("%u]\n", decomp_buf[i]);
	}*/

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
			//option_3(sock_pack);
		}else if(opt[0] == '4'){
			option_4(sock_pack);
		}else if(opt[0] == '5'){
			//option_5(sock_pack);
		}else if(opt[0] == '6'){
			//option_6();
		}else if(opt[0] == '7'){
			option_7();
		}else if(opt[0] == '8'){
			//option_8();
		}
	}

	return 0;
}
