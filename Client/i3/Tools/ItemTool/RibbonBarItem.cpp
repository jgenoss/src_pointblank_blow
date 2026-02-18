// ItemRibbonBar.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ItemTool.h"
#include "RibbonBarItem.h"
#include "resource.h"
#include "MainFrm.h"

// ItemRibbonBar

IMPLEMENT_DYNAMIC(cRibbonBarItem, CMFCRibbonBar)

cRibbonBarItem::cRibbonBarItem()
:	m_pCategoryHome(NULL)
,	m_nCategoryCnt(0)
,	m_pBtnWeaponDlg(NULL)
{

}

cRibbonBarItem::~cRibbonBarItem()
{
}


BEGIN_MESSAGE_MAP(cRibbonBarItem, CMFCRibbonBar)
	ON_WM_CREATE()
	//ON_EN_SETFOCUS(IDC_EDIT_PATH,			&cRibbonBarItem::OnEnSetfocusEditPath)
	//ON_EN_CHANGE(IDC_EDIT_PATH,				&cRibbonBarItem::OnEnChangeEditPath)
	//ON_EN_CHANGE(IDC_COMBO_PANNEL_CATEGORY, &cRibbonBarItem::OnEnChangeEditPath)
	ON_CBN_SELCHANGE(IDC_COMBO_PANNEL_CATEGORY, &cRibbonBarItem::OnCbnSelchangeComboPannelCategory)
	ON_CBN_EDITCHANGE(IDC_COMBO_PANNEL_CATEGORY, &cRibbonBarItem::OnCbnEditchangeComboPannelCategory)
END_MESSAGE_MAP()


void cRibbonBarItem::AddRibbonPanel(const UINT& uiTabID)
{
	m_vPanelList.push_back(uiTabID);
}

void cRibbonBarItem::UpdateRibbonChildPanel()
{
	//패널을 꼭 업데이트 해야한다.
	m_pPanelCatecory[E_PANEL_HOME] = UpdateRibbonPanel(IDS_RIBBON_HOME);

	for (unsigned int i = 0; i < m_vPanelList.size(); ++i)
	{
		UINT uiPanelID = m_vPanelList[i];

		mIndexByNameCon::iterator iter = m_mPanelIDByName.find(uiPanelID);
		if (iter != m_mPanelIDByName.end())
		{
			CString strPanelName = iter->second;

			char* szWidthTmp = {"-----------------------"};
			CMFCRibbonPanel* pPanelClipboard = m_pPanelCatecory[E_PANEL_HOME]->AddPanel(strPanelName);

			CMFCRibbonButton* pBtnPaste = new CMFCRibbonButton(uiPanelID, szWidthTmp,0,i);
			pBtnPaste->SetAlwaysLargeImage(TRUE);
			pPanelClipboard->Add(pBtnPaste);

			if (uiPanelID == ID_VIEW_WEAPON_TAB)
			{
				pPanelClipboard->Add( new CMFCRibbonSeparator( FALSE));
				pPanelClipboard->Add( new CMFCRibbonButton( ID_BTN_WEAPON_EDITOR, "WeaponDlg"));


				//m_pBtnWeaponDlg = new CMFCRibbonButton( ID_BTN_WEAPON_EDITOR,"(Open)" );
				//pPanelClipboard->Add(m_pBtnWeaponDlg);
			}
		}
	}
}

void cRibbonBarItem::AddPathEditorPanel()
{
	//패널을 꼭 업데이트 해야한다.
	CMFCRibbonCategory* pCategory = m_pPanelCatecory[E_PANEL_HOME];

	CMFCRibbonPanel* pPanelClipboard = pCategory->AddPanel(STR_ROOT_PATH_BTN);
	pPanelClipboard->SetCenterColumnVert();

	CMFCRibbonButtonsGroup* pButtonsGroup1 = new CMFCRibbonButtonsGroup;

	{
		CMFCRibbonEdit* pEdit = new CMFCRibbonEdit(IDC_EDIT_PATH, MAX_PATH);
		pEdit->SetEditText(_T("Edit"));
		pButtonsGroup1->AddButton(pEdit);

		CMFCRibbonButton* pButton = new CMFCRibbonButton(IDC_OPEN_PATH_DLG,"....");
		pButtonsGroup1->AddButton(pButton);
	}

	pPanelClipboard->Add(pButtonsGroup1);
}

void cRibbonBarItem::AddUtilityPanel()
{
	CMFCRibbonCategory* pCategory = m_pPanelCatecory[E_PANEL_HOME];

	CMFCRibbonPanel* pPanelClipboard = pCategory->AddPanel(STR_UTILITY_BTN);
	//pPanelClipboard->SetCenterColumnVert();

	{
		CMFCRibbonComboBox* pCombo = new CMFCRibbonComboBox(IDC_COMBO_PANNEL_CATEGORY, TRUE);
		pCombo->AddItem(_T("111111"));
		pCombo->AddItem(_T("222222"));
		pCombo->AddItem(_T("333333"));
		pCombo->SelectItem(0);
		pPanelClipboard->Add(pCombo);
	}
	{
		CMFCRibbonCheckBox* pCheckbox = new CMFCRibbonCheckBox(IDC_CHECK_DO_ROTATE,"회전");
		pPanelClipboard->Add(pCheckbox);
	}
}

CMFCRibbonCategory* cRibbonBarItem::UpdateRibbonPanel(const UINT& uiCategoryID)
{
	//카테고리는 현재 1개만 생성하자..
	if (this->GetCategoryCount() > 0)
	{
		this->RemoveCategory(0);
		m_pBtnWeaponDlg = NULL;
	}

	BOOL bNameValid;
	CString strTemp;

	bNameValid = strTemp.LoadString(uiCategoryID);
	ASSERT(bNameValid);
	CMFCRibbonCategory* pCategory = this->AddCategory(strTemp, IDB_WRITESMALL, IDB_PAGES_SMALL_HC, CSize(16,16),CSize(32,32),0);

	return pCategory;
}

void cRibbonBarItem::OnVisibleWeaponEdit(const bool& bVisible)
{
	if (m_pBtnWeaponDlg)
	{
		CMFCRibbonPanel* pPanel = m_pBtnWeaponDlg->GetParentPanel();
		pPanel->Remove(3);
		m_pBtnWeaponDlg = NULL;
	}
	else
	{
		
	}
}

// ItemRibbonBar 메시지 처리기입니다.
int cRibbonBarItem::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCRibbonBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	CString strTemp;
	bNameValid = strTemp.LoadString(IDS_RIBBON_FILE);
	ASSERT(bNameValid);

	// 창 이미지를 로드합니다.
	m_PanelImages.SetImageSize(CSize(16, 16));
	m_PanelImages.Load(IDB_BUTTONS);

	// 초기화 주 단추입니다.
	m_MainButton.SetImage(IDB_MAIN);
	m_MainButton.SetText(_T("\nf"));
	m_MainButton.SetToolTipText(strTemp);
	this->SetApplicationButton(&m_MainButton, CSize (45, 45));
	
	CMFCRibbonMainPanel* pMainPanel = this->AddMainCategory(strTemp, IDB_FILESMALL, IDB_FILELARGE);

	bNameValid = strTemp.LoadString(IDS_RIBBON_NEW);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_NEW, strTemp, 0, 0));
	bNameValid = strTemp.LoadString(IDS_RIBBON_OPEN);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_OPEN, strTemp, 1, 1));
	bNameValid = strTemp.LoadString(IDS_RIBBON_SAVE);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE, strTemp, 2, 2));
	bNameValid = strTemp.LoadString(IDS_RIBBON_SAVEAS);
	ASSERT(bNameValid);
	pMainPanel->Add(new CMFCRibbonButton(ID_FILE_SAVE_AS, strTemp, 3, 3));

	bNameValid = strTemp.LoadString(IDS_RIBBON_HOME);
	ASSERT(bNameValid);
	CMFCRibbonCategory* pCategoryHome = this->AddCategory(strTemp, IDB_WRITESMALL, IDB_WRITELARGE);
	
	// "클립보드" 패널을 만듭니다.
	bNameValid = strTemp.LoadString(IDS_RIBBON_CLIPBOARD);
	ASSERT(bNameValid);
	CMFCRibbonPanel* pPanelClipboard = pCategoryHome->AddPanel(strTemp, m_PanelImages.ExtractIcon(27));

	bNameValid = strTemp.LoadString(IDS_RIBBON_PASTE);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnPaste = new CMFCRibbonButton(ID_EDIT_PASTE, strTemp, 0, 0);
	pPanelClipboard->Add(pBtnPaste);

	bNameValid = strTemp.LoadString(IDS_RIBBON_CUT);
	ASSERT(bNameValid);
	pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_CUT, strTemp, 1));
	bNameValid = strTemp.LoadString(IDS_RIBBON_COPY);
	ASSERT(bNameValid);
	pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_COPY, strTemp, 2));
	bNameValid = strTemp.LoadString(IDS_RIBBON_SELECTALL);
	ASSERT(bNameValid);
	pPanelClipboard->Add(new CMFCRibbonButton(ID_EDIT_SELECT_ALL, strTemp, -1));

	// "보기" 패널을 만들고 추가합니다.
	bNameValid = strTemp.LoadString(IDS_RIBBON_VIEW);
	ASSERT(bNameValid);
	CMFCRibbonPanel* pPanelView = pCategoryHome->AddPanel(strTemp, m_PanelImages.ExtractIcon (7));

	bNameValid = strTemp.LoadString(IDS_RIBBON_STATUSBAR);
	ASSERT(bNameValid);
	CMFCRibbonButton* pBtnStatusBar = new CMFCRibbonCheckBox(ID_VIEW_STATUS_BAR, strTemp);
	pPanelView->Add(pBtnStatusBar);

	// 탭의 오른쪽에 요소를 추가합니다.
	bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE);
	ASSERT(bNameValid);
	CMFCRibbonButton* pVisualStyleButton = new CMFCRibbonButton(0, strTemp, 0, 0);

	pVisualStyleButton->SetMenu(IDR_THEME_MENU, FALSE /* 기본값 없음 명령입니다. */, TRUE /* 오른쪽 맞춤입니다. */);

	bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE_TIP);
	ASSERT(bNameValid);
	pVisualStyleButton->SetToolTipText(strTemp);
	bNameValid = strTemp.LoadString(IDS_RIBBON_STYLE_DESC);
	ASSERT(bNameValid);
	pVisualStyleButton->SetDescription(strTemp);
	this->AddToTabs(pVisualStyleButton);

	// 빠른 실행 도구 모음 명령을 추가합니다.
	CList<UINT, UINT> lstQATCmds;

	lstQATCmds.AddTail(ID_FILE_NEW);
	lstQATCmds.AddTail(ID_FILE_OPEN);
	lstQATCmds.AddTail(ID_FILE_SAVE);
	lstQATCmds.AddTail(ID_FILE_PRINT_DIRECT);

	this->SetQuickAccessCommands(lstQATCmds);
	this->AddToTabs(new CMFCRibbonButton(ID_APP_ABOUT, _T("\na"), m_PanelImages.ExtractIcon (0)));

	//인덱스에 따른 String값을 설정해둔다..
	//유동적으로 추가 삭제를 할려고 했으나 MFC버튼 이벤트 아뒤를 미리등록 시켜두어야 하므로
	//직접 미리 셋팅 해두어야 한다.
	m_mPanelIDByName[ID_VIEW_WEAPON_TAB]	= STR_WEAPON_BTN;
	m_mPanelIDByName[ID_VIEW_GOODS_TAB]		= STR_GOODS_BTN;
	m_mPanelIDByName[ID_VIEW_EQUIPMENT_TAB] = STR_EQUIPMENT_BTN;
	m_mPanelIDByName[ID_VIEW_CHARACTER_TAB] = STR_CHARACTER_BTN;
	m_mPanelIDByName[ID_VIEW_ABILITY_TAB]	= STR_ABILITY_BTN;

	//패널을 집어 넣자..
// 	AddRibbonPanel(ID_VIEW_WEAPON_TAB);
// 	AddRibbonPanel(ID_VIEW_GOODS_TAB);
// 	AddRibbonPanel(ID_VIEW_EQUIPMENT_TAB);
// 	AddRibbonPanel(ID_VIEW_CHARACTER_TAB);
// 	AddRibbonPanel(ID_VIEW_ABILITY_TAB);

	return 0;
}

void cRibbonBarItem::OnFileNew()
{
	
}
void cRibbonBarItem::OnFileOpen()
{
	
}

void cRibbonBarItem::OnCbnSelchangeComboPannelCategory()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void cRibbonBarItem::OnCbnEditchangeComboPannelCategory()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
