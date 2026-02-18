#include "stdafx.h"
#include "MainFrm.h"
#include "resource.h"



void CMainFrame::OnViewWeaponTab()
{
	g_pMainSCFramework->SetCurrentTAB(E_TAB_WEAPON);
	_SetCurrentType(E_PEF_WEAPON);
	m_wndPropertyEdit.SetPefData("", g_pPefList->getRoot());
}

void CMainFrame::OnUpdateViewWeaponTab(CCmdUI *pCmdUI)
{
	if (g_pMainSCFramework->GetCurrentTAB() == E_TAB_WEAPON)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);

}


void CMainFrame::OnViewGoodsTab()
{
	g_pMainSCFramework->SetCurrentTAB(E_TAB_GOODS);
	_SetCurrentType(E_PEF_GOODS);
	m_wndPropertyEdit.SetPefData("", g_pPefList->getRoot());
}

void CMainFrame::OnUpdateViewGoodsTab(CCmdUI *pCmdUI)
{
	if (g_pMainSCFramework->GetCurrentTAB() == E_TAB_GOODS)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);

}

void CMainFrame::OnViewEquipmentTab()
{
	g_pMainSCFramework->SetCurrentTAB(E_TAB_EQUIPMENT);
	_SetCurrentType(E_PEF_EQUIPMENT);
	m_wndPropertyEdit.SetPefData("", g_pPefList->getRoot());
}

void CMainFrame::OnUpdateViewEquipmentTab(CCmdUI *pCmdUI)
{
	if (g_pMainSCFramework->GetCurrentTAB() == E_TAB_EQUIPMENT)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);

}

void CMainFrame::OnViewCharacterTab()
{
	g_pMainSCFramework->SetCurrentTAB(E_TAB_CHARACTER);
	_SetCurrentType(E_PEF_CHARA);
	m_wndPropertyEdit.SetPefData("", g_pPefList->getRoot());
}

void CMainFrame::OnUpdateViewCharacterTab(CCmdUI *pCmdUI)
{
	if (g_pMainSCFramework->GetCurrentTAB() == E_TAB_CHARACTER)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);

}

void CMainFrame::OnViewAbilityTab()
{
	g_pMainSCFramework->SetCurrentTAB(E_TAB_ABILITY);
}

void CMainFrame::OnUpdateViewAbilityTab(CCmdUI *pCmdUI)
{
	if (g_pMainSCFramework->GetCurrentTAB() == E_TAB_ABILITY)
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);

}

void CMainFrame::OnBtnOpenWeaponEditor()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	bool bWeaponDlg = m_pDlgWeapon->IsVisible();
	bWeaponDlg = !bWeaponDlg;
	m_pDlgWeapon->SetVisible(bWeaponDlg);
}

void CMainFrame::OnUpdateOpenWeaponEditor(CCmdUI *pCmdUI)
{
	
}

void CMainFrame::OnPath()
{

}

void CMainFrame::OnUpdatePath(CCmdUI *pCmdUI)
{
	pCmdUI->SetText(g_strRootPath);
}

void CMainFrame::OnEnSetfocusEditPath()
{
	
}

void CMainFrame::OnPathButton()
{
	GUTIL::SetWorkingFolder();
}

void CMainFrame::OnUpdatePathButton(CCmdUI *pCmdUI)
{
	
}

void CMainFrame::OnUtilityCategoryCombo()
{
	cItemTabManager* pTabManager	= g_pMainSCFramework->GetItemTabManager();
	cRibbonBarItem* pItem			= pTabManager->GetWndRibbonBar();
	CMFCRibbonComboBox* pCombo		= DYNAMIC_DOWNCAST(CMFCRibbonComboBox, pItem->FindByID(IDC_COMBO_PANNEL_CATEGORY));

	int nCurSel = pCombo->GetCurSel();

	CString item= pCombo->GetItem(nCurSel);
}

void CMainFrame::OnUpdateUtilityCategoryCombo(CCmdUI *pCmdUI)
{
	
}

void CMainFrame::OnUtilityCategoryRotateBtn()
{
	cUI3DView* p3DView = g_pMainSCFramework->GetUIMain()->Get3DView();
	if (p3DView)
	{

		if (p3DView->IsRotate())
		{
			p3DView->SetRotate(false);
		}
		else
		{
			p3DView->SetRotate(true);
		}
	}
	
}

void CMainFrame::OnUpdateUtilityCategoryRotateBtn(CCmdUI *pCmdUI)
{
	cUI3DView* p3DView = g_pMainSCFramework->GetUIMain()->Get3DView();
	if (p3DView)
	{
		if (p3DView->IsRotate())
		{
			pCmdUI->SetCheck(1);
			//p3DView->SetRotate(false);
		}
		else
		{
			pCmdUI->SetCheck(0);
			//p3DView->SetRotate(true);
		}
		
		//g_pMainSCFramework->GetUIMain()->SetRotate(false);
	}
}