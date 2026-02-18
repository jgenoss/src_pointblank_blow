#include "pch.h"
#include "AvatarLoadMacroTexThread.h"
#include "AvatarManager.h"

#include "AvatarGen.h"

I3_CLASS_INSTANCE( AvatarLoadMacroTexThread);

AvatarLoadMacroTexThread::~AvatarLoadMacroTexThread()
{
	Clear();
}

void AvatarLoadMacroTexThread::Run( void *pData)
{
	if( m_EmptyFlag > 0 )
	{
		IDirect3DSurface9 * pDestSurf, * pSrcSurf;

		i3Texture * pSrcTex = m_LoadInfo._pSrcTex;

		if( m_LoadInfo._pSrcTex->IsPreload() )
		{
			while( m_LoadInfo._pSrcTex->GetFullLoadedTexture() == nullptr)
			{
				i3Timer::Sleep( 10);
			}

			pSrcTex = m_LoadInfo._pSrcTex->GetFullLoadedTexture();
		}

		i3Texture * pAlterTex = i3Texture::new_object();
#if defined( I3_DEBUG)
		pAlterTex->SetName( "MacroAlterTex");
#endif

		INT32 x = m_LoadInfo._offsetX;
		INT32 y = m_LoadInfo._offsetY;
		INT32 w = m_LoadInfo._destWidth;
		INT32 h = m_LoadInfo._destHeight;

		INT32 lv = MIN( pSrcTex->GetLevelCount(), m_LoadInfo._pDestTex->GetLevelCount());

		pAlterTex->setThreadMode(true);
		if( m_LoadInfo._bUseMacroTexture)
			pAlterTex->Create( w, h, lv, pSrcTex->GetPersistFormat(), pSrcTex->GetUsageFlag());
		else
			pAlterTex->Create( pSrcTex->GetWidth(), pSrcTex->GetHeight(), lv, pSrcTex->GetPersistFormat(), pSrcTex->GetUsageFlag());
		pAlterTex->setThreadMode(false);


		INT32 sc = 0;

		for( INT32 i = m_LoadInfo._pDestTex->getSurfaceCount()-1; i > -1; i--)
		{
			sc++;

			INT32 j = pAlterTex->getSurfaceCount()-sc;
			if( j < 0)
				break;

			INT32 width = (m_LoadInfo._destWidth / (1<<i));
			INT32 height = (m_LoadInfo._destHeight / (1<<i));

			RECT destRT;
			destRT.left = m_LoadInfo._offsetX / (1<<i);
			destRT.top = m_LoadInfo._offsetY / (1<<i);
			destRT.right = destRT.left + width;
			destRT.bottom = destRT.top + height;

			I3ASSERT( j < static_cast<INT32>(pAlterTex->GetLevelCount()));
			pAlterTex->Lock( j, 0);

			I3ASSERT( j < static_cast<INT32>(pSrcTex->GetLevelCount()));
			pSrcTex->SafeLock( j, I3G_LOCK_READONLY);

			if( m_LoadInfo._bUseMacroTexture)
			{
				cAvatarManager::CopyTexture( pAlterTex, pSrcTex, j, j, x, y, w, h);
			}
			else
			{
				I3ASSERT( pAlterTex->GetDataSize( j) == pSrcTex->GetDataSize( j));
				i3mem::Copy( pAlterTex->GetLockedBuffer( j), pSrcTex->GetLockedBuffer( j), pSrcTex->GetDataSize( j));
			}
			
			pSrcTex->SafeUnlock( j);
			pAlterTex->Unlock( j);
 
			pDestSurf = m_LoadInfo._pDestTex->getSurface( i)->getSurface();
			pSrcSurf = pAlterTex->getSurface( j)->getSurface();

			INT32 cnt = 0;
			while( pDestSurf == nullptr)
			{	// ResetDevice시 Surface가 해제되었을 수 있다. 기둘~
				i3Timer::Sleep( 10);	// 잠시 기둘
				pDestSurf = m_LoadInfo._pDestTex->getSurface( i)->getSurface();
				cnt++;
				if( cnt>1000)
				{
					I3PRINTLOG(I3LOG_FATAL,  "get Dest Surface fail!!!!!");
				}
			}

			I3ASSERT( destRT.left >= 0);
			I3ASSERT( destRT.right <= m_LoadInfo._pDestTex->getSurface( i)->getWidth());
			I3ASSERT( destRT.top >= 0);
			I3ASSERT( destRT.bottom <= m_LoadInfo._pDestTex->getSurface( i)->getHeight());
			

			cnt = 0;
			while( pSrcSurf == nullptr)
			{
				i3Timer::Sleep( 10);	// 잠시 기둘
				pSrcSurf = pAlterTex->getSurface( j)->getSurface();
				cnt++;
				if( cnt>1000)
				{
					I3PRINTLOG(I3LOG_FATAL,  "get Src surface fail!!!!!");
				}
			}

			// 32ms 많이 느림...
			HRESULT Rv = ::D3DXLoadSurfaceFromSurface( pDestSurf, NULL, &destRT,
													pSrcSurf, NULL, NULL,
													D3DX_FILTER_LINEAR,
													0);

			if( FAILED( Rv))
			{
				WCHAR wstrDesc[MAX_STRING_COUNT];
				DXGetErrorDescription(Rv, wstrDesc, MAX_STRING_COUNT);
				I3PRINTLOG(I3LOG_FATAL,  "Texture를 Macro Texture에 포함할 수 없습니다. %s", wstrDesc);
				goto EXIT;
			}
		}

		// Mipmap
		IDirect3DTexture9 * pDXTex = ((i3TextureDX *)m_LoadInfo._pDestTex)->GetDXImage();
		if( pDXTex != nullptr)
		{
			pDXTex->AddDirtyRect( nullptr);
		}

EXIT :
		I3_SAFE_RELEASE( pAlterTex);

#if defined( I3_DEBUG) && 0
		SYSTEMTIME info;
		::GetLocalTime( &info);
		I3TRACE( "Load Complate Macrotexture(%d:%d:%d) %s\n", info.wMinute, info.wSecond, info.wMilliseconds, pSrcTex->GetName());
#endif

		Clear();

		::InterlockedDecrement( (volatile LONG*) &m_EmptyFlag);
	}
}


void AvatarLoadMacroTexThread::SetLoad( tagLOADMACROTEXDATA * pData)
{
	pData->CopyTo( &m_LoadInfo);

	pData->_pDestTex = nullptr;
	pData->_pSrcTex = nullptr;
	pData->_pOwner = nullptr;

	::InterlockedIncrement( (volatile LONG*) &m_EmptyFlag);
	Wakeup();
}

void AvatarLoadMacroTexThread::Clear( void)
{
	if( m_LoadInfo._pOwner != nullptr)
	{
		AvatarGen * pGen = reinterpret_cast<AvatarGen*>( m_LoadInfo._pOwner);
		I3ASSERT( pGen != nullptr);
		if( pGen->FinishLoadMacroTexture( m_LoadInfo._pDestTex, m_LoadInfo._pSrcTex, m_LoadInfo._pTexInfo))
		{
			::InterlockedIncrement( (volatile LONG*) &m_Loaded);
		}

		I3_MUST_RELEASE(pGen);
		
		m_LoadInfo._pOwner = nullptr;
		m_LoadInfo.Clear();
	}
}
