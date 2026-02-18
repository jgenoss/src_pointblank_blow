#if !defined( __I3_TEXT_ATTR_DX_H)
#define __I3_TEXT_ATTR_DX_H

#if defined (I3G_DX)

#include "i3TextCommonDefine.h"
#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3TextAttrDX : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3TextAttrDX, i3RenderAttr);

protected:
	i3GfxFontDX	*		m_pGfxFontDX = nullptr;
	char *				m_pTextBuf = nullptr;

	UINT32				m_PosX = 0;
	UINT32				m_PosY = 0;
	VEC2D				m_Position;
	UINT32				m_nWidth = 0;
	UINT32				m_nHeight = 0;
	INT32				m_nTextMaxCount = 0;
	INT32				m_curTextBufCount = 0;
	UINT32				m_AlignX = ALIGN_LEFT;
	UINT32				m_AlignY = ALIGN_TOP;
	COLOR				m_TextColor = { 255, 255, 255, 255 };
	COLOR				m_TextShadowColor = { 0, 0, 0, 255 };
	UINT32				m_Weight = 0;
	INT32				m_nTextWidth = 0;
	INT32				m_nTextHeight = 0;
    
	FONT_SHADOW_QUALITY	m_nShadowQuality = FONT_SHADOW_QUALITY_LOW;
	bool				m_bEnable = true;
	bool				m_bShadowEnable = false;
	bool				m_bAutoWrap = false;

	// Volatile members
	i3Shader *			m_pDrawShader = nullptr;
public:
	i3TextAttrDX(void);
	virtual ~i3TextAttrDX(void);

	/* Font Weights */
	//FW_DONTCARE         0
	//FW_THIN             100
	//FW_EXTRALIGHT       200
	//FW_LIGHT            300
	//FW_NORMAL           400
	//FW_MEDIUM           500
	//FW_SEMIBOLD         600
	//FW_BOLD             700
	//FW_EXTRABOLD        800
	//FW_HEAVY            900

	/* Font Quality */
	//DEFAULT_QUALITY         0
	//DRAFT_QUALITY           1
	//PROOF_QUALITY           2
	//#if(WINVER >= 0x0400)
	//NONANTIALIASED_QUALITY  3
	//ANTIALIASED_QUALITY     4
	//#endif /* WINVER >= 0x0400 */

	bool	Create( const char* pszFont, INT32 nTextMaxCount, INT32 nSize = 10, UINT32 nWeight = FW_NORMAL, bool bItalic = false, DWORD dwQuality = DEFAULT_QUALITY);
	
	bool		SetText( const char *szText);
	const char*	getText(void)											{ return m_pTextBuf;					}
	void	DrawText(void);
	INT32	CalculateFontSize( INT32 nSize);
	void	setEnable( bool bEnable);	
	void	setPos(UINT32 nX, UINT32 nY);
	void	setWidth(UINT32 nWidth)	;
	void	setHeight(UINT32 nHeight);
	void	SetAlign( UINT32 AlignX, UINT32 AlignY );
	void	SetColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a);
	void	SetColor( COLOR * pColor);
	void	SetShadowColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a);
	void	SetShadowColor( COLOR * pColor);
	void	setShadowEnable( bool bEnable, FONT_SHADOW_QUALITY nQuality);
	void	setAutoWrapEnable( bool bEnable);
		
	bool	getEnable()												{ return m_bEnable;						}
	INT32	getPosX(void)											{ return m_PosX;						}
	INT32	getPosY(void)											{ return m_PosY;						}
	INT32	getWidth(void)											{ return m_nWidth;						}
	INT32	getHeight(void)											{ return m_nHeight;						}
	INT32	getTextWidth(void)										{ return m_nTextWidth;					}
	INT32	getTextHeight(void)										{ return m_nTextHeight;					}
	VEC2D *	GetPos(void);

	// Caret°ü·Ã
	INT32	GetCaretPosX(INT32 nLength);
	INT32	GetCharacterSizeX( const char *pszText);
	INT32	GetCharacterSizeY( const char *pszText);

	virtual void Apply( i3RenderContext * pContext) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	#if defined( I3_DEBUG)
	virtual void Dump(void) override;
	#endif
};

#endif

#endif	//__I3_TEXT_ATTR_DX_H
