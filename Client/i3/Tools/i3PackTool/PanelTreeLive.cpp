// PanelTreeLive.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "i3PackTool.h"
#include "PanelTreeLive.h"
#include "GlobalVar.h"

#include "i3Base/string/ext/remove_end_slash.h"
#include "i3Base/string/ext/contain_string.h"

using namespace std;

// CPanelTreeLive 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPanelTreeLive, CDialog)

CPanelTreeLive::CPanelTreeLive(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelTreeLive::IDD, pParent)
{

}

CPanelTreeLive::~CPanelTreeLive()
{
}

void CPanelTreeLive::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PACK_TREE2, m_TreeCtrl);
}


BEGIN_MESSAGE_MAP(CPanelTreeLive, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_PACK_TREE2, &CPanelTreeLive::OnNMClickPackTree2)
	ON_NOTIFY(NM_DBLCLK, IDC_PACK_TREE2, &CPanelTreeLive::OnNMDblclkPackTree2)
	ON_NOTIFY(NM_RCLICK, IDC_PACK_TREE2, &CPanelTreeLive::OnNMRClickPackTree2)
//	ON_NOTIFY(TVN_SELCHANGED, IDC_PACK_TREE2, &CPanelTreeLive::OnTvnSelchangedPackTree2)
//	ON_NOTIFY(TVN_DELETEITEM, IDC_PACK_TREE2, &CPanelTreeLive::OnTvnDeleteitemPackTree2)
END_MESSAGE_MAP()


// CPanelTreeLive 메시지 처리기입니다.

BOOL CPanelTreeLive::OnInitDialog()
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

	//MakeTree();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CPanelTreeLive::MakeTree()
{
	//__SP(0);
	CString	workingPath;
	m_iPackDataArr = 0;

	if(g_pOption->m_szLivePath != NULL)
	{
		workingPath.Format("%s\\Pack\\*.*", g_pOption->m_szLivePath);
	}
	else
	{
		return;
	}
	//__EP(0);

	//__SP(1);
	m_TreeCtrl.DeleteAllItems();
	BOOL bWorking = m_FileFinder.FindFile(workingPath);
	CString pszPath;
	i3PackNode * pNode;
	//__EP(1);
	//
	//__SP(2);
	while(bWorking)
	{
		i3PackVolume file;
		//여기서 팩파일 조사해서 파일 경로 알아낸 후 map 및 트리컨트롤에 추가.
		bWorking = m_FileFinder.FindNextFile();

		if (!m_FileFinder.IsHidden() && !m_FileFinder.IsDirectory())
		{
			pszPath = m_FileFinder.GetFilePath();

			if( file.LoadFromFile( pszPath) == false)
			{
				NotifyBox( m_hWnd, "%s 파일을 열 수 없습니다.", pszPath);
				return;
			}

			pNode = (i3PackNode *) file.getRoot();
			_Rec_SetPackTree(pNode, pszPath, TVI_ROOT);
		}
	}
	//__EP(2);
	//RESET_PROFILE();
	//REPORT_PROFILE();
}

void CPanelTreeLive::_Rec_SetPackTree(i3PackNode * pNode, CString pszPath, HTREEITEM hParent)
{
	// 여기서 먼저 hParent를 검색해서 그곳에 새로운 폴더나 파일을 추가한다.
	HTREEITEM newParent;
	INT32 i = 0;
	i3PackNode * pChild;
	TVINSERTSTRUCT item;
	I3_PACK_FILE_REF * pFile;

	if (hParent == TVI_ROOT)
	{
		newParent = FindParentItem(pNode);
	}
	else
	{
		newParent = FindParentItem(pNode, hParent);
	}

	for (i = 0; i < pNode-> getChildCount(); i++)
	{
		pChild = (i3PackNode *) pNode->getChild( i);
		_Rec_SetPackTree( pChild, pszPath, newParent);
	}

	item.hInsertAfter = TVI_LAST;
	item.item.mask = TVIF_TEXT | TVIF_PARAM;
	item.hParent = newParent;
	m_PackData[m_iPackDataArr++].packName = pszPath;

	for (i = 0; i < pNode->getFileCount(); i++)	
	{
		
		pFile = (I3_PACK_FILE_REF *) pNode->getFile(i);
		// 리턴받은 newParent에 파일 추가.
		item.item.pszText = pFile->m_szName;
		item.item.lParam = (LPARAM) &m_PackData[m_iPackDataArr];
		m_TreeCtrl.InsertItem( &item);
	}
}

// 있는 폴더면 찾아서 리턴, 없는 폴더면 추가시켜서 리턴
HTREEITEM CPanelTreeLive::FindParentItem(i3PackNode * pNode, HTREEITEM hParent)
{
	if (pNode->getChildCount() == 0 && pNode->getFileCount() == 0)
	{
		return hParent;
	}

	HTREEITEM currentItem, newItem;

	char * nodeName;
	nodeName = (char *) pNode->GetName();

	if (hParent == TVI_ROOT)
	{
		currentItem = m_TreeCtrl.GetRootItem();
	}
	else
	{
		currentItem = m_TreeCtrl.GetChildItem(hParent);
	}

	while (currentItem)
	{
		if(nodeName == m_TreeCtrl.GetItemText(currentItem))
		{
			return currentItem;
		}
		else
		{
			currentItem = m_TreeCtrl.GetNextSiblingItem(currentItem);
		}
	}

	// currentItem == NULL -> 해당 폴더가 트리에 없으므로 추가해준다.
	TVINSERTSTRUCT item;
	item.hParent = hParent;
	item.hInsertAfter = TVI_LAST;
	if(pNode->getChildCount() != 0 || pNode->getFileCount() != 0)		item.item.cChildren = 1;
	else																item.item.cChildren = 0;
	item.item.iImage = m_idxIconNormal;
	item.item.iSelectedImage = m_idxIconSelect;
	item.item.pszText = nodeName;
	item.item.mask = TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;

	newItem = m_TreeCtrl.InsertItem( &item);

	// Parent Item의 cChildren 항목 수정
	{
		TVITEM info;
		memset( &info, 0, sizeof(info));
		info.mask = TVIF_CHILDREN | TVIF_HANDLE;
		info.cChildren = 1;
		info.hItem = hParent;

		m_TreeCtrl.SetItem( &info);
	}

	return newItem;
}

void CPanelTreeLive::OnExport(void)
{
	i3TDKFolderSelectDialog dlg;
	i3FileStream strmSrc;

	FindCheckItem(m_TreeCtrl.GetRootItem());

	if (m_ExportFile.size() == 0)
	{
		NotifyBox( m_hWnd, "Export 하도록 선택된 파일이 없습니다.");
		return;
	}

	if( dlg.Execute( m_hWnd, "Export할 폴더를 선택하세요.") == FALSE)
		return;

	PreExport(dlg.GetSelectedFolderPath());
}

void CPanelTreeLive::FindCheckItem(HTREEITEM hItem)
{
	HTREEITEM hChildItem = m_TreeCtrl.GetChildItem(hItem);

	while (hChildItem != NULL)
	{
		if (m_TreeCtrl.ItemHasChildren(hChildItem))
		{
			FindCheckItem(hChildItem);
		}
		else
		{
			if (m_TreeCtrl.GetCheck(hChildItem))
			{
				AddExportFile(hChildItem);
			}
		}

		hChildItem = m_TreeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
	}
}

void CPanelTreeLive::AddExportFile(HTREEITEM hItem)
{
	map<CString, CString>::iterator map_it;
	
	PACK_NAME * packData = (PACK_NAME *) m_TreeCtrl.GetItemData(hItem);
	if (packData == NULL)
	{
		return;
	}

	CString packName = packData->packName;
	CString fileName = m_TreeCtrl.GetItemText(hItem);

	map_it = m_ExportFile.find(packName);
	if (map_it == m_ExportFile.end())
	{
		m_ExportFile.insert(pair<CString, CString>(packName, fileName));
	} 
	else
	{
		CString exportFile = map_it->second;
		exportFile += fileName;

		m_ExportFile[packName] = exportFile;
	}
}

void CPanelTreeLive::PreExport(const char * pszPath)
{
	map<CString, CString>::iterator map_it;

	for (map_it = m_ExportFile.begin(); map_it != m_ExportFile.end(); map_it++)
	{
		i3::rc_string packName = map_it->first;

		i3FileStream strmSrc;
		if( strmSrc.Open( packName, STREAM_READ | STREAM_SHAREREAD) == FALSE)
		{
			NotifyBox( m_hWnd, "%s 파일을 열 수 없습니다.", packName);
			return;
		}

		i3PackVolume file;
		if( file.LoadFromFile( packName.c_str()) == false)
		{
			NotifyBox( m_hWnd, "%s 파일을 열 수 없습니다.", packName);
			return;
		}

		i3PackNode * pNode = (i3PackNode *) file.getRoot();

		_Rec_Export( &strmSrc, pNode, pszPath, map_it->second);
	}

	m_ExportFile.clear();
}

void CPanelTreeLive::_Rec_Export( i3FileStream * pFile, i3PackNode * pNode, const char * pszPath, CString strFileList)
{
	char szBase[ MAX_PATH];
	INT32 i;

	if( i3::generic_is_iequal( pNode->GetName(), "/") == false)
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

			if (i3::contain_string(strFileList, pRef->m_szName) != -1)		// 조건 걸면 원하는 파일만 export 된다.
			{
				I3TRACE( "SAVING : %s\n", szPath);

				if( strm.Create( szPath, STREAM_WRITE) == FALSE)
				{
					I3PRINTLOG(I3LOG_WARN, "%s 파일을 생성할 수 없습니다.", szPath);
					continue;
				}
			}

			pFile->SetPosition( pRef->m_Offset, STREAM_BEGIN);

			strm.CopyFrom( pFile, pRef->m_Size);
		}
	}

	for( i = 0; i < pNode->getChildCount(); i++)
	{
		i3PackNode * pChild = (i3PackNode *) pNode->getChild( i);

		_Rec_Export( pFile, pChild, (const char *) szBase, strFileList);
	}
}

void CPanelTreeLive::CheckChildItem(HTREEITEM hItem)
{
	HTREEITEM hChildItem = m_TreeCtrl.GetChildItem(hItem);

	while (hChildItem != NULL)
	{
		m_TreeCtrl.SetCheck(hChildItem, TRUE);

		if (m_TreeCtrl.ItemHasChildren(hChildItem))
		{
			CheckChildItem(hChildItem);
		}

		hChildItem = m_TreeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
	}
}
void CPanelTreeLive::UncheckChildItem(HTREEITEM hItem)
{
	HTREEITEM hChildItem = m_TreeCtrl.GetChildItem(hItem);

	while (hChildItem != NULL)
	{
		m_TreeCtrl.SetCheck(hChildItem, FALSE);

		if (m_TreeCtrl.ItemHasChildren(hChildItem))
		{
			UncheckChildItem(hChildItem);
		}

		hChildItem = m_TreeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
	}
}

void CPanelTreeLive::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_TreeCtrl.m_hWnd))
	{
		m_TreeCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelTreeLive::OnNMClickPackTree2(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	UINT nHitFlags = 0;
	HTREEITEM hItem = m_TreeCtrl.HitTest(pt, &nHitFlags);

	if (hItem == NULL)
		return;

	if (nHitFlags == TVHT_ONITEMSTATEICON)	// click된 위치가 check box면 true
	{
		m_TreeCtrl.SelectItem(hItem);

		if (!m_TreeCtrl.GetCheck(hItem))
		{
			if (m_TreeCtrl.ItemHasChildren(hItem))		CheckChildItem(hItem);
		}
		else
		{
			if (m_TreeCtrl.ItemHasChildren(hItem))		UncheckChildItem(hItem);
		}
	}

	*pResult = 0;
}

void CPanelTreeLive::OnNMDblclkPackTree2(NMHDR *pNMHDR, LRESULT *pResult)
{
	map<CString, CString>::iterator map_it;

	char szTempPath[MAX_PATH] = {0, };
	GetTempPath(MAX_PATH, szTempPath);

	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();
	HTREEITEM hOpenItem = hItem;

	PACK_NAME * packData = (PACK_NAME *) m_TreeCtrl.GetItemData(hItem);
	if (packData == NULL)
	{
		return;
	}

	CString filePath;
	while(m_TreeCtrl.GetItemText(hItem) != "/")
	{
		filePath = m_TreeCtrl.GetItemText(hItem) + filePath;
		filePath = "\\" + filePath;
		hItem = m_TreeCtrl.GetParentItem(hItem);
	}

	AddExportFile(hOpenItem);

	PreExport(szTempPath);

	i3::remove_end_slash(szTempPath);
	CString openFile;
	openFile.Format("%s", szTempPath);
	openFile += filePath;

	ShellExecute(m_hWnd, "open", openFile, NULL, NULL, SW_SHOW);

	*pResult = 0;
}

void CPanelTreeLive::OnNMRClickPackTree2(NMHDR *pNMHDR, LRESULT *pResult)
{
	CMenu	Menu, *  pPopup;
	POINT pt;
	UINT32	cmd;

	GetCursorPos( &pt);

	Menu.LoadMenu( IDR_POPUP_PACKFILE);

	pPopup = Menu.GetSubMenu( 0);

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
		pt.x, pt.y, (CWnd *) this, NULL);

	switch( cmd)
	{
	case ID_PACKFILE_EXPORTTO :		OnExport();		break;
	default:
		break;
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

//void CPanelTreeLive::OnTvnSelchangedPackTree2(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//	*pResult = 0;
//}

//void CPanelTreeLive::OnTvnDeleteitemPackTree2(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//PACK_NAME * packData = (PACK_NAME *) m_TreeCtrl.GetItemData(pNMTreeView->itemOld.hItem);
	//if (packData != NULL)
	//{
	//	delete packData;
	//}

//	*pResult = 0;
//}


LRESULT CPanelTreeLive::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CDialog::WindowProc(message, wParam, lParam);
}