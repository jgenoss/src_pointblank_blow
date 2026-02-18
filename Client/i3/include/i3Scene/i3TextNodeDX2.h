#ifndef __I3_TEXT_NODE_DX2_H__
#define __I3_TEXT_NODE_DX2_H__

#if defined (I3G_DX)

#include "i3Gfx.h"
#include "i3AttrSet.h"
#include "i3TextAttrDX2.h"
#include "i3Geometry.h"
#include "i3Sprite2D.h"

class I3_EXPORT_SCENE	i3TextFontSetInfo
{
public:
	i3TextFontSetInfo() {}
	i3TextFontSetInfo(const char * FontName, UINT32 CharSet = DEFAULT_CHARSET);

	const char *	GetFontName() const			{ return m_FontName; }
	void			SetFontName(const char * FontName)	{ i3::safe_string_copy(m_FontName, FontName, 256); }
	UINT32			GetCharset() const			{ return m_CharsSet; }
	void			SetCharset(UINT32 CharSet)	{ m_CharsSet = CharSet; }
private:
	char	m_FontName[256] = { 0 };
	UINT32	m_CharsSet = DEFAULT_CHARSET;
};


class I3_EXPORT_SCENE i3TextNodeDX2 : public i3AttrSet
{
	I3_EXPORT_CLASS_DEFINE( i3TextNodeDX2, i3AttrSet);

protected:
	i3TextAttrDX2*			m_pTextAttrDX = nullptr;
	i3Geometry*				m_pGeomery = nullptr;	
	i3AttrSet*				m_pAttrSet = nullptr;

public:
	virtual ~i3TextNodeDX2(void);

	bool		Create( const i3TextFontSetInfo * pFontSetInfo, INT32 nTextMaxCount = 256, INT32 nSize = 10, UINT32 nWeight = FW_NORMAL, bool bItalic = false, bool bUnderLine = false, INT32 nQuality = DEFAULT_QUALITY,
		bool bEnable = false, FONT_SHADOW_QUALITY nShadowQuality = FONT_SHADOW_QUALITY_LOW, REAL32 OffSet = 1.0f, bool bUseVertexBank = false, bool bTransformedVertex = true);
	bool		Create( const char* pszFont, INT32 nTextMaxCount = 256, INT32 nSize = 10, UINT32 nWeight = FW_NORMAL, bool bItalic = false, bool bUnderLine = false, INT32 nQuality = DEFAULT_QUALITY,
						bool bEnable = false, FONT_SHADOW_QUALITY nShadowQuality = FONT_SHADOW_QUALITY_LOW, REAL32 OffSet = 1.0f, bool bUseVertexBank = false, bool bTransformedVertex = true);
	void		SetTextEllipsis(const i3::wliteral_range& wTextRng);

	void			SetText( const i3::wliteral_range& wTextRng, bool bParseCmd = true)		{	m_pTextAttrDX->SetText( wTextRng, bParseCmd);	}							

//	{ 
//#ifdef I3_DEBUG
//		SetName(pszText);			// 이 기능은 포기한다..
//#endif
//		m_pTextAttrDX->SetText( pszText, bParseCmd);	
//	}	

	const wchar_t*	getText(void)	const										{ return m_pTextAttrDX->getText();				}
	const i3::wstring& getTextString() const									{ return m_pTextAttrDX->getTextString();        }
	void		SetEnable( bool bFlag)
	{
		if( bFlag)
			RemoveFlag( I3_NODEFLAG_DISABLE);
		else
			AddFlag( I3_NODEFLAG_DISABLE);
	}

	i3GfxFontDX2*getFont(void)																			{ return m_pTextAttrDX->getFont();}

	bool		GetEnable(void)																			{ return IsFlag( I3_NODEFLAG_DISABLE) == false; }

	void		setPos(UINT32 nX, UINT32 nY)															{ m_pTextAttrDX->SetPos( nX, nY);					}
	void		getPos( VEC2D * pVec)																	{ i3Vector::Set( pVec, (REAL32) m_pTextAttrDX->getPosX(), (REAL32) m_pTextAttrDX->getPosY()); }
	void		SetOffsetPos( REAL32 fX, REAL32 fY)														{ m_pTextAttrDX->SetOffsetPos( fX, fY);				}
	void		setWidth(UINT32 nWidth)																	{ m_pTextAttrDX->SetWidth( nWidth);					}
	void		setHeight(UINT32 nHeight)																{ m_pTextAttrDX->SetHeight( nHeight);				}
	void		SetTextSpace( INT32 nX, INT32 nY)														{ m_pTextAttrDX->SetTextSpace( nX, nY);				}
	void		setSize( UINT32 nWidth, UINT32 nHeight)													{ m_pTextAttrDX->SetWidth( nWidth); m_pTextAttrDX->SetHeight( nHeight);	}
	void		SetAlign( UINT32 AlignX, UINT32 AlignY )												{ m_pTextAttrDX->SetAlign( AlignX, AlignY );		}
	void		SetColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a)												{ m_pTextAttrDX->SetColor( r, g, b, a );			}
	void		SetColor( COLOR * pColor)																{ m_pTextAttrDX->SetColor( pColor );				}
	void		SetAlpha( UINT8 nAlpha)																	{ m_pTextAttrDX->SetAlpha( nAlpha);					}
	void		SetShadowColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a)										{ m_pTextAttrDX->SetShadowColor( r, g, b, a );		}
	void		SetShadowColor( COLOR * pColor)															{ m_pTextAttrDX->SetShadowColor( pColor);			}	
	void		SetShadowAlpha( UINT8 nAlpha)															{ m_pTextAttrDX->SetShadowAlpha( nAlpha);			}
	void		setAutoWrapEnable( bool bEnable)														{ m_pTextAttrDX->SetAutoWrapEnable( bEnable);		}
	void		setFontSkip(bool bFlag)																	{ m_pTextAttrDX->setFontSkip( bFlag);				}
	void		setOutCharacterProcess(bool bFlag)														{ m_pTextAttrDX->setOutCharaProcess( bFlag);		}
	i3TextAttrDX2*	getTextAttrDX(void)																	{ return m_pTextAttrDX;								}
	UINT32		getState(void)																			{ return m_pTextAttrDX->getState();					}
	bool		isState( UINT32 mask)																	{ return m_pTextAttrDX->isState(mask);				}

	// Caret관련
	INT32		getCaretPosX( INT32 nLength = 0)														{ return m_pTextAttrDX->GetCaretPosX( nLength);		}
	INT32		GetCaretPosY( INT32 nLength = 0)														{ return m_pTextAttrDX->GetCaretPosY( nLength);		}
	INT32		getCharacterSizeX( const i3::wliteral_range& wTextRng)	const							{ return m_pTextAttrDX->GetCharacterSizeX(wTextRng);	}
	INT32		CalcTextWidth( const i3::wliteral_range& wTextRng)	const								{ return m_pTextAttrDX->CalcTextWidth( wTextRng);   }
	INT32		getTextWidth(void)																		{ return m_pTextAttrDX->getTextWidth();				}
	INT32		getTextHeight(void)																		{ return m_pTextAttrDX->getTextHeight();			}
	INT32		getLineCount(void)																		{ return m_pTextAttrDX->getLineCount();				}

	INT32		getPosX(void)																			{ return m_pTextAttrDX->getPosX();					}
	INT32		getPosY(void)																			{ return m_pTextAttrDX->getPosY();					}
	INT32		getWidth(void)																			{ return m_pTextAttrDX->getWidth();					}
	INT32		getHeight(void)																			{ return m_pTextAttrDX->getHeight();				}

	void		AdjustOffset( REAL32 diffWidth)															{ m_pTextAttrDX->AdjustOffset( diffWidth);			}
};

#endif

#endif // __I3_TEXT_NODE_DX2_H__
