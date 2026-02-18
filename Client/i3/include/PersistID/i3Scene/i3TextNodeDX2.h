#ifndef __I3_TEXT_NODE_DX2_H__
#define __I3_TEXT_NODE_DX2_H__

#if defined (I3G_DX)

#include "i3Gfx.h"
#include "i3AttrSet.h"
#include "i3TextAttrDX2.h"
#include "i3Geometry.h"
#include "i3Sprite2D.h"

class I3_EXPORT_SCENE i3TextNodeDX2 : public i3AttrSet
{
	I3_CLASS_DEFINE( i3TextNodeDX2);

protected:
	i3TextAttrDX2*			m_pTextAttrDX;
	i3Geometry*				m_pGeomery;	
	i3AttrSet*				m_pAttrSet;

public:
	i3TextNodeDX2(void);
	virtual ~i3TextNodeDX2(void);

	BOOL		Create( const char* pszFont, INT32 nTextMaxCount = 256, INT32 nSize = 10, UINT32 nWeight = FW_NORMAL, BOOL bItalic = FALSE, BOOL bUnderLine = FALSE, INT32 nQuality = DEFAULT_QUALITY,
						BOOL bEnable = FALSE, FONT_SHADOW_QUALITY nShadowQuality = FONT_SHADOW_QUALITY_LOW, REAL32 OffSet = 1.0f, BOOL bUseVertexBank = FALSE);
	void		SetTextEllipsis(char* pszText);

	void		SetText( char* pszText, bool bParseCmd = true)											{ m_pTextAttrDX->SetText( pszText, bParseCmd);	}	
	char*		getText(void)																			{ return m_pTextAttrDX->getText();				}

	void		SetEnable( BOOL bFlag)
	{
		if( bFlag)
			RemoveFlag( I3_NODEFLAG_DISABLE);
		else
			AddFlag( I3_NODEFLAG_DISABLE);
	}

	BOOL		GetEnable(void)																			{ return IsFlag( I3_NODEFLAG_DISABLE) == FALSE; }

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
	void		SetShadowColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a)										{ m_pTextAttrDX->SetShadowColor( r, g, b, a );		}
	void		SetShadowColor( COLOR * pColor)															{ m_pTextAttrDX->SetShadowColor( pColor);			}	
	void		setAutoWrapEnable( BOOL bEnable)														{ m_pTextAttrDX->SetAutoWrapEnable( bEnable);		}
	void		setFontSkip(BOOL bFlag)																	{ m_pTextAttrDX->setFontSkip( bFlag);				}
	i3TextAttrDX2*	getTextAttrDX(void)																	{ return m_pTextAttrDX;								}
	UINT32		getState(void)																			{ return m_pTextAttrDX->getState();					}
	bool		isState( UINT32 mask)																	{ return m_pTextAttrDX->isState(mask);				}

	// Caret°ü·Ã
	INT32		getCaretPosX( INT32 nLength = 0)														{ return m_pTextAttrDX->GetCaretPosX( nLength);		}
	INT32		GetCaretPosY( INT32 nLength = 0)														{ return m_pTextAttrDX->GetCaretPosY( nLength);		}
	INT32		getCharacterSizeX( char *pszText)														{ return m_pTextAttrDX->GetCharacterSizeX(pszText);	}
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
