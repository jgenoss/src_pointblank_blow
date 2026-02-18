//#include "i3FrameworkPCH.h"
#include "stdafx.h"
//#include "../../i3Framework.h"

#include "UIUndoInfoDelete.h"
#include "i3UIManager.h"

//#if defined( I3_DISTRIBUTE)

I3_CLASS_INSTANCE( UIUndoInfoDelete); //, UIUndoInfo);

UIUndoInfoDelete::UIUndoInfoDelete(void)
{
	m_pParent = NULL;
}

UIUndoInfoDelete::~UIUndoInfoDelete(void)
{
	I3_SAFE_RELEASE( m_pParent);
}

void UIUndoInfoDelete::setObject( i3UIControl * pObj)
{
//	I3ASSERT( pObj != NULL);
	I3ASSERT( i3::kind_of<i3UIControl*>(pObj)); //->IsTypeOf( i3UIControl::static_meta()) );

	UIUndoInfo::setObject( pObj);

	I3_REF_CHANGE( m_pParent, pObj->getParent());

	i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, pObj);
}

bool UIUndoInfoDelete::Undo( i3UIScene * pScene)
{
	i3UI::ReCreateControl( m_pElement, m_pParent);
	
	i3TDK::Update( NULL, I3_TDK_UPDATE_ADD, m_pElement, m_pElement->getTemplate()->GetInstanceMeta());

	UIUndoInfo::Undo( pScene);
	
	return true;
}

bool UIUndoInfoDelete::Redo( i3UIScene * pScene)
{
	i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, m_pElement);

	i3UI::DeleteControl( m_pElement);

	UIUndoInfo::Redo( pScene);

	return true;
}

void UIUndoInfoDelete::PrintUndoInfo( void)
{
	I3TRACE("UIUndoInfoDelete\n");
}

//#endif	// I3_DISTRIBUTE
