#ifndef _ENCRYPTION_H
#define _ENCRYPTION_H

void encrypt(unsigned char*, int);
void decrypt(unsigned char*, int);
void print_message(unsigned char*, int);

void CBC_xor(unsigned char*, unsigned char*, int);
void CBC_block_encr(unsigned char*, int);
void CBC_block_decr(unsigned char*, int);
void CBC_encrypt(unsigned char*, int);
void CBC_decrypt(unsigned char*, int);

#endif	//_ENCRYPTION_H
