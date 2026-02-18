#pragma once
#include "afxwin.h"

#define I3GUI_GUITREE_SHOW_NAME		0x00000001
#define	I3GUI_GUITREE_SHOW_FLAGS	0x00000002
#define I3GUI_GUITREE_SHOW_ATTRS	0x00000003

class CGuiTreeCtrl : public CWnd
{
	DECLARE_DYNAMIC(CGuiTreeCtrl)

protected:
	CTreeCtrl		m_TreeCtrl;
	i3GameNode	*	m_pRoot;
	CImageList	*	m_pImageList;
	DWORD			m_dwStyle;
	UINT32			m_ShowStyle;

	INT32			m_nIndexOfFirstTDKImage;

	HTREEITEM		m_hSelected;
	HTREEITEM		m_hSrcItem;
	HTREEITEM		m_hTargetItem;
	bool			m_bDragging;
	bool			m_bDropEnable;
	HCURSOR			m_OldClassCursor;

	HCURSOR			m_hDropCursor;
	HCURSOR			m_hNoDropCursor;

	char			m_szSearchString[MAX_PATH];

protected:

	HTREEITEM		AddTreeItem( i3GameNode * pObj, HTREEITEM hParent, HTREEITEM hInsertAfter = TVI_LAST);
	void			Rec_SetTree( HTREEITEM hParentItem, i3GameNode * pNode, HTREEITEM hInsertAfter = TVI_LAST);
	void			ExchangePosition( HTREEITEM hSrcItem, HTREEITEM hTargetItem);

	INT32			GetChildItemCount( i3GameNode * pNode);

	void			_Rec_CreateClone( i3GuiObjBase * pSrcNode, i3GuiObjBase * pDstNode, I3_COPY_METHOD method);
	i3GuiObjBase *	_CreateClone( i3GuiObjBase * pSrcNode, I3_COPY_METHOD method);

	////
public:
	CTreeCtrl *		GetTreeCtrl( void)						{ return & m_TreeCtrl;}

	void			SetGuiGraph( i3GameNode * pRoot );
	i3GameNode *	GetGuiGraph(void)						{ return m_pRoot;}

	void			SetCurrentRootNode( HTREEITEM hItem);

	void			OnChangeSelectItem( HTREEITEM hOld, HTREEITEM hNew );

	void			MakeItemName( char * pszStr, i3GameNode * pNode );
	void			GetCheckedItem( i3::vector<HTREEITEM>& List);
	BOOL			IsValidItem( HTREEITEM hItem);

	void			DragDrop( HTREEITEM	hSrcItem, HTREEITEM hTargetItem, bool bClone);

	//
	void			CheckMnemonics(void);

	/////
	void			Expand( HTREEITEM hItem);
	void			Collapse( HTREEITEM hItem);
	void			SelectAll(void);
	void			UnselectAll(void);
	void			ReverseSelection(void);
	void			SelectDescendant( HTREEITEM hItem);
	void			UnselectDescendant( HTREEITEM hItem);
	void			SelectByType(void);
	void			Delete( HTREEITEM hItem);

	void			OnFindNode(HTREEITEM hItem = TVI_ROOT);
	void			OnFindNext();

	void			ExportNode(HTREEITEM hItem);
	void			ImportNode(HTREEITEM hItem);

	void			InsertControl( HTREEITEM hItem );
	void			InsertWindow( HTREEITEM hItem );
	void			SetTemplate( HTREEITEM hItem );
public:
	CGuiTreeCtrl(void);
	virtual ~CGuiTreeCtrl(void);

	BOOL			Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnNMRclickGuiTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTVNBeginDrag( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnTVNClicked( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnTVNSelChanged( NMHDR * pNMHDR, LRESULT * pResult );
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};
