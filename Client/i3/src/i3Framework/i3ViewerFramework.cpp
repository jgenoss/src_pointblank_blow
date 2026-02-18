#include "i3FrameworkPCH.h"
#include "i3Viewer.h"
#include "i3ViewerFramework.h"
#include "i3DebugFont.inc"
#include "i3EffectManager.h"

#if defined( I3_WINDOWS)
#include "PersistID/i3Framework/i3ViewerPanel.h"
#include "i3FrameworkMeta.h"
#include "res/resource.h"
#include <mmsystem.h>
#endif

#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/get_exec_path.h"

I3_CLASS_INSTANCE( i3ViewerFramework);

i3ViewerFramework::i3ViewerFramework(void)
{
}

i3ViewerFramework::~i3ViewerFramework(void)
{
	I3_SAFE_RELEASE( m_pCurCamera);
	I3_SAFE_RELEASE( m_pGrid);
	I3_SAFE_RELEASE( m_pLightTx);
	I3_SAFE_RELEASE( m_pAttrSet);
	I3_SAFE_RELEASE( m_pPlayerAttrSet );

#if defined( I3_WINDOWS)
	I3_SAFE_RELEASE( m_pDebugTextNode);
	I3_SAFE_RELEASE( m_pDebugTimer);
#endif

	I3_SAFE_RELEASE( m_pLoadedScene);

	I3_SAFE_RELEASE( m_pDebugFont);	

	I3_SAFE_RELEASE( m_pPointLight);
}

void i3ViewerFramework::SetGridShow( bool bShow)
{
	m_bShowGrid = bShow;
	if( bShow)
	{
		m_pGrid->RemoveFlag(I3_NODEFLAG_DISABLE);
	}
	else
	{
		m_pGrid->AddFlag(I3_NODEFLAG_DISABLE);
	}
}

#if defined( I3_WINDOWS)
void i3ViewerFramework::SetPanelVisible( bool bShow)
{
	m_bPanelVisible = bShow;
}
#endif

void i3ViewerFramework::ScreenToWorld( INT32 scrX, INT32 scrY, VEC3D * pVec1, REAL32 zDistance)
{
	MATRIX mat;
	VEC3D vec1, vec2, dir;
	REAL32 dx, dy;
	REAL32	Width = 0.0f, Height = 0.0f;

#if defined(	I3_WINDOWS)
	Width = (REAL32) GetViewer()->GetViewWidth();
	Height = (REAL32) GetViewer()->GetViewHeight();
#endif

	i3Matrix::Inverse( &mat, nullptr, m_pCurCamera->GetProjectionMatrix());

	// Screen 좌표를 0.0~1.0 범위의 좌표로 Normalize한다.
	dx = ((REAL32)scrX / Width * 2.0f) - 1.0f;
	dy = ((REAL32)scrY / Height * 2.0f) - 1.0f;

	if( m_pCurCamera->GetMode() == i3Camera::I3_CAMERA_MODE_PERSPECTIVE)
	{
		i3Vector::Set( &vec1, dx, dy, 0.1f);
		i3Vector::Set( &vec2, dx, dy, 0.5f);

		i3Vector::TransformCoord( &vec1, &vec1, &mat);
		i3Vector::TransformCoord( &vec2, &vec2, &mat);

		i3Vector::Sub( &dir, &vec2, &vec1);

		i3Vector::Normalize( &dir, &dir);

		i3Vector::Scale( pVec1, &dir, zDistance);
	}
	else
	{
		i3Vector::Set( &vec1, dx, dy, 0.0f);

		i3Vector::TransformCoord( pVec1, &vec1, &mat);
	}
}

void i3ViewerFramework::ScreenToWorld( INT32 x1, INT32 y1, INT32 x2, INT32 y2, VEC3D * pDiff, REAL32 zDist)
{
	MATRIX tmx;
	VEC3D v1, v2;
	REAL32 dx, dy, dz;
	REAL32	Width = 0.0f, Height = 0.0f;

#if defined(	I3_WINDOWS)
	Width = (REAL32) GetViewer()->GetViewWidth();
	Height = (REAL32) GetViewer()->GetViewHeight();
#endif

	#if defined( I3_FIXED_POINT)
		dz = i3FXD::Div( zDist - m_pCurCamera->GetNear(), (m_pCurCamera->GetFar() - m_pCurCamera->GetNear()));

		// Screen 좌표를 0.0~1.0 범위의 좌표로 Normalize한다.
		dx = ( i3FXD::Div( i3FXD::FromInt(x1), (Width >> 1))) - __RT(1.0f);
		dy = ( i3FXD::Div( i3FXD::FromInt(y1), (Height >> 1))) - __RT(1.0f);
		i3Vector::Set( &v1, dx, dy, dz);

		dx = __RT( ((REAL32)x2 / Width * 2.0f) - 1.0f);
		dy = __RT( ((REAL32)y2 / Height * 2.0f) - 1.0f);
		i3Vector::Set( &v2, dx, dy, dz);
	#else
		dz = (zDist - m_pCurCamera->GetNear()) / (m_pCurCamera->GetFar() - m_pCurCamera->GetNear());

		// Screen 좌표를 0.0~1.0 범위의 좌표로 Normalize한다.
		dx = ((REAL32)x1 / Width * 2.0f) - 1.0f;
		dy = ((REAL32)y1 / Height * 2.0f) - 1.0f;
		i3Vector::Set( &v1, dx, dy, dz);

		dx = ((REAL32)x2 / Width * 2.0f) - 1.0f;
		dy = ((REAL32)y2 / Height * 2.0f) - 1.0f;
		i3Vector::Set( &v2, dx, dy, dz);
	#endif

	i3Matrix::Inverse( &tmx, nullptr, m_pCurCamera->GetProjectionMatrix());

	i3Vector::TransformCoord( &v1, &v1, &tmx);
	i3Vector::TransformCoord( &v2, &v2, &tmx);

	i3Vector::Sub( pDiff, &v2, &v1);
}

void i3ViewerFramework::SetCurrentCamera( i3Camera * pCam)
{
	if(m_pCurCamera != nullptr)
	{
		I3_MUST_RELEASE( m_pCurCamera);
	}

	m_pCurCamera = pCam;

	OnChangeCamera();

	if( m_pCurCamera != nullptr)
	{
		I3_SAFE_ADDREF( m_pCurCamera);

		OnResetCameraPosition();
		OnUpdateCameraTransform();
		OnUpdateCameraProjection();
	}
}

void i3ViewerFramework::SetCameraProperty( REAL32 dist, REAL32 fnear, REAL32 ffar)
{
	m_pCurCamera->SetNearFar(fnear, ffar);

	OnUpdateCameraProjection();

	OnChangeCamera();
	OnResetCameraPosition();
}

void i3ViewerFramework::SetBound( VEC3D * pMin, VEC3D * pMax)
{
	VEC3D vec;

	i3Vector::Copy( &m_BoundMin, pMin);
	i3Vector::Copy( &m_BoundMax, pMax);

	// Grid Size
	{
		REAL32 GridSz;

		i3Vector::Sub( &vec, pMax, pMin);
		GridSz = i3Math::Mul( MAX( getX( &vec), getZ( &vec)), 2.5f);

		if( GridSz > 0.0f)
		{
			i3Matrix::SetScale( m_pGrid->GetMatrix(), GridSz, 1.0f, GridSz);
		}
		else
		{
			i3Matrix::Identity( m_pGrid->GetMatrix());
		}
	}

	OnChangeBound();
}

static I3SG_TRAVERSAL_RESULT _SetAnimTimeProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	REAL32 * pTM = (REAL32 *) pUserData;

	if( i3::kind_of<i3Transform* >(pNode))
	{
		i3Transform * pTrans = (i3Transform *) pNode;

		pTrans->SetPlayTime( *pTM);
	}
	else if( i3::same_of<i3Skin* >(pNode))
	{
		i3Skin * pSkin = (i3Skin *) pNode;

		pSkin->SetPlayAnim( *pTM);
	}
	else if( i3::same_of<i3TimeSequence* >(pNode))
	{
		i3TimeSequence * pTimeSeq = (i3TimeSequence *) pNode;

		pTimeSeq->SetLocalTime( *pTM);
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void i3ViewerFramework::SetAnimationTime( REAL32 absTime)
{
	m_timePlay = absTime;
	m_pTimer->GetDeltaSec();

	OnChangeTime( absTime);

	if( m_pLoadedScene != nullptr)
	{
		i3Scene::Traverse( m_pLoadedScene, _SetAnimTimeProc, (void *) &absTime);
	}
}

void i3ViewerFramework::SetCameraMoveMode( I3_VIEWER_CAM_MOVE_MODE mode)
{
	i3Matrix::Copy( &m_SavedCamMatrix[ m_CamMoveMode], GetCurrentCamera()->GetMatrix());

	m_CamMoveMode = mode;

	GetCurrentCamera()->SetMatrix( &m_SavedCamMatrix[ mode]);
}

void i3ViewerFramework::OnCreate(void)
{
	COLORREAL col;

	//
	{
		m_pTimer = GetViewer()->GetTimer();
	}

	// Light
	{	
		m_pLightAttr = i3LightAttr::new_object_ref();

		//m_pLightAttr->setLightType( I3G_LIGHT_DIRECTIONAL);
		//i3Color::Set( &col, 0.5f, 0.5f, 0.5f, 1.0f);
		//m_pLightAttr->setAmbient( &col);
		
		i3Color::Set( &col, 2.0f, 2.0f, 2.0f, 1.0f);
		m_pLightAttr->setDiffuse( &col);
		m_pLightAttr->setSpecular( &col);

		m_pLightAttr->setRange( 10.0f);
		m_pLightAttr->setInnerAngle( I3_DEG2RAD( 20.0f));
		m_pLightAttr->setOuterAngle( I3_DEG2RAD( 40.0f));

		{
			i3Light * pLit = m_pLightAttr->GetLightObject();

			pLit->removeStyle( I3G_LIGHT_STYLE_SHADOWMAP);			// ShadowMap
			pLit->setShadowMapSize( 1024);
		}

		i3LightSet * pLightSet = i3LightSet::new_object_ref();
		pLightSet->AddLightAttr( m_pLightAttr);

		m_pLightTx = i3Transform::new_object();
		m_pLightTx->AddChild( pLightSet);

		getSgContext()->addLight( m_pLightAttr->GetLightObject());

		m_LightRho = -65.0f;
		m_LightTheta = 30.0f;
		m_LightDist = 30.0f;

		{
			MATRIX * pMat = m_pLightTx->GetMatrix();

			i3Matrix::SetTranslate( pMat, 0.0f, 0.0f, m_LightDist);
			i3Matrix::PostRotateXDeg( pMat, m_LightRho);
			i3Matrix::PostRotateYDeg( pMat, m_LightTheta);
		}

		Get3DRoot()->AddChild( m_pLightTx);
	}

	{
		// Font
		{
			i3MemStream strm;

			strm.Open( (const char *) s_FontTahoma_8pt, STREAM_READ | STREAM_SHAREREAD, 0x7FFFFFFF);

			m_pDebugFont = i3Font::LoadFromFile( &strm);
			I3ASSERT( m_pDebugFont);
		}

		// Text Node
		{
			m_pDebugTextNode = i3TextNode::new_object();
			m_pDebugTextNode->Create( m_pDebugFont, 1024, true);
			m_pDebugTextNode->DeactivateNode();
			m_pDebugTextNode->SetText( L"");
			m_pDebugTextNode->SetPos( 5, 5);
			m_pDebugTextNode->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);		
			m_pDebugTextNode->SetColor(110, 110, 110, 255);		

			Get2DRoot()->AddChild( m_pDebugTextNode);
		}

		{
			m_pDebugTimer = i3Timer::new_object();
		}
	}

	#if defined( I3G_DX)
	{
		m_pAttrSet = i3AttrSet::new_object();

		Get3DRoot()->AddChild( m_pAttrSet);

		{
			m_pLightingEnableAttr = i3LightingEnableAttr::new_object_ref();
			m_pLightingEnableAttr->Set(true);

			m_pAttrSet->AddAttr( m_pLightingEnableAttr);

			
		}

		m_pLoadedRoot = i3Node::new_object_ref();
		m_pAttrSet->AddChild( m_pLoadedRoot);
	}

	{
		m_pPlayerAttrSet = i3AttrSet::new_object();

		Get3DRoot()->AddChild(m_pPlayerAttrSet);
	
		{
			m_pPlayerLightingEnableAttr = i3LightingEnableAttr::new_object_ref();
			m_pPlayerLightingEnableAttr->Set(true);

			m_pPlayerAttrSet->AddAttr( m_pPlayerLightingEnableAttr);
		}

		m_pLoadedPlayerRoot = i3Node::new_object_ref();
		m_pPlayerAttrSet->AddChild( m_pLoadedPlayerRoot);
	}
	#endif

	// Camera
	{
		SetCurrentCamera( GetDefaultCamera());
	}

	{
		COLORREAL col;

		m_pPointLight = i3Light::new_object();

		i3Color::Set( &col, (REAL32) 2.0f, 2.0f, 2.0f, 1.0f);
		m_pPointLight->SetDiffuse( &col);

		i3Color::Set( &col, 1.0f, 1.0f, 1.0f, 1.0f);
		m_pPointLight->SetSpecular( &col);

		m_pPointLight->SetLightType( I3G_LIGHT_POINT);
		m_pPointLight->setRange( 8.0f);
	}

	// Grid Lines
	{
		i3VertexArray * pVA;
		i3GeometryAttr * pGeoAttr;
		i3Geometry * pGeo;
		i3AttrSet * pAttrSet;
		VEC3D origin;

		i3Vector::Set( &origin, 0.0f, 0.0f, 0.0f);

		pGeoAttr = i3GeometryAttr::new_object_ref();

		{
			i3VertexFormat fmt;

			fmt.SetHasPosition( true);
			fmt.SetHasColor( true);

			pVA = i3VertexArray::new_object_ref();

			pVA->Create( &fmt, 101 * 101 * 2, 0);

			pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 0);

			_SetGridSize( pVA, pGeoAttr, 100.0f, 100.0f);
		}

		pGeo = i3Geometry::new_object_ref();
		pGeo->AppendGeometryAttr( pGeoAttr);

		pAttrSet = i3AttrSet::new_object_ref();
		pAttrSet->AddChild( pGeo);

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

			pAttr->Set(false);
			pAttrSet->AddAttr( pAttr);
		}


		m_pGrid = i3Transform::new_object();
		m_pGrid->AddChild( pAttrSet);

		//m_pRoot->AddChild( m_pGrid);
		Get3DRoot()->AddChild(m_pGrid);
	}

	m_bPlaying = true;

	GetViewer()->Play();

	{
		VEC3D vmin, vmax;
		
		i3Vector::Set( &vmin, -0.5f, -0.5f, -0.5f);
		i3Vector::Scale( &vmax, &vmin, -1.0f);

		SetBound( &vmin, &vmax);
	}

	// Effect Manager
	{
		Get3DRoot()->AddChild( m_pEffectManager->GetNode());
	}

	//Post Process
	{
		//PostProcess.xml 파일을 로딩해야 한다.
		char szExecPath[MAX_PATH];
		i3::get_exec_path(szExecPath);
		i3::generic_string_cat(szExecPath, "\\PostProcess.xml");

		if(i3System::IsFile(szExecPath))
		{
			i3XMLFile xmlFile;
			if(xmlFile.Load(szExecPath))
			{
				m_pViewer->GetPostProcessManager()->LoadPostProcessFromXML(&xmlFile);
			}
		}
		else
		{
			//xml 파일이 없다.
			//기본으로 Post Process를 생성한다.
			//만들어둔 Post Process들은 기본적으로 disabled 상태로.
			i3PostProcessBloom * pBloom = i3PostProcessBloom::new_object_ref();
			m_pViewer->GetPostProcessManager()->AddPostProcess(pBloom);
			pBloom->SetEnable(false);
			
		}
	}

#if defined( I3_PHYSX)
	GetViewer()->GetPhysixContext()->setEnableSimulate(true);
#endif

	i3Framework::OnCreate();

	m_State = STATE_NONE;
}

void i3ViewerFramework::OnChangeScene(void)
{
	if( m_pLoadedScene != nullptr)
	{
		i3BoundBox * pBox = i3BoundBox::new_object();

		m_pLoadedScene->GetWrappingBound( pBox);

		{
			VEC3D * pMin, * pMax, temp;

			pMin = pBox->GetMin();
			pMax = pBox->GetMax();

			i3Vector::Sub( &temp, pMax, pMin);

			if( i3Vector::LengthSq( &temp) <= 0.0f)
			{
				i3Vector::Set( pMin, -1.0f, -1.0f, -1.0f);
				i3Vector::Set( pMax, 1.0f, 1.0f, 1.0f);
			}
		}

		SetBound( pBox->GetMin(), pBox->GetMax());

		I3_MUST_RELEASE( pBox);
	}

	UpdateAnimationDuration();

	#if defined( I3_WINDOWS)
		//UpdatePanelDuration();
	#endif

	if( (m_bPlaying == true) && (!GetViewer()->IsState( I3_VIEWER_STATE_PAUSED)))
	{
		Play();
	}
}

bool i3ViewerFramework::OnStartPlay(void)
{
	return true;
}

bool i3ViewerFramework::OnStartPause(void)
{
	return true;
}

void i3ViewerFramework::Play(void)
{
	if( OnStartPlay())
	{
		m_bPlaying = true;

		if( m_pLoadedScene != nullptr)
		{
			GetViewer()->Play();
		}
	}
}

void i3ViewerFramework::Pause(void)
{
	if( OnStartPause())
	{
		m_bPlaying = false;

		if( m_pLoadedScene != nullptr)
		{
			GetViewer()->Pause();
		}
	}
}

void i3ViewerFramework::UpdateLightTx(void)
{
	{
		MATRIX * pMat = m_pLightTx->GetMatrix();

		i3Matrix::SetTranslate( pMat, 0.0f, 0.0f, m_LightDist);
		i3Matrix::PostRotateXDeg( pMat, m_LightRho);
		i3Matrix::PostRotateYDeg( pMat, m_LightTheta);

		i3Matrix::PostTranslate( pMat, &m_BasePos);

		//I3TRACE( "%f %f\n", m_LightRho, m_LightTheta);
	}
}

////////////////////////////////////////////////////////////////
//
static I3SG_TRAVERSAL_RESULT _UpdateAnimDurationProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	REAL32 * pDur = (REAL32 *) pUserData;
	REAL32 dur;

	if( i3::kind_of<i3Transform* >(pNode))
	{
		i3Transform * pTrans = (i3Transform *) pNode;
	
		dur = pTrans->GetDuration();
		*pDur = MAX( *pDur, dur);
	}
	else if( i3::kind_of<i3Skin* >(pNode))
	{
		i3Skin * pSkin = (i3Skin *) pNode;

		dur = pSkin->GetDuration();
		*pDur = MAX( *pDur, dur);
	}
	else if( i3::same_of<i3TimeSequence* >(pNode))
	{
		//i3TimeSequence * pTimeSeq = (i3TimeSequence *) pNode;

		//pTimeSeq->SetLocalTime( *pTM);
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void i3ViewerFramework::UpdateAnimationDuration(void)
{
	m_AnimDuration = 0.0f;

	if( m_pLoadedScene != nullptr)
	{
		i3Scene::Traverse( m_pLoadedScene, _UpdateAnimDurationProc, (void *) &m_AnimDuration);
	}
}
//
/////////////////////////////////////////////////////////////////

void i3ViewerFramework::MoveCamera( VEC3D Pos)
{
	MATRIX * pMatrix = 	m_pCurCamera->GetMatrix();

	i3Vector::Add( i3Matrix::GetPos( pMatrix), i3Matrix::GetPos( pMatrix), &Pos);

	OnUpdateCameraTransform();
}

void i3ViewerFramework::RotCamera( VEC3D Rot) 
{	
	MATRIX * pMatrix = 	m_pCurCamera->GetMatrix();

	i3Vector::Add( i3Matrix::GetPos(pMatrix), i3Matrix::GetPos(pMatrix), &Rot); 

	OnUpdateCameraTransform();
}

void i3ViewerFramework::Zoom( REAL32 rate)
{
	VEC3D vec;
	MATRIX * pCamera = m_pCurCamera->GetMatrix();
	MATRIX tmp;

	rate = i3Math::Mul( rate, 2.0f);

	i3Vector::Copy( &vec, i3Matrix::GetAt( pCamera));
	i3Vector::Normalize( &vec, &vec);
	i3Vector::Scale( &vec, &vec, rate);

	i3Matrix::SetTranslate( &tmp, getX(&vec), getY(&vec), getZ(&vec));

	i3Matrix::Mul( pCamera, pCamera, &tmp);

	OnUpdateCameraTransform();
	OnUpdateCameraProjection();
}

void i3ViewerFramework::PivotRotate( REAL32 RotX, REAL32 RotY, REAL32 /*RotZ*/)
{
    // Camera matrix
    MATRIX * pCamera;
	VEC3D vecRight, localPos;
	i3Camera * pCurCam = GetCurrentCamera();

	#if defined( I3_FIXED_POINT)
		RotX = i3FXD::Mul( RotX, __RT(5.0f));
		RotY = i3FXD::Mul( RotY, __RT(5.0f));
	#else
		RotX *= 5.0f;
		RotY *= 5.0f;
	#endif

	pCamera = pCurCam->GetMatrix();

	i3Matrix::Copy( pCamera, &m_DragStartMatrix);
	i3Vector::Sub( &localPos, i3Matrix::GetPos( pCamera), &m_BasePos);

	// make it in local space
	i3Matrix::SetPos( pCamera, &localPos);

	// rotate around local right vector
	i3Vector::Copy( &vecRight, i3Matrix::GetRight( pCamera));
	i3Vector::Normalize( &vecRight, &vecRight);

	i3Matrix::PostRotateAxis( pCamera, (const VEC3D *) &vecRight, RotY);

	// rotate around world y axis
	i3Matrix::PostRotateAxis( pCamera, (const VEC3D *) &I3_YAXIS, RotX);

	// return to in world-space
	i3Vector::Add( &localPos, i3Matrix::GetPos( pCamera), &m_BasePos);
	i3Matrix::SetPos( pCamera, &localPos);

	OnUpdateCameraTransform();
}

void i3ViewerFramework::SetAutoPivot(void)
{
	i3Camera * pCam = GetCurrentCamera();
	VEC3D	sline, eline, center, pos;
	REAL32 t;

	i3Vector::Copy( &sline, i3Matrix::GetPos( pCam->GetMatrix()));

	i3Vector::Scale( &eline, i3Matrix::GetAt( pCam->GetMatrix()), -100000.0f);
	i3Vector::Add( &eline, &sline, &eline);

	i3Vector::Add( &center, &m_BoundMin, &m_BoundMax);
	i3Vector::Scale( &center, &center, 0.5f);

	if( i3Math::GetPerpendicularPoint( &sline, &eline, &center, &pos, &t))
	{
		i3Vector::Copy( &m_BasePos, &pos);
	}
}

void i3ViewerFramework::SetPivot( VEC3D * pVec)
{
	i3Vector::Copy( &m_BasePos, pVec);
}

void i3ViewerFramework::Pan( REAL32 dx, REAL32 dy, REAL32 dz)
{
	VEC3D vecRight, vecUp, vecAt;
	MATRIX * pCam;

	pCam = &m_DragStartMatrix;

	i3Vector::Copy( &vecRight, i3Matrix::GetRight( pCam));
	i3Vector::Copy( &vecUp, i3Matrix::GetUp( pCam));
	i3Vector::Copy( &vecAt, i3Matrix::GetAt( pCam));

	i3Vector::Normalize( &vecRight, &vecRight);
	i3Vector::Normalize( &vecUp, &vecUp);
	i3Vector::Normalize( &vecAt, &vecAt);

	i3Vector::Scale( &vecRight, &vecRight, (-dx));
	i3Vector::Scale( &vecUp, &vecUp, ( dy));
	i3Vector::Scale( &vecAt, &vecAt, ( dz));

	i3Vector::Add( &vecRight, &vecRight, &vecUp);
	//i3Vector::Add( &vecRight, &vecRight, &vecAt);

	{
		MATRIX tmp;

		i3Matrix::SetPos( &tmp, &vecRight);
		i3Matrix::Mul( GetCurrentCamera()->GetMatrix(), &m_DragStartMatrix, &tmp);
	}

	OnUpdateCameraTransform();
}

bool i3ViewerFramework::LoadI3S( const char * pszPath)
{
	i3SceneFile file;
	UINT32 Rc;
	i3Node * pNode;

	Rc = file.Load( pszPath);
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN,  "Could not load %s file.", pszPath);
		return false;
	}

	pNode = file.GetSceneGraph();
	if( pNode == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN,  "Invalid I3S file.", pszPath);
		return false;
	}

	m_FileName = pszPath;

	SetSceneGraph( pNode);

	return true;
}

bool i3ViewerFramework::CloseI3S()
{
	//Post Process를 저장한다.
	bool bResult = false;
	{
		char szExecPath[MAX_PATH];
		i3::get_exec_path(szExecPath);
		i3::generic_string_cat(szExecPath, "\\PostProcess.xml");

		i3XMLFile xmlFile;

		m_pViewer->GetPostProcessManager()->SavePostProcessToXML(&xmlFile);
		bResult = xmlFile.Save(szExecPath);
	}

	return bResult;
}

void i3ViewerFramework::SetSceneGraph( i3Node * pNode, bool bReset)
{
	i3Node * pParent = OnQueryAttachNode(nullptr);

	if( m_pLoadedScene != nullptr)
	{
		pParent->RemoveChild( m_pLoadedScene);
		I3_MUST_RELEASE( m_pLoadedScene);
	}

	m_pLoadedScene = pNode;

	if( pNode != nullptr)
	{
		pParent->AddChild( pNode);
		I3_SAFE_ADDREF( m_pLoadedScene);
	}

	UpdateBound();

	if( bReset)
	{
		i3NamedResourceManager::Reset();

		OnChangeScene();
		OnResetCameraPosition();
		OnUpdateCameraTransform();
	}
}

i3Node * i3ViewerFramework::OnQueryAttachNode( i3GameRes * pRes)
{
	return m_pLoadedRoot;
}

i3Node * i3ViewerFramework::OnPlayerQueryAttachNode( i3GameRes * pRes)
{
	return m_pLoadedPlayerRoot;
}


void i3ViewerFramework::OnChangeBound(void)
{
	VEC3D * pMin, * pMax;
	VEC3D vec;

	pMin = GetBoundMin();
	pMax = GetBoundMax();

	i3Vector::Sub( &vec, pMax, pMin);
	m_CamDist = i3Math::Mul( i3Vector::Length( &vec), 1.3f);

	#if defined( I3_FIXED_POINT)
		m_CamNear = i3FXD::Mul( m_CamDist, __RT( 0.0001f));
		m_CamFar =	i3FXD::Mul( m_CamDist, __RT( 5.0f));
		m_ZoomRate = i3FXD::Mul( m_CamDist, __RT( 0.01f));
	#else
		m_CamNear = m_CamDist * 0.01f;
		m_CamFar = m_CamDist * 5.0f;
		m_ZoomRate = m_CamDist * 0.01f;
	#endif


	if((m_CamFar > 1.0f) &&(m_CamNear < 0.1f))
		m_CamNear = 0.1f;

	if( m_CamNear > 0.5f)
		m_CamNear = 0.5f;

	I3TRACE( "Near:%f    Far:%f\n", m_CamNear, m_CamFar);

	i3Vector::Add( &vec, pMax, pMin);

	i3Vector::Scale( &m_BasePos, &vec, 0.5f);

	SetCameraProperty(m_CamDist, m_CamNear, m_CamFar);

	m_LightDist = m_CamDist;
	UpdateLightTx();

	{
		i3SceneGraphContext * pCtx = getSgContext();

		pCtx->setManualBoundEnable( true);
		pCtx->setBound( pMin, pMax);

//		getPlayerSgContext()->setManualBoundEnable( true);
//		getPlayerSgContext()->setBound( pMin, pMax);
		getSgContext()->setManualBoundEnable( true);
		getSgContext()->setBound( pMin, pMax);

	}
}

void i3ViewerFramework::OnRender(void)
{
	m_timePlay += GetViewer()->getDeltaTime();

	if( m_timePlay > m_AnimDuration)
		m_timePlay -= m_AnimDuration;
}

#define		MOVE_VEL		0.4f

void i3ViewerFramework::OnUpdate( REAL32 timeDelta)
{
	if( m_CamMoveMode == I3_VIEWER_CAM_MOVE_FPS)
	{
		i3Camera * pCam = GetCurrentCamera();
		VEC3D * pRight = i3Matrix::GetRight( pCam->GetMatrix());
		VEC3D * pAt = i3Matrix::GetAt( pCam->GetMatrix());
		VEC3D pos, vec;

		i3Vector::Set( &pos, 0.0f, 0.0f, 0.0f);

		if( m_CamMoveKey & I3_VIEWER_KEY_LEFT)
		{
			i3Vector::Scale( &vec, pRight, -MOVE_VEL);
			i3Vector::SetY( &vec, 0.0f);
            i3Vector::Add( &pos, &pos, &vec);
		}
		else if( m_CamMoveKey & I3_VIEWER_KEY_RIGHT)
		{
			i3Vector::Scale( &vec, pRight, MOVE_VEL);
			i3Vector::SetY( &vec, 0.0f);
			i3Vector::Add( &pos, &pos, &vec);
		}

		if( m_CamMoveKey & I3_VIEWER_KEY_FRONT)
		{
			i3Vector::Scale( &vec, pAt, -MOVE_VEL);
			i3Vector::SetY( &vec, 0.0f);
            i3Vector::Add( &pos, &pos, &vec);
		}
		else if( m_CamMoveKey & I3_VIEWER_KEY_BACK)
		{
			i3Vector::Scale( &vec, pAt, MOVE_VEL);
			i3Vector::SetY( &vec, 0.0f);
            i3Vector::Add( &pos, &pos, &vec);
		}

		if( m_CamMoveKey & I3_VIEWER_KEY_UP)
		{
            i3Vector::ScaleAdd( &pos, &I3_YAXIS, MOVE_VEL);
		}
		else if( m_CamMoveKey & I3_VIEWER_KEY_DOWN)
		{
			i3Vector::ScaleAdd( &pos, &I3_YAXIS, -MOVE_VEL);
		}

		if( m_CamMode == I3_VIEWER_CAM_PIVOT_ROTATE)
		{
			REAL32 amount = 0.3f;
			MATRIX mtx;
			VEC3D rot;
			REAL32 xDeg = 0.0f, yDeg = 0.0f;
			i3Camera  * pCurCam = GetCurrentCamera();

			#if defined( I3_WINDOWS)
			POINT pt;

			GetCursorPos( &pt);

			ScreenToClient( GetViewer()->GetWindowHandle(), &pt);

			xDeg = (REAL32)(pt.y - m_DragStartPt.y) * amount;
			yDeg = (REAL32)(pt.x - m_DragStartPt.x) * amount;

			#endif

			xDeg = -I3_DEG2RAD(xDeg);
			yDeg = -I3_DEG2RAD( yDeg);

			i3Vector::Set( &rot, xDeg, yDeg, 0.0f);
			i3Vector::Add( &m_FPSCamRot, &m_OldFPSCamRot, &rot);

			i3Matrix::SetRotateX( &mtx, i3Vector::GetX( &m_FPSCamRot));
			i3Matrix::PostRotateY( &mtx, i3Vector::GetY( &m_FPSCamRot));
			i3Matrix::SetPos( &mtx, i3Matrix::GetPos( pCurCam->GetMatrix()));
			
			pCurCam->SetMatrix( &mtx);
			
			OnUpdateCameraTransform();
		}

		MoveCamera( pos);
	}

	i3GameNode::OnUpdate( timeDelta);

	m_timeDebugText += m_pDebugTimer->GetDeltaSec();

	if( m_timeDebugText > 0.2f)
	{
		m_timeDebugText = 0.0f;
		ShowDebugText();
	}
}

void i3ViewerFramework::OnResetCameraPosition(void)
{
	MATRIX * pCamera = GetDefaultCamera()->GetMatrix();

	i3Matrix::Identity( pCamera);	
	i3Matrix::SetTranslate( pCamera, getX(&m_BasePos), getY(&m_BasePos), getZ(&m_BasePos) + m_CamDist);	
}

void i3ViewerFramework::OnUpdateCameraTransform(void)
{
}

void i3ViewerFramework::OnUpdateCameraProjection(void)
{
	i3Camera * pCam;
	REAL32 scrW, scrH;

	scrW = (REAL32) m_pViewer->GetRenderContext()->GetCurrentRTWidth();
	scrH = (REAL32) m_pViewer->GetRenderContext()->GetCurrentRTHeight();

	pCam = GetCurrentCamera();

	if( pCam == nullptr)
		return;

	switch( pCam->GetMode())
	{
		case i3Camera::I3_CAMERA_MODE_ORTHOGONAL :
			{
				REAL32 w, h, l;
				MATRIX * pCamera = pCam->GetMatrix();
				VEC3D vec, pc;

				i3Vector::Copy( &vec, i3Matrix::GetAt( pCamera));
				i3Vector::Normalize( &vec, &vec);
				i3Vector::Scale( &vec, &vec, -1.0f);

				#if defined( I3_FIXED_POINT)
					VEC3D campos, basepos;

					i3Vector::Copy( &basepos, &m_BasePos);
					i3Vector::Copy( &campos, i3Matrix::GetPos( pCamera));
					l = NearestPlanePoint( &basepos, &vec, &campos, &pc);
				#else
					l = NearestPlanePoint( &m_BasePos, &vec, i3Matrix::GetPos( pCamera), &pc);
				#endif

				l = i3Math::abs( l);

				w = l;
				h = l * (scrH / scrW);

				pCam->SetOrthogonal(w, h, pCam->GetNear(), pCam->GetFar());
			}
			break;

		default :
			{
				REAL32 w, h;
				
				#if defined( I3_FIXED_POINT)
					w = i3FXD::FromInt( pRT->GetWidth());
					h = i3FXD::FromInt( pRT->GetHeight());
				#else
					w = scrW;
					h = scrH;
				#endif

				pCam->SetPerspective( pCam->GetFOV(), w, h, pCam->GetNear(), pCam->GetFar());
			}
			break;
	}
}

void i3ViewerFramework::OnInput(void)
{
	i3InputController * pCtrl;
	i3InputDeviceManager * pInputManager;
	i3Camera * pCam;

#if defined( I3_WINDOWS)
	i3InputKeyboard * pKB;
#endif

	pInputManager = m_pViewer->GetInputDeviceManager();
	pCam = GetCurrentCamera();

#if defined( I3_WINDOWS)
	if( pInputManager->GetKeyboard() != nullptr)
	{
		pKB = pInputManager->GetKeyboard();

		if( pKB->GetStrokeState( I3I_KEY_EQUAL))
		{
			ToggleDebugControl();
		}

		if( m_bDebugControl)
		{
			if( pKB->GetPressState( I3I_KEY_LEFT))
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate( I3_DEG2RAD(-0.5f), 0.0f, 0.0f);
			}

			if( pKB->GetPressState( I3I_KEY_RIGHT))
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate(I3_DEG2RAD(0.5f), 0.0f, 0.0f);
			}

			if( pKB->GetPressState( I3I_KEY_UP))
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate( 0.0f, I3_DEG2RAD(-0.5f), 0.0f);
			}

			if( pKB->GetPressState( I3I_KEY_DOWN))
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate( 0.0f, I3_DEG2RAD(0.5f), 0.0f);
			}

			if( pKB->GetPressState( I3I_KEY_Z))
			{
				Zoom(0.5f);
			}

			if( pKB->GetPressState( I3I_KEY_X))
			{
				Zoom(-0.5f);
			}
		}

		if( pKB->GetPressState( I3I_KEY_LCTRL))
		{
			bool bUpdate = false;

			if( pKB->GetPressState( I3I_KEY_LEFT))
			{
				m_LightTheta -= 0.3f;
				bUpdate = true;
			}
			else if( pKB->GetPressState( I3I_KEY_RIGHT))
			{
				m_LightTheta += 0.3f;
				bUpdate = true;
			}

			if( pKB->GetPressState( I3I_KEY_UP))
			{
				m_LightRho -= 0.3f;
				bUpdate = true;
			}
			else if( pKB->GetPressState( I3I_KEY_DOWN))
			{
				m_LightRho += 0.3f;
				bUpdate = true;
			}

			if( pKB->GetPressState( I3I_KEY_PGUP))
			{
				m_LightDist += 0.1f;
				bUpdate = true;
			}
			else if( pKB->GetPressState( I3I_KEY_PGDN))
			{
				m_LightDist -= 0.1f;
				bUpdate = true;
			}

			if( bUpdate)
				UpdateLightTx();
		}
	}
#endif

	if( pInputManager->GetControllerCount() > 0)
	{
		pCtrl = pInputManager->GetController( 0);

		{
			REAL32 axis;

			axis = pCtrl->GetAnalogValue( I3I_ANALOG_DIR_X);
			if( i3Math::abs(axis) > I3I_ANALOG_DEAD_MAX )
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate(I3_DEG2RAD( axis), 0.0f, 0.0f);
			}

			axis = pCtrl->GetAnalogValue( I3I_ANALOG_DIR_Y);
			if( i3Math::abs(axis) > I3I_ANALOG_DEAD_MAX )
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate( 0.0f, I3_DEG2RAD( axis), 0.0f);
			}

			if(pCtrl->GetPressState() & I3I_BTN_LLEFT)
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate(I3_DEG2RAD( -0.5f), 0.0f, 0.0f);
			}

			if( pCtrl->GetPressState() & I3I_BTN_LRIGHT)
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate(I3_DEG2RAD( 0.5f), 0.0f, 0.0f);
			}

			if( pCtrl->GetPressState() & I3I_BTN_LUP)
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate(0.0f, I3_DEG2RAD( -0.5f), 0.0f);
			}

			if( pCtrl->GetPressState() & I3I_BTN_LDOWN)
			{
				i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
				PivotRotate( 0.0f, I3_DEG2RAD( 0.5f), 0.0f);
			}

			if( pCtrl->GetPressState() & I3I_BTN_RLEFT)
			{

				Zoom( i3Math::Mul(0.5f, m_ZoomRate));
			}
			if( pCtrl->GetPressState() & I3I_BTN_RDOWN)
			{
				Zoom( i3Math::Mul(-0.5f, m_ZoomRate));
			}
		}
	}

	i3Framework::OnInput();
}

void i3ViewerFramework::OnSize( i3::pack::POINT2D * pSize)
{
	i3Framework::OnSize( pSize);
}

void i3ViewerFramework::ShowDebugText(void)
{
	m_wstrDebugText.clear();

	{
		
		i3GfxPerformanceReport * pReport = GetViewer()->GetRenderContext()->GetPerformanceReport();
		
		m_wstrDebugText += i3::format_string( L"FPS        : %.2f\n", pReport->GetFPS());
		
#if !defined( I3_NO_PROFILE)
		
		m_wstrDebugText += i3::format_string( L"* Total    : %.2f\n", m_pViewer->getProfileData( I3_VIEWER_PROFILE_TOTAL));
		m_wstrDebugText += i3::format_string( L"* Trace      : %.2f\n", m_pViewer->getProfileData( I3_VIEWER_PROFILE_TRACE_3D));
		m_wstrDebugText += i3::format_string( L"* Render(3D) : %.2f\n", m_pViewer->getProfileData( I3_VIEWER_PROFILE_RENDER_3D));
		m_wstrDebugText += i3::format_string( L"* Flush Draw : %.2f\n", m_pViewer->getProfileData( I3_VIEWER_PROFILE_END));
		m_wstrDebugText += i3::format_string( L"* Effect     : %.2f\n", m_pViewer->getProfileData( I3_VIEWER_PROFILE_EFFECT));
		m_wstrDebugText += i3::format_string( L"* sgContext  : %.2f\n", m_pViewer->getProfileData( I3_VIEWER_PROFILE_SGCONTEXT));
#endif
	
		m_wstrDebugText += i3::format_string( L"Tri Count  : %d\n", pReport->GetTriangleCountPerFrame());
		m_wstrDebugText += i3::format_string( L"Draw Calls : %d\n", pReport->GetDrawCallCountPerFrame());

		if( !m_wstrDebugText.empty() ) 
		{
			m_pDebugTextNode->SetText(m_wstrDebugText);
		}
		else
		{
			m_pDebugTextNode->DeactivateNode();
		}
	}
}

void i3ViewerFramework::SetDebugTextEnable( bool bFlag)		
{ 
	if( bFlag)
	{
		m_pDebugTextNode->ActivateNode();
	}
	else
	{
		m_pDebugTextNode->DeactivateNode();
	}
}

bool i3ViewerFramework::GetDebugTextEnable(void)				
{
	bool bFlag = false;

	if(m_pDebugTextNode->GetFlag() & I3_NODEFLAG_DISABLE)
	{
		bFlag = false;
	}
	else
	{
		bFlag = true;
	}

	return bFlag;
}	

#if defined( I3_WINDOWS)
void i3ViewerFramework::OnLButtonDown( UINT32 nFlag, POINT point)
{
	i3Framework::OnLButtonDown( nFlag, point);
	if( getFocus() != nullptr)
		return;

	HWND hwnd = m_pViewer->GetWindowHandle();
	i3Camera * pCam = GetCurrentCamera();

	I3ASSERT( hwnd != nullptr);

	::SetCapture( hwnd);

	i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
	m_DragStartPt = point;

	if( m_CamMoveMode == I3_VIEWER_CAM_MOVE_FPS)
	{
		m_CamMode = I3_VIEWER_CAM_PIVOT_ROTATE;

		i3Vector::Copy( &m_OldFPSCamRot, &m_FPSCamRot);
	}
}

void i3ViewerFramework::OnLButtonUp( UINT32 nFlag, POINT point)
{
	i3Framework::OnLButtonUp( nFlag, point);
	if( getFocus() != nullptr)
		return;

	m_CamMode = I3_VIEWER_CAM_NONE;

	::ReleaseCapture();
}

void i3ViewerFramework::OnLButtonDblClk( UINT32 nFlag, POINT point)
{
	i3Framework::OnLButtonDblClk( nFlag, point);
	if( getFocus() != nullptr)
		return;

	if( GetViewer()->IsState( I3_VIEWER_STATE_PAUSED))
		GetViewer()->Play();
	else
		GetViewer()->Pause();
}

void i3ViewerFramework::OnMButtonDown( UINT32 nFlag, POINT point)
{
	i3Framework::OnMButtonDown( nFlag, point);
	if( getFocus() != nullptr)
		return;

	HWND hwnd = m_pViewer->GetWindowHandle();
	i3Camera * pCam = GetCurrentCamera();

	if( m_CamMoveMode == I3_VIEWER_CAM_MOVE_3DSMAX)
	{
		I3ASSERT( hwnd != nullptr);

		::SetCapture( hwnd);

		SetAutoPivot();

		i3Matrix::Copy( &m_DragStartMatrix, pCam->GetMatrix());
		m_DragStartPt = point;

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

void i3ViewerFramework::OnMButtonUp( UINT32 nFlag, POINT point)
{
	i3Framework::OnMButtonUp( nFlag, point);
	if( getFocus() != nullptr)
		return;

	m_CamMode = I3_VIEWER_CAM_NONE;

	::ReleaseCapture();
}

void i3ViewerFramework::OnRButtonDown( UINT32 nFlag, POINT point)
{
	i3Framework::OnRButtonDown( nFlag, point);
	if( getFocus() != nullptr)
		return;

	i3SceneGraphContext * pCtx = this->getSgContext();

	if( m_bActivePointLight == false)
	{
		pCtx->addLight( m_pPointLight);
		m_bActivePointLight = true;

		VEC3D pos, vec1;
		MATRIX tmp;
		REAL32 dist;

		i3Vector::Sub( &vec1, i3Matrix::GetPos( GetCurrentCamera()->GetMatrix()), &m_BasePos);

		dist = i3Vector::Length( &vec1);
		if( dist <= I3_EPS)
			dist = 1.0f;

		ScreenToWorld( point.x, point.y, &pos, dist);

		i3Matrix::SetTranslate( &tmp, &pos);
		//i3Matrix::Mul( &tmp, GetCurrentCamera()->GetMatrix(), &tmp);

		m_pPointLight->SetWorldSpaceTransform( &tmp);
	}

	HWND hwnd = m_pViewer->GetWindowHandle();
	::SetCapture( hwnd);
}

void i3ViewerFramework::OnRButtonUp( UINT32 nFlag, POINT point)
{
	i3Framework::OnRButtonDown( nFlag, point);
	if( getFocus() != nullptr)
		return;

	i3SceneGraphContext * pCtx = this->getSgContext();

	m_CamMode = I3_VIEWER_CAM_NONE;

	if( m_bActivePointLight)
	{
		pCtx->removeLight( m_pPointLight);
		m_bActivePointLight = false;
	}

	ReleaseCapture();
}

void i3ViewerFramework::OnMouseMove( UINT32 nFlags, POINT point)
{
	i3Framework::OnMouseMove( nFlags, point);
	if( getFocus() != nullptr)
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

					xDeg = (REAL32)(m_DragStartPt.x - point.x) * amount;
					yDeg = (REAL32)(m_DragStartPt.y - point.y) * amount;

					PivotRotate(I3_DEG2RAD(xDeg), I3_DEG2RAD(yDeg), 0.0f);
				}
				break;

			case I3_VIEWER_CAM_PAN :
				if((m_CamModeFlag & I3_VIEWER_CAM_DISABLE_PAN) == 0)
				{
					VEC3D vec1;
					VEC3D end1;
					VEC3D start1;
					REAL32 dist;

					i3Vector::Sub( &vec1, i3Matrix::GetPos( pCurCam->GetMatrix()), &m_BasePos);

					dist = i3Vector::Length( &vec1);
					if( dist <= I3_EPS)
						dist = 1.0f;

					ScreenToWorld( point.x, point.y, &end1, dist);
					ScreenToWorld( m_DragStartPt.x, m_DragStartPt.y, &start1, dist);

					i3Vector::Sub( &vec1, &end1, &start1);

					Pan( getX(&vec1), getY(&vec1), getZ(&vec1));
				}
				break;

			case I3_VIEWER_CAM_ZOOM :
				if((m_CamModeFlag & I3_VIEWER_CAM_DISABLE_ROTATE) == 0)
				{
					REAL32 amount = 0.5f;
					REAL32 yDeg;

					#if defined( I3_FIXED_POINT)
						yDeg = i3FXD::Mul( __RT( (REAL32)(point.y - m_DragStartPt.y)), amount);
					#else
						yDeg = (REAL32)(point.y - m_DragStartPt.y) * amount;
					#endif

					m_DragStartPt = point;
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
		if( dist <= I3_EPS)
			dist = 1.0f;

		ScreenToWorld( point.x, point.y, &pos, dist);

		i3Matrix::SetTranslate( &tmp, &pos);
		//i3Matrix::Mul( &tmp, GetCurrentCamera()->GetMatrix(), &tmp);

		m_pPointLight->SetWorldSpaceTransform( &tmp);
	}
}

void i3ViewerFramework::OnMouseWheel( UINT nFlag, short zDelta, POINT point)
{
	i3Framework::OnMouseWheel( nFlag, zDelta, point);
	if( getFocus() != nullptr)
		return;

	VEC3D vec;
	i3Camera * pCam = GetCurrentCamera();
	MATRIX * pCamera = pCam->GetMatrix();
	MATRIX tmp;
	REAL32 len;

	if( m_CamMoveMode == I3_VIEWER_CAM_MOVE_3DSMAX)
	{
		i3Vector::Copy( &vec, i3Matrix::GetAt( pCamera));
		i3Vector::Normalize( &vec, &vec);

		if( nFlag & MK_CONTROL)
			len = 0.01f;
		else
			len = 0.05f;

		if( zDelta > 0)
		{
			i3Vector::Scale( &vec, &vec, -m_CamDist * len);
		}
		else
		{
			i3Vector::Scale( &vec, &vec, m_CamDist * len);
		}

		/*
		#if defined( I3_FIXED_POINT)
			VEC3D campos, basepos;

			i3Vector::Copy( &campos, i3Matrix::GetPos( pCamera));
			i3Vector::Copy( &basepos, &m_BasePos);

			len = NearestPlanePoint( &basepos, &vec, &campos, &pc);
		#else
			len = NearestPlanePoint( &m_BasePos, &vec, i3Matrix::GetPos( pCamera), &pc);
		#endif
		
		if( zDelta > 0)
			len = len / -5.0f;
		else
			len = len / 5.0f;
			*/

		//i3Vector::Scale( &vec, &vec, len);

		i3Matrix::SetTranslate(&tmp, getX(&vec), getY(&vec), getZ(&vec));

		i3Matrix::Mul( pCamera, pCamera, &tmp);

		OnUpdateCameraTransform();
		OnUpdateCameraProjection();
	}
}

bool i3ViewerFramework::isShadowEnabled(void)
{
	i3Light * pLit = m_pLightAttr->GetLightObject();

	return pLit->isStyle( I3G_LIGHT_STYLE_SHADOWMAP);
}

void i3ViewerFramework::setShadowEnable( bool bFlag)
{
	i3Light * pLit = m_pLightAttr->GetLightObject();

	if( bFlag)
		pLit->addStyle( I3G_LIGHT_STYLE_SHADOWMAP);			// ShadowMap
	else
		pLit->removeStyle( I3G_LIGHT_STYLE_SHADOWMAP);			// ShadowMap
}

#define LIGHTTX_STEP		1.0f

void i3ViewerFramework::OnKeyDown( UINT32 nKey)
{
	#if defined( I3_DEBUG)
	if( GetKeyState( VK_LCONTROL) & 0x80)
	{
		switch( nKey)
		{
			case VK_F3 :	
				GetViewer()->SetDumpState( true);
				break;

			case 'A' :
			case VK_LEFT :
				m_LightTheta -= LIGHTTX_STEP;
				break;
			
			case 'D' :
			case VK_RIGHT :
				m_LightTheta += LIGHTTX_STEP;
				break;

			case 'W' :
			case VK_UP :
				m_LightRho -= LIGHTTX_STEP;
				break;
			
			case 'S' :
			case VK_DOWN :
				m_LightRho += LIGHTTX_STEP;
				break;

			case 'L' :
				break;

			case 'P' :
				break;

			case 'H' :				
				break;

			case 'M' :
				{
					i3Light * pLit = m_pLightAttr->GetLightObject();

					if( pLit->isStyle( I3G_LIGHT_STYLE_SHADOWMAP))
					{
						pLit->removeStyle( I3G_LIGHT_STYLE_SHADOWMAP);			// ShadowMap
						I3TRACE( "SHADOW OFF\n");
					}
					else
					{
						pLit->addStyle( I3G_LIGHT_STYLE_SHADOWMAP);			// ShadowMap
						I3TRACE( "SHADOW ON\n");
					}
				}
				break;

			case 'B' :
				{
					i3Node * pNode = i3SceneUtil::BuildSphereMesh( 1.0f, true, false, 30, 30);

					this->SetSceneGraph( pNode);
				}
				break;

			case 'C' :
				SetPanelVisible( !m_bPanelVisible);
				break;

			case VK_PRIOR :
				m_LightDist += 0.1f;				
				break;

			case VK_NEXT :
				m_LightDist -= 0.1f;				
				break;

			case VK_F11 :
				SaveScreenShot();
				break;
		}

		UpdateLightTx();
	}
	#endif

	switch( nKey)
	{
		case 'L' :
			//m_pViewer->SetDefaultLightEnable( ! m_pViewer->GetDefaultLightEnable());
			break;

		case 'G' :
			SetGridShow( !GetGridShow());
			break;

		case '1' :		SetCameraMoveMode( I3_VIEWER_CAM_MOVE_3DSMAX);	break;
		case '3' :		SetCameraMoveMode( I3_VIEWER_CAM_MOVE_FPS);		break;

		case 'A' :
		case VK_LEFT :		m_CamMoveKey |= I3_VIEWER_KEY_LEFT;		break;

		case 'D' :
		case VK_RIGHT :		m_CamMoveKey |= I3_VIEWER_KEY_RIGHT;		break;

		case 'W' :
		case VK_UP :		m_CamMoveKey |= I3_VIEWER_KEY_FRONT;	break;

		case 'S' :
		case VK_DOWN :		m_CamMoveKey |= I3_VIEWER_KEY_BACK;		break;

		case 'R' :
		case VK_PRIOR :		m_CamMoveKey |= I3_VIEWER_KEY_UP;		break;

		case 'F' :
		case VK_NEXT :		m_CamMoveKey |= I3_VIEWER_KEY_DOWN;		break;

		case VK_ADD :		m_KeyBoardKey |= I3_VIEWER_KEY_NUMPAD_PLUS;		break;
		case VK_SUBTRACT :	m_KeyBoardKey |= I3_VIEWER_KEY_NUMPAD_MINUS;	break;

		case VK_SHIFT	:	m_KeyBoardKey |= I3_VIEWER_KEY_SHIFT;		break;
		case VK_CONTROL :	m_KeyBoardKey |= I3_VIEWER_KEY_CONTROL;		break;

		case VK_MULTIPLY :	m_KeyBoardKey |= I3_VIEWER_KEY_FRONT; break;
		case VK_DIVIDE :	m_KeyBoardKey |= I3_VIEWER_KEY_BACK;  break;
	}

	i3Framework::OnKeyDown( nKey);
}

void i3ViewerFramework::OnKeyUp( UINT32 nKey)
{
	switch( nKey)
	{
		case 'A' :
		case VK_LEFT :		m_CamMoveKey &= ~I3_VIEWER_KEY_LEFT;		break;

		case 'D' :
		case VK_RIGHT :		m_CamMoveKey &= ~I3_VIEWER_KEY_RIGHT;		break;

		case 'W' :
		case VK_UP :		m_CamMoveKey &= ~I3_VIEWER_KEY_FRONT;		break;

		case 'S' :
		case VK_DOWN :		m_CamMoveKey &= ~I3_VIEWER_KEY_BACK;		break;

		case 'R' :
		case VK_PRIOR :		m_CamMoveKey &= ~I3_VIEWER_KEY_UP;			break;

		case 'F' :
		case VK_NEXT :		m_CamMoveKey &= ~I3_VIEWER_KEY_DOWN;		break;

		case VK_ADD :		m_KeyBoardKey &= ~I3_VIEWER_KEY_NUMPAD_PLUS;	break;
		case VK_SUBTRACT :	m_KeyBoardKey &= ~I3_VIEWER_KEY_NUMPAD_MINUS;	break;

		case VK_SHIFT	:	m_KeyBoardKey &= ~I3_VIEWER_KEY_SHIFT;			break;
		case VK_CONTROL :	m_KeyBoardKey &= ~I3_VIEWER_KEY_CONTROL;		break;

		case VK_MULTIPLY :	m_KeyBoardKey &= ~I3_VIEWER_KEY_FRONT; break;
		case VK_DIVIDE :	m_KeyBoardKey &= ~I3_VIEWER_KEY_BACK;  break;
	}

#if defined( I3_DEBUG)
	{
		i3InputDeviceManager * pManager = m_pViewer->GetInputDeviceManager();
		i3InputKeyboard * pKbd = pManager->GetKeyboard();

		bool bShift = (GetKeyState( VK_LSHIFT) & 0x80) != 0;

		if( (pKbd == nullptr) && ((GetKeyState( VK_LCONTROL) & 0x80) != 0))
		{
			switch( nKey)
			{
				case VK_F1 :
					if( bShift)
					{
						if( m_pViewer->getProfileMode() == i3Viewer::PROFILE_RENDER)
							m_pViewer->setProfileMode( i3Viewer::PROFILE_NONE);
						else
							m_pViewer->setProfileMode( i3Viewer::PROFILE_RENDER);
					}
					else
					{
						m_pViewer->ReportPerformance();
					}
					break;

				case VK_F2 :
					if( bShift)
					{
						if( m_pViewer->getProfileMode() == i3Viewer::PROFILE_FRAMEWORK)
							m_pViewer->setProfileMode( i3Viewer::PROFILE_NONE);
						else
							m_pViewer->setProfileMode( i3Viewer::PROFILE_FRAMEWORK);
					}
					else
					{
						REPORT_PROFILE();
					}
					break;

				case VK_F3 :
					if( bShift)
					{
						if( m_pViewer->getProfileMode() == i3Viewer::PROFILE_PHYSIX)
							m_pViewer->setProfileMode( i3Viewer::PROFILE_NONE);
						else
							m_pViewer->setProfileMode( i3Viewer::PROFILE_PHYSIX);
					}
					else
					{
						I3TRACE( "MEMORY STATE : %d bytes (%d blocks)\n", i3mem::GetAllocSize(), i3mem::GetAllocCount());

						i3MemDump(I3MEMORY_DEBUG_EACH | I3MEMORY_DEBUG_FILE);
					}
					break;

				case VK_F4 :
					if( bShift)
					{
						if( m_pViewer->getProfileMode() == i3Viewer::PROFILE_CUSTOM)
							m_pViewer->setProfileMode( i3Viewer::PROFILE_NONE);
						else
							m_pViewer->setProfileMode( i3Viewer::PROFILE_CUSTOM);
					}
					break;

			}
		}
	}
#endif

	i3Framework::OnKeyUp( nKey);
}

void i3ViewerFramework::SaveScreenShot(void)
{	
	char szCurDir[1024] = {0,};
	char szTemp[1024] = {0,};
	char szScreenShot[1024] = {0,};

	// 날짜 및 시간 얻어오기
	SYSTEMTIME sysTime;
	::GetLocalTime(&sysTime);
			
	GetCurrentDirectory( 1024, szCurDir);

	// start working for files
	//i3::safe_string_copy( szScreenShot, szCurDir, 1024);
	i3mem::FillZero( szScreenShot, sizeof(char)*1024);
	i3::safe_string_copy( szScreenShot, szCurDir, 1024);
	i3::generic_string_cat(szScreenShot, "\\ScreenShot");	

	CreateDirectory( szScreenShot, nullptr);

	sprintf( szTemp, "ScreenShot/SC%d%02d%02d_%02d%02d%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

	GetViewer()->GetRenderContext()->SaveScreenShotToFile( szTemp, "png");
}

#endif

void i3ViewerFramework::SelectTarget( HWND hwndReport)
{
}

void i3ViewerFramework::UnselectAllTarget(void)
{
}

void i3ViewerFramework::_SetGridSize( i3VertexArray * pVA, i3GeometryAttr * pGeoAttr, REAL32 sx, REAL32 sy)
{
	INT32 i, idx = 0;
	INT32 cx, cy;
	VEC3D pos;
	COLOR hiline, normline;
	REAL32 rsx, rsy;

	rsx = 1.0f / sx;
	rsy = 1.0f / sy;

	cx = (INT32)sx + 1;
	cy = (INT32)sy + 1;

	I3ASSERT( (cx * cy * 2) <= (INT32) pVA->GetCount());
	
	pVA->Lock( 0, 0, cx * cy);

	i3Vector::Set( &pos, 0.0f, 0.0f, 0.0f);

	i3Color::Set( &hiline, (UINT8) 128, 128, 128, 255);
	i3Color::Set( &normline, (UINT8) 64, 64, 64, 255);

	// 가로 방향 (0->+, 원점 포함)
	pos.x = -(sx * 0.5f) * rsx;

	for( i = 0; i < cx; i++)
	{
		pos.z = (sy * 0.5f) * rsy;
		pVA->SetPosition( idx, &pos);	idx++;

		pos.z = -sy * 0.5f * rsy;
		pVA->SetPosition( idx, &pos); idx++;

		if( (i % 5) == 0)
			pVA->SetColor( idx - 2, 2, &hiline);
		else
			pVA->SetColor( idx - 2, 2, &normline);

		pos.x += (1.0f * rsx);
	}

	// 가로 방향 (0->+, 원점 포함)
	pos.z = -(sy * 0.5f) * rsy;
	for( i = 0; i < cy; i++)
	{
		pos.x = sx * 0.5f * rsx;
		pVA->SetPosition( idx, &pos); idx++;

		pos.x = -sx * 0.5f * rsx;
		pVA->SetPosition( idx, &pos); idx++;

		if( (i % 5) == 0)
			pVA->SetColor( idx - 2, 2, &hiline);
		else
			pVA->SetColor( idx - 2, 2, &normline);

		pos.z += (1.0f * rsy);
	}

	pVA->Unlock();

	pGeoAttr->SetPrimitiveCount( cx * cy);
}


void i3ViewerFramework::setShadowMapSize( INT32 cx)
{
	i3Light * pLight = m_pLightAttr->GetLightObject();

	pLight->setShadowMapSize( cx);

	if( pLight->isStyle( I3G_LIGHT_STYLE_SHADOWMAP))
	{
		pLight->CreateShadowMap();
	}
}
