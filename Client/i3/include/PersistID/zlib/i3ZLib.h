#if !defined( __I3_ZLIB_H)
#define __I3_ZLIB_H

#include "i3Base.h"

#if (defined( I3_WINDOWS) || defined( I3_WINCE)) && defined( I3_DLL)
	#if defined( I3_ZLIB_BUILD)
		#define	I3_EXPORT_ZLIB		__declspec( dllexport)
	#else
		#define	I3_EXPORT_ZLIB		__declspec( dllimport)
	#endif
#else
	#if !defined( I3_EXPORT_ZLIB)
		#define	I3_EXPORT_ZLIB
	#endif
#endif


enum fileorigin {
	FOSET, FOCUR, FOEND
} ;

#pragma pack(1)
typedef struct _tagI3ZIPFILEHEAD {
	unsigned long	cent_file_header_sig;
	unsigned char	version_made_by;
	unsigned char	host_os;
	unsigned char	version_needed_to_extract;
	unsigned char	os_needed_to_extract;

	unsigned short	general_purpose_bit_flag;
	unsigned short	compression_method;
	unsigned short	last_mod_file_time;
	unsigned short	last_mod_file_date;
	unsigned long	crc32;
	unsigned long	compressed_size;
	unsigned long	uncompressed_size;
	unsigned short	filename_length;
	unsigned short	extra_field_length;

	unsigned short	file_comment_length;
	unsigned short	disk_number_start;
	unsigned short	internal_file_attrib;
	unsigned long	external_file_attrib;

	//UINT32 compressed_len; /* total bit length of compressed file mod 2^32 */
	//UINT32 bits_sent;      /* bit length of compressed data sent mod 2^32 */

	unsigned long	offset_lcl_hdr_frm_frst_disk;
} I3ZIPFILEHEAD;
#pragma pack()


#pragma pack(2)
typedef struct _tagI3ZIPLOCALHEAD {
	unsigned long	signature;
	unsigned short	version;

	unsigned short	general_purpose_bit_flag;
	unsigned short	compression_method;
	unsigned short	last_mod_file_time;
	unsigned short	last_mod_file_date;
	unsigned long	crc32;
	unsigned long	compressed_size;
	unsigned long	uncompressed_size;
	unsigned short	filename_length;
	unsigned short	extra_field_length;
} I3ZIPLOCALHEAD;
#pragma pack()

#pragma pack(4)
typedef struct tag_I3ZIP_FILEINFO
{
	char		*m_pszFileName;
	I3ZIPFILEHEAD * m_pAddr;
} I3ZIP_FILEINFO;
#pragma pack()

namespace i3ZLib
{
	enum LEVEL
	{
		NO_COMRESS = 0,
		BEST_SPEED = 1,
		BEST_SIZE = 9,
		DEFAULT = -1
	};

	I3_EXPORT_ZLIB	UINT32	CalcCompressedSize( UINT32 SrcSize);

	I3_EXPORT_ZLIB	BOOL	Compress( UINT8 * pSrcBuf, UINT32 SrcLen, UINT8 * pDestBuf, UINT32 * pDestSize, LEVEL level = DEFAULT);
	I3_EXPORT_ZLIB	BOOL	Decompress( UINT8 * pSrcBuf, UINT32 SrcLen, UINT8 * pDestBuf, UINT32 * pDestSize);
	I3_EXPORT_ZLIB	INT32	Decompress2( UINT8 * pSrcBuf, UINT32 SrcLen, UINT8 * pDestBuf, UINT32 * pDestSize );

	//////////////////////////////////////////////////////////////////////////////////////
	// zip file lib : by dfly79
	// PackedZipFile Open
	I3_EXPORT_ZLIB BOOL	ZipFileOpen( i3FileStream * pOutStream, i3List * pOutFileList, const char * pszFileName );
	// 클로즈
	I3_EXPORT_ZLIB BOOL ZipFileClose( i3FileStream * pStream, i3List * pFileList );
	
	// AddFiletoZip
	I3_EXPORT_ZLIB UINT32 FileAddToZip( i3FileStream * pOutStream, i3List * pFileList, const char * pszAddFileName );

	// 파일 리스트 작성
	// return			: HeaderSize;
	// pZStream			: Opened ZipFile
	// pOutFileList		: I3ZIP_FILEINFO's List
	I3_EXPORT_ZLIB INT32 CreateZipFileList( i3FileStream * pZStream, i3List * pOutFileList );

	// Get&FindFromPacked
	// pOutStream		: Zip에서 찾은 파일 아웃
	// pZStream			: Zip 스트림(in) -- 번저 오픈을 한 Zip파일
	// pZFileList		: Zip 파일을 오픈할 시 내부 파일 리스트(in)
	// pszFileName		: 찾을 파일 이름
	I3_EXPORT_ZLIB UINT32 FileOpenFromZip( i3FileStream * pOutStream, i3FileStream * pZStream, i3List * pZFileList, const char * pszFileName );
	
	I3_EXPORT_ZLIB INT32 _FindSig( i3FileStream *fInfo, const char * sig, INT32 len );
	I3_EXPORT_ZLIB UINT8 * _FileOpenFromZip( i3FileStream * pfStream, INT32 offset );
};

#endif
