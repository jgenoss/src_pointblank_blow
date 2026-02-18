#pragma once

#include "i3TDKUpdateSystem.h"

// i3TDKGameGraphTreeCtrl

class I3_EXPORT_TDK i3TDKGameGraphTreeCtrl : public CWnd
{
	DECLARE_DYNAMIC(i3TDKGameGraphTreeCtrl)

protected:
	CTreeCtrl		m_TreeCtrl;
	i3GameNode *	m_pRoot = nullptr;
	CImageList *	m_pImageList = nullptr;
	DWORD			m_dwStyle = 0;
	UINT32			m_ShowStyle = 0;

	HTREEITEM		m_hSrcItem = nullptr;
	HTREEITEM		m_hTargetItem = nullptr;
	bool			m_bDragging = false;
	bool			m_bDropEnable = false;
	HCURSOR			m_OldClassCursor = nullptr;
	HCURSOR			m_hDropCursor = nullptr;
	HCURSOR			m_hNoDropCursor = nullptr;

protected:
	void		_SetTreeItem( TVITEM * pInfo, i3ElementBase * pObj, const TCHAR* szMetaName, HTREEITEM hItem = nullptr);
	HTREEITEM	AddTreeItem( i3ElementBase * pObj, HTREEITEM hParent);
	void		Rec_SetTree( HTREEITEM hParentItem, i3GameNode * pNode);

	INT32		getChildItemCount( i3ElementBase * pObj);

	void		AddIcon( INT32 id);

	void		DragDrop( HTREEITEM hSrcItem, HTREEITEM hTargetItem, bool bClone);

	HTREEITEM	_FindTreeItemByObj( i3ElementBase * pObj);
	void		OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

public:
	CTreeCtrl *		GetTreeCtrl(void)						{ return &m_TreeCtrl; }

	void			setGameGraph( i3GameNode * pNode);
	i3GameNode *	getGameGraphRoot(void)					{ return m_pRoot; }

//	void		GetCheckedItem( List * pList);		// 쓰이지 않으므로 주석..
	BOOL		IsValidItem( HTREEITEM hItem);

	void		SetShowStyle( UINT32 style);
	UINT32		GetShowStyle(void)								{ return m_ShowStyle; }
	void		AddShowStyle( UINT32 style);
	void		RemoveShowStyle( UINT32 style);

	void		Expand( HTREEITEM hItem);
	void		Collapse( HTREEITEM hItem);
	void		SelectAll(void);
	void		UnselectAll(void);
	void		ReverseSelection(void);
	void		SelectDescendant( HTREEITEM hItem);
	void		UnselectDescendant( HTREEITEM hItem);
	void		SelectByType(void);
	void		Delete( HTREEITEM hItem);
	void		CollapseNode( HTREEITEM hItem);
	void		CollapseTransform( HTREEITEM hItem);
	void		RemoveNodeName(void);
	void		RemoveBoundBox(void);
	void		SetBoundBox(void);
	void		VisibleBoundBox( HTREEITEM hItem);
	void		InsertNode( HTREEITEM hParentItem);
	void		InsertAttr( HTREEITEM hParentItem);
	void		ImportNode( HTREEITEM hParentItem);
	void		ExportNode( HTREEITEM hSelItem);

	virtual	void	OnPopupMenu( CMenu * pMenu, HTREEITEM hSelItem);
	virtual void	OnSelectMenu( UINT id);

	virtual void	OnChangeSelectItem( HTREEITEM hOld, HTREEITEM hNew);
	virtual void	OnSetGameGraph(void);

public:
	i3TDKGameGraphTreeCtrl() {}
	virtual ~i3TDKGameGraphTreeCtrl();

	BOOL	Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMRclickBoneTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawBoneTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTVNBeginDrag( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnDestroy();
	afx_msg void OnTVNSelChanged( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnTVNKeyDown( NMHDR * pNMHDR, LRESULT * pResult);

protected:

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


