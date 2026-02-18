#pragma once
#include "afxcmn.h"


// CPanel_Log dialog

class CPanel_Log : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanel_Log)

protected:
	INT32		m_MaxFileProgress;
	INT32		m_CurFile;

	INT32		m_MaxOpt;
	INT32		m_CurOpt;
	INT32		m_MaxOptSub;
	INT32		m_CurOptSub;

public:
	CPanel_Log(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanel_Log();

// Dialog Data
	enum { IDD = IDD_PANEL_LOG };

	void	ReplaceControls( INT32 cx = -1, INT32 cy = -1);

	void	OnOptFile( INT32 idx);
	void	OnOptOpt( INT32 idx);
	void	OnOptEnd(void);
	void	OnOptCancel(void);

	void	StartProcessing(void);
	void	EndProcessing(void);

	static void _MyProgressProc( I3OPT_PROGRESS_REPORT report, INT32 param1, INT32 param2, const char * pszMsg, void * pUserData);

	void	UpdateProgressFile(void);
	void	UpdateProgressOpt(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_PRG_File;
	CProgressCtrl m_PRG_Opt;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
