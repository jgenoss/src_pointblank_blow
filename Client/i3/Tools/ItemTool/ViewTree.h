#pragma once


// cViewTree

class cViewTree : public CTreeCtrl
{
	DECLARE_DYNAMIC(cViewTree)

public:
	cViewTree();
	virtual ~cViewTree();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


