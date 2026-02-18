#pragma once

#include "../Control/TemplateTree.h"
#include "UIPaneBase.h"
// CUIPaneScreen

class CUIPaneScreen : public CUIPaneBase
{
	DECLARE_DYNAMIC(CUIPaneScreen)

protected:
	i3UIScene		*	m_pScene;
	HTREEITEM			m_pRootItem;

	CTemplateTree		m_TreeCtrl;

	//
	HTREEITEM			m_hSrcItem;
	HTREEITEM			m_hTargetItem;
	bool				m_bDragging;
	bool				m_bDropEnable;
	CImageList	*		m_pDragImage;
	bool				m_bSkipMessage;	//tvn selchanged message를 skip여부
	//HCURSOR				m_OldClassCursor;

	bool			m_bHovering;

public:
	CUIPaneScreen();
	virtual ~CUIPaneScreen();

	void	Reset();
	void	SetScene( i3UIScene * pScene);

	void	AdjustTreeItem( i3GameNode * pNode);

	void		_SetTreeItem( TVITEM * pInfo, i3NamedElement * pNode, HTREEITEM hItem = NULL);
	HTREEITEM	AddTreeItem( i3NamedElement * pNode, HTREEITEM hParent, HTREEITEM hInsertAfter = TVI_LAST);
	void		Rec_SetTree( HTREEITEM hParentItem, i3GameNode * pNode);
	void		Rec_InsertTree( HTREEITEM hParentItem, i3GameNode * pNode, HTREEITEM hInsertAfter);

public:
	INT32			AddControl( i3UIControl	*	pCtrl);
	INT32			RemoveControl( i3UIControl	*	pCtrl);
	INT32			RemoveControl( INT32 idx);

	INT32			FindControl( i3UIControl *pCtrl);

public:
	BOOL		IsValidItem( HTREEITEM hItem);
	HTREEITEM	FindItem( i3UIControl* pCtrl);

	// Popup Menu
	void		Delete( HTREEITEM hItem);	
	void		Rename( HTREEITEM hItem);
	void		SetScript( HTREEITEM hItem);
	void		RemoveScript( HTREEITEM hItem);
	void		EditScript( HTREEITEM hItem);

	void		Cut( HTREEITEM hItem);		
	void		Copy( HTREEITEM hItem);
	void		Paste( HTREEITEM hItem);

	// 잡다 트리 컨트롤
	void		DragDrop( HTREEITEM hSrcItem, HTREEITEM hTargetItem, bool bClone);
	void		DragMove( HTREEITEM hSrcItem, HTREEITEM hTargetItem, bool bClone = false);

	void		Expand( HTREEITEM hItem);
	void		Collapse( HTREEITEM hItem);
	void		SelectAll(void);
	void		UnselectAll(void);

	//
	

	//메뉴
	virtual	void	OnPopupMenu( CMenu * pMenu, HTREEITEM hSelItem);
	virtual void	OnSelectMenu( UINT id);

	virtual BOOL	PreTranslateMessage(MSG* pMsg);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	HTREEITEM		_FindItemByName( HTREEITEM hRoot, const char* pszName);
	void			_SetUniqueName( i3UIControl* pCtrl);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTVNSelChanged( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnRClickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnTVNBeginDrag( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnTVNEndLabelEdit( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnNMClickSceneTree(NMHDR * pNMHDR, LRESULT *pResult);
	afx_msg void OnTVNKeyDown( NMHDR * pNMHDR, LRESULT *pResult);

	void					OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	
public:
	afx_msg void OnSysKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	
protected:
	void		_GetItemNames( HTREEITEM hRoot, i3::vector<i3::rc_string>& StrList);
public:
	void		GetItemNames( i3::vector<i3::rc_string>& StrList);
	void		returnToEditState(void);		// tree view에서 설정된 enable state에 맞춰 control들의 enable state를 역으로 설정합니다.

	//HTREEITEM		getRootItem( void)		{	return m_pRootItem;	}

public:
	void		_getAllChildControls( HTREEITEM hRoot, i3::vector<i3UIControl*>& Out);			//recursive
	void		getAllChildControls( i3ElementBase* pParent, i3::vector<i3UIControl*>& Out);
	
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void		MoveUp( HTREEITEM hItem);
	void		MoveDown( HTREEITEM hItem);

	void		EditTemplate( HTREEITEM hItem);

	// Import / Export Scene
	void		ImportScene( HTREEITEM hItem);
	void		ExportScene( HTREEITEM hItem);
	void		ShowListBox(HTREEITEM hItem);
	void		ShowDummyLine(HTREEITEM hItem);
};


