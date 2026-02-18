#include "i3FrameworkPCH.h"
#include "i3VirtualTextureIO.h"
#include "i3VirtualTexture.h"
#include "i3VirtualTextureCache.h"

#include "i3UIManager.h"

#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

#include "i3Base/string/ext/change_file_ext.h"

#include "i3VTFileFormatController.h"

I3_CLASS_INSTANCE( i3VirtualTextureIO);

#define VT_QUE_UNITSIZE	1024


namespace i3VTexImp
{
	
	I3VT_IOREQ * ResizeQueueBuffer(i3::vector<I3VT_IOREQ*>& inoutQueue)
	{
		I3VT_IOREQ * pReq = (I3VT_IOREQ*)i3MemAlloc(sizeof(I3VT_IOREQ) * VT_QUE_UNITSIZE);
		i3mem::FillZero(pReq, sizeof(I3VT_IOREQ) * VT_QUE_UNITSIZE);
		UINT32 preCnt = inoutQueue.size();

		inoutQueue.resize(preCnt + VT_QUE_UNITSIZE);

		for (INT32 i = 0; i < VT_QUE_UNITSIZE; i++)
		{
			inoutQueue[preCnt + i] = &pReq[i];
		}

		return pReq;
	}

}




i3VirtualTextureIO::i3VirtualTextureIO(void)
{
	i3mem::FillZero( &m_Ovl, sizeof(m_Ovl));
	m_Queue.reserve( VT_QUE_UNITSIZE * 4);
	i3VTexImp::ResizeQueueBuffer(m_Queue);
}

i3VirtualTextureIO::~i3VirtualTextureIO(void)
{
	
	_DestroyIOThread( true);
	
	I3_SAFE_RELEASE( m_pSignal_IO);

	I3_SAFE_RELEASE( m_pSignal_Thread);

	_FreeAllQueueBuffer();
}

void i3VirtualTextureIO::_FreeAllQueueBuffer( void)
{
	for( UINT32 i = 0; i < m_Queue.size(); i+=VT_QUE_UNITSIZE)
	{
		I3VT_IOREQ * pReq = m_Queue.at( i);
		I3MEM_SAFE_FREE( pReq);
	}

	m_Queue.clear();
}

bool i3VirtualTextureIO::Create()
{
	m_pSignal_IO = i3Signal::new_object();
	m_pSignal_IO->Create( true);

	m_pSignal_Thread = i3Signal::new_object();
	m_pSignal_Thread->Create( false);
	m_pSignal_Thread->Reset();

	_CreateIOThread();

	return true;
}

void i3VirtualTextureIO::_CreateIOThread(void)
{
	I3ASSERT( m_pThread == nullptr);

	m_pSignal_Thread->Reset();

	m_pThread = i3VirtualTextureIO_Thread::new_object();

	m_pThread->setup( this, m_pSignal_IO, m_pSignal_Thread);
	m_pThread->Create( "VirtualTextureIO_Thread", 0, 4096, nullptr, PRIORITY_NORMAL);
}

void i3VirtualTextureIO::_DestroyIOThread( bool bWait)
{
	I3_SAFE_RELEASE( m_pThread);

	m_pSignal_Thread->Reset();
}


INT32 i3VirtualTextureIO::GetNextHead(void)
{
	//INT32 idxNext;

	//idxNext = m_idxHead + 1;
	//if( idxNext >= MAX_VTQUEUE)
	//	idxNext = 0;

	//while( (idxNext == m_idxTail) ||
	//	(m_Queue[ idxNext].m_State == I3VT_REQSTATE_READY ||
	//	m_Queue[ idxNext].m_State == I3VT_REQSTATE_LOADING ) )
	//{
	//	//// Ring-Buffer 구조이기 때문에, 과다한 Request가 들어올 경우
	//	//// Loading 중인 Queue까지 덮어쓸 수 있다.
	//	//// Tail의 다음에 기록해 둔다.
	//	idxNext ++;
	//	if( idxNext >= MAX_VTQUEUE)
	//		idxNext = 0;

	//	//I3TRACE( "I/O Request exceed %d\n", MAX_VTQUEUE);

	//	m_pSignal_Thread->Raise();

	//	i3Timer::Sleep( 1);	// mainthread에서 기다려보자..
	//}

	//return idxNext;
	return -1;
}

I3VT_REQ_HNDL i3VirtualTextureIO::ReqLoad( I3VT_IOHANDLE * hndl, UINT64 offset, UINT32 sz, char * pLoadBuf, INT32 keyUser, i3VirtualTextureCache * pCache)
{
	I3ASSERT( hndl != nullptr);

	m_Mutex.Enter();

	if( m_idxHead >= m_Queue.size())
		m_idxHead = 0;

	I3VT_IOREQ * pReq = m_Queue[ m_idxHead];
	
	UINT32 cnt = 0;
	while( pReq->m_State == I3VT_REQSTATE_READY ||
		pReq->m_State == I3VT_REQSTATE_LOADING )
	{
		m_idxHead++;
		if( m_idxHead >= m_Queue.size())
			m_idxHead = 0;

		pReq = m_Queue[ m_idxHead];

		cnt++;
		if( cnt >= m_Queue.size())
		{	// 새로 버퍼를 늘리자.
			m_idxHead = static_cast<INT32>(m_Queue.size());
			pReq = i3VTexImp::ResizeQueueBuffer(m_Queue); 
			break;
		}
	}

	// 파일 사이즈 검증
#if defined( I3_DEBUG)
	DWORD high, low;
	low = ::GetFileSize( hndl, &high);
	UINT64 size = (UINT64)low | ((UINT64)high << 32);
	I3ASSERT( offset + sz <= size);
#endif

	{
		pReq->m_bRead	= true;
		pReq->m_Offset	= offset;
		pReq->m_Size	= sz;
		pReq->m_pVTex	= hndl;
		pReq->m_State	= I3VT_REQSTATE_READY;

		pReq->m_pCache	= pCache;
		pReq->m_Key		= keyUser;
		pReq->m_pBuffer	= pLoadBuf;
		pReq->m_bReadEnd = false;
	}
	m_Mutex.Leave();
	
	m_pSignal_Thread->Raise();

	return m_idxHead;
}

I3VT_REQ_HNDL i3VirtualTextureIO::ReqWrite( I3VT_IOHANDLE * hndl, UINT64 offset, UINT32 sz, char * pBuf)
{
	m_Mutex.Enter();

	I3VT_IOREQ * pReq = m_Queue[ m_idxHead];
	
	UINT32 cnt = 0;
	while( pReq->m_State == I3VT_REQSTATE_READY ||
		pReq->m_State == I3VT_REQSTATE_LOADING )
	{
		m_idxHead++;
		if( m_idxHead >= m_Queue.size())
			m_idxHead = 0;

		pReq = m_Queue[ m_idxHead];

		cnt++;
		if( cnt >= m_Queue.size())
		{	// 새로 버퍼를 늘리자.
			m_idxHead = static_cast<INT32>(m_Queue.size());
			pReq = i3VTexImp::ResizeQueueBuffer(m_Queue); 
			break;
		}
	}

	{
		pReq->m_bRead		= false;
		pReq->m_Offset	= offset;
		pReq->m_Size		= sz;
		pReq->m_pVTex		= hndl;
		pReq->m_State		= I3VT_REQSTATE_READY;

		pReq->m_pCache	= nullptr;
		pReq->m_Key		= 0;
		pReq->m_pBuffer	= pBuf;
		pReq->m_bReadEnd = false;
	}
	m_Mutex.Leave();

	m_pSignal_Thread->Raise();

	return m_idxHead;
}

bool i3VirtualTextureIO::IsFinished( I3VT_REQ_HNDL hndl)
{
	I3VT_IOREQ * pReq;

	I3_BOUNDCHK( hndl, static_cast<INT32>(m_Queue.size()));

	pReq = m_Queue[ hndl];

	if( pReq->m_State == I3VT_REQSTATE_FINISHED)
		return true;

	return false;
}

bool i3VirtualTextureIO::Wait( I3VT_REQ_HNDL hndl)
{
	I3ASSERT( m_pSignal_IO != nullptr);
	I3_BOUNDCHK( hndl, static_cast<INT32>(m_Queue.size()));
	
	I3VT_IOREQ * pReq;

	pReq = m_Queue[ hndl];

	while( pReq->m_State != I3VT_REQSTATE_FINISHED)
	{
		m_pSignal_Thread->Raise();	// Thread가 wait상태일 수 있다.
		i3Timer::Sleep( 1);
	}

	return pReq->m_State == I3VT_REQSTATE_FINISHED;
}

I3VT_IOREQ * i3VirtualTextureIO::getTailReq( void)
{
	return m_Queue[ m_idxTail];
}

bool i3VirtualTextureIO::_IsEmpty( void)
{
	// 다음 Req가 Ready 상태라면...
	for( UINT32 i = 0; i < m_Queue.size(); i++)
	{
		if( m_Queue[m_idxTail]->m_State == I3VT_REQSTATE_READY)
			return false;

		m_idxTail++;
		if( m_idxTail >= m_Queue.size())
			m_idxTail = 0;
	}

	return true;
}

//
// I/O Thread에서 호출된다!!!!!
//
bool i3VirtualTextureIO::_Kick(void)
{
	// Tail 위치의 상태를 확인하고, 만약 Ready 상태이면 I/O를 시작시킨다.
	I3VT_IOREQ * pReq = getTailReq();
	I3VT_IOHANDLE * pInfo;

	if( pReq == nullptr)
		return false;

	if( pReq->m_State == I3VT_REQSTATE_READY)
	{
		// Texture 정보
		pInfo = pReq->m_pVTex;

		pReq->m_State = I3VT_REQSTATE_LOADING;

		m_pSignal_IO->Reset();

		if( pReq->m_bRead)
		{
			return _Read( pInfo->m_hFile, pReq->m_Offset, pReq->m_Size, pReq->m_pBuffer
#if defined( I3_DEBUG)
				, pInfo->m_szPath
#endif
				);
		}
		else
		{
			return _Write( pInfo->m_hFile, pReq->m_Offset, pReq->m_Size, pReq->m_pBuffer);
		}
	}

	return false;
}

//
// I/O Thread에서 호출된다!!!!!
//
bool i3VirtualTextureIO::_WaitForCompletion(void)
{
	I3VT_IOREQ * pReq = getTailReq();
	I3VT_IOHANDLE* pInfo = pReq->m_pVTex;

	DWORD dwTrans = 0;

	if( pReq->m_State == I3VT_REQSTATE_LOADING)
	{
		//다 읽었는지 체크, 다 읽었다면.. wait를 하지 않는다.
		//if( pReq->m_bReadEnd == false)
		{
			while( ::GetOverlappedResult(pInfo->m_hFile, &m_Ovl, &dwTrans, FALSE) == FALSE )
			{
				DWORD err = ::GetLastError();
				if( err == ERROR_IO_INCOMPLETE )	// I/O가 완전히 종료될 때까지 대기...
					continue;

				m_pSignal_IO->WaitFor();
				I3TRACE( "--------------->WaitFor VT File %d\n", err);
				break;
			}

		//	pReq->m_bReadEnd = true;
		}

		if( pReq->m_pCache != nullptr)
		{
			i3VirtualTextureCache * pCache = pReq->m_pCache;

			//if( pCache->CompleteLoad( pReq->m_pBuffer, pReq->m_Key) )
			while( pCache->CompleteLoad( pReq->m_pBuffer, pReq->m_Key) == false)
			{
				i3Timer::Sleep(1);
			}
		}

		pReq->m_State = I3VT_REQSTATE_FINISHED;
	}

	return true;
	
};


bool i3VirtualTextureIO::_Read( __HANDLE_TYPE hFile, UINT64 offset, UINT32 sz, char * pDestBuf
#if defined( I3_DEBUG)
							   , const char * pszFilePath
#endif
							   )
{
#if defined( I3_WINDOWS)
	DWORD rb;

	m_Ovl.Internal = 0;
	m_Ovl.InternalHigh = 0;
	m_Ovl.Offset = (UINT32)(offset & 0xFFFFFFFF);
	m_Ovl.OffsetHigh = (UINT32)(offset >> 32);
	m_Ovl.hEvent = m_pSignal_IO->getEventObject();

	DWORD high;
	DWORD low = ::GetFileSize( hFile, &high);
	UINT64 fileSize = (UINT64)low | ((UINT64)high << 32);

	I3ASSERT( offset < fileSize);

	BOOL bResult = ::ReadFile( hFile, pDestBuf, sz, &rb, &m_Ovl);

	DWORD dwRet = 0; 
	if (FALSE == bResult)
	{
		dwRet = GetLastError();
		if( dwRet != ERROR_IO_PENDING )
		{
			if( dwRet == ERROR_HANDLE_EOF)
			{
#if defined( I3_DEBUG)
				I3PRINTLOG(I3LOG_NOTICE,  "low : %d, high : %d, offset : %d, size : %d", low, high, offset, sz);
				I3PRINTLOG(I3LOG_FATAL,  "Load VT file err = EOF. %s", pszFilePath);
#else
				I3PRINTLOG(I3LOG_FATAL,  "Load VT file err = EOF");
#endif
			}
			else
			{
#if defined( I3_DEBUG)
				I3PRINTLOG(I3LOG_FATAL,  "Load VT file err = %d (%s)", dwRet, pszFilePath);
#else
				I3PRINTLOG(I3LOG_FATAL,  "Load VT file err = %d", dwRet);
#endif
			}
			return false;
		}

	}
#endif

	return true;
}

bool i3VirtualTextureIO::_Write( __HANDLE_TYPE hFile, UINT64 offset, UINT32 sz, char * pDestBuf)
{
#if defined( I3_WINDOWS)
	DWORD wb;

	m_Ovl.hEvent = m_pSignal_IO->getEventObject();
	m_Ovl.Internal = 0;
	m_Ovl.InternalHigh = 0;
	m_Ovl.Offset = (UINT32)(offset & 0xFFFFFFFF);
	m_Ovl.OffsetHigh = (UINT32)(offset >> 32);

	WriteFile( hFile, pDestBuf, sz, &wb, &m_Ovl);
#endif

	return true;
}

void i3VirtualTextureIO::_CancelIO( __HANDLE_TYPE hFile)
{
#if defined( I3_WINDOWS)
	::CancelIo( hFile);
#endif
}


/////////////////////////////////////////////////////////////////
void i3VirtualTextureIO::Pause( bool bCancelIO )
{
	I3VT_IOREQ * pReq = getTailReq();

	// I/O Thread를 정지시킨다.
	{
		_DestroyIOThread( true);
	}

	if( bCancelIO)
	{
		if( pReq->m_State == I3VT_REQSTATE_LOADING)
		{
			I3VT_IOHANDLE * pInfo;

			pInfo = pReq->m_pVTex;

			// I/O Cancel
			_CancelIO( pInfo->m_hFile);
		}
	}

	// 모든 Request를 제거
	m_Mutex.Enter();
	{
		m_idxHead = m_idxTail = 0;
	}
	m_Mutex.Leave();
}

void i3VirtualTextureIO::Resume(void)
{
	// ResetDevice가 2번 호출될 수 있다.
	if( m_pThread == nullptr)
		_CreateIOThread();
}

bool i3VirtualTextureIO::IsEmpty( void)
{
	for( UINT32 i = 0; i < m_Queue.size(); i++)
	{
		if( m_Queue[i]->m_State == I3VT_REQSTATE_READY ||
			m_Queue[i]->m_State == I3VT_REQSTATE_LOADING)
			return false;
	}

	return true;
}


void i3VirtualTextureIO::WakeupIOThread( void)
{
	m_pSignal_Thread->Raise();
}
