#if !defined( __I3_FILE_STREAM_H)
#define __I3_FILE_STREAM_H

#include "i3Stream.h"


#if defined( I3_WIPI)
	#define		DEFCACHE_SIZE		8192
#else
	#define		DEFCACHE_SIZE		32768
#endif

#if defined( I3_WINDOWS ) || defined( I3_XBOX ) || defined( I3_WINCE )
#define I3_USE_FILE_HANDLE
#endif


class I3_EXPORT_BASE i3FileStream : public i3Stream
{
	I3_CLASS_DEFINE( i3FileStream);

protected:
	__HANDLE_TYPE		m_pFile;

	UINT32 m_Pos;
	UINT32 m_InternalOffset;


	i3PackFile *		m_pPackFile;

	char FileName[MAX_PATH];

	static BOOL	m_bCaseSensitive;

public:
	i3FileStream(void);
	virtual ~i3FileStream(void);

	void * GetHandle(void)				{ return (void *)m_pHandle; }

	char *  GetFileName(void)			{ return FileName; }

	// 대소문자 구분하기
	static void	SetCaseSensitive( BOOL bTrue = TRUE )	{ m_bCaseSensitive = bTrue; }
	static BOOL	IsCaseSensitive()						{ return m_bCaseSensitive; }

	virtual BOOL  Create( const char *Name, INT32 Acc, UINT32 Size = 0);
	virtual BOOL  Open( const char *Name, INT32 Acc, UINT32 Size = 0);
	virtual BOOL  Close(void);

	virtual UINT32 Read( void * pBuf, INT32 Length);
	virtual UINT32 Write( const void * pBuf, INT32 Length);
	virtual UINT32 SetPosition( INT32 Pos, INT32 MoveType = STREAM_BEGIN);
	virtual UINT32 GetPosition(void);

	// 파일 사이즈 계산하기
	UINT32	CalculateSize(void);

	static bool		isFile( const char * pszPath);

	#ifdef I3_WINDOWS
		static i3FileStream * FromHandle( HANDLE Handle);
	#endif
	#ifdef I3_PSP
		static void SetPrefix( const char *szPrefix );
		static const char* GetPrefix();
		static void SetHDD();
		static void SetUMD();
	#endif
	
	#ifdef I3_DEBUG
		static	void SetLogEnable( const char * pszFile, BOOL bEnable);
		static  void WriteOpenLog( const char * szFileName );
	#endif
};


#endif
