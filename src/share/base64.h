#ifndef BASE64_HEADER
#define BASE64_HEADER

int base64_decode(unsigned char in[], unsigned char out[], int len);
int base64_encode(unsigned char in[], unsigned char out[], int len, int newline_flag);

#endif