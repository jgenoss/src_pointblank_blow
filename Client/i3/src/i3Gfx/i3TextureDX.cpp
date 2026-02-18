#include "i3GfxType.h"
#include "i3RenderContext.h"
#include "i3GfxGlobalVariable.h"
#include "i3Base/itl/container_util.h"
#include "i3Base/string/ext/extract_fileext.h"

#include <queue>

#if defined( I3G_DX)
#include "i3GfxUtil.h"
#include "i3TextureDX.h"
#include "i3GfxUtilDX.h"
#include "i3GfxGlobalVariableDX.h"
#include "i3ImageFile.h"
#include "i3ImageUtil.h"
#include "i3Input/Dxerr.h"

#define TX_REQ_BUF_COUNT 128

enum TX_REQ_TYPE {
	TX_REQ_TYPE_NONE = 0,
	TX_REQ_TYPE_RESERVE,
	TX_REQ_TYPE_CREATE,
	TX_REQ_TYPE_LOCK,
	TX_REQ_TYPE_UNLOCK,
	TX_REQ_TYPE_CREATESURFACE,
	TX_REQ_TYPE_GETSURFACE,
};

struct TEXDXREQINFO {
	UINT32			_idx = 0;
	i3TextureDX *	_pTex = nullptr;
	UINT32			_param1 = 0;
	UINT32			_param2 = 0;
	UINT32			_param3 = 0;
	UINT32			_param4 = 0;
	UINT32			_param5 = 0;
	DWORD			_flag = 0;
	UINT32			_result = 0;
	TX_REQ_TYPE		_type = TX_REQ_TYPE_NONE;
};

static bool		sTexReqInit = false;
static UINT32	s_TexReqRead = 0;
static UINT32	s_TexReqWrite = 0;
static TEXDXREQINFO	s_TxReq[TX_REQ_BUF_COUNT];
static TEXDXREQINFO	s_TxAck[TX_REQ_BUF_COUNT];

static i3Mutex2 sTxReqMutex;

I3_CLASS_INSTANCE( i3TextureDX);

i3TextureDX::i3TextureDX(void)
{
	memset(&m_LockRect, 0 , sizeof(D3DLOCKED_RECT));
	
	if( sTexReqInit == false)
	{
		i3mem::FillZero( s_TxReq, sizeof( s_TxReq));
		i3mem::FillZero( s_TxAck, sizeof( s_TxAck));
		sTexReqInit = true;
	}
}

i3TextureDX::~i3TextureDX(void)
{
	i3::cu::for_each_safe_release_clear(m_SurfaceList, &i3Surface::Release);
	
	if( m_pTexture != nullptr)
	{
		NetworkDump_Free( m_pTexture, __FILE__, __LINE__);
		m_pTexture->Release();
		m_pTexture = nullptr;
	}
}

UINT32 i3TextureDX::CreateDXBuffer( UINT32 width, UINT32 height, UINT32 Levels, DWORD rUsage, D3DFORMAT rFormat, D3DPOOL pool)
{
	i3Monitor2 mon( &sTxReqMutex);

	while( s_TexReqWrite - s_TexReqRead >= TX_REQ_BUF_COUNT)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	UINT32 idx = s_TexReqWrite;//sGetTexReqInfoIdx();

	while( s_TxAck[ idx % TX_REQ_BUF_COUNT]._type != TX_REQ_TYPE_NONE)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	TEXDXREQINFO * pReq = &s_TxReq[idx % TX_REQ_BUF_COUNT];

	pReq->_type = TX_REQ_TYPE_CREATE;
	pReq->_pTex = this;
	I3_MUST_ADDREF( pReq->_pTex);
	I3ASSERT( pReq->_pTex->GetRefCount() > 1);
	pReq->_param1 = width;
	pReq->_param2 = height;
	pReq->_param3 = Levels;
	pReq->_flag = rUsage;
	pReq->_param4 = (UINT32) rFormat;
	pReq->_param5 = (UINT32) pool;
	pReq->_idx = idx;

	//s_TxReqQue.push( pReq);
	s_TexReqWrite++;

	return idx;
}

UINT32 i3TextureDX::LockDXBuffer( UINT32 level, D3DLOCKED_RECT * pLockRect, RECT * pRect, I3G_LOCKMETHOD lock)
{
	i3Monitor2 mon( &sTxReqMutex);

	while( s_TexReqWrite - s_TexReqRead >= TX_REQ_BUF_COUNT)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	UINT32 idx = s_TexReqWrite;	//sGetTexReqInfoIdx();

	while( s_TxAck[ idx % TX_REQ_BUF_COUNT]._type != TX_REQ_TYPE_NONE)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	TEXDXREQINFO * pReq = &s_TxReq[idx % TX_REQ_BUF_COUNT];

	pReq->_type = TX_REQ_TYPE_LOCK;
	pReq->_pTex = this;
	I3_MUST_ADDREF( pReq->_pTex);
	I3ASSERT( pReq->_pTex->GetRefCount() > 1);
	pReq->_param1 = level;
	pReq->_param2 = (UINT32) pLockRect;
	pReq->_param3 = (UINT32) pRect;
	pReq->_param4 = (UINT32) lock;
	pReq->_idx = idx;

	//s_TxReqQue.push( pReq);
	s_TexReqWrite++;

	return idx;
}

UINT32 i3TextureDX::UnlockDXBuffer( UINT32 level)
{
	i3Monitor2 mon( &sTxReqMutex);

	while( s_TexReqWrite - s_TexReqRead >= TX_REQ_BUF_COUNT)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	UINT32 idx = s_TexReqWrite;	//sGetTexReqInfoIdx();

	while( s_TxAck[ idx % TX_REQ_BUF_COUNT]._type != TX_REQ_TYPE_NONE)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	TEXDXREQINFO * pReq = &s_TxReq[idx % TX_REQ_BUF_COUNT];

	pReq->_type = TX_REQ_TYPE_UNLOCK;
	pReq->_pTex = this;
	I3_MUST_ADDREF( pReq->_pTex);
	I3ASSERT( pReq->_pTex->GetRefCount() > 1);
	pReq->_param1 = level;
	pReq->_idx = idx;

	//s_TxReqQue.push( pReq);
	s_TexReqWrite++;
	
	return idx;
}

UINT32 i3TextureDX::CreateDXSurface( UINT32 width, UINT32 height, D3DFORMAT rFormat)
{
	i3Monitor2 mon( &sTxReqMutex);

	while( s_TexReqWrite - s_TexReqRead >= TX_REQ_BUF_COUNT)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	UINT32 idx = s_TexReqWrite;	//sGetTexReqInfoIdx();

	while( s_TxAck[ idx % TX_REQ_BUF_COUNT]._type != TX_REQ_TYPE_NONE)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	TEXDXREQINFO * pReq = &s_TxReq[idx % TX_REQ_BUF_COUNT];

	pReq->_type = TX_REQ_TYPE_UNLOCK;
	pReq->_pTex = this;
	I3_MUST_ADDREF( pReq->_pTex);
	I3ASSERT( pReq->_pTex->GetRefCount() > 1);
	pReq->_param1 = width;
	pReq->_param2 = height;
	pReq->_param3 = rFormat;
	pReq->_idx = idx;

	//s_TxReqQue.push( pReq);
	s_TexReqWrite++;

	return idx;
}

UINT32 i3TextureDX::GetDXSurface( UINT32 level)
{
	i3Monitor2 mon( &sTxReqMutex);

	while( s_TexReqWrite - s_TexReqRead >= TX_REQ_BUF_COUNT)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	UINT32 idx = s_TexReqWrite;//sGetTexReqInfoIdx();

	while( s_TxAck[ idx % TX_REQ_BUF_COUNT]._type != TX_REQ_TYPE_NONE)
	{	// 메인쓰레드에서 처리할때까지 기다리자.
		i3Timer::Sleep(1);
	}

	TEXDXREQINFO * pReq = &s_TxReq[idx % TX_REQ_BUF_COUNT];

	pReq->_type = TX_REQ_TYPE_GETSURFACE;
	pReq->_pTex = this;
	I3_MUST_ADDREF( pReq->_pTex);
	I3ASSERT( pReq->_pTex->GetRefCount() > 1);
	pReq->_param1 = level;
	pReq->_idx = idx;
	
	//s_TxReqQue.push( pReq);
	s_TexReqWrite++;

	return idx;
}

UINT32 i3TextureDX::GetReqResult( UINT32 req)
{
	UINT32 result = 0;

	bool bWait = true;

	while( bWait)
	{
		for( UINT32 i = 0; i < TX_REQ_BUF_COUNT; i++)
		{
			TEXDXREQINFO * pAck = &s_TxAck[i];

			if( pAck->_idx == req &&
				pAck->_type > TX_REQ_TYPE_RESERVE )
			{
				result = pAck->_result;
				pAck->_type = TX_REQ_TYPE_NONE;

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

static i3Mutex2 sTextureUpdateMutex;

void i3TextureDX::UpdateForThread( void)
{
	i3Monitor2 mon( &sTextureUpdateMutex);

	UINT32 cnt = s_TexReqWrite - s_TexReqRead;	//s_TxReqQue.size();

	for( UINT32 i = 0; i < cnt; i++)
	{
		UINT32 idx = s_TexReqRead % TX_REQ_BUF_COUNT;
		TEXDXREQINFO * pReq = &s_TxReq[ idx];	//s_TxReqQue.front();

		I3ASSERT( pReq->_pTex != nullptr);
		I3ASSERT( pReq->_type > TX_REQ_TYPE_RESERVE);

		{
			switch( pReq->_type)
			{
			case TX_REQ_TYPE_CREATE :
				{
					HRESULT rv = D3DXCreateTexture( g_pD3DDevice, pReq->_param1, pReq->_param2,
						pReq->_param3, pReq->_flag, (D3DFORMAT) pReq->_param4, (D3DPOOL) pReq->_param5, (IDirect3DTexture9**)&pReq->_result);
					CHECKRESULT(rv);
				
					NetworkDump_Malloc( (IDirect3DTexture9*)pReq->_result, sizeof(IDirect3DTexture9), __FILE__, __LINE__);
				}
				break;

			case TX_REQ_TYPE_LOCK :
				{
					pReq->_result = pReq->_pTex->GetDXImage()->LockRect( pReq->_param1, (D3DLOCKED_RECT*)pReq->_param2, (const RECT*) pReq->_param3, pReq->_param4);
					CHECKRESULT(pReq->_result);
				}
				break;

			case TX_REQ_TYPE_UNLOCK :
				{
					pReq->_result = pReq->_pTex->GetDXImage()->UnlockRect( pReq->_param1);
					CHECKRESULT(pReq->_result);
				}
				break;

			case TX_REQ_TYPE_CREATESURFACE :
				{
					HRESULT rv = g_pD3DDevice->CreateDepthStencilSurface( pReq->_param1, pReq->_param2, (D3DFORMAT) pReq->_param3, D3DMULTISAMPLE_NONE,
						0, FALSE, (IDirect3DSurface9**)&pReq->_result, NULL);
					CHECKRESULT(rv);
				}
				break;

			case TX_REQ_TYPE_GETSURFACE :
				{
					pReq->_pTex->GetDXImage()->GetSurfaceLevel( pReq->_param1, (IDirect3DSurface9**)&pReq->_result);
				}
				break;
			}

			//UINT32 write = findTexAckInfoIdx();
			s_TxAck[idx] = *pReq;

			I3_SAFE_RELEASE( pReq->_pTex);
			pReq->_type = TX_REQ_TYPE_NONE;
		}
		
		//s_TxReqQue.pop();
		s_TexReqRead++;
	}
}

void i3TextureDX::LostDeviceForThread( void)
{
	/*for( UINT32 i = 0; i < TX_REQ_BUF_COUNT; i++)
	{
		TEXDXREQINFO * pReq = &s_TxReq[i];
		I3_SAFE_RELEASE( pReq->_pTex);
		pReq->_type = TX_REQ_TYPE_NONE;
	}

	::InterlockedExchange( (LONG*) &s_TexReqWrite, (LONG) 0);

	s_TexAckWrite = 0;*/
}

void i3TextureDX::SetDXTexture( IDirect3DTexture9 * pTexture)
{
	HRESULT Rv;
	INT32 i;
	i3Surface * pSurface;
	IDirect3DSurface9 * pDXSurface;

	if( m_pTexture != nullptr)
	{
		NetworkDump_Free( m_pTexture, __FILE__, __LINE__);
		m_pTexture->Release();
	}
	m_pTexture = pTexture;

	I3ASSERT( m_pTexture->GetLevelCount() > 0);

	// Surface의 개수를 맞춘다.
	{
		INT32 gap = m_pTexture->GetLevelCount() - getSurfaceCount();

		if( gap > 0)
		{
			for( i = 0; i < gap; i++)
			{
				pSurface = i3Surface::new_object();
				pSurface->setThreadMode( getThreadMode());

				m_SurfaceList.push_back( pSurface);
			}
		}
		else if( gap < 0)
		{
			for( i = 0; i < -gap; i++)
			{
				pSurface = getSurface( m_pTexture->GetLevelCount());

				I3_MUST_RELEASE(pSurface);

				m_SurfaceList.erase(m_SurfaceList.begin() + m_pTexture->GetLevelCount());
			}
		}
	}

	for( i = 0; i < (INT32) m_pTexture->GetLevelCount(); i++)
	{
		pSurface = getSurface( i);

		Rv = m_pTexture->GetSurfaceLevel( i, &pDXSurface);

		#if defined( I3_DEBUG)
		CHECKRESULT( Rv);
		#endif

		I3ASSERT( pDXSurface != nullptr);

#if defined( I3_DEBUG)
		if( GetNameString().length() > 0)
			pSurface->SetSurface( pDXSurface, m_PersistFormat, GetUsageFlag(), GetName(), 0);
		else
			pSurface->SetSurface( pDXSurface, m_PersistFormat, GetUsageFlag(), __FILE__, __LINE__);
#else
		pSurface->SetSurface( pDXSurface, m_PersistFormat, GetUsageFlag());
#endif
	}

	// setting up actual image informations.
	pSurface = getSurface( 0);

	m_Width			= pSurface->getWidth();
	m_Height		= pSurface->getHeight();
	_setFormat(		 pSurface->getFormat());

	if( m_PersistFormat == I3G_IMAGE_FORMAT_NONE)
	{
		m_PersistFormat = pSurface->getFormat();
	}
}



#if defined( I3_DEBUG)
static bool	isPowOf2( UINT32 sz)
{
	UINT32 i, mask = 1;

	for( i = 0; i < 32; i++)
	{
		if( mask == sz)
			return true;

		mask = mask << 1;
	}

	return false;
}
#endif

bool i3TextureDX::Create( UINT32 width, UINT32 height, UINT32 Levels, I3G_IMAGE_FORMAT format, I3G_USAGE Usage, D3DPOOL ppool)
{
	HRESULT Rv;
	IDirect3DTexture9 * pTex;
	DWORD rUsage;
	D3DPOOL pool = ppool;
	D3DFORMAT rFormat;

#if defined( I3_DEBUG)
	I3ASSERT( isPowOf2( width));
	I3ASSERT( isPowOf2( height));
#endif

	i3Texture::Create( width, height, Levels, format, Usage);

	rUsage = i3DXUT_GetNativeUsage( Usage);

	// Runtime Format을 갱신한다.
	_setFormat( format);

	rFormat = i3DXUT_GetNativeImageFormat( format);

	if( format & I3G_IMAGE_FORMAT_MASK_DEPTH)
		rUsage |= D3DUSAGE_DEPTHSTENCIL;

	if( !(rUsage & D3DUSAGE_DEPTHSTENCIL))
	{
		if( format & I3G_IMAGE_FORMAT_MASK_INDEXED)
		{
			if( i3RenderContext::GetCaps()->IsEnableAlphaCLUT() == false)
			{
				format = I3G_IMAGE_FORMAT_BGRA_8888;
			}
		}

		if( rUsage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL))
		{
			pool = D3DPOOL_DEFAULT;
		}

		if( rUsage & D3DUSAGE_WRITEONLY)
		{
			pool = D3DPOOL_DEFAULT;
		}
		else
		{
			if( rUsage & D3DUSAGE_DYNAMIC)
				pool = D3DPOOL_SYSTEMMEM;
		}

		if( Usage & I3G_USAGE_FORCE_VRAM)
		{
			pool = D3DPOOL_DEFAULT;
		}
		else if( Usage & I3G_USAGE_FORCE_SYSRAM)
		{
			pool = D3DPOOL_SYSTEMMEM;
		}

		Rv = D3DXCreateTexture( g_pD3DDevice, width, height, Levels, rUsage, rFormat, pool, &pTex);
	
		NetworkDump_Malloc( pTex, sizeof(IDirect3DTexture9), __FILE__, __LINE__);

		if( Rv == D3DERR_OUTOFVIDEOMEMORY)
		{
			I3PRINTLOG(I3LOG_NOTICE, "D3DXCreateTexture D3DERR_OUTOFVIDEOMEMORY");
			// Video Memory가 고갈되었다면, 기존에 Loading된 Texture, VertexBuffer 등을 VRAM에서 내린 후
			// 다시 시도한다.
			g_pD3DDevice->EvictManagedResources();

			Rv = D3DXCreateTexture( g_pD3DDevice, width, height, Levels, rUsage, rFormat, pool, &pTex);
			NetworkDump_Malloc( pTex, sizeof(IDirect3DTexture9), __FILE__, __LINE__);
		}

		if( FAILED( Rv))
		{
			#if defined( I3_DEBUG)
				DXTRACE_ERR(L"i3TextureDX::Create() - Could not create IDirect3DTexture9 object.", Rv);
			#endif

			switch( Rv)
			{
			case D3DERR_OUTOFVIDEOMEMORY :	
				if( m_CallBackExternalOutOfMemory)	m_CallBackExternalOutOfMemory();
				I3ERRLOG( i3Error::_OUTOFVIDEOMEMORY);	break;
			default :
				I3ERRLOG( i3Error::_UNSUPPORTED_GPU);	break;
			}

			CHECKRESULT(Rv);

			return false;
		}

		SetDXTexture( pTex);
	}
	else
	{
		IDirect3DSurface9 * pDXSurface;
		i3Surface * pSurface;

		{
			Rv = g_pD3DDevice->CreateDepthStencilSurface( width, height, rFormat, D3DMULTISAMPLE_NONE,
					0, FALSE, &pDXSurface, NULL);
		}

		if( Rv == D3DERR_OUTOFVIDEOMEMORY)
		{
			I3PRINTLOG(I3LOG_NOTICE, "CreateDepthStencilSurface D3DERR_OUTOFVIDEOMEMORY");
			// Video Memory가 고갈되었다면, 기존에 Loading된 Texture, VertexBuffer 등을 VRAM에서 내린 후
			// 다시 시도한다.
			g_pD3DDevice->EvictManagedResources();

			Rv = g_pD3DDevice->CreateDepthStencilSurface( width, height, rFormat, D3DMULTISAMPLE_NONE,
				0, FALSE, &pDXSurface, NULL);
		}

		if( FAILED( Rv))
		{
#if defined( I3_DEBUG)
			DXTRACE_ERR(L"i3TextureDX::Create() - Could not create IDirect3DTexture9 object.", Rv);
#endif

			switch( Rv)
			{
			case D3DERR_OUTOFVIDEOMEMORY :	
				if( m_CallBackExternalOutOfMemory)	m_CallBackExternalOutOfMemory();
				I3ERRLOG( i3Error::_OUTOFVIDEOMEMORY);	break;
			default :
				I3ERRLOG( i3Error::_UNSUPPORTED_GPU);	break;
			}

			CHECKRESULT(Rv);

			return false;
		}

		if( getSurfaceCount() > 0)
		{
			pSurface = getSurface( 0);
		}
		else
		{
			pSurface = i3Surface::new_object();
			pSurface->setThreadMode( getThreadMode());
			m_SurfaceList.push_back( pSurface);
		}

#if defined( I3_DEBUG)
		if( GetNameString().length() > 0)
			pSurface->SetSurface( pDXSurface, m_PersistFormat, GetUsageFlag(), GetName(), 0);
		else
			pSurface->SetSurface( pDXSurface, m_PersistFormat, GetUsageFlag(), __FILE__, __LINE__);
#else
		pSurface->SetSurface( pDXSurface, m_PersistFormat, GetUsageFlag());
#endif
	}

	return true;
}

bool i3TextureDX::Create( UINT32 width, UINT32 height, UINT32 Levels, I3G_IMAGE_FORMAT format, I3G_USAGE Usage)
{
	HRESULT Rv;
	IDirect3DTexture9 * pTex;
	DWORD rUsage;
	D3DPOOL pool = D3DPOOL_MANAGED;
	D3DFORMAT rFormat;

	i3Texture::Create( width, height, Levels, format, Usage);

	rUsage = i3DXUT_GetNativeUsage( Usage);

	// Runtime Format을 갱신한다.
	_setFormat( format);

	rFormat = i3DXUT_GetNativeImageFormat( format);

	if( format & I3G_IMAGE_FORMAT_MASK_DEPTH)
		rUsage |= D3DUSAGE_DEPTHSTENCIL;

	if( !(rUsage & D3DUSAGE_DEPTHSTENCIL))
	{
		if( format & I3G_IMAGE_FORMAT_MASK_INDEXED)
		{
			if( i3RenderContext::GetCaps()->IsEnableAlphaCLUT() == false)
			{
				format = I3G_IMAGE_FORMAT_BGRA_8888;
			}
		}

		if( rUsage & (D3DUSAGE_RENDERTARGET | D3DUSAGE_DEPTHSTENCIL))
		{
			pool = D3DPOOL_DEFAULT;
		}

		if( rUsage & D3DUSAGE_WRITEONLY)
		{
			pool = D3DPOOL_DEFAULT;
		}
		else
		{
			if( rUsage & D3DUSAGE_DYNAMIC)
				pool = D3DPOOL_SYSTEMMEM;
		}

		if( Usage & I3G_USAGE_FORCE_VRAM)
		{
			pool = D3DPOOL_DEFAULT;
		}
		else if( Usage & I3G_USAGE_FORCE_SYSRAM)
		{
			pool = D3DPOOL_SYSTEMMEM;
		}

		Rv = D3DXCreateTexture( g_pD3DDevice, width, height, Levels, rUsage, rFormat, pool, &pTex);
	
		NetworkDump_Malloc( pTex, sizeof(IDirect3DTexture9), __FILE__, __LINE__);

		if( Rv == D3DERR_OUTOFVIDEOMEMORY)
		{
			I3PRINTLOG(I3LOG_NOTICE, "D3DXCreateTexture D3DERR_OUTOFVIDEOMEMORY");
			// Video Memory가 고갈되었다면, 기존에 Loading된 Texture, VertexBuffer 등을 VRAM에서 내린 후
			// 다시 시도한다.
			g_pD3DDevice->EvictManagedResources();

			Rv = D3DXCreateTexture( g_pD3DDevice, width, height, Levels, rUsage, rFormat, pool, &pTex);
		
			NetworkDump_Malloc( pTex, sizeof(IDirect3DTexture9), __FILE__, __LINE__);
		}

		if( FAILED( Rv))
		{
			#if defined( I3_DEBUG)
				DXTRACE_ERR(L"i3TextureDX::Create() - Could not create IDirect3DTexture9 object.", Rv);
			#endif

				switch( Rv)
				{
				case D3DERR_OUTOFVIDEOMEMORY :	
					if( m_CallBackExternalOutOfMemory)	m_CallBackExternalOutOfMemory();
					I3ERRLOG( i3Error::_OUTOFVIDEOMEMORY);	break;
				default :
					I3ERRLOG( i3Error::_UNSUPPORTED_GPU);	break;
				}

				CHECKRESULT( Rv);
				return false;
		}

		SetDXTexture( pTex);
	}
	else
	{
		IDirect3DSurface9 * pDXSurface;
		i3Surface * pSurface;

		{
			Rv = g_pD3DDevice->CreateDepthStencilSurface( width, height, rFormat, D3DMULTISAMPLE_NONE,
					0, FALSE, &pDXSurface, NULL);
		}

		if( Rv == D3DERR_OUTOFVIDEOMEMORY)
		{
			I3PRINTLOG(I3LOG_NOTICE, "CreateDepthStencilSurface D3DERR_OUTOFVIDEOMEMORY");
			// Video Memory가 고갈되었다면, 기존에 Loading된 Texture, VertexBuffer 등을 VRAM에서 내린 후
			// 다시 시도한다.
			g_pD3DDevice->EvictManagedResources();

			Rv = g_pD3DDevice->CreateDepthStencilSurface( width, height, rFormat, D3DMULTISAMPLE_NONE,
				0, FALSE, &pDXSurface, NULL);
		}

		if( FAILED( Rv))
		{
			#if defined( I3_DEBUG)
				DXTRACE_ERR(L"i3TextureDX::Create() - Could not create IDirect3DTexture9 object.", Rv);
			#endif

			switch( Rv)
			{
				case D3DERR_OUTOFVIDEOMEMORY :	
					if( m_CallBackExternalOutOfMemory)	m_CallBackExternalOutOfMemory();
					I3ERRLOG( i3Error::_OUTOFVIDEOMEMORY);
					break;
				default :						I3ERRLOG( i3Error::_UNSUPPORTED_GPU);	break;
			}

			CHECKRESULT(Rv);

			return false;
		}

		if( getSurfaceCount() > 0)
		{
			pSurface = getSurface( 0);
		}
		else
		{
			pSurface = i3Surface::new_object();
			pSurface->setThreadMode( getThreadMode());
			m_SurfaceList.push_back( pSurface);
		}

#if defined( I3_DEBUG)
		if( GetNameString().length() > 0)
			pSurface->SetSurface( pDXSurface, m_PersistFormat, GetUsageFlag(), GetName(), 0);
		else
			pSurface->SetSurface( pDXSurface, m_PersistFormat, GetUsageFlag(), __FILE__, __LINE__);
#else
		pSurface->SetSurface( pDXSurface, m_PersistFormat, GetUsageFlag());
#endif
	}

	return true;
}

bool i3TextureDX::Create( const char * pszPath, UINT32 /*Levels*/)
{
	i3ImageFile file;

	return file.Load(pszPath, this);
}

bool i3TextureDX::Destroy( bool bLostDevice)
{
	INT32 i;
	bool bVolatile = false;
	//bool bReleaseTex = false;

	// Surface를 제거하지 않고 남겨 두어야 하는 경우 
	// - Persist Data가 없으며, 내용이 Volatile이 아닌 경우
	//bLeaveSurface	= (getRestoreType() == I3G_RESTORE_MEM) && bLostDevice;
	bVolatile		= (GetUsageFlag() & I3G_USAGE_RENDERTARGET) || (GetFormat() & I3G_IMAGE_FORMAT_MASK_DEPTH);

	bool bTemp = getThreadMode();
	setThreadMode( false);

	{
		for( i = 0; i < getSurfaceCount(); i++)
		{
			if( m_pLockedBuf[ i] != nullptr)
			{
				Unlock( i);
			}

			i3Surface * pSurface = getSurface( i);

			pSurface->setThreadMode( false);

			pSurface->Destroy();

			pSurface->setThreadMode( bTemp);
		}
	}

	// DX_Texture가 제거되는 경우
	// 1. 모든 경우 : Lost Device (FullScreen <-> Window Screen)
	// 2. Render Target : Resize가 되는 경우
	if( bLostDevice || bVolatile)
	{
		NetworkDump_Free( m_pTexture, __FILE__, __LINE__);
		I3_SAFE_RELEASE_NODBG( m_pTexture);
	}

	setThreadMode( bTemp);

	return i3GfxResource::Destroy( bLostDevice);
}

bool	i3TextureDX::isDestroyed( void)
{
	if( m_pTexture != nullptr) return false;
	
	return true;
}


bool i3TextureDX::Revive( i3RenderContext * pCtx)
{
	bool Rv = false;
	INT32 i;
	bool bVolatile;

	bool bTemp = getThreadMode();
	setThreadMode( false);

	if( m_pTexture != nullptr)
	{
		IDirect3DSurface9 * pDXSurface = nullptr;		

		for( i = 0; i < getSurfaceCount(); i++)
		{
			i3Surface * pSurface = getSurface( i);

			pSurface->setThreadMode( false);

			//if( pSurface->getSurface() == nullptr)
			{
				m_pTexture->GetSurfaceLevel( i, &pDXSurface);
				
#if defined( I3_DEBUG)
				if( GetNameString().length() > 0)
					pSurface->SetSurface( pDXSurface, m_PersistFormat, GetUsageFlag(), GetName(), 0);
				else
					pSurface->SetSurface( pDXSurface, m_PersistFormat, GetUsageFlag(), __FILE__, __LINE__);
#else
				pSurface->SetSurface( pDXSurface, m_PersistFormat, GetUsageFlag());
#endif
			}

			pSurface->setThreadMode( bTemp);
		}

		setThreadMode( bTemp);
		return true;
	}

	if( getRestoreType() != I3G_RESTORE_FILE)
	{
		for( i = 0; i < getSurfaceCount(); i++)
		{
			getSurface( i)->setThreadMode( false);
		}

		Rv = Create( GetWidth(), GetHeight(), m_LevelCount, m_PersistFormat, GetUsageFlag());

		if( Rv == true)
		{
			bVolatile		= (GetUsageFlag() & I3G_USAGE_RENDERTARGET) || (GetFormat() & I3G_IMAGE_FORMAT_MASK_DEPTH);

			for( i = 0; i < getSurfaceCount(); i++)
			{
				getSurface( i)->Revive();
				getSurface( i)->setThreadMode( bTemp);
			}

			SetLoaded(true);
		}
	}
	else
	{
		i3ImageFile file;

		//I3TRACE( "[REVIVE FILE] %s (%d)\n", getResourceLoadPath(), getResourceLoadOffset());

		if( file.Load( getResourceLoadPath(), this, getResourceLoadOffset()) == false)
		{
			setThreadMode( bTemp);
			return false;
		}
	}

	setThreadMode( bTemp);
	return Rv;
}

char * i3TextureDX::Lock( INT32 level, I3G_LOCKMETHOD lockmode, RECT * pRect)
{
	HRESULT Rv;

	if( m_pTexture == nullptr)
		return nullptr;

	I3_BOUNDCHK( level, MAX_MIPMAP_LEVEL);
	m_LockCount[ level]++;
	if( m_LockCount[ level] >= 2)
		return m_pLockedBuf[ level];

	D3DLOCKED_RECT lockRect;
	//if( getThreadMode())
	//{
	//	UINT32 req = LockDXBuffer( level, &lockRect, pRect, lockmode);

	//	Rv = GetReqResult( req);	// blocked
	//}
	//else
		Rv = m_pTexture->LockRect( level, &lockRect, pRect, lockmode);

	if( FAILED( Rv))
	{
		CHECKRESULT( Rv);
		return nullptr;
	}
	
	m_LockedLevel = static_cast<INT8>(level);
	m_LockMethod = lockmode;
	m_pLockedBuf[ level] = (char *) (char *) lockRect.pBits;

	return m_pLockedBuf[ level];
}

void i3TextureDX::Unlock( INT32 level)
{
	HRESULT Rv;

	I3ASSERT( m_pTexture != nullptr);

	I3_BOUNDCHK( level, MAX_MIPMAP_LEVEL);

	m_LockCount[ level]--;
	if( m_LockCount[ level] > 0)
		return;

	//if( getThreadMode())
	//{
	//	UINT32 req = UnlockDXBuffer( level);
	//	Rv = GetReqResult( req);	// blocked
	//}
	//else
		Rv = m_pTexture->UnlockRect( level);
	if( FAILED( Rv))
	{
		//I3PRINTLOG(I3LOG_FATAL,  "Could not unlock the image %s.", DXGetErrorDescription(Rv));
		CHECKRESULT( Rv);
		return;
	}

	
	m_LockedLevel = -1;
	m_pLockedBuf[ level] = nullptr;
}

char * i3TextureDX::SafeLock( INT32 level, I3G_LOCKMETHOD lockmode, RECT * pRect)
{
	m_Mutex.Enter();

	return Lock( level, lockmode, pRect);
}

bool i3TextureDX::SafeUnlock( INT32 level)
{
	Unlock( level);

	m_Mutex.Leave();

	return true;
}


#if defined( I3_DEBUG)
void i3TextureDX::OnQueryStat( I3G_RES_STAT * pInfo)
{
	INT32 i;

	if( GetUsageFlag() & I3G_USAGE_RENDERTARGET)
		pInfo->m_RTCount++;
	else
		pInfo->m_TexCount++;

	if( (GetWidth() == 0) || (GetHeight() == 0))
	{
		// Graphic Option에 의해 강제적으로 Loading이 되지 않은
		// 경우가 실제로 있다.
		return;
	}

	for( i = 0; i < (INT32) GetLevelCount(); i++)
	{
		i3Surface * pSurface = getSurface( i);

		if( GetUsageFlag() & I3G_USAGE_RENDERTARGET)
		{
			pInfo->m_RTSize += pSurface->GetDataSize();
		}
		else
		{
			pInfo->m_TexSize += pSurface->GetDataSize();
		}
	}
}

void i3TextureDX::Dump( const char * pszPath)
{
	i3Surface * pSurface = getSurface( 0);
	D3DXIMAGE_FILEFORMAT fmt;

	{
		char szExt[ MAX_PATH];

//		i3String::SplitFileExt( pszPath, szExt, sizeof( szExt));
		i3::extract_fileext(pszPath, szExt);

		if( i3::generic_is_iequal( szExt, "BMP") )
			fmt = D3DXIFF_BMP;
		else if( i3::generic_is_iequal( szExt, "TGA") )
			fmt = D3DXIFF_TGA;
		else if(i3::generic_is_iequal( szExt, "PNG") )
			fmt = D3DXIFF_PNG;
		else if( i3::generic_is_iequal( szExt, "DDS") )
			fmt = D3DXIFF_DDS;
		else if( i3::generic_is_iequal( szExt, "HDR") )
			fmt = D3DXIFF_HDR;
		else if( i3::generic_is_iequal( szExt, "PFM") )
			fmt = D3DXIFF_PFM;
		else
			fmt = D3DXIFF_JPG;
	}

	::D3DXSaveSurfaceToFile( pszPath,  fmt, pSurface->getSurface(), NULL, NULL);
}

#endif
#endif