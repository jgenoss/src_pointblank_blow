// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CClassTreeWnd window

enum UITEMPLATE_TISTATE
{
	UIT_IMASK_EMPTY = 0,
	UIT_IMASK_SCRIPT,
	UIT_IMASK_ENABLE,
	UIT_IMASK_DISABLE,
	/*UIT_IMASK_LOCK,
	UIT_IMASK_UNLOCK,*/

	UIT_IMASK_MAX,
};

class CTemplateTree: public CTreeCtrl
{
// Overrides
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;

// Implementation
public:
	CTemplateTree();
	virtual ~CTemplateTree();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

protected:
	CImageList * m_pImageList;		//Normal

	CImageList	m_ImageList2;		//State

public:
	void	InitImageList(void);
	INT32	GetIconIndex( i3ElementBase * pElement);
	INT32	GetLockStateImage( i3UIControl * pControl);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	void	setItemStateImage(HTREEITEM hItem, void * pData);

	//afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
