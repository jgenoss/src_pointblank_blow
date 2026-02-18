#include "pch.h"
#include "i3LevelDesign.h"
#include "CTimeEventCtrl.h"
#include "DlgActionSetWorldSection.h"
#include "DlgSetMapSectorIndex.h"
#include "GlobalVariable.h"

CTimeEventCtrl::CTimeEventCtrl(void)
{
	m_CurAction = NULL;
}

CTimeEventCtrl::~CTimeEventCtrl(void)
{

}

void CTimeEventCtrl::_OnActionPlaySound( i3TimeEventInfo * pInfo, i3ActionPlaySound * pAction)
{
	i3TDKActionSoundCtrl dlg(this);

	dlg.SetAction( pAction);

	i3TDK::SetResInstance();

	if( dlg.DoModal() == IDOK)
	{
		HWND hwndParent = ::GetParent( m_hWnd);

		::PostMessage( hwndParent, WM_USER_TIME_EVENT_EDITED, 0, 0);
	}

	i3TDK::RestoreResInstance();

	i3ActionPlaySound* pSAction = (i3ActionPlaySound*)pAction;

	i3SoundNode * pSNode = pSAction->getSoundNode();

	if( pSNode != NULL)
	{
		i3SoundPlayInfo * pPlayInfo = pSNode->getSoundPlayInfo();
		if( pPlayInfo != NULL)
		{
			i3Sound * pSnd = pPlayInfo->getSound();
			if( pSnd != NULL)
			{
				pInfo->setDuration( pSnd->GetDuration());
				//REAL32 duration = 0.0f;
/*
				//update duration
				UINT32 i;
				i3TimeEventInfo * pInfo;

				duration = 0.0f;
				for( i = 0; i < m_pTimeEvent->getEventCount(); i++)
				{
					pInfo = (i3TimeEventInfo *) m_pTimeEvent->getEvent( i);

					duration = MAX( duration, pInfo->getDuration() + pInfo->getStartTime());
				}

				m_pTimeEvent->setDuration( duration);
				*/
			}
		}
	}
}

void CTimeEventCtrl::_OnActionSetMapSector( i3TimeEventInfo * pInfo, i3ActionSetMapSector * pAction)
{
	CDlgSetMapSectorIndex dlg;

	if( dlg.Execute( pAction))
	{
		HWND hwndParent = ::GetParent( m_hWnd);

		::PostMessage( hwndParent, WM_USER_TIME_EVENT_EDITED, 0, 0);
	}
}

void CTimeEventCtrl::_OnActionSetWorldSection( i3TimeEventInfo * pInfo, i3ActionSetWorldSection * pAction)
{
	CDlgActionSetWorldSection dlg;

	if( dlg.Execute( pAction))
	{
		HWND hwndParent = ::GetParent( m_hWnd);

		::PostMessage( hwndParent, WM_USER_TIME_EVENT_EDITED, 0, 0);
	}
}

void CTimeEventCtrl::_OnActionQuerySpecDialog( i3TimeEventInfo * pInfo, i3Action * pAction)
{
	INT32 i, cnt;
	i3ClassMeta * pMeta;
	i3GameObjPropertyDialog * pDlg;

	if( g_pInterface == NULL)
		return;

	cnt = g_pInterface->OnQueryPropertyDlgCount();

	for( i = 0; i < cnt; i++)
	{
		g_pInterface->OnQueryPropertyDlg( g_hInterface, i, &pMeta, &pDlg);

		if( pAction->kind_of( pMeta) && (pDlg != NULL))
		{
			pDlg->SetObject( pAction);

			pDlg->DoModal();
		}
	}
}

void CTimeEventCtrl::OnPopupHotEdit( INT32 idx)
{
	if( idx == -1)
		return;

	i3TimeEventInfo * pInfo;

	pInfo = m_pTimeEvent->getEvent( idx);

	pInfo->addState( I3_TIMEEVENT_STATE_SELECTED);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pInfo);

	if( pInfo->getAction() != NULL)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pInfo->getAction());
	}

	if( pInfo->getAction() != NULL)
	{
		i3Action * pAction = (i3Action*)pInfo->getAction();

		if( m_CurAction != pAction)
		{
			if( i3::kind_of<i3ActionPlaySound*>(pAction)) //->IsTypeOf( i3ActionPlaySound::static_meta()))
			{
				_OnActionPlaySound( pInfo, (i3ActionPlaySound *) pAction);
			}
			else if( i3::same_of<i3ActionSetWorldSection*>(pAction)) //->IsExactTypeOf( i3ActionSetWorldSection::static_meta()))
			{
				_OnActionSetWorldSection( pInfo, (i3ActionSetWorldSection *) pAction);
			}
			else if( i3::same_of<i3ActionSetMapSector*>(pAction)) //->IsExactTypeOf( i3ActionSetMapSector::static_meta()))
			{
				_OnActionSetMapSector( pInfo, (i3ActionSetMapSector *) pAction);
			}
			else
			{
				_OnActionQuerySpecDialog( pInfo, pAction);
			}
		}

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pInfo->getAction());
		
	}
	else
	{
		m_CurAction = NULL;
	}

	Invalidate();

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_EDIT, m_pTimeEvent);
}

void CTimeEventCtrl::OnPopupMenu( CMenu * pPopup, NC_HIT hit, INT32 idx)
{
	if( idx == -1)
		return;

	i3TimeEventInfo * pInfo;

	pInfo = m_pTimeEvent->getEvent( idx);

	if( pInfo->getAction() != NULL)
	{
		i3Action * pAction = (i3Action*)pInfo->getAction();
		if( m_CurAction != pAction )
		{
			pPopup->AppendMenu( MF_STRING, (INT_PTR)ID_TIMEEVENT_NEWEVENT + 1, "Edit...");
		}
	}
}

void CTimeEventCtrl::OnMenuSelected( UINT32 cmd, NC_HIT hit, POINT * pPoint, INT32 idx)
{
	if( cmd == ID_TIMEEVENT_NEWEVENT + 1)
	{
		OnPopupHotEdit( idx);
	}
}
