#ifndef _LZW_H
#define _LZW_H

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>

#define DICT_SIZE 1500
#define DICT_STR_MAXLINE 100

int dict_contains(uint16_t*, int*, char, char*, int);
uint16_t str_to_code(uint16_t*, uint16_t*, int*, char*, int);
void add_to_dict(uint16_t*, uint16_t*, int*, uint16_t*, char*, int);
void lzw_compress(unsigned char *msg, int msglen, uint16_t *out_buf, int *out_buf_len);

#endif	//_LZW_H
