#include "stdafx.h"
#include "resource.h"
#include "ImageListPane.h"
#include "MainFrm.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_fileext.h"

IMPLEMENT_DYNAMIC(CImageListPane, CPaneBase)

BEGIN_MESSAGE_MAP(CImageListPane, CPaneBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_RESOURCELIST, &CImageListPane::OnNMDblclkTreeResourcelist)
END_MESSAGE_MAP()

CImageListPane::CImageListPane()
{
}


CImageListPane::~CImageListPane()
{
}

void CImageListPane::AdjustLayout()
{
	CRect rectClient;
	GetClientRect(rectClient);

	//백그라운드
	m_DlgBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

	CWnd* pWindow = m_DlgBar.GetDlgItem(IDC_TREE_RESOURCELIST);
	if(pWindow)
		pWindow->SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CImageListPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPaneBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_DlgBar.Create(this, IDD_IMAGELIST, WS_CHILD | WS_VISIBLE, IDD_IMAGELIST);

	m_ImageList.DeleteImageList();
	m_ImageList.Create(16, 16, ILC_COLOR32 | ILC_MASK, 2, 0);

	{
		HICON hIcon = ::LoadIcon(g_hInstTDK, MAKEINTRESOURCE(IDI_NODE));

		m_ImageList.Add(hIcon);

		::DestroyIcon(hIcon);

		hIcon = ::LoadIcon(g_hInstTDK, MAKEINTRESOURCE(IDI_RESOURCE));

		m_ImageList.Add(hIcon);

		::DestroyIcon(hIcon);
	}

	CTreeCtrl* pResourceTreeView = (CTreeCtrl*)m_DlgBar.GetDlgItem(IDC_TREE_RESOURCELIST);
	pResourceTreeView->SetImageList(&m_ImageList, TVSIL_NORMAL);

	AdjustLayout();

	return 0;
}

void CImageListPane::Init()
{
	LoadImage();
}

void CImageListPane::OnSize(UINT nType, int cx, int cy)
{
	CPaneBase::OnSize(nType, cx, cy);

	AdjustLayout();
}
void CImageListPane::LoadImage()
{
	const char* chWorkingDir = i3ResourceFile::GetWorkingFolder();

	TVINSERTSTRUCT tvi;
	tvi.hParent = TVI_ROOT;
	tvi.hInsertAfter = TVI_LAST;
	tvi.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	tvi.item.pszText = "Image";
	tvi.item.iImage = 0;
	tvi.item.iSelectedImage = 0;
	tvi.item.cchTextMax = 50;
	tvi.item.state = 0;
	tvi.item.cChildren = 0;

	CTreeCtrl* pResourceTreeView = (CTreeCtrl*)m_DlgBar.GetDlgItem(IDC_TREE_RESOURCELIST);
	HTREEITEM hItem = pResourceTreeView->InsertItem(&tvi);

	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
	BOOL bResult = TRUE;

	char chTempPath[MAX_PATH];
	sprintf_s(chTempPath, "%s\\Image", chWorkingDir);
	ChildbyLoadImage(chTempPath, tvi, hItem);
}

//모든 폴더검색하여 tga파일을 읽어들인다
void CImageListPane::ChildbyLoadImage(char* chDir, TVINSERTSTRUCT tvi, HTREEITEM hItem)
{
	HANDLE hSrch;
	WIN32_FIND_DATA wfd;
	BOOL bResult = TRUE;

	char chDirectoryPath[MAX_PATH];
	sprintf_s(chDirectoryPath, "%s\\*.*", chDir);
	hSrch = FindFirstFile(chDirectoryPath, &wfd);
	CTreeCtrl* pResourceTreeView = (CTreeCtrl*)m_DlgBar.GetDlgItem(IDC_TREE_RESOURCELIST);
	while (bResult)
	{
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(wfd.cFileName, ".") && strcmp(wfd.cFileName, ".."))
			{
				tvi.hParent = hItem;
				tvi.hInsertAfter = TVI_LAST;
				tvi.item.pszText = wfd.cFileName;
				tvi.item.iImage = 0;
				tvi.item.iSelectedImage = 0;
				HTREEITEM hChildItem = pResourceTreeView->InsertItem(&tvi);

				char _TempPath[MAX_PATH];
				sprintf_s(_TempPath, "%s\\%s", chDir, wfd.cFileName);
				ChildbyLoadImage(_TempPath, tvi, hChildItem);
			}
		}
		else
		{
			char chfileext[32];
			i3::extract_fileext(wfd.cFileName, chfileext);

			if (i3::generic_is_iequal(chfileext, "tga") || i3::generic_is_iequal(chfileext, "i3i"))
			{
				tvi.hParent = hItem;
				tvi.hInsertAfter = TVI_LAST;
				tvi.item.pszText = wfd.cFileName;
				tvi.item.iImage = 1;
				tvi.item.iSelectedImage = 1;
				pResourceTreeView->InsertItem(&tvi);
			}
		}
		bResult = FindNextFile(hSrch, &wfd);
	}
	FindClose(hSrch);
}

//Tree컨트롤에 이미지 리스트 전부 밀어넣기 
void CImageListPane::OnNMDblclkTreeResourcelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame * pMainWnd = (CMainFrame *)AfxGetMainWnd();

	CTreeCtrl* pResourceTreeView = (CTreeCtrl*)m_DlgBar.GetDlgItem(IDC_TREE_RESOURCELIST);
	HTREEITEM hitem = pResourceTreeView->GetSelectedItem(); // 현재 선택되어진 놈의 핸들을 가져온다.

	if (hitem != NULL && pResourceTreeView)
	{
		HTREEITEM hParentItem = NULL;

		i3::string sPath;
		HTREEITEM hSerchitem = hitem;
		i3::vector<i3::string> vecFolder;

		//부모 노드 찾기
		while (true)
		{
			hParentItem = pResourceTreeView->GetParentItem(hSerchitem);
			if (hParentItem)
			{
				i3::string str = pResourceTreeView->GetItemText(hParentItem);

				vecFolder.push_back(str);
				hSerchitem = hParentItem;
			}
			else
			{
				break;
			}
		}

		for (INT32 i = vecFolder.size() - 1; i >= 0; i--)
		{
			sPath += vecFolder[i];
			sPath += "\\";
		}
		i3::string sfildName = pResourceTreeView->GetItemText(hitem);
		sPath += sfildName;

		char chfileext[32];
		i3::extract_fileext(sfildName, chfileext);

		//이미지출력
		if (i3::generic_is_iequal(chfileext, "tga") || i3::generic_is_iequal(chfileext, "i3i"))
		{
			pMainWnd->GetTextureView()->DisplayTex(sPath);
		}
	}

	*pResult = 0;
}