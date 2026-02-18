#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CPanel_Files dialog

class CPanel_Files : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanel_Files)

protected:
	void	UpdateWorkDir(void);
	void	UpdateAll(void);

	void	OnDelete(void);
	void	OnSelectAll(void);
	void	OnInvertSelection(void);
	void	OnRemoveNonExistFiles(void);
	void	OnImportFileList( bool bMerge);
	void	OnExportFileList(void);

	void	ReplaceControls( INT32 cx = -1, INT32 cy = -1);

public:
	void	StartProcessing(void);
	void	EndProcessing(void);

	void	AddFile( const char * pszAbsPath);
	void	LoadFileList( const char * pszPath, bool bMerge = false);

public:
	CPanel_Files(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanel_Files();

// Dialog Data
	enum { IDD = IDD_PANEL_FILES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_LIST_File;
	CEdit m_ED_WorkDir;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;

public:
	afx_msg void OnNMRClickListFile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
