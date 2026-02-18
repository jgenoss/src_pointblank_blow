#ifndef __I3_TEXT_NODE_H__
#define __I3_TEXT_NODE_H__

#include "i3Gfx.h"
#include "i3AttrSet.h"
#include "i3TextureBindAttr.h"
#include "i3BlendEnableAttr.h"
#include "i3BlendModeAttr.h"
#include "i3TextureWrapAttr.h"
#include "i3AlphaTestEnableAttr.h"
#include "i3TextAttr.h"
#include "i3Geometry.h"

//
// i3TextAttr을 손쉽게 쓰기위해서 SceneGraph를 미리 구성하여둔다.
// 
//		i3TextNode
//			|
//			+- i3BlendModeAttr
//			|
//			+- i3TextureBindAttr
//			|
//			+- i3Geometry
//				|
//				+- i3TextAttr
//		


class I3_EXPORT_SCENE i3TextNode : public i3AttrSet
{
	I3_CLASS_DEFINE( i3TextNode);
protected:

	I3G_BLEND				m_SrcBlend;
	I3G_BLEND				m_DestBlend;

	i3TextAttr*				m_pTextAttr;
	i3Geometry*				m_pGeomery;
	i3BlendEnableAttr *		m_pBlendEnableAttr;
	i3BlendModeAttr*		m_pBlendModeAttr;
	i3AlphaTestEnableAttr *	m_pAlphaTestAttr;

public:
	i3TextNode(void);
	virtual ~i3TextNode(void);

	BOOL		Create( i3Font *pFont, UINT32 nMaxCount, BOOL bAlwaysUpdate = FALSE );

	i3TextAttr* GetTextAttr(){ return m_pTextAttr;	}

	void		SetEnable( BOOL bFlag)									{ m_pTextAttr->SetEnable( bFlag);}
	BOOL		GetEnable()												{ return m_pTextAttr->GetEnable();}

	void		SetAlign( UINT32 AlignX, UINT32 AlignY )				{ m_pTextAttr->SetAlign( AlignX, AlignY );	}
	void		SetAutoWrap( BOOL bTrue, UINT32 nMaxWidth )				{ m_pTextAttr->SetAutoWrap( bTrue, nMaxWidth );	}

	void		SetPos( RT_VEC2D *pPos )								{ m_pTextAttr->SetPos( pPos );	}
	void		SetPos( RT_REAL32 x, RT_REAL32 y )						{ m_pTextAttr->SetPos( x, y );	}
	RT_VEC2D *	GetPos()												{ return m_pTextAttr->GetPos(); }

	void		SetScale( REAL32 fScaleX, REAL32 fScaleY)		{ m_pTextAttr->SetScale( fScaleX, fScaleY);	}

	void		SetSize( UINT32 width, UINT32 height )					{ m_pTextAttr->SetSize( width, height ); }
	UINT32		GetWidth()												{ return m_pTextAttr->GetWidth(); }
	UINT32		GetHeight()												{ return m_pTextAttr->GetHeight();}
	UINT32		GetTotalWidth()											{ return m_pTextAttr->GetTotalWidth(); }
	UINT32		GetTotalHeight()										{ return m_pTextAttr->GetTotalHeight(); }

	void		SetHeightGap( INT32 nGap )								{ m_pTextAttr->SetHeightGap( nGap ); }
	INT32		GetHeightGap()											{ return m_pTextAttr->GetHeightGap(); }

	void		SetColor( COLOR * pColor)								{ m_pTextAttr->SetColor( pColor );	}
	void		SetColor( UINT8 r,UINT8 g,UINT8 b,UINT8 a)				{ m_pTextAttr->SetColor( r, g, b, a );	}
	COLOR *		GetColor()												{ return m_pTextAttr->GetColor(); }

	BOOL		SetText( const char *szText )							{ return m_pTextAttr->SetText( szText );	}
	BOOL		SetText( const WCHAR16 *pText )							{ return m_pTextAttr->SetText( pText );	}
	WCHAR16 *	GetText()												{ return m_pTextAttr->GetText();		}

	void		setWordLineFeed( BOOL bEnable)							{ m_pTextAttr->setWordLineFeed( bEnable);	}
	BOOL		getWordLineFeed()										{ return m_pTextAttr->getWordLineFeed();	}

	void		setProgressText( BOOL bEnable)							{ m_pTextAttr->setProgressText( bEnable);	}
	BOOL		getProgressText()										{ return m_pTextAttr->getProgressText();	}

	INT32		getTextCount()											{ return m_pTextAttr->getTextCount();		}
	i3TextInfo* getTextInfoByIndex( INT32 index)						{ return m_pTextAttr->getTextInfoByIndex(index);	}

	void		SetBlendEnable( BOOL bTrue = TRUE );
	void		SetSrcBlend( I3G_BLEND blend);
	I3G_BLEND	GetSrcBlend(void)	{ return m_SrcBlend; }

	void		SetDestBlend( I3G_BLEND blend);
	I3G_BLEND	GetDestBlend(void)	{ return m_DestBlend; }
	
	void		StartProgressText( BOOL bStart)							{ m_pTextAttr->StartProgressText(bStart); }
	void		setProgressTextScrollSpeed( REAL32 fSpeed)				{ m_pTextAttr->setProgressTextScrollSpeed(fSpeed); }
	void		ResetProgressText()										{ m_pTextAttr->ResetProgressText();		}
	void		SetImmediatelyProgressText()							{ m_pTextAttr->SetImmediatelyProgressText();	}

	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
