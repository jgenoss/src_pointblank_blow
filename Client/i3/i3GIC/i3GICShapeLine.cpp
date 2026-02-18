#include "stdafx.h"
#include "i3GICShapeLine.h"
#include "i3GICDiagramContext.h"
#include "i3GICProperty.h"
#include "i3GICColorProperty.h"
#include "i3GICLinkPoint.h"

#if defined( I3_DEBUG)

I3_CLASS_INSTANCE( i3GICShapeLine);

#define				MAX_POINTS		64

static POINT		s_Points[ MAX_POINTS];

i3GICShapeLine::i3GICShapeLine(void)
{
	{
		m_StartPoint.m_pCtrl = i3GICControlPoint::new_object_ref();
		m_StartPoint.m_pCtrl->setCPStyle( I3GIC_CP_MOVE_LINESTART);
		m_StartPoint.m_pCtrl->setPos( 0.f, 0.f);
		AddChild( m_StartPoint.m_pCtrl);
	}

	i3Vector::Set( &m_EndPoint.m_Point, 100.f, 100.f);

	{
		m_EndPoint.m_pCtrl = i3GICControlPoint::new_object_ref();
		m_EndPoint.m_pCtrl->setCPStyle( I3GIC_CP_MOVE_LINEEND);
		m_EndPoint.m_pCtrl->setPos( 100.f, 100.f);
		AddChild( m_EndPoint.m_pCtrl);
	}
	
	setSize( 100.f, 100.f);
	setLineColor( RGB( 0, 0, 0));

	setStyle( I3GIC_STYLE_LINE | I3GIC_STYLE_NOUPDATE_SIDE_LINK);
}

i3GICShapeLine::~i3GICShapeLine(void)
{
	_DeleteGDILineObject();
}

void i3GICShapeLine::_CreateGDILineObject( void)
{
	I3ASSERT( m_hpenLine == nullptr);

	m_hpenLine = CreatePen( m_LineStyle, m_LineWidth, m_LineColor);
	I3ASSERT( m_hpenLine);
}

void i3GICShapeLine::_DeleteGDILineObject( void)
{
	if( m_hpenLine != nullptr)
	{
		::DeleteObject( m_hpenLine);
		m_hpenLine = nullptr;
	}
}

/*virtual*/ void i3GICShapeLine::OnCreate( void)
{
	{
		m_StartPoint.m_pLink = i3GICLinkPoint::new_object_ref();
		m_StartPoint.m_pLink->setPos( 0.f, 0.f);
		m_StartPoint.m_pLink->SetName( _T("Start"));
		AddChild( m_StartPoint.m_pLink);
	}

	{
		m_EndPoint.m_pLink = i3GICLinkPoint::new_object_ref();
		m_EndPoint.m_pLink->setPos( 100.f, 100.f);
		m_EndPoint.m_pLink->SetName( _T("End"));
		AddChild( m_EndPoint.m_pLink);
	}
}

void i3GICShapeLine::_DragPoint( i3GICDiagramContext * pCtx, UINT32 sem, REAL32 dcx, REAL32 dcy)
{
	VEC2D vec;
	
	if( sem & I3GIC_CP_MOVE_LINESTART)
	{
		REAL32 rSnapX = getX( &m_StartPoint.m_vSavedPos) + dcx;
		REAL32 rSnapY = getY( &m_StartPoint.m_vSavedPos) + dcy;

		if( pCtx->LinkSnapPos( this, &rSnapX, &rSnapY, m_StartPoint.m_pLink) == false)
			pCtx->GridSnapPos( &rSnapX, &rSnapY);

		i3Vector::Set( &vec, rSnapX, rSnapY);

		setStartPoint( &vec);

		m_StartPoint.m_pCtrl->setPos( &vec);
	}
	else if( sem & I3GIC_CP_MOVE_LINEEND)
	{
		REAL32 rSnapX = getX( &m_EndPoint.m_vSavedPos) + dcx;
		REAL32 rSnapY = getY( &m_EndPoint.m_vSavedPos) + dcy;

		if( pCtx->LinkSnapPos( this, &rSnapX, &rSnapY, m_EndPoint.m_pLink) == false)
			pCtx->GridSnapPos( &rSnapX, &rSnapY);

		i3Vector::Set( &vec, rSnapX, rSnapY);

		setEndPoint( &vec);
		
		// Control point
		m_EndPoint.m_pCtrl->setPos( &vec);
		
		// Link point
		if( m_EndPoint.m_pLink != nullptr)
			m_EndPoint.m_pLink->setPos( &vec);

		i3Vector::Sub( &vec, &vec, &getStartPoint()->m_Point);
		setSize( &vec);
	}
}

void i3GICShapeLine::OnDraw( i3GICDiagramContext * pCtx)
{
	I3ASSERT( isStyle( I3GIC_STYLE_GDIPLUS) == false);

	if( !isStyle( I3GIC_STYLE_INVISIBLE) && (m_hpenLine != nullptr) )
	{
		pCtx->selectObject( m_hpenLine);
		pCtx->moveTo( (INT32) (m_StartPoint.m_Point.x ), (INT32) (m_StartPoint.m_Point.y ) );
		pCtx->lineTo( (INT32) (m_EndPoint.m_Point.x ), (INT32) (m_EndPoint.m_Point.y ) );
	}

	i3GICShape::OnDraw( pCtx);
}

bool i3GICShapeLine::_HitTest( REAL32 x, REAL32 y)
{
	REAL32 t;
	VEC3D Point, Pt1, Pt2, IPt, diff;

	i3Vector::Set( &Point, x, y, 0.0f);

	Pt1.x = m_StartPoint.m_Point.x;
	Pt1.y = m_StartPoint.m_Point.y;
	Pt1.z = 0.0f;

	Pt2.x = m_EndPoint.m_Point.x;
	Pt2.y = m_EndPoint.m_Point.y;
	Pt2.z = 0.0f;

	if( i3Math::GetPerpendicularPoint( &Pt1, &Pt2, &Point, &IPt, &t))
	{
		i3Vector::Sub( &diff, &IPt, &Point);

		if( i3Vector::LengthSq( &diff) <= 8)
			return true;
	}

	return false;
}

bool i3GICShapeLine::OnHitTest( i3GICDiagramContext * pCtx, REAL32 x, REAL32 y)
{
	// Control Point Hit Test
	if( i3GICShape::OnHitTest( pCtx, x, y) )
		return true;

	if( isState( I3GIC_STATE_DISABLED))				return false;
	if( isStyle( I3GIC_STYLE_NOHITTEST))			return false;

	if( _HitTest( x, y))
	{
		if( pCtx->getCurrentHitResult()->m_rDepth < getDepth() )
		{
			pCtx->getCurrentHitResult()->m_pHitShape = this;
			pCtx->getCurrentHitResult()->m_Semantic = I3GIC_CP_MOVE_ALL;
			pCtx->getCurrentHitResult()->m_rDepth = getDepth();
			return true;
		}
	}

	return false;
}

bool i3GICShapeLine::OnHitTest( i3GICDiagramContext * pCtx, i3::pack::RECT * pRect, i3::vector<i3GICShape*>& List)
{
	if( i3GICShape::OnHitTest( pCtx, pRect, List))
		return true;

	if( isState( I3GIC_STATE_DISABLED))				return false;
	if( isStyle( I3GIC_STYLE_NOHITTEST))			return false;

	VEC2D vP1, vP2;

	i3Vector::Set( &vP1, m_StartPoint.m_Point.x, m_StartPoint.m_Point.y);
	i3Vector::Set( &vP2, m_EndPoint.m_Point.x, m_EndPoint.m_Point.y);

	if( i3Math::GetIntersectLineFromRect( &vP1, &vP2, pRect))
	{
		List.push_back( this);
		return true;
	}

	return false;
}

/*virtual*/ void i3GICShapeLine::OnUpdateByLink( i3GICDiagramContext * pCtx, i3GICLinkPoint * pEffector, i3GICLinkPoint * pLinker)
{
	i3GICShape::OnUpdateByLink( pCtx, pEffector, pLinker);

	UINT32 sem = I3GIC_CP_MOVE_LINESTART;

	if( m_StartPoint.m_pLink == pLinker)	sem = I3GIC_CP_MOVE_LINESTART;
	else if( m_EndPoint.m_pLink == pLinker)	sem = I3GIC_CP_MOVE_LINEEND;
	else
	{
		I3PRINTLOG(I3LOG_FATAL,  "invalid link point..");
	}

	_DragPoint( pCtx, sem, getX( pEffector->getUpdatedPos()), getY( pEffector->getUpdatedPos()));
}

/*virtual*/ bool i3GICShapeLine::OnDragCP( i3GICDiagramContext * pCtx)
{
	// Mouse Click 시점의 Hit Result를 사용
	GIC_HIT_RESULT * pResult = pCtx->getHitResult( GIC_HIT_RESULT_MOUSEDOWN);

	_DragPoint( pCtx, pResult->m_Semantic, pCtx->getDeltaX(), pCtx->getDeltaY());

	return true;
}

/*virtual*/ bool i3GICShapeLine::OnPreDragMove( REAL32 x, REAL32 y)
{
	i3Vector::Set( &m_StartPoint.m_Point, x, y);
	i3Vector::Set( &m_EndPoint.m_Point, x + getWidth(), y + getHeight());

	return true;
}

/*virtual*/	bool i3GICShapeLine::OnBeginDrag(void)
{
	bool bRv = i3GICShape::OnBeginDrag();

	i3Vector::Copy( &m_StartPoint.m_vSavedPos, &m_StartPoint.m_Point);
	i3Vector::Copy( &m_EndPoint.m_vSavedPos, &m_EndPoint.m_Point);

	return bRv;
}

/*virtual*/	bool i3GICShapeLine::OnCancelDrag(void)
{
	bool bRv = i3GICShape::OnCancelDrag();

	setStartPoint( &m_StartPoint.m_vSavedPos);
	setEndPoint( &m_EndPoint.m_vSavedPos);

	return bRv;
}

/*virtual*/ bool i3GICShapeLine::OnSelect( bool bFlag)
{
	return i3GICShape::OnSelect( bFlag);
}

/*virtual*/ void i3GICShapeLine::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GICShape::CopyTo( pDest, method);

	i3GICShapeLine * pDestShape = (i3GICShapeLine*) pDest;

	i3Vector::Copy( &pDestShape->getStartPoint()->m_Point, &m_StartPoint.m_Point);
	pDestShape->getStartPoint()->m_State = m_StartPoint.m_State;
	i3Vector::Copy( &pDestShape->getEndPoint()->m_Point, &m_EndPoint.m_Point);
	pDestShape->getEndPoint()->m_State = m_EndPoint.m_State;

	pDestShape->setLineStyle( m_LineStyle);
	pDestShape->setLineWidth( m_LineWidth);
	pDestShape->setLineColor( m_LineColor);
}


/*virtual*/ void i3GICShapeLine::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3GICShape::OnBuildObjectList( List);

	if( m_StartPoint.m_pLink != nullptr)
		List.push_back( m_StartPoint.m_pLink);

	if( m_EndPoint.m_pLink != nullptr)
		List.push_back( m_EndPoint.m_pLink);
}


#pragma pack( push, 8)

namespace pack
{
	struct GIC_SHAPE_LINE
	{
		UINT8		m_ID[4] = { 'G', 'L', 'N', '1' };
		UINT32		m_PointCount = 0;
		UINT32		pad[32] = { 0 };
	};
}

#pragma pack( pop)

UINT32	i3GICShapeLine::OnSave( i3ResourceFile * pResFile)
{
	pack::GIC_SHAPE_LINE data;
	UINT32 Rc, Result;

	i3Stream * pStream = pResFile->GetStream();

	Result = i3GICShape::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_PointCount = 2;

	Rc = pStream->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	i3::pack::VEC2D vec;

	vec.x = m_StartPoint.m_Point.x;
	vec.y = m_StartPoint.m_Point.y;

	Rc = pStream->Write( &vec, sizeof(vec));
	I3_CHKIO( Rc);
	Result += Rc;

	OBJREF ref;

	ref = pResFile->GetObjectPersistID( m_StartPoint.m_pLink);
	Rc = pStream->Write( &ref, sizeof( ref));
	I3_CHKIO( Rc);
	Result += Rc;

	vec.x = m_EndPoint.m_Point.x;
	vec.y = m_EndPoint.m_Point.y;

	Rc = pStream->Write( &vec, sizeof(vec));
	I3_CHKIO( Rc);
	Result += Rc;

	ref = pResFile->GetObjectPersistID( m_EndPoint.m_pLink);
	Rc = pStream->Write( &ref, sizeof( ref));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3GICShapeLine::OnLoad( i3ResourceFile * pResFile)
{
	pack::GIC_SHAPE_LINE data;
	UINT32 Rc, Result;

	i3Stream * pStream = pResFile->GetStream();

	Result = i3GICShape::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	{
		i3::pack::VEC2D vec;

		Rc = pStream->Read( &vec, sizeof(vec));
		I3_CHKIO( Rc);
		Result += Rc;

		m_StartPoint.m_Point.x = vec.x;
		m_StartPoint.m_Point.y = vec.y;

		OBJREF ref;

		Rc = pStream->Read( &ref, sizeof( ref));
		I3_CHKIO( Rc);
		Result += Rc;

		if( ref != 0)
			m_StartPoint.m_pLink = (i3GICLinkPoint*) pResFile->FindObjectByID( ref);

		Rc = pStream->Read( &vec, sizeof(vec));
		I3_CHKIO( Rc);
		Result += Rc;

		m_EndPoint.m_Point.x = vec.x;
		m_EndPoint.m_Point.y = vec.y;

		Rc = pStream->Read( &ref, sizeof( ref));
		I3_CHKIO( Rc);
		Result += Rc;

		if( ref != 0)
			m_EndPoint.m_pLink = (i3GICLinkPoint*) pResFile->FindObjectByID( ref);

		// End Point의 Control Point를 업데이트 해줘야 한다.
		m_EndPoint.m_pCtrl->setPos( getWidth(), getHeight());
	}

	return Result;
}

/*virtual*/ bool i3GICShapeLine::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3GICShape::OnSaveXML( pFile, pXML) == false)
		return false;

	OBJREF ref;

	pXML->addAttr( "StartPointPosX", m_StartPoint.m_Point.x);
	pXML->addAttr( "StartPointPosY", m_StartPoint.m_Point.y);
	pXML->addAttr( "StartPointState", (UINT32) m_StartPoint.m_State);	

	ref = pFile->GetObjectPersistID( m_StartPoint.m_pLink);
	pXML->addAttr( "StartPointLink", (INT32) ref);

	pXML->addAttr( "EndPointPosX", m_EndPoint.m_Point.x);
	pXML->addAttr( "EndPointPosY", m_EndPoint.m_Point.y);
	pXML->addAttr( "EndPointState", (UINT32) m_EndPoint.m_State);

	ref = pFile->GetObjectPersistID( m_EndPoint.m_pLink);
	pXML->addAttr( "EndPointLink", (INT32) ref);

	pXML->addAttr( "LineStyle", (UINT32) m_LineStyle);
	pXML->addAttr( "LineWidth", m_LineWidth);

	pXML->addAttr( "LineColor", (UINT32) m_LineColor);

	return true;
}

/*virtual*/ bool i3GICShapeLine::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3GICShape::OnLoadXML( pFile, pXML) == false)
		return false;

	OBJREF ref;

	pXML->getAttr( "StartPointPosX", &m_StartPoint.m_Point.x);
	pXML->getAttr( "StartPointPosY", &m_StartPoint.m_Point.y);
	pXML->getAttr( "StartPointState", (UINT32*) &m_StartPoint.m_State);

	pXML->getAttr( "StartPointLink", (INT32*) &ref);
	if( ref != 0)
		m_StartPoint.m_pLink = (i3GICLinkPoint*) pFile->FindObjectByID( ref);
	
	pXML->getAttr( "EndPointPosX", &m_EndPoint.m_Point.x);
	pXML->getAttr( "EndPointPosY", &m_EndPoint.m_Point.y);
	pXML->getAttr( "EndPointState", (UINT32*) &m_EndPoint.m_State);

	pXML->getAttr( "EndPointLink", (INT32*) &ref);
	if( ref != 0)
		m_EndPoint.m_pLink = (i3GICLinkPoint*) pFile->FindObjectByID( ref);
	
	pXML->getAttr( "LineStyle", (UINT32*) &m_LineStyle);
	pXML->getAttr( "LineWidth", &m_LineWidth);

	pXML->getAttr( "LineColor", (UINT32*) &m_LineColor);

	// End Point의 Control Point를 Update해줘야 한다.
	m_EndPoint.m_pCtrl->setPos( getWidth(), getHeight());

	return true;
}


/*virtual*/ void i3GICShapeLine::OnProperty( CMFCPropertyGridCtrl * pCtrl)
{
	i3GICShape::OnProperty( pCtrl);

	CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(_T("Line"));

	// Line Style
	COleVariant varStyle( "Solid");
	i3GICProperty * pPropLineStyle = new i3GICProperty( "Line Style", varStyle, "선의 종류입니다.");
	pPropLineStyle->setShape( this);
	pPropLineStyle->setUpdateType( GIC_PROPERTY_UPDATE_LINESTYLE);
	pPropLineStyle->AddOption( "Solid");
	pPropLineStyle->AddOption( "Dash");
	pPropLineStyle->AddOption( "Dot");
	pPropLineStyle->AddOption( "DashDot");
	pPropLineStyle->AddOption( "DashDotDot");

	varStyle.SetString( pPropLineStyle->GetOption( (int) getLineStyle()), VT_BSTR);
	pPropLineStyle->SetValue( varStyle);

	pProperty->AddSubItem( pPropLineStyle);

	// Line Witdh
	COleVariant varWidth( (long)m_LineWidth, VT_UI4);
	i3GICProperty * pPropWidth = new i3GICProperty( "Line Width", varWidth, "선 두께입니다.");
	pPropWidth->setShape( this);
	pPropWidth->setUpdateType( GIC_PROPERTY_UPDATE_LINEWIDTH);
	pPropWidth->EnableSpinControl( TRUE, 1, 10);

	pProperty->AddSubItem( pPropWidth);

	// Color
	i3GICColorProperty * pPropLineColor = new i3GICColorProperty( "Color", getLineColor() );
	pPropLineColor->EnableOtherButton( "Other");
	pPropLineColor->EnableAutomaticButton( "Default", RGB( 0, 0, 0));
	pPropLineColor->setShape( this);
	pPropLineColor->setUpdateType( GIC_PROPERTY_UPDATE_COLOR_LINE);
	pProperty->AddSubItem( pPropLineColor);


	pCtrl->AddProperty( pProperty);

	if( m_StartPoint.m_pLink != nullptr)
		m_StartPoint.m_pLink->OnProperty( pCtrl);

	if( m_EndPoint.m_pLink != nullptr)
		m_EndPoint.m_pLink->OnProperty( pCtrl);
}

#endif	// I3_DEBUG

