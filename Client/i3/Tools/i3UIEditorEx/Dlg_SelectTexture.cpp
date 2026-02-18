// Dlg_SelectTexture.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "Dlg_SelectTexture.h"
#include "Dlg_SubsetSpec.h"
#include "i3UIEditorDoc.h"
#include "DlgProgress.h"
#include "MainFrm.h"

#include "i3Base/string/ext/resolve_path_token.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/generic_string_cat.h"

#include "i3Framework/i3UI/i3VTFileFormat.h"
#include "i3Framework/i3UI/i3VTSubsetController.h"
#include "i3Framework/i3UI/i3UIFilePathMgr.h"

#include "i3Base/string/system/copy_files_with_wildcards.h"
#include "i3Base/string/system/collect_directoryname.h"
#include "i3Base/string/ext/contain_string_head.h"
#include "i3Base/string/system/delete_directory.h"
#include "i3Base/string/system/prepare_directory.h"

#define		SPLIT_REG_NAME			"TexSelSplitPos"
#define		SELTEX_X_REG_NAME		"TexSelX"
#define		SELTEX_Y_REG_NAME		"TexSelY"
#define		SELTEX_CX_REG_NAME		"TexSelCX"
#define		SELTEX_CY_REG_NAME		"TexSelCY"

// CDlg_SelectTexture dialog
static CProgressCtrl m_ProgressBar;

IMPLEMENT_DYNAMIC(cTreeTexture, CTreeCtrl)

BEGIN_MESSAGE_MAP(cTreeTexture, CTreeCtrl)
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

cTreeTexture::cTreeTexture()
{

}

cTreeTexture::~cTreeTexture()
{

}

void cTreeTexture::OnSetFocus(CWnd* pOldWnd)
{
	
	((CDlg_SelectTexture*)m_pParent)->OnManualSetFocus();
}

typedef HRESULT (FAR WINAPI *SHPROC)( int, REFIID, void **);

IMPLEMENT_DYNAMIC(CDlg_SelectTexture, CDialog)

/*
static INT32	_CompProc( I3VT_TEXINFO * pInfo1, I3VT_TEXINFO * pInfo2)
{
	return i3String::NCompare( pInfo1->m_szName, pInfo2->m_szName, MAX_PATH -1);
}

static INT32	_FindProc( I3VT_TEXINFO * pInfo, const char * pszName)
{
	return i3String::NCompare( pInfo->m_szName, pInfo->m_szName, MAX_PATH -1);
}
*/
CDlg_SelectTexture::CDlg_SelectTexture(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_SelectTexture::IDD, pParent)
{
	m_pVTex = NULL;

	m_bDragging = false;
	m_bInSplitPos = false;
	m_RightMargine = 0;
	m_pSelTex = NULL;

	m_bNeedToSaveTemplate_ByEditVTex = false;
	m_bNeedToSaveTemplate_ByVTexFormat = true;			// 일반적으로 VTexFormat이 구버전이면 UIL저장이 병행된다..
	m_szPath = "";
}

CDlg_SelectTexture::~CDlg_SelectTexture()
{
}

void CDlg_SelectTexture::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_BTN_Ok);
	DDX_Control(pDX, IDCANCEL, m_BTN_Cancel);
	DDX_Control(pDX, IDC_BTN_ADD, m_BTN_AddTexture);
	DDX_Control(pDX, IDC_TREE, m_TreeCtrl);
	DDX_Control(pDX, IDC_ED_SEARCH, m_ED_Search);
	DDX_Control(pDX, IDC_BTN_DELETE_TEXTURE, m_btnDelete);
	DDX_Control(pDX, IDC_BTN_SELECT_ADJUST_TEXTURE, m_btnAdjust);
	DDX_Control(pDX, IDC_BTN_SELECT_ADJUST_VTEXLIST, m_btnNewAdjust);
	DDX_Control(pDX, IDC_BTN_CHANGE_FORMAT, m_BTN_Compress);
	DDX_Control(pDX, IDC_ADJUST_PROGRESS, m_ProgressBar);
	DDX_Control(pDX, IDC_BTN_COPY_N_RELOAD_VTEXINFO, m_btnCopyAndReload);

}

bool CDlg_SelectTexture::Execute( i3VirtualTexture * pVTex)
{
	m_pVTex = pVTex;

	if( DoModal() == IDCANCEL)
	{
		return false;
	}

	return true;
}

void CDlg_SelectTexture::ChangeCursor( bool bSizing)
{
	if( m_bInSplitPos != bSizing)
	{
		if( bSizing)
		{
			::SetClassLong( m_hWnd, GCL_HCURSOR, (LONG) m_curSize);
			::SetCursor( m_curSize);
		}
		else
		{
			::SetClassLong( m_hWnd, GCL_HCURSOR, (LONG) m_curNormal);
			::SetCursor( m_curNormal);
		}

		m_bInSplitPos = bSizing;
	}
}

HTREEITEM CDlg_SelectTexture::FindTreeItemByName( HTREEITEM hParent, const char * pszName)
{
	HTREEITEM hChild = m_TreeCtrl.GetChildItem( hParent);

	while( hChild != NULL)
	{
		CString name = m_TreeCtrl.GetItemText( hChild);

		if( i3::generic_is_iequal( LPCTSTR( name), pszName) )
			return hChild;

		hChild = m_TreeCtrl.GetNextItem( hChild, TVGN_NEXT);
	}

	return NULL;
}

void CDlg_SelectTexture::ParsePath( const char * pszPath)
{
	HTREEITEM hParent = TVI_ROOT, hItem;
	i3::vector<i3::rc_string> tokenList;
	
	char	szFull[ MAX_PATH];

	i3::resolve_path_token( pszPath, tokenList);

	szFull[0] = 0;
	
	if (tokenList.empty())
		return;

	for(size_t i = 0; i < tokenList.size() - 1; i++)
	{
		const char * pszToken = tokenList[i].c_str();

		if( szFull[0] != 0)
			i3::generic_string_cat( szFull, "/");

		i3::generic_string_cat( szFull, pszToken);

		hItem = FindTreeItemByName( hParent, pszToken);

		if( hItem == NULL)
		{
			hItem = m_TreeCtrl.InsertItem( pszToken, m_idxFolderImage, m_idxSelFolderImage, hParent);

			// Subset Rule이 존재하는지 검사
			{
				i3VTFileFormat* pFF = m_pVTex->GetFileFormat();
				i3VTSubsetController* pSubsetController = pFF->GetSubsetController();
				I3ASSERT(pSubsetController);

				i3VTSubsetRule* pRule = pSubsetController->FindSubsetRule( szFull);

				if( pRule != NULL)
					m_TreeCtrl.SetItemData( hItem, (DWORD_PTR) pRule);
			}

			m_TreeCtrl.EnsureVisible( hItem);
		}

		hParent = hItem;
	}
}

//
// getRefTexture의 path --> 파일패스의 그것과 대충 일치..(등록된 놈 위주로 트리가 표현됨)
//

// 출력은 영문자 순서가 좋으므로 고친다..

void CDlg_SelectTexture::UpdateAllTextures(void)
{
	I3VT_TEXINFO * pInfo;

	i3::vector<I3VT_TEXINFO*> vecForSort;
	vecForSort.reserve(m_pVTex->getRefTextureCount());

	for (INT32 i = 0; i < m_pVTex->getRefTextureCount(); i++)
	{
		pInfo = m_pVTex->getRefTexture(i);
		vecForSort.push_back(pInfo);
	}

	struct comp
	{
		bool operator()(const I3VT_TEXINFO* lhs, const I3VT_TEXINFO* rhs) const
		{
			return i3::generic_is_iless(lhs->m_strName, rhs->m_strName);
		}
	};

	i3::stable_sort(vecForSort.begin(), vecForSort.end(), comp());
	const size_t num = vecForSort.size();

	for (size_t i = 0; i < num; ++i)
	{
		pInfo = vecForSort[i];
		ParsePath(pInfo->m_strName.c_str());
	}
	
}


BEGIN_MESSAGE_MAP(CDlg_SelectTexture, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_ADD, &CDlg_SelectTexture::OnBnClickedBtnAdd)
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &CDlg_SelectTexture::OnTvnSelchangedTree)
	ON_EN_CHANGE(IDC_ED_SEARCH, &CDlg_SelectTexture::OnEnChangeEdSearch)
//	ON_WM_SHOWWINDOW()
//	ON_WM_CLOSE()
ON_BN_CLICKED(IDC_BTN_DELETE_TEXTURE, &CDlg_SelectTexture::OnBnClickedBtnDeleteTexture)
ON_BN_CLICKED(IDC_BTN_SELECT_ADJUST_TEXTURE, &CDlg_SelectTexture::OnBnClickedBtnSelectAdjustTexture)
ON_BN_CLICKED(IDC_BTN_SELECT_ADJUST_VTEXLIST, &CDlg_SelectTexture::OnBnClickedBtnSelectAdjustVTexList)
ON_BN_CLICKED(IDC_BTN_COPY_N_RELOAD_VTEXINFO, &CDlg_SelectTexture::OnBnClickedBtnCopyAndReloadVTexInfo)
ON_NOTIFY(NM_RCLICK, IDC_TREE, &CDlg_SelectTexture::OnNMRClickTree)
ON_NOTIFY(NM_CUSTOMDRAW, IDC_TREE, &CDlg_SelectTexture::OnNMCustomdrawTree)
ON_WM_DESTROY()
ON_BN_CLICKED(IDC_BTN_CHANGE_FORMAT, &CDlg_SelectTexture::OnBnClickedBtnChangeFormat)
END_MESSAGE_MAP()


// CDlg_SelectTexture message handlers

void CDlg_SelectTexture::LoadProfiles(void)
{
	m_SplitPos = theApp.GetProfileInt( "Settings", SPLIT_REG_NAME, 128);

	INT32 x, y, cx, cy;

	x = theApp.GetProfileInt( "Settings", SELTEX_X_REG_NAME, -1);
	y = theApp.GetProfileInt( "Settings", SELTEX_Y_REG_NAME, -1);
	cx = theApp.GetProfileInt( "Settings", SELTEX_CX_REG_NAME, -1);
	cy = theApp.GetProfileInt( "Settings", SELTEX_CY_REG_NAME, -1);

	if( (cx > 0) && (cy > 0))
	{
		SetWindowPos( NULL, x, y, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CDlg_SelectTexture::SaveProfiles(void)
{
	theApp.WriteProfileInt( "Settings", SPLIT_REG_NAME, m_SplitPos);

	CRect rt;
	GetWindowRect( &rt);

	theApp.WriteProfileInt( "Settings", SELTEX_X_REG_NAME, rt.left);
	theApp.WriteProfileInt( "Settings", SELTEX_Y_REG_NAME, rt.top);
	theApp.WriteProfileInt( "Settings", SELTEX_CX_REG_NAME, rt.Width());
	theApp.WriteProfileInt( "Settings", SELTEX_CY_REG_NAME, rt.Height());
}



BOOL CDlg_SelectTexture::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		CRect rt;

		rt.SetRect( 0, 0, 100, 100);

		m_TexCtrl.Create( WS_CHILD | WS_VISIBLE, rt, this, 101);
		m_TexCtrl.SetVirtualTexture( m_pVTex);
	}

	{
		m_curNormal = ::LoadCursor( NULL, IDC_ARROW);
		m_curSize = ::LoadCursor( NULL, IDC_SIZEWE);
	}

	{
		LoadProfiles();
	}

	{
		//먼가 누수가남.. 그래서 엔진거 그대로 가져와서 만듬..
		HRESULT rv;
		SHPROC _SHGetImageList;
		HANDLE			hModule;
		IImageList *	pIImageList;

		hModule = LoadLibrary( "Shell32.dll");

		_SHGetImageList = (SHPROC) GetProcAddress( (HMODULE) hModule, (LPCTSTR) "SHGetImageList");
		I3ASSERT( _SHGetImageList != NULL);

		rv = _SHGetImageList( SHIL_SMALL, IID_IImageList, (void **) &pIImageList);
		if( FAILED( rv))
		{
			AfxMessageBox( "Could not get the System Image List interface object.");
			return NULL;
		}

		m_pImageList = new CImageList;
		m_pImageList->Attach( (HIMAGELIST) pIImageList);

		m_TreeCtrl.SetImageList( m_pImageList, TVSIL_NORMAL);

		{
			SHFILEINFO fi;

			SHGetFileInfo( i3ResourceFile::GetWorkingFolder(), FILE_ATTRIBUTE_DIRECTORY, &fi, sizeof(fi), SHGFI_SYSICONINDEX);
			m_idxFolderImage = fi.iIcon;

			SHGetFileInfo( i3ResourceFile::GetWorkingFolder(), FILE_ATTRIBUTE_DIRECTORY, &fi, sizeof(fi), SHGFI_SYSICONINDEX | SHGFI_SELECTED);
			m_idxSelFolderImage = fi.iIcon;
		}
	}

	PlaceControls();

	{
		UpdateAllTextures();

		SearchRefTextures();
	}
	
	{
		if( m_pVTex->getFormat() == I3G_IMAGE_FORMAT_DXT5)
		{
			m_BTN_Compress.SetWindowText( "Uncompress");
		}
		else
		{
			m_BTN_Compress.SetWindowText( "Compress");
		}
	}

	m_TreeCtrl.SetEventWnd(this);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_SelectTexture::PlaceControls( INT32 cx, INT32 cy)
{
	if( cx < 0)
	{
		CRect rt;

		GetClientRect( &rt);

		cx = rt.Width();
		cy = rt.Height();
	}

	const INT32		BTN_CX	= 80;
	const INT32		BTN_CY	= 24;

	INT32 texCtrl_Width = cx - BTN_CX - 9 - m_SplitPos - 4;

	m_TreeCtrl.SetWindowPos( NULL, 3, 3, m_SplitPos - 2 - 3, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	m_ED_Search.SetWindowPos( NULL, m_SplitPos + 2, 3, texCtrl_Width, 20, SWP_NOACTIVATE | SWP_NOZORDER);

	m_TexCtrl.SetWindowPos( NULL, m_SplitPos + 2, 25 - 20, texCtrl_Width, cy - 25 - 3, SWP_NOACTIVATE | SWP_NOZORDER);

	m_ProgressBar.SetWindowPos(NULL, 3, cy-20, cx, cy - 25 - 3, SWP_NOACTIVATE | SWP_NOZORDER);

	m_RightMargine = ((m_SplitPos + 2) + texCtrl_Width) - 128;

	m_BTN_Ok.SetWindowPos(	NULL,		cx - BTN_CX - 3, 3,						BTN_CX, BTN_CY, SWP_NOACTIVATE | SWP_NOZORDER);
	m_BTN_Cancel.SetWindowPos(	NULL,	cx - BTN_CX - 3, 3 + BTN_CY + 3,		BTN_CX, BTN_CY, SWP_NOACTIVATE | SWP_NOZORDER);
	m_BTN_AddTexture.SetWindowPos( NULL,cx - BTN_CX - 3, 3 + (BTN_CY << 1) + 3,	BTN_CX, BTN_CY, SWP_NOACTIVATE | SWP_NOZORDER);
	m_btnDelete.SetWindowPos( NULL,		cx - BTN_CX - 3, 3 + (BTN_CY * 3) + 3,	BTN_CX, BTN_CY, SWP_NOACTIVATE | SWP_NOZORDER);
	m_btnAdjust.SetWindowPos( NULL,		cx - BTN_CX - 3, 3 + (BTN_CY * 4) + 3,	BTN_CX, BTN_CY, SWP_NOACTIVATE | SWP_NOZORDER);
	m_btnNewAdjust.SetWindowPos(NULL, cx - BTN_CX - 3, 3 + (BTN_CY * 5) + 3, BTN_CX, BTN_CY, SWP_NOACTIVATE | SWP_NOZORDER);
	m_BTN_Compress.SetWindowPos( NULL,	cx - BTN_CX - 3, 3 + (BTN_CY * 6) + 3,	BTN_CX, BTN_CY, SWP_NOACTIVATE | SWP_NOZORDER);
	m_btnCopyAndReload.SetWindowPos(NULL, cx - BTN_CX - 3, 3 + (BTN_CY * 7) + 3, BTN_CX, BTN_CY, SWP_NOACTIVATE | SWP_NOZORDER);

}

void CDlg_SelectTexture::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_TexCtrl.m_hWnd))
	{
		PlaceControls( cx, cy);
	}
}

void CDlg_SelectTexture::OnCancel()
{
	SaveProfiles();	

	CDialog::OnCancel();
}


void CDlg_SelectTexture::OnOK()
{
	m_pSelTex = m_TexCtrl.getSelectedRefTexture();

	SaveProfiles();	

	CDialog::OnOK();
}

I3VT_TEXINFO *	CDlg_SelectTexture::getSelectedRefTexture(void)
{
	return m_pSelTex;
}

void CDlg_SelectTexture::OnManualSetFocus()
{
	this->SetFocus();
}

void CDlg_SelectTexture::OnBnClickedBtnAdd()
{
	TCHAR szFilters[] = "TGA파일(*.tga)|*.tga|i3i 파일(*.i3i)|*.i3i|비트맵파일(*.bmp)|*.bmp|PNG 파일(*.png)|*.png|";

	CFileDialog fileDlg(TRUE, _T("tga"), _T("*.tga"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if( fileDlg.DoModal() != IDOK)
		return;

	// check already inserted texture
	i3Framework * pFramework = i3Framework::getCurrentFramework();
	i3ResourceManager * pResManager = pFramework->GetResourceManager();

#if defined( I3_ENGINE3)
	i3ResourceObject* pResObj = pResManager->LoadResource( LPCTSTR( fileDlg.GetPathName()));
	
	i3Texture* pTexture = NULL;
	if( i3::kind_of<i3Texture*>(pResObj)) //->IsTypeOf( i3Texture::static_meta()))
	{
		pTexture = (i3Texture*)pResObj;
	}
#else
	i3ResourceObject * pResObj = pResManager->LoadResource( (LPCTSTR)fileDlg.GetPathName(), 0);		//2.0

	i3Texture * pTexture = NULL;
	
	if( i3::kind_of<i3GameResTexture*>(pResObj)) //->IsTypeOf( i3GameResTexture::static_meta()))
	{
		pTexture = ((i3GameResTexture*)pResObj)->getTexture();
	}
	
	//int nCnt = pTexture->GetRefCount();

	//i3Texture * pTexture = (i3GameResTexture*)pResManager->LoadResource( (LPCTSTR)fileDlg.GetPathName(), 0);		//2.0
#endif
	
	if( pTexture == NULL)
	{
		I3PRINTLOG(I3LOG_WARN,"failed to load file");
		return ;
	}

	char conv[ MAX_PATH];
	i3::make_relative_path( i3ResourceFile::GetWorkingFolder(), pTexture->getResourceLoadPath(), conv );


	if( m_pVTex->FindRefTexture( conv ) == NULL)
	{

		INT32 x, y;

		i3VTFileFormat* pFF = m_pVTex->GetFileFormat();

		if(pFF->PlaceTexture( pTexture, &x, &y))
		{
			pFF->Flush();
			UpdateAllTextures();
			SearchRefTextures();

			Invalidate();
		}
		else
		{
			I3TRACE("error\n");
		}
	}
	else
	{
		::AfxMessageBox( "이미 등록된 Texture입니다.");
	}

	//일단 여기서 누수가 나므로 한개 줄여준다..
	pTexture->Release();
}

BOOL CDlg_SelectTexture::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	m_TexCtrl.OnMouseWheel( nFlags, zDelta, pt);

	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void CDlg_SelectTexture::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_bInSplitPos)
	{
		m_bDragging = true;

		SetCapture();
	}
}

void CDlg_SelectTexture::OnLButtonUp(UINT nFlags, CPoint point)
{
	if( m_bDragging)
	{
		ReleaseCapture();

		m_bDragging = false;
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CDlg_SelectTexture::OnMouseMove(UINT nFlags, CPoint point)
{
	if( m_bDragging)
	{
		m_SplitPos = point.x;

		if( m_SplitPos < 32)
			m_SplitPos = 32;
		else if( m_SplitPos > m_RightMargine)
		{
			m_SplitPos = m_RightMargine;
		}

		PlaceControls();
	}
	else
	{
		INT32 gap = abs( point.x - m_SplitPos);

		ChangeCursor( gap < 3);
	}

	CDialog::OnMouseMove(nFlags, point);
}




void CDlg_SelectTexture::SearchRefTextures( const char * pszPath, bool bOnlyName)
{
	INT32 i;

	i3::vector_multiset<I3VT_TEXINFO*, I3VT_TEXINFO_Comp> refList;			// 확실하지 않아서 멀티셋처리..
	refList.clear();

//	refList.SetCompareProc( (COMPAREPROC)_CompProc);

	char szName[ MAX_PATH], szPath[MAX_PATH];

	// 비교를 위해 패스 스타일과 case를 맞춘다.
	if( pszPath != NULL)
	{
		i3::string_ncopy_nullpad( szPath, pszPath, MAX_PATH);
		i3::make_unix_path( szPath);
		i3::to_upper( szPath);
		m_szPath = pszPath;
	}

	for( i = 0; i < m_pVTex->getRefTextureCount(); i++)
	{
		I3VT_TEXINFO * pInfo = m_pVTex->getRefTexture( i);

		if( pszPath != NULL && pszPath[0] != 0)
		{
			i3::safe_string_copy( szName, pInfo->m_strName, MAX_PATH );
			i3::to_upper( szName);
			i3::make_unix_path( szName);

			if( bOnlyName == false)
			{
				if( i3::contain_string( szName, szPath) != -1)
				{
					refList.insert( pInfo);
				}
			}
			else
			{
				if( i3::contain_string( szName, szPath) != -1)
				{
					refList.insert( pInfo);
				}
			}
		}
		else
		{
			refList.insert( pInfo);
		}

		
	}
	m_TexCtrl.SetRefTextureList( refList);
}

void CDlg_SelectTexture::GetFullPath( HTREEITEM hItem, char * pszStr, INT32 len)
{
	CString str = "";

	while( hItem != NULL)
	{
		CString name = m_TreeCtrl.GetItemText( hItem);

		str = name + "/" + str;

		hItem = m_TreeCtrl.GetParentItem( hItem);
	}

	i3::safe_string_copy( pszStr, LPCTSTR( str), len );
}

void CDlg_SelectTexture::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pInfo = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	if( pInfo->itemNew.hItem != NULL)
	{
		char conv[ MAX_PATH];

		GetFullPath( pInfo->itemNew.hItem, conv, sizeof(conv));

		SearchRefTextures( conv);
	}
	
	*pResult = 0;
}


void CDlg_SelectTexture::OnEnChangeEdSearch()
{
	char conv[512];

	m_ED_Search.GetWindowText( conv, sizeof(conv));

	if( conv[0] == NULL)
	{
		SearchRefTextures( NULL);
	}
	else
	{
		SearchRefTextures( conv, true);
	}
}


void CDlg_SelectTexture::OnBnClickedBtnDeleteTexture()
{
	// TODO: Add your control notification handler code here
	INT32 i;

	if( m_pVTex == NULL)
		return;

	i3Framework * pFramework = i3Framework::getCurrentFramework();
	i3ResourceManager * pResManager = pFramework->GetResourceManager();

	m_pSelTex = m_TexCtrl.getSelectedRefTexture();

	if( m_pSelTex != NULL)
	{
		for( i = 0; i < m_pVTex->getRefTextureCount(); i++)
		{
			I3VT_TEXINFO * pInfo = m_pVTex->getRefTexture( i);

			if( pInfo == m_pSelTex)
			{
				i3Texture * pTex = pResManager->LoadTexture( pInfo->m_strName.c_str());
				if( pTex != NULL)
				{
					m_pVTex->RemoveRefTexture( pTex);
					m_pSelTex = NULL;

					m_pVTex->GetFileFormat()->Flush();
//					m_pVTex->Flush();

					UpdateAllTextures();
					SearchRefTextures(m_szPath.c_str());
					Invalidate();

					pTex->Release();
				}
				break;
			}
		}
	}

	m_bNeedToSaveTemplate_ByEditVTex = true;
}

void CALLBACK CDlg_SelectTexture::ProgressCallBack(int nRange, int nPos)
{
	if (nRange != -1)
	{
		assert(nRange < 65536);

		m_ProgressBar.SetRange(0, (short)nRange);
	}
	if (nPos != -1)
		m_ProgressBar.SetPos(nPos);
}

void CDlg_SelectTexture::OnBnClickedBtnSelectAdjustTexture()
{
//	i3UIManager * pUI = i3UI::getManager();

// 	cDlgProgress* dlgProgress = new cDlgProgress();
// 	dlgProgress->Create(IDD_DLG_PROGRESS,NULL);
// 	dlgProgress->ShowWindow(SW_SHOW);
// 	dlgProgress->InitProgress(pUI->GetMaxCnt());
	
	// TODO: Add your control notification handler code here
	
	int nRes = AfxMessageBox("Adjust VirtualTexture ?", MB_YESNO);
	i3UI::SetCallbackFunction(ProgressCallBack);
	if (nRes == IDYES)
	{
		if (i3UI::getVirtualTexture() != NULL)
			i3UI::getVirtualTexture()->DeleteWrongTexture();
		i3UI::AdjustVirtualTexture(false);

		UpdateAllTextures();
		SearchRefTextures(m_szPath.c_str());

		m_TexCtrl.ModifyThumb(&m_ProgressBar);

		m_bNeedToSaveTemplate_ByEditVTex = true;
		m_bNeedToSaveTemplate_ByVTexFormat = true;		// 강제 저장가능해야한다.

		Invalidate();

		CMainFrame* pMainWnd = (CMainFrame *)AfxGetMainWnd();
		if (pMainWnd)
			pMainWnd->UpdateVirtualTextureFiles();

		//I3_SAFE_DELETE(dlgProgress);

		theApp.SwitchView(EVIEW_TEMPLATE);
		theApp.SwitchView(EVIEW_SCREEN);

		::AfxMessageBox("Adjust Virtual Texture OK!!!");
	}
}

void CDlg_SelectTexture::OnBnClickedBtnSelectAdjustVTexList()
{
	bool wasNewVTexFormat = i3UIFilePathMgr::i()->Isi3VTexListFormat();

	int nRes = AfxMessageBox("Adjust VTexList?", MB_YESNO);
	i3UI::SetCallbackFunction(ProgressCallBack);
	if (nRes == IDYES)
	{
		if (i3UI::getVirtualTexture() != NULL)
			i3UI::getVirtualTexture()->DeleteWrongTexture();
		i3UI::AdjustVirtualTexture(true);

		m_TexCtrl.ModifyThumb(&m_ProgressBar);

		UpdateAllTextures();
		SearchRefTextures(m_szPath.c_str());
	
		m_bNeedToSaveTemplate_ByEditVTex = true;
		m_bNeedToSaveTemplate_ByVTexFormat = !wasNewVTexFormat;	// 버추얼텍스쳐의 컨버팅의 경우에만, 템플릿도 강제 저장하도록 한다..
		
		Invalidate();

		CMainFrame* pMainWnd = (CMainFrame *)AfxGetMainWnd();
		if (pMainWnd)
			pMainWnd->UpdateVirtualTextureFiles();

		//I3_SAFE_DELETE(dlgProgress);

		theApp.SwitchView(EVIEW_TEMPLATE);
		theApp.SwitchView(EVIEW_SCREEN);

		::AfxMessageBox("Adjust VTexList OK!!!");
	}

}

void CDlg_SelectTexture::OnBnClickedBtnCopyAndReloadVTexInfo()
{
	if (i3UIFilePathMgr::i()->IsLanguageLocaleEnabled() == false)
	{
		::AfxMessageBox("Locale Dev doesn't need copy job....");
		return;
	}
	
	const i3::literal_range rngPrefix = "Copy i3UIL, i3VTex, i3VTexList, i3Subset, i3RefTex files.\n\n from ";

	bool dest_is_VTexListFormat = i3UIFilePathMgr::i()->Isi3VTexListFormat();

	i3::rc_string strSrcFolder = (dest_is_VTexListFormat) ? i3UIFilePathMgr::i()->Retrieve_CommonUI_VTexDirectory_i3VTexListFormat(false)
		                                        : i3UIFilePathMgr::i()->Retrieve_CommonUI_VTexDirectory_i3VTexFormat(false);

	i3::rc_string strDestFolder = i3UIFilePathMgr::i()->Retrieve_Current_VTexDirectory(false);

	i3::stack_string strMsg = rngPrefix;
	strMsg += strSrcFolder;
	strMsg += "\n\n to ";
	strMsg += strDestFolder;
	
	int nRes = AfxMessageBox( strMsg.c_str(), MB_YESNO);

	if (nRes != IDYES)
		return;

	CDialog::OnOK();				// 걍 닫아버림.. 닫기전에...복사처리는 있는대로 다 해둘것...i3UIL, i3VTex, i3VTexList, i3Subset, i3RefTex ...
									// i3VTexSub와 i3VTexImage파일은 복사하지 않는다..--> Adjust대상임..
									// 복사가 끝났다면, ReOpen과 같은 상태로 재로딩해두어야 한다..
	//
	i3UIManager* pUI = i3UI::getManager();
	if (pUI == NULL)
		return;

	i3UILibrary* pLib = pUI->getUILibrary();
	i3::rc_string strOldLibFilePath = pLib->GetNameString();			// 참조를 붙이면 안됨..

	pUI->CloseTemplateLibrary();
	pUI->CloseVirtualTexture();

	i3::rc_string strAbsSrcFolder = (dest_is_VTexListFormat) ? i3UIFilePathMgr::i()->Retrieve_CommonUI_VTexDirectory_i3VTexListFormat(true)
													 : i3UIFilePathMgr::i()->Retrieve_CommonUI_VTexDirectory_i3VTexFormat(true);

	i3::rc_string strAbsDestFolder = i3UIFilePathMgr::i()->Retrieve_Current_VTexDirectory(true);

	i3::vector<i3::literal_range> ext_list;
	ext_list.push_back("*.i3Uil");
	ext_list.push_back("*.i3VTex");
	ext_list.push_back("*.i3RefTex");
	ext_list.push_back("*.i3Subset");
	ext_list.push_back("*.i3VTexList");
	ext_list.push_back("*.lua");
	ext_list.push_back("*.luab");
	ext_list.push_back("*.i3UILibHdr");		// 신규 템플릿 헤더 포맷..
	i3::copy_files_with_wildcard(strAbsSrcFolder, ext_list, strAbsDestFolder);
	
	// 
	i3::vector<i3::rc_string> vecSubDirForTemplate;

	{
		i3::vector<i3::rc_string> vecSubDir;
		i3::collect_directoryname(strAbsSrcFolder, vecSubDir);
		i3::literal_range rngTemplateHead = "Template_";

		const size_t numSubDir_SrcFolder = vecSubDir.size();

		for (size_t i = 0; i < numSubDir_SrcFolder; ++i)
		{
			if (i3::icontain_string_head(vecSubDir[i], rngTemplateHead))
			{
				vecSubDirForTemplate.push_back(vecSubDir[i]);
			}
		}
	}

	if (!vecSubDirForTemplate.empty())
	{
		const size_t numSubDir_SrcFolder = vecSubDirForTemplate.size();

		i3::stack_string fullDirName_Src;
		i3::stack_string fullDirName_Dst;
		i3::vector<i3::literal_range> dummy_list;
		dummy_list.push_back("*.*");

		for (size_t i = 0; i < numSubDir_SrcFolder; ++i)
		{
			fullDirName_Src = strAbsSrcFolder;
			fullDirName_Src += '/';
			fullDirName_Src += vecSubDirForTemplate[i];

			fullDirName_Dst = strAbsDestFolder;
			fullDirName_Dst += '/';
			fullDirName_Dst += vecSubDirForTemplate[i];

			i3::delete_directory(fullDirName_Dst);
			i3::prepare_directory(fullDirName_Dst);
			
			i3::copy_files_with_wildcard(fullDirName_Src, dummy_list, fullDirName_Dst, true);

		}
	}
	
	pUI->LoadTemplateLibrary(strOldLibFilePath.c_str());  // "UIRe\\PointBlankRe.i3Uil");			// 이거 문제있음......
	
	//
}									
									


void CDlg_SelectTexture::OnBnClickedBtnChangeFormat()
{
	if( m_pVTex->getFormat() != I3G_IMAGE_FORMAT_DXT5)
	{
		// Compress
		m_pVTex->ChangeFormat( I3G_IMAGE_FORMAT_DXT5);

	}
	else
	{
		// Uncompress
		m_pVTex->ChangeFormat( I3G_IMAGE_FORMAT_BGRA_8888);
	}

	if( m_pVTex->getFormat() == I3G_IMAGE_FORMAT_DXT5)
	{
		m_BTN_Compress.SetWindowText( "Uncompress");
	}
	else
	{
		m_BTN_Compress.SetWindowText( "Compress");
	}
}


void CDlg_SelectTexture::OnNMRClickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu Menu, * pPopup;
	POINT pt;
	UINT32 cmd;
	HTREEITEM hSel = NULL;
//	List MetaList;		//쓰이지 않아 주석..
	GetCursorPos( &pt);

	// Select
	{
		POINT cpt;
		UINT flag;

		cpt = pt;

		m_TreeCtrl.ScreenToClient( &cpt);

		hSel = m_TreeCtrl.HitTest( cpt, &flag);

		if ((hSel != NULL) && (TVHT_ONITEM & flag))
		{
			m_TreeCtrl.Select( hSel, TVGN_CARET);
		}
	}

	{
		Menu.LoadMenu( IDR_POPUP_SUBSET);
	}

	pPopup = Menu.GetSubMenu( 0);

	if( hSel == NULL)
	{
		pPopup->EnableMenuItem( ID_SUBSET_NEWSUBSET, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_SUBSET_DELETESUBSETS, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		pPopup->EnableMenuItem( ID_SUBSET_PROPERTY, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
	}
	else
	{
		if( m_TreeCtrl.GetItemData( hSel) != NULL)
		{
			// Rule이 있다.
			pPopup->EnableMenuItem( ID_SUBSET_NEWSUBSET, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		}
		else
		{
			pPopup->EnableMenuItem( ID_SUBSET_DELETESUBSETS, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
			pPopup->EnableMenuItem( ID_SUBSET_PROPERTY, MF_DISABLED | MF_BYCOMMAND | MF_GRAYED);
		}
	}

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, NULL);

	switch( cmd)
	{
		case ID_SUBSET_NEWSUBSET :		NewRule( hSel);	break;
		case ID_SUBSET_DELETESUBSETS :	DeleteRule( hSel);	break;
		case ID_SUBSET_PROPERTY :		PopupSpec( hSel); break;
		default :
			break;
	}

	*pResult = 0;
}

void CDlg_SelectTexture::OnNMCustomdrawTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVCUSTOMDRAW pNMCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);
	
	HDC dc = pNMCD->nmcd.hdc;
	RECT * pRect = & pNMCD->nmcd.rc;

	switch( pNMCD->nmcd.dwDrawStage)
	{
		//case CDDS_POSTERASE :
		//case CDDS_POSTPAINT :
		//case CDDS_PREERASE :
		case CDDS_PREPAINT :		*pResult = CDRF_NOTIFYITEMDRAW;	return;

		//case CDDS_ITEM :
		//case CDDS_ITEMPOSTERASE :
		//case CDDS_ITEMPOSTPAINT :
		//case CDDS_ITEMPREERASE :
		case CDDS_ITEMPREPAINT :
			{
				HTREEITEM hItem = (HTREEITEM) pNMCD->nmcd.dwItemSpec;

				i3VTSubsetRule * pRule = (i3VTSubsetRule *) pNMCD->nmcd.lItemlParam;

				if( pRule != NULL)
				{
					CRect rc;
					char conv[256];

					m_TreeCtrl.GetItemRect( hItem, &rc, TRUE);
					rc.left = rc.right;
					rc.right = pRect->right;
					rc.top += 1;
					rc.bottom += 1;

					SetTextColor( dc, RGB( 0, 0, 255));

					i3::snprintf( conv, _countof( conv), "[%s]", pRule->m_strName);

					DrawText( dc, conv, strlen( conv), &rc, DT_VCENTER);

					SetTextColor( dc, pNMCD->clrText);
				}

				*pResult = CDRF_DODEFAULT;
			}
			return;
		//case CDDS_SUBITEM :
	}

	*pResult = 0;
}

void CDlg_SelectTexture::GetFullPath( HTREEITEM hItem, char * pszDest)
{
	i3::vector<HTREEITEM> list;
	CString name;

	while( hItem != NULL)
	{
		list.push_back( hItem);

		hItem = m_TreeCtrl.GetParentItem( hItem);
	}

	*pszDest = 0;
	

	for(size_t i = list.size() ; i > 0; i--)
	{
		hItem = list[i - 1];

		name = m_TreeCtrl.GetItemText( hItem);

		i3::generic_string_cat( pszDest, LPCTSTR( name));

		if( i > 0)
			i3::generic_string_cat( pszDest, "/");
	}
}

void CDlg_SelectTexture::NewRule( HTREEITEM hItem)
{
	i3VTSubsetRule rule;

	CDlg_SubsetSpec dlg;

	{
		char rulePath[MAX_PATH];
		GetFullPath(hItem, rulePath);
		rule.m_strPath = rulePath;
	}

	{
		CString name = m_TreeCtrl.GetItemText( hItem);
		rule.m_strName = LPCTSTR(name);
	}

	{
		rule.m_LimitSize = 10 * 1024 * 1024;			// 10메가가 매직넘버로 박혀있었음..그냥 방치..(신규포맷에선 쓰이지 않음)
	}
	
	if( dlg.Execute( &rule))
	{
		i3VTFileFormat* pFF = m_pVTex->GetFileFormat();
		i3VTSubsetController* pSubsetController = pFF->GetSubsetController();
		I3ASSERT(pSubsetController);

		i3VTSubsetRule * pRule = pSubsetController->AddSubsetRule( rule.m_strPath.c_str(), rule.m_strName.c_str(), rule.m_LimitSize);

		m_TreeCtrl.SetItemData( hItem, (DWORD_PTR) pRule);
		m_TreeCtrl.Invalidate();
	}
}

void CDlg_SelectTexture::DeleteRule( HTREEITEM hItem)
{
	i3VTSubsetRule * pRule;

	pRule = (i3VTSubsetRule *) m_TreeCtrl.GetItemData( hItem);

	if( pRule != NULL)
	{
		i3VTFileFormat* pFF = m_pVTex->GetFileFormat();
		i3VTSubsetController* pSubsetController = pFF->GetSubsetController();
		I3ASSERT(pSubsetController);

		pSubsetController->RemoveSubsetRule( pRule);

		m_TreeCtrl.SetItemData( hItem, (DWORD_PTR) NULL);

		m_TreeCtrl.Invalidate();
	}
}

void CDlg_SelectTexture::PopupSpec( HTREEITEM hItem)
{
	CDlg_SubsetSpec dlg;
	i3VTSubsetRule * pRule;

	pRule = (i3VTSubsetRule *) m_TreeCtrl.GetItemData( hItem);

	if( pRule != NULL)
	{
		dlg.Execute( pRule);
	}
	else
	{
		AfxMessageBox( "There is no rule in the folder");
	}
}

void CDlg_SelectTexture::OnDestroy()
{
	if(m_bNeedToSaveTemplate_ByEditVTex == true)
	{
	
		CFrameWnd * pMainWnd = (CFrameWnd *) AfxGetMainWnd();
		Ci3UIEditorDoc * pDoc = ( Ci3UIEditorDoc*) pMainWnd->GetActiveDocument();


		bool isNewVTexFormat = i3UIFilePathMgr::i()->Isi3VTexListFormat();

		if (isNewVTexFormat == false)
		{
			AfxMessageBox("Virtual Texture의 변경으로 인해, 구버전 템플릿은 반드시 저장해야 합니다. 저장합니다");
			pDoc->SaveTemplateLibrary(false);			// 강제로 구버전으로 돌린다..
		}
		else
		{
			bool isNewTemplateFormat = false;
			i3UILibrary* pUILib = i3UI::getUILibrary();
			if (pUILib)
				isNewTemplateFormat = pUILib->IsNewTemplateLibraryFormat();
			
			if (m_bNeedToSaveTemplate_ByVTexFormat == true)
			{
				AfxMessageBox("VirtualTexture -> VTexList로 컨버팅됨으로 인해, UILibHdr파일과 Template 폴더가 저장됩니다. 저장합니다");
				pDoc->SaveTemplateLibrary(true);			// 강제로 신버전으로 변경...
			}
			else
			{
				AfxMessageBox("VTexList 포맷안 에서는 템플릿파일 저장과정이 없습니다.");
			}
		}

//		i3UIManager * pUI = i3UI::getManager();
//		i3::vector<I3UI_SCENE_INFO*> SceneList= pUI->getSceneInfoList();
//		char Scenename[256];
//		strcpy(Scenename, SceneList[0]->m_strSceneFile.c_str());
	
		// 재 오픈 함수가 이제는 필요없을듯 하여 여기서 주석처리함..
		m_pVTex->GetFileFormat()->ReOpen();		// Open(m_pVTex->GetName()); // (char*)m_pVTex->GetPreFullPath().c_str());

		//리소스 다시로드하려고 ``;
		
		theApp.SwitchView( EVIEW_TEMPLATE);
		theApp.SwitchView( EVIEW_SCREEN);
		// 위 기존 Switch 코드는 지우지 마세요... AdjustVirtualTexture시에도 호출되도록 조정..

	}
	if (m_pImageList)
	{
		delete m_pImageList;
		m_pImageList = NULL;
	}
	CDialog::OnDestroy();
}
