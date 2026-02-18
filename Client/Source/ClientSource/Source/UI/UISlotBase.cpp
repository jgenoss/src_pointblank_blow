#include "pch.h"
#include "UISlotBase.h"

//I3_ABSTRACT_CLASS_INSTANCE( UISlotBase, UIBase);

I3_CLASS_INSTANCE( UISlotBase);

UISlotBase::UISlotBase()
{	
	ZeroMemory(&m_Rect, sizeof(i3::pack::RECT));
}

UISlotBase::~UISlotBase()
{
}

void UISlotBase::CalculatePos( VEC3D * pOut, INT32 argX, INT32 argY, VEC3D * pOffset, VEC2D * pSize, REAL32 px, REAL32 py)
{
	VEC3D vPos;
	I3ASSERT( pSize != nullptr);

	if( pOffset != nullptr)
		i3Vector::Copy( &m_vOffset, pOffset);

	if( pSize != nullptr)
		i3Vector::Copy( &m_vSize, pSize);

	i3Vector::Set( &vPos, getX( &m_vOffset) + (getX( &m_vSize) * argX) + px, getY( &m_vOffset) + (getY( &m_vSize) * argY) + py, 0.f);
	SetPos( &vPos);

	if( pOut != nullptr)
		i3Vector::Copy( pOut, &vPos);
}

/*virtual*/ void UISlotBase::EnableSlot( INT32 argX, INT32 argY, VEC3D * pOffset, VEC3D * pOut, REAL32 px, REAL32 py)
{
	SetEnable( true);

	if( pOffset != nullptr)
		i3Vector::Copy( &m_vOffset, pOffset);

	CalculatePos( pOut, argX, argY, &m_vOffset, &m_vSize, px, py);

	if( m_pFrameWnd)
	{
		m_Rect.left = 0.f;
		m_Rect.top = 0.f;

		m_Rect.right = getX( m_pFrameWnd->getSize());
		m_Rect.bottom = getY( m_pFrameWnd->getSize());
	}
}

/*virtual*/ void UISlotBase::EnableSlot2( REAL32 totalHeight, REAL32 totalXOffset, REAL32 totalYOffset,VEC3D * pOffset, VEC3D * pOut)
{
	SetEnable( true);

	VEC3D	vPos;
	if( pOffset != nullptr)
		i3Vector::Copy( &m_vOffset, pOffset);

	i3Vector::Set( &vPos, getX( &m_vOffset) + totalXOffset, getY( &m_vOffset) + totalHeight + totalYOffset, 0.f);
	SetPos( &vPos);

	if( m_pFrameWnd)
	{
		m_Rect.left = 0.f;
		m_Rect.top = 0.f;

		m_Rect.right = getX( m_pFrameWnd->getSize());
		m_Rect.bottom = getY( m_pFrameWnd->getSize());
	}
}

void UISlotBase::SetPos( VEC3D * pPos)
{
	if( m_pFrameWnd)
	{	
		m_pFrameWnd->setPos( pPos);
	}
}

VEC3D *	UISlotBase::GetPos()
{
	if (m_pFrameWnd) return m_pFrameWnd->getPos();
	return &m_vOriginalPos;
}

VEC2D * UISlotBase::GetSize( void)
{
	if( m_pFrameWnd)
		return m_pFrameWnd->getSize();

	return &m_vSize;
}

