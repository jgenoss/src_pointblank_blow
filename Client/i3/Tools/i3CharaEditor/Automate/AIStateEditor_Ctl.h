
#ifndef _AISTATEEDIT_CTL_H_
#define _AISTATEEDIT_CTL_H_

#ifdef _USE_TOOLAM_

#include <vector>
#include <afxwin.h>
#include <afxcview.h>


namespace AIStateEdit
{


class AIStateEditListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(AIStateEditListCtrl)

public:
	AIStateEditListCtrl();
	virtual ~AIStateEditListCtrl();

	void RefreshAIState();

	LPCTSTR MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset);
	void SetImgListState(UINT bitmapID, INT bitmapW=16, INT growFactor=1, 
		COLORREF clrMask=RGB(255,0,0));

protected:
	void ClearView();
	void RepaintSelectedItems();

	bool SetFullRowSel(bool fullRowSel);

	void GenerateHeaderColumnData();
	void GenerateRow();

protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT drItemInfo);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// afx message handlers
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnSetImageList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetTextColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetTextBkColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSetBkColor(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);

private:
	std::vector<LVCOLUMN*>						m_columns;
	BOOL										m_fullRowSel;
	CImageList									m_stateImgLst;

		// modified by message handler
	bool										m_doClientWidthSel;
	int											m_clientWidth;
	int											m_stateImgOffW;
	COLORREF									m_clrTextBk;
	COLORREF									m_clrText;
	COLORREF									m_clrBk;
};

//class AIStateListViewFrame : public i3TDKDockFrameWnd
//{
//
//public:
//	AIStateListViewFrame();
//	virtual ~AIStateListViewFrame();
//
//	LRESULT _RouteWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
//
//protected: 
//	DECLARE_DYNAMIC(AIStateListViewFrame)
//
//protected:
//	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
//	
//protected:
//	DECLARE_MESSAGE_MAP()
//
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	afx_msg void OnDestroy();
//	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
//
//private:
//	AIStateEditListView				m_listview;
//
//public:
//	INT32							m_frameW;
//	INT32							m_frameH;
//	CDialog*						m_dlg;
//};


} // end of namespace AIStateEdit

#endif // _USE_TOOLAM_

#endif
