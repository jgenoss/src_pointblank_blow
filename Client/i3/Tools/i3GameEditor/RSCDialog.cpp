// RSCDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3GameEditor.h"
#include "RSCDialog.h"
#include ".\rscdialog.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/make_relative_path.h"

// RSCDialog dialog

IMPLEMENT_DYNAMIC(RSCDialog, CDialog)
RSCDialog::RSCDialog(CWnd* pParent /*=NULL*/)
	: CDialog(RSCDialog::IDD, pParent)
{
	m_pCopyList = NULL;
	m_pszWorkDir = NULL;

	m_FileList.clear();
}

RSCDialog::~RSCDialog()
{
}

void RSCDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RSC, m_RSCListBox);
	DDX_Control(pDX, ID_RSC_ADD, m_BnRscAdd);
	DDX_Control(pDX, IDOK, m_BnRscClose);
	DDX_Control(pDX, ID_RSC_DELETE, m_BnRscDelete);
	DDX_Control(pDX, ID_RSC_EDIT, m_BnRscEdit);
 }


BEGIN_MESSAGE_MAP(RSCDialog, CDialog)
	ON_BN_CLICKED(ID_RSC_ADD, OnBnClickedRscAdd)
	ON_BN_CLICKED(ID_RSC_DELETE, OnBnClickedRscDelete)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(ID_RSC_EDIT, OnBnClickedRscEdit)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_LBN_DBLCLK(IDC_LIST_RSC, OnLbnDblclkListRsc)
	ON_BN_CLICKED(IDNO, OnBnClickedNo)
END_MESSAGE_MAP()


void RSCDialog::ClearRSC( void)
{
	m_RSCListBox.ResetContent();
	m_FileList.clear();
}

void RSCDialog::AddRSC( const char * pszPath)
{ 
	//	파일 추가	
	INT32 ret = m_RSCListBox.AddString( pszPath);
	if( ret >= 0)
	{
		m_FileList.push_back( pszPath);
	}
	else
	{
		I3PRINTLOG(I3LOG_WARN, "Fail to added RSC file");
	}
}

void RSCDialog::RemoveRSC( INT32 idx)
{
	//	파일 삭제
	INT32 ret = m_RSCListBox.DeleteString( idx);
	if( ret < 0)
	{
		__CHECKW_RT( 1, "Fail to removed RSC file");		
	}
	
	i3::vu::erase_index(m_FileList, idx);
}

void RSCDialog::RemoveRSC( char * pszPath)
{
	//	파일 삭제
	INT32 idx = m_RSCListBox.FindString( 0, pszPath);
	
	RemoveRSC( idx);
}

// RSCDialog message handlers

LRESULT RSCDialog::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	
	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL RSCDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pszWorkDir = (char *) g_Profile.GetWorkDir();

	//	받아온 리스트로 FileList를 초기화한다.
	if( m_pCopyList)
	{
		for( size_t i = 0; i < m_pCopyList->size(); i++)
		{
			AddRSC( (*m_pCopyList)[i].c_str());
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void RSCDialog::OnBnClickedRscAdd()
{
	const INT32 maxSize = 4096;

	char szFullPath[maxSize] = "";
	char szFile[256] = "";
	
	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof( OPENFILENAME));
	ofn.hwndOwner = this->GetSafeHwnd();
	ofn.lStructSize = sizeof( OPENFILENAME);
	ofn.lpstrFilter = "RSC 파일 (*.rsc)\0*.rsc\0";
	ofn.lpstrFile = szFullPath;
	ofn.nMaxFile = maxSize;
	ofn.lpstrFileTitle = szFile;
	ofn.nMaxFileTitle = 256;
	ofn.lpstrInitialDir = m_pszWorkDir;
	ofn.lpstrTitle = "파일 선택";
	ofn.lpstrDefExt = "rsc";
	ofn.Flags = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_NONETWORKBUTTON | OFN_NOTESTFILECREATE | 
				OFN_ALLOWMULTISELECT | OFN_EXPLORER;

	if( GetOpenFileName( &ofn) != 0)
	{
		char szPath[256] = "";

		if( strlen( (char *) g_Profile.GetWorkDir()) > 0)
			i3::make_relative_path( (char *) g_Profile.GetWorkDir(), szFullPath, szPath);
		else
			i3::safe_string_copy( szPath, szFullPath, 256 );

		i3::make_unix_path( szPath);

		if( strlen( szFile) > 0)		//	select single file
		{
			AddRSC( szPath);
		}
		else							//	select multi files
		{
			char * buffer = szFullPath + strlen( szFullPath) + 1;//	첫번째는 파일 경로이므로 다음 파일 이름으로 넘어간다.
	
			while( 1)
			{
				if( *buffer == 0)	break;	//	모든 파일 이름을 처리 할때까지 

				//	상대 경로가 들어간 파일 경로를 만든다.
				i3::safe_string_copy( szFile, szPath, 256 );
				i3::generic_string_cat( szFile, "\\");
				i3::generic_string_cat( szFile, buffer);

				i3::make_unix_path( szFile);

				AddRSC( szFile);

				buffer = buffer + strlen( buffer) + 1;			//	다음 파일 이름으로
			}
		}
	}

	m_pszWorkDir = NULL;
}

void RSCDialog::OnBnClickedRscDelete()
{
	//	여러개를 선택했을 경우
	INT32 count = m_RSCListBox.GetSelCount();
	INT32 idx[32];

	m_RSCListBox.GetSelItems( count, idx);	//여러개의 아이템의 인덱스를 가져온다.
	for( INT32 i = count - 1; i >= 0; i--)
	{
		RemoveRSC( idx[ i]);
	}
}

void RSCDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	i3TDKGameRegistry::Destroy();

	OnOK();
}

void RSCDialog::OnBnClickedNo()
{
	// TODO: Add your control notification handler code here
	i3TDKGameRegistry::Destroy();

	OnCancel();
}

void RSCDialog::OnBnClickedRscEdit()
{
	char szRSCPath[ 1024] = "";

	STARTUPINFO			si;
	PROCESS_INFORMATION	pi;

	memset( &si, 0, sizeof( si));
	memset( &pi, 0, sizeof( pi));
	si.cb = sizeof( STARTUPINFO);
	si.dwFlags = 0;
	
	char szFileName[256] = ""; 
	char * pszWorkDir = (char *) g_Profile.GetWorkDir();

	//	현재 선택된 RSC파일을 찾는다.
	INT32 idx = m_RSCListBox.GetCurSel();
	m_RSCListBox.GetText( idx, szFileName); 

	//	실행할 프로세서의 경로를 만든다.
	i3::safe_string_copy( szRSCPath, (char *) g_Profile.GetRSCFile(), 1024 );
	i3::generic_string_cat( szRSCPath, " ");

	//	파일이름을 넣는다
	if( strlen( szFileName) > 0)
	{	
		if( strlen( pszWorkDir) > 0)		//	상대 경로가 지정된 경우
		{
			i3::generic_string_cat( szRSCPath, pszWorkDir);
			i3::generic_string_cat( szRSCPath, "\\");
		}

		char szRegFileName[256];
		i3::change_file_ext_copy( szRegFileName, szFileName, REG_FILE_EXT);
		i3::generic_string_cat( szRSCPath, szRegFileName);	//	2번째 인자로 i3Reg파일이름
		
		//	Registry
//		CMainFrame * pMain = theApp.GetMainFrm();
//		char * pszFileName = pMain->GetWorkFileName();

		RegData data;
		data._pi3Framework = g_pGameFramework;
		i3::safe_string_copy( data._szWorkingDir, g_Profile.GetWorkDir(), 256 );

		i3::safe_string_copy( data._szRSCFileName, data._szWorkingDir, 256 );
		i3::generic_string_cat( data._szRSCFileName, "\\");
		i3::generic_string_cat( data._szRSCFileName, szFileName);

		i3TDKGameRegistry::Make( &data);

		sprintf( szFileName, "%s\\%s", data._szWorkingDir, szRegFileName);
		i3TDKGameRegistry::Save( szFileName);
	}	

	//	프로세스 실행하기
	BOOL ret = CreateProcess( NULL, szRSCPath, NULL, NULL, FALSE, 0, NULL, g_Profile.GetWorkDir(), &si, &pi);
	
	//	실패하면
	if( ret == 0)
	{
		char szError[256] = "";
		sprintf( szError, "(CODE: %d) Cannot run process RSC Gen \r\n\"%s\"", GetLastError(), szRSCPath);
		
		__CHECKW_RT( 1, szError);
	}
}

int RSCDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void RSCDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
}

void RSCDialog::OnLbnDblclkListRsc()
{
	OnBnClickedRscEdit();
}
