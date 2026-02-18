// i3Stream.h
#ifndef __I3STREAM_H
#define __I3STREAM_H

#include "i3ElementBase.h"
#include "i3MetaTemplate.h"

#define STREAM_ERR				0xFFFFFFFF

#define STREAM_BEGIN			1
#define STREAM_CURRENT			2
#define STREAM_END				3

#define STREAM_READ			0x00000001
#define STREAM_WRITE		0x00000002
#define STREAM_SHAREREAD	0x00000004
#define STREAM_SHAREWRITE	0x00000008

class i3PackFile;

class I3_EXPORT_BASE i3Stream : public i3ElementBase
{
	I3_ABSTRACT_CLASS_DEFINE( i3Stream);

public:

	enum STREAMCODE {
		STREAM_NOERROR,									// No Error
		STREAM_NOTINITIALIZED,							// Not initialized
		STREAM_NOTENOUGHSPACE,							// Not enough space
		STREAM_IOERROR,									// Some I/O error
		STREAM_TIMEOUT,									// Timeout
		STREAM_EOS,										// End Of Stream
		STREAM_ACCESSVIOLATION							// Access Violation
	};

protected:
	UINT32 m_Size;
	UINT32	m_AccessUnit;
	UINT32 m_RecordSize;
	STREAMCODE m_LastCode;
	INT32 m_Access;

	//INT32		m_HandleIdx; 
	void	*	m_pHandle;		
public:

	i3Stream(void);
	virtual ~i3Stream(void);

	UINT32			GetSize(void)						{ return m_Size; }
	STREAMCODE		GetLastError(void)					{ return m_LastCode; }
	INT32			GetAccessMode(void)					{ return m_Access; }

	virtual void *			GetHandle(void)				{ return m_pHandle; }

	void	BeginRecord(void)							{ m_RecordSize = 0; }
	UINT32	GetRecordSize(void)							{ return m_RecordSize; }

	virtual BOOL  Create( const char * pszName, INT32 Acc, UINT32 Size = 0)		= 0;
	virtual BOOL  Open( const char * pszName, INT32 Acc, UINT32 Size = 0)		= 0;
	virtual BOOL  Close(void)													= 0;

	virtual UINT32 Read( void *Buf, INT32 Length)									= 0;
	virtual UINT32 CopyTo( i3Stream *Stream, UINT32 Length);
	virtual UINT32 Write( void *Buf, INT32 Length)									= 0;
	virtual UINT32 CopyFrom( i3Stream *Stream, UINT32 Length);
	virtual UINT32 SetPosition( INT32 Pos, INT32 MoveType)							= 0;
	virtual UINT32 GetPosition(void)												= 0;

	virtual UINT32 ReadLine( char * pStr, INT32 BufLength = -1 );
	virtual UINT32 WriteLine( char * pStr);
	virtual UINT32 Printf( char *szBuf, const char *szFmt, ... );
};

class I3_EXPORT_BASE i3MemStream : public i3Stream
{
	I3_CLASS_DEFINE( i3MemStream);

protected:
	UINT32 m_Pos;
	bool	m_bExternMem;
public:
	i3MemStream(void);
	virtual ~i3MemStream(void);

	virtual BOOL  Create( const char * pNewPointer, INT32 Acc, UINT32 Size);
	virtual BOOL  Open( const char * pNewPointer, INT32 Acc, UINT32 Size);
	virtual BOOL  Close(void);

	virtual UINT32 Read( void *Buf, INT32 Length);
	virtual UINT32 Write( void *Buf, INT32 Length);
	virtual UINT32 SetPosition( INT32 Pos, INT32 MoveType);
	virtual UINT32 GetPosition(void);

	static i3MemStream * FromHandle( void *pNew);
	static i3MemStream * CreateFromFile( const char * pszPath);
};

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
	UINT32 m_InternalPos;
	UINT32 m_InternalOffset;

	INT32	m_CacheSize;
	INT32	m_CacheLoadPos;
	INT32	m_CacheHeadPos;
	INT32	m_CacheEndPos;
	BOOL	m_CacheModified;

	i3PackFile *		m_pPackFile;

	//INT32		m_CacheBufIdx; 
	char	*	m_pCacheBuf;

	char FileName[MAX_PATH];

	static BOOL	m_bCaseSensitive;

	virtual UINT32	_Read( void * pBuf, INT32 Length);
	virtual UINT32	_Write( void * pBuf, INT32 Length);

	virtual UINT32	_GetPosition(void);
	virtual UINT32	_SetPosition( INT32 Pos, INT32 MoveType);

	void	ResetCache(void);

	// 파일 사이즈 계산하기
	UINT32	CaculateSize(void);

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
	virtual UINT32 Write( void * pBuf, INT32 Length);
	virtual UINT32 SetPosition( INT32 Pos, INT32 MoveType);
	virtual UINT32 GetPosition(void);

	BOOL	SetCacheSize( INT32 Size);
	void	Flush(void);

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

class I3_EXPORT_BASE i3NullStream : public i3Stream
{
	I3_CLASS_DEFINE( i3NullStream);

protected:
	UINT32 m_Pos;

public:
	i3NullStream(void);
	virtual ~i3NullStream(void);

	virtual BOOL Create( const char *Name, INT32 Acc, UINT32 Size = 0);
	virtual BOOL Open( const char *Name, INT32 Acc, UINT32 Size = 0);
	virtual BOOL Close(void);
	
	virtual UINT32 Read( void *Buf, INT32 Length);
	virtual UINT32 Write( void *Buf, INT32 Length);
	virtual UINT32 SetPosition( INT32 Pos, INT32 MoveType);
	virtual UINT32 GetPosition(void);
};

#endif
