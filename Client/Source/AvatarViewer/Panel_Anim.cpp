// Panel_Anim.cpp : implementation file
//

#include "pch.h"
#include "AvatarViewer.h"
#include "Panel_Anim.h"
#include "i3Base/itl/range/generic_empty.h"
#include "i3Base/string/ext/contain_string.h"

// CPanel_Anim
const static CString PATH_SWAT_FEMALE_1PV = "\\Chara\\SWAT_Female\\1PV";
const static CString PATH_SWAT_FEMALE_3PV = "\\Chara\\SWAT_Female\\3PV";
const static CString PATH_SWAT_MALE_1PV = "\\Chara\\SWAT_Male\\1PV";
const static CString PATH_SWAT_MALE_3PV = "\\Chara\\SWAT_Male\\3PV";
IMPLEMENT_DYNAMIC(CPanel_Anim, CDockablePane)

CPanel_Anim::CPanel_Anim()
{

}

CPanel_Anim::~CPanel_Anim()
{
}


BEGIN_MESSAGE_MAP(CPanel_Anim, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_PAINT()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ANIMATION, &CPanel_Anim::OnTvnSelchangedTreeAnimation)
	ON_NOTIFY(NM_CLICK, IDC_TREE_ANIMATION, &CPanel_Anim::OnNMClickTreeAnimation)
END_MESSAGE_MAP()

void CPanel_Anim::DoDataExchange(CDataExchange* pDX)
{
	CDockablePane::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ANIMATION, m_TreeCtrl);
}


int CPanel_Anim::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 뷰를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	m_TreeCtrl.Create(dwViewStyle,rectDummy, this, IDC_TREE_ANIMATION);


// 	HTREEITEM hClass = m_TreeCtrl.InsertItem(_T("CFakeAboutDlg"), 1, 1, hRoot);
// 	m_TreeCtrl.InsertItem(_T("CFakeAboutDlg()"), 3, 3, hClass);
// 
// 	m_TreeCtrl.Expand(hRoot, TVE_EXPAND);



//	HTREEITEM hRoot = m_TreeCtrl.InsertItem(_T("Male"), 0, 0);
//	m_TreeCtrl.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
	LoadAnimation();
	
	return 0;
}

void CPanel_Anim::LoadAnimation()
{
	m_TreeCtrl.DeleteAllItems();

	if (i3::generic_empty(i3ResourceFile::GetWorkingFolder()) )
		return;
	else
	{
		::BOOL bDir = SetCurrentDirectory(i3ResourceFile::GetWorkingFolder());
		if (FALSE == bDir)
		{
			i3ResourceFile::SetWorkingFolder("");
			return;
		}
	}


	CString strFemale_1PV	= i3ResourceFile::GetWorkingFolder() + PATH_SWAT_FEMALE_1PV;
	CString strFemale_3PV	= i3ResourceFile::GetWorkingFolder() + PATH_SWAT_FEMALE_3PV;
	CString strMale_1PV		= i3ResourceFile::GetWorkingFolder() + PATH_SWAT_MALE_1PV;
	CString strMale_3PV		= i3ResourceFile::GetWorkingFolder() + PATH_SWAT_MALE_3PV;

	HTREEITEM hRoot = m_TreeCtrl.InsertItem(_T("FeMale"), 0, 0);
	m_TreeCtrl.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	HTREEITEM hClass = m_TreeCtrl.InsertItem(_T("1PV"), 1, 1, hRoot);
	m_TreeCtrl.SetItemState(hClass, TVIS_BOLD, TVIS_BOLD);
	FindAnimationFiles(strFemale_1PV, "", hClass);

	hClass = m_TreeCtrl.InsertItem(_T("3PV"), 1, 1, hRoot);
	m_TreeCtrl.SetItemState(hClass, TVIS_BOLD, TVIS_BOLD);
	FindAnimationFiles(strFemale_3PV, "", hClass);

	hRoot = m_TreeCtrl.InsertItem(_T("Male"), 0, 0);
	m_TreeCtrl.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	hClass = m_TreeCtrl.InsertItem(_T("1PV"), 1, 1, hRoot);
	m_TreeCtrl.SetItemState(hClass, TVIS_BOLD, TVIS_BOLD);
	FindAnimationFiles(strMale_1PV, "", hClass);

	hClass = m_TreeCtrl.InsertItem(_T("3PV"), 1, 1, hRoot);
	m_TreeCtrl.SetItemState(hClass, TVIS_BOLD, TVIS_BOLD);
	FindAnimationFiles(strMale_3PV, "", hClass);
}

void CPanel_Anim::FindAnimationFiles(const CString& strRoot, const CString& strSub, HTREEITEM& hRoot)
{
	HANDLE				Handle		= INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA		findData;

	CString strPath = strRoot;

	char prevPath[ MAX_PATH ];
	::GetCurrentDirectory( MAX_PATH, prevPath );
	BOOL bDir = ::SetCurrentDirectory( strPath );
	if (FALSE == bDir)
	{
		::SetCurrentDirectory(prevPath);
		return;
	}


	
	Handle = ::FindFirstFile( "*.*", &findData);
	if (Handle == INVALID_HANDLE_VALUE)
		return;

	do
	{
		CString strFileName = findData.cFileName;

		if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if( strFileName == _T(".") || strFileName == _T("..") || strFileName == _T(".svn") )
			{
				continue;
			}

			HTREEITEM hSubRoot = m_TreeCtrl.InsertItem(strFileName, 0, 0, hRoot);
			FindAnimationFiles( strRoot+"\\"+strFileName, strFileName, hSubRoot );
		}
		else
		{
			if (hRoot != NULL)
			{
				if( i3::contain_string( strFileName.GetString(), "i3a") >= 0 ||
					i3::contain_string( strFileName.GetString(), "I3A") >= 0 )
				{
					CString strFullFilePath = strRoot + "\\" + strFileName;
					HTREEITEM hItem = NULL;
					hItem = m_TreeCtrl.InsertItem(strFileName, 1, 1, hRoot);
					m_mAnimNames[m_nCnt] = strFullFilePath;
					m_TreeCtrl.SetItemData(hItem, (DWORD_PTR)m_nCnt) ;
					m_nCnt++;
				}
			}
		}
	} while( TRUE == ::FindNextFile( Handle, &findData ) );

	::FindClose(Handle);
	::SetCurrentDirectory( prevPath );
}

void CPanel_Anim::OnPaint()
{
	CDockablePane::OnPaint();
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	CRect rectTree;
	m_TreeCtrl.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}
void CPanel_Anim::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	m_TreeCtrl.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CPanel_Anim::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_TreeCtrl;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 클릭한 항목을 선택합니다.
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CPanel_Anim::OnTvnSelchangedTreeAnimation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM item = m_TreeCtrl.GetSelectedItem();
	int nIdx = (int)m_TreeCtrl.GetItemData(item);

	CString strFullPath = m_mAnimNames[nIdx];

	if (!strFullPath.IsEmpty() && strFullPath.Find("i3a") >= 0)
		g_pFramework->LoadAnimation(NULL,strFullPath);
	*pResult = 0;
}

void CPanel_Anim::OnNMClickTreeAnimation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMITEM = (LPNMITEMACTIVATE) pNMHDR;  
	int m_nItem = pNMITEM->iItem;

	*pResult = 0;
}
