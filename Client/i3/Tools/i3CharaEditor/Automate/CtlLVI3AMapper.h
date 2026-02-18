
#ifndef _CTLLVI3AMAPPER_H_
#define _CTLLVI3AMAPPER_H_

#include <CommCtrl.h>
#include "afxcview.h"

#include <vector>


class CtlLVI3AMapper : public CListView
{
	DECLARE_DYNCREATE(CtlLVI3AMapper)

	// Parent Overrides
public:
//	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
//	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
//	virtual void OnInitialUpdate();
//	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);

public:
	CtlLVI3AMapper();
	virtual ~CtlLVI3AMapper();

	void RefreshAIState();
	void ClearView();
	
private:
	void GenerateHeaderColumnData();
	void GenerateRow();

private:
	CListCtrl*									m_lv;
	std::vector<LVCOLUMN*>						m_columns;

	// Afx Message Handle
public:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg void OnColumnClick(UINT id, NMHDR* pNotifyStruct, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


#endif