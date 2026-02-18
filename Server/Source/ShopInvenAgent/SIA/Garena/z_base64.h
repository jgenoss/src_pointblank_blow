#ifndef _D_Z_BASE64_H__
#define _D_Z_BASE64_H__

INT32 base64_encode(char const* bytes_to_encode, UINT32 in_len, char * ret);
INT32 base64_decode(char const* encoded_string, char * ret);

#endif
