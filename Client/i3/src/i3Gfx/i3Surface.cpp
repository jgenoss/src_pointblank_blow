#include "i3GfxType.h"
#include "i3Surface.h"
#include "i3ImageUtil.h"
#include "i3GfxUtil.h"
#include "../include/i3Gfx/i3GfxGlobalVariable.h"

#if defined( I3G_DX)
#include "i3Input/dxerr.h"
#include "i3GfxUtilDX.h"
#include "../include/i3Gfx/i3GfxGlobalVariableDX.h"
#endif

#define SUR_REQ_BUF_COUNT	128

enum SURFACE_REQ_TYPE {
	SURFACE_REQ_TYPE_NONE = 0,
	SURFACE_REQ_TYPE_RESERVE,
	SURFACE_REQ_TYPE_GETDESC,
	SURFACE_REQ_TYPE_UPDATESF,
	SURFACE_REQ_TYPE_LOCK,
	SURFACE_REQ_TYPE_UNLOCK,
};

struct SFREQINFO {
	UINT32			_idx = 0;
	i3Surface *		_pSurface = nullptr;
	UINT32			_param1 = 0;
	UINT32			_param2 = 0;
	UINT32			_param3 = 0;

	DWORD			_dparam = 0;

	UINT32			_result = 0;
	SURFACE_REQ_TYPE	_type = SURFACE_REQ_TYPE_NONE;
};

static bool		sSurfaceReqInit = false;
static UINT32	s_SurfaceReqRead = 0;
static UINT32	s_SurfaceReqWrite = 0;
static SFREQINFO	s_SurfaceReq[SUR_REQ_BUF_COUNT];
static SFREQINFO	s_SurfaceAck[SUR_REQ_BUF_COUNT];

static i3Mutex2	sSurfaceReqMutex;

I3_CLASS_INSTANCE( i3Surface);

i3Surface::i3Surface(void)
{
	if( sSurfaceReqInit == false)
	{
		i3mem::FillZero( s_SurfaceReq, sizeof( s_SurfaceReq));
		i3mem::FillZero( s_SurfaceAck, sizeof( s_SurfaceAck));

		sSurfaceReqInit = true;
	}
}

i3Surface::~i3Surface(void)
{
	I3_SAFE_RELEASE( m_pClut);
	I3MEM_SAFE_FREE_POINTER( m_pPersistData );

#if defined( I3G_DX)
	NetworkDump_Free( m_pSurface, __FILE__, __LINE__);
	I3_SAFE_RELEASE_NODBG( m_pSurface);
#endif
}

UINT32 i3Surface::GetDescForThread( D3DSURFACE_DESC * pDesc)
{
	i3Monitor2 mon( &sSurfaceReqMutex);

	while( s_SurfaceReqWrite - s_SurfaceReqRead >= SUR_REQ_BUF_COUNT)
	{
		i3Timer::Sleep(1);
	}

	UINT32 idx = s_SurfaceReqWrite;

	while( s_SurfaceAck[ idx % SUR_REQ_BUF_COUNT]._type != SURFACE_REQ_TYPE_NONE)
	{
		i3Timer::Sleep(1);
	}
	
	SFREQINFO * pReq = &s_SurfaceReq[ idx % SUR_REQ_BUF_COUNT];

	pReq->_type = SURFACE_REQ_TYPE_GETDESC;
	pReq->_pSurface = this;
	I3_MUST_ADDREF( pReq->_pSurface);
	I3ASSERT( pReq->_pSurface->GetRefCount() > 1);
	pReq->_param1 = (UINT32) pDesc;
	pReq->_idx = idx;

	s_SurfaceReqWrite++;

	return idx;
}

UINT32 i3Surface::UpdateSurfaceForThread( RECT * pSrcRect, i3Surface * pDest, POINT * pDestP)
{
	i3Monitor2 mon( &sSurfaceReqMutex);

	while( s_SurfaceReqWrite - s_SurfaceReqRead >= SUR_REQ_BUF_COUNT)
	{
		i3Timer::Sleep(1);
	}

	UINT32 idx = s_SurfaceReqWrite;

	while( s_SurfaceAck[ idx % SUR_REQ_BUF_COUNT]._type != SURFACE_REQ_TYPE_NONE)
	{
		i3Timer::Sleep(1);
	}

	SFREQINFO * pReq = &s_SurfaceReq[ idx % SUR_REQ_BUF_COUNT];

	pReq->_type = SURFACE_REQ_TYPE_UPDATESF;
	pReq->_pSurface = this;
	I3_MUST_ADDREF( pReq->_pSurface);
	I3ASSERT( pReq->_pSurface->GetRefCount() > 1);
	pReq->_param1 = (UINT32) pSrcRect;
	pReq->_param2 = (UINT32) pDest;
	pReq->_param3 = (UINT32) pDestP;
	pReq->_idx = idx;

	s_SurfaceReqWrite++;

	return idx;
}

UINT32 i3Surface::LockSurfaceForThread( D3DLOCKED_RECT * pInfo, DWORD lock)
{
	i3Monitor2 mon( &sSurfaceReqMutex);

	while( s_SurfaceReqWrite - s_SurfaceReqRead >= SUR_REQ_BUF_COUNT)
	{
		i3Timer::Sleep(1);
	}

	UINT32 idx = s_SurfaceReqWrite;

	while( s_SurfaceAck[ idx % SUR_REQ_BUF_COUNT]._type != SURFACE_REQ_TYPE_NONE)
	{
		i3Timer::Sleep(1);
	}

	SFREQINFO * pReq = &s_SurfaceReq[ idx % SUR_REQ_BUF_COUNT];

	pReq->_type = SURFACE_REQ_TYPE_LOCK;
	pReq->_pSurface = this;
	I3_MUST_ADDREF( pReq->_pSurface);
	I3ASSERT( pReq->_pSurface->GetRefCount() > 1);
	pReq->_param1 = (UINT32) pInfo;
	pReq->_dparam = lock;
	pReq->_idx = idx;

	s_SurfaceReqWrite++;

	return idx;
}

UINT32 i3Surface::UnlockSurfaceForThread( void)
{
	i3Monitor2 mon( &sSurfaceReqMutex);

	while( s_SurfaceReqWrite - s_SurfaceReqRead >= SUR_REQ_BUF_COUNT)
	{
		i3Timer::Sleep(1);
	}

	UINT32 idx = s_SurfaceReqWrite;

	while( s_SurfaceAck[ idx % SUR_REQ_BUF_COUNT]._type != SURFACE_REQ_TYPE_NONE)
	{
		i3Timer::Sleep(1);
	}

	SFREQINFO * pReq = &s_SurfaceReq[ idx % SUR_REQ_BUF_COUNT];

	pReq->_type = SURFACE_REQ_TYPE_UNLOCK;
	pReq->_pSurface = this;
	I3_MUST_ADDREF( pReq->_pSurface);
	pReq->_idx = idx;
	I3ASSERT( pReq->_pSurface->GetRefCount() > 1);

	s_SurfaceReqWrite++;

	return idx;
}

UINT32 i3Surface::GetReqResult( UINT32 req)
{
	UINT32 result = 0;
	bool bWait = true;

	while( bWait)
	{
		for( UINT32 i = 0; i < SUR_REQ_BUF_COUNT; i++)
		{
			SFREQINFO * pAck = &s_SurfaceAck[i];

			if( pAck->_idx == req &&
				pAck->_type > SURFACE_REQ_TYPE_RESERVE )
			{
				result = pAck->_result;
				pAck->_type =  SURFACE_REQ_TYPE_NONE;

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

static i3Mutex2 sSurfaceUpdateMutex;

void i3Surface::UpdateForThread( void)
{
	i3Monitor2 mon( &sSurfaceUpdateMutex);

	UINT32 cnt = s_SurfaceReqWrite - s_SurfaceReqRead;	//s_SurfaceReqQue.size();

	for( UINT32 i = 0; i < cnt; i++)
	{
		UINT32 idx = s_SurfaceReqRead % SUR_REQ_BUF_COUNT;
		SFREQINFO * pReq = &s_SurfaceReq[ idx];	//s_SurfaceReqQue.front();

		I3ASSERT( pReq->_pSurface != nullptr);
		I3ASSERT( pReq->_type > SURFACE_REQ_TYPE_RESERVE);

		{
			switch( pReq->_type)
			{
			case SURFACE_REQ_TYPE_GETDESC :
				{
					pReq->_result = (UINT32) pReq->_pSurface->getSurface()->GetDesc( (D3DSURFACE_DESC*) pReq->_param1);
					CHECKRESULT(pReq->_result);
				}
				break;

			case SURFACE_REQ_TYPE_UPDATESF :
				{
					pReq->_result = (UINT32) g_pD3DDevice->UpdateSurface( pReq->_pSurface->getSurface(),
						(const RECT*) pReq->_param1, ((i3Surface*)pReq->_param2)->getSurface(), (const POINT*) pReq->_param3);
					CHECKRESULT(pReq->_result);
				}
				break;

			case SURFACE_REQ_TYPE_LOCK :
				{
					pReq->_result = (UINT32) pReq->_pSurface->getSurface()->LockRect( (D3DLOCKED_RECT*) pReq->_param1, NULL, (DWORD) pReq->_dparam);
					CHECKRESULT(pReq->_result);
				}
				break;
			case SURFACE_REQ_TYPE_UNLOCK :
				{
					pReq->_pSurface->getSurface()->UnlockRect();
				}
				break;
			default :
				I3PRINTLOG(I3LOG_FATAL,  "아직 등록하지 않은 타입!!!");
				break;
			}

			/*UINT32 write = findSurfaceAckInfoIdx();
			I3ASSERT( write != 0xFFFFFFFF);*/
			s_SurfaceAck[idx] = *pReq;

			I3_SAFE_RELEASE( pReq->_pSurface);
			pReq->_type = SURFACE_REQ_TYPE_NONE;
		}
		
		//s_SurfaceReqQue.pop();
		s_SurfaceReqRead++;
	}
}

void i3Surface::LostDeviceForThread( void)
{
	/*for( UINT32 i = 0; i < SUR_REQ_BUF_COUNT; i++)
	{
		SFREQINFO * pReq = &s_SurfaceReq[i];
		I3_SAFE_RELEASE( pReq->_pSurface);
		pReq->_type = SURFACE_REQ_TYPE_NONE;
	}

	::InterlockedExchange( (LONG*) &s_SurfaceReqWrite, 0);
	s_SurfaceAckWrite = 0;*/
}

void i3Surface::SetClut( i3Clut * pClut)
{
	I3_REF_CHANGE( m_pClut, pClut);
}

void i3Surface::SetPersistData( INT8 * pData)
{
	I3MEM_SAFE_FREE_POINTER( m_pPersistData );

	m_pPersistData = pData;
}

void i3Surface::setFormat( I3G_IMAGE_FORMAT fmt)
{
	m_Format = fmt;
}

INT32 i3Surface::GetDataSize(void)
{
	return i3Gfx::CalcRasterBufferSize( m_Width, m_Height, 1, m_Format);
}

bool i3Surface::Lock( I3G_LOCKMETHOD lock)
{
#if defined( I3G_DX)
	HRESULT hRv;
	D3DLOCKED_RECT info;

	I3ASSERT( m_pSurface != nullptr);

	//if( m_bInThread)
	//{
	//	UINT32 req = LockSurfaceForThread( &info, lock);
	//	hRv = (HRESULT) GetReqResult( req);	// blocked
	//}
	//else
	hRv = m_pSurface->LockRect(&info, NULL, lock);
	if (FAILED(hRv))
	{
		DXTRACE_ERR(L"", hRv);
		WCHAR wstrDesc[256];
		DXGetErrorDescription(hRv, wstrDesc, 256);
		I3TRACE(" -- %s", wstrDesc);
		return false;
	}

	m_pLockedBuf = (char *) info.pBits;
#else
	
	if( m_pLockedBuf == nullptr)
	{
		m_pLockedBuf = (char *) i3MemAlloc( GetDataSize());
		I3ASSERT( m_pLockedBuf != nullptr);
	}
#endif

	return true;
}

void i3Surface::Unlock(void)
{
#if defined( I3G_DX)
	//if( m_bInThread)
	//{
	//	UINT32 req = UnlockSurfaceForThread();
	//	GetReqResult( req);	// blocked;
	//}
	//else
		m_pSurface->UnlockRect();

	m_pLockedBuf = nullptr;
#endif
	m_pLockedBuf = nullptr;
}

static i3Mutex2 sSFLockMutex;

bool i3Surface::SafeLock( I3G_LOCKMETHOD lock)
{
	m_Mutex.Enter();

	return Lock( lock);
}

void i3Surface::SafeUnlock( void)
{
	Unlock();

	m_Mutex.Leave();
}

void i3Surface::Destroy(void)
{
	bool bLockable = !(getUsage() & I3G_USAGE_RENDERTARGET) && !(getFormat() & I3G_IMAGE_FORMAT_MASK_DEPTH) && (getUsage() & I3G_USAGE_RESTORE_MEM);

#if defined( I3G_DX)
	if( m_pSurface == nullptr)
		return;
#endif

	if( m_pLockedBuf != nullptr)
	{
		Unlock();
	}
	

	if( bLockable)
	{
		if( m_pPersistData == nullptr)
		{
			m_pPersistData = (INT8 *) i3MemAlloc( GetDataSize());
			I3ASSERT( m_pPersistData != nullptr);
		}

		//INT32 cnt = 0;
		bool bTemp = m_bInThread;

		m_bInThread = false;

		Lock();
		i3mem::Copy( m_pPersistData, getLockedBuffer(), GetDataSize());
		Unlock();

		m_bInThread = bTemp;
	}

#if defined( I3G_DX)
	NetworkDump_Free( m_pSurface, __FILE__, __LINE__);
	I3_SAFE_RELEASE_NODBG( m_pSurface);
#endif
}

void i3Surface::Revive(void)
{
	bool bLockable = !(getUsage() & I3G_USAGE_RENDERTARGET) && !(getFormat() & I3G_IMAGE_FORMAT_MASK_DEPTH);

	if( bLockable == false)
		return;

	if( m_pPersistData != nullptr)
	{
		bool bTemp = m_bInThread;
		m_bInThread = false;

		MakeRuntimeForm( false);
		I3MEM_SAFE_FREE( m_pPersistData);

		m_bInThread = bTemp;
	}
}

void i3Surface::MakeRuntimeForm( bool bMakeLinear)
{
	UINT32 Width, Height;
	char * pSrc, * pDest;

	Width = getWidth();
	Height = getHeight();

	INT32 errorCount = 0;

	I3ASSERT( m_pPersistData != nullptr);

	while( Lock() == false)
	{// 다른 쓰레드에서 사용중일 수 있다. 잠시 기둘..
		i3Timer::Sleep( 10);

		errorCount++;
		if( errorCount > 10)
		{
			I3PRINTLOG(I3LOG_FATAL,  "lock fail!!!!!");
		}
	}

	pSrc = (char *) getPersistData();
	pDest = getLockedBuffer();

	{

#if defined( I3G_DX)
		if( !g_pRenderContext->IsCompressedTexture())
		{
			i3Image_Convert( m_PersistFormat, pSrc, Width, Height, 1, getClut(), 
					I3G_IMAGE_FORMAT_ARGB_1555, pDest, 4, getClut());
		}
		else
#endif
		{
			if( getPersistFormat() == getFormat())
			{
				i3mem::Copy( pDest, pSrc, m_Pitch*m_Height);
			}
			else
			{
				i3Image_Convert( m_PersistFormat, pSrc, Width, Height, 1, getClut(), 
					m_Format, pDest, 4, getClut());
			}
		}
	}

	Unlock();
}

void i3Surface::MakePersistForm(void)
{
	if( getPersistData() == nullptr)
	{
		Lock();

		m_pPersistData = (INT8*)i3MemAlloc( m_Pitch*m_Height);
		I3ASSERT( m_pPersistData != nullptr);

		i3mem::Copy( m_pPersistData, getLockedBuffer(), m_Pitch*m_Height);
		setPersistFormat( getFormat());
		
		Unlock();
	}
}

void i3Surface::DeletePersistData(void)
{
	I3MEM_SAFE_FREE( m_pPersistData);	
}

#if defined( I3G_DX)
void i3Surface::SetSurface( IDirect3DSurface9 * pObj, I3G_IMAGE_FORMAT persistFormat, UINT32 usage)
{
	D3DSURFACE_DESC desc;
	HRESULT hRv;
	
	I3_SAFE_RELEASE_NODBG( m_pSurface);
	m_pSurface = pObj;

	hRv = m_pSurface->GetDesc( &desc);

	if( FAILED( hRv))
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not retreive information about surface.");
		return;
	}

	m_Width = static_cast<UINT16>(desc.Width);
	m_Height = static_cast<UINT16>(desc.Height);
	setFormat(i3DXUT_GetI3ImageFormat( desc.Format));
	m_PersistFormat			=	persistFormat;
	m_Usage					=	i3DXUT_GetI3Usage( desc.Usage) | (usage & I3G_USAGE_RESTORE_MEM);
	m_BitsPerPixel			= static_cast<UINT8>(i3Gfx::GetImageFormatBitCount( m_Format));
	I3ASSERT( m_BitsPerPixel != 0xFF);
	m_Stride				= (m_BitsPerPixel + 7) >> 3;
	m_Pitch					= (m_Width * m_BitsPerPixel + 7) >> 3;
}

#if defined( I3_DEBUG)
void i3Surface::SetSurface( IDirect3DSurface9 * pObj, I3G_IMAGE_FORMAT persistFormat, UINT32 usage, const char * pszFile, INT32 line)
{
	NetworkDump_Free( m_pSurface, __FILE__, __LINE__);
	SetSurface( pObj, persistFormat, usage);
	NetworkDump_Malloc( m_pSurface, i3Gfx::CalcRasterBufferSize( m_Width, m_Height, 1, m_Format), pszFile, line);
}
#endif

void i3Surface::CopySurfaceTo( INT32 sx, INT32 sy, INT32 cx, INT32 cy, i3Surface * pDest, INT32 dx, INT32 dy)
{
	RECT srcRC;
	POINT destPT;

	I3ASSERT( g_pD3DDevice != nullptr);

	srcRC.left = sx;
	srcRC.top = sy;
	srcRC.right = sx + cx;
	srcRC.bottom = sy + cy;

	destPT.x = dx;
	destPT.y = dy;

	HRESULT Rv;
	
	if( m_bInThread)
	{
		UINT32 req = UpdateSurfaceForThread( &srcRC, pDest, &destPT);

		Rv = (HRESULT) GetReqResult( req);	// blocked
	}
	else
		Rv = g_pD3DDevice->UpdateSurface( m_pSurface, &srcRC, pDest->getSurface(), &destPT);

	CHECKRESULT( Rv);
}
#endif
