#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "OptThread.h"

// CPanel_Opt dialog

class CPanel_Opt : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanel_Opt)

protected:
	HFONT	m_hFontTitle;
	HFONT	m_hFontConfig;
	CImageList *	m_pImageList;
	COptThread *	m_pThread;

	char	m_szConfigPath[ MAX_PATH];

protected:
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

	void	AddOpt( i3SceneOptimizer * pOpt);
	void	RemoveOpt( i3SceneOptimizer * pOpt);
	void	RemoveAllOpt(void);
	INT32	FindOpt( i3SceneOptimizer * pOpt);
	void	UpdateItem( INT32 idx, i3SceneOptimizer * pOpt);
	void	UpdateAllItem(void);
	INT32	getSelectedItem(void);

	void	ResolveXML( i3XMLElement * pXML, char * pszStr, INT32 length);

public:
	CPanel_Opt(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanel_Opt();

	void	StartProcessing(void);
	void	EndProcessing(void);

	void	LoadConfig( const char * pszPath);

// Dialog Data
	enum { IDD = IDD_PANEL_OPT_SELECTED };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_BTN_Open;
	CButton m_BTN_Save;
	CButton m_BTN_SelAll;
	CButton m_BTN_Del;
	CButton m_BTN_Run;
	CButton m_BTN_Stop;
	CListCtrl m_LIST_Opt;
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnBnClickedBtnOpenConfig();
	afx_msg void OnBnClickedBtnSaveConfig();
	afx_msg void OnBnClickedBtnSelectall();
	afx_msg void OnBnClickedBtnDelete();
	afx_msg void OnBnClickedBtnRun();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnNMDblclkListOpts(NMHDR *pNMHDR, LRESULT *pResult);
};
