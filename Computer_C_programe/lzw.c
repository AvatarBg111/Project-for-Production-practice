#include "lzw.h"

//Static dictionary
char dict_strings[DICT_SIZE][DICT_STR_MAXLINE];

//Receives string and returns if it's present in the dictionary
int dict_contains(uint16_t *dict_strs_sizes, int *dict_size, char new_char, char *encd_str, int encd_str_len){
	int n = 0;

	encd_str[encd_str_len++] = new_char;

	for(int i = 0; i < *dict_size; i++){
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
uint16_t str_to_code(uint16_t *dict_strs_sizes, uint16_t *dict_codes, int *dict_size, char *encd_str, int encd_str_len){
	int n = 0;

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

	return -1;
}

void add_to_dict(uint16_t *dict_strs_sizes, uint16_t *dict_codes, int *dict_size, uint16_t *last_spec_code, char *encd_str, int encd_str_len){
	(*dict_size)++;

	memset(dict_strings[(*dict_size) - 1], 0, DICT_STR_MAXLINE);
	for(int i = 0; i < encd_str_len; i++)
		dict_strings[(*dict_size) - 1][i] = encd_str[i];

	dict_strs_sizes[(*dict_size) - 1] = encd_str_len;

	if(encd_str_len == 1 || encd_str_len == 0)
		dict_codes[(*dict_size) - 1] = encd_str[0];
	else
		dict_codes[(*dict_size) - 1] = ++(*last_spec_code);
}

void lzw_compress(unsigned char *msg, int msglen, uint16_t *out_buf, int *out_buf_len){
	//Variables needed for programe
	uint16_t dict_strs_sizes[DICT_SIZE], dict_codes[DICT_SIZE], last_spec_code = 255;
	char encd_str[DICT_STR_MAXLINE] = {}, new_char;
	int while_loop_start_flag = 1, encd_str_len = 0, dict_size = 0, msg_indx = 0;

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
		(*out_buf_len)++;
	}
}
