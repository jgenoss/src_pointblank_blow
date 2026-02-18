// PanelFileList.cpp : implementation file
//

#include "stdafx.h"
#include "i3PackTool.h"
#include "PanelFileList.h"
#include "GlobalVar.h"
#include "SingleFile.h"

#include "i3Base/string/ext/integral_to_comma_str.h"

// CPanelFileList dialog

IMPLEMENT_DYNAMIC(CPanelFileList, CDialog)
CPanelFileList::CPanelFileList(CWnd* pParent /*=NULL*/)
	: CDialog(CPanelFileList::IDD, pParent)
{
}

CPanelFileList::~CPanelFileList()
{
}

void CPanelFileList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FILE_LIST, m_FileListCtrl);
}

void CPanelFileList::UpdateAll( i3PackNode * pNode)
{
	INT32 i, idx;
	char szPath[ MAX_PATH], szFull[ MAX_PATH];
	SHFILEINFO fi;

	m_FileListCtrl.DeleteAllItems();

	{
		char szTemp[MAX_PATH];

		szTemp[0] = 0;
		pNode->GetFullPath( szTemp);

		sprintf( szPath, "%s\\%s", g_pOption->m_szWorkPath, szTemp);
	}

	for( i = 0; i < pNode->getFileCount(); i++)
	{
		I3_PACK_FILE_REF * pFile = (I3_PACK_FILE_REF *) pNode->getFile( i);

		{
			sprintf( szFull, "%s\\%s", szPath, pFile->m_szName);

			SHGetFileInfo( szFull, FILE_ATTRIBUTE_NORMAL, &fi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX);
		}

		idx = m_FileListCtrl.InsertItem(	LVIF_IMAGE | LVIF_TEXT, 
											i, 
											pFile->m_szName,
											0, 0,
											fi.iIcon,
											(LPARAM) pFile);

		UpdateItem( idx, pFile);
	}
}

void CPanelFileList::UpdateItem( INT32 idx, I3_PACK_FILE_REF * pFile)
{
	char conv[256];

	m_FileListCtrl.SetItemData( idx, (DWORD_PTR) pFile);

	// Name
	// 이미 했으므로...

	// Attribute
	if( pFile->m_Attr & I3_PACK_FILE_ATTR_SECURE)
		m_FileListCtrl.SetItemText( idx, 1, "S");

	// Size
	i3::integral_to_comma_str( pFile->m_Size, conv, sizeof(conv) - 1);
	m_FileListCtrl.SetItemText( idx, 2, conv);

	// Offset
	i3::integral_to_comma_str( pFile->m_Offset, conv, sizeof(conv) - 1);
	m_FileListCtrl.SetItemText( idx, 3, conv);
}

INT32 CPanelFileList::_findItemByFile( I3_PACK_FILE_REF * pFile)
{
	INT32 i;
	I3_PACK_FILE_REF * pTemp;

	for( i = 0; m_FileListCtrl.GetItemCount(); i++)
	{
		pTemp = (I3_PACK_FILE_REF *) m_FileListCtrl.GetItemData( i);

		if( pTemp == pFile)
			return i;
	}

	return -1;
}

BEGIN_MESSAGE_MAP(CPanelFileList, CDialog)
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_FILE_LIST, OnNMClickFileList)
END_MESSAGE_MAP()


// CPanelFileList message handlers

BOOL CPanelFileList::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_FileListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT);
		m_FileListCtrl.InsertColumn( 0, "Name", LVCFMT_LEFT, 220);
		m_FileListCtrl.InsertColumn( 1, "Attributes", LVCFMT_LEFT, 100);
		m_FileListCtrl.InsertColumn( 2, "Size", LVCFMT_LEFT, 90);
		m_FileListCtrl.InsertColumn( 3, "Offset", LVCFMT_LEFT, 90);

		m_FileListCtrl.SetImageList( g_pSysImageList, LVSIL_SMALL);
	}

	i3TDK::RegisterUpdate( m_hWnd, i3PackNode::static_meta(), I3_TDK_UPDATE_ALL);
	i3TDK::RegisterUpdate( m_hWnd, SingleFile::static_meta(), I3_TDK_UPDATE_ALL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanelFileList::OnOK()
{
}

void CPanelFileList::OnCancel()
{
}

void CPanelFileList::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_FileListCtrl.m_hWnd))
	{
		m_FileListCtrl.SetWindowPos( NULL, 3, 3, cx - 6, cy - 6, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void CPanelFileList::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	switch( pInfo->m_Event)
	{
		case I3_TDK_UPDATE_SELECT :	
			if( i3::same_of<i3PackNode*>(pInfo->m_pMeta)) //->IsExactTypeOf( i3PackNode::static_meta()))
				UpdateAll( (i3PackNode *) pInfo->m_pObject); 
			break;

		case I3_TDK_UPDATE_EDIT :
			if( i3::same_of<i3PackNode*>(pInfo->m_pMeta)) //->IsExactTypeOf( i3PackNode::static_meta()))
			{
			}
			else if( i3::same_of<SingleFile*>(pInfo->m_pMeta)) //->IsExactTypeOf( SingleFile::static_meta()))
			{
				INT32 idx = _findItemByFile( (I3_PACK_FILE_REF *) pInfo->m_pObject);

				if( idx != -1)
				{
					UpdateItem( idx, (I3_PACK_FILE_REF *) pInfo->m_pObject);
				}
			}
			break;
	}
}

LRESULT CPanelFileList::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

INT32 CPanelFileList::_getSelectedItem(void)
{
	POSITION pos = m_FileListCtrl.GetFirstSelectedItemPosition();

	if( pos == NULL)
		return -1;

	return m_FileListCtrl.GetNextSelectedItem( pos);
}

void CPanelFileList::OnNMClickFileList(NMHDR *pNMHDR, LRESULT *pResult)
{
	INT32 idx;
	I3_PACK_FILE_REF * pFile;

	idx = _getSelectedItem();
	if( idx == -1)
	{
		i3TDK::Update( m_hWnd, I3_TDK_UPDATE_UNSELECT, NULL, SingleFile::static_meta());
		return;
	}

	pFile = (I3_PACK_FILE_REF *) m_FileListCtrl.GetItemData( idx);

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_SELECT, (i3ElementBase *) pFile, SingleFile::static_meta());

	*pResult = 0;
}
