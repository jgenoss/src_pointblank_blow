#if !defined( __I3_TEXT_ATTR_DX2_H)
#define __I3_TEXT_ATTR_DX2_H

#if defined (I3G_DX)

#include "i3TextCommonDefine.h"
#include "i3RenderAttr.h"
#include "i3Sprite2D.h"

#define FONT_OFFSET_CHARA_COUNT	8

typedef struct _ParseCtx
{
	DISP_CHAR_INFO *	_pChar;
	COLOR				_Color;
	INT32				_x;
	INT32				_y;
	UINT32				_Flag;
} PARSE_CTX;

typedef struct _DrawGroup
{
	INT16	m_TextureIndex;
	INT32	m_idxCharStart;
	INT32	m_cntPrim;
} DRAW_GROUP;

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
	I3_CLASS_DEFINE( i3TextAttrDX2);

protected:	
	i3RenderContext *			m_pContext;
	i3GfxFontDX2 *				m_pFont;
	char *						m_pszText;
	i3VertexFormat				m_format;

	INT32						m_MaxCount;
	INT32						m_Count;
	UINT32						m_State;

	INT32						m_LineCount;

	INT32						m_PosX;
	INT32						m_PosY;
	REAL32						m_fOffsetPosX;
	REAL32						m_fOffsetPosY;			
	INT32						m_nSpaceHorz;
	INT32						m_nSpaceVert;
	INT32						m_nWidth;
	INT32						m_nHeight;
	INT32						m_nTextWidth;
	INT32						m_AlignX;
	INT32						m_AlignY;
	FONT_SHADOW_QUALITY			m_nShadowQuality;
	REAL32						m_fShadowOffSet;

	COLOR						m_TextColor;
	COLOR						m_TextColor_BackUp;
	COLOR						m_TextShadowColor;
	
	i3VertexArray *				m_pVA;

	INT32						m_nCurPosX;
	INT32						m_nCurPosY;

	INT32						m_VtxStride;

	INT32						m_idxDisplayStart;

	//
	DISP_CHAR_INFO *			m_pCharTable;

	DRAW_GROUP					m_DrawGroup[ MAX_DRAW_GROUP];
	INT32						m_DrawGroupCount;

	INT32						m_VisibleLineCount;

	//
	BOOL						m_bUseVertexBank;
	VERTEX_BANK_ELEMENT*		m_pVBElement;	

protected:
	DISP_CHAR_INFO *			_SetText( PARSE_CTX * pCtx, char* pszText, bool bParseCmd = true);
	bool						_ParseCommand( PARSE_CTX * pCtx, const char * szCmd);
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

	BOOL			Create( const char * pszFontType, INT32 nMaxCount = 256, INT32 nFontSize = 10, INT32 nFontWeight = FW_NORMAL, BOOL bItalic = FALSE, BOOL bUnderLine = FALSE, INT32 nQuality = DEFAULT_QUALITY,
							BOOL bShadowEnable = FALSE, FONT_SHADOW_QUALITY nShadowQuality = FONT_SHADOW_QUALITY_LOW, REAL32 shadowOffset = 1.0f, BOOL bUseVertexBank = FALSE);	
    		
	void			SetText( char* pszText, bool bParseCmd = true);
	char *			getText(void)							{ return m_pszText; }
	
	void			SetEnable( BOOL bEnable);
	void			SetPos(INT32 nX, INT32 nY);
	void			SetTextSpace( INT32 nX, INT32 nY);
	void			SetOffsetPos( REAL32 fX, REAL32 fY);	
	void			SetWidth(INT32 nWidth)	;
	void			SetHeight(INT32 nHeight);
	void			SetAlign( INT32 AlignX, INT32 AlignY );
	void			SetColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a);
	void			SetColor( COLOR * pColor);
	void			SetShadowColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a);
	void			SetShadowColor( COLOR * pColor);
	void			SetAutoWrapEnable( BOOL bEnable);

	// Caret°ü·Ã
	INT32			GetCaretPosX(INT32 nLength);
	INT32			GetCaretPosY(INT32 nLength);

	UINT32			getState(void)				{ return m_State; }
	void			addState( UINT32 mask)		{ m_State |= mask; }
	void			removeState( UINT32 mask)	{ m_State &= ~mask; }
	bool			isState( UINT32 mask)		{ return (m_State & mask) != 0; }

	BOOL			getEnable()					{ return isState( I3_TEXT_STATE_ENABLE); }

	void			setFontSkip(BOOL bFlag)		
	{
		if( bFlag)
			addState( I3_TEXT_STATE_SKIP);
		else
			removeState( I3_TEXT_STATE_SKIP);
	}

	INT32			getPosX(void)				{ return m_PosX;			}
	INT32			getPosY(void)				{ return m_PosY;			}
	INT32			getWidth(void)				{ return m_nWidth;			}
	INT32			getHeight(void)				{ return m_nHeight;			}
	INT32			getTextWidth(void)			{ return m_nTextWidth;		}
	INT32			getTextHeight(void)			{ return m_pFont->getGlyphHeight(); }
	INT32			getLineCount(void)			{ return m_LineCount;		}
	INT32			getVisibleLineCount(void)	{ return m_VisibleLineCount;}

	INT32			GetCharacterSizeX( char * pszText);

	void			AdjustOffset( REAL32 diffWidth);

	void			ConnectVertexBank( UINT32 nCount);
	
	virtual void	Apply( i3RenderContext * pContext);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

	#if defined( I3_DEBUG)
	virtual void Dump(void);
	#endif
};

#endif

#endif	//__I3_TEXT_ATTR_DX2_H
