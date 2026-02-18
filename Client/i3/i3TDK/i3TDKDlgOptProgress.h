#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "i3TDKLogCtrl.h"

// i3TDKDlgOptProgress dialog

class i3TDKDlgOptProgress : public CDialog
{
	DECLARE_DYNAMIC(i3TDKDlgOptProgress)
protected:
	i3SceneOptimizer *		m_pOpt = nullptr;
	i3TDKLogCtrl			m_LogCtrl;
	INT32					m_Max = 0;
	INT32					m_SubMax = 0;

	TCHAR					m_szProgress[256] = { 0 };
	TCHAR					m_szSubProgress[256] = { 0 };

public:
	i3TDKDlgOptProgress(CWnd* pParent = nullptr) : CDialog(i3TDKDlgOptProgress::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKDlgOptProgress() {}

// Dialog Data
	enum { IDD = IDD_DLG_OPTPROGRESS };

	void			Execute( i3SceneOptimizer * pOpt);
	void			Log( const TCHAR* pszStr);

	static void		_MyProgressProc( I3OPT_PROGRESS_REPORT report, INT32 param1, INT32 param2, const char * pszMsg, void * pUserData);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ED_Name;
	CStatic m_TXT_Progress;
	CProgressCtrl m_ProgressCtrl;
	CStatic m_ST_LogPH;
	CButton m_BTN_Close;
	virtual BOOL OnInitDialog();
	CStatic m_TXT_SubProgress;
	CProgressCtrl m_SubProgressCtrl;
};
