#include "stdafx.h"
#include "i3GICShapeCircleBar.h"
#include "i3GICProperty.h"
#include "i3GICDiagramContext.h"
#include "i3GICColorProperty.h"
#include "i3GICControlPoint.h"

#if defined( I3_DEBUG)

I3_CLASS_INSTANCE( i3GICShapeCircleBar);

i3GICShapeCircleBar::i3GICShapeCircleBar()
{
	setBarColor( RGB( 255, 0, 0));
	
	setStyle( I3GIC_STYLE_CIRCLEBAR);
}

i3GICShapeCircleBar::~i3GICShapeCircleBar()
{
	_DeleteGDIBarObject();
}

void i3GICShapeCircleBar::_CreateGDIBarObject( void)
{
	I3ASSERT( m_hpenBar == nullptr);

	m_hpenBar = ::CreatePen( PS_SOLID, m_uBarWidth, m_BarColor);
	I3ASSERT( m_hpenBar != nullptr);
}

void i3GICShapeCircleBar::_DeleteGDIBarObject( void)
{
	if( m_hpenBar != nullptr)
	{
		::DeleteObject( m_hpenBar);
		m_hpenBar = nullptr;
	}
}

/*virtual*/ void i3GICShapeCircleBar::OnAfterDraw( i3GICDiagramContext * pCtx)
{
	if( !isStyle( I3GIC_STYLE_INVISIBLE) )
	{
		// 추가 Bar를 그린다.
		if( m_hpenBar != nullptr)
		{
			REAL32 rRad = I3_DEG2RAD( m_rBarAngle);
			INT32 posX = (INT32) (getPosX() + getWidth() * 0.5f);
			INT32 posY = (INT32)(getPosY() + getHeight() * 0.5f);
			INT32 x = (INT32) (i3Math::cos( rRad) * getWidth() * 0.5f);
			INT32 y = (INT32) -(i3Math::sin( rRad) * getHeight() * 0.5f);

			//m_pBarCP[1]->setPos( (REAL32)x, (REAL32)y);

			pCtx->selectObject( m_hpenBar);

			pCtx->moveTo( posX, posY);
			pCtx->lineTo( posX + x, posY + y);
		}
	}
}

/*virtual*/	bool i3GICShapeCircleBar::OnHitTest( i3GICDiagramContext * pCtx, REAL32 x, REAL32 y)
{
	// Bar CP를 체크
	/*
	REAL32 t;
	VEC3D Point, Pt1, Pt2, IPt, diff;

	i3Vector::Set( &Point, x, y, 0.0f);

	Pt1.x = m_pBarCP[0]->getPosX();
	Pt1.y = m_pBarCP[0]->getPosY();
	Pt1.z = 0.0f;

	Pt2.x = m_pBarCP[1]->getPosX();
	Pt2.y = m_pBarCP[1]->getPosY();
	Pt2.z = 0.0f;

	if( i3Math::GetPerpendicularPoint( &Pt1, &Pt2, &Point, &IPt, &t))
	{
		i3Vector::Sub( &diff, &IPt, &Point);

		if( i3Vector::LengthSq( &diff) <= 8)
			return true;
	}
	*/

	return i3GICShapeCircle::OnHitTest( pCtx, x, y);
}

/*virtual*/ void i3GICShapeCircleBar::OnBindDiagram( bool bBind)
{
	i3GICShapeCircle::OnBindDiagram( bBind);

	if( bBind)	_CreateGDIBarObject();
	else		_DeleteGDIBarObject();
}

/*virtual*/ void i3GICShapeCircleBar::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GICShapeCircle::CopyTo( pDest, method);

	i3GICShapeCircleBar * pDestShape = (i3GICShapeCircleBar*) pDest;

	pDestShape->setAngle( getAngle());

	pDestShape->setBarWidth( getBarWidth());

	pDestShape->setBarColor( getBarColor());
}

	
/*virtual*/ UINT32 i3GICShapeCircleBar::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3GICShapeCircle::OnSave( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	Rc = pStream->Write( &m_rBarAngle, sizeof( m_rBarAngle));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

/*virtual*/ UINT32 i3GICShapeCircleBar::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3GICShapeCircle::OnLoad( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	Rc = pStream->Read( &m_rBarAngle, sizeof( m_rBarAngle));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

/*virtual*/ bool i3GICShapeCircleBar::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3GICShapeCircle::OnSaveXML( pFile, pXML) == false)
		return false;

	pXML->addAttr( _T("BarValue"), m_rBarAngle);
	pXML->addAttr( _T("BarWidth"), m_uBarWidth);
	pXML->addAttr( _T("BarColor"), (UINT32) m_BarColor);

	return true;
}

/*virtual*/ bool i3GICShapeCircleBar::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3GICShapeCircle::OnLoadXML( pFile, pXML) == false)
		return false;

	pXML->getAttr( _T("BarValue"), &m_rBarAngle);
	pXML->getAttr( _T("BarWidth"), &m_uBarWidth);
	pXML->getAttr( _T("BarColor"), (UINT32*) &m_BarColor);

	return true;
}

/*virtual*/ void i3GICShapeCircleBar::OnProperty( CMFCPropertyGridCtrl * pCtrl)
{
	i3GICShapeCircle::OnProperty( pCtrl);

	CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(_T("Value Bar"));

	// Rounded corner value
	COleVariant varBar( m_rBarAngle);
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
