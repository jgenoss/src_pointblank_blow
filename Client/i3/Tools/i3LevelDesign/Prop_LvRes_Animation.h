#pragma once
#include "afxcmn.h"


// CProp_LvRes_Animation dialog
#include "ModifiedListCtrl.h"

class CProp_LvRes_Animation : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CProp_LvRes_Animation)

protected:
	i3LevelResAnimation	*	m_pResAnimation;

public:
	CProp_LvRes_Animation(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProp_LvRes_Animation();

// Dialog Data
	enum { IDD = IDD_PROP_LVRES_ANIMATION };

	virtual void	SetObject( i3ElementBase * pObj) override;

protected:
	void	_UpdateResInfo( void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CModifiedListCtrl m_listAnimInfo;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
