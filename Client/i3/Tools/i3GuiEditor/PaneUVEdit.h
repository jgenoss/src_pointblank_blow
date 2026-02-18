#pragma once


// PaneUVEdit dialog
#include "UVEditCtrl.h"

class CPaneUVEdit : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPaneUVEdit)

public:
	CPaneUVEdit(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneUVEdit();

// Dialog Data
	enum { IDD = IDD_PANE_UVEDIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CUVEditCtrl	m_UVEditCtrl;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog() override;

protected:
	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo );
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
};
