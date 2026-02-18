// i3Optimizer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "i3Optimizer.h"

//#define		PRINTF			I3TRACE
#define		PRINTF			printf

static i3Node *			g_pRoot = NULL;
static i3Node *			g_pCommonScene = NULL;
static i3IniParser *	g_pParser = NULL;	
static HWND				g_hwndMain = NULL;
static char				g_szWndClass[32] = "i3Optimizer_wndClass";
static i3Viewer *		g_pViewer = NULL;
static i3RenderContext * g_pContext = NULL;
static OptimizerInfo	g_OptInfo;
static i3StringList		g_FileList;
static i3StringList		g_ErrorReport;
static INT32			g_nProcessedFileCnt = 0;
static INT32			g_nSucceedFileCnt = 0;

void RecordError( const char *format, ...)
{
	char szTemp[256]= "";

	va_list marker;

	va_start( marker, format);

	vsprintf( szTemp, format, marker);

	g_ErrorReport.Add( szTemp);
}

int _tmain(int argc, _TCHAR* argv[])
{
	INT32 ret = 0;
	HINSTANCE hInstance;
	char *	pszPath = NULL;
	char	szBufExt[32] = "";	

	PRINTF( "\n=====================================\n");
	PRINTF( "i3Optimizer\n");
	PRINTF( "Copyright (c) 2007. Zepetto.Inc\n");
	PRINTF( "Programed by KOMET\n");
	PRINTF( "=====================================\n");

	//PRINTF( "Run - argc(%d)\n", argc);

	hInstance = (HINSTANCE) GetCurrentProcess();

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, 0)) 
	{
		ret = -1;
		goto LABEL_OUT;
	}

	/////////////////////////////////////////////////////////////////////////////
	//	First Initialize
	if( !MainInit())
	{
		PRINTF( "프로그램의 초기화에 실패했습니다.\r\n종료합니다."); 
		return -1;
	}
	
	//	Parser		
	g_pParser = i3IniParser::NewObject();
	I3ASSERT( g_pParser != NULL);

	pszPath = argv[ argc - 1];
	i3String::SplitFileExt( pszPath, szBufExt, sizeof( szBufExt));
	
	//	if help?
	if( argc == 1 || (i3String::Compare( "-help", pszPath) == 0) || (i3String::Compare( "-?", pszPath) == 0))
	{
		_printHelp();
		goto LABEL_OUT;		
	}

	//	ini 파일 먼저 파싱
	if( i3String::Compare( "ini", szBufExt) == 0)
	{
		if( !g_pParser->OpenFromFile( pszPath))
		{
			RecordError("Fail to open ini file. (%s)", pszPath);

			ret = -1;
			goto LABEL_OUT;
		}

		//	먼저 파싱
		_preRead( g_pParser);
	}
	else
	{
		RecordError("Error! Need (*.ini) File on argument tail.");

		ret = -1;
		goto LABEL_OUT;		
	}		
	
	//	Run to optimize mesh
	for(int i = 1; i < argc; i++)
	{		
		i3Node *	pRoot = NULL;
		pszPath = (char *) argv[i];
		
		if( !_processFile( pszPath))
		{
			ret = -1;
			goto LABEL_OUT;
		}
	}

	//	ini에 메시 파일이 있다면 처리
	for( INT32 i = 0; i < g_FileList.GetCount(); i++)
	{		
		pszPath = (char *) g_FileList.GetItem( i);

		if( !_processFile( pszPath))
		{			
			ret = -1;
			goto LABEL_OUT;
		}		
	}
		
LABEL_OUT:
	
	PRINTF( "\n==================== Result Report ====================\n");
	PRINTF( "Processed file count : %d\n", g_nProcessedFileCnt);
	PRINTF( "Succeed file count : %d\n", g_nSucceedFileCnt);

	PRINTF( "\nError count : %d\n", g_ErrorReport.GetCount());

	for( INT32 i = 0; i < g_ErrorReport.GetCount(); i++)
	{
		PRINTF( "-- ERROR(%d) : %s\n", i+1, (char *) g_ErrorReport.GetItem( i));
	}

	if( ret == 0)
		PRINTF( "\nDone.\n");
	else
		PRINTF( "\nFailed.\n");

	//	Release all
	Cleanup();

	//	g_OptInfo.m_bIgnoreRun가 TRUE면 무조건 성공으로 프로그램 종료
	if( g_OptInfo.m_bIgnoreRun)	
		return 0;

	return ret;
}

void _printHelp(void)
{
	PRINTF( "[Infomation]\n");
	PRINTF( "i3Optimizer [source].i3s ... [config].ini\n");
	PRINTF( "ex) i3Optimizer sample.i3s sample.ini\n");
	PRINTF( "\nThank you!\n");
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= g_szWndClass;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hWnd = CreateWindow( g_szWndClass, "", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   g_hwndMain = hWnd;

   //ShowWindow(hWnd, nCmdShow);
   //UpdateWindow(hWnd);

   return TRUE;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL MainInit(void)
{
	i3MemoryInit();

	i3OptRegisterMetas();	

	SetDefaultOptimize();

	i3Texture::SetConcreteClass( i3TextureDX::GetClassMeta());
	i3VertexArray::SetConcreteClass( i3VertexArrayMem::GetClassMeta());

	g_pViewer = i3Viewer::NewObject();
	I3ASSERT( g_pViewer != NULL );

	g_FileList.Clear();
	g_ErrorReport.Clear();

	PRINTF( "\nInitializing...\nPlease wait...\n\n");

	i3VideoInfo video;

	video.m_Width = 100;
	video.m_Height = 100;

	return g_pViewer->Create( (void *) g_hwndMain, &video,
					I3I_DEVICE_MASK_CONTROLLER | I3I_DEVICE_MASK_KEYBOARD, 
					TRUE);
	
	
	//	RenderContext 생성
	/*{
		i3VideoInfo Format;

		Format.m_bFullScreen = false;
		Format.m_FrameBufferCount = 1;
		Format.m_Width = 10;
		Format.m_Height = 10;
		Format.m_RenderTargetFormat = I3G_IMAGE_FORMAT_BGRX_8888;
		Format.m_ZBufferBits = 16;
		Format.m_StencilBufferBits = 0;

		PRINTF( "Initializing...\nPlease wait...\n\n");

		g_pContext = i3RenderContext::NewObject();

		return g_pContext->Create( &Format, (void *) g_hwndMain);		
	}*/
}

void SetDefaultOptimize(void)
{
	//	Optimizer 정보 초기화
	g_OptInfo.m_bOutputInfo = TRUE;
	g_OptInfo.m_nOutputFile = OUTPUT_FILE_NEW;
	g_OptInfo.m_bIgnoreRun = FALSE;	
}

void Cleanup(void)
{
	I3_SAFE_RELEASE( g_pViewer);
	I3_SAFE_RELEASE( g_pContext);
	I3_SAFE_RELEASE( g_pParser);
}

i3Node * _openMeshFile( const char * pszFileName)
{
	i3SceneFile file;

	//	Read *.i3s Mesh File
	if( file.Load( pszFileName) == STREAM_ERR)
	{
		RecordError( "File load error \"%s\"", pszFileName);

		return NULL;
	}
	
	g_pCommonScene	= file.GetCommonSceneGraph();
	if( g_pCommonScene != NULL)
	{
		g_pCommonScene->AddRef();
	}
		
	g_pRoot			= file.GetSceneGraph();
	if( g_pRoot != NULL)
	{
		g_pRoot->AddRef();

		if( g_OptInfo.m_bOutputInfo) PRINTF( "\nFile load ok \"%s\" \n", pszFileName);
	}
	else
	{
		RecordError( "Error - Fail to get root node \"%s\"", pszFileName);
	}

	return g_pRoot;
}

BOOL _saveMeshFile( const char * pszFileName)
{
	i3SceneFile file;
	char szSaveFileName[256];

	if( g_OptInfo.m_nOutputFile == OUTPUT_FILE_OVER)
	{
		i3String::Copy( szSaveFileName, pszFileName, 256 );			
	}
	else if( g_OptInfo.m_nOutputFile == OUTPUT_FILE_NEW)
	{
		//	파일명 뒤에 새로운 이름을 붙인다.
		i3String::AddPostfix( szSaveFileName, 256, pszFileName, OPT_NEW_POST_FIX);
	}
	else
	{
		PRINTF("File not saved.\n"); 
		return TRUE;
	}

	if( g_pRoot)
	{
		i3SceneGraphInfo * pInfo = i3SceneGraphInfo::NewObject();

		pInfo->setCommonSg( g_pCommonScene);
		pInfo->setInstanceSg( g_pRoot);

		file.setSceneGraphInfo( pInfo);
		//file.SetSceneGraph( g_pRoot);
		
		if( file.Save( szSaveFileName) == STREAM_ERR)
		{
			RecordError("Fail to save file \"%s\"", szSaveFileName);

			I3_SAFE_RELEASE( pInfo);

			return FALSE;
		}
		else
		{
			if( g_OptInfo.m_bOutputInfo) PRINTF( "File save ok \"%s\"\n", szSaveFileName);
		}

		I3_SAFE_RELEASE( pInfo);
	}
	else
	{
		RecordError("Fail to save. not set mesh root.."); 
		return FALSE;
	}

	return TRUE;
}

void _preRead( i3IniParser * pParser)
{
	if( !pParser) return;

	///////////////////////////////////////////////////////////////////////////		OPT_SECTION_OPTION
	if( pParser->ReadSection( OPT_SECTION_OPTION))
	{
		//	OPT_OUTPUT_INFO
		pParser->GetValue( OPT_OUTPUT_INFO, g_OptInfo.m_bOutputInfo, &g_OptInfo.m_bOutputInfo);

		if( g_OptInfo.m_bOutputInfo) PRINTF( "\n(%s)\n", OPT_SECTION_OPTION);

		//	OPT_OUTPUT_FILE
		char szTemp[256] = OPT_OUTPUT_FILE_NO;
		pParser->GetValue( OPT_OUTPUT_FILE, szTemp, sizeof( szTemp));

		if( i3String::Compare( OPT_OUTPUT_FILE_NEW, szTemp) == 0)
			g_OptInfo.m_nOutputFile = OUTPUT_FILE_NEW;
		if( i3String::NCompare( OPT_OUTPUT_FILE_OVER, szTemp, 4) == 0)
			g_OptInfo.m_nOutputFile = OUTPUT_FILE_OVER;
		if( i3String::Compare( OPT_OUTPUT_FILE_NO, szTemp) == 0)
			g_OptInfo.m_nOutputFile = OUTPUT_FILE_NO;
	
		if( g_OptInfo.m_bOutputInfo) PRINTF( ": Output File = %s\n", szTemp);

		//	OPT_IGNORE_RUN
		pParser->GetValue( OPT_IGNORE_RUN, g_OptInfo.m_bIgnoreRun, &g_OptInfo.m_bIgnoreRun);
		if( g_OptInfo.m_bOutputInfo) PRINTF( ": Ignore Run = %d\n", g_OptInfo.m_bIgnoreRun);
	}

	///////////////////////////////////////////////////////////////////////////		OPT_SECTION_FILES
	if( pParser->ReadSection( OPT_SECTION_FILES))
	{
		INT32 idx = pParser->GetSectionIndex( OPT_SECTION_FILES);
				
		i3IniSectionInfo * pSectionInfo = pParser->GetSection( idx);
		I3ASSERT( pSectionInfo != NULL);
		
		//	ini에 등록된 파일이름 리스트에 저장
		for( UINT32 i = 0; i < pSectionInfo->NumLines; i++)
		{
			i3IniValue * pValue = pParser->GetValueByIndex( i);

			if( i3String::Compare( OPT_FILES_MESH, pValue->szLabel) == 0)
				g_FileList.Add( pValue->szValue);	
		}
	}

	if( pParser->ReadSection( "AllExt"))
	{
		INT32 idx = pParser->GetSectionIndex( "AllExt");
				
		i3IniSectionInfo * pSectionInfo = pParser->GetSection( idx);
		I3ASSERT( pSectionInfo != NULL);
		
		//	ini에 등록된 파일이름 리스트에 저장
		for( UINT32 i = 0; i < pSectionInfo->NumLines; i++)
		{
			i3IniValue * pValue = pParser->GetValueByIndex( i);

			if( i3String::Compare( "Ext", pValue->szLabel) == 0)
			{
				HANDLE hSearch; 
				DWORD dwAttrs; 
				WIN32_FIND_DATA FileData; 
				BOOL fFinished = FALSE; 
			
				// Start searching for .TXT files in the current directory. 
				hSearch = FindFirstFile( pValue->szValue, &FileData); 
				if (hSearch == INVALID_HANDLE_VALUE) 
				{ 
					I3TRACE("No .i3s files found."); 
					return;
				} 
						
				// Copy each .TXT file to the new directory 
				// and change it to read only, if not already. 
						
				while (!fFinished) 
				{ 
					dwAttrs = GetFileAttributes(FileData.cFileName); 
					if (!(dwAttrs & FILE_ATTRIBUTE_DIRECTORY)) 
					{ 
						g_FileList.Add( FileData.cFileName);	
					}

					if (!FindNextFile(hSearch, &FileData)) 
					{
						if (GetLastError() == ERROR_NO_MORE_FILES) 
						{ 
							I3TRACE( "No more .i3s files.");
							fFinished = TRUE; 
						} 
						else 
						{ 
							printf("Couldn't find next file."); 
							return;
						} 
					}
				} 
						
				// Close the search handle. 
				FindClose(hSearch);
			}
		}
	}
}

BOOL _checkIgnoreRun( void)
{
	if( !g_OptInfo.m_bIgnoreRun)	
	{
		return FALSE;
	}
	else
	{
		PRINTF( "Error Ignored.\n");
	}

	return TRUE;
}

BOOL _processFile( const char * pszPath)
{ 
	BOOL ret = TRUE;
	char	szBufExt[32] = "";	

	i3String::SplitFileExt( pszPath, szBufExt, sizeof( szBufExt));
	if( i3String::Compare( "i3s", szBufExt) == 0)
	{
		//	메시 파일 열기
		if( _openMeshFile( pszPath) == NULL)
		{
			ret = _checkIgnoreRun();
		}
		else
		{
			if( g_pRoot != NULL)
			{
				//	Parsing and Optimizing
				if( _runOpt( g_pParser, pszPath))
				{			
					//	다시 저장
					if( !_saveMeshFile( pszPath))	
					{
						ret = _checkIgnoreRun();
					}
					else
					{	//	성공
						g_nSucceedFileCnt++;
						ret = TRUE;
					}
				}
				else
				{
					ret = _checkIgnoreRun();
				}
			}
			else
			{				
				ret = _checkIgnoreRun();
			}
		}
	}
	else if( i3String::Compare( "ini", szBufExt) == 0)
	{
		//	Skipped
		return ret;
	}		
	else
	{
		RecordError( "Warning! Invalid file name (%s)", pszPath);
		ret = _checkIgnoreRun();
	}

	g_nProcessedFileCnt++;

	return ret;
}

BOOL _runOpt( i3IniParser * pParser, const char * pszFileName /*= NULL*/)
{
	BOOL ret = FALSE;
	const char * pSectionName = NULL;

	if( !pParser || !g_pRoot)		return FALSE;

	for( INT32 i = 0; i < pParser->GetSectionCount(); i++)
	{
		//	파서에서 현재 섹션의 이름을 가져온다.
		if( pParser->ReadSectionByIndex( i))
			pSectionName = pParser->GetCurSectionName();
		else
			pSectionName = NULL;

		//	현재 섹션에 해당하는 Class가 실제 엔진에 존재하는지 검사
		i3ClassMeta * pMeta = i3ClassMeta::FindClassMetaByName( pSectionName);
		if( pMeta != NULL)
		{
			i3SceneOptimizer * pOpt = (i3SceneOptimizer *) CREATEINSTANCE( pMeta);

			if( pOpt->IsTypeOf( i3SceneOptimizer::GetClassMeta()) == FALSE)
			{
				//  실행하지 못하는 Class.
				//  Warning 처리
				RecordError( "warning! Not register meta class in i3Engine(%s)", pSectionName);
				continue;
			}

			pOpt->AddRef();

			if( g_OptInfo.m_bOutputInfo) PRINTF( "Step %d: (%s) Processing...", i, pSectionName);

			//	모든 Opt 클래스에 PROPERTY_OUTPUT 필드로 파일이름을 전송한다.
			pOpt->SetProperty( PROPERTY_OUTPUT, (char *) pszFileName);

			//	현재 섹션의 값 갯수만큼 해당 Optimize 엔진 클래스를 호출하여 값을 넘겨준다.
			for( INT32 idx = 0; idx < (INT32) pParser->GetValueCount(); idx++)
			{		
				i3IniValue * pValue = pParser->GetValueByIndex( idx);
				
				//	ini의 값 설정을 먼저합니다.
				pOpt->SetProperty( pValue->szLabel , pValue->szValue);  

				//if( g_OptInfo.m_bOutputInfo) PRINTF( "SetProperty : %s = %s\n", pValue->szLabel , pValue->szValue);  
			}    
  
			//	모든 노드를 돌며 Optimize 함수를 실행
			pOpt->Trace( g_pRoot);

			//	SceneRoot가 바뀠으면
			if( g_pRoot != pOpt->getTraceRoot() && pOpt->getTraceRoot() != NULL)
			{
				g_pRoot = pOpt->getTraceRoot();
				//g_pRoot->AddRef();
			}

			pOpt->Release();

			if( g_OptInfo.m_bOutputInfo) PRINTF( "Ok\n", i);
		}
		else
		{
			if( i3String::Compare( pSectionName, OPT_SECTION_OPTION) != 0 &&
				i3String::Compare( pSectionName, OPT_SECTION_FILES) != 0 &&
				i3String::Compare( pSectionName, "AllExt") != 0)
			{
				RecordError( "Warning! Not exist class (%s)", pSectionName);

				if( !g_OptInfo.m_bIgnoreRun)	
					return FALSE;
			}
		}
	}	

	return TRUE;
}