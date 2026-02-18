#pragma once
#include "afxcmn.h"

// Ci3Tree dialog

class Ci3Tree : public CDialog
{
	DECLARE_DYNAMIC(Ci3Tree)

protected:
	CImageList				m_ImageList;
	HCURSOR					m_hDragCursor; 
	HCURSOR					m_hNoDropCursor; 
	HTREEITEM				m_hDragItem;		//For Select Object Node
	HTREEITEM				m_hDropItem;		

	BOOL					m_Dragging; 
	BOOL					m_DropEnable; 
	LONG					m_OldClassCursor;
public:
	Ci3Tree(CWnd* pParent = NULL);   // standard constructor
	virtual ~Ci3Tree();

	i3Node			*		m_pScene; 
	HTREEITEM				m_hParent;
	i3ElementBase	*		m_pSelElement; 
	
// Dialog Data
	enum { IDD = IDD_DIALOG_TREE };

protected:	
	void	AssignSceneGraphToTree(void); 
	void	i3SceneGraphTree(i3Node * pNode); 
	void	PutTreeNode(HTREEITEM hItem, i3ElementBase * pElement);
	

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()


	//PopUp Memu Cmd	
	void	ContorlNode(HTREEITEM hItem, UINT cmd); 
	void	DeleteElement(HTREEITEM hItem);

	void	InserNode(HTREEITEM hItem);
	void	InserAttr(HTREEITEM hItem);
	void	ImportNode(HTREEITEM hItem);
	void	ExportNode(HTREEITEM hItem);
	void	SetBoundBox(HTREEITEM hItem);
	void	RemoveBoundBox(HTREEITEM hItem);
	void	AppendColliderBox( HTREEITEM hItem);
	void	AppendColliderSphere( HTREEITEM hItem);
	
	void	DragMoveItem(HTREEITEM hDropItem, HTREEITEM hDragItem); 

	void	Otitransform(HTREEITEM hItem); 		

	void	DeleteName( HTREEITEM hItem );
	void	DeleteNameDes( HTREEITEM hItem );

	//flzjtlvm gkrp 
	void	reDeleteName(i3Node * pNode, i3List * pList); 

public:
	void	SetSceneGraph(i3Node * pNode); 

	void	BuildRenderableColliderMesh(void);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CTreeCtrl m_TreeControl;
	afx_msg void OnTvnSelchangedTreeScene(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMRclickTreeScene(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	afx_msg void OnTvnBegindragTreeScene(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnNodeGeometrytool();
};
