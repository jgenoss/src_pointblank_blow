#include "pch.h"
#include "z_base64.h"

static const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static inline bool is_base64(unsigned char c) 
{
  return (isalnum(c) || (c == '+') || (c == '/'));
}

INT32 base64_encode(char const* bytes_to_encode, UINT32 in_len, char * ret) 
{
  INT32 i = 0;
  INT32 j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  UINT32 ui32Idx = 0;
  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret[ui32Idx++] += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret[ui32Idx++] += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret[ui32Idx++] += '=';

  }

  return ui32Idx;
}

INT32 base64_decode(char const* encoded_string, char * ret) 
{
  UINT32 in_len = strlen(encoded_string);
  UINT32 i = 0;
  UINT32 j = 0;
  UINT32 in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];

  UINT32 ui32Idx = 0;
  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
	  {
		if( 'A' <= char_array_4[i] && 'Z' >= char_array_4[i] )
			char_array_4[i] = char_array_4[i] - 'A';
		else if( 'a' <= char_array_4[i] && 'z' >= char_array_4[i] )
			char_array_4[i] = 26 + char_array_4[i] - 'a';
		else if( '0' <= char_array_4[i] && '9' >= char_array_4[i] )
			char_array_4[i] = 52 + char_array_4[i] - '0';
		else if( '+' == char_array_4[i] )
			char_array_4[i] = 62;
		else if( '/' == char_array_4[i] )
			char_array_4[i] = 63;
	  }

      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret[ui32Idx++] = char_array_3[i];
      i = 0;
    }
  }

  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;

    for (j = 0; j <4; j++)
	  {
		if( 'A' <= char_array_4[j] && 'Z' >= char_array_4[j] )
			char_array_4[j] = char_array_4[j] - 'A';
		else if( 'a' <= char_array_4[j] && 'z' >= char_array_4[j] )
			char_array_4[j] = 26 + char_array_4[j] - 'a';
		else if( '0' <= char_array_4[j] && '9' >= char_array_4[j] )
			char_array_4[j] = 52 + char_array_4[j] - '0';
		else if( '+' == char_array_4[j] )
			char_array_4[j] = 62;
		else if( '/' == char_array_4[j] )
			char_array_4[j] = 63;
	  }

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; (j < i - 1); j++)
		ret[ui32Idx++] = char_array_3[j];
  }

  return ui32Idx;
}
