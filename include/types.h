#ifndef TYPES_H
#define TYPES_H
#include <stdio.h>
 
typedef unsigned char      byte;    // Byte is a char
typedef unsigned short int word16;  // 16-bit word is a short int
typedef signed short int   signed_word16;  // 16-bit word is a short int
typedef unsigned int       word32;  // 32-bit word is an int

void charToByte(char* chars, byte* bytes, unsigned int count);

void charToByte(const char* chars, byte* bytes, unsigned int count);

void byteToChar(byte* bytes, char* chars, unsigned int count);

word16 twosComplement(word16 number);

byte lsbyte(word16 number);

byte msbyte(word16 number);

inline word16 mergeBytes(byte msbyte, byte lsbyte)
{
    word16 number = msbyte;
//     printf("mergeBytes(): msbyte = %d; lsbyte = %d\n", msbyte, lsbyte);
    number <<= 8;
//     printf("mergeBytes(): number = %d\n", number);
    number += lsbyte;
//     printf("mergeBytes(): number = %d\n", number);
    return number;
}
inline word32 mergeWords(word16 msword, word16 lsword){
    word32 number = msword;
//     printf("mergeWords(): msword = %d; lsword = %d\n", msword, lsword);
    number <<= 16;
//     printf("mergeWords(): number = %d\n", number);
    number += lsword;
//     printf("mergeWords(): number = %d\n", number);
    return number;
}


unsigned int mergeBytes(byte msb, byte b, byte lsb);

unsigned int mergeBytes(byte msb, byte mb, byte lb, byte lsb);
#endif