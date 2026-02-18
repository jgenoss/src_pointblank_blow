#include "i3GfxType.h"

#if defined( I3G_DX)
#include "i3IndexArray.h"
#include "i3GfxGlobalVariableDx.h"
#include "i3GfxUtilDX.h"
#include "i3RenderContext.h"
#include "i3GfxGlobalVariable.h"

#include <queue>

#define IA_REQ_BUF_COUNT 128

enum IA_REQ_TYPE {
	IA_REQ_TYPE_NONE = 0,
	IA_REQ_TYPE_RESERVE,
	IA_REQ_TYPE_CREATE,
	IA_REQ_TYPE_LOCK,
	IA_REQ_TYPE_UNLOCK
};

struct IADXREQINFO {
	UINT32			_idx = 0;
	UINT32			_param1 = 0;
	UINT32			_param2 = 0;
	UINT32			_param3 = 0;
	UINT32			_param4 = 0;
	i3IndexArray *	_IA = nullptr;

	UINT32			_result = 0;
	IA_REQ_TYPE		_type = IA_REQ_TYPE::IA_REQ_TYPE_NONE;
};

static bool			s_IAReqInit = false;
static UINT32		s_IAReqRead = 0;
static UINT32		s_IAReqWrite = 0;
static IADXREQINFO	s_IAReq[IA_REQ_BUF_COUNT];
static IADXREQINFO	s_IAAck[IA_REQ_BUF_COUNT];

static i3Mutex2		sDXIAMutex;

i3IndexArray::i3IndexArray(void)
{
	if( s_IAReqInit == false)
	{
		i3mem::FillZero( s_IAReq, sizeof( s_IAReq));
		i3mem::FillZero( s_IAAck, sizeof( s_IAAck));
		s_IAReqInit = true;
	}
}

i3IndexArray::~i3IndexArray(void)
{
	NetworkDump_Free( m_pBuffer, __FILE__, __LINE__);
	I3_SAFE_RELEASE_NODBG( m_pBuffer);
	I3MEM_SAFE_FREE( m_pResData);
}

UINT32 i3IndexArray::CreateDXBuffer( UINT32 Sz, UINT32 Usage, D3DFORMAT fmt, D3DPOOL Pool)
{
	i3Monitor2 mon( &sDXIAMutex);

	while( s_IAReqWrite - s_IAReqRead >= IA_REQ_BUF_COUNT)
	{
		i3Timer::Sleep(1);
	}

	UINT32 idx = s_IAReqWrite;	//sFindDXIAReqInfoIdx();
	
	while( s_IAAck[ idx % IA_REQ_BUF_COUNT]._type != IA_REQ_TYPE_NONE)
	{
		i3Timer::Sleep(1);
	}

	IADXREQINFO * pReq = &s_IAReq[idx % IA_REQ_BUF_COUNT];

	pReq->_IA = this;
	I3_MUST_ADDREF( pReq->_IA);
	I3ASSERT( pReq->_IA->GetRefCount() > 1);
	pReq->_param1 = Sz;
	pReq->_param2 = Usage;
	pReq->_param3 = (UINT32) fmt;
	pReq->_param4 = (UINT32) Pool;
	pReq->_idx = idx;
	pReq->_type = IA_REQ_TYPE_CREATE;

	//s_IAReqQueue.push( pReq);
	s_IAReqWrite++;

	return idx;
}

UINT32 i3IndexArray::LockDXBuffer( UINT32 start, UINT32 size, I3G_LOCKMETHOD lock)
{
	i3Monitor2 mon( &sDXIAMutex);

	while( s_IAReqWrite - s_IAReqRead >= IA_REQ_BUF_COUNT)
	{
		i3Timer::Sleep(1);
	}

	UINT32 idx = s_IAReqWrite;	//sFindDXIAReqInfoIdx();
	
	while( s_IAAck[ idx % IA_REQ_BUF_COUNT]._type != IA_REQ_TYPE_NONE)
	{
		i3Timer::Sleep(1);
	}

	IADXREQINFO * pReq = &s_IAReq[idx % IA_REQ_BUF_COUNT];
	pReq->_IA = this;
	I3_MUST_ADDREF( pReq->_IA);
	I3ASSERT( pReq->_IA->GetRefCount() > 1);
	
	pReq->_param1 = start;
	pReq->_param2 = size;
	pReq->_param3 = (UINT32) lock;
	pReq->_idx = idx;
	pReq->_type = IA_REQ_TYPE_LOCK;

	//s_IAReqQueue.push( pReq);
	s_IAReqWrite++;

	return idx;
}

UINT32 i3IndexArray::UnlockDXBuffer( void)
{
	i3Monitor2 mon( &sDXIAMutex);

	while( s_IAReqWrite - s_IAReqRead >= IA_REQ_BUF_COUNT)
	{
		i3Timer::Sleep(1);
	}

	UINT32 idx = s_IAReqWrite;	//sFindDXIAReqInfoIdx();
	
	while( s_IAAck[ idx % IA_REQ_BUF_COUNT]._type != IA_REQ_TYPE_NONE)
	{
		i3Timer::Sleep(1);
	}

	IADXREQINFO * pReq = &s_IAReq[idx % IA_REQ_BUF_COUNT];
	pReq->_IA = this;
	I3_MUST_ADDREF( pReq->_IA);
	I3ASSERT( pReq->_IA->GetRefCount() > 1);
	pReq->_idx = idx;
	pReq->_type = IA_REQ_TYPE_UNLOCK;
	
	//s_IAReqQueue.push( pReq);
	s_IAReqWrite++;

	return idx;
}

UINT32 i3IndexArray::GetReqResult( UINT32 req)
{
	UINT32 result = 0;

	bool bWait = true;

	while( bWait)
	{
		for( UINT32 i = 0; i < IA_REQ_BUF_COUNT; i++)
		{
			IADXREQINFO * pAck = &s_IAAck[ i];

			if( pAck->_idx == req &&
				pAck->_type > IA_REQ_TYPE_RESERVE)
			{
				result = pAck->_result;
				
				pAck->_IA = nullptr;
				pAck->_type = IA_REQ_TYPE_NONE;

				bWait = false;
				break;
			}
		}

		if( bWait)
		{
			i3Timer::Sleep( 1);
		}
	}

	return result;
}

static i3Mutex2 sIndexArrayUpdateMutex;

void i3IndexArray::UpdateForThread( void)
{
	i3Monitor2 mon( &sIndexArrayUpdateMutex);

	UINT32 maxCnt = s_IAReqWrite - s_IAReqRead;//s_IAReqQueue.size();

	for( UINT32 i = 0; i < maxCnt; i++)
	{
		UINT32 idx = s_IAReqRead % IA_REQ_BUF_COUNT;
		IADXREQINFO * pReq = &s_IAReq[ idx];	//s_IAReqQueue.front();

		I3ASSERT( pReq->_IA != nullptr);
		I3ASSERT( pReq->_type > IA_REQ_TYPE_RESERVE );
		{
			switch( pReq->_type)
			{
			case IA_REQ_TYPE_CREATE:
				{
					pReq->_result = g_pD3DDevice->CreateIndexBuffer( pReq->_param1, pReq->_param2,
						(D3DFORMAT) pReq->_param3, (D3DPOOL) pReq->_param4, &pReq->_IA->m_pBuffer, NULL);
					CHECKRESULT(pReq->_result);
					NetworkDump_Malloc( pReq->_IA->m_pBuffer, pReq->_param1, __FILE__, __LINE__); 
				}
				break;

			case IA_REQ_TYPE_LOCK:
				{
					pReq->_result = (UINT32) pReq->_IA->GetDxBuffer()->Lock( pReq->_param1, pReq->_param2,
						(VOID **) &pReq->_IA->m_pLockedData, (I3G_LOCKMETHOD) pReq->_param3);
					CHECKRESULT(pReq->_result);
				}
				break;

			case IA_REQ_TYPE_UNLOCK:
				{
					pReq->_result = (UINT32) pReq->_IA->GetDxBuffer()->Unlock();
					CHECKRESULT(pReq->_result);
				}
				break;
			default :
				I3PRINTLOG(I3LOG_FATAL,  "아직 등록하지 않은 타입!!!!");
				break;
			}

			s_IAAck[idx] = *pReq;

			I3_SAFE_RELEASE( pReq->_IA);
			pReq->_type = IA_REQ_TYPE_NONE;
		}

		s_IAReqRead++;
	}
}

bool i3IndexArray::Create( UINT32 count, I3G_USAGE usage, UINT32 vaCount)
{
	HRESULT Rv;
	UINT32 Sz;
	DWORD dxUsage;
	D3DPOOL Pool;
	D3DFORMAT fmt;

	if( vaCount > 0xFFFF)
	{
		m_DataType = I3G_IDX_32BITS;
	}
	else
	{
		m_DataType = I3G_IDX_16BITS;
	}

	m_Count = count;

	if( m_DataType == I3G_IDX_16BITS)
	{
		Sz = sizeof(UINT16) * count;
		fmt = D3DFMT_INDEX16;
	}
	else
	{
		Sz = sizeof(UINT32) * count;
		fmt = D3DFMT_INDEX32;
	}
	
	SetUsageFlag( usage);

	dxUsage = i3DXUT_GetNativeUsage( usage);
    
	if( usage & I3G_USAGE_SOFTWAREPROCESSING )
	{
		Pool = D3DPOOL_SYSTEMMEM;
	}
	else if( usage & (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE))
	{
		Pool = D3DPOOL_DEFAULT;
	}
	else
	{
		Pool = D3DPOOL_MANAGED;
	}

	if( m_bInThread)
	{
		UINT32 req = CreateDXBuffer( Sz, dxUsage, fmt, Pool);

		Rv = (HRESULT) GetReqResult( req);	// blocked
	}
	else
	{
		Rv = g_pD3DDevice->CreateIndexBuffer( Sz, dxUsage, fmt, Pool, &m_pBuffer, NULL);
		NetworkDump_Malloc( m_pBuffer, Sz, __FILE__, __LINE__);
	}

	if( Rv == D3DERR_OUTOFVIDEOMEMORY)
	{
		I3PRINTLOG(I3LOG_NOTICE, "CreateIndexBuffer D3DERR_OUTOFVIDEOMEMORY");
		// Video Memory가 고갈되었다면, 기존에 Loading된 IndexBuffer 등을 VRAM에서 내린 후
		// 다시 시도한다.
		g_pD3DDevice->EvictManagedResources();

		Rv = g_pD3DDevice->CreateIndexBuffer( Sz, dxUsage, fmt, Pool, &m_pBuffer, NULL);
		NetworkDump_Malloc( m_pBuffer, Sz, __FILE__, __LINE__);
	}

	if( FAILED( Rv))
	{
		switch( Rv)
		{
		case D3DERR_OUTOFVIDEOMEMORY :	
			if( m_CallBackExternalOutOfMemory)	m_CallBackExternalOutOfMemory();
			I3ERRLOG( i3Error::_OUTOFVIDEOMEMORY);
			break;
		default :
			I3ERRLOG( i3Error::_UNSUPPORTED_GPU);	break;
		}

		CHECKRESULT(Rv);
		return false;
	}

	I3ASSERT( !FAILED( Rv));

	return true;
}

bool i3IndexArray::Create( UINT32 count, I3G_USAGE usage, I3G_IDX_TYPE type)
{
	HRESULT Rv;
	UINT32 Sz;
	DWORD dxUsage;
	D3DPOOL Pool;
	D3DFORMAT fmt;

	m_DataType = type;
	m_Count = count;

	if( m_DataType == I3G_IDX_16BITS)
	{
		Sz = sizeof(UINT16) * count;
		fmt = D3DFMT_INDEX16;
	}
	else
	{
		Sz = sizeof(UINT32) * count;
		fmt = D3DFMT_INDEX32;
	}

	SetUsageFlag( usage);

	dxUsage = i3DXUT_GetNativeUsage( usage);

	if( usage & I3G_USAGE_SOFTWAREPROCESSING )
	{
		Pool = D3DPOOL_SYSTEMMEM;
	}
	else if( usage & (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE))
	{
		Pool = D3DPOOL_DEFAULT;
	}
	else
	{
		Pool = D3DPOOL_MANAGED;
	}

	if( m_bInThread)
	{
		UINT32 req = CreateDXBuffer( Sz, dxUsage, fmt, Pool);

		Rv = (HRESULT) GetReqResult( req);	// blocked
	}
	else
	{
		Rv = g_pD3DDevice->CreateIndexBuffer( Sz, dxUsage, fmt, Pool, &m_pBuffer, nullptr);
		NetworkDump_Malloc( m_pBuffer, Sz, __FILE__, __LINE__);
	}

	if( Rv == D3DERR_OUTOFVIDEOMEMORY)
	{
		I3PRINTLOG(I3LOG_NOTICE, "CreateIndexBuffer D3DERR_OUTOFVIDEOMEMORY");
		// Video Memory가 고갈되었다면, 기존에 Loading된 IndexBuffer 등을 VRAM에서 내린 후
		// 다시 시도한다.
		g_pD3DDevice->EvictManagedResources();

		Rv = g_pD3DDevice->CreateIndexBuffer( Sz, dxUsage, fmt, Pool, &m_pBuffer, nullptr);
		NetworkDump_Malloc( m_pBuffer, Sz, __FILE__, __LINE__);
	}

	if( FAILED( Rv))
	{
		switch( Rv)
		{
		case D3DERR_OUTOFVIDEOMEMORY :	
			if( m_CallBackExternalOutOfMemory)	m_CallBackExternalOutOfMemory();
			I3ERRLOG( i3Error::_OUTOFVIDEOMEMORY);
			break;
		default :
			I3ERRLOG( i3Error::_UNSUPPORTED_GPU);	break;
		}

		CHECKRESULT(Rv);
		return false;
	}

	I3ASSERT( !FAILED( Rv));

	return true;
}

void i3IndexArray::SetDataType( I3G_IDX_TYPE type)
{
	I3ASSERT( m_pBuffer == nullptr);

	m_DataType = type;
}

bool i3IndexArray::Lock( UINT32 StartIdx, UINT32 count, I3G_LOCKMETHOD lock)
{
	HRESULT Rv;
	UINT32 lockStart, lockSz, unitSz;

	I3ASSERT( m_pBuffer != nullptr);

	m_LockStartIdx = StartIdx;

	if( m_DataType == I3G_IDX_16BITS)
		unitSz = sizeof(UINT16);
	else
		unitSz = sizeof( UINT32);

	lockStart = unitSz * StartIdx;
	
	if( count == 0)
	{
		count = m_Count;
	}

	m_LockCount = count;

	I3ASSERT((m_LockStartIdx + m_LockCount) <= m_Count);

	lockSz = unitSz * count;

	I3ASSERT( m_pLockedData == nullptr);

	Rv = m_pBuffer->Lock( lockStart, lockSz, (VOID **) &m_pLockedData, lock);
	CHECKRESULT( Rv);

	m_State |= I3G_INDEXARRAY_STATE_LOCKED;

	return true;
}

void i3IndexArray::Unlock(void)
{
	HRESULT Rv;

	I3ASSERT( m_pBuffer != nullptr);

	/*if( m_bInThread)
	{
		UINT32 req = UnlockDXBuffer();
		Rv = (HRESULT) GetReqResult( req);
	}
	else*/
		Rv = m_pBuffer->Unlock();
	I3ASSERT( !FAILED( Rv));

	m_LockCount = 0;
	m_pLockedData = nullptr;
	m_State &= ~I3G_INDEXARRAY_STATE_LOCKED;
}

bool i3IndexArray::SafeLock( UINT32 StartIdx, UINT32 count, I3G_LOCKMETHOD lock)
{
	m_Mutex.Enter();

	return Lock( StartIdx, count, lock);
}

bool i3IndexArray::SafeUnlock(void)
{
	Unlock();

	m_Mutex.Leave();
	return true;
}

void i3IndexArray::SetIndex( UINT32 idx, UINT32 val)
{
	I3ASSERT( m_pLockedData != nullptr);
	I3ASSERT( idx >= m_LockStartIdx);
	I3ASSERT( idx < (m_LockStartIdx + m_Count));

#if defined( I3_DEBUG)
	if( m_DataType == I3G_IDX_16BITS)
	{
		I3ASSERT( idx < 0x00010000)
	}
#endif

	
	
	if( m_DataType == I3G_IDX_16BITS)
	{
		UINT16 * pDest = (UINT16 *) m_pLockedData;

		*(pDest + (idx - m_LockStartIdx)) = (UINT16) (val & 0xFFFF);
	}
	else
	{
		UINT32 * pDest = (UINT32 *) m_pLockedData;

		*(pDest + (idx - m_LockStartIdx)) = val;
	}
}

void i3IndexArray::SetIndices( UINT32 idx, UINT32 Count, UINT16 * pIndices, UINT32 off)
{
	UINT32 i;

	I3ASSERT( idx >= m_LockStartIdx);
	I3ASSERT( idx < (m_LockStartIdx + m_Count));
	I3ASSERT( (idx + Count) <= (m_LockStartIdx + m_Count));

	if( m_DataType == I3G_IDX_16BITS)
	{
		UINT16 * pDest = (UINT16 *) m_pLockedData;

		I3ASSERT( off < 0xFFFF);

		pDest = pDest + (idx - m_LockStartIdx);

		for( i = 0; i < Count; i++)
		{
			*pDest = *pIndices + (UINT16)off;

			pDest++;
			pIndices++;
		}
	}
	else
	{
		UINT32 * pDest = (UINT32 *) m_pLockedData;

		pDest = pDest + (idx - m_LockStartIdx);

		for( i = 0; i < Count; i++)
		{
			*pDest = *pIndices + off;

			pDest++;
			pIndices++;
		}
	}
}

void i3IndexArray::SetIndices( UINT32 idx, UINT32 Count, UINT32 * pIndices, UINT32 off)
{
	UINT32 i;

	I3ASSERT( idx >= m_LockStartIdx);
	I3ASSERT( idx < (m_LockStartIdx + m_Count));
	I3ASSERT( (idx + Count) <= (m_LockStartIdx + m_Count));

	if( m_DataType == I3G_IDX_16BITS)
	{
		UINT16 * pDest = (UINT16 *) m_pLockedData;

		pDest = pDest + (idx - m_LockStartIdx);

		for( i = 0; i < Count; i++)
		{
			UINT32 val = *pIndices + off;

			I3ASSERT( val < 0x00010000);

			*pDest = (UINT16) (val & 0x0000FFFF);

			pDest++;
			pIndices++;
		}
	}
	else
	{
		UINT32 * pDest = (UINT32 *) m_pLockedData;

		pDest = pDest + (idx - m_LockStartIdx);

		for( i = 0; i < Count; i++)
		{
			*pDest = *pIndices + off;

			pDest++;
			pIndices++;
		}
	}
}

UINT32 i3IndexArray::GetIndex( UINT32 idx)
{
	I3ASSERT( m_pLockedData != nullptr);

	I3ASSERT( idx >= m_LockStartIdx);
	I3ASSERT( idx < (m_LockStartIdx + m_Count));

	if( m_DataType == I3G_IDX_16BITS)
	{
		UINT16 * pDest = (UINT16 *) m_pLockedData;

		return *(pDest + (idx - m_LockStartIdx));
	}


	UINT32 * pDest = (UINT32 *) m_pLockedData;

	return *(pDest + (idx - m_LockStartIdx));
}

void i3IndexArray::MakeRestoreData( void)
{
	I3MEM_SAFE_FREE( m_pResData);

	if( m_DataType == I3G_IDX_16BITS)
		m_ResDataSize = GetCount() * sizeof(UINT16);
	else
		m_ResDataSize = GetCount() * sizeof(UINT32);

	m_pResData = (char *) i3MemAlloc( m_ResDataSize);

	Lock( 0, 0, I3G_LOCK_READONLY);

	memcpy( m_pResData, GetData(), m_ResDataSize);

	Unlock();
}

bool i3IndexArray::Destroy( bool bLostDevice)
{
	if( m_pBuffer == nullptr) return true;

	bool bDestroy = false;

	if( GetUsageFlag() & (I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE))
	{
		bDestroy = true;
	}
	else
	{
		if( bLostDevice)
			bDestroy = true;
	}

	bool bTemp = m_bInThread;
	m_bInThread = false;

	if( IsLocked() )
	{	// 다른 쓰레드에서 Lock한 상태로 DeviceLost가 될 수 있다.
		SafeUnlock();
	}

	if (g_pRenderContext->IsRenderFlag( I3RENDER_FLAG_DEVICE_RESET_NOTLOST_ERROR) == false)
	{
		if( getRestoreType() == I3G_RESTORE_MEM)
		{
			MakeRestoreData();
		}
	}

	if( bDestroy)
	{
		NetworkDump_Free( m_pBuffer, __FILE__, __LINE__);
		I3_SAFE_RELEASE_NODBG( m_pBuffer);
	}

	m_bInThread = bTemp;

	return i3GfxResource::Destroy( bLostDevice);
}

bool	i3IndexArray::isDestroyed( void)
{
	if( m_pBuffer != nullptr) return false;
	
	return true;
}


bool i3IndexArray::Revive( i3RenderContext * pCtx)
{
	if( m_pBuffer != nullptr)
		return true;

	bool bTemp = m_bInThread;
	m_bInThread = false;

	if( GetCount() > 0 )
	{
		switch( getRestoreType())
		{
			case I3G_RESTORE_NONE :
				{
					// 데이터는 저장할 필요가 없는 것이라 판단.
					// 그러나, 새롭게 생성은 해주어야 한다.
					Create( GetCount(), GetUsageFlag(), GetDataType());

					//I3ASSERT( GetUsageFlag() & I3G_USAGE_FREQUENT_UPDATE);
				}
				break;

			case I3G_RESTORE_MEM :
				{
					Create( GetCount(), GetUsageFlag(), GetDataType());

					INT32 cnt = 0;
					while( SafeLock( 0) == false)
					{
						i3Timer::Sleep(1);
						cnt++;
						if( cnt > 1000)
						{
							I3PRINTLOG(I3LOG_FATAL,  "safe lock fail !!!!!");

							return false;
						}
					}

					memcpy( GetData(), m_pResData, m_ResDataSize);

					SafeUnlock();

					I3MEM_SAFE_FREE( m_pResData);
					m_ResDataSize = 0;
				}
				break;

			case I3G_RESTORE_FILE :
				{
					m_bInThread = bTemp;
					return i3GfxResource::Revive( pCtx);
				}

		}
	}

	m_bInThread = bTemp;

	return true;
}

#if defined( I3_DEBUG)
void i3IndexArray::OnQueryStat( I3G_RES_STAT * pInfo)
{
	pInfo->m_IBCount++;

	if( m_pBuffer != nullptr)
	{
		D3DINDEXBUFFER_DESC desc;

		m_pBuffer->GetDesc( &desc);

		pInfo->m_IBSize += desc.Size;
	}
}
#endif

#endif