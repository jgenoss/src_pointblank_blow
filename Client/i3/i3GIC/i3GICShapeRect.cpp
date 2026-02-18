#include "stdafx.h"
#include "i3GICShapeRect.h"
#include "i3GICDiagramContext.h"
#include "i3GICControlPoint.h"
#include "i3GICProperty.h"
#include "i3GICColorProperty.h"
#include "i3GICLinkPoint.h"

#if defined( I3_DEBUG)

I3_CLASS_INSTANCE( i3GICShapeRect);

static UINT32 s_CPStyle[] =
{
	I3GIC_CP_RESIZE_UP,										// I3GIC_CORNER_TOP = 0,
	I3GIC_CP_RESIZE_LEFT | I3GIC_CP_RESIZE_UP,				// I3GIC_CORNER_LEFT_TOP,
	I3GIC_CP_RESIZE_LEFT,									// I3GIC_CORNER_LEFT,
	I3GIC_CP_RESIZE_LEFT | I3GIC_CP_RESIZE_DOWN,			// I3GIC_CORNER_LEFT_BOTTOM,
	I3GIC_CP_RESIZE_DOWN,									// I3GIC_CORNER_BOTTOM,
	I3GIC_CP_RESIZE_RIGHT | I3GIC_CP_RESIZE_DOWN,			// I3GIC_CORNER_RIGHT_BOTTOM,
	I3GIC_CP_RESIZE_RIGHT,									// I3GIC_CORNER_RIGHT,
	I3GIC_CP_RESIZE_RIGHT | I3GIC_CP_RESIZE_UP,				// I3GIC_CORNER_RIGHT_TOP,
};

i3GICShapeRect::i3GICShapeRect(void)
{
	INT32 i;

	i3GICControlPoint * pCP;
	char szName[64];
	
	for( i = 0; i < I3GIC_CORNER_MAX; i++)
	{
		pCP = i3GICControlPoint::new_object_ref();
		pCP->setCPStyle( s_CPStyle[i]);
		i3::snprintf( szName, sizeof( szName), _T("CP_%d"), i);
		pCP->SetName( szName);
		m_pCP[ i] = pCP;
		AddChild( pCP);
	}
	
	setStyle( I3GIC_STYLE_RECT);
}

/*virtual*/ void i3GICShapeRect::OnCreate( void)
{
	INT32 i;
	char szName[ 4];

	i3GICShapeFigure::OnCreate();

	for( i = 0; i < 5; i++)
	{
		m_pLP[ i] = i3GICLinkPoint::new_object_ref();
		i3::snprintf( szName, sizeof( szName), _T("%d"), i);
		m_pLP[ i]->SetName( szName);

		AddChild( m_pLP[ i]);
	}

	setSize( 150.0f, 90.0f);
}

void	i3GICShapeRect::OnDraw( i3GICDiagramContext * pCtx)
{
	INT32 _x1, _y1, _x2, _y2;

	if( !isStyle( I3GIC_STYLE_INVISIBLE) )
	{
		_x1 = (INT32) getPosX();
		_y1 = (INT32) getPosY();
		_x2 = (INT32) (getPosX() + getWidth());
		_y2 = (INT32) (getPosY() + getHeight());

		_SelectPenAndBrush( pCtx);

		if( m_rCornerRadius == 0.f)
		{
			pCtx->rectangle( _x1, _y1, _x2, _y2);
		}
		else
		{
			pCtx->roundRect( _x1, _y1, _x2, _y2, (INT32) (m_rCornerRadius * 2.f), (INT32) (m_rCornerRadius * 2.f) );
		}
	}

	OnAfterDraw( pCtx);

	i3GICShape::OnDraw( pCtx);
}

bool			i3GICShapeRect::OnPreSize( REAL32 cx, REAL32 cy)
{
	INT32 i;
	i3GICControlPoint * pCP;
	REAL32 x, y;

	for( i = 0; i < I3GIC_CORNER_MAX; i++)
	{
		pCP = m_pCP[i];

		x = cx * 0.5f;
		y = cy * 0.5f;

		if( pCP->getCPStyle() & I3GIC_CP_RESIZE_UP)
			y = 0.0f;
		else if( pCP->getCPStyle() & I3GIC_CP_RESIZE_DOWN)
			y = cy;

		if( pCP->getCPStyle() & I3GIC_CP_RESIZE_LEFT)
			x = 0.0f;
		else if( pCP->getCPStyle() & I3GIC_CP_RESIZE_RIGHT)
			x = cx;

		pCP->setPos( x, y);
	}

#if 1
	VEC2D v;

	if( m_pLP[0] != nullptr)
	{
		i3Vector::Set( &v, (cx - getX( getSavedSize())) * 0.5f, 0);
		i3Vector::Add( m_pLP[0]->getUpdatedPos(), m_pLP[0]->getUpdatedPos(), &v);
		i3Vector::Set( &v, cx * 0.5f, 0.f);
		m_pLP[ 0]->setPos( &v);
	}

	if( m_pLP[1] != nullptr)
	{
		i3Vector::Set( &v, 0.f, (cy - getY( getSavedSize())) * 0.5f);
		i3Vector::Add( m_pLP[1]->getUpdatedPos(), m_pLP[1]->getUpdatedPos(), &v);
		i3Vector::Set( &v, 0.f, cy * 0.5f);
		m_pLP[ 1]->setPos( &v);
	}

	if( m_pLP[2] != nullptr)
	{
		i3Vector::Set( &v, (cx - getX( getSavedSize())) * 0.5f, (cy - getY( getSavedSize())) );
		i3Vector::Add( m_pLP[2]->getUpdatedPos(), m_pLP[2]->getUpdatedPos(), &v);
		i3Vector::Set( &v, cx * 0.5f, cy);
		m_pLP[ 2]->setPos( &v);
	}

	if( m_pLP[3] != nullptr)
	{
		i3Vector::Set( &v, (cx - getX( getSavedSize()) ), (cy - getY( getSavedSize())) * 0.5f);
		i3Vector::Add( m_pLP[3]->getUpdatedPos(), m_pLP[3]->getUpdatedPos(), &v);
		i3Vector::Set( &v, cx, cy * 0.5f);
		m_pLP[ 3]->setPos( &v);
	}

	if( m_pLP[4] != nullptr)
	{
		i3Vector::Set( &v, (cx - getX( getSavedSize())) * 0.5f, (cy - getY( getSavedSize())) * 0.5f );
		i3Vector::Add( m_pLP[4]->getUpdatedPos(), m_pLP[4]->getUpdatedPos(), &v);
		i3Vector::Set( &v, cx * 0.5f, cy * 0.5f);
		m_pLP[ 4]->setPos( &v);
	}
#endif

	return true;
}

/*virtual*/ bool i3GICShapeRect::OnAllLinkerSnapToLink( i3GICDiagramContext * pCtx, REAL32 * pX, REAL32 * pY)
{
	i3GICShape::OnAllLinkerSnapToLink( pCtx, pX, pY);

	INT32 i;

	REAL32 rTempX;
	REAL32 rTempY;

	if( m_pLP[0] == nullptr)
		return false;

	for( i = 0; i < 5; i++)
	{
		rTempX = *pX + m_pLP[i]->getPosX();
		rTempY = *pY + m_pLP[i]->getPosY();

		if( pCtx->LinkSnapPos( this, &rTempX, &rTempY, m_pLP[i]) == true)
		{
			// 좌표를 Link point에 맞춰서 돌려놓는다.
			*pX = rTempX - m_pLP[i]->getPosX();
			*pY = rTempY - m_pLP[i]->getPosY();
			return true;
		}
	}

	return false;
}

/*virtual*/	void i3GICShapeRect::OnAfterDragMove( REAL32 x, REAL32 y)
{
	INT32 i;

	if( m_pLP[0] == nullptr)
		return;

	for( i = 0; i < 5; i++)
	{
		i3Vector::Set( m_pLP[i]->getUpdatedPos(), (x - getX( getSavedPos())), (y - getY( getSavedPos()) ) );
	}
}


bool i3GICShapeRect::OnHitTest( i3GICDiagramContext * pCtx, REAL32 x, REAL32 y)
{
	i3::pack::RECT rt;
	VEC2D pt;

	if( i3GICShape::OnHitTest( pCtx, x, y))
		return true;

	if( isState( I3GIC_STATE_DISABLED))				return false;
	if( isStyle( I3GIC_STYLE_NOHITTEST))			return false;

	// Child 중에 하나가 Hit 되었다면 자신에 대한 Test는 하지 않는다.
	pt.x = x;
	pt.y = y;

	getRect( &rt);

	rt.left += getX( pCtx->getStack()->getTop());
	rt.right += getX( pCtx->getStack()->getTop());
	rt.top += getY( pCtx->getStack()->getTop());
	rt.bottom += getY( pCtx->getStack()->getTop());

	i3Math::NormalizeRect( &rt);

	if( i3Math::PtInRect( &rt, &pt))
	{
		if( pCtx->getCurrentHitResult()->m_rDepth < getDepth() )
		{
			pCtx->getCurrentHitResult()->m_pHitShape = this;
			pCtx->getCurrentHitResult()->m_Semantic = I3GIC_CP_MOVE_ALL;
			pCtx->getCurrentHitResult()->m_rDepth = getDepth();
//			I3TRACE( I3STR("Hit Shape Rect : %s\n", GetName()));
			return true;
		}
	}

	return false;
}

bool	i3GICShapeRect::OnHitTest( i3GICDiagramContext * pCtx, i3::pack::RECT * pRect, i3::vector<i3GICShape*>& List)
{
	i3::pack::RECT rt;

	if( i3GICShape::OnHitTest( pCtx, pRect, List) )
		return true;

	if( isState( I3GIC_STATE_DISABLED))				return false;
	if( isStyle( I3GIC_STYLE_NOHITTEST))			return false;

	getRect( &rt);

	rt.left += getX( pCtx->getStack()->getTop());
	rt.right += getX( pCtx->getStack()->getTop());
	rt.top += getY( pCtx->getStack()->getTop());
	rt.bottom += getY( pCtx->getStack()->getTop());

	i3Math::NormalizeRect( &rt);

	if( i3Math::RectInRect( &rt, pRect) != TOUCH_NONE)
	{
		List.push_back( this);

		return true;
	}

	return false;
}


/*virtual*/ void i3GICShapeRect::OnUpdateByLink( i3GICDiagramContext * pCtx, i3GICLinkPoint * pEffector, i3GICLinkPoint * pLinker)
{
	i3GICShapeFigure::OnUpdateByLink( pCtx, pEffector, pLinker);

	// Mouse Click 시점의 Hit Result를 사용
	GIC_HIT_RESULT * pResult = pCtx->getHitResult( GIC_HIT_RESULT_MOUSEDOWN);

	if( pResult->m_Semantic & I3GIC_CP_MOVE_ALL)
	{
		VEC2D vPos;

		i3Vector::Add( &vPos, &m_vSavedPos, pEffector->getUpdatedPos());

		setPos( &vPos);
	}
	else if( pResult->m_Semantic & I3GIC_CP_RESIZE_ALL)
	{
		VEC2D vPos;

		i3Vector::Add( &vPos, &m_vSavedPos, pEffector->getUpdatedPos());

		setPos( &vPos);
	}
}


/*virtual*/ void i3GICShapeRect::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GICShapeFigure::CopyTo( pDest, method);

	i3GICShapeRect * pDestShape = (i3GICShapeRect*) pDest;

	pDestShape->setCornerRadius( m_rCornerRadius);
}

/*virtual*/ void i3GICShapeRect::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3GICShapeFigure::OnBuildObjectList( List);

	INT32 i;

	if( m_pLP[ 0] == nullptr)
		return;

	for( i = 0; i < 5; i++)
	{
		if( m_pLP[ i] != nullptr)
			List.push_back(m_pLP[i]);
	}
}

#pragma pack( push, 8)

namespace pack
{
	struct GIC_SHAPE_RECT
	{
		INT8		m_ID[4] = { 'G', 'S', 'R', '1' };
		COLORREF	m_BorderColor = 0;
		UINT32		pad[8] = { 0 };
	};

	struct GIC_SHAPE_RECT2
	{
		INT8		m_ID[4] = { 'G', 'S', 'R', '2' };
		COLORREF	m_BorderColor = 0;
		REAL32		m_rCornerRadius = 0.0f;
		UINT32		pad[7] = { 0 };
	};
}

#pragma pack( pop)

UINT32	i3GICShapeRect::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::GIC_SHAPE_RECT2 data;

	i3Stream * pStream = pResFile->GetStream();

	Result = i3GICShapeFigure::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_rCornerRadius = m_rCornerRadius;
	
	Rc = pStream->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	INT32 i;
	OBJREF ref;

	for( i = 0; i < 5; i++)
	{
		ref = pResFile->GetObjectPersistID( m_pLP[ i]);
		Rc = pStream->Write( &ref, sizeof( ref));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32	i3GICShapeRect::OnLoad( i3ResourceFile * pResFile)
{
	INT32 i;
	OBJREF ref;
	UINT32 Rc, Result;

	i3Stream * pStream = pResFile->GetStream();

	Result = i3GICShapeFigure::OnLoad( pResFile);
	I3_CHKIO( Result);

	UINT8 Ver[4];
	UINT32 cur = pStream->GetPosition();
	Rc = pStream->Read( Ver, sizeof( Ver));
	I3_CHKIO( Rc);

	pStream->SetPosition( cur);

	if( Ver[0] == 'G' && Ver[1] == 'S' && Ver[2] == 'R' && Ver[3] == '1')
	{	// Old
		pack::GIC_SHAPE_RECT data;
		Rc = pStream->Read( &data, sizeof(data));
		I3_CHKIO( Rc);
		Result += Rc;

		setBorderColor( data.m_BorderColor);
	}
	else
	{
		pack::GIC_SHAPE_RECT2 data2;
		Rc = pStream->Read( &data2, sizeof(data2));
		I3_CHKIO( Rc);
		Result += Rc;

		for( i = 0; i < 5; i++)
		{
			Rc = pStream->Read( &ref, sizeof( ref));
			I3_CHKIO( Rc);
			Result += Rc;

			if( ref != 0)
			{
				m_pLP[ i] = (i3GICLinkPoint*) pResFile->FindObjectByID( ref);
			}
		}

		setCornerRadius( data2.m_rCornerRadius);

		OnPreSize( getWidth(), getHeight());
	}

	return Result;
}

/*virtual*/ bool i3GICShapeRect::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	INT32 i;
	char conv[260];

	if( i3GICShapeFigure::OnSaveXML( pFile, pXML) == false)
		return false;

	pXML->addAttr( "CornerRound", m_rCornerRadius);

	OBJREF ref;
	for( i = 0; i < 5; i++)
	{
		ref = pFile->GetObjectPersistID( m_pLP[i]);

		i3::snprintf( conv, sizeof( conv), _T( "Linker%d"), i);
		pXML->addAttr( conv, (INT32) ref);
	}

	return true;
}

/*virtual*/ bool i3GICShapeRect::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	INT32 i;
	char conv[260];
	OBJREF ref;

	if( i3GICShapeFigure::OnLoadXML( pFile, pXML) == false)
		return false;

	pXML->getAttr( "CornerRound", &m_rCornerRadius);

	for( i = 0; i < 5; i++)
	{
		i3::snprintf( conv, sizeof( conv), _T("Linker%d"), i);
		if( pXML->getAttr( conv, (INT32*) &ref) != nullptr)
		{
			if( ref != 0)
			{
				i3GICLinkPoint * pLink = (i3GICLinkPoint*) pFile->FindObjectByID( ref);

				if( pLink != nullptr)
					m_pLP[i] = pLink;
			}
		}
	}

	// Control Point의 위치를 한번 업데이트 해야한다.
	OnPreSize( getWidth(), getHeight());

	return true;
}


/*virtual*/ void i3GICShapeRect::OnProperty( CMFCPropertyGridCtrl * pCtrl)
{
	i3GICShapeFigure::OnProperty( pCtrl);

	if( getStyle() & I3GIC_STYLE_RECT)
	{
		CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(_T("Rectangle"));

		// Rounded corner value
		COleVariant varRound( (long)m_rCornerRadius, VT_UI4);
		i3GICProperty * pPropRoundValue = new i3GICProperty( _T( "Rounded Corner Value"), varRound);
		pPropRoundValue->setShape( this);
		pPropRoundValue->setUpdateType( GIC_PROPERTY_UPDATE_ROUNDVALUE);
		pPropRoundValue->EnableSpinControl( TRUE, 0, (INT32) MIN( getWidth(), getHeight()) );

		pProperty->AddSubItem( pPropRoundValue);

		pCtrl->AddProperty( pProperty);
	}

	if( m_pLP[0] != nullptr)
	{
		INT32 i;

		for( i = 0; i < 5; i++)
			m_pLP[ i]->OnProperty( pCtrl);
	}
}

#endif // I3_DEBUG
