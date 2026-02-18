#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "ExportThread.h"

// CDlgExport dialog

class CDlgExport : public CDialog
{
	DECLARE_DYNAMIC(CDlgExport)
protected:
	CExportThread *	m_pThread;

	void	_LaunchGame(void);

public:
	CDlgExport(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgExport();

// Dialog Data
	enum { IDD = IDD_DLG_EXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	CEdit m_StateTextCtrl;
	CProgressCtrl m_ProgressCtrl;
	CListCtrl m_LogCtrl;
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	virtual void OnOK() override;
public:
	CEdit m_ED_GameBin;
	CButton m_CHK_SerializeRes;
	CButton m_CHK_LaunchGame;
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedChkExportRes();
	CComboBox m_CB_StageType;
	afx_msg void OnCbnSelchangeCbStagetype();
};
