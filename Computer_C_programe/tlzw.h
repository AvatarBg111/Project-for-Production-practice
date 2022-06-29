#ifndef _TLZW
#define _TLZW

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>

typedef struct lzw_struct{
	int dictionary_size;
	uint16_t last_spec_code;
	unsigned char **strings;
	uint16_t *string_sizes;
	uint16_t *dictionary_codes;
	unsigned char *char_buffer;
	uint16_t *char_buffer_len;
	uint16_t *uint_buffer;
	uint16_t *uint_buffer_len;
}lzw_struct;

void init_lzw_struct(lzw_struct*, unsigned char*, uint16_t*, uint16_t*, uint16_t*);
uint16_t dict_contains(lzw_struct*, unsigned char*, uint16_t);
int16_t string_to_dict_code(lzw_struct*, unsigned char*, uint16_t);
void encode_temp_str_to_output(lzw_struct*, unsigned char*, uint16_t);
void add_lzw_code(lzw_struct*, unsigned char*, uint16_t);
void create_new_symbol_to_dict(lzw_struct*, uint16_t*);
void compress(lzw_struct*);
void lzw_compress(unsigned char*, uint16_t*, uint16_t*, uint16_t*);
void lzw_decompress(uint16_t*, uint16_t, unsigned char*, uint16_t*);

#endif
