#include "lzw.h"

//Static dictionary
unsigned char dict_strings[DICT_SIZE][DICT_STR_MAXLINE];
uint16_t strs_sizes[DICT_SIZE], codes[DICT_SIZE];

//Print dictionary
void print_dict(uint16_t size){
	uint16_t index = size - 256;

	printf("\n");
	for(uint16_t i = 0; i < index; i++){
		printf("{");
		for(uint16_t n = 0; n < strs_sizes[i]; n++){
			printf("%u, ", dict_strings[i][n]);
		}
		printf("}(%u)  ", codes[i]);
	}
	printf("\n");
}

void print_dict_spec(uint16_t size, uint16_t *strs_sizess, uint16_t *codess){
	uint16_t index = size - 256;

	printf("\n");
	for(uint16_t i = 0; i < index; i++){
		printf("{");
		for(uint16_t n = 0; n < strs_sizess[i]; n++){
			printf("%u, ", dict_strings[i][n]);
		}
		printf("}(%u)  ", codess[i]);
	}
	printf("\n");
}

//Receives string and returns if it's present in the dictionary
uint16_t dict_contains(uint16_t *dict_strs_sizes, uint16_t *dict_size, unsigned char new_char, unsigned char *encd_str, uint16_t encd_str_len){
	uint16_t n = 0;

	encd_str[encd_str_len++] = new_char;

	for(uint16_t i = 0; i < *dict_size; i++){
		if(dict_strs_sizes[i] == encd_str_len){
			for(n; n < encd_str_len; n++){
				if(encd_str[n] != dict_strings[i][n])
					break;
			}
			if(n == encd_str_len)
				return 1;
			n = 0;
		}
	}

	return 0;
}

//Receives string from dictionary and returns it's code value
uint16_t str_to_code(uint16_t *dict_strs_sizes, uint16_t *dict_codes, uint16_t *dict_size, unsigned char *encd_str, uint16_t encd_str_len){
	uint16_t n = 0;

	for(int i = 0; i < *dict_size; i++){
		if(dict_strs_sizes[i] == encd_str_len){
			for(n; n < encd_str_len; n++){
				if(encd_str[n] != dict_strings[i][n])
					break;
			}
			if(n == encd_str_len){
				return dict_codes[i];
			}
			n = 0;
		}
	}

	return 0;
}

void add_to_dict(uint16_t *dict_strs_sizes, uint16_t *dict_codes, uint16_t *dict_size, uint16_t *last_spec_code, unsigned char *encd_str, uint16_t encd_str_len){
	(*dict_size)++;

	memset(dict_strings[(*dict_size) - 1], 0, DICT_STR_MAXLINE);
	for(uint16_t i = 0; i < encd_str_len; i++)
		dict_strings[(*dict_size) - 1][i] = encd_str[i];

	dict_strs_sizes[(*dict_size) - 1] = encd_str_len;

	if(encd_str_len == 1 || encd_str_len == 0)
		dict_codes[(*dict_size) - 1] = encd_str[0];
	else
		dict_codes[(*dict_size) - 1] = ++(*last_spec_code);
}

void lzw_compress(unsigned char *msg, uint16_t msglen, uint16_t *out_buf, uint16_t *out_buf_len){
	//Variables needed for programe
	uint16_t dict_strs_sizes[DICT_SIZE], dict_codes[DICT_SIZE], last_spec_code = 255;
	unsigned char encd_str[DICT_STR_MAXLINE] = {}, new_char;
	uint16_t while_loop_start_flag = 1, encd_str_len = 0, dict_size = 0, msg_indx = 0;

	while(while_loop_start_flag || msg_indx <= msglen - 1){
		if(while_loop_start_flag)	while_loop_start_flag = 0;

		//Load new character to "new char"
		new_char = msg[msg_indx++];

		if(dict_contains(&dict_strs_sizes[0], &dict_size, new_char, &encd_str[0], encd_str_len) == 1){
			//Add new char to encoded string
			encd_str[encd_str_len++] = new_char;
		}else{
			//Load encoded string code into output buffer
			if(encd_str_len == 1){
				out_buf[(*out_buf_len)] = encd_str[0];
				(*out_buf_len)++;
			}else if(encd_str_len > 1){
				out_buf[*out_buf_len] = str_to_code(&dict_strs_sizes[0], &dict_codes[0], &dict_size, &encd_str[0], encd_str_len);
				if(out_buf[*out_buf_len] > 3000)
					printf("\n\nhere 1\n\n");
				(*out_buf_len)++;
			}

			//Add new char to string
			++encd_str_len;
			encd_str[encd_str_len - 1] = new_char;

			//Add encoded string to dictionary
			add_to_dict(&dict_strs_sizes[0], &dict_codes[0], &dict_size, &last_spec_code, &encd_str[0], encd_str_len);

			//Delete encoded string and add the new char to it
			memset(encd_str, 0, encd_str_len);
			encd_str_len = 1;
			encd_str[0] = new_char;
		}
	}

	//Load last encoded string code into output buffer
	if(encd_str_len == 1){
		out_buf[(*out_buf_len)] = encd_str[0];
		(*out_buf_len)++;
	}else if(encd_str_len > 1){
		out_buf[*out_buf_len] = str_to_code(&dict_strs_sizes[0], &dict_codes[0], &dict_size, &encd_str[0], encd_str_len);
		if(out_buf[*out_buf_len] > 3000)
			printf("\n\nhere 2\n\n");
		(*out_buf_len)++;
	}

	print_dict_spec(300, &dict_strs_sizes[0], &dict_codes[0]);
}

uint16_t create_new_symbol_to_dict(uint16_t *prev_code, uint16_t dict_cnt){
	codes[dict_cnt - 254] = dict_cnt + 1;

	if(*prev_code <= 255)
		strs_sizes[dict_cnt - 254] = 2;
	else
		strs_sizes[dict_cnt - 254] = strs_sizes[*prev_code - 255] + 1;

	for(int i = 0; i < strs_sizes[dict_cnt - 254] - 1; i++){
		if(*prev_code <= 255)
			dict_strings[dict_cnt - 254][i] = *prev_code;
		else
			dict_strings[dict_cnt - 254][i] = dict_strings[*prev_code - 255][i];
	}

	if(*prev_code <= 255)
		dict_strings[dict_cnt - 254][strs_sizes[dict_cnt - 254] - 1] = *prev_code;
	else
		dict_strings[dict_cnt - 254][strs_sizes[dict_cnt - 254] - 1] = dict_strings[*prev_code - 255][0];

	return ++dict_cnt;
}

uint16_t compress(unsigned char *msg, uint16_t msglen){
	//Variables needed for programe
	//uint16_t strs_sizes[DICT_SIZE], codes[DICT_SIZE];
	uint16_t last_spec_code = 255;
	unsigned char encd_str[DICT_STR_MAXLINE] = {}, new_char;
	uint16_t while_loop_start_flag = 1, encd_str_len = 0, dict_size = 0, msg_indx = 0;

	while(while_loop_start_flag || msg_indx <= msglen - 1){
		if(while_loop_start_flag)	while_loop_start_flag = 0;

		//Load new character to "new char"
		new_char = msg[msg_indx++];

		if(dict_contains(&strs_sizes[0], &dict_size, new_char, &encd_str[0], encd_str_len) == 1){
			//Add new char to encoded string
			encd_str[encd_str_len++] = new_char;
		}else{
			//Add new char to string
			++encd_str_len;
			encd_str[encd_str_len - 1] = new_char;

			//Add encoded string to dictionary
			add_to_dict(&strs_sizes[0], &codes[0], &dict_size, &last_spec_code, &encd_str[0], encd_str_len);

			//Delete encoded string and add the new char to it
			memset(encd_str, 0, encd_str_len);
			encd_str_len = 1;
			encd_str[0] = new_char;
		}
	}
	return last_spec_code;
}

void lzw_decompress(uint16_t *msg, uint16_t msglen, unsigned char *ret_buf, uint16_t *ret_buf_len){
	uint16_t while_loop_start_flag = 1, msg_indx = 0;
	uint16_t prev_code = 0, curr_code = 0;
	uint16_t dict_cnt = 255;
	
	//Test variables
	uint16_t counter = 100;

	while(while_loop_start_flag || msg_indx <= msglen - 1){
		if(while_loop_start_flag)	while_loop_start_flag = 0;

		curr_code = msg[msg_indx++];
		if(curr_code <= dict_cnt){
			if(curr_code <= 255){
				(*ret_buf_len)++;
				ret_buf[*ret_buf_len - 1] = curr_code;
			}else{
				for(uint16_t i = 0; i < strs_sizes[curr_code - 255]; i++){
					(*ret_buf_len)++;
					ret_buf[*ret_buf_len - 1] = dict_strings[curr_code - 255][i];
				}
			}
		}else{
			dict_cnt = compress(ret_buf, *ret_buf_len);
			if(curr_code <= dict_cnt){
				for(uint16_t i = 0; i < strs_sizes[curr_code - 255]; i++){
					(*ret_buf_len)++;
					ret_buf[*ret_buf_len - 1] = dict_strings[curr_code - 255][i];
				}
			}else{
				dict_cnt = create_new_symbol_to_dict(&prev_code, dict_cnt);
				if(curr_code <= dict_cnt){
					for(uint16_t i = 0; i < strs_sizes[curr_code - 255]; i++){
						(*ret_buf_len)++;
						ret_buf[*ret_buf_len - 1] = dict_strings[curr_code - 255][i];
					}
				}else{
					printf("\n\nErr: Encountered an unknown code while decompressing!\n\n");
					print_dict(dict_cnt + 10);
					exit(0);
				}
			}
		}
		prev_code = curr_code;
		/*if(--counter == 0){
			msg_indx = msglen;
		}*/
	}
}
