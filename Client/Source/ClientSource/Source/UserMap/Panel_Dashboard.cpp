#include "pch.h"
#include "Panel_Dashboard.h"
#include "UserMap.h"
#include "StageMapEditor.h"

I3_CLASS_INSTANCE( CPanel_Dashboard, i3GameNode);

CPanel_Dashboard::CPanel_Dashboard(void)
{
}

CPanel_Dashboard::~CPanel_Dashboard(void)
{
}

void CPanel_Dashboard::Create( CStageMapEditor * pStage)
{
	m_pStage = pStage;

	m_pWnd		= (i3UIFrameWnd *) i3UI::FindFrameWndByName( "Dashboard", this);
	I3ASSERT( m_pWnd != NULL);

	m_pBTN_Save		= (i3UIButton *) m_pWnd->FindChildByName( "Save");
	I3ASSERT( m_pBTN_Save != NULL);

	m_pBTN_Register		= (i3UIButton *) m_pWnd->FindChildByName( "Register");
	I3ASSERT( m_pBTN_Register != NULL);

	m_pBTN_MapSetting		= (i3UIButton *) m_pWnd->FindChildByName( "MapSetting");
	I3ASSERT( m_pBTN_MapSetting != NULL);

	m_pBTN_Test		= (i3UIButton *) m_pWnd->FindChildByName( "Test");
	I3ASSERT( m_pBTN_Test != NULL);

	m_pBTN_Undo		= (i3UIButton *) m_pWnd->FindChildByName( "Undo");
	I3ASSERT( m_pBTN_Undo != NULL);

	m_pBTN_Delete		= (i3UIButton *) m_pWnd->FindChildByName( "Delete");
	I3ASSERT( m_pBTN_Delete != NULL);

	m_pBTN_Copy		= (i3UIButton *) m_pWnd->FindChildByName( "Copy");
	I3ASSERT( m_pBTN_Copy != NULL);

	m_pBTN_Cut		= (i3UIButton *) m_pWnd->FindChildByName( "Cut");
	I3ASSERT( m_pBTN_Cut != NULL);

	m_pBTN_Paste		= (i3UIButton *) m_pWnd->FindChildByName( "Paste");
	I3ASSERT( m_pBTN_Paste != NULL);

	m_pBTN_Exit		= (i3UIButton *) m_pWnd->FindChildByName( "Exit");
	I3ASSERT( m_pBTN_Exit != NULL);

	m_pWnd->EnableCtrl( TRUE);
}

void CPanel_Dashboard::Save(void)
{
}

void CPanel_Dashboard::PromptSave(void)
{
	CUserMap * pMap = m_pStage->getMap();

	if((pMap->hasName() == false) || (pMap->GetName()[0] == 0))
	{
		// 맵의 이름이 입력되지 않았다면 Dialog를 띄운다.
		i3UI::DoModal( "MapEditor/DlgMapName.i3UI", true, &m_DlgMapName_Driver);
	}
	else
	{
		// 저장.
		Save();
	}
}

BOOL CPanel_Dashboard::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
	switch( event)
	{
		case I3_EVT_UI_NOTIFY :
			{
				I3UI_CONTROL_NOTIFY * pInfo = (I3UI_CONTROL_NOTIFY *) param2;
				i3UIControl * pCtrl = pInfo->m_pCtrl;

				switch( pInfo->m_nEvent)
				{
					case I3UI_EVT_CLICKED :
						if( pCtrl == m_pBTN_Save)
						{
							// Save
							PromptSave();
						}
						else if( pCtrl == m_pBTN_Register)
						{
							// 등록
							I3TRACE( "Register\n");
						}
						break;

					case I3UI_EVT_MODALRESULT :
						{
							switch( param2)
							{
								case I3UI_MODALRESULT_YES :
									Save();
									break;

								case I3UI_MODALRESULT_NO :
									I3TRACE( "NO\n");
									break;

								case I3UI_MODALRESULT_CANCEL :
									I3TRACE( "CANCEL\n");
									break;
							}
						}
						break;
				}
			}
			break;
	}

	return i3GameNode::OnEvent( event, pObj, param2, code);
}
