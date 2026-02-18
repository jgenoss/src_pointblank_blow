#include "stdafx.h"
#include "i3TDKUpdateSystem.h"

I3_CLASS_INSTANCE( i3TDKUpdateSystem);

i3TDKUpdateSystem::~i3TDKUpdateSystem(void)
{
	RemoveAllNotify();
}

INT32 i3TDKUpdateSystem::FindNotify( HWND hwnd, i3ClassMeta* pMeta, UINT32 EventMask)
{
	//해당 hwnd, pMeta, EventMask를 갖는 I3_TDK_UPDATE_REG_INFO가 m_UpdateList에 있는지 찾습니다.

	for( size_t i=0; i< m_UpdateList.size(); i++)
	{
		I3_TDK_UPDATE_REG_INFO* pInfo = m_UpdateList[i];
		
		I3ASSERT( pInfo != nullptr);

		if( pInfo->m_hWindow == hwnd && pInfo->m_EventMask == EventMask)
		{
			if( pInfo->m_pClassMeta == pMeta)
			{
				return i;
			}
		}
	}

	return -1;
}

void i3TDKUpdateSystem::AddNotify( HWND hwnd, i3ClassMeta * pMeta, UINT32 EventMask)
{
	I3_TDK_UPDATE_REG_INFO * pNew;
	//INT32 idx;
	//idx = i3MemAlloc(sizeof(I3_TDK_UPDATE_REG_INFO)); 
	pNew = (I3_TDK_UPDATE_REG_INFO *) i3MemAlloc(sizeof(I3_TDK_UPDATE_REG_INFO)); 

	memset( pNew, 0, sizeof(I3_TDK_UPDATE_REG_INFO));

	pNew->m_hWindow = hwnd;
	pNew->m_pClassMeta = pMeta;
	pNew->m_EventMask = EventMask;

	m_UpdateList.push_back( pNew);
}

void i3TDKUpdateSystem::RemoveNotify( INT32 idx)
{
	I3_TDK_UPDATE_REG_INFO * pInfo = m_UpdateList[idx];

	m_UpdateList.erase( m_UpdateList.begin() + idx);

	i3MemFree( pInfo);
}

void i3TDKUpdateSystem::RemoveNotify( HWND hwnd)
{
	I3_TDK_UPDATE_REG_INFO * pInfo;
	
	for( size_t i = 0; i < m_UpdateList.size(); i++)
	{
		pInfo = m_UpdateList[i];

		if( pInfo->m_hWindow == hwnd)
		{
			m_UpdateList.erase( m_UpdateList.begin() + i);

			i3MemFree( pInfo);
			i--;
		}
	}
}

void i3TDKUpdateSystem::RemoveAllNotify(void)
{

	I3_TDK_UPDATE_REG_INFO * pInfo;

	for( size_t i = 0; i < m_UpdateList.size(); i++)
	{
		pInfo = m_UpdateList[i];

		i3MemFree( pInfo);
	}

	m_UpdateList.clear();
}


void i3TDKUpdateSystem::Update( HWND hwnd, UINT32 event, i3ElementBase * pObj, i3ClassMeta * pMeta)
{
	I3_TDK_UPDATE_REG_INFO * pInfo;
	UINT32 evt;
	bool bNotify;
	I3_TDK_UPDATE_INFO Data;

	if( m_bPending)
		return;

	if(pMeta == nullptr)
	{
		I3ASSERT( pObj != nullptr);
		pMeta = pObj->meta();
	}

	Data.m_hwndFrom = hwnd;
	Data.m_Event = event;
	Data.m_pMeta = pMeta;
	Data.m_pObject = pObj;

	for(size_t i = 0; i < m_UpdateList.size(); i++)
	{
		pInfo = m_UpdateList[i];

		bNotify = false;

		evt = pInfo->m_EventMask & event;
		if( evt != 0)
		{
			if((pMeta != nullptr) && (pObj != nullptr))
			{
				if( pInfo->m_pClassMeta == nullptr)
				{
					bNotify = true;
				}
				else if( pMeta->kind_of( pInfo->m_pClassMeta))
				{
					bNotify = true;
				}
			}
			else if( pMeta != nullptr)
			{
				if( pInfo->m_pClassMeta != nullptr &&
					pMeta->kind_of( pInfo->m_pClassMeta))
					bNotify = true;
			}
			/*else
			{
				bNotify = true;
			}*/
		}

		if( bNotify)
			SendMessage( pInfo->m_hWindow, WM_TDK_UPDATE, (WPARAM) &Data, (LPARAM)nullptr);
	}
}

//--------------------------------------------------------------------------------
static i3TDKUpdateSystem	s_UpdateSys;

I3_EXPORT_TDK
void i3TDK::RegisterUpdate( HWND hwnd, i3ClassMeta * pMeta, UINT32 EventMask)
{
	//일단 찾아보고 없는 경우에만 추가합니다. 
	if( s_UpdateSys.FindNotify( hwnd, pMeta, EventMask) == -1)
		s_UpdateSys.AddNotify( hwnd, pMeta, EventMask);
}

I3_EXPORT_TDK
void i3TDK::Update( HWND hwnd, UINT32 event, i3ElementBase * pObj, i3ClassMeta * pMeta)
{
	s_UpdateSys.Update( hwnd, event, pObj, pMeta);
}

I3_EXPORT_TDK	
bool i3TDK::GetPendingUpdateState(void)
{
	return s_UpdateSys.getPendingState();
}

I3_EXPORT_TDK	
void i3TDK::SetPendingUpdateState( bool bState)
{
	s_UpdateSys.setPendingState( bState); 
}
