#include "stdafx.h"
#include "i3GICShapeRectBar.h"
#include "i3GICProperty.h"
#include "i3GICDiagramContext.h"
#include "i3GICColorProperty.h"

#if defined( I3_DEBUG)

I3_CLASS_INSTANCE( i3GICShapeRectBar);

i3GICShapeRectBar::i3GICShapeRectBar()
{
	setStyle( I3GIC_STYLE_RECTBAR);
}

i3GICShapeRectBar::~i3GICShapeRectBar()
{
	_DeleteGDIBarObject();
}

void i3GICShapeRectBar::_CreateGDIBarObject( void)
{
	I3ASSERT( m_hpenBar == nullptr);
	m_hpenBar = ::CreatePen( PS_SOLID, m_uBarWidth, m_BarColor);
	I3ASSERT( m_hpenBar != nullptr);
}

void i3GICShapeRectBar::_DeleteGDIBarObject( void)
{
	if( m_hpenBar != nullptr)
	{
		::DeleteObject( m_hpenBar);
		m_hpenBar = nullptr;
	}
}

/*virtual*/	void i3GICShapeRectBar::OnAfterDraw( i3GICDiagramContext * pCtx)
{
	if( !isStyle( I3GIC_STYLE_INVISIBLE) && (m_hpenBar != nullptr))
	{
		// Bar를 그린다.
		pCtx->selectObject( m_hpenBar);

		INT32 x = (INT32)(getPosX() + (getWidth() * m_rValue * 0.01f));

		pCtx->moveTo( x, (INT32) getPosY());
		pCtx->lineTo( x, (INT32) (getPosY() + getHeight()) );
	}
}

/*virtual*/ void i3GICShapeRectBar::OnBindDiagram( bool bBind)
{
	i3GICShapeRect::OnBindDiagram( bBind);

	if( bBind)		_CreateGDIBarObject();
	else			_DeleteGDIBarObject();
}

/*virtual*/ void i3GICShapeRectBar::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GICShapeRect::CopyTo( pDest, method);

	i3GICShapeRectBar * pDestShape = (i3GICShapeRectBar*) pDest;

	pDestShape->setBarValue( m_rValue);
	pDestShape->setBarColor( m_BarColor);
	pDestShape->setBarWidth( m_uBarWidth);
}


#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 GIC_RECT_BAR_INFO
	{
		INT8	m_ID[4] = { 'G', 'R', 'B', '1' };
		REAL32	m_rValue = 0.0f;
		UINT32	m_BarColor = 0;
		UINT32	m_uBarWidth = 0;
		UINT32	pad[4] = { 0 };

	};
}

#if defined( I3_COMPILER_VC)
#endif

/*virtual*/ UINT32 i3GICShapeRectBar::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::GIC_RECT_BAR_INFO	info;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3GICShapeRect::OnSave( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	info.m_rValue = m_rValue;
	info.m_BarColor = (UINT32) m_BarColor;
	info.m_uBarWidth = m_uBarWidth;

	Rc = pStream->Write( &info, sizeof( info));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

/*virtual*/ UINT32 i3GICShapeRectBar::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::GIC_RECT_BAR_INFO	info;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3GICShapeRect::OnLoad( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	Rc = pStream->Read( &info, sizeof( info));
	I3_CHKIO( Rc);
	Result += Rc;

	m_rValue = info.m_rValue;
	m_BarColor = (COLORREF) info.m_BarColor;
	m_uBarWidth = info.m_uBarWidth;

	return Result;
}

/*virtual*/ bool i3GICShapeRectBar::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3GICShapeRect::OnSaveXML( pFile, pXML) == false)
		return false;

	pXML->addAttr( "BarValue", m_rValue);
	pXML->addAttr( "BarColor", (UINT32) m_BarColor);
	pXML->addAttr( "BarWidth", m_uBarWidth);

	return true;
}

/*virtual*/ bool i3GICShapeRectBar::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3GICShapeRect::OnLoadXML( pFile, pXML) == false)
		return false;

	pXML->getAttr( "BarValue", &m_rValue);
	pXML->getAttr( "BarColor", (UINT32*) &m_BarColor);
	pXML->getAttr( "BarWidth", &m_uBarWidth);

	return true;
}


/*virtual*/ void i3GICShapeRectBar::OnProperty( CMFCPropertyGridCtrl * pCtrl)
{
	i3GICShapeRect::OnProperty( pCtrl);

	CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(_T("Value Bar"));

	COleVariant varBar( m_rValue);
	i3GICProperty * pPropValue = new i3GICProperty( _T( "Value"), varBar);
	pPropValue->setShape( this);
	pPropValue->setUpdateType( GIC_PROPERTY_UPDATE_BARVALUE);

	pProperty->AddSubItem( pPropValue);

	// Bar Width
	COleVariant varWidth( (long)m_uBarWidth, VT_UI4);
	i3GICProperty * pPropWidth = new i3GICProperty( _T( "Width"), varWidth);
	pPropWidth->EnableSpinControl( TRUE, 1, 20);
	pPropWidth->setShape( this);
	pPropWidth->setUpdateType( GIC_PROPERTY_UPDATE_BARWIDTH);

	pProperty->AddSubItem( pPropWidth);

	// Bar color
	i3GICColorProperty * pPropColor = new i3GICColorProperty( _T("Color"), getBarColor() );
	pPropColor->EnableOtherButton( _T("Other"));
	pPropColor->EnableAutomaticButton( _T("Default"), RGB( 255, 0, 0));
	pPropColor->setShape( this);
	pPropColor->setUpdateType( GIC_PROPERTY_UDPATE_COLOR_BAR);
	pProperty->AddSubItem( pPropColor);

	pCtrl->AddProperty( pProperty);

}

#endif	// I3_DEBUG
