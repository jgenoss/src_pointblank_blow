#include "stdafx.h"
#include "GameInterface.h"
#include "Framework.h"
#include "GlobalVar.h"
#include "resource.h"
#include "Stage.h"

#define		PAGE_SIZE			(4096<<2)

void GameInterface::OnRegisterMeta(void)
{
	{
		// Error Log initialize
		i3Error::Init();
		i3Error::SetExceptionHandlerOption( I3MINDUMP_WITHDATASEGS );
		i3Error::SetChannelName( I3LOG_NOTICE, "error.log" );
		i3Error::SetChannelName( I3LOG_FATAL, "error.log");
		i3Error::SetChannelName( I3LOG_WARN, "error.log");

        i3Error::SetChannel( I3LOG_NOTICE, I3ERR_CHN_FILE | I3ERR_CHN_DEBUG );

		#if defined( I3_DEBUG) || defined( I3_RELEASE_PROFILE) //???????????????????
			i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_DEBUG | I3ERR_CHN_FILE | I3ERR_CHN_CALLBACK);
		#else
			i3Error::SetChannel( I3LOG_WARN, I3ERR_CHN_DEBUG | I3ERR_CHN_FILE);
		#endif

		i3Error::SetChannel( I3LOG_FATAL, I3ERR_CHN_CALLBACK | I3ERR_CHN_FILE);

		I3NOTICE( "=============================================");
		I3NOTICE( "Start Log( %s, %s )", __DATE__, __TIME__ );
		I3NOTICE( "=============================================");
	}

	i3FrameworkRegisterMetas();
	i3GuiRegisterMetas();

	{
		CFramework::RegisterMeta();
		CStage::RegisterMeta();
	}
}

bool GameInterface::OnInitInstance( void * pInstHandle, void * pScrHandle)
{
	g_hInstance = (HINSTANCE) pInstHandle;
	g_hWnd		= (HWND) pScrHandle;

	return true;
}

void GameInterface::OnExitInstance( void)
{
	i3System::TerminateSys();
}

void GameInterface::GetVideoConfig( i3VideoInfo * pVideo)
{
	pVideo->m_bFullScreen			= FALSE;
	pVideo->m_Width					= 640;
	pVideo->m_Height				= 480;
	pVideo->m_ZBufferBits			= 24;
	pVideo->m_StencilBufferBits		= 0;
	pVideo->m_RenderTargetFormat	= I3G_IMAGE_FORMAT_BGRX_8888;
	pVideo->m_bVSync				= FALSE;
	pVideo->m_RefreshRate			= (REAL32) 60.0f;		

	pVideo->m_MultiSampling			= 0;
	pVideo->m_MultiSamplingQuality	= 0;
}

bool GameInterface::CreatePreFramework(HWND hwnd)
{
	return true;
}

i3Framework * GameInterface::CreateFramework( i3Viewer * pViewer)
{
	i3Framework * pFramework = NULL;

	g_pViewer = pViewer;
	g_hWnd = pViewer->GetWindowHandle();

	pViewer->SetPendingUpdateEnable( true);

	//Framework
	{
		i3ResourceFile file;

		file.Load( "Template.i3Game");

		g_pFramework = (CFramework *) file.getKeyObject();
		pFramework = g_pFramework;

		I3ASSERT( g_pFramework != NULL);
		I3ASSERT( g_pFramework->IsExactTypeOf( CFramework::GetClassMeta()));

		g_pFramework->AddRef();
	}

	if( ! g_pFramework->Create( pViewer ))
	{
		I3FATAL("Could Not Create", "g_pFramework");
		return NULL;
	}

	return pFramework;
}

bool GameInterface::OnInitFramework( i3Framework * pFramework, const char * pszCmdLine)
{
	ShowWindow( g_hWnd, SW_SHOW);

	return true;
}

UINT32	GameInterface::GetViewportStyle(void)
{
	return WS_POPUP | WS_BORDER | WS_MINIMIZEBOX | WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU;
}

UINT32	GameInterface::GetIcon(void)
{
	return IDI_TEMPLATE;
}

bool GameInterface::OnDrive( i3Viewer * pViewer)
{
	if( g_pViewer != NULL )
	{	
		g_pViewer->UpdateInput();

		g_pViewer->Update();
        
		g_pViewer->Trace();
		
		g_pViewer->OnlyRender();
	}

	return true;
}

__declspec( dllexport)
i3GameInterface *	__stdcall i3CreateGameInterface( const char * pszRunPath)
{
	GameInterface * pInterface = new GameInterface;

	return pInterface;
}

#if defined( I3_WINDOWS) && defined( I3_DEBUG)//프로퍼티 쿼리 디버그 
INT32 GameInterface::OnQueryPropertyDlgCount(void)
{
	return 7;
}

void GameInterface::OnQueryPropertyDlg( HINSTANCE hInst, INT32 idx, i3ClassMeta ** ppMeta, i3GameObjPropertyDialog ** ppDlg)
{
	HINSTANCE hOld = AfxGetResourceHandle();

	AfxSetResourceHandle( hInst);

	switch( idx)
	{
		case 0 :		// CGameObjectJumper
			//*ppMeta = CGameObjectJumper::GetClassMeta();
			//*ppDlg = new CObjSpec_Warp;

			//(*ppDlg)->Create( CObjSpec_Warp::IDD, NULL);
			break;

		case 1 :		// TargetTrigger
			//*ppMeta = TargetTrigger::GetClassMeta();
			//*ppDlg = new CObjSpec_TargetTrigger;

			//(*ppDlg)->Create( CObjSpec_TargetTrigger::IDD, NULL);
			break;

		case 2 :
			//*ppMeta = TargetObject::GetClassMeta();
			//*ppDlg = new CObjSpec_TargetObject;

			//(*ppDlg)->Create( CObjSpec_TargetObject::IDD, NULL);
			break;
		case 3 :
			//*ppMeta = GameScreenObject::GetClassMeta();
			//*ppDlg = new CObjSpec_ScreenObject;
			//(*ppDlg)->Create( CObjSpec_ScreenObject::IDD, NULL);
			break;

		default :
			*ppMeta = NULL;
			*ppDlg = NULL;
			break;
	}

	AfxSetResourceHandle( hOld);
}
#endif

