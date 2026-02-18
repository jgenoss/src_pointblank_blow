#if !defined( __CUI_PANE_TEMPLATE_EXPLORER_H__)
#define __CUI_PANE_TEMPLATE_EXPLORER_H__

#include "UIPaneBase.h"

//템플릿 data는 트리구조로 저장된다. 
//노드는 FilterNode와 TemplateNode 두 종류가 있다. 
//ListView, TreeView는 단지 데이터를 보여주는 용도로만 사용됨.
//ListView, TreeView의 Item의 lParam에 노드의 포인터를 연결, 1:1매핑시킨다. 
class CUIPaneTemplateExplorer : public CUIPaneBase
{
	DECLARE_DYNAMIC( CUIPaneTemplateExplorer)

protected:
	DECLARE_MESSAGE_MAP()

	i3UILibrary*	m_pLibrary;

	CImageList		m_ImageListToolBar;
	
	CListCtrl		m_ListView;
	CTreeCtrl		m_TreeView;

	CToolBarCtrl		m_ToolBar;
	CStatusBarCtrl		m_StatusBar;

	CEdit				m_ED_Find;
	CString				m_FindFilter;

	i3UILTreeNode_Filter*		m_pRoot;
	i3UILTreeNode_Filter*		m_pCur;
	i3UILTreeNode_Template*		m_pCurTNode;

	BOOL			m_bShowListView;

	// drag & drop in listview
	bool			m_bDragItem;
	CImageList*		m_pDragImageList;
	INT32			m_nDragItem;
	INT32			m_nDropTarget;
	INT32			m_nOldTarget;
	HTREEITEM		m_hDragItem;
	HTREEITEM		m_hDropTarget;
	HTREEITEM		m_hOldTarget;

	// copy, cut, paste
	i3::vector<i3TreeNode*>	m_CopiedNodeList;		//i3TreeNode*
	bool			m_bCut;

	bool			m_bHovering;

protected:

	///////////////////////////////////////////////////////
	//m_ListView를 접근하는 함수들
	void	_InitListView( void);
	void	_InsertListViewItem( i3TreeNode* pNode);
	void	_UpdateListView( void);

	///////////////////////////////////////////////////////
	//m_TreeView를 접근하는 함수들
	void	_InitTreeView( void);
	void	_RecInsertTreeItem( i3TreeNode* pNode, HTREEITEM hParent = NULL);
	void	_UpdateTreeView( void);

	//
	bool		_BuildTree( void);
	bool		_GetUniqueName( i3TreeNode* pNode, const char* pszDefaultName, char* pBuf, INT32 nBufSize);
	bool		_IsUniqueName( i3TreeNode* pNode, const char* pszName);
	void		_GetCurrentFilterPath( char* pBuf, INT32 nBufSize);

	bool		_isEditable( i3TreeNode* pNode);
	bool		_isScriptEditable( i3TreeNode* pNode);
public:
	CUIPaneTemplateExplorer();
	virtual ~CUIPaneTemplateExplorer();

	void		SetUILibrary( i3UILibrary* pLib);
	void		UpdateView( void);

	i3UILTreeNode_Filter*		ConvertToFilterNode( i3TreeNode* pNode);
	i3UILTreeNode_Template*		ConvertToTemplateNode( i3TreeNode* pNode);

protected:
	// Events
	void	OnBtnTreeView( void);
	void	OnBtnIconView( void);
	void	OnBtnReportView( void);
	void	OnBtnUpperLevel( void);
	void	OnBtnNewFilter( void);
	void	OnBtnNewTemplate( void);
	void	OnBtnExportTemplate( i3UILTreeNode_Template* pNode);
	void	SetScript( void);
	void	RemoveScript( void);
	void	OnBtnEditScript( void);
	void	OnBtnSelectAll( void);
	void	OnUnselectAll( void);

	void	OnBtnRemove( void);
	void	OnBtnCopy( void);
	void	OnBtnCut( void);
	void	OnBtnPaste( i3TreeNode* pTarget = NULL);

	void	OnKeyDownReturn( void);
	void	OnKeyDownEscape( void);

	void	OnChangeFindString(void);

	//wrapper
	INT32			GetSelectedNodes( i3::vector<i3TreeNode*>& Out);	//list item: i3TreeNode*
	i3TreeNode*		GetFocusedNode( void);	
	i3TreeNode*		GetTreeNode( INT32 iItem);			// wrapper: DWORD_PTR GetItemData(   int nItem ) const;
	i3TreeNode*		GetTreeNode( HTREEITEM hItem);
	INT32			GetLVItemIndex( i3TreeNode* pNode);
	HTREEITEM		_RecFindTreeItem( HTREEITEM hRoot, i3TreeNode* pNode);
	HTREEITEM		GetTVItemIndex( i3TreeNode* pNode);

	HTREEITEM		_RecFindTreeItemByName( HTREEITEM hRoot, const char * pszName);
	HTREEITEM		FindTreeNodeByName( const char * pszName);

	void	setCurrentTNode( i3UILTreeNode_Template* pNodeToEdit = NULL);

	//ToolBar Button State(Enable/Disable)
	void		UpdateButtonStates( void);

	//StatusBar
	void		UpdateStatusBarText( void);

	void		EditLabel( i3TreeNode* pNode);

	void		_EditTemplate( i3UILTreeNode_Template * pTNode = NULL);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	
	afx_msg void OnLVNKeydown( NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLVNColumnClick( NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLVNEndLabelEdit( NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLVNBeginDrag( NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLVNItemChanged( NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLVNNMDblClk( NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnTVNBeginDrag( NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTVNNMDblClk( NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTVNEndLabelEdit( NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTVNSelChanged( NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTVNNMRClick( NMHDR* pNMHDR, LRESULT* pResult);


	afx_msg void OnTTNGetDispInfo( NMHDR* pNMHDR, LRESULT* pResult);
	
	
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	INT32		GetImageIndex( i3TreeNode* pNode);

	void	OnLoadTemplate( i3UITemplate * pTemplate);

	void		UpdateOtherTemplateWindows( i3UILTreeNode_Template* pTNode);

	i3TreeNode*		CreateClone( i3TreeNode* pNode);
	void			AdjustNames( i3TreeNode* pNode);

	void			Rec_AddToLibrary( i3TreeNode* pNode);
	void			Rec_RemoveFromLibrary( i3TreeNode* pNode);

	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void			EditTemplate( i3UITemplate * pTemplate);

	typedef std::vector<char*> TreeNodeCon;
	TreeNodeCon m_vTreeNode;

	bool	IsDragItem() { return m_bDragItem; }
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs) override;

	void	OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
};

#endif //__CUI_PANE_TEMPLATE_EXPLORER_H__