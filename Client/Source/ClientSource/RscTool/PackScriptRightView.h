#pragma once


// PackScriptRightView view

#include "i3MFCListCtrl.h"
#include "PackMode.h"

class PackScriptFile;
class PackDlg;

class PackScriptRightView : public CView
{
	DECLARE_DYNCREATE(PackScriptRightView)

protected:
	PackScriptRightView();           // protected constructor used by dynamic creation
	virtual ~PackScriptRightView();

public:
	void				UpdateListCtrl(class PackScriptFolder* pData);
	void				DoPackScriptJobWithSelectedItems(PackMode mode);
	
	PackScriptFolder*	GetCurrPackScriptFolder() const { return m_folder; }

	void				Update_OnRename(PackScriptFile* file);
	void				Update_BeforeDelete(PackScriptFile* file);
	void				Update_OnAddFile(PackScriptFile* file );


public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

private:
	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	i3MFCListCtrl	m_listCtrl;
	PackScriptFolder*	m_folder;



public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


