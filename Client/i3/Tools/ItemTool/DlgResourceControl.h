#pragma once
#include "afxcmn.h"
#include "ListResourceFile.h"
#include "afxwin.h"
class COutlookBar : public CMFCOutlookBar
{
	virtual BOOL AllowShowOnPaneMenu() const { return TRUE; }
	virtual void GetPaneName(CString& strName) const { BOOL bNameValid = strName.LoadString(IDS_OUTLOOKBAR); ASSERT(bNameValid); }
};
// cDlgResourceControl 대화 상자입니다.

enum eFILTER
{
	E_FILTER_I3S,
	E_FILTER_I3I,
	E_FILTER_WAV,
	MAX_FILTER
};

class cDlgResourceControl : public CDialog
{
	DECLARE_DYNAMIC(cDlgResourceControl)

public:
	cDlgResourceControl(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~cDlgResourceControl();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_ADD_RESOURCE };

public:
	void			InitFileTree(const CString& strPath, const CString& strRootName);
	void			EnumChildItems(LPSHELLFOLDER pFolder, LPITEMIDLIST pParent, HTREEITEM hParent);
	void			EnumFolderFiles(LPSHELLFOLDER pFolder, LPITEMIDLIST pParent, HTREEITEM hParent);
	void			FillItem(TVINSERTSTRUCT &tvi, LPSHELLFOLDER pFolder, LPITEMIDLIST pidl, LPITEMIDLIST pidlRelative);
	void			FillItemToList(TVINSERTSTRUCT &tvi, LPSHELLFOLDER pFolder, LPITEMIDLIST pidl, LPITEMIDLIST pidlRelative);
	void			OnFileFilter();

	void			UTStrretToString(LPITEMIDLIST pidl, LPSTRRET pStr, LPSTR pszBuf, LPWSTR szGet);
	void			OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	void			OnItemAddToList(NMHDR* pNMHDR, LRESULT* pResult);

	LPITEMIDLIST	ILAppend(LPITEMIDLIST pidlParent, LPITEMIDLIST pidlChild);
	LPITEMIDLIST	ILGetNextItemID(LPCITEMIDLIST pidl);
	UINT			ILGetSize(LPCITEMIDLIST pidl);

	CString			GetFullPath(const CString& str);

	void			GetTreeFullPath(HTREEITEM hParent, CString path, CString& strOut);
	BOOL			DeleteAllFolder( CString strPath );
	BOOL			DeleteFile2( const char * pszFileName, bool bUndo );
private:
	COutlookBar			m_wndNavigationBar;
	CMFCShellTreeCtrl	m_wndTree;
	LPSHELLFOLDER		m_pDesktopFolder;
	LPMALLOC			m_pMalloc;
	LPITEMIDLIST		m_pidlDesktop;
	HTREEITEM			m_hDesktopItem;
	
	CTreeCtrl			m_treeExplorer;
	cListResourceFile	m_listFiles;
	int					m_nListCnt;
	int					m_nTreeCnt;

	typedef std::map<HTREEITEM, CString> mStrNameCon;
	mStrNameCon m_mFolder;

	struct sFileInfo
	{
		CString strName;
		int nIcon;
	};
	typedef std::vector<sFileInfo> vFileInfo;
	typedef std::vector<CString> vFileName;
	vFileName m_vFilterName;
	vFileInfo m_vCurrentFileName;
	vFileInfo m_vAllFileName;

	NM_TREEVIEW* m_pNMCurrentTreeView;
	HTREEITEM m_hItem;
	CString* m_strRootDir;
	CString m_subDir;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL	OnInitDialog();

	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStrucmait);
	afx_msg void	OnTvnItemexpandingTreeExplorer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnTvnItemChangingTreeExplorer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnTvnSelchangedTreeExplorer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void	OnHdnItemdblclickListFiles(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_btn_file_all;
	CButton m_btn_file_i3s;
	CButton m_btn_file_i3i;
	CButton m_btn_file_wav;
	afx_msg void OnBnClickedSubPathRoot();
	afx_msg void OnBnClickedSubPathWav();
	afx_msg void OnBnClickedSubPathWeapon();
	afx_msg void OnPath();
	afx_msg void OnUpdatePath(CCmdUI *pCmdUI);
	afx_msg void OnEnSetfocusEditPath();

	afx_msg void OnBnClickedCheckFileAll();
	afx_msg void OnBnClickedCheckFile();
	afx_msg void OnBnClickedBtnResAdd();

	void FileSubPath(HTREEITEM hItem, CString& strPath);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTvnKeydownTreeExplorer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTreeExplorer(NMHDR *pNMHDR, LRESULT *pResult);
};
