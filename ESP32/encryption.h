#ifndef _ENCRYPTION_H
#define _ENCRYPTION_H

#include <stdio.h>
#include <string.h>

void _encrypt(unsigned char*, int);
void _decrypt(unsigned char*, int);
void print_message(unsigned char*, int);

void CBC_xor(unsigned char*, unsigned char*, int);
void CBC_block_encr(unsigned char*, int);
void CBC_block_decr(unsigned char*, int);
void CBC_encrypt(unsigned char*, int);
void CBC_decrypt(unsigned char*, int);

#endif //_ENCRYPTION_H