#include "stdafx.h"
#include "i3GICControlPoint.h"
#include "i3GICDiagramContext.h"

#if defined( I3_DEBUG)

I3_CLASS_INSTANCE( i3GICControlPoint);

i3GICControlPoint::i3GICControlPoint(void)
{
	addNodeStyle( I3_TREENODE_STYLE_VOLATILE);

	setStyle( I3GIC_STYLE_CONTROL);
	
	i3Vector::Set( &m_vSize, 5.f, 5.f);
}

bool	i3GICControlPoint::OnSelect( bool bFlag)
{
	i3GICShape::OnSelect( bFlag);

	// Control Point의 경우에는 자신이 직접 선택되는 경우는 없다.
	// OnSelect() 함수가 호출되는 시점은 Owner Shape가 선택되었을 때이며,
	// 이때는 화면에 표출되었다가,
	// 선택 해제가 되면 화면에서 사라져야 한다.

	if( bFlag)	removeState( I3GIC_STATE_DISABLED);
	else		addState( I3GIC_STATE_DISABLED);

	return true;
}

void i3GICControlPoint::OnDraw( i3GICDiagramContext * pCtx)
{
	if( !isStyle( I3GIC_STYLE_INVISIBLE) )
	{
		POINT pt;

		pt.x = (INT32) getPosX();
		pt.y = (INT32) getPosY();

		pCtx->GetDrawPos( &pt);

		pCtx->toDP( &pt);

		pCtx->SetModeToDevice();

		pCtx->selectObject( ::GetStockObject( WHITE_BRUSH));
		pCtx->selectObject( pCtx->getCPPen());
		pCtx->DP_Ellipse( pt.x - CPSZ, pt.y - CPSZ, pt.x + CPSZ, pt.y + CPSZ);

		pCtx->SetModeToDraw();
	}

	i3GICShape::OnDraw( pCtx);
}

bool	i3GICControlPoint::OnHitTest( i3GICDiagramContext * pCtx, REAL32 x, REAL32 y)
{
	i3::pack::RECT rt;
	POINT sz, pos;

	if( isState( I3GIC_STATE_DISABLED))				return false;
	if( isStyle( I3GIC_STYLE_NOHITTEST))			return false;
	
	sz.x = CPSZ;
	sz.y = CPSZ;
	pCtx->toLP( &sz);

	pos.x = (INT32) getPosX();
	pos.y = (INT32) getPosY();

	pCtx->GetDrawPos( &pos);

	rt.left		= (REAL32)(pos.x - sz.x);
	rt.top		= (REAL32)(pos.y - sz.y);
	rt.right	= (REAL32)(pos.x + sz.x);
	rt.bottom	= (REAL32)(pos.y + sz.y);

	i3Math::NormalizeRect( &rt);

	VEC2D cpos;
	cpos.x = x;
	cpos.y = y;

	if( i3Math::PtInRect( &rt, &cpos))
	{
		pCtx->getCurrentHitResult()->m_pHitShape = this;
		pCtx->getCurrentHitResult()->m_Semantic = getCPStyle();
		pCtx->getCurrentHitResult()->m_rDepth = getDepth();
		
		return true;
	}

	return false;
}

/*virtual*/ void i3GICControlPoint::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GICShape::CopyTo( pDest, method);

	i3GICControlPoint * pCP = (i3GICControlPoint*) pDest;

	pCP->setCPStyle( getCPStyle());
}

#endif

