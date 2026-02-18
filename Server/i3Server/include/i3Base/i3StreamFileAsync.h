#if !defined( __I3_STREAM_ASYNC_FILE_H)
#define __I3_STREAM_ASYNC_FILE_H

#include "i3StreamFile.h"


class I3_EXPORT_BASE i3AsyncFileStream : public i3FileStream
{
	I3_CLASS_DEFINE( i3AsyncFileStream);
protected:
	UINT32		m_TargetPos;

	#if defined( I3_WINDOWS)
	HANDLE				m_hIOCP;
	OVERLAPPED			m_Ovl;

	void				_SetupAsync(void);
	void				_DestroyAsync(void);
	#endif

public:
	i3AsyncFileStream(void);
	virtual ~i3AsyncFileStream(void);

	virtual BOOL  Create( const char * pszName, INT32 Acc, UINT32 Size = 0);
	virtual BOOL  Open( const char * pszName, INT32 Acc, UINT32 Size = 0);
	virtual BOOL  Close(void);

	virtual UINT32 Read( void * pBuf, INT32 Length);
	virtual UINT32 Write( const void * pBuf, INT32 Length);
	virtual UINT32 SetPosition( INT32 Pos, INT32 MoveType = STREAM_BEGIN);
	virtual UINT32 GetPosition(void);

	bool	WaitCompletion();
	bool	CancelIO(void);
	bool	HasFinished(void);
};

#endif
