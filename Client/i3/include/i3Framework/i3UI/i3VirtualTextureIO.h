#if !defined( __I3_VIRTUAL_TEXTURE_IO_H)
#define __I3_VIRTUAL_TEXTURE_IO_H

#include "i3VirtualTextureDefine.h"
#include "i3VirtualTextureIO_Thread.h"

// Virtual Texture에 대한 I/O를 담당.
//
// 여러 개의 i3VirtualTexture들이 모두 하나의 i3VirtualTextureIO Class를 통해
// I/O 처리를 행한다.
//

class I3_EXPORT_FRAMEWORK i3VirtualTextureIO : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3VirtualTextureIO, i3ElementBase);

	friend i3VirtualTextureIO_Thread;

protected:
	
	i3Mutex				m_Mutex;

	UINT32				m_idxHead = 0;				// 새로운 Request가 삽입되는 위치
	UINT32				m_idxTail = 0;				// File I/O가 수행되는 위치.
	i3::vector<I3VT_IOREQ*>	m_Queue;

	i3Signal *			m_pSignal_IO = nullptr;

	i3VirtualTextureIO_Thread *		m_pThread = nullptr;
	i3Signal *			m_pSignal_Thread = nullptr;

#if defined( I3_WINDOWS)
	OVERLAPPED			m_Ovl;
#endif

protected:
	void				_FreeAllQueueBuffer( void);

	// File 

	bool				_Read( __HANDLE_TYPE hFile, UINT64 offset, UINT32 sz, char * pDestBuf
#if defined( I3_DEBUG)
		, const char * pszFilePath
#endif
		);
	bool				_Write( __HANDLE_TYPE hFile, UINT64 offset, UINT32 sz, char * pDestBuf);
	void				_CancelIO( __HANDLE_TYPE hFile);

	// Request Queue
	I3VT_IOREQ *		getTailReq(void);

	bool				_IsEmpty( void);
	bool				_Kick(void);
	bool				_WaitForCompletion(void);

	INT32				GetNextHead(void);
	

protected:
	void				_CreateIOThread(void);
	void				_DestroyIOThread( bool bWait = true);

public:
	i3VirtualTextureIO(void);
	virtual ~i3VirtualTextureIO(void);

	bool				Create();
	I3VT_REQ_HNDL		ReqLoad( I3VT_IOHANDLE * hndl, UINT64 offset, UINT32 sz, char * pBuf, INT32 keyUser, i3VirtualTextureCache * pCache = nullptr);
	I3VT_REQ_HNDL		ReqWrite( I3VT_IOHANDLE * hndl, UINT64 offset, UINT32 sz, char * pBuf);

	bool				IsFinished( I3VT_REQ_HNDL hndl);
	bool				Wait( I3VT_REQ_HNDL hndl);

	void				Pause( bool bCancelIO);
	void				Resume(void);

	bool				IsEmpty( void);
	void				WakeupIOThread( void);

	void				DestroyThread() { _DestroyIOThread(); }


};

#endif
