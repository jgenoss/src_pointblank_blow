#pragma once


// PanelGame dialog

class PanelGame : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(PanelGame)

public:
	PanelGame(CWnd* pParent = NULL);   // standard constructor
	virtual ~PanelGame();

	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Dialog Data
	enum { IDD = IDD_PANEL_GAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	UINT32	_trackPopupMenu( CPoint point);
	void	_clearPanel( void);
	void	_addNewStage( HTREEITEM pTreeItem, char * pszName = NULL);
	void	_addStage( HTREEITEM pTreeItem, i3StageInfo * pStageInfo);
	void	_addNewLayer( HTREEITEM pTreeItem, char * pszName = NULL);
	void	_addLayer( HTREEITEM pTreeItem, i3SgLayer * pLayer);
	void	_deleteStage( HTREEITEM pTreeItem, BOOL bQuestion = TRUE, BOOL bUpdateSpec = TRUE);	
	void	_deleteLayer( HTREEITEM pTreeItem, BOOL bQuestion = TRUE, BOOL bUpdateSpec = TRUE);
	void	_addNewInsMng( HTREEITEM pTreeItem);
	void	_deleteInsMng( HTREEITEM pTreeItem);

	void	_addInstancelist( HTREEITEM pTreeItem, i3StageInfo * pStageInfo);
	void	_addInsMng( HTREEITEM pTreeItem, i3StageInstanceInfo * pInsInfo);
	
	HTREEITEM	_setCurItem( void);

protected:
	CTreeCtrl	m_TreeCtrl;
	HTREEITEM	m_hGameRoot;
	HTREEITEM	m_hStagesRoot;
	HTREEITEM	m_hLayersRoot;
	HTREEITEM	m_hCurItem;

	INT32		m_nAddStageNum;		//	추가되는 임시 스테이지 번호
	INT32		m_nAddLayerNum;		//	추가되는 임시 레이어 번호

public:
	void	ResetGame( BOOL bFrameworkClear = TRUE);
	void	BuildTreeCtrl( i3Framework * pFramework);

	BOOL	UpdateSelectedItem( void);
	void *	GetItemData( HTREEITEM hItem)		{	return (hItem ? (void *) m_TreeCtrl.GetItemData( hItem) : NULL);		}
	void *	GetSelectedItemData( void)			{	return GetItemData( m_hCurItem);			}
	HTREEITEM	GetSelectedItem( void)			{	return m_hCurItem;							}

public:

	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog() override;
	afx_msg void OnTvnEndlabeleditRscTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickRscTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnDestroy();	
	afx_msg void OnDropFiles(HDROP hDropInfo);	
	afx_msg void OnNMClickRscTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
};
