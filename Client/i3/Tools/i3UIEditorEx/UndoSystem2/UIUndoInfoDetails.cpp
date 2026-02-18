//#include "i3FrameworkPCH.h"
#include "stdafx.h"
//#include "../../i3Framework.h"

#include "UIUndoInfoDetails.h"
#include "UIUndoSystem.h"

//#if defined( I3_DISTRIBUTE)

I3_ABSTRACT_CLASS_INSTANCE( UIUndoInfoDetails, UIUndoInfo);

UIUndoInfoDetails::UIUndoInfoDetails()
{
	m_pInfo = NULL;
}

UIUndoInfoDetails::~UIUndoInfoDetails()
{
	I3MEM_SAFE_FREE( m_pInfo);
}

void UIUndoInfoDetails::_rec_AddToFriendList( UIUndoInfo* pInfo, INT32 nElementID, i3List* pList)
{
	if( pInfo->GetChildInfoCount() > 0)
	{
		i3List* pChildList = pInfo->GetChildInfoList();

		for( INT32 i=0; i< pChildList->GetCount(); i++)
		{
			UIUndoInfo* pChildInfo = (UIUndoInfo*)pChildList->GetItem( i);
			_rec_AddToFriendList( pChildInfo, nElementID, pList);
		}
	}

	if( pInfo->getElementID() == nElementID && pInfo != this)
	{
		pList->Add( pInfo);
	}
}

void UIUndoInfoDetails::UpdateFriendList( void)
{
	INT32 cnt = 0;

	m_FriendList.Clear();

	UIUndoSystem* pSystem = UIUndo::getUndoSystem();
	if( pSystem != NULL)
	{
		i3List list;
		pSystem->getInfoList( &list);

		//Trace
		/*I3TRACE("----------------------------------------\n");
		for( INT32 i=0; i< list.GetCount(); i++)
		{
			UIUndoInfo* pInfo = (UIUndoInfo*)list.GetItem( i);
			I3TRACE("%d: %d\n", i, pInfo->getElementID());
		}
		I3TRACE("----------------------------------------\n");*/

		for( INT32 i=list.GetCount()-1; i >= 0 ; i--)
		{
			UIUndoInfo* pInfo = (UIUndoInfo*)list.GetItem( i);

			//recursive... 
			_rec_AddToFriendList( pInfo, m_nElementID, &m_FriendList);

			if( pInfo->getElementID() == m_nElementID && pInfo != this)
			{
				m_FriendList.Add( pInfo);
				cnt++;
			}
		}
	}

	I3TRACE("UpdateFriendList: %d\n", cnt);
}

void UIUndoInfoDetails::ShareElementWithFriends( void)
{
	INT32 cnt = 0;

	for( INT32 i=0; i< m_FriendList.GetCount(); i++)
	{
		UIUndoInfo* pInfo = (UIUndoInfo*)m_FriendList.GetItem( i);

		pInfo->UIUndoInfo::setObject( m_pElement);

		cnt++;
	}

	I3TRACE("ShareElementWithFriends: %d\n", cnt);
}




//#endif	// I3_DISTRIBUTE
