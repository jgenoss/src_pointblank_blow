#pragma once


// CDlg_Merge_1PV_3PV dialog

struct GROUP_INFO
{
	char	m_1PV[MAX_PATH] = { 0 };
	char	m_3PV[MAX_PATH] = { 0 };
	char	m_szName[128] = { 0 };
	char	m_szPath[MAX_PATH] = { 0 };
};

class CDlg_Merge_1PV_3PV : public CDialog
{
	DECLARE_DYNAMIC(CDlg_Merge_1PV_3PV)

protected:
	i3::vector<GROUP_INFO*>	m_GroupList;

protected:
	void	BuildGroups( i3::vector<i3::rc_string>& List);
	void	Merge( GROUP_INFO * pInfo);
	void	MergeAI(void);

public:
	CDlg_Merge_1PV_3PV(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_Merge_1PV_3PV();

// Dialog Data
	enum { IDD = IDD_MERGE_1PV_3PV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
