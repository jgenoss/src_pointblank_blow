#pragma once
#include "afxwin.h"
#include "AIGraphView.h"

// CPanelAIGraph dialog

enum EDIT_MODE
{
	EDIT_MODE_SELECT,
	EDIT_MODE_CREATE_AI,
	EDIT_MODE_LINK,
};

class CPanelAIGraph : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanelAIGraph)

protected:
	CAIGraphView 		m_View;
	EDIT_MODE			m_EditMode;

	char				m_szSrc[256];
	char				m_szDest[256];

protected:
	void			OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	void			AdjustAnimPath( i3AIState * pState);

public:
	CPanelAIGraph(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelAIGraph();

// Dialog Data
	enum { IDD = IDD_PANEL_AI_GRAPH };

	void			SetEditMode( EDIT_MODE mode);
	void			SetAI( AI * pAI);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnCancel();
	virtual void OnOK();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnCreateai();
	afx_msg void OnBnClickedBtnCreatenext();
	afx_msg void OnBnClickedBtnSelmode();
	CButton m_SelCtrl;
	CButton m_CreateAICtrl;
	CButton m_LinkAICtrl;
	CEdit m_NameCtrl;
	CComboBox m_AIStateCtrl;
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnCheckTemplate();
	afx_msg void OnBnClickedBtnAdjust();
	afx_msg void OnBnClickedBtnReplace();
};
