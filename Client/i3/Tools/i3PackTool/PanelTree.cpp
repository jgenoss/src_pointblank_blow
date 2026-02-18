// PanelTree.cpp : implementation file
//

#include "stdafx.h"
#include "i3PackTool.h"
#include "PanelTree.h"
#include "GlobalVar.h"
#include ".\paneltree.h"

#include "i3Base/string/ext/resolve_path_token.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/contain_string.h"

// CPanelTree dialog

IMPLEMENT_DYNAMIC(CPanelTree, CDialog)
CPanelTree::CPanelTree(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelTree::IDD, pParent)
{
}

CPanelTree::~CPanelTree()
{
}

void CPanelTree::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PACK_TREE, m_TreeCtrl);
}

void CPanelTree::_Rec_SetTree( i3PackNode * pNode, HTREEITEM hParent)
{
	HTREEITEM hItem;
	char * pszName;
	TVINSERTSTRUCT item;

	if( pNode->hasName() == false)
		pszName = "<Nonamed>";
	else
		pszName = (char *) pNode->GetName();

	item.hParent = hParent;
	item.hInsertAfter = TVI_LAST;
	item.item.cChildren = pNode->getChildCount();
	item.item.cchTextMax = 0;
	item.item.hItem = 0;
	item.item.iImage = m_idxIconNormal;
	item.item.iSelectedImage = m_idxIconSelect;
	item.item.lParam = (LPARAM) pNode;
	item.item.pszText = pszName;
	item.item.state = 0;
	item.item.stateMask = 0;
	item.item.mask = TVIF_CHILDREN | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE | TVIF_TEXT;

	hItem = m_TreeCtrl.InsertItem( &item);

	// Parent Item의 cChildren 항목 수정
	{
		TVITEM info;

		memset( &info, 0, sizeof(info));
		info.mask = TVIF_CHILDREN | TVIF_HANDLE;
		info.cChildren = 1;
		info.hItem = hParent;

		m_TreeCtrl.SetItem( &info);
	}

	// Recursive
	INT32 i;
	i3PackNode * pChild;

	for( i = 0; i < pNode->getChildCount(); i++)
	{
		pChild = (i3PackNode *) pNode->getChild( i);

		_Rec_SetTree( pChild, hItem);
	}
}

void CPanelTree::UpdateAll(void)
{
	if (m_TreeCtrl)
		m_TreeCtrl.DeleteAllItems();

	if( g_pPack != NULL)
	{
		_Rec_SetTree( g_pPack);
	}
}

bool CPanelTree::IsRelativePath( const char * pszPath)
{
	i3::vector<i3::rc_string> base;
	i3::vector<i3::rc_string> path;

	i3::resolve_path_token( g_pOption->m_szWorkPath, base);
	i3::resolve_path_token( pszPath, path);

	for( size_t i = 0; i < base.size(); i++)
	{
	//	if( i3String::Compare( base.Items[i], path.Items[i]) != 0)
		if ( base[i] != path[i])
			return false;
	}

	return true;
}

i3PackNode * CPanelTree::PreparePath( const char * pszPath, bool bIsFile)
{
	char szPath[ MAX_PATH];

	if( IsRelativePath( pszPath) == false)
	{
		NotifyBox( m_hWnd, "파일을 추가할 수 없습니다.\n%s 경로는 Working Directory에 포함된 경로가 아닙니다.\n파일이 포함된 경로를 다시 확인해주세요.", pszPath);
		return NULL;
	
	}

	// 상대 Path로 만든다.
	i3::make_relative_path( g_pOption->m_szWorkPath, pszPath, szPath );

	i3::vector<i3::rc_string>	token;
	i3::resolve_path_token( szPath, token);
	i3PackNode * pParent, * pNode;
	
	HTREEITEM hParent;

	pParent = g_pPack;

	size_t cnt = token.size();

	// 만약 주어진 경로가 폴더가 아니라 파일에 대한 것이라면
	// 가장 마지막의 Token은 File명이기 때문에 고려하지 않는다.
	if(!token.empty() && bIsFile)
		cnt --;

	for( size_t i = 0; i < cnt; i++)
	{
		const char * pszName = token[i].c_str();

		pNode = (i3PackNode *) pParent->FindChildByName( pszName);

		hParent = i3TDK::FindTreeItemByData( &m_TreeCtrl, pParent);
		I3ASSERT( hParent != NULL);

		if( pNode == NULL)
		{
			// 없던 Path... 추가해 준다.
			pNode = i3PackNode::new_object_ref();
			pNode->SetName( pszName);

			pParent->AddChild( pNode);

			_Rec_SetTree( pNode, hParent);
		}

		pParent = pNode;
	}

	return pParent;
}

void CPanelTree::Add( const char * pszPath, BOOL IncludeSubDir)
{	
	const char* ch= ".i3Pack";
	INT32 Index = i3::contain_string(pszPath,ch);
	
	//i3Pack파일은 따로 처리합니다.
	if(Index != -1)
	{
		i3PackVolume file;

		if( file.LoadFromFile( pszPath) == false)
		{
			NotifyBox( m_hWnd, "%s 파일을 열 수 없습니다.", pszPath);
			return;
		}

		i3PackNode * pNode = (i3PackNode *) file.getRoot();

		if(!i3::same_of<i3PackNode*>(pNode)) //->IsExactTypeOf( i3PackNode::static_meta()) == false )
		{
			NotifyBox( m_hWnd, "%s 파일을 올바른 i3Pack 파일이 아닙니다.", pszPath);
			return;
		}

		I3_REF_CHANGE( g_pPack, pNode);
		strcpy( g_szPath, pszPath);
		strcpy( g_szVolumeName, pszPath);

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SCENE, g_pPack);

	}
	else
	{
		i3PackNode * pNode;

		if( i3System::IsDirectory( pszPath))
		{
			pNode = PreparePath( pszPath, false);

			AddDirectory( pszPath, pNode, IncludeSubDir);

			return;
		}


		CFileFind	FindCtx;
		BOOL IsFind = FindCtx.FindFile(pszPath);
		if ( FALSE == IsFind )
		{
			PRINTF( "이름이 잘못되었거나 없는 파일(폴더)입니다.");
			return;
		}
		
		pNode = PreparePath( pszPath, true);
		I3ASSERT( pNode != NULL);
		if( IsRelativePath( pszPath) == false)
			return;
		//같은목록을 추가하려고 하면 기존의 리스트를 삭제하고 다시 추가합니다. 
		for(INT32 i =0; i < pNode->getFileCount();i++)
		{
			const char* ch2 = pNode->getFile(i)->m_szName;
			INT32 result = i3::contain_string(pszPath,ch2);
			if(result != -1)
				pNode->RemoveFile(pNode->getFile(i)->m_szName);
		}


		while (IsFind)
		{
			IsFind = FindCtx.FindNextFile();

			if (FindCtx.IsHidden() || FindCtx.IsSystem())
				continue;

			if (FindCtx.IsDirectory())
				continue;
		
			pNode->AddFile(FindCtx.GetFileName(), 0, static_cast<UINT32>(FindCtx.GetLength()), 0);
		}
	
	
	}
	
}

void CPanelTree::AddDirectory( const char * pszPath, i3PackNode * pNode, BOOL IncludeSubDir)
{
	_BuildFileList( pszPath, pNode);

	if (! IncludeSubDir)
		return;


	CString	szPath;
	szPath.Format("%s\\*.*", pszPath);

	CFileFind	FindCtx;
	BOOL IsFind = FindCtx.FindFile(szPath);

	while( IsFind )
	{
		IsFind = FindCtx.FindNextFile();
		if ( FindCtx.IsHidden() || FindCtx.IsSystem() )
			continue;

		if ( FindCtx.IsDots() )
			continue;

		if (! FindCtx.IsDirectory())
			continue;

		CString	FileName = FindCtx.GetFileName();
		i3PackNode * pNewNode = (i3PackNode *) pNode->FindChildByName( FileName);
		if( pNewNode == NULL)
		{
			pNewNode = i3PackNode::new_object_ref();

			pNode->AddChild( pNewNode);
			pNewNode->SetName((const char*) FileName);

			{
				HTREEITEM hParent = i3TDK::FindTreeItemByData( &m_TreeCtrl, pNode);
				I3ASSERT( hParent != NULL);

				_Rec_SetTree( pNewNode, hParent);
			}
		}


		szPath.Format("%s/%s", pszPath, FileName);
		AddDirectory( szPath, pNewNode, IncludeSubDir);
	}
}

void CPanelTree::Remove( const char * pszPath, const BOOL includeSubDirOK)
{
	CFileFind	FindCtx;

	BOOL IsFind = FindCtx.FindFile(pszPath);
	while(IsFind)
	{
		IsFind = FindCtx.FindNextFile();

		if (FindCtx.IsDots())
			continue;

		if (FindCtx.IsSystem())
			continue;

		if (!includeSubDirOK)
		{
			if (FindCtx.IsDirectory())
				continue;
		}

		// 상대 Path로 만든다.
		char RelPath[512] = {};
		CString str = FindCtx.GetFilePath();
		
		i3::make_relative_path( g_pOption->m_szWorkPath, LPCTSTR(FindCtx.GetFilePath()), RelPath );

		I3_PACK_FILE_REF * pFile;
		i3PackNode * pNode = g_pPack->FindPath( RelPath, &pFile);
		if( pNode == NULL)
		{
			PRINTF( "%s 파일을 찾을 수 없어, 제거하지 못했음.\n", pszPath);
			continue;
		}

		if( pFile != NULL)
		{
			// 파일 제거
			CString	FileName = FindCtx.GetFileName();
			pNode->RemoveFile( FileName);
		}
		else
		{
			// Directory 제거.
			HTREEITEM hItem = i3TDK::FindTreeItemByData( &m_TreeCtrl, pNode);

			if( hItem != NULL)
			{
				m_TreeCtrl.DeleteItem( hItem);
			}

			i3PackNode * pParent = (i3PackNode *) pNode->getParent();

			pParent->RemoveChild( pNode);
		}
	}

}

BOOL CPanelTree::_BuildFileList( const char * pszPath, i3PackNode * pParent)
{
	WIN32_FIND_DATA fi;
	HANDLE hFind;
	char szPath[ MAX_PATH];
	UINT32		mask;

	pParent->SetFileCount( 0);

	sprintf( szPath, "%s\\*.*", pszPath);

	mask = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_DIRECTORY;

	hFind = FindFirstFile( szPath, &fi);

	while( hFind != INVALID_HANDLE_VALUE)
	{
		if( (fi.dwFileAttributes & mask) == 0)
		{
			if( g_pOption->isExceptFile( fi.cFileName) == false)
			{
				pParent->AddFile( fi.cFileName, 0, fi.nFileSizeLow, 0);
			}
		}

		BOOL bRv = FindNextFile( hFind, &fi);
		if( bRv == FALSE)
			break;
	}

	FindClose( hFind);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CPanelTree, CDialog)
	ON_WM_DROPFILES()
	ON_NOTIFY(NM_CLICK, IDC_PACK_TREE, OnNMClickPackTree)
	ON_NOTIFY(NM_RCLICK, IDC_PACK_TREE, OnNMRclickPackTree)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_PACK_TREE, OnTvnSelchangedPackTree)
END_MESSAGE_MAP()


// CPanelTree message handlers

BOOL CPanelTree::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_TreeCtrl.SetImageList( g_pSysImageList, TVSIL_NORMAL);

		SHFILEINFO fi;
		char conv[MAX_PATH];

		GetCurrentDirectory( sizeof(conv), conv);

		SHGetFileInfo( conv, FILE_ATTRIBUTE_NORMAL, &fi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX);
		m_idxIconNormal = fi.iIcon;

		SHGetFileInfo( conv, FILE_ATTRIBUTE_NORMAL, &fi, sizeof(SHFILEINFO), SHGFI_OPENICON | SHGFI_SYSICONINDEX);
		m_idxIconSelect = fi.iIcon;
	}

	{
		i3TDK::RegisterUpdate( m_hWnd, i3PackNode::static_meta(), I3_TDK_UPDATE_ALL);
	}

	DragAcceptFiles( TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelTree::OnOK()
{
}

void CPanelTree::OnCancel()
{
}

void CPanelTree::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_TreeCtrl.m_hWnd))
	{
		m_TreeCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}


void CPanelTree::OnDropFiles(HDROP hDropInfo)
{
	INT32	nCount, i;
	char	TempName[MAX_PATH << 1];
	i3PackNode * pNode;
	DWORD	attr;

 	nCount = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);

	for( i = 0; i < nCount; ++i)
	{
		DragQueryFile( hDropInfo, i, TempName, sizeof( TempName) -1);

		attr = GetFileAttributes( TempName);

		if( attr & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY))
			continue;

		if( attr & FILE_ATTRIBUTE_DIRECTORY)
		{
			pNode = PreparePath( TempName, false);

			if( pNode != NULL)
				AddDirectory( TempName, pNode, TRUE);
		}
		else
		{
			Add( TempName, TRUE);
		}
	}

	UpdateAll();

	DragFinish( hDropInfo);

	CDialog::OnDropFiles(hDropInfo);
}

void CPanelTree::OnNMClickPackTree(NMHDR *pNMHDR, LRESULT *pResult)
{

	*pResult = 0;
}

void CPanelTree::OnNMRclickPackTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu	Menu, *  pPopup;
	POINT pt;
	UINT32	cmd;
//	HTREEITEM	hSel = NULL;

	GetCursorPos( &pt);

	Menu.LoadMenu( IDR_POPUP_PACKNODE);

	pPopup = Menu.GetSubMenu( 0);

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
			pt.x, pt.y, (CWnd *) this, NULL);

	switch( cmd)
	{
		case ID_PACKNODE_DELETE:		OnDelete();		break;
		case ID_PACKNODE_EXPORTTO :		OnExport();		break;
		default:
			break;
	}

	*pResult = 0;
}

void CPanelTree::OnSelect( I3_TDK_UPDATE_INFO * pInfo)
{
	switch( pInfo->m_Event)
	{
		case I3_TDK_UPDATE_SCENE :		UpdateAll();	break;
	}
}

void CPanelTree::OnDelete(void)
{
	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();

	if( hItem == NULL)
		return;

	i3PackNode * pPackNode = (i3PackNode *) m_TreeCtrl.GetItemData( hItem);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_REMOVE, pPackNode);

	pPackNode->RemoveFromParent();

	m_TreeCtrl.DeleteItem( hItem);
}

void CPanelTree::OnExport(void)
{
	i3TDKFolderSelectDialog dlg;
	i3FileStream strmSrc;

	if( dlg.Execute( m_hWnd, "Export할 폴더를 선택하세요.") == FALSE)
		return;

	if( g_szVolumeName[0] == 0)
	{
		AfxMessageBox( "Export할 Volume 원본 파일이 없습니다.");
		return;
	}

	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();

	if( hItem == NULL)
		return;

	if( strmSrc.Open( g_szVolumeName, STREAM_READ | STREAM_SHAREREAD) == FALSE)
	{
		NotifyBox( m_hWnd, "%s 파일을 열 수 없습니다.", g_szVolumeName);
		return;
	}

	i3PackNode * pPackNode = (i3PackNode *) m_TreeCtrl.GetItemData( hItem);

	_Rec_Export( &strmSrc, pPackNode, dlg.GetSelectedFolderPath());
}

void CPanelTree::_Rec_Export( i3FileStream * pFile, i3PackNode * pNode, const char * pszPath)
{
	char szBase[ MAX_PATH];
	INT32 i;

	if(i3::generic_is_iequal( pNode->GetName(), "/") == false)
	{
		sprintf( szBase, "%s/%s", pszPath, pNode->GetName());
		i3System::PrepareDirectory( szBase);
	}
	else
	{
		strcpy( szBase, pszPath);
	}

	// 파일 저장
	for( i = 0; i < pNode->getFileCount(); i++)
	{
		char szPath[MAX_PATH];

		I3_PACK_FILE_REF * pRef = pNode->getFile( i);

		{
			i3FileStream strm;

			sprintf( szPath, "%s/%s", szBase, pRef->m_szName);

			I3TRACE( "SAVING : %s\n", szPath);

			if( strm.Create( szPath, STREAM_WRITE) == FALSE)
			{
				I3PRINTLOG(I3LOG_WARN, "%s 파일을 생성할 수 없습니다.", szPath);
				continue;
			}

			pFile->SetPosition( pRef->m_Offset, STREAM_BEGIN);

			strm.CopyFrom( pFile, pRef->m_Size);
		}
	}

	for( i = 0; i < pNode->getChildCount(); i++)
	{
		i3PackNode * pChild = (i3PackNode *) pNode->getChild( i);

		_Rec_Export( pFile, pChild, (const char *) szBase);
	}
}

LRESULT CPanelTree::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnSelect( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CPanelTree::OnTvnSelchangedPackTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;

	if( hItem == NULL)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, NULL, i3PackNode::static_meta());
	}
	else
	{
		i3PackNode * pNode = (i3PackNode *) m_TreeCtrl.GetItemData( hItem);

		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, pNode);
	}

	*pResult = 0;
}
