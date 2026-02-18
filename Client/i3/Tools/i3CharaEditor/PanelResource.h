#include "afxcmn.h"
#pragma once

enum RSC_CATEGORY
{
	RSC_CHARA = 0,
	RSC_SCENEGRAPH,
	RSC_LOD,
	RSC_ANIMATION,
	RSC_SOUND,
	RSC_TEXTURE,
	RSC_AI,

	RSC_TYPECOUNT
};

// CPanelResource dialog

class CPanelResource : public CDialog
{
	DECLARE_DYNAMIC(CPanelResource)

protected:
	HTREEITEM	m_hRootItem[ RSC_TYPECOUNT];
	bool		m_bDragging;
	i3ElementBase *	m_pDragObject;
	CImageList *	m_pDragImage;
	CImageList *	m_pTreeCtrlImageList;
	HTREEITEM	m_hTargetItem, m_hDragItem;

	HCURSOR		m_hCursor[2];

protected:
	void		OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

	HTREEITEM	getRootItem( i3ResourceObject * pRes);
	void		GetInfoString( char * pszBuf, i3ResourceObject * pRes);

	HTREEITEM	AddRes( HTREEITEM hParentItem, i3ResourceObject * pRes);
	HTREEITEM	FindRes( i3ResourceObject * pRes);
	void		RemoveRes( i3ResourceObject * pRes);
	void		RemoveRes( HTREEITEM hItem);
	void		RemoveAllRes( i3ClassMeta * pType = NULL);
	void		UpdateRes( HTREEITEM hItem, i3ResourceObject * pRes);
	void		UpdateAllRes(void);
	void		UpdateAllLOD(void);
	void		UpdateChara(void);

	i3AIContext *	_CreateCloneAITree( i3AIContext * pNode);
	void		_Rec_UpdateAI( HTREEITEM hParentItem, i3GameNode * pNode);
	void		UpdateAllAI(void);

	static bool	_AITraverseProc( i3GameNode * pNode, i3GameNodeTraversal * pTr, void * pUserData);

	void		_AdjustPopupMenu( CMenu * pMenu, HTREEITEM hItem, i3ElementBase * pObj);

	void		OnDelete( HTREEITEM hItem, i3ElementBase * pObj);
	void		OnAddLODFromSg( i3ElementBase * pObj);
	void		OnLODMoveUp( HTREEITEM hItem);
	void		OnLODMoveDown( HTREEITEM hItem);
	void		OnLODSort(void);
	void		OnNewAI( HTREEITEM hItem, i3ElementBase * pObj);
	void		OnAIExport( HTREEITEM hItem, i3ElementBase * pObj);
	void		OnAIImport( HTREEITEM hItem, i3ElementBase * pObj);
	void		OnDumpAI( HTREEITEM hItem, i3ElementBase * pObj);
	void		OnRenameFolder( );
	void		OnAddAIStateToChilds( HTREEITEM hItem, i3ElementBase * pObj);

	INT32		DumpAIContext( i3AIContext * pContext, i3::vector<i3::rc_string>& List);

public:
	CPanelResource(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelResource();

// Dialog Data
	enum { IDD = IDD_PANEL_RESOURCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_TreeCtrl;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnTvnSelchangedRscTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickRscTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBeginlabeleditRscTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnEndlabeleditRscTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBegindragRscTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);


//////////////////////////////////////////////////////////////////////////
#ifdef _USE_TOOLAM_
//////////////////////////////////////////////////////////////////////////
public:
	BOOL AM_I3CHR_SetName(const char* name);
	BOOL AM_I3CHR_AddI3SLOD_HERO(const char* workpath, const char* subpath, 
		const char* name, DWORD lodFlag, std::vector<std::string>* heroNames);

#endif // end of _USE_TOOLAM_

};
