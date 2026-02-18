#pragma once

#include "afxwin.h"

#ifdef _USE_TOOLAM_
#include "ToolAM.h"
#endif


// CDlgAiAni 대화 상자입니다.

class CDlgAiAni : public CDialog	
#ifdef _USE_TOOLAM_
	, public ToolAM::CmdQueue::ReportCallBack
#endif
{
	DECLARE_DYNAMIC(CDlgAiAni)

public:
	CDlgAiAni(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgAiAni();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_AIANI};

	static int NotifyBox(HWND hWnd, const char* fmt, ...);

	void AddMsg(const char* msg);

#ifdef _USE_TOOLAM_
	// ToolAM::CmdQueue::ReportCallBack
	virtual void ToolAM_CmdQueue_Report(DWORD errFlag, ToolAM::CmdQueue::Data& cmdData,
		const char* message);
	virtual void ToolAM_CmdQueue_Report2(const char* message);
#endif

public:
	char m_ownerWorkDir[MAX_PATH];
private:
	CEdit m_Edit_1;
	CEdit m_Edit_2;
	CEdit m_Edit_4;

	CButton m_B_Trans_1;
	CButton m_B_Trans_2;
	CButton m_B_UI_Idle;
	char m_szFilePath[MAX_PATH];

	ChraEditor_NotifyBox_PFN			m_oldNotifyBoxPFN;
	static CDlgAiAni*					ms_cur;

	CComboBox m_AI_List;

	CString ListAutoRepierCharaName;
	void OnOpenChara( const char * pszPath);
	void OnCloseChara(void);
	void OnSaveChara( const char * pszPath);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
public:
	afx_msg void OnClickedBtn_OK();
	afx_msg void OnEnChangeEdI3sname();
	afx_msg void OnEnChangeEdI3sname2();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk3();
	afx_msg void OnBnClickedOk4();
	afx_msg void OnBnClickedOk5();
	afx_msg void OnLvnItemchangedExceptList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk2();
	afx_msg void OnBnClickedOk6();
	afx_msg void OnBnClickedOk7();
	afx_msg void OnBnClickedOk8();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnCbnSelchangeAiList();
	afx_msg void OnLvnItemchangedExceptList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEdPath4();
	afx_msg void OnBnClickedChkEnable3();
	afx_msg void OnBnClickedChkEnable();
	afx_msg void OnBnClickedChkEnable2();
	afx_msg void OnBnClickedOk9();
	afx_msg void OnEnChangeEdPath3();

	afx_msg void OnBnClickedOk11();
	afx_msg void InsertV1CharaListFromPreset();
	afx_msg void OnBnClickedOk10();
};


class CDlgAiAni_V1Preset : public CDialog
{
	DECLARE_DYNAMIC(CDlgAiAni_V1Preset)

public:
	CDlgAiAni_V1Preset(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgAiAni_V1Preset();

	enum { IDD = IDD_DLG_AIANI_V1_PRESET };

	struct Dat
	{
		char m_str[4][MAX_PATH] = { { 0 } };
	};

	typedef i3::vector<Dat>				Rst;
	Rst									m_rst;
	char								m_workDir[MAX_PATH];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	void RefreshRow(i3::vector<INT32>* rows=NULL);
	void SetFilter(const char* filterName=NULL);

public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBtnCancel();
	afx_msg void OnBtnOK();
	afx_msg void OnBtn_Set();
	afx_msg void OnBtn_UnSet();
	afx_msg void OnCB_ChangePreset();


private:
	std::vector<LVCOLUMN*>			m_columns;

	CComboBox*						m_cbPreset;
	CListCtrl*						m_lbPreset;
};