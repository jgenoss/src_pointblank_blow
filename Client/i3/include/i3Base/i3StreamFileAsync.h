#if !defined( __I3_STREAM_ASYNC_FILE_H)
#define __I3_STREAM_ASYNC_FILE_H

#include "i3StreamFile.h"


class I3_EXPORT_BASE i3AsyncFileStream : public i3FileStream
{
	I3_EXPORT_CLASS_DEFINE( i3AsyncFileStream, i3FileStream);
protected:
	UINT32		m_TargetPos = 0;

	#if defined( I3_WINDOWS)
		HANDLE				m_hIOCP = INVALID_HANDLE_VALUE;
		OVERLAPPED			m_Ovl;		// [initialize at constructor]
	void				_SetupAsync(void);
	void				_DestroyAsync(void);
	#endif

public:
	i3AsyncFileStream(void);

	virtual bool  Create(const i3::rc_string& strFileName, INT32 Acc, UINT32 Size = 0) override;
	virtual bool  Open(const i3::rc_string& strFileName, INT32 Acc, UINT32 Size = 0) override;
	virtual bool  Close(void) override;
	virtual bool  IsOpened() const { return i3FileStream::IsOpened(); }

	virtual UINT32 Read( void * pBuf, INT32 Length) override;
	virtual UINT32 Write( const void * pBuf, INT32 Length) override;
	virtual UINT32 SetPosition( INT32 Pos, INT32 MoveType = STREAM_BEGIN) override;
	virtual UINT32 GetPosition(void) const { return m_Pos; }

	bool	WaitCompletion();
	bool	CancelIO(void);
	bool	HasFinished(void);
};

#endif
