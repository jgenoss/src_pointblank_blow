#if !defined( __I3_TEXT_ATTR_DX2_H)
#define __I3_TEXT_ATTR_DX2_H

#if defined (I3G_DX)

#include "i3TextCommonDefine.h"
#include "i3RenderAttr.h"
#include "i3Sprite2D.h"

#define FONT_OFFSET_CHARA_COUNT	8

struct PARSE_CTX
{
	DISP_CHAR_INFO *	_pChar = nullptr;
	COLOR				_Color;
	INT32				_x = 0;
	INT32				_y = 0;
	UINT32				_Flag = 0;
} ;

struct DRAW_GROUP
{
	INT16	m_TextureIndex = 0;
	INT32	m_idxCharStart = 0;
	INT32	m_cntPrim = 0;
} ;

#define			I3_TEXT_STATE_UPDATE_ALIGN			0x00000001
#define			I3_TEXT_STATE_UPDATE_POS			0x00000002
#define			I3_TEXT_STATE_UPDATE_COLOR			0x00000004
#define			I3_TEXT_STATE_UPDATE_TEX			0x00000008
#define			I3_TEXT_STATE_ENABLE				0x80000000

#define			I3_TEXT_STATE_AUTOWRAP				0x00010000
#define			I3_TEXT_STATE_SHADOW				0x00020000
#define			I3_TEXT_STATE_SKIP					0x00040000
#define			I3_TEXT_STATE_SETNOSKIP				0x00080000

#define			MAX_DRAW_GROUP						32

class I3_EXPORT_SCENE i3TextAttrDX2 : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3TextAttrDX2, i3RenderAttr);

protected:	
	i3RenderContext *			m_pContext = nullptr;
	i3GfxFontDX2 *				m_pFont = nullptr;
	i3::wstring					m_wstrText;
	i3VertexFormat				m_format;

	INT32						m_MaxCount = 0;
	INT32						m_Count = 0;
	UINT32						m_State = 0;

	INT32						m_LineCount = 0;

	INT32						m_PosX = 0;
	INT32						m_PosY = 0;
	REAL32						m_fOffsetPosX = 0.0f;
	REAL32						m_fOffsetPosY = 0.0f;
	INT32						m_nSpaceHorz = 0;
	INT32						m_nSpaceVert = 0;
	INT32						m_nWidth = FONT_TEXTURE_WIDTH;
	INT32						m_nHeight = FONT_TEXTURE_HEIGHT;
	INT32						m_nTextWidth = 0;
	INT32						m_AlignX = ALIGN_LEFT;
	INT32						m_AlignY = ALIGN_TOP;
	FONT_SHADOW_QUALITY			m_nShadowQuality = FONT_SHADOW_QUALITY_NORMAL;
	REAL32						m_fShadowOffSet = 0.0f;

	COLOR						m_TextColor = { 255, 255, 255, 255 };
	COLOR						m_TextColor_BackUp = { 255, 255, 255, 255 };
	COLOR						m_TextShadowColor = { 0, 0, 0, 255 };
	
	i3VertexArray *				m_pVA = nullptr;

	INT32						m_nCurPosX = 0;
	INT32						m_nCurPosY = 0;

	INT32						m_VtxStride = 0;

	INT32						m_idxDisplayStart = 0;

	//
	DISP_CHAR_INFO *			m_pCharTable = nullptr;

	DRAW_GROUP					m_DrawGroup[ MAX_DRAW_GROUP];
	INT32						m_DrawGroupCount = 0;

	INT32						m_VisibleLineCount = 0;

	//
	bool						m_bUseVertexBank = false;
	VERTEX_BANK_ELEMENT*		m_pVBElement = nullptr;

	bool						m_bIsOutChar = false;
	bool						m_bSameText = false;
	bool						m_bOutCharacterProcess = false;

	// Volatile members
	i3Shader *					m_pDrawShader = nullptr;

protected:
	void						_CreateVertex(void);
	DISP_CHAR_INFO *			_SetText( PARSE_CTX * pCtx, const i3::wliteral_range& rngText, bool bParseCmd = true);
	bool						_ParseCommand( PARSE_CTX * pCtx, const wchar_t* wszCmd);
	void						_ArrangeText(void);

	void						_SetColor(void);

	inline
	INT32						_getCharWidth( INT32 idx)
	{
		return m_pCharTable[idx].m_pGlyph->nGlyphWidth + m_nSpaceHorz;
	}

	inline
	INT32						_calcGlyphWidth( INT32 idx1, INT32 idx2)
	{
		INT32 w, i;

		for( i = idx1, w = 0; i <= idx2; i++)
		{
			w += _getCharWidth( i);
		}

		return w;
	}

public:
	i3TextAttrDX2(void);
	virtual ~i3TextAttrDX2(void);

	bool			CreateEx( const char * pszFontType, UINT32 Charset = DEFAULT_CHARSET, INT32 nMaxCount = 256, INT32 nFontSize = 10, INT32 nFontWeight = FW_NORMAL, bool bItalic = false, bool bUnderLine = false, INT32 nQuality = DEFAULT_QUALITY,
							bool bShadowEnable = false, FONT_SHADOW_QUALITY nShadowQuality = FONT_SHADOW_QUALITY_LOW, REAL32 shadowOffset = 1.0f, bool bUseVertexBank = false, bool bTransformedVertex = true);
    		
	void			SetText( const i3::wliteral_range& rngText, bool bParseCmd = true);
	const wchar_t*		getText(void) const						{ return m_wstrText.c_str(); }
	const i3::wstring&	getTextString() const					{ return m_wstrText; }

	i3GfxFontDX2*	getFont(void)	{	return m_pFont;}
	
	void			SetEnable( bool bEnable);
	void			SetPos(INT32 nX, INT32 nY);
	void			SetTextSpace( INT32 nX, INT32 nY);
	void			SetOffsetPos( REAL32 fX, REAL32 fY);	
	void			SetWidth(INT32 nWidth)	;
	void			SetHeight(INT32 nHeight);
	void			SetAlign( INT32 AlignX, INT32 AlignY );
	void			SetColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a);
	void			SetColor( COLOR * pColor);
	void			SetAlpha( UINT8 nAlpha);	
	void			SetShadowColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a);
	void			SetShadowColor( COLOR * pColor);
	void			SetShadowAlpha( UINT8 nAlpha);
	void			SetAutoWrapEnable( bool bEnable);

	INT32			CalcTextWidth(const i3::wliteral_range& rng) const;

	// Caret°ü·Ã
	INT32			GetCaretPosX(INT32 nLength);
	INT32			GetCaretPosY(INT32 nLength);

	UINT32			getState(void)				{ return m_State; }
	void			addState( UINT32 mask)		{ m_State |= mask; }
	void			removeState( UINT32 mask)	{ m_State &= ~mask; }
	bool			isState( UINT32 mask)		{ return (m_State & mask) != 0; }

	bool			getEnable()					{ return isState( I3_TEXT_STATE_ENABLE); }

	void			setFontSkip(bool bFlag)		
	{
		if( bFlag)
			addState( I3_TEXT_STATE_SKIP);
		else
			removeState( I3_TEXT_STATE_SKIP);
	}

	INT32			getPosX(void)				{ return m_PosX;			}
	INT32			getPosY(void)				{ return m_PosY;			}
	void			setPosY(INT32 iPosY)		{ m_PosY = iPosY;			}
	INT32			getWidth(void)				{ return m_nWidth;			}
	INT32			getHeight(void)				{ return m_nHeight;			}
	INT32			getTextWidth(void)			{ return m_nTextWidth;		}
	INT32			getTextHeight(void)			{ return m_pFont->getGlyphHeight(); }
	INT32			getLineCount(void)			{ return m_LineCount;		}
	INT32			getLineHeight(void)			{ return m_pFont->getGlyphHeight() + m_nSpaceVert; }

	INT32			getVisibleLineCount(void)	{ return m_VisibleLineCount;}
	bool			isOutChar(void)				{ return m_bIsOutChar;		}
	bool			isSameText(void)			{ return m_bSameText;		}
	void			setOutCharaProcess(bool bFlag)	{ m_bOutCharacterProcess = bFlag;	}


	INT32			GetCharacterSizeX( const i3::wliteral_range& rng) const;

	void			AdjustOffset( REAL32 diffWidth);

	void			ConnectVertexBank( UINT32 nCount);
	
	virtual void	Apply( i3RenderContext * pContext) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

	virtual void	OnLostDevice( bool bLostDevice) override;
	virtual void	OnRevive( i3RenderContext * pCtx) override;

	#if defined( I3_DEBUG)
	virtual void Dump(void) override;
	#endif
};

#endif

#endif	//__I3_TEXT_ATTR_DX2_H
