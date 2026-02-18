#ifndef __I3_TEXT_NODE_DX_H__
#define __I3_TEXT_NODE_DX_H__

#if defined (I3G_DX)

#include "i3Gfx.h"
#include "i3AttrSet.h"
#include "i3TextAttrDX.h"
#include "i3Geometry.h"

class I3_EXPORT_SCENE i3TextNodeDX : public i3AttrSet
{
	I3_CLASS_DEFINE( i3TextNodeDX);

protected:
	i3TextAttrDX*			m_pTextAttrDX;
	i3Geometry*				m_pGeomery;

public:
	i3TextNodeDX(void);
	virtual ~i3TextNodeDX(void);

	BOOL		Create(  const char* pszFont, INT32 nTextMaxCount, INT32 nSize = 9, UINT32 nWeight = FW_NORMAL, BOOL bItalic = FALSE, DWORD dwQuality = DEFAULT_QUALITY);

	i3TextAttrDX* GetTextAttrDX()										{ return m_pTextAttrDX;								}
	void		SetEnable( BOOL bFlag)									{ m_pTextAttrDX->setEnable( bFlag);					}
	BOOL		GetEnable()												{ return m_pTextAttrDX->getEnable();				}

	void		SetText( char* pszText)									{ m_pTextAttrDX->SetText( pszText);					}
	char*		getText(void)											{ return m_pTextAttrDX->getText();					}
	void		setPos(UINT32 nX, UINT32 nY)							{ m_pTextAttrDX->setPos( nX, nY);					}
	void		setWidth(UINT32 nWidth)									{ m_pTextAttrDX->setWidth( nWidth);					}
	void		setHeight(UINT32 nHeight)								{ m_pTextAttrDX->setHeight( nHeight);				}
	void		setSize( UINT32 nWidth, UINT32 nHeight)					{ m_pTextAttrDX->setWidth( nWidth); m_pTextAttrDX->setHeight( nHeight);	}
	void		SetAlign( UINT32 AlignX, UINT32 AlignY )				{ m_pTextAttrDX->SetAlign( AlignX, AlignY );		}
	void		SetColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a)				{ m_pTextAttrDX->SetColor( r, g, b, a );			}
	void		SetColor( COLOR * pColor)								{ m_pTextAttrDX->SetColor( pColor );				}
	void		SetShadowColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a)		{ m_pTextAttrDX->SetShadowColor( r, g, b, a );		}
	void		SetShadowColor( COLOR * pColor)							{ m_pTextAttrDX->SetShadowColor( pColor);			}	
	void		setAutoWrapEnable( BOOL bEnable)						{ m_pTextAttrDX->setAutoWrapEnable( bEnable);		}
	void		setShadowEnable(BOOL bEnable, FONT_SHADOW_QUALITY nQuality = FONT_SHADOW_QUALITY_LOW)	{ m_pTextAttrDX->setShadowEnable( bEnable, nQuality);			}

	// Caret°ü·Ã
	INT32		getCaretPosX( INT32 nLength = 0)						{ return m_pTextAttrDX->GetCaretPosX( nLength);		}
	INT32		getCharacterSizeX( char *pszText)						{ return m_pTextAttrDX->GetCharacterSizeX(pszText);	}
	INT32		getTextWidth(void)										{ return m_pTextAttrDX->getTextWidth();				}
	
	INT32		getPosX(void)											{ return m_pTextAttrDX->getPosX();					}
	INT32		getPosY(void)											{ return m_pTextAttrDX->getPosY();					}
	VEC2D *		getPos(void)											{ return m_pTextAttrDX->GetPos();					}
	INT32		getWidth(void)											{ return m_pTextAttrDX->getWidth();					}
	INT32		getHeight(void)											{ return m_pTextAttrDX->getHeight();				}
};

#endif

#endif // __I3_TEXT_NODE_DX_H__
