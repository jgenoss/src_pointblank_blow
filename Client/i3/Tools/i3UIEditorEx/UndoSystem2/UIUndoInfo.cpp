//#include "i3FrameworkPCH.h"
//#include "UndoSystem/UIUndoInfo.h"
#include "stdafx.h"
#include "UIUndoInfo.h"
#include "UIUndoSystem.h"
#include "i3Base/itl/container_util.h"

//#include "../../i3Framework.h"

//#if defined( I3_DISTRIBUTE)

I3_CLASS_INSTANCE( UIUndoInfo); //, i3NamedElement);

INT32	g_nElementID = 0;

UIUndoInfo::UIUndoInfo(void)
{
	m_pElement	= NULL;
}

UIUndoInfo::~UIUndoInfo(void)
{
	i3::cu::for_each_safe_release(m_ChildList, &UIUndoInfo::Release);
	I3_SAFE_RELEASE( m_pElement);
}

void UIUndoInfo::AddChildUndoInfo( UIUndoInfo * pChildInfo)
{
	if( pChildInfo == NULL)
		return;
	m_ChildList.push_back( pChildInfo);
}

INT32 UIUndoInfo::GetElementList( i3::vector<i3UIControl*>& List)
{
	List.clear();

	if( !m_ChildList.empty() )
	{
		const size_t undoCnt = m_ChildList.size();
		for( size_t i = 0 ; i < undoCnt; ++i)
		{
			UIUndoInfo * pInfo = m_ChildList[i];
			I3ASSERT( pInfo != NULL);

			List.push_back( pInfo->getObject());
		}
	}
	else
	{
		List.push_back( m_pElement);
	}

	return List.size();
}

bool UIUndoInfo::Undo( i3UIScene* pScene)
{
	if( !m_ChildList.empty() )
	{
		//Undo는 역순으로 실행
		const size_t nChildCount = m_ChildList.size();
		for( size_t i = nChildCount  ; i > 0; --i)
		{
			UIUndoInfo * pInfo = m_ChildList[i - 1];
			I3ASSERT( pInfo != NULL);
			pInfo->Undo( pScene);
		}
	}

	return false;
}

bool UIUndoInfo::Redo( i3UIScene* pScene)
{
	if(  !m_ChildList.empty() )
	{
		const size_t nChildCount = m_ChildList.size();
		for( size_t i = 0; i < nChildCount; ++i)
		{
			UIUndoInfo * pInfo = m_ChildList[i];
			I3ASSERT( pInfo != NULL);
			pInfo->Redo( pScene);
		}
	}

	return false;
}

void UIUndoInfo::PrintUndoInfo( void)
{
	I3TRACE( "UndoInfo: %d child\n", (INT32)m_ChildList.size());
	for( size_t i=0; i< m_ChildList.size(); i++)
	{
		I3TRACE( "-");
		UIUndoInfo* pInfo = m_ChildList[i];
		pInfo->PrintUndoInfo();
	}
}

//#endif	// I3_DISTRIBUTE

