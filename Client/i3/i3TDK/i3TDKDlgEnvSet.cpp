// i3TDKDlgEnvSet.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKDlgEnvSet.h"
#include ".\i3tdkdlgenvset.h"


// i3TDKDlgEnvSet dialog

IMPLEMENT_DYNAMIC(i3TDKDlgEnvSet, CDialog)

void i3TDKDlgEnvSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_PROJECT_LIST, m_LV_Project);
	DDX_Control(pDX, IDC_TDK_ED_WORKDIR, m_ED_WorkDir);
	DDX_Control(pDX, IDC_TDK_RICHED_DESC, m_ED_Desc);
}

bool i3TDKDlgEnvSet::Execute(void)
{
	INT32 rv;

	i3TDK::SetResInstance();

	rv = DoModal();

	i3TDK::RestoreResInstance();

	if( rv == IDCANCEL)
		return false;

	return true;
}

void i3TDKDlgEnvSet::_UpdateAll(void)
{
	INT32 i, cnt;

	m_LV_Project.DeleteAllItems();

	cnt = g_pTDKEnvSet->getProjectCount();

	for( i = 0; i < cnt; i++)
	{
		_UpdateProject( i);
	}
}

void i3TDKDlgEnvSet::_UpdateProject( INT32 idx)
{
	I3TDK_PROJECT_INFO info;
	INT32 idxImage = I3TDK_IMAGELIST_BLANK;

	g_pTDKEnvSet->getProject( idx, &info);

	if( i3::generic_is_iequal( g_pTDKEnvSet->getCurProject(), info.m_szProject) )
		idxImage = I3TDK_IMAGELIST_ATTR;

	m_LV_Project.InsertItem( idx, info.m_szProject, idxImage);
	m_LV_Project.SetItemText( idx, 1, info.m_szWorkDir);
}

void	i3TDKDlgEnvSet::_SelectProject( INT32 idx)
{
	m_idxSelProject = idx;

	if( idx != -1)
	{
		I3TDK_PROJECT_INFO info;

		info.init();

		g_pTDKEnvSet->getProject( idx, &info);

		m_ED_WorkDir.SetWindowText( info.m_szWorkDir);
		m_ED_Desc.SetWindowText( info.m_szDesc);
	}
	else
	{
		m_ED_WorkDir.SetWindowText( _T("") );
		m_ED_Desc.SetWindowText( _T("") );
	}

	m_ED_WorkDir.EnableWindow( idx != -1);
	m_ED_Desc.EnableWindow( idx != -1);
}

void	i3TDKDlgEnvSet::OnNewProject(void)
{
 	i3TDKDlgRename dlg;
 
 	if( dlg.Execute( _T("새롭게 생성할 Project 명을 입력해주세요..."), nullptr) == FALSE)
 		return;

	I3TDK_PROJECT_INFO info;

	info.init();

	i3::safe_string_copy( info.m_szProject, dlg.getNameString(), 256);

	g_pTDKEnvSet->setProject( &info);

	_UpdateAll();
}

void	i3TDKDlgEnvSet::OnDelete(void)
{
	INT32 idx = _getSelectedIndex();

	if (idx == -1)
		return;

	I3TDK_PROJECT_INFO pinfo;
	g_pTDKEnvSet->getProject(idx, &pinfo);
	g_pTDKEnvSet->DeleteProject(&pinfo);

	_UpdateAll();
}

void	i3TDKDlgEnvSet::OnSetActiveProject(void)
{
	INT32 idx = _getSelectedIndex();

	if( idx == -1)
		return;

	I3TDK_PROJECT_INFO info;

	g_pTDKEnvSet->getProject( idx, &info);

	g_pTDKEnvSet->SelectProject( info.m_szProject);

	_UpdateAll();
}

BEGIN_MESSAGE_MAP(i3TDKDlgEnvSet, CDialog)
	ON_NOTIFY(NM_RCLICK, IDC_TDK_PROJECT_LIST, OnNMRclickTdkProjectList)
	ON_NOTIFY(NM_CLICK, IDC_TDK_PROJECT_LIST, OnNMClickTdkProjectList)
	ON_EN_KILLFOCUS(IDC_TDK_ED_WORKDIR, OnEnKillfocusTdkEdWorkdir)
	ON_EN_KILLFOCUS(IDC_TDK_RICHED_DESC, OnEnKillfocusTdkRichedDesc)
	ON_NOTIFY(NM_DBLCLK, IDC_TDK_PROJECT_LIST, OnNMDblclkTdkProjectList)
END_MESSAGE_MAP()


// i3TDKDlgEnvSet message handlers

BOOL i3TDKDlgEnvSet::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_LV_Project.SetExtendedStyle( LVS_EX_FULLROWSELECT);

		m_LV_Project.InsertColumn( 0, _T("Name"), LVCFMT_LEFT, 150);
		m_LV_Project.InsertColumn( 1, _T("Working Directory"), LVCFMT_LEFT, 280);

		m_LV_Project.SetImageList( g_pTDKGlobalRes->m_pTDKImageList, LVSIL_SMALL);
	}

	_UpdateAll();

	_SelectProject( -1);			// Unselection

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

INT32	i3TDKDlgEnvSet::_getSelectedIndex(void)
{
	INT32 idx = -1;
	POSITION pos;

	pos = m_LV_Project.GetFirstSelectedItemPosition();

	if( pos != nullptr)
	{
		idx = m_LV_Project.GetNextSelectedItem( pos);
	}

	return idx;
}

void i3TDKDlgEnvSet::OnNMClickTdkProjectList(NMHDR *pNMHDR, LRESULT *pResult)
{
	INT32 idx;

	idx = _getSelectedIndex();

	_SelectProject( idx);

	*pResult = 0;
}


void i3TDKDlgEnvSet::OnNMRclickTdkProjectList(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu menu, * pPopup;
	CPoint pt, cpt;
	UINT32 cmd;

	GetCursorPos( &pt);

	cpt = pt;
	ScreenToClient( &cpt);

	BOOL bLoadMenu = menu.LoadMenu( IDR_POPUP_PROJECT);
	if (TRUE == bLoadMenu)
	{
		pPopup = menu.GetSubMenu( 0);

		cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
			pt.x, pt.y, (CWnd *) this, nullptr);

		switch( cmd)
		{
		case ID_PROJECT_NEW :				OnNewProject();			break;
		case ID_PROJECT_DELETE :			OnDelete();				break;
		case ID_PROJECT_SETACTIVEPROJECT :	OnSetActiveProject();	break;
		}

	}

	
	*pResult = 0;
}


void i3TDKDlgEnvSet::OnEnKillfocusTdkEdWorkdir()
{
	if( m_ED_WorkDir.GetModify() == FALSE)
		return;

	if( m_idxSelProject == -1)
		return;

	I3TDK_PROJECT_INFO info;

	info.init();

	// 현재의 Project 설정을 읽어온다.
	g_pTDKEnvSet->getProject( m_idxSelProject, &info);

	// 새로운 WorkDir을 구한다.
	m_ED_WorkDir.GetWindowText( info.m_szWorkDir, sizeof( info.m_szWorkDir));

	if (FALSE == ::SetCurrentDirectory(info.m_szWorkDir))
	{
		MessageBox(_T("해당 폴더를 찾을수 없습니다."), MB_OK);
		i3mem::FillZero( &info.m_szWorkDir, sizeof(info.m_szWorkDir));
	}

	// Update
	g_pTDKEnvSet->setProject( &info);

	m_ED_WorkDir.SetModify( FALSE);

	_UpdateAll();
	UpdateData(FALSE);
}

void i3TDKDlgEnvSet::OnEnKillfocusTdkRichedDesc()
{
	if( m_ED_Desc.GetModify() == FALSE)
		return;

	if( m_idxSelProject == -1)
		return;

	I3TDK_PROJECT_INFO info;

	info.init();

	// 현재의 Project 설정을 읽어온다.
	g_pTDKEnvSet->getProject( m_idxSelProject, &info);

	// 새로운 WorkDir을 구한다.
	m_ED_Desc.GetWindowText( info.m_szDesc, sizeof( info.m_szDesc));

	// Update
	g_pTDKEnvSet->setProject( &info);

	m_ED_Desc.SetModify( FALSE);
}

void i3TDKDlgEnvSet::OnNMDblclkTdkProjectList(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnSetActiveProject();

	*pResult = 0;
}

void i3TDKDlgEnvSet::OnOK()
{
	i3ResourceFile::SetWorkingFolder( (char *) g_pTDKEnvSet->getCurWorkDir());
	i3ResourceFile::setSearchExternRefEnable( true);

	CDialog::OnOK();
}
