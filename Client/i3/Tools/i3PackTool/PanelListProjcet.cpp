#include "stdafx.h"
#include "PanelListProjcet.h"
#include "GlobalVar.h"
#include "MainFrm.h"
#include "i3Base/string/ext/atoi.h"
#include "i3Base/string/ext/extract_directoryname.h"

IMPLEMENT_DYNAMIC(CPanelListProjcet, CDialog)

BEGIN_MESSAGE_MAP(CPanelListProjcet, CDialog)
	ON_WM_SIZE()

	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROJECT, &CPanelListProjcet::OnNMRClickListProject)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROJECT, &CPanelListProjcet::OnNMDblclkListProject)
END_MESSAGE_MAP()

CPanelListProjcet::CPanelListProjcet()
{
}

CPanelListProjcet::~CPanelListProjcet()
{
	Save();
	RemoveAllInfo();
}

void CPanelListProjcet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_PROJECT, m_ListCtrl);
}

void CPanelListProjcet::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
 
 	if (::IsWindow(m_ListCtrl.m_hWnd))
 	{
 		m_ListCtrl.SetWindowPos(NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
 	}
}

BOOL CPanelListProjcet::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ListCtrl.SetImageList(g_pSysImageList, TVSIL_NORMAL);

	Load();
	return TRUE;
}

void CPanelListProjcet::InsertProject(char* pchProjcetName)
{
	LVITEM item;
	item.mask = LVIF_TEXT | LVIF_IMAGE | LVCFMT_LEFT;
	item.iSubItem = 0;
	item.iImage = 3;
	item.iItem = m_vecProjInfo.size();
	item.pszText = pchProjcetName;
	m_ListCtrl.InsertItem(&item);
}

void CPanelListProjcet::OnNMRClickListProject(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CMenu	Menu, *pPopup;
	POINT pt;
	UINT32	cmd;
	//	HTREEITEM	hSel = NULL;

	GetCursorPos(&pt);

	Menu.LoadMenu(IDR_POPUP_PROJECT);

	pPopup = Menu.GetSubMenu(0);

	cmd = pPopup->TrackPopupMenuEx(TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
		pt.x, pt.y, (CWnd *) this, NULL);

	switch (cmd)
	{
		case ID_NEW_PROJECT:
			AddProject();
			break;
		case ID_DELETE_PROJECT:
			RemoveInfo();
			break;
		case ID_EDIT_PROJECT:
			EditInfo();
			break;
	}
	*pResult = 0;
}

void CPanelListProjcet::AddProject()
{
	CDlgProject dlg("", "", "");
	if (dlg.DoModal())
	{
		ProjectDirInfo* pinfo = new ProjectDirInfo;

		if (i3::generic_string_size(dlg.GetName()) > 0)
		{
			pinfo->CopyTo(&dlg);
			InsertProject((char*)dlg.GetName());
			m_vecProjInfo.push_back(pinfo);
		}
		else
			delete pinfo;
	}
}

void CPanelListProjcet::RemoveAllInfo()
{
	for (UINT32 i = 0; i < m_vecProjInfo.size(); i++)
	{
		delete m_vecProjInfo[i];
	}
	m_vecProjInfo.clear();
}

void CPanelListProjcet::RemoveInfo()
{
	INT32 nSelectIdx = m_ListCtrl.GetSelectionMark();
	if (nSelectIdx != -1)
	{
		i3::vector<ProjectDirInfo*>::iterator it;
		INT32 i = 0;
		for (it = m_vecProjInfo.begin(); it != m_vecProjInfo.end(); ++it)
		{
			if (i == nSelectIdx)
			{
				delete (*it);
				m_vecProjInfo.erase(it);

				break;
			}

			i++;
		}

		m_ListCtrl.DeleteAllItems();
		{
			for (UINT32 i = 0; i < m_vecProjInfo.size(); i++)
			{
				InsertProject(m_vecProjInfo[i]->_ProjName);
			}
		}
	}
}

void CPanelListProjcet::EditInfo()
{
	INT32 nSelectIdx = m_ListCtrl.GetSelectionMark();
	if (nSelectIdx != -1)
	{
		CDlgProject dlg(m_vecProjInfo[nSelectIdx]->_ProjName,
			m_vecProjInfo[nSelectIdx]->_QAPath, m_vecProjInfo[nSelectIdx]->_LivePath);

		if (dlg.DoModal())
		{
			m_vecProjInfo[nSelectIdx]->CopyTo(&dlg);
			m_ListCtrl.SetItemText(nSelectIdx, 0, dlg.GetName());
		}
	}
}


void CPanelListProjcet::OnNMDblclkListProject(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_vecProjInfo.size() > (UINT32)pNMItemActivate->iItem)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->m_PanelTreeScript.SetPath(m_vecProjInfo[pNMItemActivate->iItem]->_QAPath,
			m_vecProjInfo[pNMItemActivate->iItem]->_LivePath);
		
	}

	*pResult = 0;
}


void CPanelListProjcet::Load()
{
	char szPath[MAX_PATH] = { NULL, };
	char szDir[MAX_PATH] = { NULL, };
	GetModuleFileName(NULL, szPath, MAX_PATH);
	i3::extract_directoryname(szPath, szDir);
	strcat(szDir, "\\PackToolConfig.ini");

	char conv[MAX_PATH];

	GetPrivateProfileString("ProjectCount", "Count", "0", conv, MAX_PATH, szDir);
	INT32 nSize = i3::atoi(conv);
	for (INT32 i = 0; i < nSize; i++)
	{
		ProjectDirInfo* pinfo = new ProjectDirInfo;

		i3::sprintf(conv, "Project%d", i);
		GetPrivateProfileString(conv, "Name", "0", pinfo->_ProjName, MAX_PATH, szDir);
		GetPrivateProfileString(conv, "QAPath", "0", pinfo->_QAPath, MAX_PATH, szDir);
		GetPrivateProfileString(conv, "LivePath", "0", pinfo->_LivePath, MAX_PATH, szDir);

		InsertProject(pinfo->_ProjName);
		m_vecProjInfo.push_back(pinfo);
	}
}

void CPanelListProjcet::Save()
{
	char szPath[MAX_PATH] = { NULL, };
	char szDir[MAX_PATH] = { NULL, };
	GetModuleFileName(NULL, szPath, MAX_PATH);
	i3::extract_directoryname(szPath, szDir);
	strcat(szDir, "\\PackToolConfig.ini");

	DeleteFile(szDir);

	char conv[MAX_PATH];
	i3::sprintf(conv, "%d", m_vecProjInfo.size());
	WritePrivateProfileString("ProjectCount", "Count", conv, szDir);

	for (UINT32 i = 0; i < m_vecProjInfo.size(); i++)
	{
		i3::sprintf(conv, "Project%d", i);
		WritePrivateProfileString(conv, "Name", m_vecProjInfo[i]->_ProjName, szDir);
		WritePrivateProfileString(conv, "QAPath", m_vecProjInfo[i]->_QAPath, szDir);
		WritePrivateProfileString(conv, "LivePath", m_vecProjInfo[i]->_LivePath, szDir);
	}
}