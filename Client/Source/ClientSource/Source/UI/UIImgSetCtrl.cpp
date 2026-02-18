#include "pch.h"
#include "UIImgSetCtrl.h"

#include "UIMath.h"

void				UIImgSetTplGroup::AddTemplate(i3UITemplate * pTemplate)
{
	m_tpls.push_back(pTemplate);
}

i3UITemplate*		UIImgSetTplGroup::GetTemplate(INT32 idx) const
{
	return m_tpls[idx];
}

INT32				UIImgSetTplGroup::GetNumTemplate() const
{
	return (INT32)m_tpls.size();
}

UIImgSetCtrl*		UIImgSetTplGroup::CreateImgSetCtrl( i3UIButtonImageSet* bound_rsc ) const
{
	UIImgSetCtrl* res = UIImgSetCtrl::new_object();
	res->Initialize(this, bound_rsc);
	return res;
}

UIImgSetCtrl*		UIImgSetTplGroup::CreateImgSetCtrl( i3UIScene* scn, const char* bound_rsc) const
{
	i3UIButtonImageSet* bound_ctrl = static_cast<i3UIButtonImageSet*>(scn->FindChildByName(bound_rsc));
	I3ASSERT(bound_ctrl);
	return UIImgSetTplGroup::CreateImgSetCtrl(bound_ctrl);
}


I3_CLASS_INSTANCE( UIImgSetCtrl);//, i3ElementBase);

UIImgSetCtrl::~UIImgSetCtrl()
{
}

void	UIImgSetCtrl::RestoreSizePos()
{
	// 실제 리소스는 소거가 안되었을수도 있다...원래의 사이즈로 복귀시킨다..
	m_pBoundImage->setPos(&m_vBoundImage_OrigPos);
	m_pBoundImage->setSize(&m_vBoundImage_OrigSize);
}

void	UIImgSetCtrl::Initialize(const UIImgSetTplGroup* tpl_grp, i3UIButtonImageSet* bound_rsc)
{
	m_tpl_grp = tpl_grp;	m_pBoundImage = bound_rsc;
	m_pBoundImage->SetViewOnly( true);
	
	m_vBoundImage_OrigPos = *m_pBoundImage->getPos();
	m_vBoundImage_OrigSize = *m_pBoundImage->getSize();

	m_vBoundImage_Pos = m_vBoundImage_OrigPos;

	VEC2D derivedPos = _GetDerivedPos();
	m_vOffset.x = m_vBoundImage_OrigPos.x - derivedPos.x;
	m_vOffset.y = m_vBoundImage_OrigPos.y - derivedPos.y;

	if (m_pBoundImage->getTemplate())
	UIImgSetCtrl::_AutoResize( m_pBoundImage->getTemplate() );
}

void UIImgSetCtrl::_Resize(REAL32 scale_w, REAL32 scale_h)
{
	REAL32 rRateY = getY( &m_vBoundImage_OrigSize ) /	scale_h;			// pShape->getHeight();
	REAL32 rRateX = getX( &m_vBoundImage_OrigSize ) /	scale_w;		// pShape->getWidth();

	VEC2D vSize;
	
	if ( rRateX >= 1.f && rRateY >= 1.f )		// 이경우엔 셰이프원본보다 바탕이미지 사이즈가 더 큰 확대 이미지가 되는데, 그다지 좋아보이지 않아 원래의 사이즈를 채용..(아이템의 경우)
	{
		vSize.x = scale_w; 
		vSize.y = scale_h;
	}
	else
	{
		if( rRateY < rRateX)
			i3Vector::Set( &vSize, scale_w * rRateY, getY(&m_vBoundImage_OrigSize ));
		else
			i3Vector::Set( &vSize, getX(&m_vBoundImage_OrigSize ), scale_h * rRateX);
	}

	VEC3D vCenter, vPos;
	i3Vector::Set( &vCenter, getX(&m_vBoundImage_OrigSize ) * 0.5f, getY( &m_vBoundImage_OrigSize ) * 0.5f, 0.f);
	i3Vector::Add( &vPos, &m_vBoundImage_Pos, &vCenter);
	i3Vector::Set( &vPos, getX( &vPos) - getX( &vSize) * 0.5f, getY( &vPos) - getY( &vSize) * 0.5f, 0.f);

	m_pBoundImage->setPos( &vPos);
	m_pBoundImage->setSize( &vSize);
}

void	UIImgSetCtrl::SetBoundImageSize( i3UITemplate* tpl )
{
	I3ASSERT(tpl);
	i3UIShape* pShape = tpl->getShape(0);
	I3ASSERT( pShape);
	I3ASSERT( pShape->getHeight() > 0 && pShape->getWidth() > 0);

	m_vTex_OrigSize.x =	pShape->getCoordW();
	m_vTex_OrigSize.y = pShape->getCoordH();

	VEC2D vSize = m_vTex_OrigSize;
	VEC3D vCenter, vPos;
	i3Vector::Set( &vCenter, getX(&m_vBoundImage_OrigSize ) * 0.5f, getY( &m_vBoundImage_OrigSize ) * 0.5f, 0.f);
	i3Vector::Add( &vPos, &m_vBoundImage_Pos, &vCenter);
	i3Vector::Set( &vPos, getX( &vPos) - getX( &vSize) * 0.5f, getY( &vPos) - getY( &vSize) * 0.5f, 0.f);

	m_pBoundImage->setPos( &vPos);
	m_pBoundImage->setSize( &vSize);
}
void	UIImgSetCtrl::_AutoResize( i3UITemplate* tpl )
{
	I3ASSERT(tpl);
	i3UIShape* pShape = tpl->getShape(0);
	I3ASSERT( pShape);
	I3ASSERT( pShape->getHeight() > 0 && pShape->getWidth() > 0);

	m_vTex_OrigSize.x =	pShape->getCoordW();
	m_vTex_OrigSize.y = pShape->getCoordH();

	_Resize( m_vTex_OrigSize.x, m_vTex_OrigSize.y );
}

void	UIImgSetCtrl::SetImage(INT32 img_idx)
{
	if ( img_idx < 0) 
	{
		m_pBoundImage->EnableCtrl(false); return;
	}

	m_pBoundImage->SetShapeIdx(img_idx);
	m_pBoundImage->EnableCtrl(true);
}


void	UIImgSetCtrl::SetImage(INT32 tpl_idx, INT32 img_idx)
{
	if ( tpl_idx < 0) 
	{
		m_pBoundImage->EnableCtrl(false); return;
	}

	I3ASSERT( tpl_idx < m_tpl_grp->GetNumTemplate() );

	i3UITemplate* target_tpl = m_tpl_grp->GetTemplate(tpl_idx);

	if ( m_pBoundImage->getTemplate() != target_tpl)
	{
		UIImgSetCtrl::_AutoResize(target_tpl);
		m_pBoundImage->SetTemplate(target_tpl);
	}
	
	m_pBoundImage->SetShapeIdx(img_idx);
	m_pBoundImage->EnableCtrl(true);
}

void	UIImgSetCtrl::SetEnable(bool bEnable)
{
	m_pBoundImage->EnableCtrl(bEnable);
}

void UIImgSetCtrl::_translate()
{
	m_pBoundImage->setPos( m_vBoundImage_Pos.x, m_vBoundImage_Pos.y );
}

void UIImgSetCtrl::SetVisible(bool show)
{
	m_pBoundImage->SetVisible(show);
}




VEC2D UIImgSetCtrl::GetPos() const
{
	VEC3D* vec = m_pBoundImage->getPos();

	VEC2D ret;

	ret.x = vec->x;
	ret.y = vec->y;
	
	return ret;
}

void UIImgSetCtrl::SetPos(const VEC2D & pos)
{
	m_vBoundImage_Pos.x = pos.x;
	m_vBoundImage_Pos.y = pos.y;

	_translate();
}
void UIImgSetCtrl::SetPos(REAL32 x, REAL32 y)
{
	m_vBoundImage_Pos.x = x;
	m_vBoundImage_Pos.y = y;

	_translate();
}

void UIImgSetCtrl::AddPos(const VEC2D & pos)
{
	m_vBoundImage_Pos.x += pos.x;
	m_vBoundImage_Pos.y += pos.y;
	
	_translate();
}

void UIImgSetCtrl::AddPos(REAL32 x, REAL32 y)
{
	m_vBoundImage_Pos.x += x;
	m_vBoundImage_Pos.y += y;
	
	_translate();
}





VEC2D UIImgSetCtrl::_GetDerivedPos() const
{
	m_pBoundImage->CalcAccumulatedPos();

	VEC3D* vec = m_pBoundImage->getAccumulatedPos();

	VEC2D ret;

	ret.x = vec->x;
	ret.y = vec->y;
	
	return ret;
}

void UIImgSetCtrl::_SetDerivedPos(const VEC2D & pos)
{
	m_vBoundImage_Pos.x = m_vOffset.x + pos.x;
	m_vBoundImage_Pos.y = m_vOffset.y + pos.y;

	_translate();
}
void UIImgSetCtrl::_SetDerivedPos(REAL32 x, REAL32 y)
{
	m_vBoundImage_Pos.x = m_vOffset.x + x;
	m_vBoundImage_Pos.y = m_vOffset.y + y;

	_translate();
}

void UIImgSetCtrl::_AddDerivedPos(const VEC2D & pos)
{
	m_vBoundImage_Pos.x += pos.x;
	m_vBoundImage_Pos.y += pos.y;
	
	_translate();
}

void UIImgSetCtrl::_AddDerivedPos(REAL32 x, REAL32 y)
{
	m_vBoundImage_Pos.x +=x;
	m_vBoundImage_Pos.y +=y;
	
	_translate();
}

void UIImgSetCtrl::Scale(REAL32 xy, bool force)
{
	Scale( xy, xy, force );
}


void UIImgSetCtrl::Scale(REAL32 x, REAL32 y, bool force)
{
	if( !force )
	{
		x = uiMath::limited(x, 0.0f, 1.0f);
		y = uiMath::limited(y, 0.0f, 1.0f);
	}

	REAL32 scale_w = m_vTex_OrigSize.x * x;
	REAL32 scale_h = m_vTex_OrigSize.y * y;

	_Resize( scale_w, scale_h );
}

void UIImgSetCtrl::SetAlpha(REAL32 alpha)
{
	COLOR* color = m_pBoundImage->getColor();

	alpha = uiMath::limited(alpha, 0.0f, 1.0f);
	color->a = (UINT8)(255 * alpha);

	m_pBoundImage->setColor( color );
}

void UIImgSetCtrl::SetOriginPos()
{
	m_pBoundImage->setPos( &m_vBoundImage_OrigPos );
}

void UIImgSetCtrl::SetOriginScale()
{
	m_pBoundImage->setSize( &m_vBoundImage_OrigSize );
}

void UIImgSetCtrl::SetOriginAlpha()
{
	SetAlpha(1.0f);
}
