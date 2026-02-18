#pragma once
#include "afxcmn.h"


// CPanelSceneGraph dialog

class CPanelSceneGraph : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanelSceneGraph)

public:
	CPanelSceneGraph(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelSceneGraph();

// Dialog Data
	enum { IDD = IDD_PANEL_SCENEGRAPH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	virtual void OnOK() override;
	virtual void OnCancel() override;

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_SgListCtrl;
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
