#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "Warn.h"
#include "FileInfo.h"
#include "ScanThread.h"

// CPanelResult dialog

class CPanelResult : public CDialog
{
	DECLARE_DYNAMIC(CPanelResult)

public:
	CPanelResult(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelResult();

// Dialog Data
	enum { IDD = IDD_PANEL_RESULT };

protected:
	i3List			m_WarnList;
	i3List			m_FileList;
	ScanThread *	m_pScanThread;

protected:
	void		_RemoveAllWarn(void);
	void		_RemoveAllFile(void);

	void		_StartScan(void);
	void		_ReportScan( char * pszName);
	void		_EndScan(void);
	void		_CancelScan(void);
	void		_AddFile( CFileInfo * pInfo);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CEdit m_ED_Path;
	CEdit m_ED_Status;
	CListCtrl m_LST_File;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnPath();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	CButton m_BTN_Go;
	CButton m_BTN_Path;
	afx_msg void OnBnClickedBtnDetectOption();
	afx_msg void OnBnClickedBtnGo();
};
