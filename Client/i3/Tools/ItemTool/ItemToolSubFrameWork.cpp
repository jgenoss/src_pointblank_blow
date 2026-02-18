#include "StdAfx.h"
#include "ItemToolSubFrameWork.h"

I3_CLASS_INSTANCE(cItemToolSubFrameWork); //, cItemToolFrameWork)

cItemToolSubFrameWork::cItemToolSubFrameWork(void)

{
}

cItemToolSubFrameWork::~cItemToolSubFrameWork(void)
{
}

void cItemToolSubFrameWork::OnCreate()
{
	i3ViewerFramework::OnCreate();
}

BOOL cItemToolSubFrameWork::Create( i3Viewer * pViewer)
{
	I3ASSERT( pViewer != NULL);

	// Scene Graph
	InitSceneGraph();

	// Viewer
	{
		//pViewer->SetFramework( this);

		m_pViewer = pViewer;

		//((cItemToolViewer*)m_pViewer)->SetSwapChainSgContext( m_pSgContext);

		//CHAR_SOLO_RENDER
		//m_pViewer->SetPlayerSceneGraphContext( m_pPlayerSgContext );

		{
			//COLOR col;

			//i3Color::Set( &col, (UINT8) 0, 0, 0, 255);
			//m_pViewer->GetRenderContext()->SetClearColor( &col);
		}
	}

	// Default 3D Camera
	{
		m_p3DCamera = i3Camera::new_object();
		m_p3DCamera->SetEnable( TRUE);

		//Get3DRoot()->AddChild( m_p3DCamera);
		m_pSgContext->setCurrentCamera( m_p3DCamera);

		//CHAR_SOLO_RENDER
// 		m_pPlayer3DCamera = i3Camera::new_object();
// 		m_pPlayer3DCamera->SetEnable( TRUE);
// 
// 		m_pPlayerSgContext->setCurrentCamera( m_pPlayer3DCamera);
	}

	// Resource Manager
	{
		m_pResManager = i3ResourceManager::new_object();
		I3ASSERT( m_pResManager != NULL);

		m_pResManager->BindFramework( this);

		AddChild( m_pResManager);
	}

	// Effect Manager
	{
		m_pEffectManager = i3EffectManager::new_object();
		I3ASSERT( m_pEffectManager != NULL);

		m_pEffectManager->BindFramework( this);

		// 기본 초기화
		m_pEffectManager->Create();

		AddChild( m_pEffectManager);
	}

	
	// 서브 프레임워크에서는 UI 안씁니다.
	// 나중에 필요할 경우 엔진의 수정이 필요합니다.
	/*m_pUIManager = i3UIManager::new_object();

	m_pUIManager->Create( this);
	m_pUIManager->SetViewer( pViewer);

	AddChild( m_pUIManager);

	setInputDispatchManager( m_pUIManager);

	_CreateUI();*/


	BuildSceneStruct();

	OnCreate();
	
	{
		if (NULL == m_pInterLinkNode)
		{
			m_pInterLinkNode = i3Node::new_object();

			m_pInterLinkNode->SetFlag( I3_NODEFLAG_VOLATILE | I3_NODEFLAG_CONTROL);
			m_pInterLinkNode->SetName( "_InterLinkNode_");
		}
	}

	return TRUE;
}

void cItemToolSubFrameWork::OnUpdate( RT_REAL32 timeDelta)
{
	i3ViewerFramework::OnUpdate(timeDelta);
}

void cItemToolSubFrameWork::OnMButtonDown( UINT32 nFlag, POINT point)
{
	//RECT rtSrc	= m_pViewer->GetRenderContext()->GetSourcePresentRect();

	INT32 nSrcW = g_pViewer->GetMaxW();//rtSrc.right - rtSrc.left;
	INT32 nSrcH = g_pViewer->GetMaxH();//rtSrc.bottom - rtSrc.top;

	INT32 nDestW = ((cItemToolViewer*)m_pViewer)->GetWeaponDlgWidth(m_eSwapChainType);
	INT32 nDestH = ((cItemToolViewer*)m_pViewer)->GetWeaponDlgHeight(m_eSwapChainType);

	INT32 nCurrX = nSrcW * point.x / nDestW;
	INT32 nCurrY = nSrcH * point.y / nDestH;

	POINT ptCurr;
	ptCurr.x = nCurrX;
	ptCurr.y = nCurrY;

	i3Framework::OnMButtonDown( nFlag, point);
	if( getFocus() != NULL)
		return;

	HWND hwnd = m_pViewer->GetWindowHandle();
	i3Camera * pCam = GetCurrentCamera();

	if( m_CamMoveMode == I3_VIEWER_CAM_MOVE_3DSMAX)
	{
		I3ASSERT( hwnd != NULL);

		::SetCapture( hwnd);

		SetAutoPivot();

		i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
		m_DragStartPt = ptCurr;

		if( GetKeyState( VK_LMENU) & 0x80)
		{
			if( nFlag & MK_CONTROL)
				m_CamMode = I3_VIEWER_CAM_ZOOM;
			else
				m_CamMode = I3_VIEWER_CAM_PIVOT_ROTATE;
		}
		else
		{
			m_CamMode = I3_VIEWER_CAM_PAN;
		}
	}
}

void cItemToolSubFrameWork::OnMouseMove(UINT32 nFlag, POINT point)
{
//	RECT rtSrc	= m_pViewer->GetRenderContext()->GetSourcePresentRect();

	INT32 nSrcW = g_pViewer->GetMaxW();//rtSrc.right - rtSrc.left;
	INT32 nSrcH = g_pViewer->GetMaxH();//rtSrc.bottom - rtSrc.top;

	INT32 nDestW = ((cItemToolViewer*)m_pViewer)->GetWeaponDlgWidth(m_eSwapChainType);
	INT32 nDestH = ((cItemToolViewer*)m_pViewer)->GetWeaponDlgHeight(m_eSwapChainType);

	if (nDestW <= 0 || nDestH <= 0)
		return;

	INT32 nCurrX = nSrcW * point.x / nDestW;
	INT32 nCurrY = nSrcH * point.y / nDestH;

	POINT ptCurr;
	ptCurr.x = nCurrX;
	ptCurr.y = nCurrY;

	i3Framework::OnMouseMove( nFlag, ptCurr);
	if( getFocus() != NULL)
		return;

	REAL32 amount = 0.4f;
	REAL32 xDeg, yDeg;
	i3Camera * pCurCam = GetCurrentCamera();

	if( m_CamMoveMode == I3_VIEWER_CAM_MOVE_3DSMAX)
	{
		switch( m_CamMode)
		{
		case I3_VIEWER_CAM_PIVOT_ROTATE :
			if((m_CamModeFlag & I3_VIEWER_CAM_DISABLE_PIVOT_ROTATE) == 0)
			{
				amount = 0.14f;

				xDeg = (REAL32)(m_DragStartPt.x - ptCurr.x) * amount;
				yDeg = (REAL32)(m_DragStartPt.y - ptCurr.y) * amount;

				PivotRotate( __RT( I3_DEG2RAD(xDeg)), __RT(I3_DEG2RAD(yDeg)), __RT_0);
			}
			break;
		case I3_VIEWER_CAM_PAN :
			if((m_CamModeFlag & I3_VIEWER_CAM_DISABLE_PAN) == 0)
			{
				RT_VEC3D vec1;
				RT_VEC3D end1;
				RT_VEC3D start1;
				RT_REAL32 dist;

				i3Vector::Sub( &vec1, i3Matrix::GetPos( pCurCam->GetMatrix()), &m_BasePos);

				dist = i3Vector::Length( &vec1);
				if( dist <= __RT( I3_EPS))
					dist = __RT_ONE;

				ScreenToWorld( ptCurr.x, ptCurr.y, &end1, dist);
				ScreenToWorld( m_DragStartPt.x, m_DragStartPt.y, &start1, dist);

				i3Vector::Sub( &vec1, &end1, &start1);

				Pan( getX(&vec1), getY(&vec1), getZ(&vec1));
			}
			break;
		case I3_VIEWER_CAM_ZOOM :
			if((m_CamModeFlag & I3_VIEWER_CAM_DISABLE_ROTATE) == 0)
			{
				RT_REAL32 amount = __RT(0.5f);
				RT_REAL32 yDeg;

#if defined( I3_FIXED_POINT)
				yDeg = i3FXD::Mul( __RT( (REAL32)(ptCurr.y - m_DragStartPt.y)), amount);
#else
				yDeg = (REAL32)(ptCurr.y - m_DragStartPt.y) * amount;
#endif

				m_DragStartPt = ptCurr;
				Zoom( yDeg);
			}
			break;
		}
	}
	else if( m_CamMoveMode == I3_VIEWER_CAM_MOVE_FPS)
	{
	}

	if( m_bActivePointLight)
	{
		VEC3D pos, vec1;
		MATRIX tmp;
		REAL32 dist;

		i3Vector::Sub( &vec1, i3Matrix::GetPos( GetCurrentCamera()->GetMatrix()), &m_BasePos);

		dist = i3Vector::Length( &vec1);
		if( dist <= __RT( I3_EPS))
			dist = __RT_ONE;

		ScreenToWorld( ptCurr.x, ptCurr.y, &pos, dist);

		i3Matrix::SetTranslate( &tmp, &pos);
		//i3Matrix::Mul( &tmp, GetCurrentCamera()->GetMatrix(), &tmp);

		m_pPointLight->SetWorldSpaceTransform( &tmp);
	}
}
