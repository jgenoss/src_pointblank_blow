#if !defined( _CUI_PANE_TEMPLATE_H__)
#define _CUI_PANE_TEMPLATE_H__

#include "UIPaneBase.h"
#include "../Control/TemplateTree.h"

// CUITemplateWindow
// CUITemplate 및 연관 클래스
//1. 같은 컨트롤에 대해서 서로 다른 모양과 동작을 정의할 수 있다.

//2. 컨트롤들을 라이브러리처럼 활용할 수 있다. (다른 사람이 만들어 놓은 것을 import하는 방식)

//3. 일련의 컨트롤들의 집합을 하나의 테마처럼 관리할 수 있다. (트리 구조)

//4. 템플릿이 갖춰야 할 3가지 조건(편집, 공유, 관리)
//	a.	편집이 편하다.
//	b.	연결성이 남아있어야 한다.
//	c.	템플릿 제작자와 스크린 제작자 사이에는 작업의 독립성이 보장되어야 한다.

//5. 필요 기능
//	a. 구조 관련
//		1. Filter : 테마 또는 유저가 구분을 위해 분류하기 위한 폴더
//			a. Filter아래에는 각 Control Type별 Folder가 존재하며, 각 type에 따라 분류됨.
//			b. Filter에는 이름을 지정해 줄 수 있음.
//		2. Template의 관리 단위는 Control로 제한한다.
//			a. 복합 Control로 이루어진 Control을 포함.
//	b. 기능 관련
//		1.Template Control 선택
//			a. Mouse LButton Click으로 선택시 선택상태
//				1. 템플릿 리스트는 클릭 이후 명시적으로 편집/추가/삭제/배치 등의 팝업컨텍스트 메뉴를 이용하여 동작.
//				( 단순 클릭시 work space에 미치게 될 영향을 고려함. 클릭시 원치 않는 화면 전환 방지)
//			b. 선택 시 WorkSpace에서 

//		2.Template Control 추가 삭제
//			a. 추가: 제공되는 Control Class중 하나를 선택하여 추가
//			b. 삭제: 선택된 Template Control을 제거
//			c. Filter를 선택하여 추가 시 Control Class선택 다이얼로그 팝업.
//			d. Control Type Folder를 선택하여 추가 시 해당 Control Class를 자동으로 추가.

///////////////////////////////////////////////////////////////////////////////////////////////////////
// CUITemplate: Template관리 Panel
// Filter의 Root를 가지고 있으며
// 그를 보여주는 Tree Ctrl을 관리 한다.
///////////////////////////////////////////////////////////////////////////////////////////////////////
class CUIPaneTemplate : public CUIPaneBase
{
	DECLARE_DYNAMIC( CUIPaneTemplate)

protected:
	i3UILibrary			*	m_pLibrary;

	i3UILTreeFilter		*	m_pRoot;
	HTREEITEM				m_pRootItem;

	CTemplateTree			m_TreeCtrl;

	bool					m_bDragging;
	bool					m_bDropEnable;
	CImageList *			m_pDragImage;
	HTREEITEM				m_hTargetItem;
	HTREEITEM				m_hDragItem;

	//CWndScript				m_wnd;
public:
	CUIPaneTemplate();
	virtual ~CUIPaneTemplate();

protected:
	DECLARE_MESSAGE_MAP()


protected:
	HTREEITEM	AddTreeItem( i3NamedElement * pNode, HTREEITEM hParent);
	INT32		getChildItemCount( i3NamedElement * pNode);

	void			_SetTreeItem( TVITEM * pInfo, i3NamedElement * pNode, HTREEITEM hItem = NULL);
	bool			_Rec_DeleteItems( i3NamedElement * pItem);
	i3NamedElement *_Rec_FindParentByChild( i3NamedElement * pRoot, i3NamedElement * pItem);
	
	void		Rec_SetTree( HTREEITEM hParentItem, i3NamedElement * pNode);

	BOOL		IsValidItem( HTREEITEM hItem);

	void		AddFilter( HTREEITEM hItem);
	void		Add( HTREEITEM hItem);		// filter면 class select dialog를 element이면 바로 add후 tree에 추가.
	void		Edit( i3UITemplate * pTemplate);		// template edit mode enable. work space에 통지하여 template control edit mode로 설정
	void		Locate( HTREEITEM hItem);	// template locate mode enable. work space에 통지하여 template control locate mode로 설정
	bool		Delete( HTREEITEM hItem);	// 해당 filter or control제거
	void		Rename( HTREEITEM hItem);

	void		Cut( HTREEITEM hItem);		// filter
	void		Copy( HTREEITEM hItem);
	void		Paste( HTREEITEM hItem);

	void		EditScript( HTREEITEM hItem);
	void		SetScript( HTREEITEM hItem);
	void		RemoveScript( HTREEITEM hItem);
	void		TestScript( HTREEITEM hItem);


	bool		setUILibRoot( i3UILTreeFilter *pFilter);	// filter root설정

public:
	i3UILibrary *	GetUILibrary( void)		{ return m_pLibrary; }
	void		SetUILibrary( i3UILibrary * pLibrary);

	// 잡다 트리 컨트롤
	void		DragDrop( HTREEITEM hSrcItem, HTREEITEM hTargetItem, bool bClone);

	void		Expand( HTREEITEM hItem);
	void		Collapse( HTREEITEM hItem);
	void		SelectAll(void);
	void		UnselectAll(void);

	// 메뉴 
	virtual	void	OnPopupMenu( CMenu * pMenu, HTREEITEM hSelItem);
	virtual void	OnSelectMenu( UINT id);

	// save / load
	bool		Export( const char * pszFileName);
	bool		Import( const char * pszFileName);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRClickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTVNEndLabelEdit( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnTVNSelChanged( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnTVNKeyDown( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnTVNLButtonDbclk( NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnTVNBeginDragTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNMDblClickTree(NMHDR * pNMHDR, LRESULT *pResult);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnMButtonDown( UINT nFlags, CPoint point);

	void					OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	void	ShowTemplateTree( i3UITemplate* pTemplate);
	void	OverWriteTemplate( i3UITemplate* pTemplate, CPoint point);
	void	HighLightItemAtPoint( POINT point, bool bHighLight = true);

};

#endif ///_CUI_PANE_TEMPLATE_H__