// DlgResourceControl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ItemTool.h"
#include "DlgResourceControl.h"
#include "Shell.h"

#include "i3Base/string/ext/extract_fileext.h"

enum eSUB_PATH
{
	E_SUB_PATH_ROOT,
	E_SUB_PATH_WEAPON,
	E_SUB_PATH_SOUND,
	MAX_SUB_PATH
};

// cDlgResourceControl 대화 상자입니다.
const static CString STR_SUB_PATH[MAX_SUB_PATH] = 
{
	"Media",
	"Weapon",
	"Sound",
};
IMPLEMENT_DYNAMIC(cDlgResourceControl, CDialog)

cDlgResourceControl::cDlgResourceControl(CWnd* pParent /*=NULL*/)
	: CDialog(cDlgResourceControl::IDD, pParent)
	,	m_nListCnt(0)
	,	m_nTreeCnt(0)
	,	m_pidlDesktop(NULL)

{
	// TODO: add construction code here
	m_pDesktopFolder = NULL;
	SHGetDesktopFolder(&m_pDesktopFolder);

	m_pMalloc = NULL;
	SHGetMalloc(&m_pMalloc);

	m_strRootDir = NULL;
	m_strRootDir = &g_strRootPath;
}

cDlgResourceControl::~cDlgResourceControl()
{
	if (m_pidlDesktop != NULL)
		m_pMalloc->Free(m_pidlDesktop);

	if (m_pMalloc != NULL)
		m_pMalloc->Release();

	if (m_pDesktopFolder != NULL)
		m_pDesktopFolder->Release();
}

void cDlgResourceControl::DoDataExchange(CDataExchange* pDX)
{

	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_EXPLORER, m_treeExplorer);
	DDX_Control(pDX, IDC_LIST_FILES,	m_listFiles);
	DDX_Control(pDX, IDC_CHECK_FILE_ALL, m_btn_file_all);
	DDX_Control(pDX, IDC_CHECK_FILE_I3S, m_btn_file_i3s);
	DDX_Control(pDX, IDC_CHECK_FILE_I3I, m_btn_file_i3i);
	DDX_Control(pDX, IDC_CHECK_FILE_WAV, m_btn_file_wav);
}


BEGIN_MESSAGE_MAP(cDlgResourceControl, CDialog)
	ON_WM_CREATE()
	ON_NOTIFY(TVN_ITEMEXPANDING, IDC_TREE_EXPLORER, &cDlgResourceControl::OnTvnItemexpandingTreeExplorer)
	ON_NOTIFY(TVN_ITEMCHANGING, IDC_TREE_EXPLORER, &cDlgResourceControl::OnTvnItemChangingTreeExplorer)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_EXPLORER, &cDlgResourceControl::OnTvnSelchangedTreeExplorer)
	
	ON_BN_CLICKED(IDC_SUBPATH_ROOT, &cDlgResourceControl::OnBnClickedSubPathRoot)
	ON_BN_CLICKED(IDC_SUBPATH_WAV,	&cDlgResourceControl::OnBnClickedSubPathWav)
	ON_BN_CLICKED(IDC_SUBPATH_WEAPON, &cDlgResourceControl::OnBnClickedSubPathWeapon)

	ON_BN_CLICKED(IDC_CHECK_FILE_ALL, &cDlgResourceControl::OnBnClickedCheckFileAll)
	ON_BN_CLICKED(IDC_CHECK_FILE_I3S, &cDlgResourceControl::OnBnClickedCheckFile)
	ON_BN_CLICKED(IDC_CHECK_FILE_I3I, &cDlgResourceControl::OnBnClickedCheckFile)
	ON_BN_CLICKED(IDC_CHECK_FILE_WAV, &cDlgResourceControl::OnBnClickedCheckFile)
	ON_BN_CLICKED(IDC_BTN_RES_ADD, &cDlgResourceControl::OnBnClickedBtnResAdd)
	ON_WM_KEYDOWN()
	ON_NOTIFY(TVN_KEYDOWN, IDC_TREE_EXPLORER, &cDlgResourceControl::OnTvnKeydownTreeExplorer)
	ON_NOTIFY(NM_CLICK, IDC_TREE_EXPLORER, &cDlgResourceControl::OnNMClickTreeExplorer)
END_MESSAGE_MAP()

int cDlgResourceControl::OnCreate(LPCREATESTRUCT lpCreateStrucmait)
{
	if (CDialog::OnCreate(lpCreateStrucmait) == -1)
		return -1;

	return 0;
}

BOOL cDlgResourceControl::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_listFiles.Init();

	if (!m_strRootDir->IsEmpty())
		InitFileTree(*m_strRootDir, "Media");
	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void cDlgResourceControl::InitFileTree(const CString& strPath, const CString& strRootName)
{
	std::string tmp = strRootName;

	m_nListCnt = 0;
	m_nTreeCnt = 0;

	m_mFolder.clear();
	m_vAllFileName.clear();
	m_vCurrentFileName.clear();;
	m_listFiles.DeleteAllItems();
	m_treeExplorer.DeleteAllItems();
	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &m_pidlDesktop);

	//CShell::PathToPidl(szRoot, &m_pidlDesktop);

	SHFILEINFO sfi;
	ZeroMemory(&sfi, sizeof(SHFILEINFO));
	HIMAGELIST hSysImageList = (HIMAGELIST) SHGetFileInfo((LPCTSTR)m_pidlDesktop, 0,
		&sfi, sizeof(SHFILEINFO), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

	TreeView_SetImageList(m_treeExplorer.GetSafeHwnd(), hSysImageList, TVSIL_NORMAL);
	ListView_SetImageList(m_listFiles.GetSafeHwnd(), hSysImageList, LVSIL_SMALL);

	///////////////////////////////////////////////////
	// Insert Desktop Folder as a Tree View Item

	// to get file info
	UINT uFlags = SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON
		| SHGFI_DISPLAYNAME;

	ZeroMemory(&sfi, sizeof(SHFILEINFO));
	SHGetFileInfo((LPCTSTR) m_pidlDesktop, 0, &sfi, sizeof(SHFILEINFO), uFlags);

	TVINSERTSTRUCT tvi;
	ZeroMemory(&tvi, sizeof(TVINSERTSTRUCT));
	tvi.hParent = TVI_ROOT;
	tvi.hInsertAfter = TVI_LAST;
	tvi.item.pszText = const_cast<LPSTR>(tmp.c_str());
	tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
	tvi.item.iImage = tvi.item.iSelectedImage = sfi.iIcon;
	tvi.item.cChildren = 1;
	tvi.item.lParam = (LPARAM) m_pidlDesktop;

	m_hDesktopItem = m_treeExplorer.InsertItem(&tvi);

	///////////////////////////////////////////////////
	// Expand desktop folder

	CShell::PathToShellFolder(strPath, m_pDesktopFolder);
	EnumChildItems(m_pDesktopFolder, m_pidlDesktop, m_hDesktopItem);

	m_treeExplorer.Expand(m_hDesktopItem, TVE_EXPAND);
}
// cDlgResourceControl 메시지 처리기입니다.

void cDlgResourceControl::EnumChildItems(LPSHELLFOLDER pFolder, LPITEMIDLIST pParent, HTREEITEM hParent)
{
	LPENUMIDLIST pEnumIDList = NULL;
	LPITEMIDLIST pItem = NULL;
	LPITEMIDLIST pidlAbs = NULL;
	ULONG ulFetched = 0;
	TVINSERTSTRUCT tvi;

	if (pFolder->EnumObjects(m_treeExplorer.GetSafeHwnd(), SHCONTF_FOLDERS, &pEnumIDList) == NOERROR)
	{
		while (pEnumIDList->Next(1, &pItem, &ulFetched) == NOERROR)
		{
			// to memorize absolute PIDL
			pidlAbs = ILAppend(pParent, pItem);

			// to set tree view item
			ZeroMemory(&tvi, sizeof(TVINSERTSTRUCT));
			tvi.hParent = hParent;
			tvi.hInsertAfter = TVI_LAST;
			tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM;
			tvi.item.lParam = (LPARAM) pidlAbs;

			// fill another properties of item(.pszText, .iImage, ...)
			FillItem(tvi, pFolder, pidlAbs, pItem);

			m_pMalloc->Free(pItem);
		}

		pEnumIDList->Release();
	}

	OnFileFilter();
}
void cDlgResourceControl::EnumFolderFiles(LPSHELLFOLDER pFolder, LPITEMIDLIST pParent, HTREEITEM hParent)
{
	LPENUMIDLIST pEnumIDList = NULL;
	LPITEMIDLIST pItem = NULL;
	LPITEMIDLIST pidlAbs = NULL;
	ULONG ulFetched = 0;
	TVINSERTSTRUCT tvi;

	if (pFolder->EnumObjects(m_treeExplorer.GetSafeHwnd(), SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pEnumIDList) == NOERROR)
	{
		while (pEnumIDList->Next(1, &pItem, &ulFetched) == NOERROR)
		{
			// to memorize absolute PIDL
			pidlAbs = ILAppend(pParent, pItem);

			// to set tree view item
			ZeroMemory(&tvi, sizeof(TVINSERTSTRUCT));
			tvi.hParent = hParent;
			tvi.hInsertAfter = TVI_LAST;
			tvi.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN | TVIF_PARAM | TVIS_EXPANDED;
			tvi.item.lParam = (LPARAM) pidlAbs;

			// fill another properties of item(.pszText, .iImage, ...)
			FillItemToList(tvi, pFolder, pidlAbs, pItem);

			m_pMalloc->Free(pItem);
		}

		pEnumIDList->Release();
	}

	OnFileFilter();
}

void cDlgResourceControl::FillItem(TVINSERTSTRUCT &tvi, LPSHELLFOLDER pFolder, LPITEMIDLIST pidl, LPITEMIDLIST pidlRelative)
{
	STRRET sName;
	TCHAR tszBuf[MAX_PATH] = {0};
	char szBuf[MAX_PATH] = {0};
	DWORD dwAttributes = 0;
	UINT uFlags = 0;
	SHFILEINFO sfi;

	// ** In the virtual folder cases, SHGetFileInfo.szDisplayName is impossible

	///////////////////////////////////////////////////
	// Set Display Name

	if (tvi.item.mask & TVIF_TEXT)
	{
		// get display name
		pFolder->GetDisplayNameOf(pidlRelative, 0, &sName);
		UTStrretToString(pidlRelative, &sName, (LPSTR)szBuf, (LPWSTR)tszBuf);

		tvi.item.pszText = szBuf;
	}

	///////////////////////////////////////////////////
	// Set Display Image

	if (tvi.item.mask & (TVIF_IMAGE | TVIF_SELECTEDIMAGE))
	{
		// get some attributes
		dwAttributes = SFGAO_FOLDER | SFGAO_LINK;
		pFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&pidlRelative, &dwAttributes);

		// get correct icon
		uFlags = SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON;

		if (dwAttributes & SFGAO_LINK)
			uFlags |= SHGFI_LINKOVERLAY;

		if (tvi.item.mask & TVIF_IMAGE)
		{
			ZeroMemory(&sfi, sizeof(SHFILEINFO));
			SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(SHFILEINFO), uFlags);

			tvi.item.iImage = sfi.iIcon;
		}
		if (tvi.item.mask & TVIF_SELECTEDIMAGE)
		{
			if (dwAttributes & SFGAO_FOLDER)
				uFlags |= SHGFI_OPENICON;

			ZeroMemory(&sfi, sizeof(SHFILEINFO));
			SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(SHFILEINFO), uFlags);

			tvi.item.iSelectedImage = sfi.iIcon;
		}
	}

	///////////////////////////////////////////////////
	// Set (+) Button or not

	if (tvi.item.mask & (TVIF_CHILDREN))
	{
		// get some attributes
		dwAttributes = SFGAO_FOLDER;
		pFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&pidlRelative, &dwAttributes);

		// get children
		tvi.item.cChildren = 0;

		if (dwAttributes & SFGAO_FOLDER)
		{
			dwAttributes = SFGAO_HASSUBFOLDER;
			pFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&pidlRelative, &dwAttributes);

			tvi.item.cChildren = (dwAttributes & SHCIDS_ALLFIELDS) ? 1 : 0;
		}
	}

	tvi.item.state = m_nTreeCnt;
	HTREEITEM htree = m_treeExplorer.InsertItem(&tvi);
//	m_treeExplorer.SetItemData(htree);

	CString strOut;
	GetTreeFullPath(tvi.hParent, tvi.item.pszText, strOut);

	m_mFolder[htree] = strOut;
	m_nTreeCnt++;
}

void cDlgResourceControl::GetTreeFullPath(HTREEITEM hParent, CString path, CString& strOut)
{
	if (hParent)
	{
		CString name = m_treeExplorer.GetItemText(hParent);
		strOut = name + "\\" + path;

		hParent = m_treeExplorer.GetParentItem(hParent);
		if (hParent)
			GetTreeFullPath(hParent,strOut,strOut);
	}
}

void cDlgResourceControl::FillItemToList(TVINSERTSTRUCT &tvi, LPSHELLFOLDER pFolder, LPITEMIDLIST pidl, LPITEMIDLIST pidlRelative)
{
	//CTreeCtrl& Tree = m_treeExplorer;

	STRRET sName;
	TCHAR tszBuf[MAX_PATH] = {0};
	char szBuf[MAX_PATH] = {0};
	DWORD dwAttributes = 0;
	UINT uFlags = 0;
	SHFILEINFO sfi;

	// ** In the virtual folder cases, SHGetFileInfo.szDisplayName is impossible

	///////////////////////////////////////////////////
	// Set Display Name

	if (tvi.item.mask & TVIF_TEXT)
	{
		// get display name
		pFolder->GetDisplayNameOf(pidlRelative, 0, &sName);
		UTStrretToString(pidlRelative, &sName, (LPSTR)szBuf, (LPWSTR)tszBuf);

		tvi.item.pszText = szBuf;
	}

	///////////////////////////////////////////////////
	// Set Display Image

	if (tvi.item.mask & (TVIF_IMAGE | TVIF_SELECTEDIMAGE))
	{
		// get some attributes
		dwAttributes = SFGAO_FOLDER | SFGAO_LINK;
		pFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&pidlRelative, &dwAttributes);

		// get correct icon
		uFlags = SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON;

		if (dwAttributes & SFGAO_LINK)
			uFlags |= SHGFI_LINKOVERLAY;

		if (tvi.item.mask & TVIF_IMAGE)
		{
			ZeroMemory(&sfi, sizeof(SHFILEINFO));
			SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(SHFILEINFO), uFlags);

			tvi.item.iImage = sfi.iIcon;
		}
		if (tvi.item.mask & TVIF_SELECTEDIMAGE)
		{
			if (dwAttributes & SFGAO_FOLDER)
				uFlags |= SHGFI_OPENICON;

			ZeroMemory(&sfi, sizeof(SHFILEINFO));
			SHGetFileInfo((LPCTSTR)pidl, 0, &sfi, sizeof(SHFILEINFO), uFlags);

			tvi.item.iSelectedImage = sfi.iIcon;
		}
	}

	///////////////////////////////////////////////////
	// Set (+) Button or not

	if (tvi.item.mask & (TVIF_CHILDREN))
	{
		// get some attributes
		dwAttributes = SFGAO_FOLDER | SHCIDS_ALLFIELDS;
		pFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&pidlRelative, &dwAttributes);

		// get children
		tvi.item.cChildren = 0;

		if (dwAttributes & SFGAO_FOLDER)
		{
			dwAttributes = SFGAO_HASSUBFOLDER;
			pFolder->GetAttributesOf(1, (LPCITEMIDLIST *)&pidlRelative, &dwAttributes);

			tvi.item.cChildren = (dwAttributes & SHCIDS_ALLFIELDS) ? 1 : 0; 
		}
	}
	if (!(dwAttributes & SFGAO_FOLDER))
	{
		sFileInfo info;
		info.strName = szBuf;
		info.nIcon = sfi.iIcon;

		m_vAllFileName.push_back(info);
	}
}
void cDlgResourceControl::OnFileFilter()
{
	m_listFiles.DeleteAllItems();
	m_nListCnt = 0;
	for (size_t ui = 0; ui < m_vAllFileName.size(); ++ui)
	{
		CString tmp = m_vAllFileName[ui].strName;
		int nPos = tmp.ReverseFind('.');
		tmp = tmp.Mid(nPos+1, tmp.GetLength());
		tmp.MakeLower();
		for (int i = 0; i < (int)m_vFilterName.size(); ++i)
		{
			if (m_vFilterName[i] == tmp)
			{
				sFileInfo info;
				info.strName = m_vAllFileName[ui].strName;
				info.nIcon = m_vAllFileName[ui].nIcon;

				m_vCurrentFileName.push_back(info);
				m_listFiles.InsertItem(m_nListCnt, m_vAllFileName[ui].strName,info.nIcon);
				m_nListCnt++;
				break;
			}
		}
	}
}

void cDlgResourceControl::UTStrretToString(LPITEMIDLIST pidl, LPSTRRET pStr, LPSTR pszBuf, LPWSTR szGet)
{
	//	lstrcpy(pszBuf, L"");

	switch(pStr->uType)
	{
	case STRRET_WSTR: // Unicode string
		WideCharToMultiByte(CP_ACP, 0, pStr->pOleStr, -1, pszBuf, MAX_PATH, NULL, NULL);
		break;

	case STRRET_OFFSET: // Offset
		lstrcpyA(pszBuf, reinterpret_cast<LPSTR>(pidl) + pStr->uOffset);
		break;

	case STRRET_CSTR: // ANSI string
		lstrcpyA(pszBuf, pStr->cStr);
		break;
	}
}

void cDlgResourceControl::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	// TODO: Add your control notification handler code here
	CTreeCtrl& Tree = m_treeExplorer;

	if (pNMTreeView->action & TVE_EXPAND)
	{
		///////////////////////////////////////////////////
		// insert item when it needs children
		if (Tree.GetChildItem( pNMTreeView->itemNew.hItem ) == NULL)
		{
			SetCursor(LoadCursor(NULL, IDC_WAIT));

			LPITEMIDLIST pidl;
			LPSHELLFOLDER pFolder;

			pidl = (LPITEMIDLIST) Tree.GetItemData(pNMTreeView->itemNew.hItem);

			if (pNMTreeView->itemNew.hItem != m_hDesktopItem)
			{
				m_pDesktopFolder->BindToObject(pidl, NULL, IID_IShellFolder, (LPVOID *)&pFolder);

				EnumChildItems(pFolder, pidl, pNMTreeView->itemNew.hItem);

				pFolder->Release();
			}
			else
				EnumChildItems(m_pDesktopFolder, pidl, pNMTreeView->itemNew.hItem);

			SetCursor(LoadCursor(NULL, IDC_ARROW));
		}
	}
	

	*pResult = 0;
}

void cDlgResourceControl::OnItemAddToList(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_listFiles.DeleteAllItems();
	m_vAllFileName.clear();
	m_vCurrentFileName.clear();

	m_nListCnt = 0;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	// TODO: Add your control notification handler code here
	CTreeCtrl& Tree = m_treeExplorer;
	
	//SetCursor(LoadCursor(NULL, IDC_WAIT));

	LPITEMIDLIST pidl;
	LPSHELLFOLDER pFolder;

	pidl = (LPITEMIDLIST) Tree.GetItemData(pNMTreeView->itemNew.hItem);

	if (pNMTreeView->itemNew.hItem != m_hDesktopItem)
	{
		m_pDesktopFolder->BindToObject(pidl, NULL, IID_IShellFolder, (LPVOID *)&pFolder);

		EnumFolderFiles(pFolder, pidl, pNMTreeView->itemNew.hItem);

		pFolder->Release();
	}
	else
		EnumFolderFiles(m_pDesktopFolder, pidl, pNMTreeView->itemNew.hItem);

	*pResult = 0;
}

// refer to `Working with Item ID Lists` of MSDN
LPITEMIDLIST cDlgResourceControl::ILGetNextItemID(LPCITEMIDLIST pidl)
{
	// Check for valid pidl.
	if (pidl == NULL)
		return NULL;

	// Get the size of the specified item identifier.
	int cb = pidl->mkid.cb;

	// If the size is zero, it is the end of the list.
	if (cb == 0)
		return NULL;

	// Add cb to pidl (casting to increment by bytes).
	pidl = (LPITEMIDLIST) (((LPBYTE) pidl) + cb);

	// Return NULL if it is null-terminating, or a pidl otherwise.
	return (pidl->mkid.cb == 0) ? NULL : (LPITEMIDLIST) pidl;
}

UINT cDlgResourceControl::ILGetSize(LPCITEMIDLIST pidl)
{
	UINT cbTotal = 0;

	if (pidl)
	{
		cbTotal += sizeof(pidl->mkid.cb); // Null terminator
		while (pidl)
		{
			cbTotal += pidl->mkid.cb;
			pidl = ILGetNextItemID(pidl);
		}
	}

	return cbTotal;
}

LPITEMIDLIST cDlgResourceControl::ILAppend(LPITEMIDLIST pidlParent, LPITEMIDLIST pidlChild)
{
	LPITEMIDLIST pidl = NULL;

	if(pidlParent == NULL)
		return NULL;
	if(pidlChild == NULL)
		return pidlParent;

	UINT cb1 = ILGetSize(pidlParent) - sizeof(pidlParent->mkid.cb);
	UINT cb2 = ILGetSize(pidlChild);

	pidl = (LPITEMIDLIST)m_pMalloc->Alloc(cb1 + cb2);

	if(pidl) {
		CopyMemory(pidl, pidlParent, cb1);
		CopyMemory(((LPSTR)pidl) + cb1, pidlChild, cb2);
	}

	return pidl;
}

void cDlgResourceControl::OnTvnItemexpandingTreeExplorer(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	OnItemexpanding(pNMHDR, pResult);
}

void cDlgResourceControl::OnTvnItemChangingTreeExplorer(NMHDR *pNMHDR, LRESULT *pResult)
{
	//NMTVITEMCHANGE *pNMTVItemChange = reinterpret_cast<NMTVITEMCHANGE *>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void cDlgResourceControl::OnTvnSelchangedTreeExplorer(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
	OnItemAddToList(pNMHDR, pResult);
}

void cDlgResourceControl::OnHdnItemdblclickListFiles(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	//OnItemexpanding(pNMHDR, pResult);
}

void cDlgResourceControl::OnBnClickedSubPathRoot()
{
	m_subDir = g_strRootPath;
	InitFileTree(m_subDir, STR_SUB_PATH[E_SUB_PATH_ROOT]);
}

void cDlgResourceControl::OnBnClickedSubPathWav()
{
	m_subDir = g_strRootPath + "\\" + STR_SUB_PATH[E_SUB_PATH_SOUND];
	InitFileTree(m_subDir, STR_SUB_PATH[E_SUB_PATH_SOUND]);
}

void cDlgResourceControl::OnBnClickedSubPathWeapon()
{
	m_subDir = g_strRootPath + "\\" + STR_SUB_PATH[E_SUB_PATH_WEAPON];
	InitFileTree(m_subDir, STR_SUB_PATH[E_SUB_PATH_WEAPON]);
}

void cDlgResourceControl::OnBnClickedCheckFileAll()
{
	m_vFilterName.clear();
	int nCheck = m_btn_file_all.GetCheck();
	if (nCheck)
	{
		m_vFilterName.push_back("i3s");
		m_vFilterName.push_back("i3a");
		m_vFilterName.push_back("wav");
		m_btn_file_i3s.SetCheck(nCheck);
		m_btn_file_i3i.SetCheck(nCheck);
		m_btn_file_wav.SetCheck(nCheck);
	}
	else
	{
		m_btn_file_i3s.SetCheck(nCheck);
		m_btn_file_i3i.SetCheck(nCheck);
		m_btn_file_wav.SetCheck(nCheck);
	}
	OnFileFilter();
	//OnItemAddToList(m_pNMCurrentTreeView, NULL);
}
void cDlgResourceControl::OnBnClickedCheckFile()
{
	m_vFilterName.clear();
	int nCheckedCnt = 0;
	int nCheck = m_btn_file_i3s.GetCheck();
	if (nCheck)
	{
		m_vFilterName.push_back("i3s");
		nCheckedCnt++;
	}

	nCheck = m_btn_file_i3i.GetCheck();
	if (nCheck)
	{
		m_vFilterName.push_back("i3a");
		nCheckedCnt++;
	}

	nCheck = m_btn_file_wav.GetCheck();
	if (nCheck)
	{
		m_vFilterName.push_back("wav");
		nCheckedCnt++;
	}
	
	if (nCheckedCnt == (int)MAX_FILTER)
	{
		m_btn_file_all.SetCheck(1);
	}
	else
	{
		m_btn_file_all.SetCheck(0);
	}
	OnFileFilter();
	//OnItemAddToList(m_pNMCurrentTreeView, NULL);
}

void cDlgResourceControl::OnBnClickedBtnResAdd()
{
	int nItem = -1;
	for (UINT32 i = 0; i < m_listFiles.GetSelectedCount(); ++i)
	{
		nItem = m_listFiles.GetNextItem(nItem, LVNI_SELECTED);

		CString pSelName = (CString)m_listFiles.GetItemText(nItem,0);

		CString strFullName = GetFullPath(pSelName);

		char szExt[32];
	//	i3String::SplitFileExt(strFullName, szExt,sizeof(szExt)-1);
		i3::extract_fileext((const char*)strFullName, szExt);

		CString tmpExt = szExt;
		tmpExt.MakeLower();
		if(tmpExt == "i3s")
		{
			g_pChara->LoadSceneGraph( strFullName);
		}
		if(tmpExt == "i3a")
		{
			g_pChara->LoadAnim( strFullName);
		}
		if(tmpExt == "wav")
		{
			g_pChara->LoadSound( strFullName);
		}
	}
	

// 	for each(sFileInfo value in m_vCurrentFileName)
// 	{
// 		CString strFullName = GetFullPath(value.strName);
// 
// 		char szExt[32];
// 		i3String::SplitFileExt(strFullName, szExt,sizeof(szExt)-1);
// 
// 		if( i3String::Compare( szExt, "i3s") == 0)
// 		{
// 			g_pChara->LoadSceneGraph( strFullName);
// 		}
// 		else if( i3String::Compare( szExt, "i3a") == 0)
// 		{
// 			g_pChara->LoadAnim( strFullName);
// 		}
// 		else if(i3String::Compare( szExt, "wav") == 0)
// 		{
// 			g_pChara->LoadSound( strFullName);
// 		}
// 	}

	I3_TDK_UPDATE_INFO Data;
	memset(&Data,0,sizeof(I3_TDK_UPDATE_INFO));
	Data.m_Event = I3_TDK_UPDATE_SELECT;

	::SendMessage(g_HwndWeaponResList, WM_TDK_UPDATE, (WPARAM) &Data, (LPARAM)NULL);

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

CString	cDlgResourceControl::GetFullPath(const CString& str)
{
	TCHAR szItem[256];
	HTREEITEM hItem = m_treeExplorer.GetSelectedItem();

	CString strPath;
	FileSubPath(hItem, strPath);

	TVITEM tvItem;
	tvItem.cchTextMax = 256;
	tvItem.pszText = szItem;
	tvItem.mask = TVIF_TEXT | TVIF_HANDLE;

	tvItem.hItem = hItem;
	m_treeExplorer.GetItem(&tvItem);

	return g_strRootPath +"\\"+ strPath +"\\"+ str;
}

void cDlgResourceControl::FileSubPath(HTREEITEM hItem, CString& strPath)
{
	TCHAR szItem[256];
	TVITEM tvItem;
	tvItem.cchTextMax = 256;
	tvItem.pszText = szItem;
	tvItem.mask = TVIF_TEXT | TVIF_HANDLE;
	tvItem.hItem = hItem;
	m_treeExplorer.GetItem(&tvItem);

	if (strPath.IsEmpty())
		strPath = CString(szItem);
	else
		strPath = CString(szItem) + "\\" + strPath;

	HTREEITEM hParentItem = m_treeExplorer.GetParentItem(hItem);
	if (hParentItem)
		FileSubPath(hParentItem, strPath);
}
void cDlgResourceControl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}

void cDlgResourceControl::OnTvnKeydownTreeExplorer(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (IDCANCEL == AfxMessageBox("폴더를 삭제하시면 하위 파일&폴더 까지 모두 삭제 되요\n삭제 할래요?",MB_OKCANCEL))
		return;

	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
	
//	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM htree = m_treeExplorer.GetSelectedItem();

	CString Tmp = m_mFolder[htree];
	int nPos = Tmp.Find("\\");
	Tmp = m_subDir + Tmp.Mid(nPos, Tmp.GetLength());
	switch(pTVKeyDown->wVKey)
	{
	case VK_DELETE:
		{
			m_treeExplorer.DeleteItem(htree);
			DeleteFile2(Tmp, true);
			//DeleteAllFolder(Tmp);
		}
		break;
	}

	*pResult = 0;
}

BOOL cDlgResourceControl::DeleteAllFolder( CString strPath )
{
	if(strPath.IsEmpty()){
		return FALSE ; 
	}

	BOOL bRval = FALSE;
//	int   nRval = 0;
	CString strNextDirPath = "";
	CString strRootPath = "";
	CFileFind fileFind;

	bRval = fileFind.FindFile(strPath);

	if(bRval == FALSE) return bRval;

	while (bRval)
	{
		bRval = fileFind.FindNextFile();

		if (fileFind.IsDots() == TRUE)
			continue;

		if (fileFind.IsDirectory())
		{
			strNextDirPath.Format("%s\\*.*", fileFind.GetFilePath());

			DeleteAllFolder(strNextDirPath);
		}
		else
		{
			//DeleteFile(fileFind.GetFilePath());
			DeleteFile2(fileFind.GetFilePath(), true);
		}
	}

	strRootPath = fileFind.GetRoot();

	fileFind.Close();

	//bRval = RemoveDirectory(strRootPath);
	DeleteFile2(strRootPath, true);

	return bRval;
}

BOOL cDlgResourceControl::DeleteFile2( const char * pszFileName, bool bUndo )
{
	if( bUndo )
	{
		SHFILEOPSTRUCT sttFile;
		char  szFileName[1024];
		int   iLen;

		iLen = _snprintf( szFileName, sizeof(szFileName), "%s", pszFileName );
		if( iLen >= sizeof(szFileName) ) 
			return FALSE;
		szFileName[iLen+1] = '\0';
		memset( &sttFile, 0, sizeof(sttFile) );

		sttFile.wFunc = FO_DELETE;
		sttFile.pFrom = szFileName;
		sttFile.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI;
		sttFile.fAnyOperationsAborted = false;

		if( SHFileOperation( &sttFile ) == 0 )
			return TRUE;

		return FALSE;
	}
    return DeleteFile( pszFileName );
}


void cDlgResourceControl::OnNMClickTreeExplorer(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}
