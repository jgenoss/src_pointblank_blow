//#include "i3FrameworkPCH.h"
#include "stdafx.h"
//#include "../../i3Framework.h"

#include "UIUndoInfoTransform.h"

//#if defined( I3_DISTRIBUTE)

I3_CLASS_INSTANCE( UIUndoInfoTransform); //, UIUndoInfo);

UIUndoInfoTransform::UIUndoInfoTransform(void)
{
	i3Vector::Set( &m_vPos, 0.0f, 0.0f, 0.0f);
	i3Vector::Set( &m_vSize, 0.0f, 0.0f);
}

UIUndoInfoTransform::~UIUndoInfoTransform(void)
{

}

void UIUndoInfoTransform::setObject( i3UIControl * pObj)
{
	//ASSERT에 걸린다면 어차피 동작불가!!! 세워버린다.
//	I3ASSERT( pObj != NULL);
	I3ASSERT( i3::kind_of<i3UIControl*>(pObj)); //->IsTypeOf( i3UIControl::static_meta()) );

	UIUndoInfo::setObject( pObj);

	i3Vector::Copy( &m_vPos, pObj->getPos());
	i3Vector::Copy( &m_vSize, pObj->getSize());

}

bool UIUndoInfoTransform::Undo( i3UIScene * pScene)
{
	I3ASSERT( m_pElement != NULL);

	VEC3D vNewPos;
	VEC2D vNewSize;

	i3Vector::Copy( &vNewPos, m_pElement->getPos());
	i3Vector::Copy( &vNewSize, m_pElement->getSize());

	m_pElement->setPos( &m_vPos);
	m_pElement->setSize( &m_vSize);

	i3Vector::Copy( &m_vPos, &vNewPos);
	i3Vector::Copy( &m_vSize, &vNewSize);

	UIUndoInfo::Undo( pScene);
	
	return true;
}

bool UIUndoInfoTransform::Redo( i3UIScene * pScene)
{
	I3ASSERT( m_pElement != NULL);

	VEC3D vNewPos;
	VEC2D vNewSize;

	i3Vector::Copy( &vNewPos, m_pElement->getPos());
	i3Vector::Copy( &vNewSize, m_pElement->getSize());

	m_pElement->setPos( &m_vPos);
	m_pElement->setSize( &m_vSize);

	i3Vector::Copy( &m_vPos, &vNewPos);
	i3Vector::Copy( &m_vSize, &vNewSize);

	UIUndoInfo::Redo( pScene);

	return true;
}

void UIUndoInfoTransform::PrintUndoInfo( void)
{
	I3TRACE("UIUndoInfoTransform: %s, x:%d, y:%d, w:%d, h:%d\n", m_pElement->meta()->class_name().c_str(), 
		(INT32)m_vPos.x, (INT32)m_vPos.y, (INT32)m_vSize.x, (INT32)m_vSize.y);
	// static_meta -> meta로 변경..(변경이유는 확실한데...고쳐진 meta()인 경우 m_pElement가 널값이 아니어야한다..) (2012.10.26.수빈)
}

//#endif
