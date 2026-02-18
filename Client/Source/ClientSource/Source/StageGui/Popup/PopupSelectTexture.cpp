#include "pch.h"
#include "GlobalVariables.h"
#include "PopupSelectTexture.h"
//#include "i3guiroot.h"

I3_CLASS_INSTANCE(CPopupSelectTexture, CPopupBase);

CPopupSelectTexture::CPopupSelectTexture()
{
	m_pSTD_Win = NULL;
	m_pSTD_Caption = NULL;
	m_pSTD_Scroll = NULL;
	m_pSTD_Preview = NULL;
	m_pSTD_OK = NULL;
	m_pSTD_Cancel = NULL;	
	m_pRoot = NULL;
	m_pStage = NULL;
}

/*virtual*/CPopupSelectTexture::~CPopupSelectTexture()
{
}


void CPopupSelectTexture::CreateSelectTexture(CStageMapEditor * pStage, i3GuiRoot * pRoot)
{
	I3ASSERT( pStage != NULL);
	I3ASSERT( pRoot != NULL);

	m_pStage = pStage;
	m_pRoot = pRoot;

	m_pSTD_Win = (i3GuiStatic*)m_pRoot->GetControlByName("STD_Win", this);
	m_pSTD_Caption = (i3GuiEditBox*)m_pRoot->GetControlByName("STD_Caption");
	m_pSTD_Scroll = (i3GuiList*)m_pRoot->GetControlByName("STD_TextureNameScroll");
	
	m_pSTD_Preview = (i3GuiStatic*)m_pRoot->GetControlByName("STD_Preview");
	m_pSTD_OK = (i3GuiButton*)m_pRoot->GetControlByName("STD_OK");
	m_pSTD_Cancel = (i3GuiButton*)m_pRoot->GetControlByName("STD_Cancel");	

	INIT_WIN_CAPTION( m_pSTD_Caption, GAME_STRING("STR_POPUP_SET_TEXTURE"));	//	"텍스쳐 지정"

	INIT_CAPTION_BUTTON(m_pSTD_OK, GAME_STRING("STR_POPUP_OK"));	//	"확인"
	INIT_CAPTION_BUTTON(m_pSTD_Cancel, GAME_STRING("STR_POPUP_CANCEL"));	//	"취소"
	
	m_pSTD_Win->SetEnable(FALSE);
}

BOOL CPopupSelectTexture::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
	if( event == I3_EVT_GUI_NOTIFY)
	{
		I3GUI_CONTROL_NOTIFY * pInfo = (I3GUI_CONTROL_NOTIFY *) param2;
		i3GuiControl * pCtrl = (i3GuiControl *) pInfo->m_pCtrl;

		if( pCtrl == m_pSTD_OK)
		{
			if( pInfo->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) OnClosePopup();
		}
		else if( pCtrl == m_pSTD_Cancel)
		{
			if( pInfo->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) OnClosePopup();
		}
	}

	return i3GameNode::OnEvent( event, pObj, param2, code);
}

/*virtual*/void CPopupSelectTexture::OnExitKey(void)
{
	OnClosePopup();
}

/*virtual*/void CPopupSelectTexture::OnOpenPopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnOpenPopup();

	m_pSTD_Win->SetEnable(TRUE);
}

/*virtual*/void CPopupSelectTexture::OnClosePopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnClosePopup();

	m_pSTD_Win->SetEnable(FALSE);
}

/*virtual*/void CPopupSelectTexture::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPopupBase::OnUpdate(rDeltaSeconds);
}