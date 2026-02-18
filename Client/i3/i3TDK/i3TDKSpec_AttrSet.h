#pragma once
#include "afxwin.h"


// i3TDKSpec_AttrSet dialog

class i3TDKSpec_AttrSet : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(i3TDKSpec_AttrSet)
protected:
	i3AttrSet *		m_pAttrSet = nullptr;

protected:

public:
	i3TDKSpec_AttrSet(CWnd* pParent = nullptr) : i3TDKDialogBase(i3TDKSpec_AttrSet::IDD, pParent) {}   // standard constructor
	virtual ~i3TDKSpec_AttrSet();

	virtual void			SetObject( i3ElementBase * pObj);

// Dialog Data
	enum { IDD = IDD_TDK_SPEC_ATTRSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	CEdit m_ED_Prio;
	afx_msg void OnEnKillfocusTdkEdPrio();
};
