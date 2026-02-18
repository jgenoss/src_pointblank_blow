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
#define	STREAM_ASYNC		0x00000010

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
	virtual UINT32 Write( const void *Buf, INT32 Length)							= 0;
	virtual UINT32 CopyFrom( i3Stream *Stream, UINT32 Length);
	virtual UINT32 SetPosition( INT32 Pos, INT32 MoveType = STREAM_BEGIN)							= 0;
	virtual UINT32 GetPosition(void)												= 0;

	virtual UINT32 ReadLine( char * pStr, INT32 BufLength = -1 );
	virtual UINT32 ReadLineW( WCHAR16 * pStr, INT32 BufLength = -1 );

	virtual UINT32 WriteLine( const char * pStr);
	virtual UINT32 WriteLineW( const WCHAR16 * pStr);

	virtual UINT32 Printf( char *szBuf, const char *szFmt, ... );
};

#endif
