#ifndef _LZW_H
#define _LZW_H

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>

#define DICT_SIZE 1500
#define DICT_MAXLINE_CAP 100
#define DICT_STR_MAXLINE 100

uint16_t dict_contains(uint16_t*, uint16_t*, unsigned char, unsigned char*, uint16_t);
uint16_t str_to_code(uint16_t*, uint16_t*, uint16_t*, unsigned char*, uint16_t);
void add_to_dict(uint16_t*, uint16_t*, uint16_t*, uint16_t*, unsigned char*, uint16_t);
void lzw_compress(unsigned char *msg, uint16_t msglen, uint16_t *out_buf, uint16_t *out_buf_len);
void lzw_decompress(uint16_t *msg, uint16_t msglen, unsigned char *out_buf, uint16_t *out_buf_len);

#endif	//_LZW_H
