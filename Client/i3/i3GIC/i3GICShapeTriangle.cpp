#include "stdafx.h"
#include "i3GICShapeTriangle.h"
#include "i3GICControlPoint.h"
#include "i3GICLinkPoint.h"
#include "i3GICDiagramContext.h"

#if defined( I3_DEBUG)

I3_CLASS_INSTANCE( i3GICShapeTriangle);

i3GICShapeTriangle::i3GICShapeTriangle()
{
	setStyle( I3GIC_STYLE_TRIANGLE | I3GIC_STYLE_GDIPLUS);
	
	for (INT32 i = 0; i < 3; i++)
	{
		m_Point[ i].m_pCtrl = i3GICControlPoint::new_object_ref();
		if( i == 0)			m_Point[ i].m_pCtrl->setCPStyle( I3GIC_CP_MOVE_POINT1);
		else if( i == 1)	m_Point[ i].m_pCtrl->setCPStyle( I3GIC_CP_MOVE_POINT2);
		else				m_Point[ i].m_pCtrl->setCPStyle( I3GIC_CP_MOVE_POINT3);

		i3Vector::Set( &m_Point[ i].m_Point, (REAL32)i * 50.f, (REAL32)(i%2 + 1)*-100.f + 150.f);
		m_Point[ i].m_pCtrl->setPos( &m_Point[ i].m_Point);
		
		AddChild( m_Point[ i].m_pCtrl);
	}
}

i3GICShapeTriangle::~i3GICShapeTriangle()
{
	INT32 i;

	for( i = 0; i < 3; i++)
	{
		I3_SAFE_RELEASE( m_Point[ i].m_pLink);
	}
}

/*virtual*/ void i3GICShapeTriangle::OnCreate( void)
{
	INT32 i;

	i3GICShapeFigure::OnCreate();

	for( i = 0; i < 3; i++)
	{
		m_Point[ i].m_pLink = i3GICLinkPoint::new_object();
		m_Point[ i].m_pLink->setPos( &m_Point[ i].m_Point);
		AddChild( m_Point[ i].m_pLink);
	}
}

/*virtual*/	void i3GICShapeTriangle::OnDraw( i3GICDiagramContext * pCtx)
{
	INT32 i;

	if( !isStyle( I3GIC_STYLE_INVISIBLE) )
	{
		POINT pt[3];
		for( i = 0; i < 3; i++)
		{
			pt[ i].x = (LONG) (getX( &m_Point[ i].m_Point) + getPosX());
			pt[ i].y = (LONG) (getY( &m_Point[ i].m_Point) + getPosY());
		}

		pCtx->drawPolygon( (GIC_LINE_STYLE) getBorderStyle(), (REAL32) getBorderSize(), getBorderColor(), getFillColor(), pt, 3);
	}

	i3GICShapeFigure::OnDraw( pCtx);
}

/*virtual*/	bool i3GICShapeTriangle::OnHitTest( i3GICDiagramContext * pCtx, REAL32 x, REAL32 y)
{
	if( i3GICShape::OnHitTest( pCtx, x, y) )
		return true;

	if( isState( I3GIC_STATE_DISABLED))				return false;
	if( isStyle( I3GIC_STYLE_NOHITTEST))			return false;

	VEC2D pos, v1, v2, v3;

	i3Vector::Set( &pos, x, y);

	i3Vector::Add( &v1, &m_Point[0].m_Point, &m_vPos);
	i3Vector::Add( &v2, &m_Point[1].m_Point, &m_vPos);
	i3Vector::Add( &v3, &m_Point[2].m_Point, &m_vPos);

	if( i3Math::IsPtInTri( &v1, &v2, &v3, &pos) )
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

/*virtual*/	bool i3GICShapeTriangle::OnHitTest( i3GICDiagramContext * pCtx, i3::pack::RECT * pRect, i3::vector<i3GICShape*>& List)
{
	if( i3GICShape::OnHitTest( pCtx, pRect, List))
		return true;

	if( isState( I3GIC_STATE_DISABLED))				return false;
	if( isStyle( I3GIC_STYLE_NOHITTEST))			return false;

	INT32 i;
	bool bInclude = false;

	for( i = 0; i < 3; i++)
	{
		if( i3Math::GetPointOnRect( getPosX() + getX( &m_Point[i].m_Point), getPosY() + getY( &m_Point[ i].m_Point), pRect) == 0)
			bInclude = true;
	}

	if( bInclude)
	{
		List.push_back( this);
		return true;
	}

	return false;
}

/*virtual*/	bool i3GICShapeTriangle::OnBeginDrag(void)
{
	INT32 i;

	if( i3GICShapeFigure::OnBeginDrag() )
	{	
		for( i = 0; i < 3; i++)
		{
			i3Vector::Copy( &m_Point[i].m_vSavedPos, &m_Point[i].m_Point);
		}

		return true;
	}

	return false;
}

/*virtual*/	bool i3GICShapeTriangle::OnCancelDrag(void)
{
	INT32 i;

	if( i3GICShapeFigure::OnCancelDrag() )
	{
		for( i = 0; i < 3; i++)
		{
			i3Vector::Copy( &m_Point[i].m_Point, &m_Point[i].m_vSavedPos);
		}

		return true;
	}

	return false;
}

/*virtual*/ bool i3GICShapeTriangle::OnDragCP( i3GICDiagramContext * pCtx)
{
	VEC2D vec;
	INT32 iPointIdx = 0;

	REAL32 dcx = pCtx->getDeltaX();
	REAL32 dcy = pCtx->getDeltaY();

	// Mouse Click 시점의 Hit Result를 사용
	GIC_HIT_RESULT * pResult = pCtx->getHitResult( GIC_HIT_RESULT_MOUSEDOWN);

	if( pResult->m_Semantic & I3GIC_CP_MOVE_POINT1)			iPointIdx = 0;
	else if( pResult->m_Semantic & I3GIC_CP_MOVE_POINT2)	iPointIdx = 1;
	else if( pResult->m_Semantic & I3GIC_CP_MOVE_POINT3)	iPointIdx = 2;

	REAL32 rSnapX = getX( &m_Point[iPointIdx].m_vSavedPos) + dcx;
	REAL32 rSnapY = getY( &m_Point[iPointIdx].m_vSavedPos) + dcy;

	if( pCtx->LinkSnapPos( this, &rSnapX, &rSnapY) == false)
		pCtx->GridSnapPos( &rSnapX, &rSnapY);
	
	i3Vector::Set( &vec, rSnapX, rSnapY);

	i3Vector::Copy( &m_Point[iPointIdx].m_Point, &vec);

	//i3Vector::Add( &vec, -getPosX(), -getPosY());

	m_Point[ iPointIdx].m_pCtrl->setPos( &vec);
	m_Point[ iPointIdx].m_pLink->setPos( &vec);

	return true;
}

/*virtual*/ UINT32 i3GICShapeTriangle::OnSave( i3ResourceFile * pResFile)
{
	INT32 i;
	i3::pack::VEC2D vec;
	UINT32 Rc, Result = 0;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3GICShapeFigure::OnSave( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	for( i = 0; i < 3; i++)
	{
		vec.x = m_Point[i].m_Point.x;
		vec.y = m_Point[i].m_Point.y;

		Rc = pStream->Write( &vec, sizeof( vec));
		I3_CHKIO( Rc);
		Result += Rc;

		Rc = pStream->Write( &m_Point[i].m_State, sizeof( m_Point[i].m_State));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

/*virtual*/ UINT32 i3GICShapeTriangle::OnLoad( i3ResourceFile * pResFile)
{
	INT32 i;
	i3::pack::VEC2D vec;
	UINT32 Rc, Result = 0;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3GICShapeFigure::OnLoad( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	for( i = 0; i < 3; i++)
	{
		Rc = pStream->Read( &vec, sizeof( vec));
		I3_CHKIO( Rc);
		Result += Rc;

		m_Point[i].m_Point.x = vec.x;
		m_Point[i].m_Point.y = vec.y;

		Rc = pStream->Read( &m_Point[i].m_State, sizeof( m_Point[i].m_State));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

/*virtual*/ bool i3GICShapeTriangle::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3GICShapeFigure::OnSaveXML( pFile, pXML) == false)
		return false;

	INT32 i;
	char conv[260];

	for( i = 0; i < 3; i++)
	{
		i3::snprintf( conv, _countof( conv), "TriP%dPosX", i);
		pXML->addAttr( conv, m_Point[i].m_Point.x);

		i3::snprintf( conv, _countof( conv), "TriP%dPosY", i);
		pXML->addAttr( conv, m_Point[i].m_Point.y);

		i3::snprintf( conv, _countof( conv), "TriP%dState", i);
		pXML->addAttr( conv, (UINT32) m_Point[i].m_State);
	}

	return true;
}

/*virtual*/ bool i3GICShapeTriangle::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3GICShapeFigure::OnLoadXML( pFile, pXML) == false)
		return false;

	INT32 i;
	char conv[260];

	for( i = 0; i < 3; i++)
	{
		i3::snprintf( conv, _countof( conv), "TriP%dPosX", i);
		pXML->getAttr( conv, &m_Point[i].m_Point.x);

		i3::snprintf( conv, _countof( conv), "TriP%dPosY", i);
		pXML->getAttr( conv, &m_Point[i].m_Point.y);

		i3::snprintf( conv, _countof( conv), "TriP%dState", i);
		pXML->getAttr( conv, (UINT32*) &m_Point[i].m_State);
	}

	return true;
}

/*virtual*/ void i3GICShapeTriangle::OnProperty( CMFCPropertyGridCtrl * pCtrl)
{
	i3GICShapeFigure::OnProperty( pCtrl);
}


#endif	// I3_DEBUG
