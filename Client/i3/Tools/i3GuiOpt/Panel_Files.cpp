// Panel_Files.cpp : implementation file
//

#include "stdafx.h"
#include "i3GuiOpt.h"
#include "Panel_Files.h"
#include "GlobalVar.h"
#include "i3Base/i3StringListBuffer.h"
#include "i3Base/string/ext/make_relative_path.h"

// CPanel_Files dialog

IMPLEMENT_DYNAMIC(CPanel_Files, i3TDKDialogBase)

CPanel_Files::CPanel_Files(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanel_Files::IDD, pParent)
{

}

CPanel_Files::~CPanel_Files()
{
}

void CPanel_Files::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILE, m_LIST_File);
	DDX_Control(pDX, IDC_ED_WORKDIR, m_ED_WorkDir);
}

void CPanel_Files::UpdateWorkDir(void)
{
	char conv[MAX_PATH];

	i3::string_ncopy_nullpad( conv, g_pTDKEnvSet->getCurWorkDir(), sizeof(conv));
	i3::make_unix_path( conv);

	m_ED_WorkDir.SetWindowText( conv);
}

void CPanel_Files::AddFile( const char * pszAbsPath)
{
	char szRelPath[ MAX_PATH];

	i3::make_relative_path( g_pTDKEnvSet->getCurWorkDir(), pszAbsPath, szRelPath  );

	i3::make_unix_path( szRelPath);

	if( i3::vu::int_index_of(g_FileList, szRelPath) == -1)
	{
		size_t idx = g_FileList.size();
		g_FileList.push_back( szRelPath);
		m_LIST_File.InsertItem( idx, szRelPath, 0);
	}
}

void	CPanel_Files::UpdateAll(void)
{
	

	m_LIST_File.DeleteAllItems();

	for(size_t i = 0; i < g_FileList.size(); i++)
	{
		m_LIST_File.InsertItem( i, g_FileList[i].c_str(), 0);
	}
}

void	CPanel_Files::OnDelete(void)
{
	INT32 i;

	for( i = 0; i < m_LIST_File.GetItemCount(); i++)
	{
		if( m_LIST_File.GetItemState( i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			m_LIST_File.DeleteItem( i);
			i3::vu::erase_index(g_FileList, i);
			i--;
		}
	}
}

void	CPanel_Files::OnSelectAll(void)
{
	INT32 i;

	for( i = 0; i < m_LIST_File.GetItemCount(); i++)
	{
		m_LIST_File.SetItemState( i, 0xFFFFFFFF, LVIS_SELECTED);
	}
}

void	CPanel_Files::OnInvertSelection(void)
{
	INT32 i;

	for( i = 0; i < m_LIST_File.GetItemCount(); i++)
	{
		if( m_LIST_File.GetItemState( i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			m_LIST_File.SetItemState( i, 0, LVIS_SELECTED);
		}
		else
		{
			m_LIST_File.SetItemState( i, 0xFFFFFFFF, LVIS_SELECTED);
		}
	}
}

void	CPanel_Files::OnRemoveNonExistFiles(void)
{
	
	char szFull[ MAX_PATH];
	const char * pszBasePath = g_pTDKEnvSet->getCurWorkDir();

	for(size_t i = 0; i < g_FileList.size(); i++)
	{
		if( pszBasePath != NULL)
		{
			i3::snprintf( szFull, sizeof(szFull), "%s/%s", pszBasePath, g_FileList[i].c_str());
		}
		else
		{
			i3::snprintf( szFull, sizeof(szFull), "%s", g_FileList[i].c_str());
		}

		if( i3System::IsFile( szFull) == FALSE)
		{
			m_LIST_File.DeleteItem( i);

			i3::vu::erase_index(g_FileList, i);

			i--;
		}
	}
}

void CPanel_Files::LoadFileList( const char * pszPath, bool bMerge)
{
	char * pBuf;
	UINT32 sz;
	i3FileStream file;

	if( file.Open( pszPath, STREAM_READ) == FALSE)
	{
		NotifyBox( m_hWnd, "Could not open %s list file.", pszPath);
		return;
	}

	sz = file.GetSize();

	if( sz > 0)
	{
		pBuf = new char[ sz];

		file.Read( pBuf, sz);

	//	g_pFileList->CopyFromBuffer( pBuf, bMerge == false);
		CopyFromBufferToStringList(g_FileList, pBuf, bMerge == false);

		delete pBuf;
	}

	file.Close();

	UpdateAll();
}

void CPanel_Files::OnImportFileList( bool bMerge)
{
	CFileDialog	Dlg( TRUE, "i3OptList", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
			"i3Engine Optimizer File List(*.i3OptList)|*.i3OptList||", this);

	if( Dlg.DoModal() != IDOK) return;

	LoadFileList( LPCTSTR( Dlg.GetPathName()), bMerge);
}


void CPanel_Files::OnExportFileList(void)
{
	CFileDialog	Dlg( FALSE, "i3OptList", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
			"i3Engine Optimizer File List (*.i3OptList)|*.i3OptList||", this);

	if( Dlg.DoModal() != IDOK )
		return;

	char * pBuf;
	INT32 sz =	GetStringListBufferSize(g_FileList);	//	g_pFileList->GetBufferSize();

	pBuf = new char[ sz];

	//g_pFileList->CopyToBuffer( pBuf, sz);
	CopyFromStringListToBuffer(g_FileList, pBuf, sz);

	{
		i3FileStream file;

		file.Create( LPCTSTR( Dlg.GetPathName()), STREAM_WRITE);

		file.Write( pBuf, sz);

		file.Close();
	}

	delete pBuf;
}

BEGIN_MESSAGE_MAP(CPanel_Files, i3TDKDialogBase)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FILE, &CPanel_Files::OnNMRClickListFile)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPanel_Files message handlers

BOOL CPanel_Files::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	{
		m_LIST_File.SetExtendedStyle( LVS_EX_FULLROWSELECT);

		m_LIST_File.InsertColumn( 0, "Path", LVCFMT_LEFT, 300);
	}

	UpdateWorkDir();

	ReplaceControls();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanel_Files::OnOK()
{
}

void CPanel_Files::OnCancel()
{
}

void CPanel_Files::OnNMRClickListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt, pt2;
	UINT cmd;
	CMenu menu, * pPopup;

	GetCursorPos( &pt);

	{
		menu.LoadMenu( IDR_POPUP_FILE);
		pPopup = menu.GetSubMenu( 0);
	}

	cmd = pPopup->TrackPopupMenuEx( TPM_LEFTALIGN | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (CWnd *) this, NULL);

	switch( cmd)
	{
		case ID_FILE_DELETE :		
			OnDelete();
			break;

		case ID_FILE_SELECTALL :
			OnSelectAll();
			break;

		case ID_FILE_INVERTSELECTION :
			OnInvertSelection();
			break;

		case ID_FILE_REMOVEUNEXISTFILES :
			OnRemoveNonExistFiles();
			break;

		case ID_FILE_IMPORTFILELIST :
			OnImportFileList( false);
			break;

		case ID_FILE_MERGEFILELIST :
			OnImportFileList( true);
			break;

		case ID_FILE_EXPORTFILELIST :
			OnExportFileList();
			break;
	}

	*pResult = 0;
}

void	CPanel_Files::StartProcessing(void)
{
	m_LIST_File.EnableWindow( FALSE);
}

void	CPanel_Files::EndProcessing(void)
{
	m_LIST_File.EnableWindow( TRUE);
}

void	CPanel_Files::ReplaceControls( INT32 cx, INT32 cy)
{
	CRect rt;

	if( cx == -1)
	{
		GetClientRect( &rt);

		cx = rt.Width();
		cy = rt.Height();
	}

	m_ED_WorkDir.GetWindowRect( &rt);
	ScreenToClient( &rt);

	m_ED_WorkDir.SetWindowPos( NULL, 0, 0, cx - rt.left - 3, rt.Height(), SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);

	m_LIST_File.SetWindowPos( NULL, rt.left, rt.bottom + 3, cx - rt.left - 3, cy - 3 - rt.bottom - 3, SWP_NOACTIVATE | SWP_NOZORDER);
}


void CPanel_Files::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if( ::IsWindow( m_LIST_File.m_hWnd))
	{
		ReplaceControls( cx, cy);
	}
}
