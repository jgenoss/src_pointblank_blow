#include "StdAfx.h"
#include "ItemToolFrameWork.h"
#include "ItemTabManager.h"

I3_CLASS_INSTANCE(cItemToolFrameWork); //, i3ViewerFramework)

cItemToolFrameWork::cItemToolFrameWork(void)
:	m_pItemTabManager(NULL)
,	m_pSgInfo(NULL)
,	m_pInterLinkNode(NULL)
,	m_pNormalViewRoot(NULL)
,	m_bResetCamera(true)
{
}

cItemToolFrameWork::~cItemToolFrameWork(void)
{
 	removeAllInitBone();
 
 	I3_SAFE_RELEASE(m_pItemTabManager);
 
 	I3_SAFE_RELEASE( m_pSgInfo);
 
 	I3_SAFE_NODE_RELEASE( m_pInterLinkNode);
 	
 	I3_SAFE_RELEASE( m_pNormalViewRoot);
}

void cItemToolFrameWork::OnCreate()
{
	i3ViewerFramework::OnCreate();

	m_pItemTabManager = cItemTabManager::new_object();
	m_pItemTabManager->InitTab(NULL);

	if (NULL == m_pInterLinkNode)
	{
		m_pInterLinkNode = i3Node::new_object();

		m_pInterLinkNode->SetFlag( I3_NODEFLAG_VOLATILE | I3_NODEFLAG_CONTROL);
		m_pInterLinkNode->SetName( "_InterLinkNode_");
	}
}

void cItemToolFrameWork::OnUpdate( RT_REAL32 timeDelta)
{
	i3ViewerFramework::OnUpdate(timeDelta);

	m_pItemTabManager->OnUpdate(timeDelta);
}

void cItemToolFrameWork::RefineSg( bool bResetView)
{
	i3Node * pRoot = NULL;

	//m_pSelNode = NULL;

	m_pInterLinkNode->RemoveFromParent();
	m_pInterLinkNode->RemoveAllChild();

	if( m_pSgInfo != NULL)
	{
		if( m_pSgInfo->getCommonSg() != NULL)
		{
			if( m_pSgInfo->getInstanceSg() != NULL)
			{
				i3Node * pAttachNode = m_pSgInfo->getCommonSg();

				while( pAttachNode->GetChildCount() > 0)
				{
					pAttachNode = pAttachNode->GetChild( pAttachNode->GetChildCount() - 1);
				}

				{
					pAttachNode->AddChild( m_pInterLinkNode);

					m_pInterLinkNode->AddChild( m_pSgInfo->getInstanceSg());
				}
			}

			pRoot = m_pSgInfo->getCommonSg();
		}
		else
		{
			pRoot = m_pSgInfo->getInstanceSg();
		}
	}

	if( pRoot != NULL)
	{
		pRoot->Instantiate();
	}

	

	I3_REF_CHANGE( m_pNormalViewRoot, pRoot);

	SetDynamicState( false);

	BuildInitBone();

	//setViewMode( VIEWER_MODE_NORMAL, bResetView);
	SetSceneGraph( m_pNormalViewRoot, true);

	UpdateBound();
}

void cItemToolFrameWork::SetSceneGraph( i3Node * pNode, bool bReset)
{
	i3Node * pParent = OnQueryAttachNode( NULL);

	if( m_pLoadedScene != NULL)
	{
		pParent->RemoveChild( m_pLoadedScene);
		m_pLoadedScene->Release();
	}

	m_pLoadedScene = pNode;

	if( pNode != NULL)
	{
		pParent->AddChild( pNode);
		m_pLoadedScene->AddRef();
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

void cItemToolFrameWork::SetDynamicState( bool bFlag)
{
	if( m_pNormalViewRoot == NULL)
		return;

	UINT32 addState, remState;

	addState = remState = 0;

	if( bFlag)
	{
		remState = I3_PHYSIX_STATE_DISABLE | I3_PHYSIX_STATE_KINEMATIC;
	}
	else
	{
		addState = I3_PHYSIX_STATE_DISABLE | I3_PHYSIX_STATE_KINEMATIC;
	}

	VEC3D vel;

	i3Vector::Set( &vel, 0.0f, 0.0f, 0.0f);

	i3Scene::ModifyDynamicState( m_pNormalViewRoot, addState, remState);
	i3Scene::SetDynamicVelocity( m_pNormalViewRoot, NULL, &vel, &vel);
}
void cItemToolFrameWork::removeAllInitBone(void)
{
	INT32 i;

	for( i = 0; i < getInitBoneCount(); i++)
	{
		INIT_BONE_INFO * pInfo = getInitBone( i);

		delete pInfo;
	}

	m_InitBoneList.clear();
}

void cItemToolFrameWork::BuildInitBone(void)
{
	removeAllInitBone();

	if( m_pNormalViewRoot != NULL)
	{
		i3Scene::Traverse( m_pNormalViewRoot, _BuildInitBoneProc, this);
	} 
}

I3SG_TRAVERSAL_RESULT cItemToolFrameWork::_BuildInitBoneProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	INT32 i;
	cItemToolFrameWork * pFramework = (cItemToolFrameWork *) pUserData;

	if( i3::kind_of<i3Body*>(pNode)) //->IsTypeOf( i3Body::GetClassMeta()))
	{
		i3Body * pBody = (i3Body *) pNode;
		i3LOD * pLOD = pBody->getFrameLOD();

		if( pLOD != NULL)
		{
			i3Skeleton * pSkel = pLOD->getSkeleton();

			for( i = 0; i < pSkel->getBoneCount(); i++)
			{
				pFramework->addInitBone( pSkel->getName( i), pSkel->getMatrix( i));
			}
		}
	}
	else if( i3::kind_of<i3Transform*>(pNode)) //->IsTypeOf( i3Transform::GetClassMeta()))
	{
		i3Transform * pTrans = (i3Transform *) pNode;

		pFramework->addInitBone( pTrans->GetName(), pTrans->GetMatrix());
	}

	return I3SG_TRAVERSAL_CONTINUE;
}


void cItemToolFrameWork::ApplyInitBone(void)
{
	if( m_pNormalViewRoot != NULL)
		i3Scene::Traverse( m_pNormalViewRoot, _ApplyInitBoneProc, this);
}

I3SG_TRAVERSAL_RESULT cItemToolFrameWork::_ApplyInitBoneProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack)
{
	INT32 i;
	cItemToolFrameWork * pFramework = (cItemToolFrameWork *) pUserData;

	if( i3::kind_of<i3Body*>(pNode)) //->IsTypeOf( i3Body::GetClassMeta()))
	{
		i3Body * pBody = (i3Body *) pNode;
		i3LOD * pLOD = pBody->getFrameLOD();

		if( pLOD != NULL)
		{
			i3Skeleton * pSkel = pLOD->getSkeleton();

			for( i = 0; i < pSkel->getBoneCount(); i++)
			{
				INIT_BONE_INFO * pInfo = pFramework->findInitBone( pSkel->getName( i));

				if( pInfo != NULL)
					pSkel->setMatrix( i, &pInfo->m_Matrix);
			}
		}
	}
	else if( i3::kind_of<i3Transform*>(pNode)) //->IsTypeOf( i3Transform::GetClassMeta()))
	{
		i3Transform * pTrans = (i3Transform *) pNode;

		INIT_BONE_INFO * pInfo = pFramework->findInitBone( pTrans->GetName());

		if( pInfo != NULL)
			pTrans->SetMatrix( &pInfo->m_Matrix);
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

INIT_BONE_INFO * cItemToolFrameWork::addInitBone( const char * pszBone, MATRIX * pMatrix)
{
	INIT_BONE_INFO * pInfo;

	pInfo = new INIT_BONE_INFO;

	i3::safe_string_copy( pInfo->m_szName, pszBone, 256 );
	i3Matrix::Copy( &pInfo->m_Matrix, pMatrix);

	m_InitBoneList.push_back( pInfo);

	return pInfo;
}


INIT_BONE_INFO * cItemToolFrameWork::findInitBone( const char * pszBone)
{
	INT32 i;

	for( i = 0; i < getInitBoneCount(); i++)
	{
		INIT_BONE_INFO * pInfo = getInitBone( i);

		if( i3::generic_is_iequal( pInfo->m_szName, pszBone) )
			return pInfo;
	}

	return NULL;
}

void cItemToolFrameWork::OnMButtonDown( UINT32 nFlag, POINT point)
{
	RECT rtSrc	= m_pViewer->GetRenderContext()->GetSourcePresentRect();
	RECT rtDest = m_pViewer->GetRenderContext()->GetDestPresentRect();

	if (rtSrc.bottom == 0 && rtSrc.top == 0 && rtSrc.left == 0 && rtSrc.right == 0)
		return;

	if (rtDest.bottom == 0 && rtDest.top == 0 && rtDest.left == 0 && rtDest.right == 0)
		return;

	INT32 nSrcW = rtSrc.right - rtSrc.left;
	INT32 nSrcH = rtSrc.bottom - rtSrc.top;

	INT32 nDestW = rtDest.right - rtDest.left;
	INT32 nDestH = rtDest.bottom - rtDest.top;

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

void cItemToolFrameWork::OnMouseMove(UINT32 nFlag, POINT point)
{
	//여기 들어오는 마우스 포인트 값은 다이얼로그의 위치 값이다.
	i3RenderContext* pRenderContext = m_pViewer->GetRenderContext();

	RECT rtSrc	= pRenderContext->GetSourcePresentRect();
	RECT rtDest = pRenderContext->GetDestPresentRect();

	if (rtSrc.bottom == 0 && rtSrc.top == 0 && rtSrc.left == 0 && rtSrc.right == 0)
		return;

	if (rtDest.bottom == 0 && rtDest.top == 0 && rtDest.left == 0 && rtDest.right == 0)
		return;

	INT32 nSrcW = rtSrc.right - rtSrc.left;
	INT32 nSrcH = rtSrc.bottom - rtSrc.top;

	INT32 nDestW = rtDest.right - rtDest.left;
	INT32 nDestH = rtDest.bottom - rtDest.top;

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


void cItemToolFrameWork::OnResetCameraPosition()
{
	if (m_bResetCamera)
	{
		i3ViewerFramework::OnResetCameraPosition();
	}
	//리셋을 하지 않는다..
	//
}