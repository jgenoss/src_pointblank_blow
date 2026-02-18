#if !defined( __I3_GFX_FONT_DX_H)
#define __I3_GFX_FONT_DX_H

#include "i3GfxResource.h"

typedef struct _FontDxInfo
{
	char	_szFont[256];
	INT32	_FontHeight;
	UINT32	_FontWeight;
	BOOL	_bItalic;
	DWORD	_dwQuality;
	ID3DXFont*	_pFont;
}FONT_DX_INFO;

class i3GfxFontDXManager : public i3GfxResource
{
	I3_CLASS_DEFINE( i3GfxFontDXManager);

protected:
	i3List				m_FontInfoList;

public:
	i3GfxFontDXManager(void);
	virtual ~i3GfxFontDXManager(void);

	INT32			CreateFont( LPDIRECT3DDEVICE9 pDevice, const char* pszFont, INT32 FontHeight, UINT32 FontWeight, BOOL bItalic, DWORD dwQuality);
	BOOL			CheckFontInfo( FONT_DX_INFO* pInfo);
	LPD3DXFONT		getFont( INT32 nID)			{ return ((FONT_DX_INFO*)m_FontInfoList.GetItem( nID))->_pFont;	}

	virtual BOOL	Destroy( bool bLostDevice);
	virtual BOOL	Revive( i3RenderContext * pCtx);

};


class I3_EXPORT_GFX i3GfxFontDX : public i3GfxResource
{
	I3_CLASS_DEFINE( i3GfxFontDX);

protected:
	LPD3DXSPRITE		m_pSprite;
	INT32				m_nFontID;

	INT32				m_FontHeight;
	UINT32				m_FontWeight;
	BOOL				m_bItalic;
	DWORD				m_dwQuality;
	char				m_szFont[256];

public:
	i3GfxFontDX(void);
	virtual ~i3GfxFontDX(void);

	BOOL			Create( LPDIRECT3DDEVICE9 pDevice, const char* pszFont, INT32 FontHeight, UINT32 FontWeight, BOOL bItalic, DWORD dwQuality);
	BOOL			RestoreDeviceObjects(void);
	void			ReleaseObject(void);

	LPD3DXFONT 		getFont(void);
	LPD3DXSPRITE 	getSprite(void)					{ return m_pSprite;			}

	virtual BOOL	Destroy( bool bLostDevice);
	virtual BOOL	Revive( i3RenderContext * pCtx);
};

#endif	// __I3_GFX_FONT_DX_H
