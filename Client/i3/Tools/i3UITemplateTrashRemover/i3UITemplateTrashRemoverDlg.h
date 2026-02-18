
// i3UITemplateTrashRemoverDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// Ci3UITemplateTrashRemoverDlg dialog
class Ci3UITemplateTrashRemoverDlg : public CDialog
{
// Construction
public:
	Ci3UITemplateTrashRemoverDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~Ci3UITemplateTrashRemoverDlg();

// Dialog Data
	enum { IDD = IDD_I3UITEMPLATETRASHREMOVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	bool	CreateRenderer(void);
	bool	SaveTemplateLibrary();

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnClear();
	CEdit m_edWorkDir;
	afx_msg void OnBnClickedBtnWorkdir();

protected:
	//Renderer Create
	i3Viewer			*		m_pViewer;
	i3Framework			*		m_pFramework;

public:
	CStatic m_stRender;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	CEdit m_edLog;

	//log
	void OutputNotice( const char* pszString, bool bConcat);
	afx_msg void OnBnClickedBtnSave();
};
