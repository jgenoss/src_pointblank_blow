#include "stdafx.h"
#include "i3LevelUndoInfo.h"
#include "i3Base/itl/container_util.h"

I3_CLASS_INSTANCE( i3LevelUndoInfo);

i3LevelUndoInfo::~i3LevelUndoInfo(void)
{
	i3::cu::for_each_container(m_UndoList, i3::fu::mem_fun_safe(&i3LevelUndoInfo::Release));
	
	I3_SAFE_RELEASE( m_pElement);
}

void i3LevelUndoInfo::AddChildUndoInfo( i3LevelUndoInfo * pChildInfo)
{
	if( pChildInfo == nullptr)
		return;

	m_UndoList.push_back(pChildInfo);
}

bool i3LevelUndoInfo::Undo(i3LevelScene * pScene)
{
	if( !m_UndoList.empty() )
	{
		//Undo는 역순으로 실행
		INT32 undoCnt = (INT32)m_UndoList.size();
		for( INT32 i = undoCnt -1 ; i >= 0; --i)
		{
			i3LevelUndoInfo * pInfo = m_UndoList[i];
			I3ASSERT( pInfo != nullptr);

			pInfo->Undo( pScene);
		}
	}

	return false;
}

bool i3LevelUndoInfo::Redo(i3LevelScene * pScene)
{
	if( !m_UndoList.empty() )
	{
		//Undo는 역순으로 실행
		INT32 undoCnt = (INT32)m_UndoList.size();
		for( INT32 i = 0; i < undoCnt; ++i)
		{
			i3LevelUndoInfo * pInfo = m_UndoList[i];
			I3ASSERT( pInfo != nullptr);

			pInfo->Redo( pScene);
		}
	}

	return false;
}