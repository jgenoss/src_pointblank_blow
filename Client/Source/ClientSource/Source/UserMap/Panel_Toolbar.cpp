#include "pch.h"
#include "Panel_Toolbar.h"
#include "StageMapEditor.h"

I3_CLASS_INSTANCE( CPanel_Toolbar, i3GameNode);

CPanel_Toolbar::CPanel_Toolbar(void)
{
	m_pStage = NULL;
}

CPanel_Toolbar::~CPanel_Toolbar(void)
{
}

void CPanel_Toolbar::Create( CStageMapEditor * pStage)
{
	I3ASSERT( pStage != NULL);

	m_pStage = pStage;
	
	m_pWnd		= (i3UIFrameWnd *) i3UI::FindFrameWndByName( "Toolbar", this);
	I3ASSERT( m_pWnd != NULL);

	m_pBTN_Select		= (i3UIButton *) m_pWnd->FindChildByName( "Select");
	I3ASSERT( m_pBTN_Select != NULL);

	m_pBTN_Move		= (i3UIButton *) m_pWnd->FindChildByName( "Move");
	I3ASSERT( m_pBTN_Move != NULL);

	m_pBTN_Rotate		= (i3UIButton *) m_pWnd->FindChildByName( "Rotate");
	I3ASSERT( m_pBTN_Rotate != NULL);

	m_pBTN_Scale		= (i3UIButton *) m_pWnd->FindChildByName( "Scale");
	I3ASSERT( m_pBTN_Scale != NULL);

	m_pBTN_Blocks		= (i3UIButton *) m_pWnd->FindChildByName( "Blocks");
	I3ASSERT( m_pBTN_Blocks != NULL);

	m_pWnd->EnableCtrl( TRUE);
}

BOOL CPanel_Toolbar::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
	if( event == I3_EVT_UI_NOTIFY)
	{
		I3UI_CONTROL_NOTIFY * pInfo = (I3UI_CONTROL_NOTIFY *) param2;
		i3UIControl * pCtrl = pInfo->m_pCtrl;

		if( pInfo->m_nEvent == I3UI_EVT_CLICKED)
		{
			if( pCtrl == m_pBTN_Blocks)
			{
				// Palette Wnd Popup
					REAL32 x = m_pBTN_Blocks->getPositionX() + m_pBTN_Blocks->getWidth() + 10.0f;
					REAL32 y = m_pBTN_Blocks->getPositionY();

				m_pStage->PopupPalette( x, y);
			}
		}
	}

	return i3GameNode::OnEvent( event, pObj, param2, code);
}
