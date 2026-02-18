#pragma once
#include "afxcmn.h"

#include "ColliderGroup.h"

// CPanelGroup dialog

class CPanelGroup : public CDialog
{
	DECLARE_DYNAMIC(CPanelGroup)

public:
	CPanelGroup(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelGroup();

	void		OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

	void		OnAddGroup( ColliderGroup * pGroup);
	void		OnRemoveGroup( ColliderGroup * pGroup);

	void		UpdateGroup( INT32 idx, ColliderGroup * pGroup);
	void		UpdateAllGroup(void);

// Dialog Data
	enum { IDD = IDD_PANEL_GROUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_GroupCtrl;
	virtual BOOL OnInitDialog();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnCancel();
	virtual void OnOK();
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
