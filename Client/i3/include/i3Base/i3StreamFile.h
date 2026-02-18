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
	I3_EXPORT_CLASS_DEFINE( i3FileStream, i3Stream);

protected:
	__HANDLE_TYPE		m_pFile = INVALID_HANDLE_VALUE;

	UINT32 m_Pos = 0;
	UINT32 m_InternalOffset = 0;

	i3PackFile *		m_pPackFile = nullptr;

	i3::rc_string		m_FileName;

	static bool	m_bCaseSensitive;

public:
	virtual ~i3FileStream(void);

	const char *			GetFileName(void) const		{ return m_FileName.c_str(); }
	const i3::rc_string&	GetFileNameString() const	{ return m_FileName; }

	// 대소문자 구분하기
	static void	SetCaseSensitive( bool bTrue = true)	{ m_bCaseSensitive = bTrue; }
	static bool	IsCaseSensitive()						{ return m_bCaseSensitive; }

	virtual bool  Create( const i3::rc_string& strFileName, INT32 Acc, UINT32 Size = 0);
	virtual bool  Open(const i3::rc_string& strFileName, INT32 Acc, UINT32 Size = 0);

	virtual bool  Close(void) override;
	virtual bool  IsOpened() const override;

	virtual UINT32 Read( void * pBuf, INT32 Length) override;
	virtual UINT32 Write( const void * pBuf, INT32 Length) override;
	virtual UINT32 SetPosition( INT32 Pos, INT32 MoveType = STREAM_BEGIN) override;
	virtual UINT32 GetPosition(void) const { return m_Pos; }

	// 파일 사이즈 계산하기
	UINT32	CalculateSize(void);

	static void		SetIsFileCallback( bool (*)(const char* ) );
	static bool		isFile( const char * pszPath);


	static i3FileStream * FromHandle( HANDLE Handle);

	
	#ifdef I3_DEBUG
		static	void SetLogEnable( const char * pszFile, bool bEnable);
		static  void WriteOpenLog( const char * szFileName );
	#endif
};


#endif
