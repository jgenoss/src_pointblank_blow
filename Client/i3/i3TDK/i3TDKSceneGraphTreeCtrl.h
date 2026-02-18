#pragma once

#include "i3TDKUpdateSystem.h"

// i3TDKSceneGraphTreeCtrl

#define		I3TDK_SGTREE_SHOW_NAME		0x00000001
#define		I3TDK_SGTREE_SHOW_FLAGS		0x00000002
#define		I3TDK_SGTREE_SHOW_ATTRS		0x00000004

class I3_EXPORT_TDK i3TDKSceneGraphTreeCtrl : public CWnd
{
	DECLARE_DYNAMIC(i3TDKSceneGraphTreeCtrl)

protected:
	CTreeCtrl		m_TreeCtrl;
	i3SceneGraphInfo *	m_pSgInfo = nullptr;
	CImageList *	m_pImageList = nullptr;
	DWORD			m_dwStyle = 0;
	UINT32			m_ShowStyle = I3TDK_SGTREE_SHOW_NAME | I3TDK_SGTREE_SHOW_FLAGS | I3TDK_SGTREE_SHOW_ATTRS;

	HTREEITEM		m_hCommonRootItem = nullptr;
	HTREEITEM		m_hInstanceRootItem = nullptr;

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
	void		Rec_SetTree( HTREEITEM hParentItem, i3Node * pNode);

	INT32		getChildItemCount( i3ElementBase * pObj);

	void		AddIcon( INT32 id);

	void		ToggleShowName(void);
	void		ToggleShowFlags(void);
	void		ToggleShowAttrs(void);

	void		DragDrop( HTREEITEM hSrcItem, HTREEITEM hTargetItem, bool bClone);

	
	void		_DoOptimize( i3SceneOptimizer * pOpt, HTREEITEM hItem, i3Node * pRoot);

	void		_OnAttrSet( i3AttrSet * pNode, HTREEITEM hItem);
	void		_OnLightSet( i3LightSet * pNode, HTREEITEM hItem);
	void		_OnGeometry( i3Geometry * pNode, HTREEITEM hItem);
	void		_OnOcTree( i3OcTreeNode * pNode, HTREEITEM hItem);
	void		_OnColliderSet( i3ColliderSet * pNode, HTREEITEM hItem);
	void		_OnSkin( i3Skin * pNode, HTREEITEM hItem);
	void		_OnBody( i3Body * pNode, HTREEITEM hItem);
	void		_OnBinaryPartitionNode( i3BinaryPartitionNode * pNode, HTREEITEM hItem);
	void		_OnTimeSequence( i3TimeSequence * pNode, HTREEITEM hItem);

	HTREEITEM	_FindTreeItemByObj( i3ElementBase * pObj);
	void		OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

public:
	CTreeCtrl *		GetTreeCtrl(void)						{ return &m_TreeCtrl; }

	void		SetSceneGraph( i3Node * pRoot);
	i3Node *	GetSceneGraph(void)							{ return (m_pSgInfo != nullptr)?m_pSgInfo->getInstanceSg():nullptr;}

	void		setSgInfo( i3SceneGraphInfo * pSgInfo);
	i3SceneGraphInfo *	getSgInfo(void)						{ return m_pSgInfo; }

	void		GetCheckedItem( i3::vector<HTREEITEM>& List);
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
	void		PopupOptProgress( i3SceneOptimizer * pOpt);

	virtual	void	OnPopupMenu( CMenu * pMenu, HTREEITEM hSelItem);
	virtual void	OnSelectMenu( UINT id);
	virtual void	OnOptimize( i3ClassMeta * pMeta, HTREEITEM hItem);

	virtual void	OnChangeSelectItem( HTREEITEM hOld, HTREEITEM hNew);
	virtual void	OnSetSceneGraph(void);

public:
	i3TDKSceneGraphTreeCtrl() {}
	virtual ~i3TDKSceneGraphTreeCtrl();

	BOOL	Create( DWORD dwStyle, const RECT &rect, CWnd * pParentWnd, UINT nID);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMRclickBoneTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawBoneTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTVNBeginDrag( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnTVNSelChanged( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnTVNKeyDown( NMHDR * pNMHDR, LRESULT * pResult);

protected:

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


