#include "types.h"

void charToByte(char* chars, byte* bytes, unsigned int count){
    for(unsigned int i = 0; i < count; i++)
        bytes[i] = (byte)chars[i];
}

void charToByte(const char* chars, byte* bytes, unsigned int count){
    for(unsigned int i = 0; i < count; i++)
        bytes[i] = (byte)chars[i];
}

void byteToChar(byte* bytes, char* chars, unsigned int count){
    for(unsigned int i = 0; i < count; i++)
         chars[i] = (char)bytes[i];
    chars[count] = NULL;
}

word16 twosComplement(word16 number)
{
    // printf("twosComplement(): original = %x\n", number);
    word16 invert_number = ~number;
    word16 complement = invert_number + 1;
    word16 pattern = 0xFFFF;
    // printf("twosComplement(): pattern = %x\n", pattern);
    complement &= pattern; // considering only 16 LSBs
    // printf("twosComplement(): complement = %x\n", complement);
    return complement;
}

byte lsbyte(word16 number)
{
    return number & 0xFF;
}

byte msbyte(word16 number)
{
    return ((number & 0xFF00) >> 8);
}




unsigned int mergeBytes(byte msb, byte b, byte lsb)
{
    unsigned int shifted_msb = msb << 16;
    word16 shifted_b = b << 8;
    return shifted_msb + shifted_b + lsb;
}

unsigned int mergeBytes(byte msb, byte mb, byte lb, byte lsb)
{
    unsigned int shifted_msb = msb << 24;
    unsigned int shifted_mb = mb << 16;
    word16 shifted_lb = lb << 8;
    return shifted_msb + shifted_mb + shifted_lb + lsb;
}
