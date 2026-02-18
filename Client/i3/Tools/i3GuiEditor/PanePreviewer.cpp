// PanePreviewer.cpp : implementation file
//

#include "stdafx.h"
#include "i3GuiEditor.h"
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

void CPanePreviewer::ReplaceControls( INT32	cx, INT32 cy )
{
	//INT32	btncx, btncy, y;
	//INT32	btncy = 0;
	CRect	btnRC;

	if( cx == -1 )
	{
		CRect rc;

		GetClientRect( &rc );
		cx = rc.Width();
		cy = rc.Height();
	}

	if( ::IsWindow( m_PreviewerPlaceHolderCtrl.m_hWnd))
	{
		INT32	viewercx, viewercy;

		//play / stop 버튼의 위치설정이 들어갈 부분입니다.
		{
		}

		/*viewercx = cx - 6;
		viewercy = cy - btncy - 9 - 20;*/
		viewercx = 1024;
		viewercy = 768;

		m_PreviewerPlaceHolderCtrl.SetWindowPos( NULL, 3, 20, viewercx, viewercy, SWP_NOACTIVATE | SWP_NOZORDER );

		m_pViewer->Resize( viewercx, viewercy );
	}
}

// CPanePreviewer message handlers
void CPanePreviewer::OnSize(UINT nType, int cx, int cy)
{
	i3TDKDialogBase::OnSize(nType, cx, cy);

	ReplaceControls( cx, cy );
	// TODO: Add your message handler code here
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

		video.m_Width				= 1024;
		video.m_Height				= 768;
		

		m_pViewer = i3Viewer::new_object();
		m_pViewer->Create( m_PreviewerPlaceHolderCtrl.m_hWnd, &video, I3I_DEVICE_MASK_CONTROLLER );
	}

	// Create Frame Work
	{
		m_pFramework = i3GuiEditorFramework::new_object();
		m_pFramework->Create( m_pViewer );
	}

	// Create Sound Context
	{
		I3SND_CONTEXTCREATE_PARAM param;
		param.m_nPlatformData   = ( UINT64)m_PreviewerPlaceHolderCtrl.m_hWnd;
		param.m_nVoiceCount		= 10;
		param.m_bHasListener	= FALSE;
		param.m_nFlags			= 0;

		m_pSoundCtx = i3SoundContext::new_object();
		m_pSoundCtx->Create( &param);

		i3SceneTracer * pTracer = m_pViewer->GetSceneTracer();
		pTracer->setSoundContext( m_pSoundCtx);
	}

	i3TDK::RegisterUpdate( m_hWnd, i3Node::static_meta(), I3_TDK_UPDATE_SELECT);
	i3TDK::RegisterUpdate( m_hWnd, i3Node::static_meta(), I3_TDK_UPDATE_SCENE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPanePreviewer::OnUpdate( I3_TDK_UPDATE_INFO * pInfo )
{
	if( pInfo->m_hwndFrom == this->m_hWnd )	return ;
	if( pInfo->m_pObject == NULL)	return;

	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		if( i3::kind_of<i3Node*>(pInfo->m_pObject)) //->IsTypeOf( i3Node::static_meta()))
		{
			if( g_pRoot->getSceneGraphNode() != NULL)
			{
				g_pRoot->getSceneGraphNode()->RemoveAllChild();
				g_pRoot->getSceneGraphNode()->AddChild( (i3Node*)pInfo->m_pObject);
			}

			Invalidate();
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_SCENE)
	{
		if( i3::kind_of<i3Node*>(pInfo->m_pObject)) //->IsTypeOf( i3Node::static_meta()))
		{
			if( g_pRoot->getSceneGraphNode() != NULL)
			{
				g_pRoot->getSceneGraphNode()->RemoveAllChild();
				g_pRoot->getSceneGraphNode()->AddChild( (i3Node*)pInfo->m_pObject);
			}

			Invalidate();
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
