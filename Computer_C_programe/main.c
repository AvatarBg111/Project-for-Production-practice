#include<time.h>
#include "main.h"
#include "encryption.h"
#include "websockets.h"
#include "tlzw.h"

#define MAX_PICTURE_SIZE 2048

#define UNCOMPRESSED_IR_PICTURES_DIR "UNCOMP"
#define COMPRESSED_IR_PICTURES_DIR "COMP"
#define DECOMPRESSED_IR_PICTURES_DIR "DECOMP"

//Structure for cheking directory parameters
struct stat st_struct = {0};

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

void make_filepath(unsigned char *filepath, char *file_dir){
	//Structure for getting local time
	time_t _time = time(NULL);
	//Structure for getting system date and time
	struct tm curr_tm = {0};

	curr_tm = *localtime(&_time);
	int tm_mday = curr_tm.tm_mday;
	int tm_mon = curr_tm.tm_mon + 1;
	int tm_year = curr_tm.tm_year + 1900;

	int tm_sec = curr_tm.tm_sec;
	int tm_min = curr_tm.tm_min;
	int tm_hour = curr_tm.tm_hour;

	if(!strcmp(file_dir, UNCOMPRESSED_IR_PICTURES_DIR)){
		sprintf(filepath, "%s/ir_pict_%02d%02d%d_%02d%02d%02d.bmp", UNCOMPRESSED_IR_PICTURES_DIR, tm_mday, tm_mon, tm_year, tm_hour, tm_min, tm_sec);
	}else if(!strcmp(file_dir, COMPRESSED_IR_PICTURES_DIR)){
		sprintf(filepath, "%s/ir_pict_%02d%02d%d_%02d%02d%02d.lzw", COMPRESSED_IR_PICTURES_DIR, tm_mday, tm_mon, tm_year, tm_hour, tm_min, tm_sec);
	}else if(!strcmp(file_dir, DECOMPRESSED_IR_PICTURES_DIR)){
		sprintf(filepath, "%s/ir_pict_%02d%02d%d_%02d%02d%02d.bmp", DECOMPRESSED_IR_PICTURES_DIR, tm_mday, tm_mon, tm_year, tm_hour, tm_min, tm_sec);
	}else{
		system("clear");
		printf("Err:Uncorrect file directory given!\n");
		exit(0);
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
	unsigned char dir_name[50] = {};
	unsigned char file_path[60] = {};
	uint16_t out_buf[MAX_PICTURE_SIZE] = {};

	//Variables for buffers
	uint16_t recv_msg_size = 0;
	uint16_t out_buf_len = 0;

	//File variables
	FILE *bmp_file, *lzw_file;
	FILE *decomp_file = NULL;

	//Creating directories
	strcpy(dir_name, UNCOMPRESSED_IR_PICTURES_DIR);
	mkdir(dir_name, 0777);
	strcpy(dir_name, COMPRESSED_IR_PICTURES_DIR);
	mkdir(dir_name, 0777);

	//Opening picture files
	make_filepath(&file_path[0], UNCOMPRESSED_IR_PICTURES_DIR);
	bmp_file = fopen(file_path, "w");

	memset(file_path, 0, strlen(file_path));
	make_filepath(&file_path[0], COMPRESSED_IR_PICTURES_DIR);
	lzw_file = fopen(file_path, "w");
	decomp_file = fopen("ir_pict_decomp.bmp", "w");

	//Receiving and decrypting IR picture
	recv_IR_pict(2, sock_pack, &bmp_ir_pict_buff[0], (int*)&recv_msg_size);
	CBC_decrypt(&bmp_ir_pict_buff[0], (int)recv_msg_size);

	//Compressing and decompressing picture buffer
	lzw_compress(&bmp_ir_pict_buff[0], &recv_msg_size, &out_buf[0], &out_buf_len);

	//Writing normal and compressed picture buffers in files
	fwrite(&bmp_ir_pict_buff[0], sizeof(unsigned char), recv_msg_size, bmp_file);
	fwrite(&out_buf[0], sizeof(uint16_t), out_buf_len, lzw_file);

	//Closing picture files
	fclose(bmp_file);
	fclose(lzw_file);
}

void option_4(){
	FILE *comp_file = NULL, *decomp_file = NULL;

	uint16_t comp_buf[MAX_PICTURE_SIZE * 2] = {};
	uint16_t comp_buf_len = 0, decomp_buf_len = 0;
	unsigned char decomp_buf[MAX_PICTURE_SIZE * 2] = {};
	unsigned char filename[50] = {};
	unsigned char file_path[60] = {};
	unsigned char sys_command[strlen(COMPRESSED_IR_PICTURES_DIR) + 10];
	unsigned char dir_name[20] = {};

	//Printing directory contents of directory with compressed pictures
	system("clear");
	sprintf(sys_command, "ls %s", COMPRESSED_IR_PICTURES_DIR);
	printf("Contents in \'%s/\' directory:\n", COMPRESSED_IR_PICTURES_DIR);
	system(sys_command);
	printf("\n--------------------------------\n\n");

	//Get input file
	printf("Type in name of file from directory COMP: ");
	sprintf(file_path, "%s/", COMPRESSED_IR_PICTURES_DIR);
	while(1){
		scanf("%s", filename);
		strcat(file_path, filename);

		comp_file = fopen(file_path, "r");
		if(comp_file){
			break;
		}else{
			system("clear");
			printf("Try again: ");
			sprintf(file_path, "%s/", COMPRESSED_IR_PICTURES_DIR);
		}
	}

	//Get input file size
	fseek(comp_file, 0, SEEK_END);
	comp_buf_len = ftell(comp_file);
	fseek(comp_file, 0, SEEK_SET);

	//Get input data
	fread(comp_buf, sizeof(uint16_t), comp_buf_len, comp_file);
	fclose(comp_file);

	//Open output file
	memset(file_path, 0, strlen(COMPRESSED_IR_PICTURES_DIR) + 10);
	make_filepath(&file_path[0], DECOMPRESSED_IR_PICTURES_DIR);
	decomp_file = fopen(file_path, "w");

	//Make output directory
	strcpy(dir_name, DECOMPRESSED_IR_PICTURES_DIR);
	mkdir(dir_name, 0777);

	//Write output data to output file
	lzw_decompress(&comp_buf[0], comp_buf_len, &decomp_buf[0], &decomp_buf_len);
	fwrite(decomp_buf, sizeof(unsigned char), (int)decomp_buf_len, decomp_file);

	//Close input and output files
	fclose(decomp_file);
}

void option_5(char **sock_pack){
	int control_arm[2];
	int chosen_arm_movement = 0;
	int option = 0;

	while(1){
		system("clear");
		printf("1.Lift/lower arm\n2.Push/pull arm\n3.Exit control arm menu\nOption (Type in option number): ");
		while(1){
			scanf("%d", &option);
			if(option == 1){
				chosen_arm_movement = option;
				break;
			}else if(option == 2){
				chosen_arm_movement = option;
				break;
			}else if(option == 3){
				return;
			}else{
				printf("Try again: ");
			}
		}

		printf("\nType in angle of movement (between 45 and 135): ");
		while(1){
			scanf("%d", &control_arm[chosen_arm_movement - 1]);
			if(control_arm[chosen_arm_movement - 1] <= 135 && control_arm[chosen_arm_movement - 1] >= 45){
				break;
			}else{
				printf("Try again: ");
			}
		}
		send_arm_controls(2, sock_pack, chosen_arm_movement, control_arm[chosen_arm_movement - 1]);
	}
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
			option_4();
		}else if(opt[0] == '5'){
			option_5(sock_pack);
		}
	}

	return 0;
}
