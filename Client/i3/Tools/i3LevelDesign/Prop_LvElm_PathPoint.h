#pragma once
#include "afxwin.h"


// CProp_LvElm_PathPoint dialog

class CProp_LvElm_PathPoint : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CProp_LvElm_PathPoint)

protected:
	i3LevelPathPoint *		m_pPoint;
	i3TDKColorPreviewCtrl	m_ColorPreviewCtrl;

public:
	virtual void	SetObject( i3ElementBase * pObj) override;

public:
	CProp_LvElm_PathPoint(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProp_LvElm_PathPoint();

// Dialog Data
	enum { IDD = IDD_PROP_LVELM_PATHPOINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ED_Priority;
	CStatic m_ColorPlaceHolder;
	afx_msg void OnEnKillfocusEdPriority();
	afx_msg void OnBnClickedBtnSelColor();
	virtual BOOL OnInitDialog() override;
protected:
	virtual void OnOK() override;
	virtual void OnCancel() override;
};
