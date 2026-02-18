#pragma once


// CPanelResource dialog
#include "ResTreeCtrl.h"
#include "afxcmn.h"
#include "afxwin.h"

class CPanelResource : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CPanelResource)

protected:
	i3LevelResRef	*m_pRoot;
	HTREEITEM		m_hTypeRoot[I3LV_RESDB_TYPE_COUNT];
	bool			m_bDragging;
	CImageList *	m_pDragImage;
	HTREEITEM		m_hTargetItem;
	HTREEITEM		m_hDragItem;
	INT32			m_FileCount;
	HTREEITEM		m_hSelectedResourceItem;	//기본적으로 제공되는 ResourceList

	char			m_szDB[ MAX_PATH];

public:
	CPanelResource(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPanelResource();

// Dialog Data
	enum { IDD = IDD_PANEL_RESOURCE };

	void	BuildResTree( void);
	void	BuildResTree( INT32 typeIdx);

	//void		Scan( const char * pszDB);

	HTREEITEM	getTypeRoot( I3LV_RESDB_ROOT_TYPE Type)	{	return m_hTypeRoot[Type];}

protected:
	//PopUp Menu
	void	_AddNewResource(void);	
	void	_DeleteResource(HTREEITEM hItem);	
	void	_RescanResourceList(void);	
	void	_SelectAll(void);
	void	_CollapseAll(void);			
	void	_ExpandAll(void);
	void	_AddNewWorld( HTREEITEM hItem);
	void	_ReplaceResource( HTREEITEM hItem);

	void	_OnUpdate( I3_TDK_UPDATE_INFO * pInfo);
	void	_OnPopupMenu( CMenu * pMenu, HTREEITEM hItem, i3LevelResRef * pRef);

	void	OnScan_Start(void);
	void	OnScan_Config( INT32 maxCount);
	void	OnScan_End(i3::vector<i3::rc_string>* pList);
	void	OnScan_Res( char * pszPath);

	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CResTreeCtrl	m_ResTreeCtrl;

	virtual BOOL OnInitDialog() override;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTvnSelchangedTreeReslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnBegindragTreeReslist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnNMRclickTreeReslist(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
public:
	afx_msg void OnResourceCheckdatabase();
	CStatic m_ST_Title;
	CProgressCtrl m_Progress;
	CEdit m_ED_File;
	afx_msg void OnDestroy();
};
