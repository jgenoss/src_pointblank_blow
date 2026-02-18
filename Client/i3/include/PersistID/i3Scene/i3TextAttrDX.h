#if !defined( __I3_TEXT_ATTR_DX_H)
#define __I3_TEXT_ATTR_DX_H

#if defined (I3G_DX)

#include "i3TextCommonDefine.h"
#include "i3RenderAttr.h"

class I3_EXPORT_SCENE i3TextAttrDX : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3TextAttrDX);

protected:
	i3GfxFontDX	*		m_pGfxFontDX;		
	char *				m_pTextBuf;	

	UINT32				m_PosX;
	UINT32				m_PosY;
	VEC2D				m_Position;
	UINT32				m_nWidth;
	UINT32				m_nHeight;
	INT32				m_nTextMaxCount;
	INT32				m_curTextBufCount;
	UINT32				m_AlignX;
	UINT32				m_AlignY;
	COLOR				m_TextColor;
	COLOR				m_TextShadowColor;
	UINT32				m_Weight;
	INT32				m_nTextWidth;
	INT32				m_nTextHeight;
    
	FONT_SHADOW_QUALITY	m_nShadowQuality;
	BOOL				m_bEnable;
	BOOL				m_bShadowEnable;
	BOOL				m_bAutoWrap;

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

	BOOL	Create( const char* pszFont, INT32 nTextMaxCount, INT32 nSize = 10, UINT32 nWeight = FW_NORMAL, BOOL bItalic = FALSE, DWORD dwQuality = DEFAULT_QUALITY);
	
	BOOL	SetText( const char *szText);
	char*	getText(void)											{ return m_pTextBuf;					}
	void	DrawText(void);
	INT32	CalculateFontSize( INT32 nSize);
	void	setEnable( BOOL bEnable);	
	void	setPos(UINT32 nX, UINT32 nY);
	void	setWidth(UINT32 nWidth)	;
	void	setHeight(UINT32 nHeight);
	void	SetAlign( UINT32 AlignX, UINT32 AlignY );
	void	SetColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a);
	void	SetColor( COLOR * pColor);
	void	SetShadowColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a);
	void	SetShadowColor( COLOR * pColor);
	void	setShadowEnable( BOOL bEnable, FONT_SHADOW_QUALITY nQuality);
	void	setAutoWrapEnable( BOOL bEnable);
		
	BOOL	getEnable()												{ return m_bEnable;						}
	INT32	getPosX(void)											{ return m_PosX;						}
	INT32	getPosY(void)											{ return m_PosY;						}
	INT32	getWidth(void)											{ return m_nWidth;						}
	INT32	getHeight(void)											{ return m_nHeight;						}
	INT32	getTextWidth(void)										{ return m_nTextWidth;					}
	INT32	getTextHeight(void)										{ return m_nTextHeight;					}
	VEC2D *	GetPos(void);

	// Caret°ü·Ã
	INT32	GetCaretPosX(INT32 nLength);
	INT32	GetCharacterSizeX( char *pszText);
	INT32	GetCharacterSizeY( char *pszText);

	virtual void Apply( i3RenderContext * pContext);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void Dump(void);
	#endif
};

#endif

#endif	//__I3_TEXT_ATTR_DX_H
