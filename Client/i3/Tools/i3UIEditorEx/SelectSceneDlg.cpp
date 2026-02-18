// SelectSceneDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SelectSceneDlg.h"
#include "MainFrm.h"
#include "i3UIEditorDoc.h"


// CSelectSceneDlg dialog

IMPLEMENT_DYNAMIC(CSelectSceneDlg, CDialog)

CSelectSceneDlg::CSelectSceneDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectSceneDlg::IDD, pParent)
{
}

CSelectSceneDlg::~CSelectSceneDlg()
{
}

void CSelectSceneDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LV_SCENELIST, m_lvScene);
	DDX_Control(pDX, ID_BTN_SETACTIVE, m_btnSetActive);
	DDX_Control(pDX, ID_BTN_SHOWHIDE, m_btnShowHide);
	DDX_Control(pDX, ID_BTN_CLOSESCENE, m_btnCloseScene);
}


BEGIN_MESSAGE_MAP(CSelectSceneDlg, CDialog)
	ON_BN_CLICKED(ID_BTN_CLOSESCENE, &CSelectSceneDlg::OnBnClickedBtnClosescene)
	ON_BN_CLICKED(ID_BTN_SETACTIVE, &CSelectSceneDlg::OnBnClickedBtnSetactive)
	ON_BN_CLICKED(ID_BTN_SHOWHIDE, &CSelectSceneDlg::OnBnClickedBtnShowhide)
	ON_NOTIFY(NM_DBLCLK, IDC_LV_SCENELIST, &CSelectSceneDlg::OnNMDblclkLvScenelist)
END_MESSAGE_MAP()


void CSelectSceneDlg::_UpdateState(void)
{
	i3UIManager * pUI = i3UI::getManager();
	if( pUI == NULL)
	{
		return;
	}

	m_lvScene.DeleteAllItems();

	i3::vector<I3UI_SCENE_INFO*>& List = pUI->getSceneInfoList();

	for( size_t i = 0;i < List.size(); ++i)
	{
		I3UI_SCENE_INFO * pInfo = List[i];
		
		I3ASSERT( pInfo != NULL);
		I3ASSERT( pInfo->m_pScene != NULL);

		char Temp[256]= "";
		int n = 1;

		i3::snprintf(Temp, MAX_PATH, "%d", i);

		m_lvScene.InsertItem( i, Temp); 

		m_lvScene.SetItemData( i, (DWORD_PTR) pInfo);

		//
		CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
		I3ASSERT( pFrame != NULL);

		Ci3UIEditorDoc * pDoc = ( Ci3UIEditorDoc*)pFrame->GetActiveDocument();
		if( pDoc && (pDoc->getCurrentScene() == pInfo->m_pScene))
		{
			i3::snprintf( Temp, 255, "* %s", pInfo->m_pScene->GetName());
			m_lvScene.SetItemText( i, n++, Temp);
		}
		else
		{
			m_lvScene.SetItemText( i, n++, pInfo->m_pScene->GetName());
		}

		if( pInfo->m_pScene->getParent() != NULL)
		{
			m_lvScene.SetItemText( i, n++, "true");
		}
		else
		{
			m_lvScene.SetItemText( i, n++, "false");
		}

		if( pInfo->m_pScene->isEnable())
		{
			m_lvScene.SetItemText( i, n++, "true");
		}
		else
		{
			m_lvScene.SetItemText( i, n++, "false");
		}

		m_lvScene.SetItemText( i, n++, pInfo->m_strSceneFile.c_str());
	}
}

// CSelectSceneDlg message handlers
BOOL CSelectSceneDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	i3UIManager * pUI = i3UI::getManager();
	if( pUI == NULL)
	{
		return FALSE;
	}

	{
		LVCOLUMN col;
		memset( &col, 0, sizeof( LVCOLUMN ) );

		INT32 n = 0;

		col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
		col.fmt = LVCFMT_CENTER;
		col.pszText = "Num";
		col.cx = 40;
		m_lvScene.InsertColumn( n++, &col);

		col.pszText = "Name";
		col.cx = 100;
		col.fmt = LVCFMT_LEFT;
		m_lvScene.InsertColumn( n++, &col);

		col.pszText = "Attach";
		col.cx = 50;
		col.fmt = LVCFMT_CENTER;
		m_lvScene.InsertColumn( n++, &col);
		
		col.pszText = "Show";
		col.cx = 40;
		m_lvScene.InsertColumn( n++, &col);

		col.pszText = "File";
		col.cx = 400;
		col.fmt = LVCFMT_LEFT;
		m_lvScene.InsertColumn( n++, &col);

		m_lvScene.SetExtendedStyle( LVS_EX_FULLROWSELECT);
	}

	_UpdateState();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectSceneDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	CDialog::OnOK();
}

void CSelectSceneDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}

void CSelectSceneDlg::OnBnClickedBtnClosescene()
{
	// TODO: Add your control notification handler code here
	INT32 count = m_lvScene.GetSelectedCount();
	INT32 i;
	for( i = 0;i < count; ++i)
	{
		POSITION pos = m_lvScene.GetFirstSelectedItemPosition();
		I3ASSERT( pos != NULL);

		INT32 idx = m_lvScene.GetNextSelectedItem( pos);
		
		I3UI_SCENE_INFO * pInfo = (I3UI_SCENE_INFO*) m_lvScene.GetItemData( idx);

		i3UIScene * pScene = pInfo->m_pScene;
		I3ASSERT( pScene != NULL);

		//
		CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
		I3ASSERT( pFrame != NULL);

		Ci3UIEditorDoc * pDoc = ( Ci3UIEditorDoc*)pFrame->GetActiveDocument();
		if( pDoc)
		{
			if( pDoc->getCurrentScene() == pScene)
			{
				pDoc->setCurrentScene( NULL, NULL);
			}
		}

		pScene->Detach();

		i3UIManager * pUI = i3UI::getManager();

		pUI->RemoveScene( pScene);
	}

	if( count > 0)
	{
		_UpdateState();

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_REMOVE, NULL, i3UIControl::static_meta());	
	}
}

void CSelectSceneDlg::OnBnClickedBtnSetactive()
{
	// TODO: Add your control notification handler code here
	INT32 count = m_lvScene.GetSelectedCount();
	if( count <= 0)
		return;

	CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
	I3ASSERT( pFrame != NULL);

	Ci3UIEditorDoc * pDoc = ( Ci3UIEditorDoc*)pFrame->GetActiveDocument();
	if( pDoc)
	{
		POSITION pos = m_lvScene.GetFirstSelectedItemPosition();
		if( pos != NULL)
		{
			INT32 idx = m_lvScene.GetNextSelectedItem( pos);

			I3UI_SCENE_INFO * pInfo = (I3UI_SCENE_INFO*) m_lvScene.GetItemData( idx);
			pInfo->m_bEnabled = true;

			i3UIScene * pScene = pInfo->m_pScene;
			I3ASSERT( pScene != NULL);
			if( pDoc)
			{
				if( pDoc->getCurrentScene() != pScene)
				{
					pDoc->setCurrentScene( pScene, NULL, false);
					pDoc->setUILibrary( i3UI::getUILibrary());

					pDoc->SetPathName( pInfo->m_strSceneFile.c_str());
				}
			}
		}
	}

	if( count > 0)
	{
		_UpdateState();
	}
}

void CSelectSceneDlg::OnBnClickedBtnShowhide()
{
	// TODO: Add your control notification handler code here
	INT32 count = m_lvScene.GetSelectedCount();
	INT32 i;
	for( i = 0;i < count; ++i)
	{
		POSITION pos = m_lvScene.GetFirstSelectedItemPosition();
		I3ASSERT( pos != NULL);

		INT32 idx = m_lvScene.GetNextSelectedItem( pos);
		
		I3UI_SCENE_INFO * pInfo = (I3UI_SCENE_INFO*) m_lvScene.GetItemData( idx);
		i3UIScene * pScene = pInfo->m_pScene;

		pInfo->m_bEnabled = !pScene->isEnable();
		I3ASSERT( pScene != NULL);

		pScene->EnableScene((BOOL)(!pScene->isEnable()));
	}

	if( count > 0)
		_UpdateState();
}

void CSelectSceneDlg::OnNMDblclkLvScenelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE*>(pNMHDR);
	// TODO: Add your control notification handler code here

	if( pNMItemActivate->iItem < 0)
		return;

	I3UI_SCENE_INFO * pInfo = (I3UI_SCENE_INFO*)m_lvScene.GetItemData( pNMItemActivate->iItem);
	if( pInfo != NULL)
	{
		CMainFrame * pFrame = (CMainFrame*)AfxGetMainWnd();
		I3ASSERT( pFrame != NULL);

		Ci3UIEditorDoc * pDoc = ( Ci3UIEditorDoc*)pFrame->GetActiveDocument();
		if( pDoc)
		{
			pInfo->m_bEnabled = true;
			if( pDoc)
			{
				if( pDoc->getCurrentScene() != pInfo->m_pScene)
				{
					pDoc->setCurrentScene( pInfo->m_pScene, NULL, false);
					pDoc->setUILibrary( i3UI::getUILibrary());

					pDoc->SetPathName( pInfo->m_strSceneFile.c_str());

					_UpdateState();
				}
			}
		}
	}

	*pResult = 0;
}
