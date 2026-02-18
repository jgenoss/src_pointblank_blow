#if !defined( __I3_GFX_FONT_DX_H)
#define __I3_GFX_FONT_DX_H

#include "i3GfxResource.h"

struct FONT_DX_INFO
{
	char	_szFont[256] = { 0 };
	INT32	_FontHeight = 0;
	UINT32	_FontWeight = 0;
	bool	_bItalic = false;
	DWORD	_dwQuality = 0;
	ID3DXFont*	_pFont = nullptr;
};

class I3_EXPORT_GFX i3GfxFontDXManager : public i3GfxResource			// 2012.08.30.수빈.dll익스포트 추가..
{
	I3_EXPORT_CLASS_DEFINE( i3GfxFontDXManager, i3GfxResource);

protected:
	i3::vector<FONT_DX_INFO*>	m_FontInfoList;

public:
	i3GfxFontDXManager(void);
	virtual ~i3GfxFontDXManager(void);

	INT32			CreateFont( LPDIRECT3DDEVICE9 pDevice, const char* pszFont, INT32 FontHeight, UINT32 FontWeight, bool bItalic, DWORD dwQuality);
	INT32			CheckFontInfo( FONT_DX_INFO* pInfo);
	LPD3DXFONT		getFont( INT32 nID)			{ return m_FontInfoList[nID]->_pFont;	}

	virtual bool	Destroy( bool bLostDevice) override;
	virtual bool	Revive( i3RenderContext * pCtx) override;

};


class I3_EXPORT_GFX i3GfxFontDX : public i3GfxResource
{
	I3_EXPORT_CLASS_DEFINE( i3GfxFontDX, i3GfxResource);

protected:
	LPD3DXSPRITE		m_pSprite = nullptr;
	INT32				m_nFontID = 0;

	INT32				m_FontHeight = 0;
	UINT32				m_FontWeight = 0;
	bool				m_bItalic = false;
	DWORD				m_dwQuality = 0;
	char				m_szFont[256] = { 0 };

public:
	virtual ~i3GfxFontDX(void);

	bool			Create( LPDIRECT3DDEVICE9 pDevice, const char* pszFont, INT32 FontHeight, UINT32 FontWeight, bool bItalic, DWORD dwQuality);
	bool			RestoreDeviceObjects(void);
	void			ReleaseObject(void);

	LPD3DXFONT 		getFont(void);
	LPD3DXSPRITE 	getSprite(void)					{ return m_pSprite;			}

	virtual bool	Destroy( bool bLostDevice) override;
	virtual bool	Revive( i3RenderContext * pCtx) override;
};

#endif	// __I3_GFX_FONT_DX_H
