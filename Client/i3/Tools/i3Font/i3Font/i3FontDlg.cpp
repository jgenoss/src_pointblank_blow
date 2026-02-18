// i3FontDlg.cpp : 구현 파일
//
#include "stdafx.h"
#include "i3Font.h"
#include "i3FontDlg.h"
#include ".\i3fontdlg.h"
#include "i3Profile.h"
#include "i3Win32Shell.h"
#include "FileFinder.h"
#include "i3Opt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL MakeImageCB( LPCTSTR szRootPath, LPCTSTR szFileName, void *pArg )
{
	Ci3FontDlg *pThis = (Ci3FontDlg*)pArg;
	return pThis->OnMakeImageCB( szRootPath, szFileName );
}

BOOL MakeStrCB( LPCTSTR szRootPath, LPCTSTR szFileName, void *pArg )
{
	Ci3FontDlg *pThis = (Ci3FontDlg*)pArg;
	return pThis->OnMakeStrCB( szRootPath, szFileName );
}

BOOL Makei3FontCB( LPCTSTR szRootPath, LPCTSTR szFileName, void *pArg )
{
	Ci3FontDlg *pThis = (Ci3FontDlg*)pArg;
	return pThis->OnMakei3FontCB( szRootPath, szFileName );
}

#define I3_SIG_FONT_DICTIONARY	"I3FD"

#pragma pack( push, 1 )
typedef struct {
	char SIG[4];
	UINT8 nMajorVersion;
	UINT8 nMinorVersion;
} I3_FONT_DICTIONARY_HEADER;
#pragma pack( pop )

void ChangePathStyle( char *szPath, BOOL bUnixStyle )
{
	char *ch = szPath;;
	if( bUnixStyle )
	{
		while( ( ch = strchr( ch, '\\' ) ) != NULL )
		{
			*ch = '/';
			ch++;
		}
	}
	else
	{
		while( ( ch = strchr( ch, '/' ) ) != NULL )
		{
			*ch = '\\';
			ch++;
		}
	}
}

void ChangePathStyle( CString &strPath, BOOL bUnixStyle )
{
	char szTemp[MAX_PATH];
	strcpy( szTemp, LPCTSTR( strPath ) );
	ChangePathStyle( szTemp, bUnixStyle );
	strPath = szTemp;
}

// 절대 경로 구해줌. 실제 파일 로드시 사용.
BOOL GetAbsolutePath( const char *szRootDir, char* szOut, const char *szIn, BOOL bUnixStyle = TRUE )
{
	if( szRootDir[0] == 0 )
	{
		strcpy( szOut, szIn );
		ChangePathStyle( szOut, bUnixStyle );
		return FALSE;
	}
	else
	{
		sprintf( szOut, "%s\\%s", szRootDir, szIn );
		ChangePathStyle( szOut, bUnixStyle );
		return TRUE;
	}
}

// 상대 경로 구해줌. 텍스쳐 이름으로 설정. 게임에서 사용.
BOOL GetRelativePath( const char *_szRootDir, char* szOut, const char *_szIn, BOOL bUnixStyle = TRUE )
{
	char szRootDir[MAX_PATH], szIn[MAX_PATH];;
	strcpy( szRootDir, _szRootDir );
	strcpy( szIn, _szIn );
	ChangePathStyle( szRootDir, TRUE );
	ChangePathStyle( szIn, TRUE );

	if( szRootDir[0] == 0 )
	{
		strcpy( szOut, szIn );
		ChangePathStyle( szOut, bUnixStyle );
		return FALSE;
	}
	else
	{
		if( szIn == NULL )
		{
			I3ASSERT_0;
			return FALSE;
		}
		else
		{
			int n = (int)strlen( szRootDir );
			if( strnicmp( szRootDir, szIn, n ) == 0 )
			{
				strcpy( szOut, szIn + n + 1 );
				ChangePathStyle( szOut, bUnixStyle );
				return TRUE;
			}
			else
			{
				strcpy( szOut, szIn );
				ChangePathStyle( szOut, bUnixStyle );
				return FALSE;
			}
		}
	}
}


int ConvertFormatCharToControlChar( wchar_t *szOut, const wchar_t *szIn )
{
	int nCount = 0;

	wchar_t *wCh = (wchar_t *)szIn;
	wchar_t *wOut = szOut;
	BOOL bControl = FALSE;

	while( *wCh != 0 )
	{
		if( bControl )
		{
			bControl = FALSE;
			switch( *wCh )
			{
			case 'n':	*wOut = 0x0A; break;	// 개행문자 처리
			default:	*wOut = *wCh; break;
			}
			wOut++;
			nCount++;
		}
		else
		{
			if( *wCh == '\\' )
			{
				bControl = TRUE;
			}
			else
			{
				*wOut = *wCh;
				wOut++;
				nCount++;
			}
		}
		wCh++;
	}

	*wOut = 0;

	return nCount;
}

//-----------------------------------------------------------------------------
// Name : INT32 FileReadLine(CFile * pFile, char * Line)
// Desc : 유니코드 파일에서 한줄 읽기.
//		  캐리지 리턴은 읽지 않고, 마지막 문자 0으로 만듬.
//-----------------------------------------------------------------------------
INT32 FileReadLineW( CFile * pFile, wchar_t * szLine, INT32 nMaxBuf )
{
	wchar_t		wCh;	
	INT32 nCount = 0;
	INT32 nReadCount = 0;
	szLine[0] = 0;

	while( pFile->Read(&wCh, 2) > 0 )
	{
		nReadCount++;
		if( wCh >= L' ' )
		{
			I3ASSERT( nCount < nMaxBuf );
			szLine[ nCount ] = wCh;
			nCount++;
		}
		else if( wCh == L'\n' )
		{
			break;
		}
		else if( wCh != L'\r' )
		{
			I3ASSERT_0;
		};
	}

	if( nReadCount <= 0 )
	{
		return -1;
	}

	I3ASSERT( nCount < nMaxBuf );
	szLine[ nCount ] = 0;
	return nCount; 
}

INT32 FileReadLine( CFile * pFile, char *szLine, INT32 nMaxBuf )
{
	char Ch;	
	INT32 nCount = 0;
	INT32 nReadCount = 0;
	szLine[0] = '\0';

	while( pFile->Read( &Ch, 1 ) > 0 )
	{
		nReadCount++;
		if( Ch >= ' ' )
		{
			I3ASSERT( nCount < nMaxBuf );
			szLine[ nCount ] = Ch;
			nCount++;
		}
		else if( Ch == '\n' )
		{
			break;
		}
		else if( Ch != '\r' )
		{
			I3ASSERT_0;
		}
	}

	if( nReadCount <= 0 )
	{
		return -1;
	}

	I3ASSERT( nCount < nMaxBuf );
	szLine[ nCount ] = 0;
	return nCount;
}

void ShowLastErrorMsg( HWND hWnd = NULL )
{
	LPVOID lpMsgBuf;
	if (!FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL ))
	{
		// Handle the error.

		// Display the string.
		MessageBox( hWnd, "Unknown", "Error", MB_OK | MB_ICONINFORMATION );
		return;
	}

	// Process any inserts in lpMsgBuf.
	// ...

	// Display the string.
	MessageBox( hWnd, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );

	// Free the buffer.
	LocalFree( lpMsgBuf );
}

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Ci3FontDlg 대화 상자

BOOL Ci3FontDlg::m_bTest = TRUE;

Ci3FontDlg::Ci3FontDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3FontDlg::IDD, pParent)
	, m_strTextFile(_T(""))
	, m_strLabelFile(_T(""))
	, m_strOutDir(_T(""))
	, m_strDictionaryFile(_T(""))
	, m_stri3FntFile(_T(""))
	, m_stri3FontFile(_T(""))
	, m_strSTRFile(_T(""))
	, m_bDumpDebugFiles(FALSE)
	, m_strName(_T("FONT_NONAME"))
	, m_strFontName(_T(""))
	, m_nFontSize(0)
	, m_nImageWidth(256)
	, m_nImageHeight(256)
	, m_nSpaceX(13)
	, m_nSpaceY(13)
	, m_nFontQuality(0)
	, m_strMediaDir(_T(""))
	, m_nImageFormat(0)
	, m_strTextDir(_T(""))
	, m_bBatchMode(FALSE)
{
	m_bNeedSave = FALSE;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	memset( &m_LogFont, 0, sizeof( LOGFONT ) );
	strcpy( m_LogFont.lfFaceName, "굴림" ); // 굴림 font for a default
	m_LogFont.lfHeight  = -( 10 * 96 ) / 72; // 16-pt font for a default
	m_LogFont.lfWeight  = 400;               // 400 = normal, 700 = bold, etc.
	m_LogFont.lfQuality = ANTIALIASED_QUALITY;
	m_nFontQuality		= ANTIALIASED_QUALITY;

	m_strProjectFile = "";

	m_bRelativePath = FALSE;
}

void Ci3FontDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TXTFILE, m_strTextFile);
	DDX_Text(pDX, IDC_EDIT_LABELFILE, m_strLabelFile);
	DDX_Text(pDX, IDC_EDIT_OUTDIR, m_strOutDir);
	DDX_Text(pDX, IDC_EDIT_DICFILE, m_strDictionaryFile);
	DDX_Text(pDX, IDC_EDIT_IMGINFOFILE, m_stri3FntFile);
	DDX_Text(pDX, IDC_EDIT_STRFILE, m_strSTRFile);
	DDX_Check(pDX, IDC_CHECK_DUMP, m_bDumpDebugFiles);
	DDX_Text(pDX, IDC_EDIT_NAME, m_strName);
	DDX_Text(pDX, IDC_EDIT_FONTNAME, m_strFontName);
	DDX_Text(pDX, IDC_EDIT_FONTSIZE, m_nFontSize);
	DDX_Text(pDX, IDC_EDIT_IMG_WIDTH, m_nImageWidth);
	DDX_Text(pDX, IDC_EDIT_IMG_HEIGHT, m_nImageHeight);
	DDX_Text(pDX, IDC_EDIT_SPACE_X, m_nSpaceX);
	DDX_Text(pDX, IDC_EDIT_SPACE_Y, m_nSpaceY);
	DDX_CBIndex(pDX, IDC_COMBO_QUALITY, m_nFontQuality);
	DDX_Text(pDX, IDC_EDIT_MEDIA_DIR, m_strMediaDir);
	DDX_CBIndex(pDX, IDC_CB_IMAGE_FORMAT, m_nImageFormat);
	DDX_Text(pDX, IDC_EDIT_TXT_DIR, m_strTextDir);
	DDX_Radio(pDX, IDC_RADIO_TXT1, m_bBatchMode);
}

BEGIN_MESSAGE_MAP(Ci3FontDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_TBLFILE, OnBnClickedButtonTblfile)
	ON_BN_CLICKED(IDC_BUTTON_TXTFILE, OnBnClickedButtonTxtfile)
	ON_BN_CLICKED(IDC_BUTTON_MAKEIMAGE, OnBnClickedButtonMakeimage)
	ON_BN_CLICKED(IDC_BUTTON_GEN_STR, OnBnClickedButtonGenSTR)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_OUTDIR, OnBnClickedButtonOutdir)
	ON_EN_UPDATE(IDC_EDIT_TXTFILE, OnEnUpdateEditTxtfile)
	ON_BN_CLICKED(IDC_BUTTON_OPENOUTDIR, OnBnClickedButtonOpenoutdir)
	ON_EN_CHANGE(IDC_EDIT_OUTDIR, OnEnChangeEditOutdir)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_OUTDIR, OnBnClickedButtonAutoOutdir)
	ON_BN_CLICKED(IDC_BUT_SELECT_FONT, OnBnClickedButSelectFont)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_BN_CLICKED(IDC_BUTTON_MEDIA_DIR, OnBnClickedButtonMediaDir)
	ON_EN_CHANGE(IDC_EDIT_MEDIA_DIR, OnEnChangeEditMediaDir)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_I3FONT, OnBnClickedButtonMakeI3font)
	ON_BN_CLICKED(IDC_BUTTON_TXT_DIR, OnBnClickedButtonTxtDir)
	ON_BN_CLICKED(IDC_RADIO_TXT2, OnBnClickedRadioTxt2)
	ON_BN_CLICKED(IDC_RADIO_TXT1, OnBnClickedRadioTxt1)
END_MESSAGE_MAP()


// Ci3FontDlg 메시지 처리기

BOOL Ci3FontDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	{
		i3Profile pf(this);

		pf.LoadWindowPos();

		m_strMediaDir = pf.GetString( "MediaDir", "" );

		ChangePathStyle( m_strMediaDir, TRUE );

		UpdateControls( FALSE );
	}

	CCommandLineInfo CmdInfo;
	AfxGetApp()->ParseCommandLine( CmdInfo );

	if( CmdInfo.m_strFileName.GetLength() > 0 )
	{
		if( ! LoadProject( LPCTSTR( CmdInfo.m_strFileName ) ) )
		{
			MessageBox( LPCTSTR( m_strError ), LPCTSTR( CmdInfo.m_strFileName ) );
			return FALSE;
		}

		SetProjectName( LPCTSTR( CmdInfo.m_strFileName ) );
		MakeAllToRelativePath();

		UpdateControls( FALSE );
	}
	
	m_pTable		= NULL;
	m_pLabelBuf		= NULL;
	m_pStringBuf	= NULL;

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void Ci3FontDlg::OnDestroy()
{
	CloseSTR();

	{
		UpdateData();

		i3Profile pf(this);

		pf.WriteString( "MediaDir", LPCTSTR( m_strMediaDir ) );
		
		pf.SaveWindowPos();
	}

	CDialog::OnDestroy();
}

// GUI Controls 업데이트
void Ci3FontDlg::UpdateControls( BOOL bUpdate )
{
	UpdateData( bUpdate );

	if( m_bBatchMode )
	{
		GetDlgItem( IDC_EDIT_TXT_DIR )->EnableWindow( TRUE );
		GetDlgItem( IDC_EDIT_TXTFILE )->EnableWindow( FALSE );
	}
	else
	{
		GetDlgItem( IDC_EDIT_TXT_DIR )->EnableWindow( FALSE );
		GetDlgItem( IDC_EDIT_TXTFILE )->EnableWindow( TRUE );
	}

	m_strFontName = m_LogFont.lfFaceName;
	//m_nFontSize =  ( ( ( -m_LogFont.lfHeight * 720 ) / 96 ) + 5 ) / 10;
	m_nFontSize  = MulDiv( -m_LogFont.lfHeight, 72, 96 );

	m_LogFont.lfQuality	= m_nFontQuality;

	SetOutputFileNames();

	UpdateData( FALSE );
}

void Ci3FontDlg::SetOutputFileNames()
{
	// 라벨 파일명
	char szLabel[256] = {0,};
	i3String::SwapExt( szLabel, (LPCTSTR)m_strTextFile, "lbl" );
	m_strLabelFile		= szLabel;

	// Text 파일명만 뽑아내기
	char szName[256] = {0,};
	i3String::SplitFileName( (LPCTSTR)m_strTextFile, szName, FALSE );

	char szDir[256] = {0,};
	strcpy( szDir, LPCTSTR( m_strTextFile ) );
	i3String::RemoveFileName( szDir );

	CString strDir;
	int LastChar = 0;
	if( m_strOutDir.GetLength() > 0 )
	{
		LastChar = m_strOutDir.GetAt( m_strOutDir.GetLength() - 1 );
	}

	if( LastChar != '\\' && LastChar != '/' )
	{
		strDir = m_strOutDir + "\\";
	}
	else
	{
		strDir = m_strOutDir;
	}

	m_strImageFileName	= strDir + szName + ".bmp";
	m_stri3FntFile		= strDir + szName + ".i3fnt";
	m_stri3FontFile		= strDir + szName + ".i3font";
	m_strDictionaryFile	= strDir + szName + ".dic";
	m_strTableFile		= strDir + szName + ".tbl";
	m_strSTRFile		= strDir + szName + ".str";
	m_strDumpLabelFile	= strDir + "_Dump_" + szName + ".lbl";
	m_strDumpTextFile	= strDir + "_Dump_" + szName + ".txt";
}

// 절대 경로로 바꿈
void Ci3FontDlg::MakeAllToAbsolutePath()
{
	if( ! m_bRelativePath )
	{
		return;
	}
	m_bRelativePath = FALSE;

	char szTemp[MAX_PATH];

	GetAbsolutePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strTextDir ) );
	m_strTextDir = szTemp;

	GetAbsolutePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strTextFile ) );
	m_strTextFile = szTemp;

	GetAbsolutePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strLabelFile ) );
	m_strLabelFile = szTemp;

	GetAbsolutePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strOutDir ) );
	m_strOutDir = szTemp;

	GetAbsolutePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strImageFileName ) );
	m_strImageFileName = szTemp;

	GetAbsolutePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_stri3FntFile ) );
	m_stri3FntFile = szTemp;

	GetAbsolutePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_stri3FontFile ) );
	m_stri3FontFile = szTemp;

	GetAbsolutePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strDictionaryFile ) );
	m_strDictionaryFile = szTemp;

	GetAbsolutePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strTableFile ) );
	m_strTableFile = szTemp;

	GetAbsolutePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strSTRFile ) );
	m_strSTRFile = szTemp;

	GetAbsolutePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strDumpLabelFile ) );
	m_strDumpLabelFile = szTemp;

	GetAbsolutePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strDumpTextFile ) );
	m_strDumpTextFile = szTemp;
}

// 상대 경로로 바꿈
void Ci3FontDlg::MakeAllToRelativePath()
{
	if( m_bRelativePath )
	{
		return;
	}

	m_bRelativePath = TRUE;

	char szTemp[MAX_PATH];

	GetRelativePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strTextDir ) );
	m_strTextDir = szTemp;
	
	GetRelativePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strTextFile ) );
	m_strTextFile = szTemp;

	GetRelativePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strLabelFile ) );
	m_strLabelFile = szTemp;

	GetRelativePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strOutDir ) );
	m_strOutDir = szTemp;

	GetRelativePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strImageFileName ) );
	m_strImageFileName = szTemp;

	GetRelativePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_stri3FntFile ) );
	m_stri3FntFile = szTemp;

	GetRelativePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_stri3FontFile ) );
	m_stri3FontFile = szTemp;

	GetRelativePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strDictionaryFile ) );
	m_strDictionaryFile = szTemp;

	GetRelativePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strTableFile ) );
	m_strTableFile = szTemp;

	GetRelativePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strSTRFile ) );
	m_strSTRFile = szTemp;

	GetRelativePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strDumpLabelFile ) );
	m_strDumpLabelFile = szTemp;

	GetRelativePath( LPCTSTR( m_strMediaDir ), szTemp, LPCTSTR( m_strDumpTextFile ) );
	m_strDumpTextFile = szTemp;
}

// 프로젝트 파일 불러오기
BOOL Ci3FontDlg::LoadProject( const char *szFileName )
{
	BOOL rv = FALSE; 
	i3IniParser Parser;
	char szBuf[256];
	int nTemp;

	if( ! Parser.OpenFromFile( szFileName ) )
	{
		m_strError.Format( "Failed to open file. (%s)", szFileName );
		return FALSE;
	}

	if( ! Parser.ReadSection( "Default" ) )
	{
		m_strError.Format( "Failed to read section [Font]" );
		goto ExitLabel;
	}

	// Get User Defined Name
	Parser.GetValue( "Name", "", szBuf );
	m_strName = szBuf;

	Parser.GetValue( "RelativePath", FALSE, &m_bRelativePath );

	Parser.GetValue( "BatchMode", FALSE, &m_bBatchMode );

	Parser.GetValue( "TextDir", "", szBuf );
	m_strTextDir = szBuf;

	Parser.GetValue( "TextPath", "", szBuf );
	m_strTextFile = szBuf;

	Parser.GetValue( "OutDir", "", szBuf );
	m_strOutDir = szBuf;

	// Get Log Font Info
	if( ! Parser.ReadSection( "Font" ) )
	{
		m_strError.Format( "Failed to read section [Font]" );
		goto ExitLabel;
	}

	Parser.GetValue( "CharSet", 0, &nTemp );
	m_LogFont.lfCharSet			= nTemp;

	Parser.GetValue( "ClipPrecision", 0, &nTemp );
	m_LogFont.lfClipPrecision	= nTemp;

	Parser.GetValue( "Escapement", 0, &nTemp );
	m_LogFont.lfEscapement		= nTemp;

	Parser.GetValue( "FaceName", "", szBuf );
	strcpy( m_LogFont.lfFaceName, szBuf );

	Parser.GetValue( "Height", 0, &nTemp );
	m_LogFont.lfHeight			= nTemp;

	Parser.GetValue( "Italic", 0, &nTemp );
	m_LogFont.lfItalic			= nTemp;

	Parser.GetValue( "Orientation", 0, &nTemp );
	m_LogFont.lfOrientation		= nTemp;

	Parser.GetValue( "OutPrecision", 0, &nTemp );
	m_LogFont.lfOutPrecision	= nTemp;

	Parser.GetValue( "PitchAndFamily", 0, &nTemp );
	m_LogFont.lfPitchAndFamily	= nTemp;

	Parser.GetValue( "Quality", 0, &nTemp );
	//m_LogFont.lfQuality			= nTemp;
	m_nFontQuality				= nTemp;

	Parser.GetValue( "StrikeOut", 0, &nTemp );
	m_LogFont.lfStrikeOut		= nTemp;

	Parser.GetValue( "Underline", 0, &nTemp );
	m_LogFont.lfUnderline		= nTemp;

	Parser.GetValue( "Weight", 0, &nTemp );
	m_LogFont.lfWeight			= nTemp;

	Parser.GetValue( "Width", 0, &nTemp );
	m_LogFont.lfWidth			= nTemp;

	// Get Image Info
	if( ! Parser.ReadSection( "Image" ) )
	{
		m_strError.Format( "Failed to read section [Font]" );
		goto ExitLabel;
	}

	Parser.GetValue( "Format", 0, &nTemp );
	m_nImageFormat	= nTemp;

	Parser.GetValue( "Width", 0, &nTemp );
	if( nTemp == 0 ) goto ExitLabel;
	m_nImageWidth	= nTemp;

	Parser.GetValue( "Height", 0, &nTemp );
	if( nTemp == 0 ) goto ExitLabel;
	m_nImageHeight	= nTemp;

	Parser.GetValue( "SpaceX", 0, &nTemp );
	if( nTemp == 0 ) goto ExitLabel;
	m_nSpaceX		= nTemp;

	Parser.GetValue( "SpaceY", 0, &nTemp );
	if( nTemp == 0 ) goto ExitLabel;
	m_nSpaceY		= nTemp;

	rv = TRUE; 
ExitLabel:
	if( rv == FALSE )
	{
		MessageBox("폰트 정보파일에 문제가 있습니다. 확인해주세요. "); 
	}

	return rv;
}

// 프로젝트 파일 저장하기
BOOL Ci3FontDlg::SaveProject( const char *szFileName )
{
	BOOL rv = FALSE; 
	i3IniParser Parser;

	if( ! Parser.Create( szFileName ) )
	{
		m_strError.Format( "Failed to create file. (%s)", szFileName );
		goto ExitLabel;
	}

	Parser.WriteSectionName( "Default" );

	// Write Default Info
	Parser.WriteValue( "Name", LPCTSTR( m_strName ) );

	Parser.WriteValue( "RelativePath", m_bRelativePath );

	Parser.WriteValue( "BatchMode", m_bBatchMode );

	Parser.WriteValue( "TextDir", LPCTSTR( m_strTextDir ) );

	Parser.WriteValue( "TextPath", LPCTSTR( m_strTextFile ) );

	Parser.WriteValue( "OutDir", LPCTSTR( m_strOutDir ) );

	// Write Log Font Info
	Parser.Write( NULL, "" );
	Parser.WriteSectionName( "Font" );

	Parser.WriteValue( "CharSet",			m_LogFont.lfCharSet			);
	Parser.WriteValue( "ClipPrecision",		m_LogFont.lfClipPrecision	);
	Parser.WriteValue( "Escapement",		m_LogFont.lfEscapement		);
	Parser.WriteValue( "FaceName",			m_LogFont.lfFaceName		);
	Parser.WriteValue( "Height",			m_LogFont.lfHeight			);
	Parser.WriteValue( "Italic",			m_LogFont.lfItalic			);
	Parser.WriteValue( "Orientation",		m_LogFont.lfOrientation		);
	Parser.WriteValue( "OutPrecision",		m_LogFont.lfOutPrecision	);
	Parser.WriteValue( "PitchAndFamily",	m_LogFont.lfPitchAndFamily	);
	//Parser.WriteValue( "Quality",			m_LogFont.lfQuality			);
	Parser.WriteValue( "Quality",			m_nFontQuality				);
	Parser.WriteValue( "StrikeOut",			m_LogFont.lfStrikeOut		);
	Parser.WriteValue( "Underline",			m_LogFont.lfUnderline		);
	Parser.WriteValue( "Weight",			m_LogFont.lfWeight			);
	Parser.WriteValue( "Width",				m_LogFont.lfWidth			);

	// Write Image Info
	Parser.Write( NULL, "" );
	Parser.WriteSectionName( "Image" );

	Parser.WriteValue( "Format",			m_nImageFormat				);
	Parser.WriteValue( "Width",				m_nImageWidth				);
	Parser.WriteValue( "Height",			m_nImageHeight				);
	Parser.WriteValue( "SpaceX",			m_nSpaceX					);
	Parser.WriteValue( "SpaceY",			m_nSpaceY					);

	rv = TRUE; 
ExitLabel:
	if( rv == FALSE )
	{
		MessageBox( "프로젝트 파일을 저장하지 못했습니다." );
	}

	return rv;
}

void Ci3FontDlg::OnBnClickedButtonAutoOutdir()
{
	UpdateControls();

	if( m_bBatchMode )
	{
		m_strOutDir = m_strTextDir;
	}
	else
	{
		if( m_strTextFile.GetLength() <= 0 ) return;

		char szTemp[MAX_PATH];
		strcpy( szTemp, LPCTSTR(m_strTextFile) );
		i3String::RemoveFileName( szTemp );
		m_strOutDir = szTemp;
	}

	UpdateControls( FALSE );
}

void Ci3FontDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void Ci3FontDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR Ci3FontDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Ci3FontDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void Ci3FontDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void Ci3FontDlg::OnBnClickedButtonTblfile()
{
	// TODO: Add your control notification handler code here	
}

void Ci3FontDlg::OnBnClickedButtonTxtfile()
{
	UpdateControls(); 

	// TODO: Add your control notification handler code here
	//Load String Text File 
	CFileDialog Dlg(TRUE,"TXT", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,
		"Font String File (*.txt) | *.TXT||", AfxGetMainWnd()); 

	if(Dlg.DoModal() != IDOK )
	{
		return ;
	}

	MakeAllToAbsolutePath();
	m_strTextFile = Dlg.GetPathName(); 
	MakeAllToRelativePath();

	UpdateControls(FALSE);
}

// 결과물 저장 폴더 설정
void Ci3FontDlg::OnBnClickedButtonOutdir()
{
	i3Win32FolderSelectDialog Dlg;

	if( Dlg.Execute( m_hWnd, "폴더를 선택하세요." ) == FALSE )
		return;

	UpdateControls();
	MakeAllToAbsolutePath();
	m_strOutDir = Dlg.szDir;
	MakeAllToRelativePath();
	UpdateControls( FALSE );
}

BOOL Ci3FontDlg::OnMakeImageCB( LPCTSTR szDir, LPCTSTR szFileName )
{
	BOOL bRet = FALSE;

	char szTxtFilename[MAX_PATH];
	sprintf( szTxtFilename, "%s/%s", szDir, szFileName );
	ChangePathStyle( szTxtFilename, TRUE );
	m_strTextFile = szTxtFilename;

	SetOutputFileNames();

	if( !MakeCharacterList() )
	{
		goto ExitLabel;
	}

	if( ! SaveImages() )
	{
		goto ExitLabel;
	}

	if( ! Savei3FontTextFile() )
	{
		goto ExitLabel;
	}

	bRet = TRUE;
ExitLabel:
    return bRet;
}

// =============================
// *.i3fnt, *.dic, 이미지 생성
// =============================
void Ci3FontDlg::OnBnClickedButtonMakeimage()
{
	BOOL bRet = FALSE;

	UpdateControls(TRUE); 	
	MakeAllToAbsolutePath();

	if( m_bBatchMode )
	{
		if( ! FindFiles( LPCTSTR( m_strTextDir ), "txt", MakeImageCB, this, FALSE ) )
		{
            goto ExitLabel;
		}
	}
	else
	{
		if( !MakeCharacterList() )
		{
			goto ExitLabel;
		}

		if( ! SaveImages() )
		{
			goto ExitLabel;
		}

		if( ! Savei3FontTextFile() )
		{
			goto ExitLabel;
		}
	}

	bRet = TRUE;
ExitLabel:
	MakeAllToRelativePath();
	if( bRet )
	{
		MessageBox( "Done." );
	}
	else
	{
		MessageBox( "Failed." );
	}
}

BOOL Ci3FontDlg::SaveImages()
{
	///////////////////////////////////////
	//이미지 & Dic 만들기 
	INT32	i;
	INT32	ImageCharCount = 0;		
	char	pBmpFileFolder[256];
	char	pBmpFileName[64];
	INT32	FileNum = 0;
	BITMAPINFO BMPI; 
	INT32	MaxCharCount, MaxCharHor, MaxCharVer; 		

	// 파일명만 가져오기
	i3String::SplitFileName( (LPCTSTR) m_strImageFileName, pBmpFileName, FALSE);

	// 경로만 가지고 오기
	strcpy(pBmpFileFolder, (LPCTSTR) m_strImageFileName);  
	i3String::RemoveFileName( pBmpFileFolder );

	///////////////////////////////////////
	//한장의 이미지파일에 들어갈수 있는 캐릭터수 계산하기 
	MaxCharHor = m_nImageWidth	/ m_nSpaceX; 
	MaxCharVer = m_nImageHeight	/ m_nSpaceY; 
	MaxCharCount = MaxCharHor * MaxCharVer; 
	MaxCharVer--; 
	MaxCharHor--; 

	///////////////////////////////////////
	//이미지 전처리
	HBITMAP		hBitmap, hOldBitmap;
	char	*	pImageBuf;
	HDC			hDC;		
	HBRUSH		hBrush;
	COLORREF	BackColor;
	COLORREF	TetxtColor;

	HFONT		hOldFont, m_hFont;
	CRect		Rect;

	hBitmap =  NULL; 
	//Set Font 

	m_hFont = CreateFontIndirect( &m_LogFont);
	BackColor = RGB( 0, 0, 0);
	TetxtColor = RGB( 255, 255, 255);

	//Dc
	{
		hDC = CreateCompatibleDC( NULL);
		hOldBitmap = (HBITMAP) SelectObject( hDC, NULL);
		SetBkMode( hDC, TRANSPARENT);
		SetTextColor( hDC, RGB( 255, 255, 255));
	}

	//Bitmap
	{			
		memset( &BMPI, 0, sizeof(BITMAPINFO));
		BMPI.bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
		BMPI.bmiHeader.biHeight			= m_nImageHeight;
		BMPI.bmiHeader.biWidth			= m_nImageWidth;
		BMPI.bmiHeader.biPlanes			= 1;
		BMPI.bmiHeader.biBitCount		= 24;
		BMPI.bmiHeader.biCompression	= BI_RGB;
	}

	//Font
	{
		hOldFont = (HFONT) SelectObject( hDC, m_hFont);
	}

	//Brush
	{			
		hBrush = CreateSolidBrush( BackColor);
	}

	memset( m_GlyphWidth, 0, sizeof(m_GlyphWidth) );

	/////////////////////////////////////
	//이미지 저장 합니다. 
	INT32 AccCharCount, AccCharhor, AccCharVer; 
	AccCharCount = AccCharhor = AccCharVer = 0; 
	//int tt; 	
	char ImageFileName[256]; 

	m_NumSavedImage = 0;
	m_DefaultGlyphHeight = 0;
	m_DefaultGlyphWidth = 0;
	for(i = 0; i < CHARSETNUM; i++)
	{			
		if(m_CharList[i] == TRUE)
		{
			wchar_t wCh[2] = { i, 0 };

			if(AccCharCount == 0)
			{
				//For Make File Name
				sprintf( ImageFileName,"%s/%s%d.bmp", pBmpFileFolder, pBmpFileName, FileNum); 
				FileNum++; 

				AccCharhor = AccCharVer = 0; 

				hBitmap = CreateDIBSection( hDC, &BMPI, DIB_RGB_COLORS, (void **)&pImageBuf, NULL, 0);
				SelectObject( hDC, hBitmap);
				//SetBkMode
				SelectObject( hDC, hBitmap);
				// Background를 지운다.
				{
					CRect ARect( 0, 0, m_nImageWidth, m_nImageHeight);
					FillRect( hDC, &ARect, hBrush);
				}
				// Text Color
				SetTextColor( hDC, TetxtColor);
			}

			Rect.left	= AccCharhor	* m_nSpaceX;
			Rect.top	= AccCharVer	* m_nSpaceY;
			Rect.right	= Rect.left		+ m_nSpaceX;
			Rect.bottom = Rect.top		+ m_nSpaceY;

			// 글자 그리기
			DrawTextW( hDC, wCh, 1, &Rect, 	DT_NOPREFIX | DT_SINGLELINE);

			// 글자의 폭 구하기
			SIZE Size;
			GetTextExtentPoint32W( hDC, wCh, 1, &Size );

			// Get char width a different way
			int charwidth;
			GetCharWidth32( hDC, i, i, &charwidth );

			ABC abc;
			if( GetCharABCWidthsW( hDC, i, i, &abc ) )
			{
				m_GlyphWidth[i] = abc.abcA + abc.abcB + abc.abcC;
			}
			else
			{
				m_GlyphWidth[i] = (UINT8)Size.cx;
			}

			// 기본 글자폭 크기
			if( m_GlyphWidth[i] > m_DefaultGlyphWidth )
			{
				m_DefaultGlyphWidth = m_GlyphWidth[i];
			}

			// Height
			if( m_DefaultGlyphHeight == 0 )
			{
				m_DefaultGlyphHeight = (UINT8)Size.cy;
			}
			else
			{
				I3ASSERT( m_DefaultGlyphHeight == Size.cy );
			}

			//For Dic File 
			ImageCharCount++; 

			//For Enter 
			//tt = 0x000d; 
			//DicFile.Write(&tt, 2); 
			//tt = 0x000a; 
			//DicFile.Write(&tt, 2); 
			
			if (MaxCharHor == AccCharhor )	
			{
				AccCharhor = 0; 
				AccCharVer++; 
			}
			else 
			{
				AccCharhor++; 
			}

			AccCharCount++; 

			//For Bitmap Image                
			if(AccCharCount == MaxCharCount)
			{
				//저장한다.				
				BOOL bExported = ExportImage( ImageFileName, pImageBuf );

				SelectObject( hDC, hOldBitmap);
				DeleteObject( hBitmap);
				hBitmap = NULL;

				if( !bExported )
				{
					goto Error;
				}

				AccCharCount = 0; 

				m_NumSavedImage++;
			}
		}


	}//For

	if(hBitmap != NULL)
	{
		//저장한다. 
		//SaveBMP( ImageFileName, pImageBuf);
		BOOL bExported = ExportImage( ImageFileName, pImageBuf );
		SelectObject( hDC, hOldBitmap);
		DeleteObject( hBitmap);
		DeleteDC( hDC );
		hBitmap = NULL;

		if( !bExported )
		{
			goto Error;
		}

		m_NumSavedImage++;
	}

	////////////////////////////////////////
	//Dic 파일을 저장합니다. 
	{
		CFile DicFile;
		if( ! DicFile.Open( (LPCTSTR)m_strDictionaryFile, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate, NULL ) )
		{
			MessageBox("Dictionary File을 생성할 수 없습니다." );
			goto Error;
		}

		I3_FONT_DICTIONARY_HEADER Header;
		strncpy( Header.SIG, I3_SIG_FONT_DICTIONARY, 4 );
		Header.nMajorVersion = 1;
		Header.nMinorVersion = 0;

		DicFile.Write( &Header, sizeof(Header) );

		INT32 nNumChar = 0;
		for(i = 0; i < CHARSETNUM; i++)
		{			
			if(m_CharList[i] == TRUE)
			{			
				nNumChar++;
			}
		}

		DicFile.Write( &nNumChar, sizeof(INT32) );

		for(i = 0; i < CHARSETNUM; i++)
		{			
			if(m_CharList[i] == TRUE)
			{
				DicFile.Write( &i, 2 ); 
			}
		}
		DicFile.Close(); 
	}

	return TRUE;
Error:
    return FALSE;
}

BOOL Ci3FontDlg::Savei3FontTextFile()
{
	i3IniParser Parser;
	if( ! Parser.Create( LPCTSTR( m_stri3FntFile ) ) )
	{
		MessageBox( "Could not create Image Info file." );
		goto Error;
	}

	Parser.WriteSectionName( "ImageInfo" );
	Parser.WriteValue( "Name", LPCTSTR( m_strName ) );
	Parser.WriteValue( "NumImage", m_NumSavedImage );
	Parser.WriteValue( "Width", m_nImageWidth );
	Parser.WriteValue( "Height", m_nImageHeight );
	Parser.WriteValue( "SpaceX", m_nSpaceY );
	Parser.WriteValue( "SpaceY", m_nSpaceY );

	Parser.Write( NULL, "" );
	Parser.WriteSectionName( "GlyphWidth" );
	Parser.WriteValue( "DefaultWidth", m_DefaultGlyphWidth );
	Parser.WriteValue( "DefaultHeight", m_DefaultGlyphHeight );
	for( int i = 0; i < 128; i++)
	{			
		if(m_CharList[i] == TRUE)
		{
			if( m_GlyphWidth[i] > 0 && m_GlyphWidth[i] != m_DefaultGlyphWidth )
			{
				char szLabel[32];
				sprintf( szLabel, "%0.4X", i );
				Parser.WriteValue( szLabel, m_GlyphWidth[i] );                
			}
		}
	}

	Parser.Close();
	return TRUE;
Error:
	return FALSE;
}

//Make Character List 
BOOL Ci3FontDlg::MakeCharacterList(void)
{
	CFile hFile; 

	//Open Text File
	if ( !hFile.Open(m_strTextFile, CFile::modeRead | CFile::shareDenyWrite, NULL) )
	{
		MessageBox("String File을 로드할수 없습니다. "); 
		return FALSE; 
	}

	UINT16 Header; 
	hFile.Read(&Header, 2); 
	if(Header != 0xfeff)
	{
		MessageBox("UniCode Txt만 지원합니다."); 
		hFile.Close(); 
		return FALSE; 
	}

	//Init Charicter List
	memset( m_CharList, 0, sizeof(BOOL) * CHARSETNUM );

	wchar_t wCh;
	BOOL bControl = FALSE;
	while( hFile.Read( &wCh, 2) > 0 )
	{
		if( wCh >= L' ' )
		{
			if( bControl )
			{
				bControl = FALSE;
				switch( wCh )
				{
				case L'n': break;
				default: m_CharList[ wCh ] = TRUE; break;
				}
			}
			else
			{
				if( wCh == L'\\' )
				{
					bControl = TRUE;
				}
				else
				{
					m_CharList[ wCh ] = TRUE;
				}
			}
		}
		else
		{
			bControl = FALSE;
			if( ( wCh != L'\r' ) && ( wCh != L'\n' ) )
			{
				MessageBox( "Control character found.", "Error" );
				hFile.Close();
				return FALSE;
			}
		}
	}

	// 기본적으로 스페이스바는 무조건 넣는다.
	m_CharList[ L' ' ] = TRUE;

	//Close Text File
	hFile.Close(); 
	return TRUE; 
}

//Save Image File 
void Ci3FontDlg::SaveBMP( LPCTSTR pszFileName, char * pImageBuf)
{
	//char szFull[MAX_PATH];
	BOOL Rv;
	HANDLE hFile = NULL;
	BITMAPFILEHEADER	FHeader;
	BITMAPINFOHEADER	BHeader;
	DWORD				writeSz;
	int sz;


	hFile = CreateFile( pszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if( hFile == INVALID_HANDLE_VALUE)
	{
		MessageBox( "Bitmap 파일을 생성할 수 없습니다", "Error", MB_OK);
		return;
	}

	sz = m_nImageWidth * 3;
	sz = ((sz + 3) >> 2) << 2;
	sz *= m_nImageHeight;


	{
		memcpy( &FHeader.bfType, "BM", 2);
		FHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sz;
		FHeader.bfReserved1 = 0;
		FHeader.bfReserved2 = 0;
		FHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		Rv = WriteFile( hFile, &FHeader, sizeof(BITMAPFILEHEADER), &writeSz, NULL);
		if((Rv == FALSE) || (writeSz != sizeof(BITMAPFILEHEADER)))
		{
//			Debug( AfxGetMainWnd()->m_hWnd, "Could not write Bitmap file header to %s file.", szFull);
			goto ExitPart;
		}
	}

	{
		memset( &BHeader, 0, sizeof(BITMAPINFOHEADER));
		BHeader.biSize = sizeof(BITMAPINFOHEADER);
		BHeader.biWidth = m_nImageWidth;
		BHeader.biHeight = m_nImageHeight;
		BHeader.biPlanes = 1;
		BHeader.biBitCount = 24;
		BHeader.biCompression = BI_RGB;

		Rv = WriteFile( hFile, &BHeader, sizeof(BITMAPINFOHEADER), &writeSz, NULL);
		if((Rv == FALSE) || (writeSz != sizeof(BITMAPINFOHEADER)))
		{
//			Debug( AfxGetMainWnd()->m_hWnd, "Could not write Bitmap information to %s file.", szFull);
			goto ExitPart;
		}
	}

	Rv = WriteFile( hFile, pImageBuf, sz, &writeSz, NULL);
	if((Rv == FALSE) || (writeSz != sz))
	{
//		Debug( AfxGetMainWnd()->m_hWnd, "Could not write bitmap data to %s file.", szFull);
		goto ExitPart;
	}

ExitPart:
	if( hFile != NULL)
	{
		CloseHandle( hFile);
	}
}

// 
void Ci3FontDlg::ConvertImage24To32( RGBQuad *pOut, RGBTripple *pIn, BOOL bWhiteRGB )
{
	RGBTripple	*pSrc = pIn;

	for( int y = 0; y < m_nImageHeight; y++ )
	{
		for( int x = 0; x < m_nImageWidth; x++ )
		{
			// get RGB component
			BYTE R = pSrc->red;
			BYTE G = pSrc->grn;
			BYTE B = pSrc->blu;

			// calculate alpha value
			BYTE A = (R+G+B)/3;

			if( bWhiteRGB )
			{
				pOut->red   = 255;
				pOut->grn   = 255;
				pOut->blu   = 255;
				pOut->alpha = A; 
			}
			else
			{
				pOut->red   = R;
				pOut->grn   = G;
				pOut->blu   = B;
				pOut->alpha = A; 
			}

			pSrc++;
			pOut++;
		}	
	}
}

//24비트 이미지를 8비트 인덱스드 그레이 스케일 이미지로 변환
void Ci3FontDlg::ConvertImage24ToIndexed8( RGBIndexed * pOut , RGBTripple *pIn )
{
	UINT8 R,G,B;
	//Set Color to Gray Index
	for( int y = 0; y < m_nImageHeight; y++ )
	{
		for( int x = 0; x < m_nImageWidth; x++ )
		{
			// get RGB component
			R = pIn->red;
			G = pIn->grn;
			B = pIn->blu;

			// Gray Scale로 변환할때의 황금비율
			*pOut = ((R*30) + (G*59) + (B*11)) /100;

			pIn++;
			pOut++;
		}
	}
}

void Ci3FontDlg::SetClut4FromIndexed8( RGBIndexed * pIn , RGBIndexed * pOutClut )
{
	INT32 nIndexLevelCount[256];
	memset( nIndexLevelCount, 0, sizeof(nIndexLevelCount) );
	RGBIndexed * pSrc	= pIn;

	//그레이 스케일에서 가진 색상값의 빈도를 카운트 한다.
	for( int y = 0; y < m_nImageHeight; y++ )
	{
		for( int x = 0; x < m_nImageWidth; x++ )
		{
			nIndexLevelCount[*pSrc]++;
			pSrc++;
		}
	}

	//빈도가 0이상인것들을 가려낸다.
	i3List	pCountList;
	for( INT32	i = 0 ; i < 256 ; i++ )
	{
		if( nIndexLevelCount[i] > 0 )
		{
			INDEXCOUNT_DATA * nData = new INDEXCOUNT_DATA;
			nData->nIndex = i;
			nData->nCount = nIndexLevelCount[i];
			pCountList.Add(nData);
		}
	}
	
	//최적화 계산 필요///////////////////////////////////////////////////////////////////
	//리스트에 들어있는 갯수가 16을 초과하는경우 빈도가 낮은 순으로 제거한다.
	if( pCountList.GetCount() > 16 )
	{
		INT32	ListCount = pCountList.GetCount();
 		while( ListCount > 16 )
		{
			INT32				nMinIndex = 0;
			INT32				nFrontLength = 0;
			INDEXCOUNT_DATA *	MinData = (INDEXCOUNT_DATA*)pCountList.GetItem(0);
			INDEXCOUNT_DATA *	pData	= NULL;
			I3ASSERT( MinData != NULL );

			//제일 낮은 빈도의 것을 고른다.
			for( INT32	i = 1 ;i < ListCount ; i++ )
			{
				pData = (INDEXCOUNT_DATA*)pCountList.GetItem( i );
				if( pData )
				{
					if( MinData->nCount > pData->nCount )
					{
						MinData = pData;
						nMinIndex = i;
					}
				}
			}
			
			pData = (INDEXCOUNT_DATA*)pCountList.GetItem(nMinIndex);
			I3_SAFE_DELETE( pData );
			pCountList.Delete(nMinIndex);
			ListCount = pCountList.GetCount();
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////

	memset( pOutClut , 0 , 16 );
	for( INT32	i = 0 ;i < 16 ; i++ )
	{
		if(pCountList.GetCount() > i)
		{
			INDEXCOUNT_DATA * pData = (INDEXCOUNT_DATA*)pCountList.GetItem(i);
			if( pData )	pOutClut[i] = pData->nIndex;
			else
			{	
				//사용된 색상이 16개 이하인경우 마지막에 선택된 색상으로 
				if( i != 0 )	pOutClut[i] = pOutClut[i-1];
				else
				{
					//일어날리 없겠지만.
					pOutClut[i] = 0;
				}
			}

			pData = NULL;
		}
		else
		{	
			//사용된 색상이 16개 이하인경우 마지막에 선택된 색상으로 
			if( i != 0 )	pOutClut[i] = pOutClut[i-1];
			else
			{
				//일어날리 없겠지만.
				pOutClut[i] = 0;
			}
		}
	}

	// RELEASE
	pCountList.SafeDeleteAll<INDEXCOUNT_DATA>();
}

//주어진 4bit Clut인덱스중에서 8bit Clut인덱스가 위치하는곳을 리턴
UINT8 Ci3FontDlg::GetIndexAtClut4( RGBIndexed * pOutClut , UINT8	nIndexAt8 )
{
	UINT8	nPostIndex = 0;//pOutClut[i]가 256레벨에서의 인덱스보다 큰 값을 갖는 최초의 인덱스를 담는다.

	//테이블중에 맞는 인덱스가 있는지 확인한다.
	for( UINT8	i = 0 ; i < 16 ; i++ )
	{
		if( pOutClut[i] == nIndexAt8 )
			return i;

		if( pOutClut[i] > nIndexAt8 )
		{
			nPostIndex = i;
			i = 17;// exit
		}
	}

	//일치하는 값이 없는경우 좌,우에있는 인덱스들과의 거리를 계산하여 가까운곳의 인덱스를 리턴
	if( nPostIndex <= 1 )	return 0;
	else
	{	
		if( (pOutClut[nPostIndex] - nIndexAt8) > nIndexAt8 - (pOutClut[nPostIndex-2]) )
		{
			return nPostIndex;
		}
		else
		{
			return nPostIndex-2;
		}
	}
}
//
void Ci3FontDlg::ConvertIndexed8ToIndexed4( RGBIndexed * pOut , RGBIndexed * pOutClut , RGBIndexed *pIn )
{
	UINT8 LEVEL1,LEVEL2;
	RGBIndexed * pSrc = pIn;
	SetClut4FromIndexed8(pIn, pOutClut );

	pSrc	= pIn;
	INT32	nWidth = (INT32)( m_nImageWidth * 0.5f );//홀수 픽셀인경우에대한 처리
	//Set Color to Gray Index
	for( int y = 0; y < m_nImageHeight; y++ )
	{
		for( int x = 0; x < nWidth; x++ )
		{
			LEVEL1 = GetIndexAtClut4(pOutClut , *pSrc);

			pSrc++;
			LEVEL2 = GetIndexAtClut4(pOutClut , *pSrc);
			*pOut =( (LEVEL2 << 4) | LEVEL1 );

			pSrc++;
			pOut++;
		}

		if( m_nImageWidth%2 )//홀수 픽셀인 이미지인경우에 대한 처리
		{
			*pOut = GetIndexAtClut4(pOutClut , *pSrc);

			pSrc++;
			pOut++;
		}
	}
}

BOOL Ci3FontDlg::ExportImage4Gray( const char *szFileName , RGBIndexed *pIn ,RGBIndexed * pClut )
{
	UINT8				*pDest;
	RGBIndexed			*pSrc = pIn;
	i3Clut				*pDestClut;
	INT32				i;
	INT32				RetValu = TRUE;
	INT32				nWidth;

	//Create Tex
	i3Texture * pDestTex = i3Texture::NewObject();
	pDestTex->Create( m_nImageWidth, m_nImageHeight, 1, I3G_IMAGE_FORMAT_I4, 0);

	//Create Clut
	pDestClut = i3Clut::NewObject();
	pDestClut->Create( 16 );

	//Set Clut
	for( i = 0; i < 16; i++)
	{
		COLOR col;
		i3Color::Set( &col , 255, 255, 255, pClut[i]);
		pDestClut->SetColor( i, &col);
	}

	pDestTex->SetClut( 0, pDestClut);

	//Set Color to Gray Index
	pDest = (UINT8*)pDestTex->Lock(0);

	if( m_nImageWidth%2 ) nWidth = (INT32)( m_nImageWidth * 0.5f ) +1;
	else				  nWidth = (INT32)( m_nImageWidth * 0.5f );

	for( int y = m_nImageHeight - 1; y > 0 ; y-- )
	{
		pSrc = pIn + ( y * nWidth );
		for( int x = 0; x < nWidth; x++ )
		{
			*pDest = *pSrc;

			pSrc++;
			pDest++;
		}	
	}
	pDestTex->Unlock();

	//Save File
	i3ImageFile file;
	i3OptI3Image OptImg;
	char szOutPath[MAX_PATH];

	if( m_nImageFormat == IMAGE_FMT_GRAY_4BPP_PSP )
	{
		i3OptPSPSwizzledImage opt;
		pDestTex = opt.MakeSwizzledImage( pDestTex );
	}
	
	strcpy( szOutPath, szFileName );
 	i3String::RemoveExt(szOutPath);

	strcat( szOutPath , ".i3i" );
	if( !file.SaveI3I(szOutPath, pDestTex) )
	{
		RetValu = FALSE;
	}

	I3_SAFE_RELEASE( pDestClut );
	I3_SAFE_RELEASE( pDestTex );

	return RetValu;
}
// Export Image
BOOL Ci3FontDlg::ExportImage( const char *szFileName, void *pImageBuffer )
{
	RGBQuad *pBuffer32 = NULL;
	RGBIndexed * pBufferIndexed8 = NULL;//8BIT INDEXED
	RGBIndexed * pBufferIndexed4 = NULL;//4BIT INDEXED
	RGBIndexed * pBufferClut	 = NULL;//4BIT CLUT
	char szTGAFileName[MAX_PATH];
	i3String::SwapExt( szTGAFileName, szFileName, "tga" );
	INT32	nWidth;

	int ret = 0;

	switch( m_nImageFormat )
	{
	case IMAGE_FMT_WHITE_32BPP_TGA:
		{
			pBuffer32 = new RGBQuad[ m_nImageWidth * m_nImageHeight ];
			ConvertImage24To32( pBuffer32, (RGBTripple*)pImageBuffer, TRUE );

			// 32 bpp TGA
			ret = ExportImage32( pBuffer32, m_nImageWidth, m_nImageHeight, 0, szTGAFileName, 0 );
		}
		break;

	case IMAGE_FMT_32BPP_TGA:
		{
			pBuffer32 = new RGBQuad[ m_nImageWidth * m_nImageHeight ];
			ConvertImage24To32( pBuffer32, (RGBTripple*)pImageBuffer, FALSE );

			// 32 bpp TGA
			ret = ExportImage32( pBuffer32, m_nImageWidth, m_nImageHeight, 0, szTGAFileName, 0 );
		}
		break;

	case IMAGE_FMT_24BPP_TGA:
		{
			// 24 bpp TGA
			ret = ExportImage24( (RGBTripple*)pImageBuffer, m_nImageWidth, m_nImageHeight, 0, szTGAFileName, 0 );
		}
		break;
	case IMAGE_FMT_GRAY_4BPP_PSP:
		{
			//8TO4의 변환에서 1BYTE에 2PIXEL의 정보가 들어가게되어 라인의 길이가 홀수인 경우에대해 처리해주어야함.
			if( m_nImageWidth%2 )	nWidth = (INT32)(m_nImageWidth * 0.5f) + 1;
			else					nWidth = (INT32)(m_nImageWidth * 0.5f);

			pBufferIndexed8 = new RGBIndexed[m_nImageWidth * m_nImageHeight];
			pBufferIndexed4 = new RGBIndexed[nWidth * m_nImageHeight];
			pBufferClut		= new RGBIndexed[16];

			ConvertImage24ToIndexed8( pBufferIndexed8 , (RGBTripple*)pImageBuffer );
			ConvertIndexed8ToIndexed4( pBufferIndexed4 , pBufferClut , pBufferIndexed8 );
			
			ret = ExportImage4Gray( szTGAFileName , pBufferIndexed4 , pBufferClut);
		}
		break;
	case IMAGE_FMT_GRAY_4BPP:
		{
			if( m_nImageWidth%2 )	nWidth = (INT32)(m_nImageWidth * 0.5f) + 1;
			else					nWidth = (INT32)(m_nImageWidth * 0.5f);

			pBufferIndexed8 = new RGBIndexed[m_nImageWidth * m_nImageHeight];
			pBufferIndexed4 = new RGBIndexed[nWidth * m_nImageHeight];
			pBufferClut		= new RGBIndexed[16];

			ConvertImage24ToIndexed8( pBufferIndexed8 , (RGBTripple*)pImageBuffer );
			ConvertIndexed8ToIndexed4( pBufferIndexed4 , pBufferClut , pBufferIndexed8 );

			ret = ExportImage4Gray( szTGAFileName , pBufferIndexed4 , pBufferClut );
		}
		break;
	}

	I3_SAFE_DELETE_ARRAY(pBufferIndexed8);
	I3_SAFE_DELETE_ARRAY(pBufferIndexed4);
	I3_SAFE_DELETE_ARRAY(pBufferClut);
	I3_SAFE_DELETE_ARRAY( pBuffer32 );

	if( ! ret )
	{
		m_strError.Format( "Failed to export image. \"%s\"", szTGAFileName );
		MessageBox( LPCTSTR( m_strError ), "Error" );
	}
		
	return ( ret != 0 );
}

// Text 파일이 바뀌는 경우 처리
void Ci3FontDlg::OnEnUpdateEditTxtfile()
{
	MakeAllToAbsolutePath();
	MakeAllToRelativePath();
	UpdateControls( TRUE );
}
// Output Dir이 바뀌는 경우 처리
void Ci3FontDlg::OnEnChangeEditOutdir()
{
	MakeAllToAbsolutePath();
	MakeAllToRelativePath();
	UpdateControls( TRUE );
}

// 저장된 디렉토리 탐색기로 열기
void Ci3FontDlg::OnBnClickedButtonOpenoutdir()
{
	char szFloder[MAX_PATH];
	GetAbsolutePath( LPCTSTR( m_strMediaDir ), szFloder, LPCTSTR( m_strOutDir), FALSE );

	char szCmd[1024];

	sprintf( szCmd, "Explorer \"%s\"", szFloder );

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );

	// Start the child process. 
	if(
		!CreateProcess
		(
			NULL,	// No module name (use command line). 
			szCmd,	// Command line. 
			NULL,	// Process handle not inheritable. 
			NULL,	// Thread handle not inheritable. 
			FALSE,	// Set handle inheritance to FALSE. 
			0,		// No creation flags. 
			NULL,	// Use parent's environment block. 
			NULL,	// Use parent's starting directory. 
			&si,	// Pointer to STARTUPINFO structure.
			&pi		// Pointer to PROCESS_INFORMATION structure.
		)
	) 
	{
		return;
		//ErrorExit( "CreateProcess failed." );
	}
}

BOOL Ci3FontDlg::OnMakeStrCB( LPCTSTR szDir, LPCTSTR szFileName )
{
	BOOL bRet = FALSE;

	char szTxtFilename[MAX_PATH];
	sprintf( szTxtFilename, "%s/%s", szDir, szFileName );
	ChangePathStyle( szTxtFilename, TRUE );
	m_strTextFile = szTxtFilename;

	SetOutputFileNames();

	BOOL bCheckDictionary = TRUE;

	if( ! LoadDictionary() )
	{
		int nID = MessageBox( "Dictionary is not available. Continue anyway?", "Warning", MB_YESNO );
		if( nID == IDNO )
		{
			goto ExitLabel;
		}
		bCheckDictionary = FALSE;
	}

	if( ! SaveSTR( bCheckDictionary ) )
	{
		goto ExitLabel;
	}

	// 디버깅용
	if( m_bDumpDebugFiles )
	{
		ConvertSTRtoLabelnUniText();
	}

	bRet = TRUE;
ExitLabel:
	return bRet;
}

// =============================
// Str 생성.
// =============================
void Ci3FontDlg::OnBnClickedButtonGenSTR()
{
	BOOL bRet = FALSE;
	UpdateControls();
	MakeAllToAbsolutePath();

	if( m_bBatchMode )
	{
		if( ! FindFiles( LPCTSTR( m_strTextDir ), "txt", MakeStrCB, this, FALSE ) )
		{
			goto ExitLabel;
		}
	}
	else
	{
		BOOL bCheckDictionary = TRUE;

		if( ! LoadDictionary() )
		{
			int nID = MessageBox( "Dictionary is not available. Continue anyway?", "Warning", MB_YESNO );
			if( nID == IDNO )
			{
				goto ExitLabel;
			}
			bCheckDictionary = FALSE;
		}

		if( ! SaveSTR( bCheckDictionary ) )
		{
			goto ExitLabel;
		}

		// 디버깅용
		if( m_bDumpDebugFiles )
		{
			ConvertSTRtoLabelnUniText();
		}
	}

	bRet = TRUE;
	MessageBox( "Done." );
ExitLabel:
	MakeAllToRelativePath();
	if( ! bRet )
	{
		MessageBox( "Failed." );
	}
}
// DIctionary File 읽기
BOOL Ci3FontDlg::LoadDictionary(void)
{
	CFile hFile; 

	//Open Text File
	if ( !hFile.Open( m_strDictionaryFile, CFile::modeRead | CFile::shareDenyWrite, NULL) )
	{
		MessageBox( "Dictionary File을 로드할수 없습니다." ); 
		return FALSE; 
	}

	I3_FONT_DICTIONARY_HEADER Header;
	hFile.Read(&Header, sizeof(Header) ); 

	if( strncmp( Header.SIG, I3_SIG_FONT_DICTIONARY, 4 ) != 0 )
	{
		MessageBox( "i3 Font Dictionary 파일이 아닙니다." ); 
		hFile.Close(); 
		return FALSE; 
	}

	if( Header.nMajorVersion != 1 || Header.nMinorVersion != 0 )
	{
		MessageBox( "지원하지 않는 i3 Font Dictionary 버전입니다." ); 
		hFile.Close(); 
		return FALSE; 
	}

	hFile.Read( &m_nNumChar, sizeof(INT32) );

	INT32 i = 0;
	memset(	m_Dictionary, 0, sizeof(m_Dictionary) );
	for( i = 0; i < m_nNumChar; i++ )
	{
		wchar_t wCh;
		if( hFile.Read( &wCh, 2 ) < 2 )
		{
			MessageBox( "i3 Font Dictionary 읽기 실패." ); 
			hFile.Close(); 
			return FALSE; 
		}
        m_Dictionary[ wCh ] = i + 1;
		m_RevDictionary[ i + 1 ] = wCh;
	}

	//Close Text File
	hFile.Close(); 

	return TRUE;
}
// *.str 저장하기
BOOL Ci3FontDlg::SaveSTR( BOOL bCheckDictionary )
{
	BOOL bRet = FALSE;

	CFile TextFile;
	CFile LabelFile;
	CFile STRFile;

	char szBuf[1024];
	wchar_t szUniBuf[1024];
	wchar_t UniHeader;
	ULONGLONG FileOffset;
	UINT32 nIndex;

	char szHead[10] = "ST";
	UINT16 nVersion = 1;
	STRING_INFO *pStrInfo = NULL;
	UINT32 nNumLines = 0;
	UINT32 nTotalLabelSize = 0;
	UINT32 nTotalStringSize = 0;

	// Open Text File
	if ( !TextFile.Open( m_strTextFile, CFile::modeRead | CFile::shareDenyWrite, NULL) )
	{
		MessageBox( "String File을 로드할수 없습니다." ); 
		goto Error;
	}

	// Open Label File
	if ( !LabelFile.Open( m_strLabelFile, CFile::modeRead | CFile::shareDenyWrite, NULL) )
	{
		MessageBox( "Label File을 로드할수 없습니다." ); 
		goto Error;
	}

	// Create STR File
	if ( !STRFile.Open( m_strSTRFile, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate, NULL) )
	{
		MessageBox( "STR File을 생성할수 없습니다." ); 
		goto Error;
	}

	// Count Lines
	{	
		UINT32 nTemp = 0;
		// Count Labels Lines
		while( FileReadLine( &LabelFile, szBuf, 1024 ) >= 0 )
		{
			nTemp++;
		}
		// Reset to begin
		LabelFile.SeekToBegin();

		// Read Header
		TextFile.Read( &UniHeader, 2 );
		if( UniHeader != 0xfeff )
		{
			MessageBox( "UniCode Text만 지원합니다." );
			goto Error;
		}

		// Count Text Lines
		while( FileReadLineW( &TextFile, szUniBuf, 1024 ) >= 0 )
		{
			nNumLines++;
		}
		// Reset to begin
		TextFile.SeekToBegin();

		// Compare Number of Lines
		if( nTemp != nNumLines )
		{
			MessageBox( "Label과 Text의 줄 수가 틀립니다." );
			goto Error;
		}
	}

	pStrInfo = new STRING_INFO[nNumLines];
	memset( pStrInfo, 0, nNumLines * sizeof(STRING_INFO) );

	// Header (2 Bytes)
	STRFile.Write( szHead, 2 );

	// Version Number (2 Bytes)
	STRFile.Write( &nVersion, sizeof(UINT16) );

	// Number of strings (4 Bytes)
	STRFile.Write( &nNumLines, sizeof(UINT32) );

	// Total Label Size (4 Bytes) : 임시로 저장
	FileOffset = (UINT32)STRFile.GetPosition();
	STRFile.Write( &nTotalLabelSize, sizeof(UINT32) );

	// Total String Size (4 Bytes) : 임시로 저장
	STRFile.Write( &nTotalStringSize, sizeof(UINT32) );

	// String Infos : 임시로 저장
	STRFile.Write( pStrInfo, nNumLines * sizeof(STRING_INFO) );

	// Labels
	{
		INT32 nSize;
		nIndex = 0;
		nTotalLabelSize = 0;
		while( ( nSize = FileReadLine( &LabelFile, szBuf, 1024 ) ) >= 0 )
		{
			nSize += 1;
			I3ASSERT( nIndex < nNumLines );
			pStrInfo[ nIndex++ ].LabelPos = nTotalLabelSize;
			STRFile.Write( szBuf, nSize );
			nTotalLabelSize += nSize;
		}
	}

	// Strings
	{
		// Header
		TextFile.Read( &UniHeader, 2 );

		INT32 nSize;
		nIndex = 0;
		nTotalStringSize = 0;
		while( ( nSize = FileReadLineW( &TextFile, szUniBuf, 1024 ) ) >= 0 )
		{
			wchar_t szTemp[1024];
			nSize = ConvertFormatCharToControlChar( szTemp, szUniBuf );

			if( bCheckDictionary )
			{
				// Check Characters with Dictionary
				int i;
				for( i = 0; i < nSize; i++ )
				{
					if( szTemp[i] >= L' ' )
					{
						if( m_Dictionary[ szTemp[i] ] <= 0 )
						{
							I3TRACE( "Dictionary에 없는 문자가 발견되었습니다.\nLine %d, Column = %d, code = %d(0x%X)",
								nIndex + 1, i + 1, szTemp[i], szTemp[i] );
							I3ASSERT_0;
						}
					}
				}
			}
			nSize = 2 * (nSize + 1);
			I3ASSERT( nIndex < nNumLines );
			pStrInfo[ nIndex++ ].StringPos = nTotalStringSize / sizeof(wchar_t);
			STRFile.Write( szTemp, nSize );
			nTotalStringSize += nSize;
		}
	}

	// Rewrite Vaild Data
	{	
		// Total Label Size (4 Bytes)
		STRFile.Seek( FileOffset, CFile::begin );
		STRFile.Write( &nTotalLabelSize, sizeof(UINT32) );

		// Total String Size (4 Bytes)
		STRFile.Write( &nTotalStringSize, sizeof(UINT32) );

		// String Infos
		STRFile.Write( pStrInfo, nNumLines * sizeof(STRING_INFO) );
	}

	bRet = TRUE;
Error:
	if( !bRet )
	{
		MessageBox( m_strTextFile );
	}
	I3_SAFE_DELETE_ARRAY( pStrInfo );
	//TextFile.Close(); 
	//LabelFile.Close();
	//STRFile.Close();
	return bRet;
}

void Ci3FontDlg::CloseSTR()
{
	I3_SAFE_DELETE_ARRAY( m_pTable	);
	I3_SAFE_DELETE_ARRAY( m_pLabelBuf	);
	I3_SAFE_DELETE_ARRAY( m_pStringBuf	);

	m_nNumString		= 0;
	m_nTotalLabelSize	= 0;
	m_nTotalStringSize	= 0;
}

BOOL Ci3FontDlg::NewBuffers()
{
	I3ASSERT( m_pTable == NULL );
	m_pTable		= new STRING_INFO[m_nNumString];
	I3ASSERT( m_pTable != NULL );

	I3ASSERT( m_pLabelBuf == NULL );
	m_pLabelBuf		= new char[m_nTotalLabelSize];
	I3ASSERT( m_pLabelBuf != NULL );

	I3ASSERT( m_pStringBuf == NULL );
	m_pStringBuf	= new wchar_t[m_nTotalStringSize / sizeof(wchar_t)];
	I3ASSERT( m_pStringBuf != NULL );

	return TRUE;
}

// *.str 파일 열기
BOOL Ci3FontDlg::OpenFromBinary( const char *szPath )
{
	CloseSTR();

	BOOL bRet = FALSE;

	i3FileStream Stream;
	if( ! Stream.Open( szPath, STREAM_READ ) )
	{
		I3TRACE( "Could not open file. (%s)\n", szPath );
		goto Error;
	}

	// Head
	char szHead[10];
	Stream.Read( szHead, 2 );
	if( szHead[0] != 'S' || szHead[1] != 'T' )
	{
		I3TRACE( "Not String Table file.\n" );
		goto Error;
	}

	// Version Number
	UINT16 nVersion;
	Stream.Read( &nVersion, sizeof(UINT16) );
	if( nVersion != 1 )
	{
		I3TRACE( "Version number is incorrect. (%d)\n", nVersion );
		goto Error;
	}

	// Number of Strings
	Stream.Read( &m_nNumString, sizeof(m_nNumString) );
	if( m_nNumString <= 0 )
	{
		I3TRACE( "Number of string is negative. (%d)\n", m_nNumString );
		goto Error;
	}

	// Total size of labels
	Stream.Read( &m_nTotalLabelSize, sizeof(m_nTotalLabelSize) );
	
	// Total size of strings
	Stream.Read( &m_nTotalStringSize, sizeof(m_nTotalStringSize) );

	// New Buffer
	if( ! NewBuffers() )
	{
		goto Error;
	}
    
	UINT32 nLength;
	nLength = Stream.Read( m_pTable, sizeof(STRING_INFO) * m_nNumString );
	if( nLength != (sizeof(STRING_INFO) * m_nNumString) )
	{
		I3TRACE( "Failed to read Information table.\n" );
		goto Error;
	}

	nLength = Stream.Read( m_pLabelBuf, m_nTotalLabelSize );
	if( nLength != m_nTotalLabelSize )
	{
		I3TRACE( "Failed to read labels.\n" );
		goto Error;
	}

	nLength = Stream.Read( m_pStringBuf, m_nTotalStringSize );
	if( nLength != m_nTotalStringSize )
	{
		I3TRACE( "Failed to read strings.\n" );
		goto Error;
	}

	bRet= TRUE;
Error:
	Stream.Close();
	return bRet;
}

// *.str 파일을 라벨과 유니코드 텏츠로 저장하기
BOOL Ci3FontDlg::ConvertSTRtoLabelnUniText()
{
	BOOL bRet = FALSE;

	CFile TextFile;
	CFile LabelFile;

	wchar_t UniHeader = 0xfeff;

	UINT32 i;
	char szCRNL[3] = { '\r', '\n', '\0' };
	wchar_t wszCRNL[3] = { L'\r', L'\n', L'\0' };

	if( ! OpenFromBinary( (LPCTSTR)m_strSTRFile ) )
	{
		MessageBox( "STR File을 읽을 수 없습니다." ); 
		goto Error;
	}

	// Creat Label File
	if ( !LabelFile.Open( m_strDumpLabelFile, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate, NULL) )
	{
		MessageBox( "Label File을 덤프할수 없습니다." ); 
		goto Error;
	}

	// Write Labels
	for( i = 0; i < m_nNumString; i++ )
	{
		char* szLabel = &m_pLabelBuf[ m_pTable[i].LabelPos ];
		UINT32 nSize = (UINT32)strlen( szLabel );
		LabelFile.Write( szLabel, nSize );
		LabelFile.Write( szCRNL, 2 );
	}

	// Create Text File
	if ( !TextFile.Open( m_strDumpTextFile, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate, NULL) )
	{
		MessageBox( "String File을 덤프할수 없습니다." ); 
		goto Error;
	}

	// Header (2 Bytes)
	TextFile.Write( &UniHeader, 2 );

	for( i = 0; i < m_nNumString; i++ )
	{
		wchar_t *wszText = &m_pStringBuf[ m_pTable[i].StringPos ];
		UINT32 nSize = (UINT32)wcslen( wszText );

	#if 0
		for( UINT32 k = 0; k < nSize; k++ )
		{
			wchar_t wCh = m_RevDictionary[ wszText[k] ];
			TextFile.Write( &wCh, sizeof(wchar_t) );
		}
	#else
		TextFile.Write( wszText, nSize * sizeof(wchar_t) );
	#endif

		TextFile.Write( wszCRNL, 2 * sizeof(wchar_t) );
	}

	bRet = TRUE;
Error:
	TextFile.Close(); 
	LabelFile.Close();
	return bRet;
}


// 폰트 선택 버튼
void Ci3FontDlg::OnBnClickedButSelectFont()
{
	UpdateControls();

	// Create the CHOOSEFONT structure
	static CHOOSEFONT cf = {0};
	cf.lStructSize = sizeof(CHOOSEFONT);
	cf.lpLogFont   = &m_LogFont;
	cf.Flags       = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_NOVERTFONTS;
	cf.nFontType   = SCREEN_FONTTYPE;

	if( 0 == ChooseFont( &cf ) )
		return;

	UpdateControls( FALSE );

	SetNeedSave();
}

// 프로젝트 이름 지정
void Ci3FontDlg::SetProjectName( const char *szName )
{
	m_strProjectFile = szName;

	if( m_strProjectFile.GetLength() > 0 )
	{
		CString strTitle;
		strTitle = "i3Font - " + m_strProjectFile;
		SetWindowText( LPCTSTR( strTitle ) );
	}
	else
	{
		SetWindowText( "i3Font" );
	}
}

const char*	Ci3FontDlg::GetProjectName()
{
	return LPCTSTR( m_strProjectFile );
}

// 새 프로젝트
void Ci3FontDlg::OnFileNew()
{
	memset( &m_LogFont, 0, sizeof( LOGFONT ) );
	strcpy( m_LogFont.lfFaceName, "굴림" ); // 굴림 font for a default
	m_LogFont.lfHeight  = -( 10 * 96 ) / 72; // 16-pt font for a default
	m_LogFont.lfWeight  = 400;               // 400 = normal, 700 = bold, etc.
	m_LogFont.lfQuality = ANTIALIASED_QUALITY;

	m_strTextFile = "";
	m_strOutDir = "";

	SetProjectName( "" );

	m_bRelativePath = FALSE;

	UpdateControls( FALSE );
}

// 프로젝트 파일 열기
void Ci3FontDlg::OnFileOpen()
{
	UpdateControls();

	// TODO: Add your control notification handler code here
	CFileDialog Dlg(TRUE,"i3FontPRJ", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		"i3Font Project File (*.i3FontPRJ) | *.i3FontPRJ||", AfxGetMainWnd()); 

	if(Dlg.DoModal() != IDOK) return; 

	if( ! LoadProject( Dlg.GetPathName() ) )
	{
		MessageBox( LPCTSTR( m_strError ), "Error" );
	}

	SetProjectName( Dlg.GetPathName() );
	MakeAllToRelativePath();

	UpdateControls(FALSE); 
}

// 프로젝트 파일 저장
void Ci3FontDlg::OnFileSave()
{
	UpdateControls();

	if( m_strProjectFile.GetLength() == 0 )
	{
		OnFileSaveas();
		return;
	}

	if( ! SaveProject( GetProjectName() ) )
	{
		MessageBox( LPCTSTR( m_strError ), "Error" );
	}

	UpdateControls(FALSE); 
}

// 프로젝트 파일 다른 이름으로 저장
void Ci3FontDlg::OnFileSaveas()
{
	UpdateControls();

	// TODO: Add your control notification handler code here
	CFileDialog Dlg( FALSE,"i3FontPRJ", LPCTSTR(m_strProjectFile), OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,
		"i3Font Project File (*.i3FontPRJ) | *.i3FontPRJ||", AfxGetMainWnd()); 

	if(Dlg.DoModal() != IDOK) return; 

	SetProjectName( Dlg.GetPathName() );

	if( ! SaveProject( GetProjectName() ) )
	{
		MessageBox( LPCTSTR( m_strError ), "Error" );
	}

	UpdateControls(FALSE); 
}

// 미디어 폴더 설정
void Ci3FontDlg::OnBnClickedButtonMediaDir()
{
	UpdateControls();

	i3Win32FolderSelectDialog Dlg;

	if( Dlg.Execute( m_hWnd, "폴더를 선택하세요." ) == FALSE )
		return;

	MakeAllToAbsolutePath();
	m_strMediaDir = Dlg.szDir;
	ChangePathStyle( m_strMediaDir, TRUE );
	MakeAllToRelativePath();

	UpdateControls( FALSE );
}

// Text Directory 설정
void Ci3FontDlg::OnBnClickedButtonTxtDir()
{
	UpdateControls();

	i3Win32FolderSelectDialog Dlg;

	if( Dlg.Execute( m_hWnd, "폴더를 선택하세요." ) == FALSE )
		return;

	MakeAllToAbsolutePath();
	m_strTextDir = Dlg.szDir;
	ChangePathStyle( m_strTextDir, TRUE );
	MakeAllToRelativePath();

	UpdateControls( FALSE );
}

void Ci3FontDlg::OnEnChangeEditMediaDir()
{
	UpdateControls();
	MakeAllToAbsolutePath();
	MakeAllToRelativePath();
	UpdateControls( TRUE );
}

BOOL Ci3FontDlg::OnMakei3FontCB( LPCTSTR szDir, LPCTSTR szFileName )
{
	BOOL bRet = FALSE;

	char szTxtFilename[MAX_PATH];
	sprintf( szTxtFilename, "%s/%s", szDir, szFileName );
	ChangePathStyle( szTxtFilename, TRUE );
	m_strTextFile = szTxtFilename;

	SetOutputFileNames();

	i3Font *pFont = NULL;

	{
		pFont = i3Font::NewObject();

		if( ! pFont->Create( LPCTSTR( m_stri3FntFile ) ) )
		{
			I3ASSERT_0;
			MessageBox( "Failed to create i3Font." );
			goto ExitLabel;
		}

		if( ! pFont->SaveToFile( LPCTSTR( m_stri3FontFile ) ) )
		{
			m_strError.Format( "Failed to save file. (%s)", LPCTSTR( m_stri3FontFile ) );
			MessageBox( LPCTSTR( m_strError ), "Error" );
			goto ExitLabel;
		}
	}


	bRet = TRUE;
ExitLabel:
	I3_SAFE_RELEASE( pFont );
	return bRet;
}

// *.i3font 만들기
void Ci3FontDlg::OnBnClickedButtonMakeI3font()
{
	BOOL bRet = FALSE;
	i3Font *pFont = NULL;

	UpdateControls();
	MakeAllToAbsolutePath();

	if( m_bBatchMode )
	{
		if( ! FindFiles( LPCTSTR( m_strTextDir ), "txt", Makei3FontCB, this, FALSE ) )
		{
			goto ExitLabel;
		}
	}
	else
	{
		pFont = i3Font::NewObject();

		if( ! pFont->Create( LPCTSTR( m_stri3FntFile ) ) )
		{
			I3ASSERT_0;
			MessageBox( "Failed to create i3Font." );
			goto ExitLabel;
		}

		if( ! pFont->SaveToFile( LPCTSTR( m_stri3FontFile ) ) )
		{
			m_strError.Format( "Failed to save file. (%s)", LPCTSTR( m_stri3FontFile ) );
			MessageBox( LPCTSTR( m_strError ), "Error" );
			goto ExitLabel;
		}
	}

	bRet = TRUE;
	MessageBox( "Done." );
ExitLabel:
	I3_SAFE_RELEASE( pFont );
	MakeAllToRelativePath();
	UpdateControls( FALSE );
}


void Ci3FontDlg::OnBnClickedRadioTxt2()
{
	UpdateControls();
}

void Ci3FontDlg::OnBnClickedRadioTxt1()
{
	UpdateControls();
}
