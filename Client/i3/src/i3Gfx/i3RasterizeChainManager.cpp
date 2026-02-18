#include "i3GfxType.h"

#include "i3GfxCaps.h"
#include "i3GfxGlobalVariable.h"
#include "i3GfxGlobalVariableDX.h"
#include "i3GfxUtil.h"
#include "i3GfxUtilDX.h"
#include "i3RasterizeChainBase.h"
#include "i3RasterizeChainManager.h"
#include "i3RasterizeChainContext.h"

#include "i3Base/itl/vector_util.h"
#include "i3Base/string/compare/generic_is_equal.h"


I3_CLASS_INSTANCE( i3RasterizeChainManager);

static i3RasterizeChainManager *	s_pInstance = nullptr;


i3RasterizeChainManager * i3RasterizeChainManager::getInstance(void)
{
	return s_pInstance;
}

i3RasterizeChainManager::i3RasterizeChainManager(void)
{
	s_pInstance = this;

	m_TempRTList.reserve( 16);
	m_NamedRTList.reserve( 16);
	m_ChainList.reserve( 16);
}

i3RasterizeChainManager::~i3RasterizeChainManager(void)
{
	

	{
		for(size_t i = 0; i < m_NamedRTList.size(); i++)
		{
			i3RasterizeChainContext* pCtx = m_NamedRTList[i];		
			I3_SAFE_RELEASE( pCtx);	
		}	

		for(size_t i=0; i<m_TempRTList.size(); i++)
		{
			i3RasterizeChainContext* pCtx = m_TempRTList[i];		
			I3_SAFE_RELEASE( pCtx);	
		}	

		m_NamedRTList.clear();
		m_TempRTList.clear();
	}

	for(size_t i = 0; i < m_ChainList.size(); i++)
	{
		i3RasterizeChainBase* pList = m_ChainList[i];
		I3_SAFE_RELEASE( pList);
	}

	m_ChainList.clear();

	I3_SAFE_RELEASE( m_pDefVA);
}


void i3RasterizeChainManager::_CreateDefVertexArray(void)
{
	VEC3D _pos[4];
	VEC2D _tex[4];
	i3VertexFormat format;
	i3RenderTarget * pRT = m_pGfx->GetRenderTarget( 0);
	REAL32 sx = (REAL32) pRT->GetWidth();
	REAL32 sy = (REAL32) pRT->GetHeight();	

	if( m_pDefVA == nullptr)
	{
		m_pDefVA = i3VertexArray::new_object();
		I3ASSERT( m_pDefVA != nullptr);

		format.SetHasPosition( true);
		format.SetTextureCoordSetCount( 1);
		format.SetTextureCoordCount( 2);	

		m_pDefVA->Create( &format, 4, 0);
	}	

	{
		//	Texture
		sx = (1.0f / sx) * 0.5f;
		sy = (1.0f / sy) * 0.5f;

		//	Position
		i3Vector::Set( &_pos[0], -1.0f, 1.0f, 0.0f);
		i3Vector::Set( &_pos[1], -1.0f, -1.0f, 0.0f);
		i3Vector::Set( &_pos[2], 1.0f, 1.0f, 0.0f);
		i3Vector::Set( &_pos[3], 1.0f, -1.0f, 0.0f);
		
		#if defined( I3G_OGL)
			i3Vector::Set( &_tex[0], 0.0f+sx, 1.0f-sy);
			i3Vector::Set( &_tex[1], 0.0f+sx, 0.0f+sy);
			i3Vector::Set( &_tex[2], 1.0f-sx, 1.0f-sy);			
			i3Vector::Set( &_tex[3], 1.0f-sx, 0.0f+sy);
		#else
			i3Vector::Set( &_tex[0], 0.0f+sx, 0.0f+sy);
			i3Vector::Set( &_tex[1], 0.0f+sx, 1.0f+sy);
			i3Vector::Set( &_tex[2], 1.0f+sx, 0.0f+sy);			
			i3Vector::Set( &_tex[3], 1.0f+sx, 1.0f+sy);
		#endif
	}

	m_pDefVA->Lock( 0);
	m_pDefVA->SetPositions( 0, 4, _pos);
	m_pDefVA->SetTextureCoords( 0, 0, 4, _tex);
	m_pDefVA->Unlock();
}


void i3RasterizeChainManager::Create( i3RenderContext * pGfx)
{
	m_bInitialized = true;

	m_pGfx = pGfx;

	// Original RT에 대한 정보를 가져옵니다.
	{
		m_DSWidth	= m_RTWidth = m_pGfx->getWindowWidth();
		m_DSHeight	= m_RTHeight = m_pGfx->getWindowHeight();
		m_RTImageFormat = m_pGfx->getWindowColorFormat();
		m_DSImageFormat = m_pGfx->getWindowDepthFormat();
	}

	_CreateDefVertexArray();
}


void i3RasterizeChainManager::OnSize()
{
	if( m_pGfx == nullptr)	return;

	if( m_RTWidth == m_pGfx->getWindowWidth() &&
		m_DSHeight == m_pGfx->getWindowHeight())
		return;

	Create( m_pGfx);


	_ResizeTarget();

	INT32 i;

	for( i = 0; i < getChainCount(); i++)
	{
		i3RasterizeChainBase * pChain = getChain( i);

		pChain->OnSize( m_pGfx);
	}
}

void i3RasterizeChainManager::OnEnableChain( i3RasterizeChainBase * pChain)
{
	_UpdateChainTarget();
}


void i3RasterizeChainManager::DestroyRenderTarget(void)
{
	
	for( size_t i = 0; i < m_NamedRTList.size(); i++)
	{
		i3RasterizeChainContext* pCtx = m_NamedRTList[i];		
		
		pCtx->Destroy();
	}	

	for( size_t i=0; i<m_TempRTList.size(); i++)
	{
		i3RasterizeChainContext* pCtx = m_TempRTList[i];		
		pCtx->Destroy();
	}	
}

I3G_IMAGE_FORMAT i3RasterizeChainManager::GetOriginalRenderTargetImageFormat(void)
{
	return m_RTImageFormat;
}

i3RasterizeChainContext * i3RasterizeChainManager::FindTempRT( INT32 w, INT32 h, I3G_IMAGE_FORMAT fmt, i3RasterizeChainContext * pExcept)
{
	INT32 i;
	i3RasterizeChainContext* pCtx;

	if( w <= 0)
		w = m_pGfx->getWindowWidth();

	if( h <= 0)
		h = m_pGfx->getWindowHeight();

	if( fmt == I3G_IMAGE_FORMAT_NONE)
		fmt = m_pGfx->getWindowColorFormat();

	for( i = 0; i < getTempRTCount(); i++)
	{
		pCtx = getTempRT( i);
		// revision 2252
		if (pCtx == nullptr)
		{
			I3PRINTLOG(I3LOG_NOTICE, "i3RasterizeChainManager::FindTempRT pCtx : nullptr Index:%d",i);
			continue;
		}

		if( ((INT32) pCtx->getWidth() == w) && ((INT32) pCtx->getHeight() == h) && (pCtx->getFormat() == fmt) && (pCtx != pExcept))
		{
			// 같은이름의 Context가 존재하므로, 그것을 반환합니다.
			return pCtx;			
		}
	}

	return nullptr;
}

i3RasterizeChainContext * i3RasterizeChainManager::GetTempRT( INT32 w, INT32 h, I3G_IMAGE_FORMAT fmt, i3RasterizeChainContext * pExcept)
{
	i3RasterizeChainContext * pRT = nullptr;

	pRT = FindTempRT( w, h, fmt, pExcept);

	if( pRT == nullptr)
	{
		bool bDepth = false;
		if( m_pGfx->GetVideoInfo()->m_MultiSampling > 0)
			bDepth = true;

		pRT = RegisterRT( w, h, fmt, bDepth, nullptr);
	}

	return pRT;
}

i3RasterizeChainContext * i3RasterizeChainManager::_RegisterRT( UINT32 w, UINT32 h, UINT32 style, I3G_IMAGE_FORMAT fmt, bool bDepth, char* pszName, bool bNamedRT)
{
	i3RasterizeChainContext* pCtx;

	if( bNamedRT)
	{
		pCtx = FindNamedRT( pszName, false);
		if( pCtx != nullptr)
		{
			if( (pCtx->getWidth() == w) && (pCtx->getHeight() == h) && (pCtx->getFormat() == fmt))
			{
				return pCtx;
			}
			else
			{
				i3::vu::remove(m_NamedRTList, pCtx);
			}
		}
	}
	else
	{
		pCtx = FindTempRT( w, h, fmt, nullptr);
		if( pCtx != nullptr)
			return pCtx;
	}

	return _AddRT( w, h, style, fmt, bDepth, pszName, bNamedRT);
}

i3RasterizeChainContext *	i3RasterizeChainManager::_AddRT( UINT32 w, UINT32 h, UINT32 style, I3G_IMAGE_FORMAT fmt, bool bDepth, char* pszName, bool bNamedRT)
{
	i3RasterizeChainContext* pCtx;

	pCtx = i3RasterizeChainContext::new_object();

	// Render Target의 Context의 값을 설정합니다.
	{
		I3G_IMAGE_FORMAT imageFormat;

		if( w > 4096)
			w = m_pGfx->getWindowWidth();

		if( h > 4096)
			h = m_pGfx->getWindowHeight();

		if( fmt == I3G_IMAGE_FORMAT_NONE)
			fmt = m_pGfx->getWindowColorFormat();

		if( fmt == I3G_IMAGE_FORMAT_NONE)
			imageFormat = m_RTImageFormat;
		else 
			imageFormat = fmt;
		
		pCtx->Create( w, h, 1, fmt, bDepth);
		pCtx->setStyle( style);

		if( pszName != nullptr)
		{
			pCtx->SetName( pszName);

			i3Texture * pTex = pCtx->getColorTarget();
			pTex->SetName( pszName);
		}
		else
		{
			char conv[256];

			i3::snprintf( conv, sizeof(conv), "Nonamed RT:%p", pCtx);
			
			i3Texture * pTex = pCtx->getColorTarget();
			pTex->SetName( conv);
		}

		if( bNamedRT)
		{
			m_NamedRTList.push_back( pCtx);
		}
		else
		{
			m_TempRTList.push_back( pCtx);
		}
	}

	return pCtx;
}

i3RasterizeChainContext* i3RasterizeChainManager::RegisterRT( char* pszName, bool bScaled, INT32 nDownScale, bool bDepth)
{
	i3RasterizeChainContext* pCtx;
	UINT32 style = 0;

	style = CTS_DEFAULT_WIDTH | CTS_DEFAULT_HEIGHT | CTS_DEFAULT_FORMAT;

	INT32				w	= m_pGfx->getWindowWidth();
	INT32				h	= m_pGfx->getWindowHeight();
	I3G_IMAGE_FORMAT	fmt	= m_pGfx->getWindowColorFormat();

	if( bScaled)
	{
		w = (INT32)(w * (1.0f/nDownScale));
		h = (INT32)(h * (1.0f/nDownScale));

		style |= CTS_DEFAULT_SCALED;
	}

	pCtx = _RegisterRT( w, h, style, fmt, bDepth, pszName, false);
	I3ASSERT( pCtx != nullptr);

	if( bScaled)	pCtx->setDownScaleRate( nDownScale);

	return pCtx;
}

i3RasterizeChainContext* i3RasterizeChainManager::RegisterNamedRT( char* pszName, bool bScaled, INT32 nDownScale, bool bDepth)
{
	i3RasterizeChainContext* pCtx;
	UINT32 style = 0;

	style = CTS_DEFAULT_WIDTH | CTS_DEFAULT_HEIGHT | CTS_DEFAULT_FORMAT;

	INT32				w	= m_pGfx->getWindowWidth();
	INT32				h	= m_pGfx->getWindowHeight();
	I3G_IMAGE_FORMAT	fmt	= m_pGfx->getWindowColorFormat();

	if( bScaled)
	{
		w = (INT32)(w * (1.0f/nDownScale));
		h = (INT32)(h * (1.0f/nDownScale));

		style |= CTS_DEFAULT_SCALED;
	}

	pCtx = _RegisterRT( w, h, style, fmt, bDepth, pszName, true);
	I3ASSERT( pCtx != nullptr);

	if( bScaled)	pCtx->setDownScaleRate( nDownScale);

	return pCtx;
}

i3RasterizeChainContext* i3RasterizeChainManager::RegisterRT( INT32 w, INT32 h, I3G_IMAGE_FORMAT fmt, bool bDepth, char* pszName)
{
	i3RasterizeChainContext* pCtx;
	UINT32 style = 0;

	if( w <= 0)
	{
		w = m_pGfx->getWindowWidth();

		style |= CTS_DEFAULT_WIDTH;
	}

	if( h <= 0)
	{
		h = m_pGfx->getWindowHeight();

		style |= CTS_DEFAULT_HEIGHT;
	}

	if( fmt == I3G_IMAGE_FORMAT_NONE)
	{
		fmt = m_pGfx->getWindowColorFormat();
		style |= CTS_DEFAULT_FORMAT;
	}

	pCtx = _RegisterRT( w, h, style, fmt, bDepth, pszName, false);
	I3ASSERT( pCtx);

	return pCtx;
}

i3RasterizeChainContext* i3RasterizeChainManager::RegisterNamedRT( INT32 w, INT32 h, I3G_IMAGE_FORMAT fmt, bool bDepth, char* pszName)
{
	i3RasterizeChainContext* pCtx;
	UINT32 style = 0;

	if( w <= 0)
	{
		w = m_pGfx->getWindowWidth();

		style |= CTS_DEFAULT_WIDTH;
	}

	if( h <= 0)
	{
		h = m_pGfx->getWindowHeight();

		style |= CTS_DEFAULT_HEIGHT;
	}

	if( fmt == I3G_IMAGE_FORMAT_NONE)
	{
		fmt = m_pGfx->getWindowColorFormat();
		style |= CTS_DEFAULT_FORMAT;
	}

	pCtx = _RegisterRT( w, h, style, fmt, bDepth, pszName, true);
	I3ASSERT( pCtx);

	return pCtx;
}

void i3RasterizeChainManager::UnregisterRT( i3RasterizeChainContext * pCtx)
{
	

	size_t idx = i3::vu::index_of(m_NamedRTList, pCtx);
	if( idx != m_NamedRTList.size())
	{
		m_NamedRTList.erase( m_NamedRTList.begin() + idx);
	}
	else
	{
		idx = i3::vu::index_of(m_TempRTList, pCtx);

		if( idx != m_TempRTList.size() )
			m_TempRTList.erase(m_TempRTList.begin() + idx);

		I3ASSERT( idx != m_TempRTList.size() );
	}
	
	I3_MUST_RELEASE( pCtx);
}

i3RasterizeChainContext* i3RasterizeChainManager::FindNamedRT( const char* pszName, bool bMustExist)
{
	

	// 기존꺼에 이미 등록되어 있는지 이름으로 검색합니다.
	for( size_t i = 0; i < m_NamedRTList.size(); i++)
	{
		i3RasterizeChainContext* pCtx = m_NamedRTList[i];

		if( i3::generic_is_equal( pCtx->GetNameString(), pszName) )
			return pCtx;
	}

	if( bMustExist)
	{
		I3PRINTLOG(I3LOG_FATAL,  "There is no match about NamedChainRTConext.. (%s)", pszName);
	}

	return nullptr;
}


i3RasterizeChainBase * i3RasterizeChainManager::_FindNextColorOut( INT32 idx)
{
	INT32 i;
	i3RasterizeChainBase * pChain;

	for( i = idx + 1; i < getChainCount(); i++)
	{
		pChain = getChain( i);

		if( pChain->getEnable() == false)
			continue;

		if( pChain->isColorOut())
			return pChain;
	}

	return nullptr;
}

void i3RasterizeChainManager::SetRenderTarget( i3RasterizeChainContext * pCtx)
{
	if( pCtx != nullptr)
	{
		// fbo complete check를 depth target의 setting이 마무리된후에 하도록 한다.
		i3Texture * pTex = pCtx->getColorTarget();

		I3ASSERT( pTex != nullptr);

		#if defined( I3G_OGL)
		m_pGfx->enableFBOCompleteCheck( false);
		#endif

		m_pGfx->SetColorRenderTarget( pTex);
		#if defined( I3G_OGL)
		m_pGfx->enableFBOCompleteCheck( true);
		#endif

		if( pCtx->getDepthTarget() != nullptr)
		{
			m_pGfx->SetDepthRenderTarget( pCtx->getDepthTarget());
		}
		else
		{
			m_pGfx->SetDepthRenderTarget( nullptr);
		}
	}
	else
	{
		//m_pGfx->resetD3DSurface();
		//m_pGfx->resetD3DDeapthSurface();

		m_pGfx->SetCurrentRenderTarget( nullptr);
	}
}

i3RasterizeChainContext * i3RasterizeChainManager::SwapTarget( void)
{
	I3ASSERT( m_pLastRT != nullptr);

	i3RasterizeChainContext * pNewRT = FindTempRT( m_pLastRT->getWidth(), m_pLastRT->getHeight(), m_pLastRT->getFormat(), m_pLastRT);

	SetRenderTarget( pNewRT);

	return pNewRT;
}

void i3RasterizeChainManager::_UpdateChainTarget(void)
{
	INT32 i, inputCnt, j;
	i3RasterizeChainBase * pChain, * pNextChain, * pFirstChain = nullptr;
	I3_CHAIN_INPUT_SPEC		input[10];
	i3RasterizeChainContext * pRT = nullptr, * pPrevRT = nullptr;
	I3G_IMAGE_FORMAT	targetFormat = I3G_IMAGE_FORMAT_BGRX_8888;
	
	I3TRACE( "------------------- Update Chain List -------------------\n");

	// 가장 높은 요구 조건의 Format을 찾는다.
	{
		INT32				bits, MaxBits, compo, MaxCompo;

		MaxBits = i3Gfx::GetImageFormatBitCount( targetFormat);
		MaxCompo = i3Gfx::GetImageFormatComponentCount( targetFormat);
		
		for( i = 0; i < getChainCount(); i++)
		{
			pChain = getChain( i);

			if( pChain->getEnable() == false)
				continue;

			if( pChain->isColorOut() == false)
				continue;

			if( pFirstChain == nullptr)
				pFirstChain = pChain;

			inputCnt = pChain->GetInputSpec( input);

			for( j = 0; j < inputCnt; j++)
			{
				if( input[j].m_Format == I3G_IMAGE_FORMAT_DONT_CARE)
				{
				}
				else
				{
					bits = i3Gfx::GetImageFormatBitCount( input[j].m_Format);
					compo = i3Gfx::GetImageFormatComponentCount( input[j].m_Format);

					if( (bits > MaxBits) && (compo > MaxCompo))
					{
						targetFormat = input[j].m_Format;
						MaxBits = bits;
						MaxCompo = compo;
					}
				}
			}
		}
	}

	// 첫번째 Chain에 대한 Input RT 조정.
	{
		m_pFirstRT = nullptr;

		if( pFirstChain != nullptr)
		{
			if( pFirstChain->GetInputSpec( input) > 0)
			{
				pRT = m_pFirstRT = GetTempRT( input[0].m_Width, input[0].m_Height, input[0].m_Format, nullptr);
			}
		}
	}

	for( i = 0; i < getChainCount(); i++)
	{
		pChain = getChain( i);

		if( pChain->getEnable() == false)
			continue;

		pNextChain = _FindNextColorOut( i);
		
		if( pChain->isColorOut() && (pNextChain != nullptr))
		{
			inputCnt = pNextChain->GetInputSpec( input);

			if( inputCnt > 0)
			{
				// Input을 필요로 한다.
				for( j = 0; j < inputCnt; j++)
				{
					if( input[j].m_Format == I3G_IMAGE_FORMAT_DONT_CARE)
						input[j].m_Format = targetFormat;

					pRT = FindTempRT( input[j].m_Width, input[j].m_Height, input[j].m_Format, pRT);
					if( pRT != nullptr)
						break;
				}

				if( pRT == nullptr)
				{
					pRT = RegisterRT( input[0].m_Width, input[0].m_Height, input[0].m_Format, false);
				}

				if( pRT == pPrevRT)
				{
					UINT32 style;

					style = CTS_DEFAULT_WIDTH | CTS_DEFAULT_HEIGHT | CTS_DEFAULT_FORMAT;

					pRT = _AddRT( input[0].m_Width, input[0].m_Height, style, input[0].m_Format, false, nullptr, false);
				}
			}
		}
		else
		{
			pRT = nullptr;		// 마지막 
		}

		if( pRT != nullptr)
		{
			I3TRACE( "CHAIN[%d] : %s -> %p(%dx%dx%s)\n", i - 1, pChain->meta()->class_name(), pRT, pRT->getWidth(), pRT->getHeight(), 
				i3Gfx::GetImageFormatName( pRT->getFormat()));
		}
		else
		{
			I3TRACE( "CHAIN[%d] : %s -> nullptr\n", i - 1, pChain->meta()->class_name());
		}

		// 이번 Input은 이전 Chain의 Output이 된다.
		pChain->setTarget( pRT);

		pPrevRT = pRT;

	}
}

void i3RasterizeChainManager::AddChain( i3RasterizeChainBase* pChain, bool bAutoAdjust)
{
	if( m_bInitialized == false)
		Create( g_pRenderContext);

	// Chain을 List에 등록합니다.	
	m_ChainList.push_back( pChain);
	I3_MUST_ADDREF( pChain);

	if( pChain->isCreated() == false)
	{
		pChain->OnCreate( this);

		pChain->setCreated();
	}

	if( bAutoAdjust)
		_UpdateChainTarget();
}


void i3RasterizeChainManager::RemoveChain( i3RasterizeChainBase * pChain, bool bAutoAdjust)
{
	

	size_t idx = i3::vu::index_of(m_ChainList, pChain);

	if( idx != m_ChainList.size() )
	{
		m_ChainList.erase( m_ChainList.begin() + idx);
		I3_MUST_RELEASE( pChain);

		if( bAutoAdjust)
			_UpdateChainTarget();
	}
}

void i3RasterizeChainManager::RemoveAllChain( bool bAutoAdjust)
{
	for(size_t i = 0; i < m_ChainList.size(); i++)
	{
		i3RasterizeChainBase * pChain = m_ChainList[i];
		I3_SAFE_RELEASE( pChain);
	}

	m_ChainList.clear();

	if( bAutoAdjust)
		_UpdateChainTarget();
}

i3RasterizeChainBase * i3RasterizeChainManager::FindChainByName( i3ClassMeta * pMeta)
{
	INT32 i;

	for( i = 0; i < getChainCount(); i++)
	{
		i3RasterizeChainBase * pChain = getChain( i);

		if( pChain->meta() == pMeta)
		{
			return pChain;
		}
	}

	return nullptr;
}

i3RasterizeChainBase* i3RasterizeChainManager::AddChainByName( const char* chainname)
{
	i3ClassMeta* pMeta = i3ClassMetaSystem::i()->find_class_meta(chainname);
						// i3ClassMeta::FindClassMetaByName( chainname);
	I3ASSERT( pMeta != nullptr);	
	i3RasterizeChainBase* pBase = (i3RasterizeChainBase*) pMeta->create_instance(); // (i3RasterizeChainBase*)CREATEINSTANCE(pMeta);

	pBase->SetEnable(true);
	AddChain( pBase);

	return pBase;
}

void i3RasterizeChainManager::RemoveChainByIndex( INT32 nIndex)
{
	i3RasterizeChainBase* pBase = m_ChainList[nIndex];

	if( pBase!= nullptr)
	{
		#if defined( I3_DISTRIBUTE)
		RemoveDebugViewTexture( pBase);
		#endif

		I3_SAFE_RELEASE_NONULL_POINTER( pBase);

		//NamedRendertarget일 경우에는 Rendertarget도 같이 삭제해줘야 한다.
		//m_NamedRTList....
		pBase->SetEnable(false);
		m_ChainList.erase( m_ChainList.begin() + nIndex);

		I3_SAFE_RELEASE( pBase);
	}

	_UpdateChainTarget();
}

void i3RasterizeChainManager::SwapChainByIndex( INT32 nIndex1, INT32 nIndex2)
{
	i3::swap(m_ChainList[nIndex1], m_ChainList[nIndex2]);
//	void* temp = m_ChainList.GetItem( nIndex1);
//	m_ChainList.SetItem( nIndex1, m_ChainList.GetItem( nIndex2));
//	m_ChainList.SetItem( nIndex2, temp);
}

void i3RasterizeChainManager::Apply( REAL32 tm, i3RenderContext * pGfx)
{
	I3RC_UPDATE info;
	COLOR oldClearColor;

	i3Color::Copy( &oldClearColor, pGfx->GetClearColor());

	//=====================================================================
	// RasterizeChain을 적용!
	INT32  i;
	i3RasterizeChainBase * pChain;

	// Output Attribute 와 RenderContext를 RasterizeChain에 설정합니다.
	{
		info.m_pGfx = pGfx;
		info.m_time = tm;
		info.m_pLastRT = m_pFirstRT;
	}

	pGfx->ResetDefaultRenderTarget();

	for( i=0; i < getChainCount(); i++)
	{
		pChain = getChain( i);
		I3ASSERT( pChain != nullptr);

		if( pChain->getEnable() == false)
			continue;

		i3RasterizeChainContext * pRT = pChain->getTarget();

		SetRenderTarget( pRT);

		if( pRT != nullptr)
			m_pLastRT = pRT;

		pGfx->SetZWriteEnable(false);
		pGfx->SetZTestEnable(false);
		pGfx->SetRenderMode( I3G_RENDER_POLYGON);
		pGfx->SetFaceCullMode( I3G_FACE_CULL_NONE);
		pGfx->SetAlphaTestEnable(false);
		pGfx->SetBlendEnable(false);
		
		pChain->ApplyChain( &info);

		info.m_pLastRT = m_pLastRT;
	}

	SetRenderTarget( nullptr);
	pGfx->SetShader( nullptr);

	pGfx->SetClearColor( &oldClearColor);
}


void i3RasterizeChainManager::_ResizeTarget(void)
{
	INT32 i, w, h;
	I3G_IMAGE_FORMAT fmt;
	i3RasterizeChainContext * pRT;
	char conv[256];
	
	for( i = 0; i < getTempRTCount(); i++)
	{
		pRT = getTempRT( i);

		if( pRT->getStyle() & (CTS_DEFAULT_WIDTH | CTS_DEFAULT_HEIGHT | CTS_DEFAULT_FORMAT))
		{
			if( pRT->getStyle() & CTS_DEFAULT_WIDTH)
			{
				w = m_RTWidth;
				if( pRT->getStyle() & CTS_DEFAULT_SCALED)
				{
					w = (INT32)(m_RTWidth * (1.0f / pRT->getDownScaleRate()));
				}
			}
			else
				w = pRT->getWidth();

			if( pRT->getStyle() & CTS_DEFAULT_HEIGHT)
			{
				h = m_RTHeight;
				if( pRT->getStyle() & CTS_DEFAULT_SCALED)
				{
					h = (INT32)(m_RTHeight* (1.0f /  pRT->getDownScaleRate()));
				}
			}
			else
				h = pRT->getHeight();

			if( pRT->getStyle() & CTS_DEFAULT_FORMAT)
				fmt = m_RTImageFormat;
			else
				fmt = pRT->getFormat();

			pRT->Create( w, h, 1, fmt, pRT->hasDepthTarget());

			{
				i3::snprintf( conv, sizeof(conv), "Nonamed RT:%p", pRT);
				i3Texture * pTex = pRT->getColorTarget();
				pTex->SetName( conv);

				i3::snprintf( conv, sizeof(conv), "Nonamed Depth RT:%p", pRT);
				pTex = pRT->getDepthTarget();

				if( pTex != nullptr)
					pTex->SetName( conv);
			}
		}
	}

	for( i = 0; i < getNamedRTCount(); i++)
	{
		pRT = getNamedRT( i);

		if( pRT->getStyle() & (CTS_DEFAULT_WIDTH | CTS_DEFAULT_HEIGHT | CTS_DEFAULT_FORMAT))
		{
			if( pRT->getStyle() & CTS_DEFAULT_WIDTH)
			{
				w = m_RTWidth;
				if( pRT->getStyle() & CTS_DEFAULT_SCALED)
				{
					w = (INT32)(m_RTWidth* (1.0f /  pRT->getDownScaleRate()));
				}
			}
			else
				w = pRT->getWidth();

			if( pRT->getStyle() & CTS_DEFAULT_HEIGHT)
			{
				h = m_RTHeight;
				if( pRT->getStyle() & CTS_DEFAULT_SCALED)
				{
					h = (INT32)(m_RTHeight* (1.0f /  pRT->getDownScaleRate()));
				}
			}
			else
				h = pRT->getHeight();

			if( pRT->getStyle() & CTS_DEFAULT_FORMAT)
				fmt = m_RTImageFormat;
			else
				fmt = pRT->getFormat();

			I3TRACE( "RT : %s : %d, %d, %s\n", pRT->GetName(), w, h, i3Gfx::GetImageFormatName( fmt));

			pRT->Create( w, h, 1, fmt, pRT->hasDepthTarget());

			{
				i3Texture * pTex = pRT->getColorTarget();

				pTex->SetName( pRT->GetNameString());
			}
		}
	}

	// anti-aliasing을 사용하게 되는경우, main rendertarget의 depth를 사용할 수 없기 때문에,
	// rasterize chain context는 해당 size보다 큰 depth buffer를 가지고 있어야 한다.
	// 이전에 만들어진 depth buffer중에서 자신보다 큰 size를 가진 depth buffer가 있으면 link하고
	// 아니라면 새로 만들어 준다.
	if( m_pGfx->GetVideoInfo()->m_MultiSampling > 0)
	{
		for( i = 0; i < getTempRTCount(); i++)
		{
			pRT = getTempRT( i);

			if( pRT->hasDepthTarget() == false)
			{
				pRT->MakeDepthTargetIfNULL( _FindAdjustDepth( pRT));
			}
		}

		for( i = 0; i < getNamedRTCount(); i++)
		{
			pRT = getNamedRT( i);
			if( pRT->hasDepthTarget() == false)
			{
				pRT->MakeDepthTargetIfNULL( _FindAdjustDepth( pRT));
			}
		}
	}
}

i3Texture * i3RasterizeChainManager::_FindAdjustDepth( i3RasterizeChainContext * pCtxExcept)
{
	UINT32 width = pCtxExcept->getWidth();
	UINT32 height = pCtxExcept->getHeight();

	INT32 i;
	i3Texture * pDepthTarget = nullptr;
	i3RasterizeChainContext * pRT = nullptr;
	for( i = 0; i < getTempRTCount(); i++)
	{
		pRT = getTempRT( i);

		if( pRT == pCtxExcept)
			continue;

		if( pRT->getWidth() < width)
			continue;

		if( pRT->getHeight() < height)
			continue;

		pDepthTarget = pRT->getDepthTarget();
		if( pDepthTarget != nullptr)
		{
			return pDepthTarget;
		}
	}

	for( i = 0; i < getNamedRTCount(); i++)
	{
		pRT = getNamedRT( i);

		if( pRT == pCtxExcept)
			continue;

		if( pRT->getWidth() < width)
			continue;

		if( pRT->getHeight() < height)
			continue;

		pDepthTarget = pRT->getDepthTarget();
		if( pDepthTarget != nullptr)
		{
			return pDepthTarget;
		}
	}

	return nullptr;
}

bool i3RasterizeChainManager::Destroy( bool bLostDevice)
{
	I3_SAFE_RELEASE( m_pDefVA);

	return i3GfxResource::Destroy( bLostDevice);
}

bool i3RasterizeChainManager::isDestroyed( void)
{
	return (m_pDefVA == nullptr);
}

bool i3RasterizeChainManager::Revive( i3RenderContext * pCtx)
{
	Create( pCtx);

	_ResizeTarget();

	_UpdateChainTarget();

	return i3GfxResource::Revive( pCtx);
}
