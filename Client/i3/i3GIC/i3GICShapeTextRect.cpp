#include "stdafx.h"
#include "i3GICShapeTextRect.h"

#if defined( I3_DEBUG)

#include "i3GICDiagramContext.h"
#include "i3GICProperty.h"
#include "i3GICColorProperty.h"
#include "i3GICFontProperty.h"

I3_CLASS_INSTANCE( i3GICShapeTextRect);

i3GICShapeTextRect::i3GICShapeTextRect()
{
	memset( &m_logFont, 0, sizeof( m_logFont));
	
	m_logFont.lfHeight = m_uSavedStretchH;
	m_logFont.lfWeight = FW_NORMAL;
	m_logFont.lfCharSet = DEFAULT_CHARSET;
	m_logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	m_logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	m_logFont.lfQuality = DEFAULT_QUALITY;
	m_logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	
	setFont( "Arial");

	setStyle( I3GIC_STYLE_TEXT);
}

i3GICShapeTextRect::~i3GICShapeTextRect()
{
	_DeleteGDIFontObject();
}

void i3GICShapeTextRect::_CreateGDIFontObject( void)
{
	I3ASSERT( m_hFont == nullptr);
	m_hFont = ::CreateFontIndirect( &m_logFont);
	I3ASSERT( m_hFont != nullptr);
}

void i3GICShapeTextRect::_DeleteGDIFontObject( void)
{
	if( m_hFont != nullptr)
	{
		::DeleteObject( m_hFont);
		m_hFont = nullptr;
	}
}

void i3GICShapeTextRect::_CalcStretchRect( void)
{
	if( m_logFont.lfHeight != getHeight())
	{
		m_logFont.lfHeight = (INT32) getHeight();

		if( isBindDiagram())
		{
			_DeleteGDIFontObject();
			_CreateGDIFontObject();
		}
	}
}

/*virtual*/ void i3GICShapeTextRect::OnDraw( i3GICDiagramContext * pCtx)
{
	i3GICShapeRect::OnDraw( pCtx);

	RECT rect;

	if( !isStyle( I3GIC_STYLE_INVISIBLE) &&
		( m_hFont != nullptr) )
	{
		rect.left = (INT32) getPosX();
		rect.top = (INT32) getPosY();
		rect.right = (INT32) (getPosX() + getWidth());
		rect.bottom = (INT32) (getPosY() + getHeight());

		pCtx->selectObject( m_hFont);

		pCtx->setTextColor( m_FontColor);
		pCtx->setBkColor( m_FontBkColor);

		if( isBkOpaque() )		pCtx->setBkMode( OPAQUE);
		else					pCtx->setBkMode( TRANSPARENT);

		if( m_bStretchToRect)
			_CalcStretchRect();

		pCtx->drawText( m_szText, &rect, DT_END_ELLIPSIS | DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
}

/*virtual*/ void i3GICShapeTextRect::OnBindDiagram( bool bBind)
{
	i3GICShapeRect::OnBindDiagram( bBind);

	if( bBind)	_CreateGDIFontObject();
	else		_DeleteGDIFontObject();
}

/*virtual*/ void i3GICShapeTextRect::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GICShapeRect::CopyTo( pDest, method);

	I3ASSERT( i3::kind_of<i3GICShapeTextRect*>(pDest));

	i3GICShapeTextRect * pDestShape = (i3GICShapeTextRect*) pDest;

	pDestShape->setText( m_szText);

	memcpy( pDestShape->getLogFont(), &m_logFont, sizeof( m_logFont));

	pDestShape->setFontColor( m_FontColor);
	pDestShape->setFontBkColor( m_FontBkColor);

	pDestShape->setStretchMode( m_bStretchToRect);
}


#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct GICSHAPE_TEXT_INFO
	{
		INT8	m_ID[4] = { 'G', 'T', 'R', '1' };
		char	m_szText[260] = { 0 };
		UINT32	m_Color = 0;
		UINT32	m_BkColor = 0;
		LOGFONTA	m_Font;
		UINT32	m_BkMode = 0;
		UINT32	pad[31] = { 0 };

		GICSHAPE_TEXT_INFO()
		{
			i3mem::FillZero(&m_Font, sizeof(m_Font));
		}
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

/*virtual*/ UINT32 i3GICShapeTextRect::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	pack::GICSHAPE_TEXT_INFO	info;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3GICShapeRect::OnSave( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	memcpy( info.m_szText, m_szText, sizeof( info.m_szText));
	info.m_Color = (UINT32) m_FontColor;
	info.m_BkColor = (UINT32) m_FontBkColor;
	memcpy( &info.m_Font, &m_logFont, sizeof( m_logFont));
	info.m_BkMode = (UINT32) m_bBkFill;

	Rc = pStream->Write( &info, sizeof( info));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

/*virtual*/ UINT32 i3GICShapeTextRect::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	pack::GICSHAPE_TEXT_INFO	info;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3GICShapeRect::OnLoad( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	Rc = pStream->Read( &info, sizeof( info));
	I3_CHKIO( Rc);
	Result += Rc;
	
	setText( (const char *) info.m_szText);
	m_FontColor = (COLORREF) info.m_Color;
	m_FontBkColor = (COLORREF) info.m_BkColor;
	memcpy( &m_logFont, &info.m_Font, sizeof( m_logFont));

	m_bBkFill = (bool) (info.m_BkMode > 0);

	return Result;
}

/*virtual*/ bool i3GICShapeTextRect::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3GICShapeRect::OnSaveXML( pFile, pXML) == false)
		return false;

	pXML->addAttr( "Text", m_szText );
	pXML->addAttr( "Font", m_logFont.lfFaceName );
	pXML->addAttr( "FontSize", m_logFont.lfHeight);
	pXML->addAttr( "FontColor", (UINT32) m_FontColor);
	pXML->addAttr( "FontBkColor", (UINT32) m_FontBkColor);
	pXML->addAttr( "FontUnderline", (UINT32) m_logFont.lfUnderline);
	pXML->addAttr( "FontItalic", (UINT32) m_logFont.lfItalic);
	pXML->addAttr( "FontWeight", (UINT32) m_logFont.lfWeight);
	pXML->addAttr( "FontBkMode", (UINT32) m_bBkFill);
	

	return true;
}

/*virtual*/ bool i3GICShapeTextRect::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3GICShapeRect::OnLoadXML( pFile, pXML) == false)
		return false;

	char szTemp[MAX_PATH];

	i3::safe_string_copy( szTemp, pXML->getAttr( "Text"), sizeof( szTemp));

	if( szTemp[0] != 0)
	{
		setText( szTemp);
	}

	i3::safe_string_copy( szTemp, pXML->getAttr( "Font"), sizeof( szTemp));

	if( szTemp[0] != 0)
	{
		setFont( szTemp);
	}

	pXML->getAttr( "FontSize", (INT32*) &m_logFont.lfHeight);
	pXML->getAttr( "FontColor", (UINT32*) &m_FontColor);
	pXML->getAttr( "FontBkColor", (UINT32*) &m_FontBkColor);
	pXML->getAttr( "FontUnderline", (UINT32*) &m_logFont.lfUnderline);
	pXML->getAttr( "FontItalic", (UINT32*) &m_logFont.lfItalic);
	pXML->getAttr( "FontWeight", (UINT32*) &m_logFont.lfWeight);
	pXML->getAttr( "FontBkMode", (UINT32*) &m_bBkFill);

	return true;
}

/*virtual*/ void i3GICShapeTextRect::OnProperty( CMFCPropertyGridCtrl * pCtrl)
{
	i3GICShapeRect::OnProperty( pCtrl);

	
	// String
	COleVariant varText( m_szText);
	i3GICProperty * pPropText = new i3GICProperty( _T( "Text Box"), varText);
	pPropText->setShape( this);
	pPropText->setUpdateType( GIC_PROPERTY_UPDATE_TEXT);

	pCtrl->AddProperty( pPropText);
}

#endif	// I3_DEBUG
