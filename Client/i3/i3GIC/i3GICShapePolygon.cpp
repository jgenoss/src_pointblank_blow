#include "stdafx.h"
#include "i3GICShapePolygon.h"
#include "i3GICControlPoint.h"
#include "i3GICLinkPoint.h"
#include "i3GICDiagramContext.h"

#if defined( I3_DEBUG)

I3_CLASS_INSTANCE( i3GICShapePolygon, i3GICShapeFigure);

i3GICShapePolygon::i3GICShapePolygon()
{
	setStyle( I3GIC_STYLE_POLYGON | I3GIC_STYLE_GDIPLUS);

	m_pPtArray = NULL;

	setPointCount( 5);
}

i3GICShapePolygon::~i3GICShapePolygon()
{
	INT32 i;
	I3GIC_POINT * pPoint;

	for( i = 0; i < getPointCount(); i++)
	{
		pPoint = getPoint( i);
		I3MEM_SAFE_FREE( pPoint);
	}
}

void i3GICShapePolygon::_CreatePoint( INT32 idx)
{
	I3GIC_POINT * pPoint = getPoint( idx);

	{
		pPoint->m_State = 0;

		pPoint->m_pCtrl = i3GICControlPoint::NewObjectRef();
		pPoint->m_pCtrl->setCPStyle( (I3GIC_CP_STYLE)(I3GIC_CP_POINTER | idx) );

		//i3Vector::Set( &pPoint->m_Point, (REAL32)idx * 50.f, (REAL32)(idx%2 + 1)*-100.f + 150.f);
		//pPoint->m_pCtrl->setPos( &pPoint->m_Point);
		
		AddChild( pPoint->m_pCtrl);

		pPoint->m_pLink = i3GICLinkPoint::NewObjectRef();
		pPoint->m_pLink->setPos( &pPoint->m_Point);
		AddChild( pPoint->m_pLink);
	}
}

void i3GICShapePolygon::_RemovePoint( INT32 idx)
{
	I3GIC_POINT * pPoint = getPoint( idx);

	I3ASSERT( pPoint != NULL);

	RemoveChild( pPoint->m_pCtrl);
	RemoveChild( pPoint->m_pLink);

	I3MEM_SAFE_FREE( pPoint);
}

/*virtual*/	void i3GICShapePolygon::OnDraw( i3GICDiagramContext * pCtx)
{
	INT32 i;

	if( !isStyle( I3GIC_STYLE_INVISIBLE) )
	{
		for( i = 0; i < getPointCount(); i++)
		{
			I3GIC_POINT * pPoint = getPoint( i);
			m_pPtArray[ i].x = (LONG) (getX( &pPoint->m_Point) + getPosX());
			m_pPtArray[ i].y = (LONG) (getY( &pPoint->m_Point) + getPosY());
		}

		pCtx->drawPolygon( (GIC_LINE_STYLE) getBorderStyle(), (REAL32) getBorderSize(), getBorderColor(), getFillColor(), m_pPtArray, getPointCount());
	}

	i3GICShapeFigure::OnDraw( pCtx);
}
/*virtual*/	bool i3GICShapePolygon::OnHitTest( i3GICDiagramContext * pCtx, REAL32 x, REAL32 y)
{
	return false;
}
/*virtual*/	bool i3GICShapePolygon::OnHitTest( i3GICDiagramContext * pCtx, I3RECT * pRect, i3List * pList)
{
	return false;
}

/*virtual*/	bool i3GICShapePolygon::OnBeginDrag(void)
{
	INT32 i;

	if( i3GICShapeFigure::OnBeginDrag() )
	{	
		for( i = 0; i < getPointCount(); i++)
		{
			I3GIC_POINT * pPoint = getPoint( i);

			i3Vector::Copy( &pPoint->m_vSavedPos, &pPoint->m_Point);
		}

		return true;
	}

	return false;
}

/*virtual*/	bool i3GICShapePolygon::OnCancelDrag(void)
{
	INT32 i;

	if( i3GICShapeFigure::OnCancelDrag() )
	{
		for( i = 0; i < getPointCount(); i++)
		{
			I3GIC_POINT * pPoint = getPoint( i);

			i3Vector::Copy( &pPoint->m_Point, &pPoint->m_vSavedPos);
		}

		return true;
	}

	return false;
}

/*virtual*/ bool i3GICShapePolygon::OnDragCP( i3GICDiagramContext * pCtx)
{
	VEC2D vec;
	INT32 iPointIdx = 0;

	REAL32 dcx = pCtx->getDeltaX();
	REAL32 dcy = pCtx->getDeltaY();

	// Mouse Click 시점의 Hit result를 사용
	GIC_HIT_RESULT * pResult = pCtx->getHitResult( GIC_HIT_RESULT_MOUSEDOWN);

	iPointIdx = (INT32) pResult->m_Semantic & 0xFF;

	I3GIC_POINT * pPoint = getPoint( iPointIdx);

	REAL32 rSnapX = getX( &pPoint->m_vSavedPos) + dcx;
	REAL32 rSnapY = getY( &pPoint->m_vSavedPos) + dcy;

	if( pCtx->LinkSnapPos( this, &rSnapX, &rSnapY) == false)
		pCtx->GridSnapPos( &rSnapX, &rSnapY);
	
	i3Vector::Set( &vec, rSnapX, rSnapY);

	i3Vector::Copy( &pPoint->m_Point, &vec);

	pPoint->m_pCtrl->setPos( &vec);
	pPoint->m_pLink->setPos( &vec);

	return false;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

typedef struct __tagi3GICShapePolygonHead
{
	UINT8	m_ID[4];
	INT32	m_PointCount;
	UINT32	pad[4];
} I3PERSIST_SHAPE_POLYGON_HEAD;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

/*virtual*/ UINT32 i3GICShapePolygon::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	I3PERSIST_SHAPE_POLYGON_HEAD head;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3GICShapeFigure::OnSave( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	memset( &head, 0, sizeof( head));
	memcpy( head.m_ID, "GSP1", 4);
	head.m_PointCount = getPointCount();

	Rc = pStream->Write( &head, sizeof( head));
	I3_CHKIO( Rc);
	Result += Rc;

	/*INT32 i;

	for( i = 0; i < getPointCount(); i++)
	{
		I3GIC_POINT * pPoint = getPoint( i);
	}*/

	return Result;
}

/*virtual*/ UINT32 i3GICShapePolygon::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	I3PERSIST_SHAPE_POLYGON_HEAD head;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3GICShapeFigure::OnLoad( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	Rc = pStream->Read( &head, sizeof( head));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

/*virtual*/ bool i3GICShapePolygon::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	return true;
}

/*virtual*/ bool i3GICShapePolygon::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	return true;
}

/*virtual*/ void i3GICShapePolygon::OnProperty( CMFCPropertyGridCtrl * pCtrl)
{
	i3GICShapeFigure::OnProperty( pCtrl);
}


#endif
