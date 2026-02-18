// i3GraphDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "i3Graph.h"
#include "i3GraphDlg.h"
#include ".\i3graphdlg.h"
#include "FolderDlg.h"
#include "TextureUsageStateDlg.h"
#include "i3Profile.h"
#include "GeometryToolDialog.h"
#include "UVViewerDialog.h"
#include "i3GraphFramework.h"
#include "DialogStatistics.h"
#include "DlgLight.h"
#include "i3XFileParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void NormalizeCmdArg( char *szOut, const char *szIn )
{
	while( *szIn )
	{
		if( *szIn != '"' )
		{
			*szOut = *szIn;
			szOut++;
		}
		szIn++;
	}
	*szOut = 0;
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
public:
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


// Ci3GraphDlg 대화 상자

Ci3GraphDlg::Ci3GraphDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3GraphDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_szCmdLineArg = NULL;
	m_bViewAsWireFrame = FALSE;
	m_bUpdateAnim = FALSE;
	m_i3sFileName[0] = 0;

	m_bSaveExternRef = FALSE;
}

void Ci3GraphDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Ci3GraphDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_DROPFILES()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
//	ON_COMMAND(ID_HELP_IMAGE, OnHelpImage)
ON_COMMAND(ID_TOOL_TEXTUREUSAGESTATE, OnToolTextureusagestate)
ON_COMMAND(ID_TOOL_CREATEBOUNDBOX, OnToolCreateboundbox)
ON_COMMAND(ID_TOOL_DELETEBOUNDBOX, OnToolDeleteboundbox)
ON_COMMAND(ID_FILE_EXIT, OnFileExit)
ON_COMMAND(ID_TOOL_DIRECTORY, OnToolDirectory)
ON_WM_KEYDOWN()
ON_COMMAND(ID_VIEW_VIEWASWIREFRAME, OnViewViewaswireframe)
ON_COMMAND(ID_NODE_GEOMETRYTOOL, OnNodeGeometrytool)
ON_COMMAND(ID_OPT_DELETEATTR, OnOptDeleteattr)
ON_COMMAND(ID_NODE_BUILDRENDERABLECOLLIDERMESH, OnNodeBuildrenderablecollidermesh)
ON_COMMAND(ID_VIEW_UPDATEANIMATION, OnViewUpdateanimation)
//ON_UPDATE_COMMAND_UI(ID_VIEW_UPDATEANIMATION, OnUpdateViewUpdateanimation)
ON_WM_CHAR()
ON_COMMAND(ID_NODE_VIEWUVS, OnNodeViewuvs)
ON_COMMAND(ID_HELP_DUMPMEMORYTOFILE, OnHelpDumpmemorytofile)
ON_COMMAND(ID_VIEW_LIGHTING, OnViewLighting)
ON_COMMAND(ID_VIEW_GRID, OnViewGrid)
ON_COMMAND(ID_VIEW_GLOWEFFECT, OnViewGloweffect)
ON_COMMAND(ID_VIEW_DETAILS, OnViewDetails)
ON_COMMAND(ID_BUILD_RENDERABLE_OCTREE, OnBuildRenderableOctree)
ON_COMMAND(ID_OPTION_BACKGROUNDCOLOR, OnOptionBackgroundcolor)
ON_COMMAND(ID_OPTION_AMBIENTCOLOR, OnOptionAmbientcolor)
ON_COMMAND(ID_GLOWEFFECT_DOWNSCALE, OnGloweffectDownscale)
ON_COMMAND(ID_GLOWEFFECT_BRIGHTPASSEXTRACT, OnGloweffectBrightpassextract)
ON_COMMAND(ID_GLOWEFFECT_GAUSSIANBLUR, OnGloweffectGaussianblur)
ON_COMMAND(ID_GLOWEFFECT_COMBINE, OnGloweffectCombine)
ON_COMMAND(ID_RASTERIZECHAIN_ALLCLEAR, OnRasterizechainAllclear)
ON_COMMAND(ID_RASTERIZECHAIN_ALLSET, OnRasterizechainAllset)
ON_COMMAND(ID_NODE_STATISTICS, OnNodeStatistics)
ON_COMMAND(ID_NODE_EXPORTGEOMETRYLIST, OnNodeExportgeometrylist)
ON_COMMAND(ID_VIEW_VIEWMAPSCALE, OnViewViewmapscale)
ON_COMMAND(ID_FILE_SAVEEXTERNALRESOURCES, OnFileSaveexternalresources)
ON_COMMAND(ID_OPTION_GLOBALLIGHTSETTING, OnOptionGloballightsetting)
ON_COMMAND(ID_RASTERIZECHAIN_TONEMAPPING, OnRasterizechainTonemapping)
ON_COMMAND(ID_FILE_IMPORT, OnFileImport)
END_MESSAGE_MAP()


// Ci3GraphDlg 메시지 처리기

BOOL Ci3GraphDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		DWORD style;

		style = ::GetClassLong( m_hWnd, GCL_STYLE);

		::SetClassLong( m_hWnd, GCL_STYLE, style | CS_DBLCLKS);
	}

	i3Profile pf( this );

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
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	//Set 
	m_pSgInfo	= NULL;
	m_pViewer	= NULL; 	

    //Make Main Window
	{
		CRect rect;
		GetClientRect( &rect);
		m_BaseSplitter.Create( I3_SPLS_EX_VERT, WS_VISIBLE | WS_CHILD, rect, 10, this, 100);

		//1
		m_SplitWnd1.Create( 0, WS_VISIBLE | WS_CHILD, rect, 10, &m_BaseSplitter, 101);		
		m_MessageWnd.Create(MAKEINTRESOURCE( IDD_DIALOG_MESSAGE ), &m_BaseSplitter);

		m_BaseSplitter.AddPane( &m_SplitWnd1 );		//SplitWnd
		m_BaseSplitter.AddPane( &m_MessageWnd );	//MessageWnd

		//2
		m_ViewerWnd.Create( WS_VISIBLE | WS_CHILD , rect, &m_SplitWnd1, 103);
		m_SplitWnd2.Create( 0, WS_VISIBLE | WS_CHILD, rect, 10, &m_SplitWnd1, 102);

		m_SplitWnd1.AddPane( &m_ViewerWnd );		//ViewerWnd
		m_SplitWnd1.AddPane( &m_SplitWnd2 );		//SplitWnd

		//3
		m_SplitWnd3.Create( I3_SPLS_EX_VERT, WS_VISIBLE | WS_CHILD, rect, 10, &m_SplitWnd2, 103);
		m_SplitWnd4.Create( I3_SPLS_EX_VERT, WS_VISIBLE | WS_CHILD, rect, 10, &m_SplitWnd2, 104);

		m_SplitWnd2.AddPane( &m_SplitWnd3 );		//SplitWnd
		m_SplitWnd2.AddPane( &m_SplitWnd4 );		//SplitWnd

		//4	
		{
			CRect rect;

			GetClientRect( &rect);
			m_TreeWnd.Create( WS_CHILD | WS_VISIBLE, rect, &m_SplitWnd3, 105); 
		}
		m_PropertyWnd.Create(MAKEINTRESOURCE( IDD_DIALOG_PROPERTY ), &m_SplitWnd3 ); 

		m_SplitWnd3.AddPane( &m_TreeWnd		);		//TreeWnd
		m_SplitWnd3.AddPane( &m_PropertyWnd	);		//PropertyWnd

		//5
		m_OptiWnd.Create(MAKEINTRESOURCE( IDD_DIALOG_OPT  ), &m_SplitWnd4 ); 
		m_SpecWnd.Create(MAKEINTRESOURCE( IDD_DIALOG_SPEC ), &m_SplitWnd4 ); 

		m_SplitWnd4.AddPane( &m_OptiWnd	);		//OptimizeWnd
		m_SplitWnd4.AddPane( &m_SpecWnd	);		//SpecWnd
		
		m_BaseSplitter.LoadWindowInfo( "i3Graph.CFG");
		m_BaseSplitter.Rearrange();

		m_RootDirectory = pf.GetString( "RootDir", "" );

		i3ResourceFile::SetWorkingFolder( (char*)LPCTSTR(m_RootDirectory));
		i3ResourceFile::setSearchExternRefEnable( true);

		m_bUpdateAnim = (BOOL) pf.GetInt( "UpdateAnimation", FALSE);
	}

	//Make Viewer
	{
		i3VideoInfo video;

		video.m_Width = 640;
		video.m_Height = 480;

		m_pViewer = i3Viewer::NewObject();
		m_pViewer->Create( (void *) m_ViewerWnd.m_hWnd, &video, I3I_DEVICE_MASK_CONTROLLER);

#if !defined( I3_NO_PROFILE )
		m_pViewer->setDebugTextEnable( true);
#endif

		m_pFramework = i3GraphFramework::NewObject();
		m_pFramework->Create( m_pViewer);

		m_ViewerWnd.SetViewer(m_pViewer);

		{
			// Ambient
			i3RenderContext * pGfx = m_pViewer->GetRenderContext();

			m_pViewer->GetRenderContext()->setAmbient( g_pConfig->getAmbient());

			// Back Color
			COLOR col;

			i3Color::Set( &col, g_pConfig->getBackColor());
			pGfx->SetClearColor( &col);
		}

		InitSceneRoot(); 
	}

	if( m_szCmdLineArg != NULL )
	{
		if( m_szCmdLineArg[0] != 0 )
		{
			char szFileName[MAX_PATH];
			NormalizeCmdArg( szFileName, m_szCmdLineArg );

			if ( CheckFileName( szFileName ) == FALSE ) 
			{	
				MessageBox( "*.i3s only!", "Error" );
			}
			else
			{
				//여기부터 실제 적인 씬 파일 로드 입니다. 
				CloseSceneRoot(); 

				m_pSgInfo = OpenI3sFile( szFileName ); 
				if( m_pSgInfo != NULL)
				{
					Seti3sFileName( szFileName ); 
					InitSceneRoot(); 
				}
			}
		}
	}

	UpdateAnimationState();

	m_pFramework->SetLight( g_pConfig->getLightDiffuse(), g_pConfig->getLightSpecular(), g_pConfig->getShadowMapSize());

	{
		i3TDK::RegisterUpdate( m_hWnd, i3Node::GetClassMeta(), I3_TDK_UPDATE_ALL);
		i3TDK::RegisterUpdate( m_hWnd, i3RenderAttr::GetClassMeta(), I3_TDK_UPDATE_ALL);
	}

	InitMenuItemCheck();

	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

/*=============================================================
//Name : 
//Desc :
=============================================================*/
void Ci3GraphDlg::OnDestroy()
{
	// TODO: Add your message handler code here
	m_TreeWnd.setSgInfo( NULL);	

	//Save Window Info
	m_BaseSplitter.SaveWindowInfo( "i3Graph.CFG" );

	i3Profile pf( this );
	pf.WriteString( "RootDir", LPCTSTR( m_RootDirectory ) );
	pf.WriteInt( "UpdateAnimation", m_bUpdateAnim);

	{
		// Ambient
		i3RenderContext * pGfx = m_pViewer->GetRenderContext();

		g_pConfig->setAmbient( m_pViewer->GetRenderContext()->getAmbient());
		
		// Back Color
		COLORREAL col;
		
		i3Color::Set( &col, pGfx->GetClearColor());
		g_pConfig->setBackColor( &col);
	}

	I3_SAFE_RELEASE( m_pSgInfo);
	I3_SAFE_RELEASE( m_pViewer);
	I3_SAFE_RELEASE( m_pFramework);

	CDialog::OnDestroy();

#if defined( I3_DEBUG )
	//For Debug Dump Memory 
	i3mem::Dump(); 
#endif
}

void Ci3GraphDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 절대 경로 구해줌. 실제 파일 로드시 사용.
BOOL Ci3GraphDlg::GetAbsolutePath( char* szOut, const char *szIn )
{
	if( m_RootDirectory[0] == 0 )
	{
		strcpy( szOut, szIn );
		return FALSE;
	}
	else
	{
		sprintf( szOut, "%s/%s", m_RootDirectory, szIn );
		return TRUE;
	}
}

// 상대 경로 구해줌. 텍스쳐 이름으로 설정. 게임에서 사용.
BOOL Ci3GraphDlg::GetRelativePath( char* szOut, const char *szIn, BOOL bUnixStyle )
{
	if( m_RootDirectory[0] == 0 )
	{
		strcpy( szOut, szIn );
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
			int n = strlen( m_RootDirectory );
			if( _strnicmp( m_RootDirectory, szIn, n ) == 0 )
			{
				strcpy( szOut, szIn + n + 1 );
				if( bUnixStyle )
				{
					char *ch = szOut;;
					while( ( ch = strchr( ch, '\\' ) ) != NULL )
					{
						*ch = '/';
						ch++;
					}
				}
				return TRUE;
			}
			else
			{
				strcpy( szOut, szIn );
				return FALSE;
			}
		}
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void Ci3GraphDlg::OnPaint() 
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
HCURSOR Ci3GraphDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*=============================================================
//Name : 
//Desc :
=============================================================*/
void Ci3GraphDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

/*=============================================================
//Name : 
//Desc :
=============================================================*/
void Ci3GraphDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

/*=============================================================
//Name : 
//Desc :
=============================================================*/
void Ci3GraphDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if( ::IsWindow( m_BaseSplitter.m_hWnd))
	{
		m_BaseSplitter.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}

}

/*=============================================================
//Name : 
//Desc :
=============================================================*/
void Ci3GraphDlg::OnDropFiles(HDROP hDropInfo)
{
	int count; 
	char FileName[256];

	count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

	if(count > 1)
	{
		//Error 한번에 하나만 로딩할수 있다. 
		CDialog::OnDropFiles(hDropInfo);
		return; 
	}

	DragQueryFile( hDropInfo, 0, FileName, sizeof(FileName) - 1);
	if ( CheckFileName(FileName) == FALSE ) 
	{
		MessageBox( "*.i3s only!", "Error" );
		CDialog::OnDropFiles(hDropInfo);
		return;
	}

	//여기부터 실제 적인 씬 파일 로드 입니다. 
	CloseSceneRoot(); 
	m_pSgInfo = OpenI3sFile( FileName ); 

	if( m_pSgInfo != NULL)
	{
		Seti3sFileName( FileName );

		InitSceneRoot(); 
	}
	
	CDialog::OnDropFiles(hDropInfo);
}

/*=============================================================
//Name : 
//Desc :
=============================================================*/
void Ci3GraphDlg::CloseSceneRoot(void)
{
	//Release Root
	if( m_pSgInfo != NULL)
	{
		m_pSgInfo->Release(); 
		m_pSgInfo = NULL; 
		InitSceneRoot();
		Seti3sFileName( NULL );
	}

	m_ViewerWnd.Invalidate();

	return; 
}

void Ci3GraphDlg::Seti3sFileName( const char *szPath )
{
	if( szPath == NULL )
	{
		strcpy( m_i3sFileName, "" );
	}
	else
	{
		strcpy( m_i3sFileName, szPath ); 
	}

	if( m_i3sFileName[0] == 0 )
	{
		SetWindowText( "i3Graph" );
	}
	else
	{
		char szName[MAX_PATH];
		sprintf( szName, "%s - i3Graph", m_i3sFileName );
		SetWindowText( szName );
	}
}

/*=============================================================
//Name : 
//Desc :
=============================================================*/
void Ci3GraphDlg::OnFileOpen(void)
{
	CString FileName; 
	CFileDialog	Dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"I3Engine File(*.i3s)|*.i3s||", this);

	if( Dlg.DoModal() != IDOK)return;

	FileName = Dlg.GetFileName();

	if ( CheckFileName( (char *)LPCTSTR(FileName) ) == FALSE ) 
	{	
		MessageBox( "*.i3s only!", "Error" );
		return; 
	}

	//여기부터 실제 적인 씬 파일 로드 입니다. 
	CloseSceneRoot(); 
	m_pSgInfo = OpenI3sFile( (char *)LPCTSTR(FileName) ); 
	if( m_pSgInfo != NULL)
	{
		Seti3sFileName( LPCTSTR(FileName) ); 

		InitSceneRoot(); 
	}
}

void Ci3GraphDlg::setSgInfo( i3SceneGraphInfo * pSgInfo)
{
	I3_REF_CHANGE( m_pSgInfo, pSgInfo);

	InitSceneRoot();
}

/*=============================================================
//Name : 
//Desc :
=============================================================*/
i3SceneGraphInfo * Ci3GraphDlg::OpenI3sFile(char * FileName)
{
	i3SceneFile file; 

	if (file.Load(FileName) == STREAM_ERR)
	{
		//Error 표시 로딩 에러 입니다. 
		CString strMsg;
		strMsg.Format( "파일을 불러오지 못했습니다. (%s)", FileName );
		MessageBox( LPCTSTR( strMsg ), "Error" );
		return NULL; 
	}

	Log(I3GRAP_LOG_NORMAL, "File Load \" %s \" ", FileName); 	
	
	i3SceneGraphInfo * pSgInfo = file.getSceneGraphInfo();

	if( pSgInfo->getCommonSg() != NULL)
	{
		pSgInfo->getCommonSg()->Instantiate();

		i3Scene::ModifyDynamicState( pSgInfo->getCommonSg(), I3_PHYSIX_STATE_KINEMATIC, 0);
	}

	if( pSgInfo->getInstanceSg() != NULL)
	{
		pSgInfo->getInstanceSg()->Instantiate();

		i3Scene::ModifyDynamicState( pSgInfo->getInstanceSg(), I3_PHYSIX_STATE_KINEMATIC, 0);

	}
	
	pSgInfo->AddRef();
	pSgInfo->SetName( FileName);

	m_ViewerWnd.Invalidate();

	return pSgInfo;
}

/*=============================================================
//Name : 
//Desc :
=============================================================*/
BOOL Ci3GraphDlg::CheckFileName(char * FileName)
{
	char Ext[32];

	i3String::SplitFileExt(FileName, Ext, sizeof(Ext) - 1);
	if( i3String::Compare( (char *)Ext, (char *)"i3s") != 0)
	{
		//Error i3s File이 아니다. 
		return FALSE; 
	}
	return TRUE; 
}

/*=============================================================
//Name : 
//Desc : 
=============================================================*/
void Ci3GraphDlg::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent ==1)	
	{
		if( m_pViewer != NULL )	m_pViewer->Render(); 		
	}
	CDialog::OnTimer(nIDEvent);
}

/*=============================================================
//Name : 
//Desc : 
=============================================================*/
void Ci3GraphDlg::OnFileClose()
{
	// TODO: Add your command handler code here
	CloseSceneRoot(); 
}

/*=============================================================
//Name : 
//Desc :
=============================================================*/
void Ci3GraphDlg::OnFileSave()
{
	if( m_i3sFileName[0] == 0 )
	{
		OnFileSaveas(); 
		return;
	}

	SaveI3sFile(m_i3sFileName); 
}

void Ci3GraphDlg::OnFileSaveexternalresources()
{
	m_bSaveExternRef = TRUE;

	OnFileSave();

	m_bSaveExternRef = FALSE;
}


/*=============================================================
//Name : 
//Desc :
=============================================================*/
void Ci3GraphDlg::OnFileSaveas()
{
	// TODO: Add your command handler code here
	CString FileName; 
	CFileDialog	Dlg( FALSE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"I3Engine File(*.i3s)|*.i3s||", this);

	if( Dlg.DoModal() != IDOK )
	{
		//Message 
		return; 
	}

	FileName = Dlg.GetFileName(); 
	SaveI3sFile( (char*)LPCTSTR(FileName) ); 
}

/*=============================================================
//Name : 
//Desc :
=============================================================*/
void Ci3GraphDlg::SaveI3sFile(char * FileName)
{
	if( m_pSgInfo == NULL)
	{
		//Message
		return; 
	}

	Log(I3GRAP_LOG_NORMAL, "File Save \" %s \" ", FileName); 	

	{//Save 
		i3SceneFile file;
		//if( m_RootDirectory.GetLength() > 0 )
		//{
		//	file.SetRootDir( LPCTSTR( m_RootDirectory ) );
		//}

		file.SetOverwriteExternalResStatus( m_bSaveExternRef);

		file.setSceneGraphInfo( m_pSgInfo);

		if(file.Save(FileName) == STREAM_ERR)
		{
			//Message ReadOnly etc.....
			DebugBox( m_hWnd, "%s 파일에 저장할 수 없습니다.", FileName);
			return; 		
		}
	}

	Seti3sFileName( FileName );
	
	//Message 
	return; 
}

/*=============================================================
//Name : void Ci3GraphDlg::InitSceneRoot(void)
//Desc : 모든 윈도우에 Scene Root를 초기화 합니다. 
=============================================================*/
void Ci3GraphDlg::InitSceneRoot(void)
{
	if( m_TreeWnd.getSgInfo() != m_pSgInfo)
		m_TreeWnd.setSgInfo( m_pSgInfo);

	if( m_TreeWnd.getAddPoint() != NULL)
	{
		m_pFramework->SetSceneGraph( m_TreeWnd.getAddPoint());
	}

	SetPropertyWnd( NULL); 
	return; 
}

/*=============================================================
//Name : void Ci3GraphDlg::Log(INT32 LogLevel, const char *format, ...)
//Desc : 
=============================================================*/
void Ci3GraphDlg::Log(INT32 LogLevel, const char *format, ...)
{
	char	tempstr[256];

	va_list marker;
	va_start( marker, format);
	vsprintf( tempstr, format, marker);

	m_MessageWnd.SetMessage(LogLevel, tempstr); 

	return; 
}

/*=============================================================
//Name : BOOL Ci3GraphDlg::SetDialogWnd(CBaseSpecDialog * pDialog)
//Desc : 
=============================================================*/
BOOL Ci3GraphDlg::SetDialogWnd(CBaseSpecDialog * pDialog)
{	
	BOOL rv; 
	rv = m_SpecWnd.AttachDialog(pDialog); 
	return rv; 
}

void Ci3GraphDlg::SetPropertyWnd(i3ElementBase * pProperty)
{
	m_PropertyWnd.SetProperty(pProperty); 
	return; 
}

//void Ci3GraphDlg::OnHelpImage()
//{
//	// TODO: Add your command handler code here
//	DWORD dwStyle = NULL;///WS_VISIBLE | WS_CAPTION | WS_THICKFRAME | WS_SYSMENU;
//	m_ImageView.Create(dwStyle,this,0);
//
//	m_ImageView.OpenImageFile();
//	//*/
//}

void Ci3GraphDlg::OnToolTextureusagestate()
{
	// TODO: Add your command handler code here

	TextureUsageStateDlg TexStateDlg; 

	char szPrefix[MAX_PATH] = "";
	i3String::SplitFileName(m_i3sFileName, szPrefix, FALSE );
	TexStateDlg.SetFileNamePrefix( szPrefix );

	//Open Dialog Box
	if(IDOK == TexStateDlg.DoModal())
	{

	}
}

BOOL Ci3GraphDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN)
	{
		//TranslateMessage( pMsg);

		//DispatchMessage( pMsg);

		return FALSE;
	}
	else if( pMsg->message == WM_CHAR)
	{
		TranslateMessage( pMsg);

		DispatchMessage( pMsg);

		return TRUE;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void Ci3GraphDlg::OnToolCreateboundbox()
{
	// TODO: Add your command handler code here
	if( m_pSgInfo == NULL)
	{
		//Message
		return; 
	}

	CreateBoundBox( m_pSgInfo->getInstanceSg()); 
	return; 

}

void Ci3GraphDlg::CreateBoundBox(i3Node *pNode)
{
	INT32 i;
	i3BoundBox * pBoundBox; 

	//Get Bound Box 
	pBoundBox = pNode->GetBound(); 

	//Create Bound Box 
	if(pBoundBox == NULL)
	{
		i3BoundBox * pBox = i3BoundBox::NewObject();
		pNode->GetWrappingBound(pBox); 
		pNode->SetBound(pBox); 
		pBox->Release(); 
	}		

	//Search ChildNode
	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		CreateBoundBox( pNode->GetChild(i));
	}
}


void Ci3GraphDlg::OnToolDeleteboundbox()
{
	// TODO: Add your command handler code here
	if( m_pSgInfo == NULL)
	{
		//Message
		return; 
	}

	DeleteBoundBox( m_pSgInfo->getInstanceSg()); 
}

void Ci3GraphDlg::DeleteBoundBox(i3Node * pNode)
{
	INT32 i;
	i3BoundBox * pBoundBox; 

	//Get Bound Box 
	pBoundBox = pNode->GetBound(); 

	//Delete Bound Box 
	if(pBoundBox != NULL)
	{
		pNode->SetBound(NULL); 
	}

	//Search ChildNode
	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		DeleteBoundBox( pNode->GetChild(i));
	}
}
void Ci3GraphDlg::OnFileExit()
{
	OnOK();
}

void Ci3GraphDlg::OnToolDirectory()
{
	CFolderDlg dlg;
	dlg.SetTitle( "Set Root Directory" );
	dlg.SetDir( LPCTSTR(m_RootDirectory) );
	if( dlg.DoModal() == IDOK )
	{
		m_RootDirectory = dlg.GetDir();

		i3ResourceFile::SetWorkingFolder( (char*)LPCTSTR(m_RootDirectory));
	}
}

void Ci3GraphDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}


void Ci3GraphDlg::OnViewViewaswireframe()
{
	m_bViewAsWireFrame ^= 1;
	GetMenu()->CheckMenuItem( ID_VIEW_VIEWASWIREFRAME, m_bViewAsWireFrame ? MF_CHECKED : MF_UNCHECKED );

	if( m_bViewAsWireFrame == TRUE)
		g_pRenderContext->SetRenderMode( I3G_RENDER_LINE);
	else
		g_pRenderContext->SetRenderMode( I3G_RENDER_POLYGON);
}

void Ci3GraphDlg::OnNodeGeometrytool()
{
	CGeometryToolDialog dlg;

	if((m_pSgInfo != NULL) && (m_pSgInfo->getInstanceSg() != NULL))
		dlg.Execute( m_pSgInfo->getInstanceSg());
}

void Ci3GraphDlg::OnOptDeleteattr()
{
	// TODO: Add your command handler code here
	// TODO: Add your command handler code here
	if( m_pSgInfo != NULL)
	{
		i3TDKSingleSelectAttrTypeDialog dlg;

		if( IDOK == dlg.DoModal() )	
		{
			i3ClassMeta * pMeta;
			pMeta = i3ClassMeta::FindClassMetaByName((LPCTSTR) dlg.m_szSelectedMeta);
			DeleteAttr( m_TreeWnd.getAddPoint(), pMeta ); 
			InitSceneRoot(); 
		}
	}
}

void Ci3GraphDlg::DeleteAttr( i3Node * pNode, i3ClassMeta * pMeta )
{
	INT32 i;

	//Delete Attr
	if( pNode->GetMeta() == i3AttrSet::GetClassMeta() )
	{
		i3AttrSet * pAttrSet = (i3AttrSet *)pNode; 
		i3RenderAttr * pAttr; 
		for( i = 0; i < pAttrSet->GetAttrCount(); i++)
		{
			pAttr = pAttrSet->GetAttr(i); 
			if(pAttr->GetMeta() == pMeta)
			{
				pAttrSet->RemoveAttr(pAttr); 
			}
		}
	}

	//Search ChildNode
	for( i = 0; i < pNode->GetChildCount(); i++)
	{
		DeleteAttr( pNode->GetChild(i) , pMeta);
	}
	return; 
}

void Ci3GraphDlg::OnNodeBuildrenderablecollidermesh()
{
	m_TreeWnd.BuildRenderableColliderMesh();
}

void Ci3GraphDlg::UpdateAnimationState(void)
{
	if( m_bUpdateAnim)
	{
		SetTimer(1, 33, NULL);		
	}
	else
	{
		KillTimer( 1);
	}
}

void Ci3GraphDlg::OnViewUpdateanimation()
{
	m_bUpdateAnim = !m_bUpdateAnim;

	CMenu * pMenu = GetMenu();

	UpdateAnimationState();

	if( m_bUpdateAnim)
	{
		pMenu->CheckMenuItem( ID_VIEW_UPDATEANIMATION, MF_CHECKED | MF_BYCOMMAND);
	}
	else
	{
		pMenu->CheckMenuItem( ID_VIEW_UPDATEANIMATION, MF_UNCHECKED | MF_BYCOMMAND);
	}
}

void Ci3GraphDlg::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

void Ci3GraphDlg::OnNodeViewuvs()
{
	CUVViewerDialog dlg;

	if((m_pSgInfo != NULL) && (m_pSgInfo->getInstanceSg() != NULL))
	{
		dlg.Execute( m_pSgInfo->getInstanceSg());
	}
}

void Ci3GraphDlg::OnHelpDumpmemorytofile()
{
	i3MemDump( I3MEMORY_DEBUG_FILE | I3MEMORY_DEBUG_EACH);
}

void Ci3GraphDlg::OnViewLighting()
{
	bool enable = !m_pFramework->getLightingEnable();
	m_pFramework->setLightingEnable( enable);
	SetSafeCheckMenuItem(ID_VIEW_LIGHTING, enable);
}

void Ci3GraphDlg::OnViewGrid()
{
	bool IsGridShow = !m_pFramework->GetGridShow();
	m_pFramework->SetGridShow(IsGridShow);
	SetSafeCheckMenuItem(ID_VIEW_GRID, IsGridShow);
}

void Ci3GraphDlg::OnViewGloweffect()
{
	m_pFramework->ToggleRasterizeChain_DownScale();
	m_pFramework->ToggleRasterizeChain_BrightPassExtract();
	m_pFramework->ToggleRasterizeChain_GaussianBlur();
	m_pFramework->ToggleRasterizeChain_BloomCombine();
}

void Ci3GraphDlg::OnViewDetails()
{
	bool	Enable = !m_pFramework->GetDebugTextEnable();
	m_pFramework->SetDebugTextEnable(Enable);
	SetSafeCheckMenuItem(ID_VIEW_DETAILS, Enable);
}

void		Ci3GraphDlg::SetSafeCheckMenuItem(UINT MenuID, BOOL IsCheck)
{
	CMenu * pMenu = GetMenu();
	if (pMenu != NULL)
	{
		UINT Check = MF_BYCOMMAND;
		Check |= IsCheck ? MF_CHECKED : MF_UNCHECKED;
		pMenu->CheckMenuItem(MenuID, Check);
	}
}

void		Ci3GraphDlg::InitMenuItemCheck()
{
	SetSafeCheckMenuItem(ID_VIEW_LIGHTING, m_pFramework->getLightingEnable());
	SetSafeCheckMenuItem(ID_VIEW_GRID, m_pFramework->GetGridShow());
	SetSafeCheckMenuItem(ID_VIEW_DETAILS, m_pFramework->GetDebugTextEnable());
}


static i3Node * ___BuildBox( VEC3D * vCenter, VEC3D *size, COLORREAL Col)
{
	i3Transform * pNewTrans = i3Transform::NewObject();

	MATRIX * pMtx = pNewTrans->GetMatrix();

	i3Matrix::SetScale( pMtx, size->x, size->y, size->z);

	i3Matrix::SetPos( pMtx, vCenter);

	i3Node * pNew = i3SceneUtil::BuildBoxMesh( FALSE, TRUE, &Col, &Col);

	pNewTrans->AddChild( pNew);

	pNew->Release();

	return pNewTrans;
}

static i3Node * __BuildRenderableMesh( i3Node * pNode)
{
	return NULL;
}

static void _Rec_BuildRend( i3Node * pNode)
{
	if( pNode == NULL)
		return;

	for( INT32 i = 0; i < pNode->GetChildCount(); ++i)
	{
		i3Node * pChild = pNode->GetChild( i);
		if( pChild != NULL)
		{
			_Rec_BuildRend( pChild);
		}
	}

	i3Node * pNew = __BuildRenderableMesh( pNode);
	if( pNew != NULL)
	{
		pNode->AddChild( pNew);

		pNew->Release();
	}
}

void Ci3GraphDlg::OnBuildRenderableOctree()
{
	if((m_pSgInfo == NULL) || (m_pSgInfo->getInstanceSg() == NULL))
		return;

	i3Node * pRoot = m_TreeWnd.getAddPoint();
	i3Node * pNode = i3Node::NewObjectRef();

	i3Node * pSrcNode = m_pSgInfo->getInstanceSg();
	if( pSrcNode != NULL)
	{
		_Rec_BuildRend( pSrcNode);
	}

	pRoot->AddChild( pNode);
}

void	Ci3GraphDlg::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event == I3_TDK_UPDATE_SELECT)
	{
		if( pInfo->m_pMeta->IsTypeOf( i3Node::GetClassMeta()))
		{
			m_pFramework->SelectNode( (i3Node *) pInfo->m_pObject);
		}

		Invalidate();
	}
}

LRESULT Ci3GraphDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void Ci3GraphDlg::OnOptionBackgroundcolor()
{
	i3TDKColorPicker dlg;
	i3RenderContext * pGfx = m_pViewer->GetRenderContext();
	COLORREAL col;

	i3Color::Set( &col, pGfx->GetClearColor());

	if( dlg.Execute( &col))
	{
		COLOR c;

		i3Color::Set( &c, dlg.getSelectedColor());

		pGfx->SetClearColor( &c);
	}
}

void Ci3GraphDlg::OnOptionAmbientcolor()
{
	i3TDKColorPicker dlg;
	i3RenderContext * pGfx = m_pViewer->GetRenderContext();

	if( dlg.Execute( pGfx->getAmbient()))
	{
		pGfx->setAmbient( dlg.getSelectedColor());
	}
}

void Ci3GraphDlg::OnGloweffectDownscale()
{
	// TODO: Add your command handler code here		
	GetMenu()->CheckMenuItem( ID_GLOWEFFECT_DOWNSCALE, m_pFramework->isEnable_DownScale() ? MF_UNCHECKED : MF_CHECKED );
	m_pFramework->ToggleRasterizeChain_DownScale();
}

void Ci3GraphDlg::OnGloweffectBrightpassextract()
{
	// TODO: Add your command handler code here	
	GetMenu()->CheckMenuItem( ID_GLOWEFFECT_BRIGHTPASSEXTRACT, m_pFramework->isEnable_BrightPassExtract() ? MF_UNCHECKED : MF_CHECKED );
	m_pFramework->ToggleRasterizeChain_BrightPassExtract();
}

void Ci3GraphDlg::OnGloweffectGaussianblur()
{
	// TODO: Add your command handler code here	
	GetMenu()->CheckMenuItem( ID_GLOWEFFECT_GAUSSIANBLUR, m_pFramework->isEnable_GaussianBlur() ? MF_UNCHECKED : MF_CHECKED );
	m_pFramework->ToggleRasterizeChain_GaussianBlur();
}

void Ci3GraphDlg::OnGloweffectCombine()
{
	// TODO: Add your command handler code here	
	GetMenu()->CheckMenuItem( ID_GLOWEFFECT_COMBINE, m_pFramework->isEnable_BloomCombine() ? MF_UNCHECKED : MF_CHECKED );
	m_pFramework->ToggleRasterizeChain_BloomCombine();
}

void Ci3GraphDlg::OnRasterizechainAllclear()
{
	// TODO: Add your command handler code here
	if( m_pFramework->isEnable_DownScale()) 
	{
		m_pFramework->ToggleRasterizeChain_DownScale();
		GetMenu()->CheckMenuItem( ID_GLOWEFFECT_DOWNSCALE, m_pFramework->isEnable_DownScale() ? MF_CHECKED : MF_UNCHECKED );
	}

	if( m_pFramework->isEnable_BrightPassExtract()) 
	{
		m_pFramework->ToggleRasterizeChain_BrightPassExtract();
		GetMenu()->CheckMenuItem( ID_GLOWEFFECT_BRIGHTPASSEXTRACT, m_pFramework->isEnable_BrightPassExtract() ? MF_CHECKED : MF_UNCHECKED );
	}

	if( m_pFramework->isEnable_GaussianBlur()) 
	{
		m_pFramework->ToggleRasterizeChain_GaussianBlur();
		GetMenu()->CheckMenuItem( ID_GLOWEFFECT_GAUSSIANBLUR, m_pFramework->isEnable_GaussianBlur() ? MF_CHECKED : MF_UNCHECKED );
	}

	if( m_pFramework->isEnable_BloomCombine()) 
	{
		m_pFramework->ToggleRasterizeChain_BloomCombine();
		GetMenu()->CheckMenuItem( ID_GLOWEFFECT_COMBINE, m_pFramework->isEnable_BloomCombine() ? MF_CHECKED : MF_UNCHECKED );
	}
}

void Ci3GraphDlg::OnRasterizechainAllset()
{
	// TODO: Add your command handler code here
	if( !m_pFramework->isEnable_DownScale()) 
	{
		m_pFramework->ToggleRasterizeChain_DownScale();
		GetMenu()->CheckMenuItem( ID_GLOWEFFECT_DOWNSCALE, m_pFramework->isEnable_DownScale() ? MF_CHECKED : MF_UNCHECKED );
	}

	if( !m_pFramework->isEnable_BrightPassExtract()) 
	{
		m_pFramework->ToggleRasterizeChain_BrightPassExtract();
		GetMenu()->CheckMenuItem( ID_GLOWEFFECT_BRIGHTPASSEXTRACT, m_pFramework->isEnable_BrightPassExtract() ? MF_CHECKED : MF_UNCHECKED );
	}

	if( !m_pFramework->isEnable_GaussianBlur()) 
	{
		m_pFramework->ToggleRasterizeChain_GaussianBlur();
		GetMenu()->CheckMenuItem( ID_GLOWEFFECT_GAUSSIANBLUR, m_pFramework->isEnable_GaussianBlur() ? MF_CHECKED : MF_UNCHECKED );
	}

	if( !m_pFramework->isEnable_BloomCombine()) 
	{
		m_pFramework->ToggleRasterizeChain_BloomCombine();
		GetMenu()->CheckMenuItem( ID_GLOWEFFECT_COMBINE, m_pFramework->isEnable_BloomCombine() ? MF_CHECKED : MF_UNCHECKED );
	}
}

void Ci3GraphDlg::OnNodeStatistics()
{
	CDialogStatistics dlg;

	dlg.Execute( m_TreeWnd.GetSceneGraph());
}
/*
static void _AddVertexArray( i3List * pList, i3GeometryAttr * pAttr, VEC3D * pmaxScale)
{
	if( (pList == NULL) ||
		(pAttr == NULL) || 
		(pAttr->GetVertexArray() == NULL))
		return;

	i3VertexArray * pVA = pAttr->GetVertexArray();

	I3SND_FMOD_POLYGON * pPoly = (I3SND_FMOD_POLYGON*)i3MemAlloc( sizeof( I3SND_FMOD_POLYGON) );

	pPoly->_nVertexCount		= pVA->GetCount();
	pPoly->_rDirectOcclusion	= 0.5f;
	pPoly->_rReverbOcclusion	= 0.5f;
	pPoly->_bDoubleSided		= 1;
	pPoly->_pvVertexArray		= (PERSIST_VEC3D*)i3MemAlloc( sizeof( PERSIST_VEC3D) * pVA->GetCount());

	pVA->Lock();
	for( UINT32 i = 0; i < pVA->GetCount(); ++i)
	{
		RT_VEC3D	vec;

		PERSIST_VEC3D * pVertex = pPoly->_pvVertexArray + i;

		pVA->GetPosition( i, &vec);

		pVertex->x = vec.x;		pmaxScale->x = MAX( pmaxScale->x, vec.x);
		pVertex->y = vec.y;		pmaxScale->y = MAX( pmaxScale->y, vec.y);
		pVertex->z = vec.z;		pmaxScale->z = MAX( pmaxScale->z, vec.z);
	}
	pVA->Unlock();

	pList->Add( pPoly);
}

static void _Rec_ExportGeometry( i3Node * pNode, i3List * pList, VEC3D * pmaxScale)
{
	if( pNode == NULL)
		return;

	INT32 i;
	for( i = 0;i < pNode->GetChildCount(); ++i)
	{
		i3Node * pChild = pNode->GetChild( i);

		_Rec_ExportGeometry( pChild, pList, pmaxScale);
	}

	if( !pNode->IsTypeOf( i3Geometry::GetClassMeta()))
		return;
	
	i3Geometry * pGeom = (i3Geometry*) pNode;

	for( i = 0; i < pGeom->GetGeometryAttrCount(); ++i)
	{
		_AddVertexArray( pList, (i3GeometryAttr*)pGeom->GetGeometryAttr( i), pmaxScale);
	}
}
*/
void Ci3GraphDlg::OnNodeExportgeometrylist()
{
	// TODO: Add your command handler code here
	i3SceneGraphInfo * pInfo = m_TreeWnd.getSgInfo();
	if( pInfo == NULL)
	{
		MessageBox( "작업중인 파일이 없습니다.", "알림", MB_OK);
		return;
	}

	/*
	i3Node * pInstRoot = pInfo->getInstanceSg();

	VEC3D	maxScale;

	i3List geomList;

	_Rec_ExportGeometry( pInstRoot, &geomList, &maxScale);

	REAL32 scale = MAX( maxScale.x, maxScale.y);
	scale = MAX( maxScale.x, scale);

	if( geomList.GetCount() > 0)
	{
		UINT32 Result, Rc;
		i3FileStream File;

		CString FileName; 
		CFileDialog	Dlg( FALSE, "fgf", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
			"i3Snd FMOD Geometry File(*.fgf)|*.fgf||", this);

		if( Dlg.DoModal() != IDOK )
		{
			//Message 
			return; 
		}

		FileName = Dlg.GetFileName(); 

		if( File.Create( FileName, STREAM_WRITE) == FALSE)
		{
			I3FATAL( "Couldn't CREATE File: %s", FileName);
			return ;
		}

		INT32	nGeomCnt = geomList.GetCount();

		Result = File.Write( &nGeomCnt, sizeof( INT32));
		if( Result == STREAM_ERR)
		{
			I3TRACE("ERROR : File.Write( &nGeomCnt)\n");
			return;
		}

		Rc	= File.Write( &scale, sizeof( REAL32));
		if( Rc == STREAM_ERR)
		{
			I3TRACE("ERROR : File.Write( &m_rMapScale)\n");
			return;
		}
		Result += Rc;

		for( INT32 i = 0; i< nGeomCnt; ++i)
		{
			I3SND_FMOD_POLYGON * pPoly = (I3SND_FMOD_POLYGON*)geomList.GetItem( i);

			Rc = File.Write( pPoly, sizeof( I3SND_FMOD_POLYGON));
			if( Rc == STREAM_ERR)
			{
				I3TRACE("ERROR :  File.Write( pPoly)\n");
				return;
			}
			Result += Rc;

			Rc = File.Write( pPoly->_pvVertexArray, sizeof( pPoly->_nVertexCount * sizeof( PERSIST_VEC3D)));
			if( Rc == STREAM_ERR)
			{
				I3TRACE("ERROR :File.Write( pPoly->_pvVertexArray)\n");
				return;
			}
			Result += Rc;
		}
	}*/
}

void Ci3GraphDlg::OnViewViewmapscale()
{
	// TODO: Add your command handler code here
	i3BoundBox Box;

	i3SceneGraphInfo * pInfo = m_TreeWnd.getSgInfo();
	if( pInfo == NULL)
	{
		MessageBox( "작업중인 파일이 없습니다.", "알림", MB_OK);
		return;
	}

	i3Node * pInstRoot = pInfo->getInstanceSg();

	pInstRoot->GetWrappingBound( &Box);

	REAL32 X = Box.GetMax()->x - Box.GetMin()->x;
	REAL32 Y = Box.GetMax()->y - Box.GetMin()->y;
	REAL32 Z = Box.GetMax()->z - Box.GetMin()->z;

	REAL32 scale = MAX( X, Y);
	scale = MAX( Z, scale);

	char conv[MAX_PATH];
	sprintf( conv, "X: %f, Y: %f, Z: %f\n\nMaxScale = %f\n\nMapScaleForMinimap: %f ", X, Y, Z, scale, MAX( X, Z));
	MessageBox( conv, "맵 크기", MB_OK);
}

BOOL Ci3GraphDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	return CDialog::PreCreateWindow(cs);
}


void Ci3GraphDlg::OnOptionGloballightsetting()
{
	CDlgLight dlg;

	if( dlg.DoModal() == IDCANCEL)
		return;

	// Light settint
	m_pFramework->SetLight( g_pConfig->getLightDiffuse(), g_pConfig->getLightSpecular(), g_pConfig->getShadowMapSize());
}

void Ci3GraphDlg::OnRasterizechainTonemapping()
{
	// TODO: Add your command handler code here
	GetMenu()->CheckMenuItem( ID_RASTERIZECHAIN_TONEMAPPING, m_pFramework->isEnable_ToneMapping() ? MF_UNCHECKED : MF_CHECKED );
	m_pFramework->ToggleRasterizeChain_ToneMapping();
}



void Ci3GraphDlg::OnFileImport()
{
	// TODO: Add your command handler code here
	TCHAR	Filter[] = {"X File(*.x)|*.x||"};
	CFileDialog dlg( TRUE, "x", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		Filter, this);
	if (IDOK != dlg.DoModal())
		return;

	CString FullPathName = dlg.GetPathName();
	ImportXFile(FullPathName);


	return;
}

void		Ci3GraphDlg::ImportXFile(const char * FullPathName)
{
	i3XFileParser	Parser;
	Parser.SetLogWnd(this);
	if ( !Parser.Open(FullPathName) )
	{
		AfxMessageBox("Import 실패!");
		return;
	}

	char	FileName[MAX_PATH];
	i3String::SplitFileName(FullPathName, FileName, TRUE);

	i3SceneGraphInfo *	pSgInfo	= Parser.GetSceneGraphInfo();
	i3Node *			pNode	= pSgInfo->getInstanceSg();

	pSgInfo->SetName(FileName);
	pNode->Instantiate();

	i3Scene::ModifyDynamicState( pNode, I3_PHYSIX_STATE_KINEMATIC, 0);


	m_ViewerWnd.Invalidate();

	CloseSceneRoot(); 
	I3_REF_CHANGE(m_pSgInfo, pSgInfo);
	if( m_pSgInfo != NULL)
	{
		Seti3sFileName(pSgInfo->GetName());
		InitSceneRoot(); 
	}

}