// i3TextAttr.h
#ifndef __FONT_TEXT_ATTR_H__
#define __FONT_TEXT_ATTR_H__

#include "i3TextCommonDefine.h"
#include "i3RenderAttr.h"
#include "i3Font.h"

enum {
	TEXT_ATTR_ALIGN_LEFT,
	TEXT_ATTR_ALIGN_CENTER,
	TEXT_ATTR_ALIGN_RIGHT,
};

enum {
	TEXT_ATTR_ALIGN_TOP,
	TEXT_ATTR_ALIGN_MIDDLE,
	TEXT_ATTR_ALIGN_BOTTOM,
};

class I3_EXPORT_SCENE i3TextInfo : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3TextInfo, i3ElementBase );
protected:
	I3_GLYPH_INFO	m_GlyphInfo;
	i3Font			*m_pFont = nullptr;

	INT32			m_X = 0, m_Y = 0;
	REAL32			m_fScaleX = 1.0f;
	REAL32			m_fScaleY = 1.0f;

	WCHAR16			m_wChar = 0;
	COLOR			m_Color;
	
	bool			m_bEnable = true;

	bool			m_bProgressText = false;
	bool			m_bVisibleText = true;

	INT32			m_nJumpIndex = 0;

	// Effect
	bool			m_bEffect = false;
	REAL32		m_rElapsedTime = 0.0f;

public:
	bool Create( i3Font *pFont );

	void SetChar( WCHAR16 wCh );
	void SetColor( COLOR *pColor );
	COLOR getColor()								{ return m_Color;					}
	void SetPosition( INT32 x, INT32 y );
	void setEnable( bool bEnable)					{ m_bEnable = bEnable;				}
	bool getEnable()								{ return m_bEnable;					}
	void setVisible( bool bEnable)					{ m_bVisibleText = bEnable;			}
	bool getVisible()								{ return m_bVisibleText;			}
	void setJumpIndex( INT32 index)					{ m_nJumpIndex = index;				}
	INT32 getJumpIndex()							{ return m_nJumpIndex;				}
	void	SetScale( REAL32 fScaleX, REAL32 fScaleY)		{ m_fScaleX = fScaleX;		m_fScaleY = fScaleY;	}
	
	INT32 GetWidth(){ return m_GlyphInfo.nWidth; }
	INT32 GetHeight(){ return m_GlyphInfo.nHeight; }
	UINT32 GetTextureIndex(){ return m_GlyphInfo.nTextureIndex; }

	#if I3_GFXCAPS_TRANSFORMED_VERTEX
		void GetTextVertexArray( i3VertexArray *pVA, UINT32 nIndex );
	#else
		void GetTextVertexArray( i3VertexArray *pVA, UINT32 nIndex, REAL32 rcx, REAL32 rcy);
	#endif

	// Effect
	void SetEffect( INT32 nIndex, REAL32 rTime );
	bool UpdateEffect( REAL32 fDeltaSeconds );
	void GetEffectVertexArray( i3VertexArray *pVA, UINT32 &nIndex );
};

class I3_EXPORT_SCENE i3TextAttr : public i3RenderAttr
{
	I3_EXPORT_CLASS_DEFINE( i3TextAttr, i3RenderAttr );
protected:
	bool					m_TextEffect = false;
	bool					m_bChangingFontColor = false;

	UINT32					*m_pTextLineArray = nullptr;
	UINT32					m_nMaxLineArray = 0;

	UINT32					m_nWidth = 0;
	UINT32					m_nHeight = 0;

	UINT32					m_MaxCount = 0;
	UINT32					m_Count = 0;
	i3VertexArray *			m_pVA[2] = { nullptr, nullptr };

	UINT32					m_BufferIndex = 0;

	bool					m_bEdited = true;			// 수정 여부
	bool					m_bAlwaysUpdate = true;

	VEC2D				m_Position;
	COLOR					m_Color = { 255, 255, 255, 255 };
	COLOR					m_BackUpColor;

	bool					m_bEnable = false;

	i3Font					*m_pFont = nullptr;
	UINT32					m_nNumTexture = 0;

	UINT32					*m_pCharCount = nullptr;

	i3::vector<i3TextInfo*> m_TextInfoList;

	i3::wstring				m_wstrText;
	i3::wstring				m_wstrTempText;

	UINT32					m_AlignX = TEXT_ATTR_ALIGN_LEFT;
	UINT32					m_AlignY = TEXT_ATTR_ALIGN_TOP;

	bool					m_bAutoWrap = false;
	UINT32					m_MaxLineWidth = 0;

	UINT32					m_TotalWidth = 0;
	UINT32					m_TotalHeight = 0;

	INT32					m_HeightGap = 0;
	
	bool					m_bProgressText = false;
	bool					m_bProgressTextStart = false;
	REAL32					m_fProgressDeltaTime = 0.0f;
	REAL32					m_fProgressTextScrollSpeed = 0.0f;
	INT32					m_nProgressTextCurrentIndex = 0;

	bool					m_bWordLineFeed = true;

	// Volatile members
	i3Shader *				m_pDrawShader = nullptr;

	
protected:

	void UpdateVertexArray( i3RenderContext * pContext);

	/*void ApplyAlwaysUpdate( i3RenderContext * pContext );
	void ApplyEdittedUpdate( i3RenderContext * pContext );*/

public:
	i3TextAttr();
	virtual ~i3TextAttr();

	bool		Create( i3Font *pFont, UINT32 nMaxCount, bool bAlwaysUpdate = false, UINT32 nMaxLine = 45);

	i3Font *	getFont()				{ return m_pFont;		}

	void		SetSize( UINT32 width, UINT32 height ) { m_nWidth = width;	m_nHeight = height; }
	UINT32		GetWidth()		{ return m_nWidth; }
	UINT32		GetHeight()		{ return m_nHeight;}

	void		SetHeightGap( INT32 nGap )	{ m_HeightGap = nGap; }
	INT32		GetHeightGap()	{ return m_HeightGap; }

	UINT32		GetTotalWidth()	{ return m_TotalWidth; }
	UINT32		GetTotalHeight(){ return m_TotalHeight; }

	void		SetEnable( bool bFlag );
	bool		GetEnable()					{ return m_bEnable; }

	void		SetAlign( UINT32 AlignX, UINT32 AlignY );
	void		SetAutoWrap( bool bTrue, UINT32 nMaxWidth );

	void		SetPos( VEC2D *pPos );
	void		SetPos( REAL32 x, REAL32 y );
	VEC2D *	GetPos()					{ return &m_Position; }

	void		SetColor( COLOR * pColor);
	void		SetColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a);
	COLOR *		GetColor()					{ return &m_Color; }

	void		SetScale( REAL32 fScaleX, REAL32 fScaleY);

	bool		SetText( const i3::wliteral_range& wTextRng );
	const i3::wstring&	GetText() const { return m_wstrText; }

	void		UpdateTextInfo();

	void		setWordLineFeed( bool bTrue)				{ m_bWordLineFeed = bTrue;								}
	bool		getWordLineFeed()							{ return m_bWordLineFeed;								}

	void		setProgressText( bool bEnable)				{ m_bProgressText = bEnable;							}
	bool		getProgressText()							{ return m_bProgressText;								}
    	
	INT32		getTextCount()								{ return (INT32)m_Count;	 							}
	i3TextInfo* getTextInfoByIndex( INT32 index)			{ return m_TextInfoList[index];	}

	void		ProcessProgressText( REAL32 rDeltaSeconds);
	void		StartProgressText( bool bStart);
	void		setProgressTextScrollSpeed( REAL32 fSpeed)		{ m_fProgressTextScrollSpeed = fSpeed;	}
	void		ResetProgressText();
	void		SetImmediatelyProgressText();


	virtual	void	Apply( i3RenderContext * pContext ) override;
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;

	virtual UINT32	OnSave( i3ResourceFile * pResFile ) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile ) override;
};

#endif //__FONT_TEXT_ATTR_H__
