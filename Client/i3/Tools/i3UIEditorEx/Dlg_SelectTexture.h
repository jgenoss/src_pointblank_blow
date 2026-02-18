#pragma once
#include "afxcmn.h"

#include "i3TDKDlg_SelectVirtualTexture.h"
#include "afxwin.h"

class cTreeTexture : public CTreeCtrl
{
	DECLARE_DYNAMIC(cTreeTexture)
public:
	cTreeTexture();
	virtual ~cTreeTexture();

	CDialog* m_pParent;
	void SetEventWnd(CDialog* pWnd) { m_pParent = pWnd; }

	DECLARE_MESSAGE_MAP()

	afx_msg void	OnSetFocus(CWnd* pOldWnd);
};
// CDlg_SelectTexture dialog

class CDlg_SelectTexture : public CDialog
{
	DECLARE_DYNAMIC(CDlg_SelectTexture)

protected:
	i3TDKDlg_SelectVirtualTexture	m_TexCtrl;

	I3VT_TEXINFO *	m_pSelTex;

	i3VirtualTexture * m_pVTex;
	cTreeTexture m_TreeCtrl;
	CImageList* m_pImageList;

	INT32			m_SplitPos;

	HCURSOR			m_curNormal;
	HCURSOR			m_curSize;
	bool			m_bDragging;
	bool			m_bInSplitPos;

	INT32			m_RightMargine;

	INT32			m_idxFolderImage;
	INT32			m_idxSelFolderImage;
	std::string		m_szPath;



protected:
	void		PlaceControls( INT32 cx = -1, INT32 cy = -1);

	void		ChangeCursor( bool bSizing);

	void		UpdateAllTextures(void);
	void		ParsePath( const char * pszPath);
	HTREEITEM	FindTreeItemByName( HTREEITEM hParent, const char * pszName);

	void		SearchRefTextures( const char * pszPath = NULL, bool bOnlyName = false);
	void		GetFullPath( HTREEITEM hItem, char * pszStr, INT32 len);

	void		LoadProfiles(void);
	void		SaveProfiles(void);

	void		GetFullPath( HTREEITEM hItem, char * pszDest);
	void		NewRule( HTREEITEM hItem);
	void		DeleteRule( HTREEITEM hItem);
	void		PopupSpec( HTREEITEM hItem);

public:
	CDlg_SelectTexture(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_SelectTexture();

	bool		Execute( i3VirtualTexture * pVTex);

	I3VT_TEXINFO *	getSelectedRefTexture(void);
	void		OnManualSetFocus();

// Dialog Data
	enum { IDD = IDD_SELECT_TEXTURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog() override;
	CButton m_BTN_Ok;
	CButton m_BTN_Cancel;
	CButton m_BTN_AddTexture;
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	virtual void OnOK() override;
public:
	afx_msg void OnBnClickedBtnAdd();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	CEdit m_ED_Search;
	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnChangeEdSearch();
//	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
//	afx_msg void OnClose();
protected:
	virtual void OnCancel() override;
public:
	afx_msg void OnBnClickedBtnDeleteTexture();
	afx_msg void OnBnClickedBtnSelectAdjustTexture();
	afx_msg void OnBnClickedBtnSelectAdjustVTexList();
	afx_msg void OnBnClickedBtnCopyAndReloadVTexInfo();

	CButton m_btnDelete;
	CButton m_btnAdjust;
	CButton m_btnNewAdjust;
	CButton m_btnCopyAndReload;

	afx_msg void OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnChangeFormat();
	CButton m_BTN_Compress;

	static void CALLBACK ProgressCallBack(int nRange, int nPos);

protected:
	bool			m_bNeedToSaveTemplate_ByEditVTex;
	bool			m_bNeedToSaveTemplate_ByVTexFormat;
};
