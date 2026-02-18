#pragma once
#include "afxcmn.h"

// PaneUVEdit dialog
class CPaneEventnAction : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPaneEventnAction)

public:
	CPaneEventnAction(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneEventnAction();

public:
// Dialog Data
	enum { IDD = IDD_PANE_EVENTNACTION };

//	void	KeyDown( UINT nChar );
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

protected:
	I3UI_EVT			m_nState;
	i3UIControl		*	m_pObj;

	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo );
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	void	_SetEventSettings( i3GameNode * pObj);
	void	_RefreshActionList();
	void	_SetActionSettings( INT32 nIdx, i3UIControl * pObj);

	void	_AddAction(UINT32 nAction);
	void	_Remove_Action( INT32	nItem);
	void	_EditAction( INT32 nItem);
public:
	CListCtrl m_ctrlStateList;
	CListCtrl m_ctrlActionList;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMClickListEnaState(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickListEnaAction(NMHDR *pNMHDR, LRESULT *pResult);
};
