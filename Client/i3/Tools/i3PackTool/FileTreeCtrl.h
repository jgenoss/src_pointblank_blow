#if !defined(AFX_FileTreeCtrl_H__67F618C6_F36A_4BE3_9903_BC975EE707C7__INCLUDED_)
#define AFX_FileTreeCtrl_H__67F618C6_F36A_4BE3_9903_BC975EE707C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileTreeCtrl.h : header file
//
#include "shlobj.h"
#include "shlwapi.h"
#include <afxcoll.h>
#include "afxtempl.h"

// STRUCTURES

typedef struct TVITEMDATA
{
   LPSHELLFOLDER lpsfParent = nullptr;
   LPITEMIDLIST  lpi = nullptr;
   LPITEMIDLIST  lpifq = nullptr;
   bool          IsFolder = false;
   int			 nNumDirs = 0, nNumFiles = 0;
   int		     nSelFiles = 0, nSelDirs = 0, nPartSelDirs = 0;
   bool			 bRefresh = false;
   CString*		 sPath = nullptr;
   CString*		 FullName = nullptr;
} *LPTVITEMDATA;

#define TVN_STATECHANGED WM_USER+10

/////////////////////////////////////////////////////////////////////////////
// CFileTreeCtrl window
//	FileTree의 컨트롤 기능 클레스 

class CFileTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CFileTreeCtrl();
	BOOL DisplayTree();

// Attributes
public:
	enum CheckState{ REFRESH = -1, UNCHECKED,  UNCHECKED_REAL, CHILD_FILE, CHILD_ALLFILES, 
		CHILD_NOFILE_FOLDER, FILE, CHILD_FILE_FOLDER, CHILD_ALLFILES_FOLDER, 
		DISABLED, NOSTATE, CHILD_NOFILE_ALLFOLDERS = 12, CHILD_FILE_ALLFOLDERS=14, CHILD_ALL};
	enum ActionType{ NONE=0, CHILDREN, PARENTS, ALL };

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
private:
	virtual bool MatchExtension(CString file);

public:
	virtual ~CFileTreeCtrl();
	virtual BOOL SetCheck(_In_ HTREEITEM hItem, _In_ BOOL fCheck = TRUE)
	{
		return CTreeCtrl::SetCheck(hItem);
	}
	void AddHidFolder(int nFolder, CString strPath="");
	void FreeHidFoldersList();
	BOOL RefreshTree();
 	static int CALLBACK TreeViewCompareProc(LPARAM, LPARAM, LPARAM);
	CString GetInfo(HTREEITEM hItem);
	int GetSelectedFiles(CString* &SelFiles);
	int GetSelectedFolders(CString* &SelFolders);
	CString GetSelectedFile();
	BOOL SetScriptRefreshFolder();
	void SetScriptCheck(bool bselect) {m_bSelScript = bselect;	}
	void SetFindFile(char* filename) {
		if(*filename == 0)
			m_bFindFile = false;	
		else
		{
			m_bFindFile = true;	
			m_strMatchFile = filename;
		}
	}

	// Generated message map functions
protected:
	bool		m_bSelScript;
	bool		m_bFindFile;
	CString		m_strMatchFile;
	CImageList m_imgList,m_imgState;
	CString m_strError;
	CStringArray m_HidFolders;
	HTREEITEM m_hDesktop;
	static LPSHELLFOLDER m_lpsfDesktop;
	CMap<HTREEITEM,HTREEITEM,CString,CString> SelFiles;
	CMap<HTREEITEM,HTREEITEM,CString,CString> SelFolders;

	BOOL GetSysImgList();
    bool FillTreeView (LPSHELLFOLDER lpsf, LPITEMIDLIST lpifq, HTREEITEM hParent);
	void GetNormalAndSelectedIcons (LPITEMIDLIST lpifq, LPTV_ITEM lptvitem);
	int GetIcon(LPITEMIDLIST lpi, UINT uFlags);
	bool CheckForMatchedFiles(CString Path);
	HTREEITEM InsertItem(LPTVINSERTSTRUCT lpInsertStruct, bool bRefresh);
	UINT DeleteChildren (HTREEITEM hItem);
	bool ExpandAllRefresh(HTREEITEM hSelectItem);
	    
	// Functions that deal with PIDLs
    LPITEMIDLIST ConcatPidls(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
    LPITEMIDLIST GetFullyQualPidl(LPSHELLFOLDER lpsf, LPITEMIDLIST lpi);
    LPITEMIDLIST CopyITEMID(LPMALLOC lpMalloc, LPITEMIDLIST lpi);
    BOOL GetName(LPSHELLFOLDER lpsf, LPITEMIDLIST  lpi, DWORD dwFlags, CString &strName);    
	LPITEMIDLIST Create(UINT cbSize);
    UINT GetSize(LPCITEMIDLIST pidl);
    LPITEMIDLIST Next(LPCITEMIDLIST pidl);

	//Functions that deal with Hidden Folders
	BOOL IsHidFolder(CString Name);

	//Functions that deal with Checkboxes
	BOOL SetItemState(HTREEITEM hItem, int newState, int nAction = ALL);
	int  GetItemState(HTREEITEM hItem, LPTVITEMDATA* fInfo = NULL);
	BOOL CheckItem (HTREEITEM hItem);
	BOOL UncheckItem (HTREEITEM hItem);
	BOOL ToggleBackward (HTREEITEM hItem);
	BOOL ToggleForward (HTREEITEM hItem);
	BOOL RefreshItem (HTREEITEM hItem);
	BOOL SelectedCompletely(int nState, LPTVITEMDATA lptvid);


	//{{AFX_MSG(CFileTreeCtrl)
	afx_msg void OnDestroy();
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FileTreeCtrl_H__67F618C6_F36A_4BE3_9903_BC975EE707C7__INCLUDED_)
