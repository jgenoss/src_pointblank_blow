#if !defined( __I3_COMPRESS_HUFFMAN_H)
#define __I3_COMPRESS_HUFFMAN_H

#include "i3ElementBase.h"

namespace i3CompressHuffman
{
	UINT32	GetResultSize(void);
	UINT32	GetSourceSize(void);

	UINT8 *	Encode( UINT8 * pData, UINT32 Size, UINT32 AccessUnitSize);
	UINT8 *	Decode( UINT8 * pData, UINT32 AccessUnitSize);

	BOOL	EncodeFile( const char * pszSrc, const char * pszDest);
	BOOL	DecodeFile( const char * pszSrc, const char * pszDest);
};

#endif
