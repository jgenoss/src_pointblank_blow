//#include "i3FrameworkPCH.h"
#include "stdafx.h"
//#include "../../i3Framework.h"

#include "UIUndoInfoCreate.h"
#include "i3UIManager.h"

//#if defined( I3_DISTRIBUTE)

I3_CLASS_INSTANCE( UIUndoInfoCreate); //, UIUndoInfo);

UIUndoInfoCreate::UIUndoInfoCreate(void)
{
	m_pParent = NULL;
}

UIUndoInfoCreate::~UIUndoInfoCreate(void)
{
	I3_SAFE_RELEASE( m_pParent);
}

void UIUndoInfoCreate::setObject( i3UIControl * pObj)
{
//	I3ASSERT( pObj != NULL);
	I3ASSERT( i3::kind_of<i3UIControl*>(pObj)); //->IsTypeOf( i3UIControl::static_meta()) );

	UIUndoInfo::setObject( pObj);

	I3_REF_CHANGE( m_pParent, pObj->getParent());

	i3TDK::Update( NULL, I3_TDK_UPDATE_ADD, pObj, pObj->getTemplate()->GetInstanceMeta());
}

bool UIUndoInfoCreate::Undo( i3UIScene * pScene)
{
	i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, m_pElement);

	i3UI::DeleteControl( m_pElement);

	UIUndoInfo::Undo( pScene);
	
	return true;
}

bool UIUndoInfoCreate::Redo( i3UIScene * pScene)
{
	i3UI::ReCreateControl( m_pElement, m_pParent);
	
	i3TDK::Update( NULL, I3_TDK_UPDATE_ADD, m_pElement, m_pElement->getTemplate()->GetInstanceMeta());

	UIUndoInfo::Redo( pScene);

	return true;
}

void UIUndoInfoCreate::PrintUndoInfo( void)
{
	I3TRACE("UIUndoInfoCreate\n");
}

//#endif	// I3_DISTRIBUTE
