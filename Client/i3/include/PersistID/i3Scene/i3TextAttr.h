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
	I3_CLASS_DEFINE( i3TextInfo );
protected:
	i3Font			*m_pFont;

	INT32			m_X, m_Y;
	REAL32			m_fScaleX;
	REAL32			m_fScaleY;

	I3_GLYPH_INFO	m_GlyphInfo;

	WCHAR16			m_wChar;
	COLOR			m_Color;
	
	BOOL			m_bEnable;

	BOOL			m_bProgressText;
	BOOL			m_bVisibleText;

	INT32			m_nJumpIndex;

	// Effect
	BOOL			m_bEffect;
	RT_REAL32		m_rElapsedTime;

public:
	i3TextInfo();
	~i3TextInfo();

	BOOL Create( i3Font *pFont );

	void SetChar( WCHAR16 wCh );
	void SetColor( COLOR *pColor );
	COLOR getColor()								{ return m_Color;					}
	void SetPosition( INT32 x, INT32 y );
	void setEnable( BOOL bEnable)					{ m_bEnable = bEnable;				}
	BOOL getEnable()								{ return m_bEnable;					}
	void setVisible( BOOL bEnable)					{ m_bVisibleText = bEnable;			}
	BOOL getVisible()								{ return m_bVisibleText;			}
	void setJumpIndex( INT32 index)					{ m_nJumpIndex = index;				}
	INT32 getJumpIndex()							{ return m_nJumpIndex;				}
	void	SetScale( REAL32 fScaleX, REAL32 fScaleY)		{ m_fScaleX = fScaleX;		m_fScaleY = fScaleY;	}
	
	//void SetScale();
	//void SetRatation();

	INT32 GetWidth(){ return m_GlyphInfo.nWidth; }
	INT32 GetHeight(){ return m_GlyphInfo.nHeight; }
	UINT32 GetTextureIndex(){ return m_GlyphInfo.nTextureIndex; }

	#if I3_GFXCAPS_TRANSFORMED_VERTEX
		void GetTextVertexArray( i3VertexArray *pVA, UINT32 nIndex );
	#else
		void GetTextVertexArray( i3VertexArray *pVA, UINT32 nIndex, RT_REAL32 rcx, RT_REAL32 rcy);
	#endif

	// Effect
	void SetEffect( INT32 nIndex, REAL32 rTime );
	BOOL UpdateEffect( RT_REAL32 fDeltaSeconds );
	void GetEffectVertexArray( i3VertexArray *pVA, UINT32 &nIndex );
};

class I3_EXPORT_SCENE i3TextAttr : public i3RenderAttr
{
	I3_CLASS_DEFINE( i3TextAttr );
protected:
	BOOL					m_TextEffect;
	BOOL					m_bChangingFontColor;

	UINT32					*m_pTextLineArray;
	UINT32					m_nMaxLineArray;

	UINT32					m_nWidth;
	UINT32					m_nHeight;

	UINT32					m_MaxCount;
	UINT32					m_Count;
	i3VertexArray *			m_pVA[2];

	UINT32					m_BufferIndex;

	BOOL					m_bEdited;			// 수정 여부
	BOOL					m_bAlwaysUpdate;

	RT_VEC2D				m_Position;
	RT_VEC2D				m_UV[2];
	COLOR					m_Color;
	COLOR					m_BackUpColor;

	BOOL					m_bEnable;

	UINT32					m_GlyphWidth;
	UINT32					m_GlyphHeight;

	i3Font					*m_pFont;
	UINT32					m_nNumTexture;

	UINT32					*m_pCharCount;
//	I3HMEM					m_hCharCountMemID;

	i3List					*m_pTextInfoList;

	WCHAR16					*m_pText;
	WCHAR16					*m_pTempText;	

	UINT32					m_AlignX;
	UINT32					m_AlignY;

	BOOL					m_bAutoWrap;
	UINT32					m_MaxLineWidth;

	UINT32					m_TotalWidth;
	UINT32					m_TotalHeight;

	INT32					m_HeightGap;
	
	BOOL					m_bProgressText;
	BOOL					m_bProgressTextStart;
	REAL32					m_fProgressDeltaTime;
	REAL32					m_fProgressTextScrollSpeed;
	INT32					m_nProgressTextCurrentIndex;

	BOOL					m_bWordLineFeed;

	
protected:

	void UpdateVertexArray( i3RenderContext * pContext);

	void ApplyAlwaysUpdate( i3RenderContext * pContext );
	void ApplyEdittedUpdate( i3RenderContext * pContext );

public:
	i3TextAttr();
	virtual ~i3TextAttr();

	BOOL		Create( i3Font *pFont, UINT32 nMaxCount, BOOL bAlwaysUpdate = FALSE, UINT32 nMaxLine = 45);

	i3Font *	getFont()				{ return m_pFont;		}

	void		SetSize( UINT32 width, UINT32 height ) { m_nWidth = width;	m_nHeight = height; }
	UINT32		GetWidth()		{ return m_nWidth; }
	UINT32		GetHeight()		{ return m_nHeight;}

	void		SetHeightGap( INT32 nGap )	{ m_HeightGap = nGap; }
	INT32		GetHeightGap()	{ return m_HeightGap; }

	UINT32		GetTotalWidth()	{ return m_TotalWidth; }
	UINT32		GetTotalHeight(){ return m_TotalHeight; }

	void		SetEnable( BOOL bFlag );
	BOOL		GetEnable()					{ return m_bEnable; }

	void		SetAlign( UINT32 AlignX, UINT32 AlignY );
	void		SetAutoWrap( BOOL bTrue, UINT32 nMaxWidth );

	void		SetPos( RT_VEC2D *pPos );
	void		SetPos( RT_REAL32 x, RT_REAL32 y );
	RT_VEC2D *	GetPos()					{ return &m_Position; }

	void		SetColor( COLOR * pColor);
	void		SetColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a);
	COLOR *		GetColor()					{ return &m_Color; }

	void		SetScale( REAL32 fScaleX, REAL32 fScaleY);

	BOOL		SetText( const char *szText );
	BOOL		SetText( const WCHAR16 *pText );
	WCHAR16*	GetText() { return m_pText; }
	void		UpdateTextInfo();

	void		setWordLineFeed( BOOL bTrue)				{ m_bWordLineFeed = bTrue;								}
	BOOL		getWordLineFeed()							{ return m_bWordLineFeed;								}

	void		setProgressText( BOOL bEnable)				{ m_bProgressText = bEnable;							}
	BOOL		getProgressText()							{ return m_bProgressText;								}
    	
	INT32		getTextCount()								{ return (INT32)m_Count;	 							}
	i3TextInfo* getTextInfoByIndex( INT32 index)			{ return (i3TextInfo *)m_pTextInfoList->GetItem(index);	}

	void		ProcessProgressText( REAL32 rDeltaSeconds);
	void		StartProgressText( BOOL bStart);
	void		setProgressTextScrollSpeed( REAL32 fSpeed)		{ m_fProgressTextScrollSpeed = fSpeed;	}
	void		ResetProgressText();
	void		SetImmediatelyProgressText();


	virtual	void	Apply( i3RenderContext * pContext );
	virtual void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method);
	virtual BOOL	IsSame( i3RenderAttr * pAttr );

	virtual UINT32	OnSave( i3ResourceFile * pResFile );
	virtual UINT32	OnLoad( i3ResourceFile * pResFile );
};

#endif //__FONT_TEXT_ATTR_H__
