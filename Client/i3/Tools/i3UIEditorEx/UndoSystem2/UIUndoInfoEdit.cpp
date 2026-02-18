//#include "i3FrameworkPCH.h"
#include "stdafx.h"
//#include "../../i3Framework.h"

//#if defined( I3_DISTRIBUTE)

#include "UIUndoInfoEdit.h"

I3_CLASS_INSTANCE( UIUndoInfoEdit); //, UIUndoInfo);

UIUndoInfoEdit::UIUndoInfoEdit(void)
{
	m_pClone = NULL;
}

UIUndoInfoEdit::~UIUndoInfoEdit(void)
{
	if( m_pClone != NULL)
	{
		i3GameNode* pParent = m_pClone->getParent();
		if( pParent != NULL)
		{
			pParent->RemoveChild( m_pClone);	//릴리즈
		}

		m_pClone->EnableCtrl( FALSE);

		I3_SAFE_RELEASE( m_pClone);
		/* 
		>>> parent에 해제 요청이 먼저 들어오는 경우엔 m_pClone이 NULL이 아니기 때문에 문제를 일으킬 수 있다.
		>>> 그래서 명시적으로 adref를 이용하여 refcount를 2 로 만든후 각자 알아서 release해준다.

		i3GameNode* pParent = m_pClone->getParent();
		if( pParent != NULL)
			pParent->RemoveChild( m_pClone);	//릴리즈
		else
			I3_SAFE_RELEASE( m_pClone);

		m_pClone = NULL;
		
		*/
	}
}

void UIUndoInfoEdit::_MakeClone( void)
{
	I3ASSERT( m_pClone == NULL);
	I3ASSERT( m_pElement != NULL);

	m_pClone = i3UI::CreateCloneControl( m_pElement, false);
	m_pClone->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);

	m_pClone->EnableCtrl( false);		//절대로 Enable되서는 안된다. 
}

void UIUndoInfoEdit::_Swap( void)
{
	BOOL bEnable = m_pElement->isEnable();

	i3UIControl* pTemp = i3UI::CreateCloneControl( m_pElement, false);

	m_pClone->CopyTo( m_pElement, I3_COPY_INSTANCE);
	
	pTemp->CopyTo( m_pClone, I3_COPY_INSTANCE);

	// release temp
	I3_SAFE_RELEASE( pTemp);
	
	m_pClone->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
	m_pClone->EnableCtrl( false);

	m_pElement->EnableCtrl( bEnable ? true : false);	//원래 Enable되어 있었다면 Enable시켜준다.
	
	m_pElement->removeGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);

	m_pElement->setSize( m_pElement->getSize());
}

void UIUndoInfoEdit::setObject( i3UIControl * pObj)
{
//	I3ASSERT( pObj != NULL);
	I3ASSERT( i3::kind_of<i3UIControl*>(pObj)); //->IsTypeOf( i3UIControl::static_meta()) );

	UIUndoInfo::setObject( pObj);

	_MakeClone();
}

bool UIUndoInfoEdit::Undo( i3UIScene * pScene)
{
	_Swap();

	UIUndoInfo::Undo( pScene);

	return true;
}

bool UIUndoInfoEdit::Redo( i3UIScene * pScene)
{
	_Swap();

	UIUndoInfo::Redo( pScene);

	return false;
}

void UIUndoInfoEdit::PrintUndoInfo( void)
{
	I3TRACE("UIUndoInfoEdit\n");
}

//#endif	// I3_DISTRIBUTE
