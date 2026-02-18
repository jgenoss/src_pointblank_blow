#pragma once
#include "resource.h"
#include "DlgProject.h"
struct ProjectDirInfo
{
	char _ProjName[MAX_PATH] = { 0 };
	char _QAPath[MAX_PATH] = { 0 };
	char _LivePath[MAX_PATH] = { 0 };

	void CopyTo(CDlgProject* pProjDlg)
	{
		i3::sprintf(_ProjName, pProjDlg->GetName());
		i3::sprintf(_QAPath, pProjDlg->GetQAPath());
		i3::sprintf(_LivePath, pProjDlg->GetLivePath());
	}
};
class CPanelListProjcet : public CDialog
{
	DECLARE_DYNAMIC(CPanelListProjcet)
	DECLARE_MESSAGE_MAP()
	CListCtrl	m_ListCtrl;
	i3::vector<ProjectDirInfo*> m_vecProjInfo;
public:
	CPanelListProjcet();
	virtual ~CPanelListProjcet();

	enum { IDD = IDD_PANEL_PROJECT_DIR };

	void InsertProject(char* pchProjcetName);
	void AddProject();
	void RemoveAllInfo();
	void RemoveInfo();
	void EditInfo();
	void Save();
	void Load();
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;
	virtual BOOL OnInitDialog() override;
	afx_msg	void OnSize(UINT nType, int cx, int cy);

public:
	afx_msg void OnNMRClickListProject(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListProject(NMHDR *pNMHDR, LRESULT *pResult);
};

