#pragma once
#include "afxwin.h"
#include "ClothFramework.h"

// CPaneViewer dialog

class CPaneViewer : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPaneViewer)

public:
	CPaneViewer(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPaneViewer();

// Dialog Data
	enum { IDD = IDD_PANE_VIEWER };

protected:
	i3Viewer *		m_pViewer;
	CClothFramework *	m_pFramework;

public:
	void	ReplaceControls( INT32 cx, INT32 cy);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CButton m_PlayCtrl;
	CButton m_StopCtrl;
	CStatic m_ViewerPlaceHolderCtrl;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnPlay();
	afx_msg void OnBnClickedBtnStop();
};
