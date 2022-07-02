#include "tlzw.h"

void init_lzw_struct(lzw_struct *lzw, unsigned char *char_buf, uint16_t *char_buf_len, uint16_t *uint_buf, uint16_t *uint_buf_len){
	lzw->dictionary_size = 0;
	lzw->last_spec_code = 255;
	lzw->char_buffer = char_buf;
	lzw->char_buffer_len = char_buf_len;
	lzw->uint_buffer = uint_buf;
	lzw->uint_buffer_len = uint_buf_len;
}

uint16_t dict_contains(lzw_struct *lzw, unsigned char *temp_str, uint16_t temp_str_len){
	for(int i = 0; i < lzw->dictionary_size; i++){
		if(lzw->string_sizes[i] == temp_str_len){
			int n = 0;
			for(n; n < lzw->string_sizes[i]; n++){
				if(lzw->strings[i][n] != temp_str[n])
					break;
			}

			if(n == temp_str_len)
				return 1;
		}
	}
	return 0;
}

int16_t string_to_dict_code(lzw_struct *lzw, unsigned char *temp_str, uint16_t temp_str_len){
	for(int i = 0; i < lzw->dictionary_size; i++){
		if(lzw->string_sizes[i] == temp_str_len){
			int n = 0;
			for(n; n < lzw->string_sizes[i]; n++){
				if(lzw->strings[i][n] != temp_str[n])
					break;
			}

			if(n == temp_str_len)
				return (int16_t)lzw->dictionary_codes[i];
		}
	}
	return (int16_t)(-1);
}

void encode_temp_str_to_output(lzw_struct *lzw, unsigned char *temp_str, uint16_t temp_str_len){
	if(temp_str_len == 1){
		lzw->uint_buffer[(*lzw->uint_buffer_len)++] = temp_str[0];
	}else if(temp_str_len > 1){
		uint16_t temp_str_code = string_to_dict_code(lzw, temp_str, temp_str_len);

		if(temp_str_code == -1){
			system("clear");
			printf("ERR: string is not contained in dictionary!\n");
			exit(0);
		}

		lzw->uint_buffer[(*lzw->uint_buffer_len)++] = temp_str_code;
	}
}

void add_lzw_code(lzw_struct *lzw, unsigned char *temp_str, uint16_t temp_str_len){
	if(lzw->dictionary_size == 0){
		++lzw->dictionary_size;

		lzw->strings = (unsigned char**)calloc(lzw->dictionary_size, sizeof(unsigned char*));
		lzw->string_sizes = (uint16_t*)calloc(lzw->dictionary_size, sizeof(uint16_t));
		lzw->dictionary_codes = (uint16_t*)calloc(lzw->dictionary_size, sizeof(uint16_t));

		lzw->strings[0] = (unsigned char*)calloc(temp_str_len, sizeof(unsigned char));
		lzw->string_sizes[0] = temp_str_len;
		for(int i = 0; i < temp_str_len; i++)
			lzw->strings[0][i] = temp_str[i];

		if(temp_str_len == 1)
			lzw->dictionary_codes[0] = temp_str[0];
		else if(temp_str_len > 1)
			lzw->dictionary_codes[0] = ++lzw->last_spec_code;
	}else{
		++lzw->dictionary_size;

		lzw->strings = (unsigned char**)realloc(lzw->strings, lzw->dictionary_size * sizeof(unsigned char*));
		lzw->string_sizes = (uint16_t*)realloc(lzw->string_sizes, lzw->dictionary_size * sizeof(uint16_t));
		lzw->dictionary_codes = (uint16_t*)realloc(lzw->dictionary_codes, lzw->dictionary_size * sizeof(uint16_t));

		lzw->strings[lzw->dictionary_size - 1] = (unsigned char*)calloc(temp_str_len, sizeof(unsigned char));
		lzw->string_sizes[lzw->dictionary_size - 1] = temp_str_len;
		for(int i = 0; i < temp_str_len; i++)
			lzw->strings[lzw->dictionary_size - 1][i] = temp_str[i];

		if(temp_str_len == 1)
			lzw->dictionary_codes[lzw->dictionary_size - 1] = temp_str[lzw->dictionary_size - 1];
		else if(temp_str_len > 1)
			lzw->dictionary_codes[lzw->dictionary_size - 1] = ++lzw->last_spec_code;
	}
}

void lzw_compress(unsigned char *msg, uint16_t *msglen, uint16_t *out_buff, uint16_t *out_buf_len){
	//Some variables
	uint16_t curr_msg_indx = 0, start_loop_flag = 1;
	unsigned char temp_str[200] = {}, temp_str_len = 0;

	//Allocate memory for lzw structure
	lzw_struct *lzw = (lzw_struct*)calloc(1, sizeof(lzw_struct));

	//Initialize lzw structure
	init_lzw_struct(lzw, msg, msglen, out_buff, out_buf_len);

	//Start compressing
	while(start_loop_flag || curr_msg_indx < *msglen){
		if(start_loop_flag) start_loop_flag = 0;

		unsigned char new_char = lzw->char_buffer[curr_msg_indx++];
		temp_str[temp_str_len++] = new_char;

		if(dict_contains(lzw, temp_str, temp_str_len)){
			//nop
		}else{
			temp_str_len--;
			encode_temp_str_to_output(lzw, temp_str, temp_str_len);
			temp_str_len++;
			add_lzw_code(lzw, temp_str, temp_str_len);

			memset(temp_str, 0, temp_str_len);
			temp_str_len = 1;
			temp_str[0] = new_char;
		}
	}
	encode_temp_str_to_output(lzw, temp_str, temp_str_len);

	//Deallocate
	free(lzw->dictionary_codes);
	free(lzw->string_sizes);
	for(int i = 0; i < lzw->dictionary_size; i++)
		free(lzw->strings[i]);
}

void create_new_symbol_to_dict(lzw_struct *lzw, uint16_t *prev_code){
	lzw->dictionary_size++;

	lzw->dictionary_codes[lzw->last_spec_code - 254] = lzw->last_spec_code + 1;

	lzw->strings = (unsigned char**)realloc(lzw->strings, lzw->dictionary_size * sizeof(unsigned char*));
	lzw->string_sizes = (uint16_t*)realloc(lzw->string_sizes, lzw->dictionary_size * sizeof(uint16_t));
	lzw->dictionary_codes = (uint16_t*)realloc(lzw->dictionary_codes, lzw->dictionary_size * sizeof(uint16_t));

	if(*prev_code <= 255)
		lzw->string_sizes[lzw->last_spec_code - 254] = 2;
	else
		lzw->string_sizes[lzw->last_spec_code - 254] = lzw->string_sizes[*prev_code - 255] + 1;

	lzw->strings[lzw->dictionary_size - 1] = (unsigned char*)calloc(lzw->string_sizes[lzw->last_spec_code - 254], sizeof(unsigned char));
	for(int i = 0; i < lzw->string_sizes[lzw->last_spec_code - 254] - 1; i++){
		if(*prev_code <= 255)
			lzw->strings[lzw->last_spec_code - 254][i] = *prev_code;
		else
			lzw->strings[lzw->last_spec_code - 254][i] = lzw->strings[*prev_code - 255][i];
	}

	if(*prev_code <= 255)
		lzw->strings[lzw->last_spec_code - 254][lzw->string_sizes[lzw->last_spec_code - 254] - 1] = *prev_code;
	else
		lzw->strings[lzw->last_spec_code - 254][lzw->string_sizes[lzw->last_spec_code - 254] - 1] = lzw->strings[*prev_code - 255][0];

	lzw->last_spec_code++;
}

void compress(lzw_struct *lzw){
	//Some variables
	uint16_t curr_msg_indx = 0, start_loop_flag = 1;
	unsigned char temp_str[500] = {}, temp_str_len = 0;

	if(lzw->dictionary_size){
		free(lzw->dictionary_codes);
		lzw->dictionary_codes = NULL;
		free(lzw->string_sizes);
		lzw->string_sizes = NULL;
		for(int i = 0; i < lzw->dictionary_size; i++)
			free(lzw->strings[i]);
		lzw->strings = NULL;
		lzw->last_spec_code = 255;
		lzw->dictionary_size = 0;
	}

	//Start compressing
	while(start_loop_flag || curr_msg_indx < *lzw->char_buffer_len){
		if(start_loop_flag) start_loop_flag = 0;

		unsigned char new_char = lzw->char_buffer[curr_msg_indx++];
		temp_str[temp_str_len++] = new_char;

		if(dict_contains(lzw, temp_str, temp_str_len)){
			//nop
		}else{
			add_lzw_code(lzw, temp_str, temp_str_len);

			memset(temp_str, 0, temp_str_len);
			temp_str_len = 1;
			temp_str[0] = new_char;
		}
	}
}

void lzw_decompress(uint16_t *msg, uint16_t msglen, unsigned char *out_buf, uint16_t *out_buf_len){
	uint16_t start_loop_flag = 1, msg_indx = 0;
	uint16_t prev_code = 0, curr_code = 0;

	//Allocate memory for lzw structure
	lzw_struct *lzw = (lzw_struct*)calloc(1, sizeof(lzw_struct));

	//Initialize lzw structure
	init_lzw_struct(lzw, out_buf, out_buf_len, msg, &msglen);

	//Start decompressing
	while(start_loop_flag || msg_indx <= msglen - 1){
		if(start_loop_flag)	start_loop_flag = 0;

		curr_code = lzw->uint_buffer[msg_indx++];
		if(curr_code <= lzw->last_spec_code){
			if(curr_code <= 255){
				(*lzw->char_buffer_len)++;
				lzw->char_buffer[*lzw->char_buffer_len - 1] = curr_code;
			}else{
				for(uint16_t i = 0; i < lzw->string_sizes[curr_code - 255]; i++){
					(*lzw->char_buffer_len)++;
					lzw->char_buffer[*lzw->char_buffer_len - 1] = lzw->strings[curr_code - 255][i];
				}
			}
		}else{
			compress(lzw);
			if(curr_code <= lzw->last_spec_code){
				for(uint16_t i = 0; i < lzw->string_sizes[curr_code - 255]; i++){
					(*lzw->char_buffer_len)++;
					lzw->char_buffer[*lzw->char_buffer_len - 1] = lzw->strings[curr_code - 255][i];
				}
			}else{
				create_new_symbol_to_dict(lzw, &prev_code);
				if(curr_code <= lzw->last_spec_code){
					for(uint16_t i = 0; i < lzw->string_sizes[curr_code - 255]; i++){
						(*lzw->char_buffer_len)++;
						lzw->char_buffer[*lzw->char_buffer_len - 1] = lzw->strings[curr_code - 255][i];
					}
				}else{
					printf("\n\nErr: Encountered an unknown code while decompressing!\n\n");
					exit(0);
				}
			}
		}
		prev_code = curr_code;
	}
}

void save_compressed_buffer(uint16_t *buf, uint16_t buf_len, FILE *file){
	unsigned char char_buf[buf_len * 2];

	for(int i = 0; i < buf_len; i++){
		char_buf[i*2] = buf[i] >> 8;
		char_buf[(i*2) + 1] = buf[i];
	}

	fwrite(char_buf, sizeof(unsigned char), buf_len * 2, file);
}

void char_buf_to_uint16_buf(unsigned char *char_buf, uint16_t char_buf_len, uint16_t *uint_buf){
	for(int i = 0; i < char_buf_len; i += 2){
		uint_buf[i / 2] |= char_buf[i];
		uint_buf[i / 2] <<= 8;
		uint_buf[i / 2] |= char_buf[i + 1];
	}
}
