
#include "i3ZLib.h"

#define ZLIB_INTERNAL						// 이 키워드를 통해 import대신 export키워드를 획득할수 있는듯 하다...(2012.04.09.수빈)

#include "zlib.h"
#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/ext/generic_string_size.h"

UINT32 i3ZLib::CalcCompressedSize( UINT32 SrcSize)
{
	return compressBound( SrcSize);
}

bool i3ZLib::Compress( UINT8 * pSrcBuf, UINT32 SrcLen, UINT8 * pDestBuf, UINT32 * pDestSize, LEVEL level )
{
	I3ASSERT( pSrcBuf != nullptr);
	I3ASSERT( pDestBuf != nullptr);
	I3ASSERT( pDestSize != nullptr);

	if( compress( pDestBuf, (uLong *) pDestSize, (const Bytef *) pSrcBuf, SrcLen) == Z_OK )
		return true;

	return false;
}


bool i3ZLib::Decompress( UINT8 * pSrcBuf, UINT32 SrcLen, UINT8 * pDestBuf, UINT32 * pDestSize)
{
	int rv;

	I3ASSERT( pSrcBuf != nullptr);
	I3ASSERT( pDestBuf != nullptr);
	I3ASSERT( pDestSize != nullptr);

	rv = uncompress( pDestBuf, pDestSize, (const Bytef *) pSrcBuf, SrcLen);
	if( rv == Z_OK)
		return true;

	I3TRACE1( "i3ZLib::Decompress() - Error : %d\n", rv);

	return false;
}

// 
INT32 i3ZLib::Decompress2( UINT8 * pSrcBuf, UINT32 SrcLen, UINT8 * pDestBuf, UINT32 * pDestSize )
{
	z_stream stream;
	INT32 err;
	
	stream.next_in = (Bytef*)pSrcBuf;
	stream.avail_in = (uInt)SrcLen;
	/* Check for source > 64K on 16-bit machine: */
	if ((uLong)stream.avail_in != SrcLen) return Z_BUF_ERROR;

	// 압축 해제 완료 시점 체크
	stream.next_out = pDestBuf;
	stream.avail_out = (uInt)*pDestSize;
	if ((uLong)stream.avail_out != *pDestSize) return Z_BUF_ERROR;

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	
	err = inflateInit2(&stream, -MAX_WBITS);
	if (err != Z_OK) return err;

	err = inflate(&stream, Z_FINISH);
	if(err != Z_STREAM_END) {
		inflateEnd(&stream);
			if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0))
			return Z_DATA_ERROR;
		return err;
	}
	*pDestSize = stream.total_out;

	err = inflateEnd(&stream);
	return err;
}

// 시그널 찾기
// fd : 파일 식별자
// fInfo : 파일 정보
// sig : 시그널
// len 길이
INT32 i3ZLib::_FindSig( i3FileStream *pfStream, const char * sig, INT32 len )
{
	UINT8 buf[32];
	unsigned long i1, i2, i;

	for( i2 = pfStream->GetSize() ; i2 > 0;)
	{
		// 32바이트씩 읽어서 체크
		i1 = i2 > sizeof(buf) - len ? i2 - (sizeof(buf) - len) : 0;
		
		pfStream->SetPosition( i1, FOSET );
		pfStream->Read( (void*) buf, i2 - i1 );

		for(i = i1; i < i2; i++)
			if (!memcmp(&buf[i - i1], sig, len))
				return i;

		memcpy(buf + sizeof(buf) - len, buf, len);
		i2 = i1;
	}

	return -1;
}

UINT8 * i3ZLib::_FileOpenFromZip( i3FileStream * pfStream, INT32 offset )
{
	UINT8 * pDest = nullptr;
	I3ZIPLOCALHEAD header;
	
	pfStream->SetPosition( offset, 0 );
	pfStream->Read( &header, 30 );

	offset += 30 + header.filename_length + header.extra_field_length;

	if (header.compression_method == 0x0000)
	{	
	}

	if (header.compression_method == 0x0008)
	{
		UINT8 * pSrc;
		UINT32 DestSize;

		DestSize = header.uncompressed_size;
		pDest = (UINT8 *) i3MemAlloc( DestSize );
		
		pSrc = (UINT8 *) i3MemAlloc( header.compressed_size );

		pfStream->SetPosition( offset, 0 );

		// 압축된 데이터를 읽는다
		if( pfStream->Read( pSrc, header.compressed_size) < 1)
		{
			I3MEM_SAFE_FREE_POINTER( pSrc );
			I3MEM_SAFE_FREE( pDest);
			return 0;
		}

		// 압축 해제
		Decompress2( pSrc, header.compressed_size, pDest, &DestSize );

		I3MEM_SAFE_FREE_POINTER( pSrc );
	}

	return pDest;
}

// 리스트를 만들어서 넘겨준다.
INT32 i3ZLib::CreateZipFileList( i3FileStream * pZStream, i3::vector<I3ZIP_FILEINFO*>* pOutFileList )
{	
	static char s_zlib_sig[] = { 'P', 'K', 0x05, 0x06 };

	INT32 offset, len, headerpos;
	I3ZIPFILEHEAD * pZipHeader;
	I3ZIPFILEHEAD * pHeader;
	I3ZIP_FILEINFO	* pFileInfo;

	// 시그널 체크
	headerpos = _FindSig( pZStream, s_zlib_sig, sizeof(s_zlib_sig) );

	if (headerpos == -1)
	{
		I3PRINTLOG(I3LOG_FATAL,  "i3ZLib::ZipFileOpen() - invalide sig." );
		return 0 ;
	}

	pZStream->SetPosition( headerpos + 12, 0 );

	pZStream->Read( &len, 4 );
	pZStream->Read( &offset, 4 );

	pZipHeader = (I3ZIPFILEHEAD *) i3MemAlloc( len );
	I3ASSERT( pZipHeader );

	pZStream->SetPosition( offset, 0 );
	pZStream->Read( pZipHeader, len );

	// 파일 리스트 만들기
	for( INT32 i = 0; i < len;)
	{	
		pFileInfo = (I3ZIP_FILEINFO *) i3MemAlloc( sizeof( I3ZIP_FILEINFO ) );
		I3ASSERT( pFileInfo );

		pFileInfo->m_pAddr = pHeader = &pZipHeader[i];

		pFileInfo->m_pszFileName = (char *) i3MemAlloc( pHeader->filename_length );
		I3ASSERT( pFileInfo->m_pszFileName );

		memcpy( pFileInfo->m_pszFileName,
			(char*)((UINT32)pHeader + 46) , pHeader->filename_length);

		i += 46 + pHeader->filename_length + pHeader->extra_field_length + pHeader->file_comment_length;
		
		pOutFileList->push_back( pFileInfo );
	}

	return offset + len;
}


bool i3ZLib::ZipFileOpen( i3FileStream * pOutStream, i3::vector<I3ZIP_FILEINFO*>* pOutFileList, const char * pszFileName )
{	
	I3ASSERT( pOutStream );
	I3ASSERT( pszFileName );

	// 파일이 있는지 확인
	if( !pOutStream->Open( pszFileName, STREAM_READ | STREAM_SHAREREAD ) )
	{
		I3PRINTLOG(I3LOG_FATAL,  "i3ZLib::ZipFileOpen() - file open failed." );
		return false;
	}

	I3TRACE(  "i3ZLib::ZipFileOpen() - %s Open Success.\n", pszFileName );

	if( pOutFileList == nullptr)
		return true;

	CreateZipFileList( pOutStream, pOutFileList );

	return true;
}

bool i3ZLib::ZipFileClose( i3FileStream * pStream, i3::vector<I3ZIP_FILEINFO*>* pFileList )
{
	INT32	i;

	I3ZIP_FILEINFO * pFileInfo;

	pStream->Close();

	if( pFileList )
	{
		if( pFileList->size() > 0 )
		{
			pFileInfo = (*pFileList)[0]; 
			I3MEM_SAFE_FREE_POINTER( pFileInfo->m_pAddr );

			for( i = 0; i < INT32(pFileList->size()) ; i++ )
			{
				pFileInfo =	(*pFileList)[i]; 			
				I3MEM_SAFE_FREE_POINTER( pFileInfo->m_pszFileName );
			}
		}
		
		pFileList->clear();
	}

	return true;
}

// 파일 쓰기

#define BUFLEN      16384
#define MAX_NAME_LEN 1024


UINT32 i3ZLib::FileAddToZip( i3FileStream * pOutStream, i3::vector<I3ZIP_FILEINFO*>* pFileList, const char * pszAddFileName )
{	
	UINT32	Result = 0;
	UINT8 * pBuf;
	UINT32 len;
	UINT8 * pDestBuf;
	UINT32 DestSize;
	INT32	i;
	bool	bCreateList = false;

	i3FileStream	File;

	I3ZIP_FILEINFO * pFileInfo;
	I3ZIPFILEHEAD * header;
	INT32	headSize = 0;

	I3ASSERT( pszAddFileName );

	if( !File.Open( pszAddFileName, STREAM_READ | STREAM_SHAREREAD ) )
	{
		//I3PRINTLOG(I3LOG_WARN,  "[i3ZLib::FileAddToZip] - file open fail. %s\n", pszAddFileName );
		return 0;
	}

	// 파일을 읽는다.
	DestSize = len = File.GetSize();
	pBuf = (UINT8 *) i3MemAlloc( len );
	I3ASSERT( pBuf );
	pDestBuf = (UINT8 *) i3MemAlloc( DestSize );
	I3ASSERT( pDestBuf );
	
	if( !File.Read( pBuf, (INT32) len ) )
	{
		File.Close();

		//I3PRINTLOG(I3LOG_FATAL,  "[i3ZLib::FileAddToZip] - Read Error. %s\n", pszAddFileName );
		goto EXIT_FAILED;
	}

	File.Close();

	// compress file data
	if( !Compress( pBuf, len, pDestBuf, &DestSize ) )
	{
		//I3PRINTLOG(I3LOG_FATAL,  "[i3ZLib::FileAddToZip] - Compress Error. %s\n", pszAddFileName );
		goto EXIT_FAILED;
	}

	// 임시 버퍼 해제
	I3MEM_SAFE_FREE_POINTER( pBuf );

	{
		HANDLE	hSrch;
		WIN32_FIND_DATA	wfd;
	
	#if defined( I3_WINCE)
		hSrch = ::FindFirstFile( (const unsigned short*)pszAddFileName, &wfd );
	#else
		hSrch = ::FindFirstFile( pszAddFileName, &wfd );
	#endif

		if( hSrch != INVALID_HANDLE_VALUE)
		{
			// Create Zip header
			header = (I3ZIPFILEHEAD *) i3MemAlloc( sizeof( I3ZIPFILEHEAD ) );

			header->compressed_size		= DestSize;
			header->uncompressed_size	= len;
			header->filename_length		= unsigned short(i3::generic_string_size( pszAddFileName ));
			header->last_mod_file_date	= (UINT16) wfd.ftLastWriteTime.dwHighDateTime;
			header->last_mod_file_time	= (UINT16) wfd.ftLastWriteTime.dwLowDateTime;
		}
		else
		{
			FindClose( hSrch);
			goto EXIT_FAILED;
		}

		FindClose( hSrch);
	}

	if( pOutStream->GetSize() )
	{
		// 기존 파일에 더한다.

		// Get FileList
		if( pFileList == nullptr)
		{
			pFileList = new i3::vector<I3ZIP_FILEINFO*>;
			I3ASSERT( pFileList );
			NetworkDump_Malloc( pFileList, sizeof( i3::vector<I3ZIP_FILEINFO*>), __FILE__, __LINE__);
			headSize = CreateZipFileList( pOutStream, pFileList );
			bCreateList = true;
		}

		// create new file head
		pFileInfo = (I3ZIP_FILEINFO *) i3MemAlloc( sizeof( I3ZIP_FILEINFO ) );
		I3ASSERT( pFileInfo );

		UINT32 strSize = strlen( pszAddFileName) + 1;
		pFileInfo->m_pszFileName = (char *) i3MemAlloc( strSize );
		strncpy( pFileInfo->m_pszFileName, pszAddFileName, strSize );

		pFileInfo->m_pAddr = header;

		pFileList->push_back( pFileInfo );

		// write head
		for( i = 0; i < INT32(pFileList->size()); i++ )
		{
			pFileInfo = (I3ZIP_FILEINFO *)	(*pFileList)[i]; 
			pOutStream->Write( pFileInfo->m_pAddr, sizeof( I3ZIPFILEHEAD ) );
		}

		// write zipped data
		//pOutStream->Read( pDestData, pFileList->GetCount() * sizeof( I3ZIPFILEHEAD ) );

		if( bCreateList )
		{
			I3MEM_SAFE_FREE_POINTER( pFileList );
		}
	}
	else
	{	
		pOutStream->Write( header, sizeof( I3ZIPFILEHEAD ) );
		pOutStream->SetPosition( sizeof( I3ZIPFILEHEAD ), STREAM_BEGIN );
		pOutStream->Write( pDestBuf, DestSize );
	}

	pOutStream->Close();

	I3MEM_SAFE_FREE_POINTER( header );
	I3MEM_SAFE_FREE_POINTER( pDestBuf );

	return DestSize;

EXIT_FAILED:
	I3MEM_SAFE_FREE_POINTER( pBuf );
	I3MEM_SAFE_FREE_POINTER( pDestBuf );

	return 0;
}

UINT32 i3ZLib::FileOpenFromZip( i3FileStream * pOutStream, i3FileStream * pZStream,
							   i3::vector<I3ZIP_FILEINFO*>* pZFileList, const char * pszFileName )
{
	INT32	i;
	UINT8 *	pData;

	i3::vector<I3ZIP_FILEINFO*>* pList;

	I3ZIP_FILEINFO * pFileInfo;
	I3ZIPFILEHEAD * header;
	
	I3ASSERT( pszFileName );
	I3ASSERT( pOutStream );

	bool bCreated = false;
	if( pZFileList == nullptr)
	{
		pList = new i3::vector<I3ZIP_FILEINFO*>;
		I3ASSERT( pList );
		NetworkDump_Malloc( pList, sizeof( i3::vector<I3ZIP_FILEINFO*>), __FILE__, __LINE__);
		CreateZipFileList( pZStream, pList );
		bCreated = true;
	}
	else
		pList = pZFileList;
	
	for(i = 0; i < INT32(pList->size()); i++)
	{
		pFileInfo = (I3ZIP_FILEINFO *) (*pList)[i];    

		if( i3::generic_is_iequal( pszFileName, pFileInfo->m_pszFileName ) )
		{	
			header = pFileInfo->m_pAddr;

			pData = _FileOpenFromZip( pZStream, header->offset_lcl_hdr_frm_frst_disk );
			I3ASSERT( pData );

			// 데이터 복사
			if( pOutStream->Create( pFileInfo->m_pszFileName, STREAM_WRITE ))
				pOutStream->Write( pData, header->uncompressed_size );

			if( bCreated)
			{
				I3_SAFE_DELETE( pList);
			}
			return 1;
		}
	}

	if( bCreated)
	{
		I3_SAFE_DELETE( pList);
	}
	
	return 0;
}
