// i3EffectEditorDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "i3EffectEditor.h"
#include "i3EffectEditorDlg.h"
#include ".\i3effecteditordlg.h"
#include "i3EffectProject.h"
#include "GlobalVariable.h"
#include "SpaceConvertConfirmDialog.h"
#include "SelectBoneDialog.h"
#include "ExportOptionDialog.h"
#include "DlgOption.h"
#include "i3Viewer.h"

#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/compare/generic_compare.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
//	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
//	virtual void OnOK();
//	virtual void OnCancel();
public:
//	afx_msg void OnToolsOptions();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//	ON_COMMAND(ID_TOOLS_OPTIONS, OnToolsOptions)

END_MESSAGE_MAP()


// Ci3EffectEditorDlg 대화 상자



Ci3EffectEditorDlg::Ci3EffectEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ci3EffectEditorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	m_pViewer = NULL;

	m_bPlay = TRUE;
	m_bPaused = FALSE;

	m_pRoot = NULL;
	m_pTimeSeq = NULL;
	m_pEffectTrans = NULL;
	m_pWorld = NULL;
	m_pSky = NULL;
	m_pModel = NULL;

	m_szFileName[0] = 0;

	g_pMainWnd = this;

	m_pSelectedBone = NULL;
	m_bAttach = FALSE;
}

void Ci3EffectEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Ci3EffectEditorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_OPENSCENEASWORLD, OnFileOpensceneasworld)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
	ON_COMMAND(ID_FILE_SAVE32774, OnFileSave32774)
	ON_COMMAND(ID_FILE_OPENSCENE, OnFileOpenscene)
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_CONTROL_PLAY, OnControlPlay)
	ON_COMMAND(ID_FILE_OPENSCENEASSKY, OnFileOpensceneassky)
	ON_COMMAND(ID_FILE_OPENSCENEASMODEL, OnFileOpensceneasmodel)
	ON_COMMAND(ID_FILE_EXPORTTOI3S, OnFileExporttoi3s)
	ON_WM_ACTIVATEAPP()
	ON_COMMAND(ID_FILE_REMOVEWORLD, OnFileRemoveworld)
	ON_COMMAND(ID_FILE_REMOVESKY, OnFileRemovesky)
	ON_COMMAND(ID_FILE_NEWPROJECT, OnFileNewproject)
	ON_COMMAND(ID_MODEL_SELECTBONE, OnModelSelectbone)
	ON_UPDATE_COMMAND_UI(ID_MODEL_SELECTBONE, OnUpdateModelSelectbone)
	ON_COMMAND(ID_FILE_REMOVEMODEL, OnFileRemovemodel)
	ON_COMMAND(ID_TOOLS_OPTIONS, OnToolsOptions)
	ON_COMMAND(ID_TOOLS_BGCOLOR, OnToolsBgcolor)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// Ci3EffectEditorDlg 메시지 처리기

void Ci3EffectEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Ci3EffectEditorDlg::OnPaint() 
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
HCURSOR Ci3EffectEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL Ci3EffectEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CRect rect;

	{
		m_hAccel = ::LoadAccelerators( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDR_ACCELERATOR1));
	}

	InitRenderCS();

	GetClientRect( &rect);
	
	{
		m_pRoot = i3AttrSet::new_object();
		m_pRoot->SetName( "Root");

		m_pTimeSeq = i3TimeSequence::new_object();
		m_pTimeSeq->SetName( "i3TimeSequence");

		m_pEffectTrans = i3Transform::new_object();
		m_pEffectTrans->AddFlag( I3_NODEFLAG_VOLATILE);
		m_pEffectTrans->SetName( "m_pEffectTrans");
		m_pEffectTrans->AddChild( m_pTimeSeq);

		m_pWorld = i3AttrSet::new_object();
		m_pWorld->SetName( "World Root");

		m_pSky = i3Node::new_object();
		m_pSky->SetName( "Sky Root");

		m_pModel = i3Node::new_object();
		m_pModel->SetName( "Model Root");

		m_pRoot->AddChild( m_pWorld);
		m_pRoot->AddChild( m_pSky);
		m_pRoot->AddChild( m_pModel);
		m_pRoot->AddChild( m_pEffectTrans);

		{
			i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();

			pAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
			pAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR);

			m_pRoot->AddAttr( pAttr);
		}
	}

	{
		m_BaseSplitter.Create( 0, WS_VISIBLE | WS_CHILD, rect, 10, this, 100);

		m_Split1.Create( I3_SPLS_EX_VERT, WS_VISIBLE | WS_CHILD, rect, 10, &m_BaseSplitter, 101);

		m_Split2.Create( 0, WS_VISIBLE | WS_CHILD, rect, 10, &m_Split1, 100);
		m_Split1.AddPane( &m_Split2);
		m_Split3.Create( 0, WS_VISIBLE | WS_CHILD, rect, 10, &m_Split1, 101);
		m_Split1.AddPane( &m_Split3);

		m_Split4.Create( I3_SPLS_EX_VERT, WS_VISIBLE | WS_CHILD, rect, 10, &m_Split3, 103);

		// 좌측 
		{
			{
				m_WndFoldCtrl.Create( "Particle", WS_CHILD | WS_VISIBLE, rect, &m_BaseSplitter, 101);

				m_ParticleSpecParticle.Create( MAKEINTRESOURCE( IDD_PARTICLESPEC_PARTICLE), &m_WndFoldCtrl);
				m_ParticleSpecObject.Create( MAKEINTRESOURCE( IDD_PARTICLESPEC_OBJECT), &m_WndFoldCtrl);
				m_ParticleSpecEmitter.Create( MAKEINTRESOURCE( IDD_PARTICLESPEC_EMITTER), &m_WndFoldCtrl);
				m_ParticleSpecPosition.Create( MAKEINTRESOURCE( IDD_PARTICLESPEC_POSITION), &m_WndFoldCtrl);
				m_ParticleSpecRotate.Create( MAKEINTRESOURCE( IDD_PARTICLESPEC_ROTATE), &m_WndFoldCtrl);
				m_ParticleSpecScale.Create( MAKEINTRESOURCE( IDD_PARTICLESPEC_SCALE), &m_WndFoldCtrl);
				m_ParticleSpecColor.Create( MAKEINTRESOURCE( IDD_PARTICLESPEC_COLOR), &m_WndFoldCtrl);

				m_WndFoldCtrl.AddPane( &m_ParticleSpecParticle, "Global");
				m_WndFoldCtrl.AddPane( &m_ParticleSpecObject, "Object");
				m_WndFoldCtrl.AddPane( &m_ParticleSpecEmitter, "Emitter");
				m_WndFoldCtrl.AddPane( &m_ParticleSpecPosition, "Position");
				m_WndFoldCtrl.AddPane( &m_ParticleSpecRotate, "Rotation");
				m_WndFoldCtrl.AddPane( &m_ParticleSpecScale, "Scale");
				m_WndFoldCtrl.AddPane( &m_ParticleSpecColor, "Color");
			}

			m_BaseSplitter.AddPane( &m_WndFoldCtrl);
			m_BaseSplitter.AddPane( &m_Split1);
		}

		//  우측 상단 (Rendering Window + Tree Window)
		{
			m_wndViewerDummy.Create( WS_CHILD | WS_VISIBLE, rect, &m_Split2, 101);

			m_Split2.AddPane( &m_wndViewerDummy);

			// 우측 (Property Window)
			{
				//m_PropertyHolder.AddPage( &m_PropertyKeyframe);

				m_PropertyHolder.Create( WS_CHILD | WS_VISIBLE, rect, &m_Split2, 102);

				m_Split2.AddPane( &m_PropertyHolder);
			}
		}

		// 우측 하단 ( TimeTrack Property + TimeTrack Control)
		{
			// Time Track
			{
				m_TimeTrackCtrl.Create( WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL, rect, &m_Split3, 101);

				m_Split3.AddPane( &m_Split4);
				m_Split3.AddPane( &m_TimeTrackCtrl);

				m_TimeTrackCtrl.SetTimeSequence( m_pTimeSeq);
			}
		}

		{
			// 우측 하단의 왼쪽 
			m_TimeTrackSpec.Create( MAKEINTRESOURCE( IDD_TIMETRACKSPEC), &m_Split4);
			m_PlayControlDlg.Create( MAKEINTRESOURCE( IDD_PLAYPANEL), &m_Split4);

			m_Split4.AddPane( &m_PlayControlDlg);
			m_Split4.AddPane( &m_TimeTrackSpec);
		}

		m_BaseSplitter.LoadWindowInfo( "i3EffectEditor.CFG");
		m_BaseSplitter.Rearrange();
	}

	{
		m_PropertyKeyframe.Create( MAKEINTRESOURCE( IDD_PROPERTY_KEYFRAME), &m_PropertyHolder);
		m_PropertyHolder.AddPanel( &m_PropertyKeyframe);
	}

	//{
		i3VideoInfo video;

		m_pViewer = i3Viewer::new_object();
		m_pViewer->Create( (void *) m_wndViewerDummy.m_hWnd, &video, I3I_DEVICE_MASK_CONTROLLER);
		m_pViewer->SetPendingUpdateEnable( false);
		
		i3RenderContext * pCtx = m_pViewer->GetRenderContext();


		m_pFramework = i3ViewerFramework::new_object_ref();
		m_pFramework->Create( m_pViewer);

		m_pViewer->Play();
		m_pFramework->Play();

		m_pFramework->SetGridShow( TRUE);
		m_wndViewerDummy.SetViewer( m_pViewer);

		m_pFramework->SetSceneGraph( m_pRoot);

		{
			VEC3D vmin, vmax;

			i3Vector::Set( &vmin, -3.0f, -3.0f, -3.0f);
			i3Vector::Set( &vmax,  3.0f,  3.0f, 3.0f);

			m_pFramework->SetBound( &vmin, &vmax);
		}

		{
			//COLOR col;
			//i3Color::Set( &col, 1.0f,0.0f,0.0f,1.0f);
			//pCtx->SetClearColor(&col);
		}

		m_pFramework->SetCameraProperty( 15.0f, 1.0f, 70000.0f);
	//}

	SetFPS( g_FPS);

	SetIcon( m_hIcon, TRUE);
	SetIcon( m_hIcon, FALSE);

	i3TDK::RegisterUpdate( m_hWnd, i3TimeSequenceInfo::static_meta(), I3_TDK_UPDATE_ALL);
	i3TDK::RegisterUpdate( m_hWnd, i3Particle::static_meta(), I3_TDK_UPDATE_ALL);

	//m_pCOL_ClearColor = new i3TDKRibbonColor( ID_TOOLS_BGCOLOR, "Clear", TRUE, 2, 2);
	//m_pCOL_ClearColor->setColor( & g_pOption->m_BackColor);
	//m_pCOL_ClearColor->SetVisible(FALSE);


	COLOR col;
	i3Color::Set( &col,  &g_pOption->m_BackColor);
	pCtx->SetClearColor(&col);



	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void Ci3EffectEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_BaseSplitter.m_hWnd))
	{
		m_BaseSplitter.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

void Ci3EffectEditorDlg::OnDestroy()
{
	UnloadProject();

	I3_SAFE_RELEASE( m_pWorld);
	I3_SAFE_RELEASE( m_pSky);
	I3_SAFE_RELEASE( m_pModel);
	I3_SAFE_RELEASE( m_pTimeSeq);
	I3_SAFE_RELEASE( m_pEffectTrans);
	I3_SAFE_RELEASE( m_pRoot);

	if( IsIconic() == FALSE)
	{
		m_BaseSplitter.SaveWindowInfo( "i3EffectEditor.CFG");
	}

	//I3_SAFE_RELEASE( m_pFramework);
	I3_SAFE_RELEASE( m_pViewer);

	CDialog::OnDestroy();
}

void Ci3EffectEditorDlg::OnTimer(UINT nIDEvent)
{
	if(m_bPlay && ( ::IsWindow( m_wndViewerDummy.m_hWnd)))
	{
		m_wndViewerDummy.Invalidate(0);
	}

	CDialog::OnTimer(nIDEvent);
}

i3Node * Ci3EffectEditorDlg::LoadI3Scene( const char * pszPath)
{
	i3SceneFile file;

	if(file.Load( pszPath) == STREAM_ERR)
	{
		DebugBox( NULL, "Could not open %s I3S file.", pszPath);
		return  NULL;
	}

	file.GetSceneGraph()->AddRef();
	
	return file.GetSceneGraph();
}

BOOL Ci3EffectEditorDlg::SaveI3Scene( const char * pszPath, i3Node * pNode)
{
	i3SceneFile file;
	i3::vector<i3PersistantElement*> list;

	list.push_back( pNode);

	file.SetObjects( list);
	file.SetSceneGraph( pNode);

	if( file.Save( pszPath) == STREAM_ERR)
	{
		DebugBox( m_hWnd, "Could not save %s i3Engine Scene file.");
		return FALSE;
	}

	return TRUE;
}

void Ci3EffectEditorDlg::UnloadProject(void)
{
	OnFileRemoveworld();
	OnFileRemovesky();
	OnFileRemovemodel();

	i3NamedResourceManager::Reset();

	g_pParticleRender->RemoveAllGroups();

	i3TDK::Update( m_hWnd, I3_TDK_UPDATE_REMOVE, m_pTimeSeq);
}

BOOL Ci3EffectEditorDlg::LoadBinaryProject( const char * pszPath)
{
	i3ResourceFile file;
	INT32 i;
	i3PersistantElement * pObj;
	i3EffectProject * pProject;

	UnloadProject();

	if( file.Load( pszPath) == STREAM_ERR)
	{
		AfxMessageBox( "프로젝트를 읽어 들일 수 없습니다.");
		return FALSE;
	}

	pProject = NULL;
	for( i = 0; i < file.GetObjectCount(); i++)
	{
		pObj = file.GetObject( i);

		if( i3::same_of<i3EffectProject*>(pObj)) //->IsExactTypeOf( i3EffectProject::static_meta()))
		{
			pProject = (i3EffectProject *) pObj;
		}
	}

	if( pProject == NULL)
	{
		AfxMessageBox( "올바른 Effect Project 파일이 아닙니다.");
		return FALSE;
	}

	{
		i3Node * pNode = pProject->GetWorld();

		SetWorld( pNode);

//		if( pNode != NULL)
//			pNode->Release();
	}

	{
		i3Node * pNode = pProject->GetSky();

		SetSky( pNode);

//		if( pNode != NULL)
//			pNode->Release();
	}

	//{
		i3Node * pNode = pProject->GetModel();

		SetModel( pNode);

//		if( pNode != NULL)
//			pNode->Release();
	//}

	if( pProject->GetTimeSeqCount() > 0)
	{
		i3TimeSequence * pTimeSeq;

		pTimeSeq = (i3TimeSequence *) pProject->GetTimeSeq( 0);
		SetTimeSequence( pTimeSeq);

//		pTimeSeq->Release();

		//UpdateAllTimeTrack();
	}

	return TRUE;
}

 BOOL Ci3EffectEditorDlg::LoadXMLProject(const char * pszPath)
 {
	i3PersistantElement * pObj;
	i3EffectProject * pProject;

 	i3XMLFile file;
	if (file.Load(pszPath) == STREAM_ERR)
	{
		AfxMessageBox("XML 읽어 들일 수 없습니다.");
		return FALSE;
	}
 
	pProject = NULL;
 	for (INT32 i = 0; i < file.GetObjectCount(); i++)
 	{
 		pObj = file.GetObject(i);
 
 		if (i3::same_of<i3EffectProject*>(pObj)) //->IsExactTypeOf( i3EffectProject::static_meta()))
 		{
 			pProject = (i3EffectProject *)pObj;
 		}
 	}

	if (pProject == NULL)
	{
		AfxMessageBox("올바른 Effect Project 파일이 아닙니다.");
		return FALSE;
	}

	{
		i3Node * pNode = pProject->GetWorld();

		SetWorld(pNode);
	}

	{
		i3Node * pNode = pProject->GetSky();

		SetSky(pNode);
	}

	i3Node * pNode = pProject->GetModel();
	SetModel(pNode);

	if (pProject->GetTimeSeqCount() > 0)
	{
		i3TimeSequence * pTimeSeq;

		pTimeSeq = (i3TimeSequence *)pProject->GetTimeSeq(0);
		SetTimeSequence(pTimeSeq);
	}
 }

BOOL Ci3EffectEditorDlg::SaveBinaryProject( const char * pszPath)
{
	i3EffectProject* project = i3EffectProject::new_object();
	i3ResourceFile file;
	i3::vector<i3PersistantElement*> list;

	if( m_pWorld->GetChildCount() > 0)
	{
		project->SetWorld( m_pWorld->GetChild( 0));
	}

	if( m_pSky->GetChildCount() > 0)
	{
		project->SetSky( m_pSky->GetChild( 0));
	}

	if( m_pModel->GetChildCount() > 0)
	{
		project->SetModel( m_pModel->GetChild( 0));
	}

	project->AddTimeSeq( m_pTimeSeq);

	list.push_back( project);

	file.SetObjects( list);
	
	I3_SAFE_RELEASE(project);

	if( file.Save( pszPath) == STREAM_ERR)
	{
		AfxMessageBox( "프로젝트를 저장할 수 없습니다.");
		return FALSE;
	}


	return TRUE;
}

BOOL Ci3EffectEditorDlg::SaveXMLProject(const char * pszPath)
{
	i3EffectProject* project = i3EffectProject::new_object();
	i3ResourceFile file;
	i3::vector<i3PersistantElement*> list;

	if (m_pWorld->GetChildCount() > 0)
	{
		project->SetWorld(m_pWorld->GetChild(0));
	}

	if (m_pSky->GetChildCount() > 0)
	{
		project->SetSky(m_pSky->GetChild(0));
	}

	if (m_pModel->GetChildCount() > 0)
	{
		project->SetModel(m_pModel->GetChild(0));
	}

	project->AddTimeSeq(m_pTimeSeq);

	list.push_back(project);

	file.SetObjects(list);

	I3_SAFE_RELEASE(project);

	i3XMLFile fileXML;
	fileXML.SetObjects(list);
	fileXML.Save(pszPath);

	return TRUE;
}

void Ci3EffectEditorDlg::SetWorld( i3Node * pWorld)
{
	m_pWorld->RemoveAllChild();

	if( pWorld != NULL)
	{
		pWorld->Instantiate();
		i3Scene::ModifyDynamicState( pWorld, I3_PHYSIX_STATE_KINEMATIC, 0);

		m_pWorld->AddChild( pWorld);
	}
}

void Ci3EffectEditorDlg::SetSky( i3Node * pSky)
{
	m_pSky->RemoveAllChild();

	if( pSky != NULL)
	{
		pSky->Instantiate();
		i3Scene::ModifyDynamicState( pSky, I3_PHYSIX_STATE_KINEMATIC, 0);

		m_pSky->AddChild( pSky);
	}
}

void Ci3EffectEditorDlg::SetModel( i3Node * pNode)
{
	m_pModel->RemoveAllChild();

	if( pNode != NULL)
	{
		pNode->Instantiate();
		i3Scene::ModifyDynamicState( pNode, I3_PHYSIX_STATE_KINEMATIC, 0);

		m_pModel->AddChild( pNode);
	}
}

void Ci3EffectEditorDlg::OnFileNewproject()
{
	m_pWorld->RemoveAllChild();
	m_pModel->RemoveAllChild();
	m_pSky->RemoveAllChild();

	m_pTimeSeq->RemoveAllSequence();

	i3Matrix::Identity( m_pEffectTrans->GetMatrix());

	//UpdateAllTimeTrack();

	SelectObject( NULL);

	m_pSelectedBone = NULL;
	m_bAttach = FALSE;
}

void Ci3EffectEditorDlg::OnFileOpensceneasworld()
{
	CFileDialog	Dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"I3Engine Scene File(*.I3S)|*.i3s||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	{
		i3Node * pNode = LoadI3Scene( (LPCTSTR) Dlg.GetPathName());
		if( pNode == NULL)
			return;

		SetWorld( pNode);

		pNode->Release();

		Invalidate();
	}
}

void Ci3EffectEditorDlg::OnFileOpensceneassky()
{
	CFileDialog	Dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"I3Engine Scene File(*.I3S)|*.i3s||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	{
		i3Node * pNode = LoadI3Scene( (LPCTSTR) Dlg.GetPathName());
		if( pNode == NULL)
			return;

		SetSky( pNode);

		pNode->Release();

		Invalidate();
	}
}

void Ci3EffectEditorDlg::OnFileOpensceneasmodel()
{
	CFileDialog	Dlg( TRUE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		"I3Engine Scene File(*.I3S)|*.i3s||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	{
		i3Node * pNode = LoadI3Scene( (LPCTSTR) Dlg.GetPathName());
		if( pNode == NULL)
			return;

		SetModel( pNode);

		pNode->Release();

		Invalidate();
	}
}


BOOL Ci3EffectEditorDlg::OnEraseBkgnd(CDC* pDC)
{
	return 0;
}

void Ci3EffectEditorDlg::SelectObject( i3ElementBase * pObj)
{
	if( pObj == NULL)
	{
		SelectParticle( NULL);
	}
	else
	{
		if( i3::same_of<i3Particle*>(pObj)) //->IsExactTypeOf( i3Particle::static_meta()))
		{
			i3Particle * pParticle = (i3Particle *) pObj;

			SelectParticle( pParticle);
		}
		else if( i3::same_of<i3TimeSequenceInfo*>(pObj)) //->IsExactTypeOf( i3TimeSequenceInfo::static_meta()))
		{
			i3TimeSequenceInfo * pInfo = (i3TimeSequenceInfo *) pObj;

			if( i3::kind_of<i3Particle*>(pInfo->GetNode())) //->IsTypeOf( i3Particle::static_meta()))
			{
				i3Particle * pParticle = (i3Particle *) pInfo->GetNode();

				SelectParticle( pParticle);
			}
		}
	}
}

void Ci3EffectEditorDlg::SelectParticle( i3Particle * pNode)
{
	i3ParticleInfo * pInfo;

	if( pNode == NULL)
		pInfo = NULL;
	else
		pInfo = pNode->GetParticleInfo();

	m_ParticleSpecParticle.SetParticle( pNode);
	m_ParticleSpecObject.SetParticle( pInfo);
	m_ParticleSpecEmitter.SetParticle( pInfo);
	m_ParticleSpecPosition.SetParticle( pInfo);
	m_ParticleSpecRotate.SetParticle( pInfo);
	m_ParticleSpecScale.SetParticle( pInfo);
	m_ParticleSpecColor.SetParticle( pInfo);
}

void Ci3EffectEditorDlg::OnUpdate( I3_TDK_UPDATE_INFO * pInfo)
{
	if( pInfo->m_Event & I3_TDK_UPDATE_ADD)
	{
		SelectObject( pInfo->m_pObject);
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		SelectObject( pInfo->m_pObject);
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
	{
		SelectObject( NULL);
	}
}

LRESULT Ci3EffectEditorDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message)
	{
		case WM_TDK_UPDATE :
			OnUpdate( (I3_TDK_UPDATE_INFO *) wParam);
			break;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void Ci3EffectEditorDlg::SetFileName( const char * pszPath)
{
	char conv[MAX_PATH + 128], szName[MAX_PATH];

	strcpy( m_szFileName, pszPath);

//	i3String::SplitFileName( pszPath, szName, FALSE);
	i3::extract_filetitle(pszPath, szName);

	sprintf( conv, "i3EffectEditor - %s", szName);
	SetWindowText( conv);
}

void Ci3EffectEditorDlg::OnFileSaveas()
{
	CFileDialog	Dlg( FALSE, "I3EFFPROJ", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,
		"I3EffectEditor Project File(*.I3EFFPROJ)|*.I3EFFPROJ||", this);

	if( Dlg.DoModal() == IDCANCEL)
		return;


	if (SaveBinaryProject(LPCTSTR(Dlg.GetPathName())))
	{
		SetFileName(PCTSTR(Dlg.GetPathName()));
	}
}


void Ci3EffectEditorDlg::OnFileSave32774()
{
	if( m_szFileName[0] == 0)
	{
		OnFileSaveas();
		return;
	}

	char szTitle[MAX_PATH], szDir[MAX_PATH], szExt[MAX_PATH];
	i3::extract_filetitle(m_szFileName, szTitle);
	i3::extract_directoryname(m_szFileName, szDir);
	i3::extract_fileext(m_szFileName, szExt);


	if (i3::generic_compare(szExt, "I3EFFXML") == 0)
	{
		i3::string szPath;
		szPath += szDir;
		szPath += "\\";
		szPath += szTitle;
		szPath += ".I3EFFPROJ";

		SaveBinaryProject(szPath.c_str());
	}
	else
	{
		SaveBinaryProject((const char *)m_szFileName);
	}

	if(i3::generic_compare(szExt, "I3EFFPROJ") == 0)
	{
		i3::string szPathXML;

		szPathXML += szDir;
		szPathXML += "\\";
		szPathXML += szTitle;
		szPathXML += ".I3EFFXML";

		SaveXMLProject(szPathXML.c_str());
	}
	else
	{
		SaveXMLProject((const char *)m_szFileName);
	}
}


void Ci3EffectEditorDlg::OnFileOpenscene()
{
	CFileDialog	Dlg(TRUE, "I3EFFPROJ", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		"I3EffectEditor Project File(*.I3EFFPROJ;*.I3EFFXML)|*.I3EFFPROJ;*.I3EFFXML||", this);

	if( Dlg.DoModal() != IDOK)
	{
		return;
	}

	char szExt[MAX_PATH];
	i3::extract_fileext(LPCTSTR(Dlg.GetPathName()), szExt);

	if( i3::generic_compare(szExt, "I3EFFPROJ") == 0)
		LoadBinaryProject( LPCTSTR( Dlg.GetPathName()));
	else
		LoadXMLProject(LPCTSTR(Dlg.GetPathName()));
	SetFileName( LPCTSTR( Dlg.GetPathName()));

	Invalidate();
}

void Ci3EffectEditorDlg::SetTimeSequence( i3TimeSequence * pTimeSeq)
{
	if( m_pTimeSeq != NULL)
	{
		m_pEffectTrans->RemoveChild( m_pTimeSeq);
		I3_SAFE_RELEASE( m_pTimeSeq);
	}

	m_pTimeSeq = pTimeSeq;

	if( m_pTimeSeq != NULL)
	{
		m_pTimeSeq->AddRef();

		m_pEffectTrans->AddChild( m_pTimeSeq);

		m_TimeTrackCtrl.SetTimeSequence( m_pTimeSeq);
	}
}

void Ci3EffectEditorDlg::OnDropFiles(HDROP hDropInfo)
{
	int count;
	char FileName[256];

	count = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

	DragQueryFile(hDropInfo, 0, FileName, sizeof(FileName) - 1);

	char szExt[MAX_PATH];
	i3::extract_fileext(FileName, szExt);

	if (i3::generic_compare(szExt, "I3EFFPROJ") == 0)
		LoadBinaryProject(FileName);
	else
		LoadXMLProject(FileName);
	SetFileName(FileName);

	Invalidate();

}

void Ci3EffectEditorDlg::Reset(void)
{
	INT32 i;
	i3Node * pNode;
	i3TimeSequenceInfo * pInfo;

	if( m_pSelectedBone != NULL)
	{
		if( m_bAttach == FALSE)
		{
			i3Matrix::Copy( m_pEffectTrans->GetMatrix(), m_pSelectedBone->GetCacheMatrix());
		}
	}

	for( i = 0; i < m_pTimeSeq->GetSequenceCount(); i++)
	{
		pInfo = m_pTimeSeq->GetSequence( i);

		pNode = pInfo->GetNode();

		if( i3::same_of<i3Particle*>(pNode))
		{
			i3Particle * pParticle = (i3Particle *) pNode;

			if( pParticle->IsStarted())
			{
				pParticle->Stop();
			}

			pParticle->Create();
		}
	}

	m_pTimeSeq->OnChangeTime( 0.0f);
	m_pViewer->SetTimeScale(1.0f);

	m_bPaused = FALSE;
}

void Ci3EffectEditorDlg::Play(void)
{
	m_bPlay = TRUE;

	if(m_bPaused)
	{
		m_bPaused = FALSE;
		m_pViewer->SetTimeScale(1.0f);
	}
	else
	{
		Reset();
	}

	SetFPS( g_FPS);
}

void Ci3EffectEditorDlg::Stop(void)
{
	KillTimer( 100);

	m_bPlay = FALSE;
}

void Ci3EffectEditorDlg::Pause(void)
{
	m_bPaused = TRUE;
	m_pViewer->SetTimeScale( 0.0f);
}

void Ci3EffectEditorDlg::StopRender(void)
{
	i3RenderContext * pCtx = m_pViewer->GetRenderContext();

	pCtx->SetSkipRender( TRUE);
	m_pViewer->SetPendingUpdateEnable( TRUE);
}

void Ci3EffectEditorDlg::GoRender(void)
{
	i3RenderContext * pCtx = m_pViewer->GetRenderContext();

	pCtx->SetSkipRender( FALSE);
	m_pViewer->SetPendingUpdateEnable( FALSE);
}


BOOL Ci3EffectEditorDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}

void Ci3EffectEditorDlg::SetFPS( UINT32 fps)
{
	UINT32 interval;

	interval = 1000 / fps;

	SetTimer( 100, interval, NULL);
}

void Ci3EffectEditorDlg::Edit( i3ElementBase * pObj, INT32 hint)
{
	if( i3::same_of<i3KeyframeTable*>(pObj)) //->IsExactTypeOf( i3KeyframeTable::static_meta()))
	{
		switch( hint)
		{
			case 0 :		
			case 1 :
				m_PropertyKeyframe.Edit( (i3KeyframeTable *) pObj, hint);	
				m_PropertyHolder.SetActivePanel( &m_PropertyKeyframe);
				break;
				
		}
	}
}

void Ci3EffectEditorDlg::OnControlPlay()
{
	Play();
}


BOOL Ci3EffectEditorDlg::PreTranslateMessage(MSG* pMsg)
{
	if( TranslateAccelerator( m_hWnd, m_hAccel, pMsg))
	{
		TranslateMessage( pMsg);
	}

	if(pMsg->message == WM_KEYDOWN)
	{
		if( (pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN))
		{
			DispatchMessage( pMsg);
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}


void Ci3EffectEditorDlg::OnOK()
{
}

void Ci3EffectEditorDlg::OnCancel()
{

	CDialog::OnCancel();
}


void Ci3EffectEditorDlg::OnFileExporttoi3s()
{
	CExportOptionDialog option;
	i3::vector<i3RenderAttr*>	TexList;
	i3TextureBindAttr * pTexBind;
	i3Texture * pTex;

	CFileDialog	Dlg( FALSE, "i3s", NULL, OFN_ENABLESIZING | OFN_EXPLORER | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT,
		"I3Engine Scene File(*.I3S)|*.i3s||", this);

	if( option.Execute() == FALSE)
		return;

	if( Dlg.DoModal() != IDOK)
		return;

	//UpdateEditor( UPDATE_UNSELECT, NULL);

	// Texture 중에서는 8 bits임에도 불구하고 실제로는 16 Color 이하만 사용하는 Texture가 있다.
	// 그것들을 미리 확인해서 4 bits Texture로 대체해 둔다.
	i3Scene::FindAttrByExactType( m_pTimeSeq, i3TextureBindAttr::static_meta(), TexList);

	{
		i3OptI3Image opt;

		opt.Trace( m_pTimeSeq);
	}

	{
		i3::vector<i3Node*> List;
		
		i3Scene::FindNodeByExactType( m_pTimeSeq, i3Particle::static_meta(), List);

		for(size_t i = 0; i < List.size(); i++)
		{
			i3Particle * pParticle = (i3Particle *) List[i];

			if( pParticle->GetTextureSequence() != NULL)
			{
				i3TextureSequence * pTexSeq = pParticle->GetTextureSequence();

				pTexSeq->SetTexture( pParticle->getTexture());
			}
		}
	}

	i3TimeSequence * pNewTimeSeq;

	pNewTimeSeq = (i3TimeSequence *) i3Scene::CreateClone( m_pTimeSeq, I3_COPY_INSTANCE);
	pNewTimeSeq->AddRef();

	// 각 TimeSequenceInfo 들 중, Hide(Disable) 된 것들은 제거한다.
	for(INT32 i = 0; i < pNewTimeSeq->GetSequenceCount(); i++)
	{
		i3TimeSequenceInfo * pInfo = pNewTimeSeq->GetSequence( i);

		if( pInfo->GetStyle() & I3SG_TIMESEQ_STYLE_DISABLE)
		{
			pNewTimeSeq->RemoveSequence( pInfo);
			i--;
		}
	}
	
	{
		i3SceneFile file;

		file.SetSceneGraph( pNewTimeSeq);

		//초기 res들은 다!!! extern res라고 되어 있다.
		{
			i3Scene::FindAttrByExactType( pNewTimeSeq, i3TextureBindAttr::static_meta(), TexList);

			for(size_t i = 0; i < TexList.size(); i++)
			{
				pTexBind = (i3TextureBindAttr *) TexList[i];

				pTex = pTexBind->GetTexture();

				if( pTex != NULL)
					pTex->SetExternRes( option.m_bExportTex);
			}

			{
				i3::vector<i3Node*> List;
				
				i3Scene::FindNodeByExactType( m_pTimeSeq, i3Particle::static_meta(), List);

				for(size_t i = 0; i < List.size(); i++)
				{
					i3Particle * pParticle = (i3Particle *) List[i];

					pTex = pParticle->getTexture();

					if( pTex != NULL)
						pTex->SetExternRes( option.m_bExportTex);
				}
			}
		}

		if( file.Save( LPCTSTR( Dlg.GetPathName())) == STREAM_ERR)
		{
			AfxMessageBox( "Could not export I3S file.");
			pNewTimeSeq->Release();
			return;
		}

		{
			i3Scene::FindAttrByExactType( pNewTimeSeq, i3TextureBindAttr::static_meta(), TexList);

			for(size_t i = 0; i < TexList.size(); i++)
			{
				pTexBind = (i3TextureBindAttr *) TexList[i];

				pTex = pTexBind->GetTexture();

				if( pTex != NULL)
					pTex->SetExternRes( TRUE);
			}

			{
				i3::vector<i3Node*> List;
				
				i3Scene::FindNodeByExactType( m_pTimeSeq, i3Particle::static_meta(), List);

				for(size_t i = 0; i < List.size(); i++)
				{
					i3Particle * pParticle = (i3Particle *) List[i];

					pTex = pParticle->getTexture();

					if( pTex != NULL)
						pTex->SetExternRes( TRUE);
				}
			}
		}
	}

	pNewTimeSeq->Release();
}

void Ci3EffectEditorDlg::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	CDialog::OnActivateApp(bActive, dwThreadID);

	if( bActive)
		SetFPS( g_FPS);
	else
		KillTimer( 100);
}

void Ci3EffectEditorDlg::ReorderSequence(void)
{
	/*
	INT32 i;
	i3TimeTrack * pTrack;
	i3TimeSequenceInfo * pInfo;
	
	for( i = 0; i < m_pTimeSeq->GetSequenceCount(); i++)
	{
		pInfo = m_pTimeSeq->GetSequence( i);

		pInfo->AddRef();
	}

	EnterRenderCS();

	m_pTimeSeq->RemoveAllSequence();
	m_pTimeSeq->RemoveAllChild();

	for( i = 0; i < m_TimeTrackCtrl.GetTrackCount(); i++)
	{
		pTrack = m_TimeTrackCtrl.GetTrack( i);

		pInfo = (i3TimeSequenceInfo *) pTrack->GetData();

		m_pTimeSeq->AddSequence( pInfo);

		pInfo->Release();
	}

	UpdateAllTimeTrack();

	g_bNeedToCreate = TRUE;

	LeaveRenderCS();
	*/
}

void Ci3EffectEditorDlg::OnFileRemoveworld()
{
	EnterRenderCS();

	m_pWorld->RemoveAllChild();

	LeaveRenderCS();
}


void Ci3EffectEditorDlg::OnFileRemovesky()
{
	EnterRenderCS();

	m_pSky->RemoveAllChild();

	LeaveRenderCS();
}


void Ci3EffectEditorDlg::OnFileRemovemodel()
{
	EnterRenderCS();

	if( m_pSelectedBone != NULL)
	{
		i3Node * pParent = m_pEffectTrans->GetParent();

		pParent->RemoveChild( m_pEffectTrans);

		m_pRoot->AddChild( m_pEffectTrans);

		m_pSelectedBone = NULL;
		m_bAttach = FALSE;
	}

	m_pModel->RemoveAllChild();

	LeaveRenderCS();
}


void Ci3EffectEditorDlg::OnModelSelectbone()
{
	CSelectBoneDialog Dlg;

	if( Dlg.Execute( m_pModel) == FALSE)
		return;

	if( Dlg.GetSelectedBone() == NULL)
		return;

	m_pSelectedBone = Dlg.GetSelectedBone();
	m_bAttach = Dlg.IsAttach();

	{
		i3Node * pParent = m_pEffectTrans->GetParent();

		pParent->RemoveChild( m_pEffectTrans);
	}

	if((m_pSelectedBone == NULL) || (m_bAttach == FALSE))
	{
		m_pRoot->AddChild( m_pEffectTrans);
	}
	else
	{
		m_pSelectedBone->AddChild( m_pEffectTrans);

		i3Matrix::Identity( m_pEffectTrans->GetMatrix());
	}
}

void Ci3EffectEditorDlg::OnUpdateModelSelectbone(CCmdUI *pCmdUI)
{
	if( m_pModel->GetChildCount() > 0)
	{
		pCmdUI->Enable( TRUE);
	}
	else
	{
		pCmdUI->Enable( FALSE);
	}
}

void Ci3EffectEditorDlg::OnToolsOptions()
{
	CDlgOption dlg;

	dlg.DoModal();
}

void Ci3EffectEditorDlg::OnToolsBgcolor()
{
	CColorDialog colorDlg;
	if (colorDlg.DoModal())
	{
		COLORREF color = colorDlg.GetColor();
		
		INT32 bitmask_red=0x0000ff;  //255
		INT32 bitmask_green=0x00ff00; //65280
		INT32 bitmask_blue=0xff0000;  //16711680

		REAL32 red =  REAL32(color & bitmask_red);
		REAL32 green = REAL32((color & bitmask_green)>>8);
		REAL32 blue = REAL32((color & bitmask_blue)>>16);

		//g_pOpt
// 		BYTE x = GetRValue(color);
// 		BYTE y = ((BYTE)(((WORD)(color)) >> 8));
// 		BYTE z = GetBValue(color);

		g_pOption->m_BackColor.r = red / 255.0f;
		g_pOption->m_BackColor.g = green / 255.0f;
		g_pOption->m_BackColor.b = blue / 255.0f;


		i3RenderContext * pCtx = m_pViewer->GetRenderContext();
		
		COLOR col;
		i3Color::Set( &col,  &g_pOption->m_BackColor);
		pCtx->SetClearColor(&col);

	}

	//m_pCOL_ClearColor->SetVisible(TRUE);
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}
