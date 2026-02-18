#if !defined( __I3_TDK_LISTCTRL_H)
#define __I3_TDK_LISTCTRL_H

//	=================================================================================
//	i3TDKListCtrl
//	Programed : komet
//	Build Date : 2006.6.8
//	Last Update : 2006.6.27
//	=================================================================================
#pragma once

#include "i3TDKListCtrlEdit.h"
#include "i3TDKListCtrlComboBox.h"
#include "i3TDKPropertyToolTipCtrl.h"

#include "i3TDKPropertySubItem.h"
#include "i3TDKPropertySubItemStatic.h"
#include "i3TDKPropertySubItemEdit.h"
#include "i3TDKPropertySubItemComboBox.h"
#include "i3TDKPropertySubItemColor.h"

struct ColumnTitleInfo
{
	TCHAR	_szName[256] = { 0 };		//	Column 이름
	INT32	_nWidthRate = 0;		//	폭 비율 (%)

} ;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// i3TDKListCtrl

class I3_EXPORT_TDK i3TDKListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(i3TDKListCtrl)

protected:
	CFont			m_Font;

	i3::vector<ColumnTitleInfo*> m_ColumnList;				//	Title of Column
	i3::vector<i3::vector<i3TDKPropertySubItem*> >	m_SubItemList;				//	SubItem List. 안에 각 컬럼에 있는 SubItem이 담긴 List가 들어 있다.
	/*								Row					Column(0)		Column(1)	Column(2)
		m_SubItemList(0) -> SubItem list of column -> i3SubItem(0), i3SubItem(1), i3SubItem(2), ...
		m_SubItemList(1) -> SubItem list of column -> i3SubItem(0), i3SubItem(1), i3SubItem(2), ...
		m_SubItemList(2) -> SubItem list of column -> i3SubItem(0), i3SubItem(1), i3SubItem(2), ...
			:
			:
	*/

	i3TDKPropertySubItem *	m_pSelectedSubItem = nullptr;

	i3TDKPropertyToolTipCtrl m_ToolTip;
public:
	i3TDKListCtrl() {}
	virtual ~i3TDKListCtrl() {}

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);	//	Override

	BOOL	Create( DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT ID);
	void	ClearAll( void);
	void	ClearItem( void);	
	void	ClearColumn( void);  
	void	AddExtenedStyle( DWORD dwAddStyle);
	
	void	SetLogFont( const LOGFONT * pLogFont);

	//	Column
	INT32	AddColumnTitle( const TCHAR* pszColumn, INT32 nWidthRate, UINT32 nFmt = LVCFMT_LEFT);
	INT32	DeleteColumnTitle( INT32 nColumnIdx);
	INT32	GetColumnTitleCount( void)			{	return (size_t)m_ColumnList.size();					}

	//	Item
	INT32	NewSubItem( INT32 nAddCount = 1, INT32 nStartIdx = -1);
	BOOL	SetSubItem( INT32 nRowIdx, INT32 nColumnIdx, i3TDKPropertySubItem * pSubItem);	
	void	UpdateSubItem( SubItemUpdateInfo * pInfo, BOOL bMessageToParent = FALSE);
	BOOL	DeleteSubItem( INT32 nRowIdx);
	INT32	GetSubItemRowCount( void)			{	return (INT32)m_SubItemList.size();				}

	//	반환 Obj는 반드시 메모리에서 삭제해야 합니다.
	i3TDKPropertySubItemStatic *	SetSubItemStatic( SubItemStaticInitStruct * pInitStruct);
	i3TDKPropertySubItemEdit *		SetSubItemEdit( SubItemEditInitStruct * pInitStruct);
	i3TDKPropertySubItemComboBox *	SetSubItemComboBox( SubItemComboBoxInitStruct * pInitStruct);
	
	i3TDKPropertySubItem * GetSubItem( INT32 nRowIdx, INT32 nColumnIdx);
	i3TDKPropertySubItem * GetSelectedItem( void)			{	return m_pSelectedSubItem;						}

	const TCHAR *	GetItemLabel( INT32 nRowIdx, INT32 nColNum);  

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);

	afx_msg void OnLvnBeginlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	afx_msg void OnLvnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnToolTipText(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
};



#endif // __I3_TDK_LISTCTRL_H

