// PanePreviewer.cpp : implementation file
//

#include "stdafx.h"
#include "i3UIEditor.h"
#include "PanePreviewer.h"
#include "Globalvariable.h"

// CPanePreviewer dialog

IMPLEMENT_DYNAMIC(CPanePreviewer, i3TDKDialogBase)
CPanePreviewer::CPanePreviewer(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanePreviewer::IDD, pParent)
{
	m_pViewer		= NULL;
	m_pFramework	= NULL;
	m_pSoundCtx		= NULL;

	m_nViewerWidth	= 1024;
	m_nViewerHeight = 768;
}

CPanePreviewer::~CPanePreviewer()
{
	I3_SAFE_RELEASE( m_pFramework );
	I3_SAFE_RELEASE( m_pViewer );
	I3_SAFE_RELEASE( m_pSoundCtx);

	g_pFramework = NULL;
}

void CPanePreviewer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PREVIEW_PLACEHOLDER, m_PreviewerPlaceHolderCtrl);
}


BEGIN_MESSAGE_MAP(CPanePreviewer, CDialog)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

// CPanePreviewer message handlers
void CPanePreviewer::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	if( ::IsWindow( m_PreviewerPlaceHolderCtrl.m_hWnd))
	{
		m_PreviewerPlaceHolderCtrl.SetWindowPos( NULL, 3, 20, m_nViewerWidth, m_nViewerHeight, SWP_NOACTIVATE | SWP_NOZORDER );
	}
}

void CPanePreviewer::SetViewerResolution( UINT32 width, UINT32 height, BOOL bUpdate)
{
	BOOL bNeedResize = FALSE;

	if( m_nViewerWidth != width || m_nViewerHeight != height)	bNeedResize = TRUE;

	m_nViewerWidth = width;
	m_nViewerHeight = height;

	if( bUpdate && bNeedResize)
	{
		if( ::IsWindow( m_PreviewerPlaceHolderCtrl.m_hWnd))
		{
			m_PreviewerPlaceHolderCtrl.SetWindowPos( NULL, 3, 20, m_nViewerWidth, m_nViewerHeight, SWP_NOACTIVATE | SWP_NOZORDER );
		}
	}
}

void CPanePreviewer::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call i3TDKDialogBase::OnPaint() for painting messages
	if( m_pViewer != NULL)
	{
		m_pViewer->Render();
	}
}

BOOL CPanePreviewer::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	SetWindowText( "미리보기");

	// Create Viewere
	{
		i3VideoInfo	video;
		video.m_bFullScreen			= false;

		video.m_Width				= (INT32)m_nViewerWidth;
		video.m_Height				= (INT32)m_nViewerHeight;
		

		m_pViewer = i3Viewer::NewObject();
		m_pViewer->Create( m_PreviewerPlaceHolderCtrl.m_hWnd, &video, I3I_DEVICE_MASK_CONTROLLER );
	}

	// Create Frame Work
	{
		m_pFramework = i3GuiEditorFramework::NewObject();
		m_pFramework->Create( m_pViewer );
	}

	// Create Sound Context
	{
		I3SND_CONTEXTCREATE_PARAM param;
		param.m_nPlatformData   = ( UINT64)m_PreviewerPlaceHolderCtrl.m_hWnd;
		param.m_nVoiceCount		= 10;
		param.m_bHasListener	= FALSE;
		param.m_nFlags			= 0;

		m_pSoundCtx = i3SoundContext::NewObject();
		m_pSoundCtx->Create( &param);

		i3SceneTracer * pTracer = m_pViewer->GetSceneTracer();
		pTracer->setSoundContext( m_pSoundCtx);
	}

	i3TDK::RegisterUpdate( m_hWnd, i3Node::GetClassMeta(), I3_TDK_UPDATE_SELECT);
	i3TDK::RegisterUpdate( m_hWnd, i3Node::GetClassMeta(), I3_TDK_UPDATE_SCENE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanePreviewer::OnUpdate( I3_TDK_UPDATE_INFO * pInfo )
{
	if( pInfo->m_hwndFrom == this->m_hWnd )	return ;
	if( pInfo->m_pObject == NULL)	return;

	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT || pInfo->m_Event & I3_TDK_UPDATE_SCENE)
	{
		if( pInfo->m_pObject->IsTypeOf( i3Node::GetClassMeta()))
		{
			i3UIManager * pManager = i3UI::getManager();
			i3Node		* pRoot = pManager->getSceneGraphNode();
			if( pRoot != NULL)
			{
				pRoot->RemoveAllChild();
				pRoot->AddChild((i3Node*)pInfo->m_pObject);

				Invalidate();
			}
		}
	}
}

LRESULT CPanePreviewer::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if( message == WM_TDK_UPDATE)
	{
		OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
	}

	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

BOOL CPanePreviewer::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN)
	{
		TranslateMessage( pMsg);

		DispatchMessage( pMsg );

		return TRUE;
	}

	return i3TDKDialogBase::PreTranslateMessage(pMsg);
}
