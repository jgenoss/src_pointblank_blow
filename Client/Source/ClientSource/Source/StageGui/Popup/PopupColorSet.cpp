#include "pch.h"
#include "GlobalVariables.h"
#include "PopupColorSet.h"
//#include "i3guiroot.h"

I3_CLASS_INSTANCE(CPopupColorSet, CPopupBase);

CPopupColorSet::CPopupColorSet()
{
	i3Color::Set( &m_SelectedColor, 0.0f, 0.0f, 0.0f, 0.0f); 

	m_pStage = NULL;
	m_pRoot = NULL;

	m_pColorSetWin = NULL;

	m_pColorSetCaption = NULL;
	m_pColorSetColor = NULL;
	m_pColorSetSelectedColor = NULL;
	m_pColorSetBrightColorSet = NULL;
	m_pColorSetRGBEditBox = NULL;

	m_pColorSetOK = NULL;
	m_pColorSetCancel = NULL;	

}

/*virtual*/CPopupColorSet::~CPopupColorSet()
{
}

void CPopupColorSet::CreateColorSet(CStageMapEditor * pStage, i3GuiRoot * pRoot)
{
	I3ASSERT( pStage != NULL);
	I3ASSERT( pRoot != NULL);

	m_pStage = pStage;
	m_pRoot = pRoot;

	m_pColorSetWin = (i3GuiStatic*)m_pRoot->GetControlByName("ColorSetWin", this);
	m_pColorSetCaption = (i3GuiEditBox*)m_pRoot->GetControlByName("ColorSetCaption");
	m_pColorSetRGBEditBox = (i3GuiEditBox*)m_pRoot->GetControlByName("ColorSetRGBEditBox");
	m_pColorSetSelectedColor = (i3GuiStatic*)m_pRoot->GetControlByName("ColorSetSelectedColor");
	m_pColorSetColor = (i3GuiStatic*)m_pRoot->GetControlByName("ColorSetColor");
	m_pColorSetBrightColorSet = (i3GuiStatic*)m_pRoot->GetControlByName("ColorSetBrightColorSet");
	m_pColorSetOK = (i3GuiButton*)m_pRoot->GetControlByName("ColorSetOK");
	m_pColorSetCancel = (i3GuiButton*)m_pRoot->GetControlByName("ColorSetCancel");


	INIT_WIN_CAPTION( m_pColorSetCaption, "»ö±ò ÁöÁ¤");	
	INIT_STATIC_CAPTION(m_pColorSetRGBEditBox, "»¡°­   0     ³ì»ö   0     ÆÄ¶û   0");

	INIT_CAPTION_BUTTON(m_pColorSetOK, "È®ÀÎ");
	INIT_CAPTION_BUTTON(m_pColorSetCancel, "Ãë¼Ò");
	
	m_pColorSetWin->SetEnable(FALSE);

}

BOOL CPopupColorSet::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code)
{
	if( event == I3_EVT_GUI_NOTIFY)
	{
		I3GUI_CONTROL_NOTIFY * pInfo = (I3GUI_CONTROL_NOTIFY *) param2;
		i3GuiControl * pCtrl = (i3GuiControl *) pInfo->m_pCtrl;

		if( pCtrl == m_pColorSetOK)
		{
			if( pInfo->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) OnClosePopup();
		}
		else if( pCtrl == m_pColorSetCancel)
		{
			if( pInfo->m_nEvent == I3GUI_BUTTON_NOTIFY_CLICKED) OnClosePopup();
		}
		else if( pCtrl == m_pColorSetColor)
		{
			if (I3GUI_STATIC_NOTIFY_CLICKED == pInfo->m_nEvent)
			{
				COLORREAL Col;
				i3Color::Set( &Col, 0.0f, 0.0f, 0.0f, 0.0f);
				

				m_pColorSetColor->GetPixelStaticCtrl(pInfo->m_nPriParam, pInfo->m_nSecParam, &Col);
				m_pColorSetSelectedColor->FillColorInTexture(&Col);
				m_pColorSetBrightColorSet->FillBrightColorInTexture(&Col);

				char szTemp[256];
				INT32 iRed = (INT32)(Col.r*255.f);
				INT32 iGreen = (INT32)(Col.g*255.f);
				INT32 iBlue = (INT32)(Col.b*255.f);

				StringCchPrintf(szTemp, 256, "»¡°­ %3d     ³ì»ö %3d     ÆÄ¶û %3d", iRed, iGreen, iBlue);
				m_pColorSetRGBEditBox->SetText(szTemp);
			}
		}
		else if( pCtrl == m_pColorSetBrightColorSet)
		{
			if (I3GUI_STATIC_NOTIFY_CLICKED == pInfo->m_nEvent)
			{
				COLORREAL Col;
				i3Color::Set( &Col, 0.0f, 0.0f, 0.0f, 0.0f);
				
				m_pColorSetBrightColorSet->GetPixelStaticCtrl(pInfo->m_nPriParam, pInfo->m_nSecParam, &Col);
				m_pColorSetSelectedColor->FillColorInTexture(&Col);
				char szTemp[256];
				INT32 iRed = (INT32)(Col.r*255.f);
				INT32 iGreen = (INT32)(Col.g*255.f);
				INT32 iBlue = (INT32)(Col.b*255.f);

				StringCchPrintf(szTemp, 256, "»¡°­ %3d     ³ì»ö %3d     ÆÄ¶û %3d", iRed, iGreen, iBlue);
				m_pColorSetRGBEditBox->SetText(szTemp);
			}
		}
	}

	return i3GameNode::OnEvent( event, pObj, param2, code);
}

/*virtual*/void CPopupColorSet::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	CPopupBase::OnUpdate(rDeltaSeconds);
}


/*virtual*/void CPopupColorSet::OnExitKey(void)
{
	OnClosePopup();
}

/*virtual*/void CPopupColorSet::OnOpenPopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnOpenPopup();

	
	m_pColorSetWin->SetEnable(TRUE);
}

/*virtual*/void CPopupColorSet::OnClosePopup(void * pArg1, void * pArg2)
{
	CPopupBase::OnClosePopup();

	m_pColorSetWin->SetEnable(FALSE);
}