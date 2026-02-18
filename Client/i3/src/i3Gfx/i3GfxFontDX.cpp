#include "i3GfxType.h"
#include "i3Base.h"
#include "i3GfxGlobalVariable.h"
#include "i3GfxGlobalVariableDX.h"
#include "i3GfxFontDX.h"
#include "i3GfxUtil.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/generic_string_size.h"

I3_CLASS_INSTANCE( i3GfxFontDXManager);

i3GfxFontDXManager::i3GfxFontDXManager(void)
{
	m_FontInfoList.reserve(32);
}

i3GfxFontDXManager::~i3GfxFontDXManager(void)
{
	Destroy( false);

	for( size_t i=0; i<m_FontInfoList.size(); i++)
	{
		FONT_DX_INFO * pListInfo = m_FontInfoList[i];
		i3MemFree( pListInfo);
	}

	m_FontInfoList.clear();
}

INT32 i3GfxFontDXManager::CreateFont( LPDIRECT3DDEVICE9 pDevice, const char* pszFont, INT32 FontHeight, UINT32 FontWeight, bool bItalic, DWORD dwQuality)
{
	INT32 nID = 0;

	if( m_FontInfoList.size() > 0)
	{
		FONT_DX_INFO Info;
		i3::string_ncopy_nullpad( Info._szFont, pszFont, i3::generic_string_size( pszFont));
		Info._FontHeight = FontHeight;
		Info._FontWeight = FontWeight;
		Info._bItalic = bItalic;
		Info._dwQuality = dwQuality;

		nID = CheckFontInfo( &Info);
		if( nID >= 0)
		{
			return nID;
		}
	}

	// Font 정보 추가
	FONT_DX_INFO * pInfo = (FONT_DX_INFO*)i3MemAlloc(sizeof(FONT_DX_INFO));
	i3::string_ncopy_nullpad( pInfo->_szFont, pszFont, i3::generic_string_size( pszFont));
	pInfo->_FontHeight = FontHeight;
	pInfo->_FontWeight = FontWeight;
	pInfo->_bItalic = bItalic;
	pInfo->_dwQuality = dwQuality;

	D3DXCreateFont(pDevice, FontHeight, 0, FontWeight, 0, bItalic, DEFAULT_CHARSET, 0, dwQuality, 0, pszFont, &pInfo->_pFont);
	m_FontInfoList.push_back( pInfo);

	pInfo->_pFont->OnResetDevice();

	return ((INT32)m_FontInfoList.size() - 1);
}

INT32 i3GfxFontDXManager::CheckFontInfo( FONT_DX_INFO* pInfo)
{	
	FONT_DX_INFO * pListInfo = nullptr;

	for(size_t i=0; i<m_FontInfoList.size(); i++)
	{
		pListInfo = m_FontInfoList[i];

		if( i3::generic_is_iequal( pInfo->_szFont, pListInfo->_szFont) )
		{	
			if( pInfo->_FontHeight == pListInfo->_FontHeight)
			{
				if( pInfo->_FontWeight == pListInfo->_FontWeight)
				{
					if( pInfo->_dwQuality == pListInfo->_dwQuality)
					{
						if( pInfo->_bItalic == pListInfo->_bItalic)
						{
							return i;
						}
					}
				}
			}
		}
	}

	return -1;
}

bool i3GfxFontDXManager::Destroy( bool bLostDevice)
{
	
	FONT_DX_INFO* pInfo = nullptr;

	for(size_t i = 0; i < m_FontInfoList.size(); i++)
	{
		pInfo = m_FontInfoList[ i];

		if( pInfo->_pFont != nullptr)
		{
			pInfo->_pFont->OnLostDevice();

			if( !bLostDevice)			// 아무리봐도, 틀린것 같다.. bLostDevice앞에 느낌표를 추가한다..(2012.09.13.수빈)
			{							// 이것때문에 종료시에 상당이 많은 DX내부 릭이 생겼었음...
				pInfo->_pFont->Release();
				pInfo->_pFont = nullptr;
			}
		}
	}

	return i3GfxResource::Destroy( bLostDevice);
}

bool i3GfxFontDXManager::Revive( i3RenderContext * pCtx)
{
	
	FONT_DX_INFO * pInfo = nullptr;

	for(size_t i = 0; i < m_FontInfoList.size(); i++)
	{
		pInfo = m_FontInfoList[i];

		if( pInfo->_pFont == nullptr)
		{
			D3DXCreateFont( (LPDIRECT3DDEVICE9)pCtx->GetNativeDevice(), pInfo->_FontHeight, 0, pInfo->_FontWeight, 0, pInfo->_bItalic, DEFAULT_CHARSET, 0, pInfo->_dwQuality, 0, pInfo->_szFont, &pInfo->_pFont);
		}

		if( pInfo->_pFont)
		{
			pInfo->_pFont->OnResetDevice();	
		}
	}

	return true;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//I3_CLASS_INSTANCE( i3GfxFontDX, i3GfxResource);
I3_CLASS_INSTANCE( i3GfxFontDX);

i3GfxFontDX::~i3GfxFontDX(void)
{
	if( m_pSprite) m_pSprite->Release();
}

bool i3GfxFontDX::Create( LPDIRECT3DDEVICE9 pDevice, const char* pszFont, INT32 FontHeight, UINT32 FontWeight, bool bItalic, DWORD dwQuality)
{
	m_FontHeight = FontHeight;
	m_FontWeight = FontWeight;
	m_bItalic = bItalic;
	m_dwQuality = dwQuality; 
	i3::string_ncopy_nullpad( m_szFont, pszFont, i3::generic_string_size( pszFont));

	m_nFontID = g_pRenderContext->getFontDxManager()->CreateFont( pDevice, pszFont, FontHeight, FontWeight, bItalic, dwQuality);
	D3DXCreateSprite(pDevice, &m_pSprite);

	return RestoreDeviceObjects();
}

bool i3GfxFontDX::RestoreDeviceObjects(void)
{
	m_pSprite->OnResetDevice();

	return true;
}

void i3GfxFontDX::ReleaseObject(void)
{
	if( m_pSprite)
	{
		m_pSprite->Release();
		m_pSprite = nullptr;
	}
}

LPD3DXFONT i3GfxFontDX::getFont(void)	
{ 
	return g_pRenderContext->getFontDxManager()->getFont(m_nFontID);
}


bool i3GfxFontDX::Destroy( bool bLostDevice)
{
	if( m_pSprite) m_pSprite->OnLostDevice();

	if( bLostDevice)
	{
		ReleaseObject();
	}

	return i3GfxResource::Destroy( bLostDevice);
}

bool i3GfxFontDX::Revive( i3RenderContext * pCtx)
{
	if( m_pSprite == nullptr)
	{
		D3DXCreateSprite( (LPDIRECT3DDEVICE9)pCtx->GetNativeDevice(), &m_pSprite);
	}

	RestoreDeviceObjects();

	return true;
}
