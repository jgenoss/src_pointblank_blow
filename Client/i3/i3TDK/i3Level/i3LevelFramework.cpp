#include "stdafx.h"
#include "i3LevelFramework.h"
#include "../resource.h"
#include "i3LevelScene.h"
#include "i3LevelElementList.h"
#include "i3LevelGlobalVariable.h"
#include "i3LevelPrimitive.h"

#include "../i3TDKClipboard.h"
#include "../i3TDKGlobalRes.h"
#include "../i3TDKGlobalVariable.h"
#include "../i3TDKDlgRename.h"

#include "i3NavMesh.h"
#include "i3NavMeshDataMgr.h"

#include "i3NavMeshViewportControl_VertexSelect.h"
#include "i3NavMeshViewportControl_VertexAdd.h"
#include "i3NavMeshViewportControl_VertexMove.h"
#include "i3NavMeshViewportControl_PolySelect.h"
#include "i3NavMeshViewportControl_PolyMove.h"
#include "i3NavMeshViewportControl_PolyCut.h"

#define		AXIS_SIZE		0.25f

I3_CLASS_INSTANCE( i3LevelFramework);

namespace
{
	//////////////////////////////////////////////////////////////////////////
	// s_CtrlMetaTable 의 ::static_meta 가 nullptr 값이 넘어오는 경우가 있어 그걸 막기 위한 코드	
	//////////////////////////////////////////////////////////////////////////
	struct InitCtrlMetaTable
	{
		InitCtrlMetaTable()
		{

			i3LevelViewportControlObjectSelect::create_meta_explicit();	
			i3LevelViewportControlObjectMove::create_meta_explicit();	
			i3LevelViewportControlObjectRotate::create_meta_explicit();
			i3LevelViewportControlObjectScale::create_meta_explicit();	
			i3LevelViewportControlObjectBrush::create_meta_explicit();
			i3LevelViewportControlPathCreate::create_meta_explicit();
			i3LevelViewportControlRespawnCreate::create_meta_explicit();
			i3LevelViewportControlSelectTarget::create_meta_explicit();
			i3NavMeshViewportControl_VertexSelect::create_meta_explicit();
			i3NavMeshViewportControl_VertexMove::create_meta_explicit();
			i3NavMeshViewportControl_VertexAdd::create_meta_explicit();
			i3NavMeshViewportControl_PolySelect::create_meta_explicit();
			i3NavMeshViewportControl_PolyMove::create_meta_explicit();
			i3NavMeshViewportControl_PolyCut::create_meta_explicit();
		}
	} _tmpInitMetaTable;
	//////////////////////////////////////////////////////////////////////////	
}


static i3ClassMeta * s_CtrlMetaTable[ i3LevelFramework::CTRL_MODE_MAX] =
{
	nullptr,												
	i3LevelViewportControlObjectSelect::static_meta(),	
	i3LevelViewportControlObjectMove::static_meta(),	
	i3LevelViewportControlObjectRotate::static_meta(),	
	i3LevelViewportControlObjectScale::static_meta(),	
	i3LevelViewportControlObjectBrush::static_meta(),
	i3LevelViewportControlPathCreate::static_meta(),
	i3LevelViewportControlRespawnCreate::static_meta(),
	i3LevelViewportControlSelectTarget::static_meta(),
	
	// navmesh
	i3NavMeshViewportControl_VertexSelect::static_meta(),
	i3NavMeshViewportControl_VertexMove::static_meta(),
	i3NavMeshViewportControl_VertexAdd::static_meta(),
	i3NavMeshViewportControl_PolySelect::static_meta(),
	i3NavMeshViewportControl_PolyMove::static_meta(),
	i3NavMeshViewportControl_PolyCut::static_meta(),
};




i3LevelFramework::i3LevelFramework(void)
{
	INT32 i;

	m_Ctx.m_pFramework	= this;
	
	{
		m_pRoot = i3Node::new_object();
		m_pRoot->SetName( "i3LevelFramework::m_pRoot");

		m_pSceneRoot = i3AttrSet::new_object();
		m_pSceneRoot->SetName( "Viewport_Root");
		m_pRoot->AddChild( m_pSceneRoot);

		m_pAxisRoot = i3Node::new_object();
		m_pAxisRoot->SetName( "Viewport_AxisRotate");
		m_pRoot->AddChild( m_pAxisRoot);
	}

	for( i = 0; i < CTRL_MODE_MAX; i++)
	{
		i3ClassMeta * pMeta = s_CtrlMetaTable[i];

		if( pMeta != nullptr)
		{
			m_pCtrlTable[i] = (i3LevelViewportControl *) pMeta->create_instance(); // CREATEINSTANCE( pMeta);
			I3_MUST_ADDREF(m_pCtrlTable[i]);

			m_pCtrlTable[i]->Create();
		}
		else
		{
			m_pCtrlTable[i] = nullptr;
		}
	}
	
	SetLevelEditMode( LEM_NORMAL );
}

i3LevelFramework::~i3LevelFramework(void)
{
	INT32 i;

	I3_SAFE_RELEASE( m_pScene );

	I3_SAFE_RELEASE( m_pFreeCamera );
	I3_SAFE_RELEASE( m_pWalkCamera );
	I3_SAFE_RELEASE( m_pTopCamera );
	I3_SAFE_RELEASE( m_pFrontCamera);		// 이것이 없어서 릭이 났었음..추가.(2012.09.11.수빈)
	//I3_SAFE_RELEASE( m_pNavmeshCamera );
	I3_SAFE_RELEASE( m_pCurCamera);

	I3_SAFE_RELEASE( m_pSceneRoot);
	I3_SAFE_RELEASE( m_pAxisRoot);
	I3_SAFE_RELEASE( m_pRoot);

	for( i = 0; i < CTRL_MODE_MAX; i++)
	{
		I3_SAFE_RELEASE( m_pCtrlTable[i]);
	}
}

void i3LevelFramework::OnCreate(void)
{
	i3ViewerFramework::OnCreate();

	SetPanelVisible( false);
	SetDebugTextEnable( FALSE);

	SetSceneGraph( m_pRoot);

	// Physix Context
	{
		i3PhysixContext * pCtx = GetViewer()->GetPhysixContext();

		for( INT32 i = 0; i < 32; i++)
		{
			pCtx->setGroupCollisionEnable( (UINT16) i);
		}
	}

	// Camera
	{
		{
			m_pFreeCamera = i3LevelCameraFree::new_object();
		
			//m_pFreeCamera->SetViewport( this);
			m_pFreeCamera->AddStyle( I3_LEVEL_STYLE_VOLATILE );
			m_pFreeCamera->SetRotateDeg( -45.0f, 45.0f, 0.0f );
			m_pFreeCamera->setPos( 10.0f, 10.0f, 10.0f );
		}

		{
			m_pWalkCamera = i3LevelCameraWalk::new_object();
			m_pWalkCamera->AddStyle( I3_LEVEL_STYLE_VOLATILE );
			//m_pWalkCamera->SetViewport( this);
		}

		{
			m_pTopCamera = i3LevelCameraTop::new_object();

			//m_pTopCamera->SetViewport( this);
			m_pTopCamera->AddStyle( I3_LEVEL_STYLE_VOLATILE );
			m_pTopCamera->setPos( 0.0f, 10.0f, 0.0f );
			m_pTopCamera->SetRotateDeg( -90.0f, 0.0f, 0.0f );
		}

		{
			m_pFrontCamera = i3LevelCameraFront::new_object();

			m_pFrontCamera->AddStyle( I3_LEVEL_STYLE_VOLATILE );
			m_pFrontCamera->setPos( 0.0f, 0.0f, 10.0f );
		}


		SetCameraMode( CAMERA_MODE_FREE );
	}

	CreateAxis();

	{
		// Text Node
		m_pTextNode = i3TextNodeDX::new_object_ref();
		m_pTextNode->Create( "Tahoma", 256, 8);
		m_pTextNode->setSize( 256, 256);
		m_pTextNode->SetText( "Camera Mode");
		m_pTextNode->setPos( 10, 10);
		m_pTextNode->setShadowEnable( TRUE );

		Get2DRoot()->AddChild( m_pTextNode);
	}

	{
		m_pTextNodeNavMesh = i3TextNodeDX::new_object_ref();
		m_pTextNodeNavMesh->Create( "Tahoma", 256, 8 );
		m_pTextNodeNavMesh->setSize( 256, 256 );
		m_pTextNodeNavMesh->SetText( "" );
		m_pTextNodeNavMesh->setPos( 10, 10 );
		m_pTextNodeNavMesh->SetAlign( ALIGN_RIGHT, ALIGN_TOP );
		m_pTextNodeNavMesh->setShadowEnable( TRUE );

		Get2DRoot()->AddChild( m_pTextNodeNavMesh );
	}

	{
		i3PhysixContext * pCtx = GetViewer()->GetPhysixContext();

		UINT16 i;

		for( i = 0; i < 32; i++)
		{
			pCtx->setGroupMaskEnable( 0, i, true);
		}
	}

	_SetCtrlMode(  CTRL_MODE_SELECT);
}

void i3LevelFramework::OnUpdateCameraProjection(void)
{
	i3ViewerFramework::OnUpdateCameraProjection();

	if( m_pCurCamera)
	{
		m_pCurCamera->UpdateCamera( this);
	}
}

void i3LevelFramework::SetScene( i3LevelScene * pScene)
{
	if( m_pScene != nullptr)
	{
		m_pSceneRoot->RemoveAllChild();

		m_pScene->RemoveElement( m_pWalkCamera);

		I3_MUST_RELEASE(m_pScene);
		m_pScene = nullptr;
	}

	m_pScene = pScene;

	if( m_pScene != nullptr)
	{
		m_pSceneRoot->AddChild( m_pScene->GetRoot());
		I3_MUST_ADDREF(m_pScene);

		{
			VEC3D * pPos;

			pPos = m_pScene->GetStartPosition();

			m_pWalkCamera->setPos( i3Vector::GetX(pPos), i3Vector::GetY(pPos) + 2.0f, i3Vector::GetZ(pPos));
			//m_pWalkCamera->SetRotate( 0.0f, m_pScene->GetStartRotation(), 0.0f);
			m_pWalkCamera->SetNear( m_pScene->GetCameraNear());
			m_pWalkCamera->SetFar( m_pScene->GetCameraFar());
		}

		m_pScene->AddElement( m_pWalkCamera, "Walker");
	}

	SetCurrentCamera( m_pCurCamera);

	setSpaceMode( I3LV_SPACE::WORLD);
	_SetCtrlMode( CTRL_MODE_SELECT);
}

void i3LevelFramework::SetClearColor( COLOR * pCol)
{
	i3RenderContext * pCtx = GetViewer()->GetRenderContext();
	
	pCtx->SetClearColor( pCol);
}

void i3LevelFramework::SetMouseCursor( HCURSOR hCursor)
{
	if( m_pViewport != nullptr)
	{
		::SetClassLong( m_pViewport->m_hWnd, GCL_HCURSOR, (LONG) hCursor);
		::SetCursor( hCursor);
	}
}

void i3LevelFramework::GetClearColor( COLOR * pCol)
{
	i3RenderContext * pCtx = GetViewer()->GetRenderContext();

	i3Color::Set( pCol, pCtx->GetClearColor());
}

void i3LevelFramework::SetCurrentCamera( i3LevelCamera * pCam)
{
	I3_REF_CHANGE( m_pCurCamera, pCam);

	if( m_pCurCamera != nullptr)
	{
		getSgContext()->setCurrentCamera( m_pCurCamera->GetCameraNode());
	}
}

void i3LevelFramework::SetCameraMode( CAMERA_MODE mode)
{
	i3LevelCamera * pNewCam = nullptr;

	switch( mode)
	{
		case CAMERA_MODE_FREE :		pNewCam = m_pFreeCamera;	break;
		case CAMERA_MODE_WALK :		pNewCam = m_pWalkCamera;	break;
		case CAMERA_MODE_TOP :		pNewCam = m_pTopCamera;		break;
		case CAMERA_MODE_FRONT:		pNewCam = m_pFrontCamera;	break;

		case CAMERA_MODE_SCENE:		
			{
				pNewCam = m_pSceneCamera;	
				m_pSceneCamera->Play(); 
			}
			break;
	}

	m_CameraMode = mode;

	if( pNewCam != nullptr)
	{
		SetCurrentCamera( pNewCam);
	}
}

void i3LevelFramework::SetCollisionEditMode( bool bFlag)
{
	
	i3::vector<i3LevelElement3D*> list;
	i3LevelElement3D * pElm;
	
	m_pScene->GetSelectedElements( list);

	for( size_t i = 0; i < list.size(); i++)
	{
		pElm = list[i];

		pElm->SetCollisionEditEnable( bFlag);
	}

	m_bCollisionEdit = bFlag;
}

void i3LevelFramework::SetCollisionVisible( bool bFlag)
{

	i3::vector<i3LevelElement3D*> list;
	i3LevelElement3D * pElm;

	if( bFlag)
	{
		m_pScene->GetSelectedElements( list);

		for(size_t i = 0; i < list.size(); i++)
		{
			pElm = list[i];

			pElm->SetCollisionVisible( true);
		}
	}
	else
	{
		for(INT32 i = 0; i < m_pScene->GetElementCount(); i++)
		{
			pElm = m_pScene->GetElement( i);

			pElm->SetCollisionVisible( false);
		}
	}
}


void i3LevelFramework::ScreenToWorld( INT32 scrX, INT32 scrY, VEC3D * pVec1, REAL32 zDistance)
{
	MATRIX mat;
	VEC3D vec1, vec2, dir;
	REAL32 dx, dy;
	INT32 w, h;

	w = m_pViewport->GetViewWidth();
	h = m_pViewport->GetViewHeight();

	i3Matrix::Inverse( &mat, nullptr, m_pCurCamera->GetProjectionMatrix());

	// Screen 좌표를 0.0~1.0 범위의 좌표로 Normalize한다.
	dx = ((REAL32)scrX / w * 2.0f) - 1.0f;
	dy = ((REAL32)scrY / h * 2.0f) - 1.0f;

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

void i3LevelFramework::ScreenToWorld( INT32 x1, INT32 y1, INT32 x2, INT32 y2, VEC3D * pDiff, REAL32 zDist)
{
	MATRIX tmx;
	VEC3D v1, v2;
	REAL32 dx, dy, dz;
	INT32 w, h;

	w = m_pViewport->GetViewWidth();
	h = m_pViewport->GetViewHeight();

	dz = (zDist - m_pCurCamera->GetNear()) / (m_pCurCamera->GetFar() - m_pCurCamera->GetNear());

	// Screen 좌표를 0.0~1.0 범위의 좌표로 Normalize한다.
	dx = ((REAL32)x1 / w * 2.0f) - 1.0f;
	dy = ((REAL32)y1 / h * 2.0f) - 1.0f;
	i3Vector::Set( &v1, dx, dy, dz);

	dx = ((REAL32)x2 / w * 2.0f) - 1.0f;
	dy = ((REAL32)y2 / h * 2.0f) - 1.0f;
	i3Vector::Set( &v2, dx, dy, dz);

	i3Matrix::Inverse( &tmx, nullptr, m_pCurCamera->GetProjectionMatrix());

	i3Vector::TransformCoord( &v1, &v1, &tmx);
	i3Vector::TransformCoord( &v2, &v2, &tmx);

	i3Vector::Sub( pDiff, &v2, &v1);
}

void i3LevelFramework::GetViewProjectionMatrix( MATRIX * pMatrix)
{
	if (m_pCurCamera == nullptr)		// 이런 경우가 있을수 있어 반드시 널검사를 해야한다...(2016.08.01.수빈)
		return;
	i3Matrix::Mul( pMatrix, m_pCurCamera->GetViewMatrix(), m_pCurCamera->GetProjectionMatrix());
}


void i3LevelFramework::SaveMatrix(void)
{
	if( m_pScene != nullptr)
		m_pScene->SaveMatrix();

	if( m_pCurAxis != nullptr)
		m_pCurAxis->SaveMatrix();
}

void i3LevelFramework::RestoreMatrix(void)
{
	if( m_pScene != nullptr)
		m_pScene->RestoreMatrix();

	if( m_pCurAxis != nullptr)
		m_pCurAxis->RestoreMatrix();
}


bool i3LevelFramework::GetPickedPos( CPoint point, VEC3D * pPos)
{
	MATRIX tmx;
	REAL32 dx, dy, t;
	VEC3D pos1, pos2, norm, origin;
	INT32 w, h;

	w = m_pViewport->GetViewWidth();
	h = m_pViewport->GetViewHeight();

	GetViewProjectionMatrix( &tmx);
	i3Matrix::Inverse( &tmx, nullptr, &tmx);

	dx = ((REAL32)point.x / w * 2.0f) - 1.0f;
	dy = ((REAL32)point.y / h * 2.0f) - 1.0f;

	i3Vector::Set( &pos1, dx, -dy, 0.0f);
	i3Vector::Set( &pos2, dx, -dy, 1.0f);

	i3Vector::TransformCoord( &pos1, &pos1, &tmx);
	i3Vector::TransformCoord( &pos2, &pos2, &tmx);

	// 우선 Raycasting을 시도한다.
	{
		I3_PHYSIX_HIT_RESULT * pResult;
		i3CollideeLine line;
		i3PhysixContext * pPhysixCtx = GetViewer()->GetPhysixContext();

		line.SetStart( &pos1);
		line.SetEnd( &pos2);

		pResult = pPhysixCtx->RaycastClosest( &line, 0);

		if( pResult != nullptr)
		{
			i3Vector::Copy( pPos, & pResult->m_Intersect);
			return true;
		}
	}

	i3Vector::Set( &norm, 0.0f, 1.0f, 0.0f);
	i3Vector::Set( &origin, 0.0f, 0.0f, 0.0f);

	IntersectTriLine( &pos1, &pos2, &norm, &origin, pPos, &t);

	return false;
}

void i3LevelFramework::Fit(void)
{
	VEC3D vmin, vmax, mid;
	i3::vector<i3LevelElement3D*> SelList;
	REAL32 range;
	MATRIX * pMatrix;

	pMatrix = m_pCurCamera->GetTransform();

	switch( m_LevelEditMode )
	{
	case	LEM_NORMAL:
			{
				m_pScene->GetSelectedElements( SelList);

				if( SelList.size() > 0)
					m_pScene->GetWrappingBoundBoxForSelect( &vmin, &vmax);
				else
					m_pScene->GetWrappingBoundBox( &vmin, &vmax);
			}
			break;

	case	LEM_NAVMESH_POINT:
	case	LEM_NAVMESH_POLYGON:
			{
				if( i3Level::GetNavMesh() )
				{
					if( true == i3Level::GetNavMesh()->GetSelectedMinMax( vmin, vmax ) )
					{
							vmin.x -= 5.0f, vmin.y -= 5.0f, vmin.z -= 5.0f;
							vmax.x += 5.0f, vmax.y += 5.0f, vmax.z += 5.0f;
					}
					else
					{
						m_pScene->GetWrappingBoundBox( &vmin, &vmax );
					}
				}
			}
			break;
	}

	range = i3Math::abs( i3Vector::GetX( &vmax) - i3Vector::GetX( &vmin));	
	range = MAX( range, i3Math::abs( i3Vector::GetY( &vmax) - i3Vector::GetY( &vmin)));
	range = MAX( range, i3Math::abs( i3Vector::GetZ( &vmax) - i3Vector::GetZ( &vmin)));

	if( range <= 0.0f)
	{
		range = 1.0f;

		i3Vector::Set( &vmin, -0.5f, -0.5f, -0.5f);
		i3Vector::Set( &vmax, 0.5f, 0.5f, 0.5f);
	}

	i3Vector::Add( &mid, &vmin, &vmax);
	i3Vector::Scale( &mid, &mid, 0.5f);

	{
		VEC3D vec;

		i3Vector::Scale( &vec, i3Matrix::GetAt( pMatrix), range * 1.5f);
		i3Vector::Add( &vec, &vec, &mid);

		i3Matrix::SetPos( pMatrix, &vec);
	}

	ResizeAxis();
	m_pViewport->InvalidateRect( nullptr);
}

void i3LevelFramework::Fit( i3LevelElement3D * pElm)
{
	m_pScene->UnselectAll();

	m_pScene->SelectElement( pElm);

	Fit();
}

void	i3LevelFramework::OnNavSelect()
{
	if( nullptr == i3Level::GetNavMesh() )
	{
		return;
	}

	bool bRes = false;	
	i3Vector::Set( &m_Ctx.m_BasePos, 0,0,0 );

	switch( GetLevelEditMode() )
	{
	case	LEM_NAVMESH_POINT:
			bRes = i3Level::GetNavMesh()->GetSelectedCenterPosition( true, m_Ctx.m_BasePos );
			break;

	case	LEM_NAVMESH_POLYGON:
			bRes = i3Level::GetNavMesh()->GetSelectedCenterPosition( false, m_Ctx.m_BasePos );
			break;
	
	default:
		break;
	}

	if( m_pCurAxis )
	{
		if( bRes )
		{
			m_pCurAxis->setPos( &m_Ctx.m_BasePos );
			m_pCurAxis->Enable();

			ResizeAxis();
		}
		else
		{
			m_pCurAxis->Disable();
		}
	}
}


void i3LevelFramework::OnSelect( i3LevelElement3D * pElm)
{
	VEC3D vmin, vmax;
	i3::vector<i3LevelElement3D*>	SelList;

	if( m_pCurAxis == nullptr)
		return;

	if( m_pScene != nullptr)
	{
		m_pScene->GetSelectedElements( SelList);
	}

	if( SelList.size() > 0)
	{
		if (m_pScene)
		{
			m_pScene->GetWrappingBoundBoxForSelect(&vmin, &vmax);
		}
		else
		{
			I3ASSERT(m_pScene);
		}

		i3Vector::Add( &m_Ctx.m_BasePos, &vmin, &vmax);
		i3Vector::Scale( &m_Ctx.m_BasePos, &m_Ctx.m_BasePos, 0.5f);

		if( SelList.size() == 1)
		{
			// 1개인 경우에만 Target Object로 설정한다.
			m_pCurAxis->setTargetObject( (i3LevelElement3D *) SelList[0]);
		}
		else
		{
			m_pCurAxis->setTargetObject( nullptr);
		}

		m_pCurAxis->setBound( vmin, vmax);

		ResizeAxis();
		m_pCurAxis->setPos( getX( &m_Ctx.m_BasePos), getY( &m_Ctx.m_BasePos), getZ( &m_Ctx.m_BasePos));

		m_pCurAxis->Enable();
	}
	else
	{
		i3Vector::Set( &m_Ctx.m_BasePos, 0.0f, 0.0f, 0.0f);
		m_pCurAxis->Disable();

		m_pCurAxis->setTargetObject( nullptr);
	}
}

void i3LevelFramework::OnUpdateEvent( I3_TDK_UPDATE_INFO * pInfo)
{
	VEC3D minVec, maxVec;
	i3ElementBase * pObj = pInfo->m_pObject;

	if( pInfo->m_Event & I3_TDK_UPDATE_SELECT)
	{
		if( pObj != nullptr)
		{
			if( i3::kind_of<i3LevelElement3D* >(pObj))
			{
				OnSelect( (i3LevelElement3D *) pObj);
			}
			else if( i3::kind_of<i3LevelRes* >(pObj))
			{
				m_pSelectedRes = (i3LevelRes *) pObj;
			}
		}
		else
		{
			i3Vector::Set( &m_Ctx.m_BasePos, 0.0f, 0.0f, 0.0f);
		}
	}
	else if( pInfo->m_Event & I3_TDK_UPDATE_UNSELECT)
	{
		OnSelect( (i3LevelElement3D *) pObj);
	}

	if( pInfo->m_Event & I3_TDK_UPDATE_REMOVE)
	{
		if( i3::kind_of<i3LevelRes*>(pInfo->m_pMeta))
		{
			i3LevelRes * pRes = (i3LevelRes *) pInfo->m_pObject;

			if( pRes != nullptr)
			{
				if( pRes == m_pSelectedRes)
					m_pSelectedRes = nullptr;
			}
			else
			{
				m_pSelectedRes = nullptr;
			}
		}
		else if( i3::kind_of<i3LevelScene *>( pInfo->m_pMeta))
		{
			// 기존의 Scene이 닫힌다.
			SetScene( nullptr);
		}
	}

	if( pInfo->m_Event & I3_TDK_UPDATE_MOVE)
	{
		if( LEM_NORMAL == GetLevelEditMode()  )
		{
			m_pScene->GetWrappingBoundBoxForSelect( &minVec, &maxVec);

			m_pCurAxis->setBound( minVec, maxVec);

			i3Vector::Add( &minVec, &minVec, &maxVec);
			i3Vector::Scale( &m_Ctx.m_BasePos, &minVec, 0.5f);

			ResizeAxis();

			m_pCurAxis->setPos( &m_Ctx.m_BasePos);
		}
		else
		{
			OnNavSelect();
		}
	}

	if( pInfo->m_Event & I3_TDK_UPDATE_SCENECAMERA )
	{
		if( i3::kind_of<i3LevelSceneCamera*>(pObj))
		{
			SetSceneCamera((i3LevelSceneCamera *) pObj); 
			SetCameraMode(CAMERA_MODE_SCENE);
		}
	}
}

void i3LevelFramework::CreateAxis(void)
{
}

void i3LevelFramework::ResizeAxis(void)
{
	if( nullptr == m_pCurCamera )
	{
		return;
	}

	MATRIX	matScale;
	MATRIX	matView;
	VEC3D	vScale, vView;

	// Update 타이밍에 따라 카메라 행렬 값에 차이가 있을 수 있어 수정 함
	//i3Matrix::Copy( &matScale, m_pCurCamera->GetTransform() );	
	i3Matrix::Inverse( &matView, nullptr, m_pCurCamera->GetTransform() );
	i3Matrix::TransformPoints( &vView, &m_Ctx.m_BasePos, 1, &matView );

	REAL32	fLength = i3Vector::Length( &vView );

	GetViewProjectionMatrix( &matScale );
	
	i3Vector::TransformCoord( &vScale, &m_Ctx.m_BasePos, &matScale);
	
	if( i3Vector::isValid( &vScale) == false)			// 실제로 이런 경우가 발생한다.
		return;

	REAL32	fScale = (fLength - i3Vector::Length( &vScale )) * AXIS_SIZE;

	if( fScale < 0.0f)
	{
		fScale = 1.0f;
	}
	
	i3Vector::Set( &vScale, fScale, fScale, fScale );

	if( m_pCurAxis )
	{
		MATRIX * pDest = m_pCurAxis->GetTransform();
		VEC3D axis;
		bool bIdentity = true;

		if( getSpaceMode() == I3LV_SPACE::LOCAL)
		{
			i3LevelElement3D * pObj = m_pCurAxis->getTargetObject();

			// 각 축의 방향을 Local 축으로 변경.
			if( pObj != nullptr)
			{
				MATRIX * pMtx = pObj->GetTransform();
				
				i3Vector::Normalize( &axis, i3Matrix::GetRight( pMtx));
				i3Matrix::SetRight( pDest, &axis);

				i3Vector::Normalize( &axis, i3Matrix::GetUp( pMtx));
				i3Matrix::SetUp( pDest, &axis);
				
				i3Vector::Normalize( &axis, i3Matrix::GetAt( pMtx));
				i3Matrix::SetAt( pDest, &axis);

				bIdentity = false;
			}
		}
		
		if( bIdentity)
		{
			i3Vector::Set( &axis, 1.0f, 0.0f, 0.0f);
			i3Matrix::SetRight( pDest, &axis);

			i3Vector::Set( &axis, 0.0f, 1.0f, 0.0f);
			i3Matrix::SetUp( pDest, &axis);

			i3Vector::Set( &axis, 0.0f, 0.0f, 1.0f);
			i3Matrix::SetAt( pDest, &axis);
		}

		m_pCurAxis->SetScale( &vScale );

		m_pCurAxis->OnUpdateTransform();
	}



}

BOOL i3LevelFramework::CollisionGizmo( CPoint point )
{
	VEC3D	vOriStart, vOriEnd;
	MATRIX	matViewProject;
	INT32	hw, hh;
	REAL32	rw, rh;

	if( i3::same_of< i3LevelAxis* >(m_pCurAxis))
		return FALSE;

	GetViewProjectionMatrix( &matViewProject);
	{
		// 화면 한가운데를 ( 0, 0)으로 간주한 Clipping-Space로의 좌표 변환용.
		hw = m_pViewport->GetViewWidth() >> 1;
		hh = m_pViewport->GetViewHeight() >> 1;

		rw = 1.0f / hw;
		rh = -1.0f / hh;

		// Clipping Space에서의 시작점과 끝점을 설정한다.
		i3Vector::Set( &vOriStart, (point.x - hw) * rw, (point.y - hh) * rh, 0.0f);
		i3Vector::Set( &vOriEnd, (point.x - hw) * rw, (point.y - hh) * rh, 0.9f);
	}

	return m_pCurAxis->OnCollision( this, &m_Ctx, &vOriStart, &vOriEnd, &matViewProject) == true;
}

void i3LevelFramework::OnUpdate( REAL32 tm)
{
	char szText[512];
	static char szCtrlModeName[CTRL_MODE_MAX][64] =
	{
		"Normal",
		"Select",
		"Move",
		"Rotate",
		"Scale",
		"Brush",
		"Path Create",
		"Respawn Create",
		"Select Target",

		// navmesh
		"Select - Navmesh Point",
		"Move - Navmesh Point",
		"Vertex Add - Navmesh Point"

		"Select - Navmesh Polygon",
		"Move - Navmesh Polygon",
	};

	sprintf( szText, "MODE: %s\nCAMERA:%s\nFPS : %0.3f", szCtrlModeName[ m_CtrlMode], m_pCurCamera->GetName(), 1.0f / tm );
	m_pTextNode->SetText( szText );

	{
	}
	
	_OnUpdate_Camera( tm );
	i3ViewerFramework::OnUpdate( tm );

	if( m_pCtrl != nullptr)
	{
		m_pCtrl->OnUpdate( tm);
	}

	if( i3Level::GetNavMesh() )
		i3Level::GetNavMesh()->Update();

	m_LevelHitMap.Update();
}


void i3LevelFramework::PopupMenu(void)
{
	m_CamMoveKey = 0;
	m_KeyBoardKey = 0;

	CMenu menu;
	POINT pt;
	UINT32 cmd;

	GetCursorPos( &pt);

	menu.CreatePopupMenu();

	menu.AppendMenu( MF_ENABLED | MF_STRING, 1, _T("&Freeze Selection") );
	menu.AppendMenu( MF_ENABLED | MF_STRING, 2, _T("&Unfreeze All") );
	menu.AppendMenu( MF_SEPARATOR, 0, _T("") );
	menu.AppendMenu( MF_ENABLED | MF_STRING, 3, _T("Freeze All Triggers") );
	menu.AppendMenu( MF_ENABLED | MF_STRING, 4, _T("Unfreeze All Triggers") );
	menu.AppendMenu( MF_SEPARATOR, 0, _T("") );
	menu.AppendMenu( MF_ENABLED | MF_STRING, 5, _T("&Copy\t\tCtrl+C") );
	menu.AppendMenu( MF_ENABLED | MF_STRING, 6, _T("&Delete\tDel") );
	menu.AppendMenu( MF_ENABLED | MF_STRING, 7, _T("Cu&t\tCtrl+X") );

	{
		i3TDKClipboard clip;

		if( clip.IsObject( i3LevelElementList::static_meta()))
			menu.AppendMenu( MF_ENABLED | MF_STRING, 11, _T("&Paste\tCtrl+V") );
		else
			menu.AppendMenu( MF_GRAYED | MF_STRING, 11, _T("&Paste\tCtrl+V") );
	}

	// Group
	menu.AppendMenu( MF_SEPARATOR, 0, _T("") );

	{
		i3::vector<i3LevelElement3D*> list;

		m_pScene->GetSelectedElements( list);

		if( list.size() > 1)
			menu.AppendMenu( MF_ENABLED | MF_STRING, 14, _T("&Group 설정...") );
		else
			menu.AppendMenu( MF_GRAYED | MF_STRING, 14, _T("&Group 설정...") );

		// 선택된 Element들 중에 Group이 설정된 것이 있는지 확인
		bool bGrouped =false;

		for( size_t i = 0; i < list.size(); i++)
		{
			i3LevelElement3D * pElm = list[i];

			if( pElm->getGroup() != nullptr)
			{
				// 하나라도 Group이 설정된 것이 있다면...
				bGrouped = true;
				break;
			}
		}

		if( bGrouped)
			menu.AppendMenu( MF_ENABLED | MF_STRING, 15, _T("Group 해제...") );
		else
			menu.AppendMenu( MF_GRAYED | MF_STRING, 15, _T("Group 해제...") );
	}

	menu.AppendMenu( MF_SEPARATOR, 0, _T("") );
	menu.AppendMenu( MF_ENABLED | MF_STRING, 8, _T("&Move") );
	menu.AppendMenu( MF_ENABLED | MF_STRING, 9, _T("&Rotate") );
	menu.AppendMenu( MF_ENABLED | MF_STRING, 10, _T("&Scale") );
	menu.AppendMenu( MF_SEPARATOR, 0, _T("") );

	if( m_bPlay == TRUE)
		menu.AppendMenu( MF_ENABLED | MF_STRING | MF_CHECKED, 13, _T("&Playing") );
	else
		menu.AppendMenu( MF_ENABLED | MF_STRING | MF_UNCHECKED, 13, _T("&Playing") );

	Pause();

	cmd = menu.TrackPopupMenuEx( TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
				pt.x, pt.y, (i3TDKViewerCtrl *) m_pViewport, nullptr);

	switch( cmd)
	{
		case 1 :	m_pScene->FreezeSelection();	break;
		case 2 :	m_pScene->UnfreezeAll();		break;
		case 3 :	m_pScene->FreezeAllTriggers();	break;
		case 4 :	m_pScene->UnfreezeAllTriggers();	break;

		case 5 :	m_pScene->CopyClipboard( m_pViewport->m_hWnd);	break;
		case 6 :	m_pScene->RemoveSelectedElement();	break;
		case 7 :	m_pScene->CopyClipboard( m_pViewport->m_hWnd);
			m_pScene->RemoveSelectedElement();
			break;
		case 11 :
			{
				CPoint point( pt.x, pt.y);

				m_pScene->PasteClipboard( m_pViewport->m_hWnd, &point);
			}
			break;

		/*case 8 :	SetEditMode( CTRL_MODE_OBJECT_MOVE);	break;
		case 9 :	SetEditMode( CTRL_MODE_OBJECT_ROTATE);	break;
		case 10 :	SetEditMode( CTRL_MODE_OBJECT_SCALE);	break;*/

		case 13 :	if( m_bPlay)		Pause();
					else				Play();
			break;

		case 14 :	// Group 설정
			SetGroup();
			break;

		case 15 :	// Group 해제
			ReleaseGroup();
			break;

	}

	Play();
}

void i3LevelFramework::Copy(void)
{
	if( m_pScene != nullptr)
	{
		m_pScene->CopyClipboard( m_pViewport->m_hWnd);
	}
}

void i3LevelFramework::Cut(void)
{
	if( m_pScene != nullptr)
	{
		m_pScene->CopyClipboard( m_pViewport->m_hWnd);
		m_pScene->RemoveSelectedElement();
	}
}

void i3LevelFramework::Paste( bool bAutoPlace)
{
	if( m_pScene != nullptr)
	{
		CPoint pt;

		GetCursorPos( &pt);

		if( bAutoPlace)
			m_pScene->PasteClipboard( m_pViewport->m_hWnd, &pt);
		else
			m_pScene->PasteClipboard( m_pViewport->m_hWnd, nullptr);
	}
}

#if 1

void i3LevelFramework::PivotRotate( REAL32 RotX, REAL32 RotY, REAL32 RotZ)
{
	if( m_pCurCamera )
		m_pCurCamera->CameraPivotRotate( m_Ctx, RotX, RotY );

	if( i3Level::GetNavMesh() )
		i3Level::GetNavMesh()->UpdateCameraMove();

	ResizeAxis();
	m_pViewport->InvalidateRect( nullptr);
}
#else
void i3LevelFramework::PivotRotate( REAL32 RotX, REAL32 RotY, REAL32 /*RotZ*/)
{
	// Camera matrix
    MATRIX * pCamera;
	VEC3D vecRight, localPos, vAt;
	REAL32 atLength = 0.0f;

	RotX *= 5.0f;
	RotY *= 5.0f;

	pCamera = m_pCurCamera->GetTransform();

	i3Matrix::Copy( pCamera, &m_Ctx.m_DragStartMatrix);
	
	atLength = i3Vector::Length( i3Matrix::GetPos( pCamera));

	i3Vector::Scale( &vAt, i3Matrix::GetAt( pCamera), atLength);//At Position
	i3Vector::Sub( &localPos, i3Matrix::GetPos( pCamera), &vAt);

	// make it in local space
	i3Matrix::SetPos( pCamera, &vAt);

	// rotate around local right vector
	i3Vector::Copy( &vecRight, i3Matrix::GetRight( pCamera));
	i3Vector::Normalize( &vecRight, &vecRight);
	i3Matrix::PostRotateAxis( pCamera, (const VEC3D *) &vecRight, RotY);

	// rotate around world y axis
	i3Matrix::PostRotateAxis( pCamera, (const VEC3D *) &I3_YAXIS, RotX);

	// return to in world-space
	i3Vector::Add( &localPos, i3Matrix::GetPos( pCamera), &localPos);
	i3Matrix::SetPos( pCamera, &localPos);

	InvalidateRect( nullptr);
}

void i3LevelFramework::PivotRotate( REAL32 RotX, REAL32 RotY, REAL32 /*RotZ*/)
{
	// Camera matrix
    MATRIX * pCamera;
	VEC3D vecRight, localPos, vAt;
	REAL32 atLength = 0.0f;

	RotX *= 5.0f;
	RotY *= 5.0f;

	pCamera = m_pCurCamera->GetTransform();

	i3Matrix::Copy( pCamera, &m_Ctx.m_DragStartMatrix);
	
	atLength = i3Vector::Length( i3Matrix::GetPos( pCamera));

	i3Vector::Scale( &vAt, i3Matrix::GetAt( pCamera), atLength);//At Position
	i3Vector::Sub( &localPos, i3Matrix::GetPos( pCamera), &vAt);

	// make it in local space
	i3Matrix::SetPos( pCamera, &vAt);

	// rotate around local right vector
	i3Vector::Copy( &vecRight, i3Matrix::GetRight( pCamera));
	i3Vector::Normalize( &vecRight, &vecRight);
	i3Matrix::PostRotateAxis( pCamera, (const VEC3D *) &vecRight, RotY);

	// rotate around world y axis
	i3Matrix::PostRotateAxis( pCamera, (const VEC3D *) &I3_YAXIS, RotX);

	// return to in world-space
	i3Vector::Add( &localPos, i3Matrix::GetPos( pCamera), &localPos);
	i3Matrix::SetPos( pCamera, &localPos);

	InvalidateRect( nullptr);
}

void i3LevelFramework::PivotRotate( REAL32 RotX, REAL32 RotY, REAL32 /*RotZ*/)
{
    // Camera matrix
    MATRIX * pCamera;
	VEC3D vecRight, localPos;

	RotX *= 5.0f;
	RotY *= 5.0f;

	pCamera = m_pCurCamera->GetTransform();

	i3Matrix::Copy( pCamera, &m_Ctx.m_DragStartMatrix);

	i3Vector::Sub( &localPos, i3Matrix::GetPos( pCamera), GetBasePosition());

	// make it in local space
	i3Matrix::SetPos( pCamera, &localPos);

	// rotate around local right vector
	i3Vector::Copy( &vecRight, i3Matrix::GetRight( pCamera));
	i3Vector::Normalize( &vecRight, &vecRight);
	i3Matrix::PostRotateAxis( pCamera, (const VEC3D *) &vecRight, RotY);

	// rotate around world y axis
	i3Matrix::PostRotateAxis( pCamera, (const VEC3D *) &I3_YAXIS, RotX);

	// return to in world-space
	i3Vector::Add( &localPos, i3Matrix::GetPos( pCamera), GetBasePosition());
	i3Matrix::SetPos( pCamera, &localPos);

	InvalidateRect( nullptr);
}
#endif

void i3LevelFramework::Rotate( REAL32 RotX, REAL32 RotY, REAL32 /*RotZ*/)
{
	if( m_pCurCamera )
		m_pCurCamera->CameraRotate( m_Ctx, RotX, RotY );

	if( i3Level::GetNavMesh() )
		i3Level::GetNavMesh()->UpdateCameraMove();

	ResizeAxis();
	m_pViewport->InvalidateRect( nullptr);
}

void i3LevelFramework::Pan( REAL32 dx, REAL32 dy, REAL32 dz)
{
	if( m_pCurCamera )
		m_pCurCamera->CameraPan( m_Ctx, dx, dy, dz );

	if( i3Level::GetNavMesh() )
		i3Level::GetNavMesh()->UpdateCameraMove();

	ResizeAxis();
	m_pViewport->InvalidateRect( nullptr);
}

void i3LevelFramework::PendingUpdate(void)
{
	GetViewer()->SetPendingUpdateEnable( true );
	GetViewer()->Pause();
}

void i3LevelFramework::RestoreUpdate(void)
{
	GetViewer()->SetPendingUpdateEnable( false );
	GetViewer()->Play();
}


void i3LevelFramework::OnLButtonDown( UINT32 nFlags, POINT point)
{
	if( m_pCurCamera )
		m_pCurCamera->SaveDragStartMatrix( m_Ctx );

	m_Ctx.m_DragStartPt = point;
	GetPickedPos( point, &m_Ctx.m_DragStartPos);

	m_pViewport->SetFocus();
	m_pViewport->SetCapture();

	if( m_pCtrl != nullptr)
	{
		m_Ctx.m_bDragging = m_pCtrl->OnLButtonDown( &m_Ctx, nFlags, point);
	}
}

void i3LevelFramework::OnLButtonUp( UINT32 nFlags, POINT point)
{
	if( m_pCtrl )
		m_pCtrl->OnLButtonUp( &m_Ctx, nFlags, point );

	m_Ctx.m_bDragging = false;

	::ReleaseCapture();
}

void i3LevelFramework::OnMButtonDown( UINT32 nFlags, POINT point)
{
	m_pViewport->SetCapture();
	m_pViewport->SetFocus();

	if( m_pCurCamera )
		m_pCurCamera->SaveDragStartMatrix( m_Ctx );
	
	m_Ctx.m_DragStartPt = point;
	GetPickedPos( point, &m_Ctx.m_DragStartPos);

	if( GetKeyState( VK_LMENU) & 0x8000)
		m_CamEditMode = I3_VIEWER_CAM_PIVOT_ROTATE;
	else
		m_CamEditMode = I3_VIEWER_CAM_PAN;

	if( m_pCtrl != nullptr)
	{
		m_pCtrl->OnMButtonDown( &m_Ctx, nFlags, point);
	}
}

void i3LevelFramework::OnMButtonUp( UINT32 nFlags, POINT point)
{
	if( m_pCtrl != nullptr)
		m_pCtrl->OnMButtonDown( &m_Ctx, nFlags, point);

	::ReleaseCapture();

	m_CamEditMode = I3_VIEWER_CAM_NONE;
}

void i3LevelFramework::OnRButtonDown( UINT32 nFlags, POINT point)
{
	m_Ctx.m_bOpenPopup = true;

	if( m_pCurCamera )
		m_pCurCamera->SaveDragStartMatrix( m_Ctx );
	
	m_Ctx.m_DragStartPt = point;
	GetPickedPos( point, &m_Ctx.m_DragStartPos);

	if( m_pCtrl != nullptr)
		m_pCtrl->OnRButtonDown( &m_Ctx, nFlags, point);

	m_CamEditMode = I3_VIEWER_CAM_ROTATE;

	m_pViewport->SetFocus();
	m_pViewport->SetCapture();
}

void i3LevelFramework::OnRButtonUp( UINT32 nFlags, POINT point)
{
	bool bOpenPopup = true;

	if( m_pCtrl != nullptr)
		bOpenPopup = !m_pCtrl->OnRButtonUp( &m_Ctx, nFlags, point);

	::ReleaseCapture();

	if( m_Ctx.m_bOpenPopup && bOpenPopup )
	{
		PopupMenu();
	}

	m_CamEditMode = I3_VIEWER_CAM_NONE;
	::ReleaseCapture();
}

void i3LevelFramework::OnMouseMove( UINT32 nFlags, POINT point)
{
	REAL32 amount = 0.4f;
	REAL32 xDeg, yDeg;

	//InvalidateRect( nullptr );

	if( m_pCtrl != nullptr)
	{		
		m_pCtrl->OnMouseMove( &m_Ctx, nFlags, point);

		if( !m_Ctx.m_bDragging )
		{
			if( !CollisionGizmo( point ) )
			{
				i3Vector::Set( &m_Ctx.m_AxisMask, 0.f, 0.f, 0.f );
			}
		}

	}

	switch( m_CamEditMode)
	{
	case	I3_VIEWER_CAM_PIVOT_ROTATE :
			if( m_pCurCamera->IsCameraFlag( I3_VIEWER_CAM_DISABLE_PIVOT_ROTATE) == FALSE)
			{
				amount = 0.1f;
				if( nFlags & MK_SHIFT)
					amount = 0.8f;
				else if( nFlags & MK_ALT)
					amount = 0.05f;

				xDeg = (REAL32)(m_Ctx.m_DragStartPt.x - point.x) * amount;
				yDeg = (REAL32)(m_Ctx.m_DragStartPt.y - point.y) * amount;

				PivotRotate( I3_DEG2RAD(xDeg), I3_DEG2RAD(yDeg), 0.0f);

				m_pCurCamera->SaveDragStartMatrix( m_Ctx );
				m_Ctx.m_DragStartPt = point;
				GetPickedPos( point, &m_Ctx.m_DragStartPos);
			}
			break;

	case	I3_VIEWER_CAM_PAN :
			if( m_pCurCamera->IsCameraFlag( I3_VIEWER_CAM_DISABLE_PAN) == FALSE)
			{
				VEC3D vec1;
				VEC3D end1;
				VEC3D start1;
				REAL32 dist;

				i3Vector::Sub( &vec1, i3Matrix::GetPos( m_pCurCamera->GetTransform()), GetBasePosition());

				dist = i3Vector::Length( &vec1);
				if( dist <= I3_EPS)
					dist = 1.0f;

				ScreenToWorld( point.x, point.y, &end1, dist);
				ScreenToWorld( m_Ctx.m_DragStartPt.x, m_Ctx.m_DragStartPt.y, &start1, dist);

				i3Vector::Sub( &vec1, &end1, &start1);

				Pan( getX(&vec1), getY(&vec1), getZ(&vec1));

				m_pCurCamera->SaveDragStartMatrix( m_Ctx );
				m_Ctx.m_DragStartPt = point;
				GetPickedPos( point, &m_Ctx.m_DragStartPos);
			}
			break;

	case	I3_VIEWER_CAM_ROTATE:
			if( m_pCurCamera->IsCameraFlag( I3_VIEWER_CAM_DISABLE_ROTATE ) == FALSE )
			{
				amount = 0.1f;
				
				if( nFlags & MK_SHIFT)
					amount = 0.8f;
				else if( nFlags & MK_ALT)
					amount = 0.05f;

				xDeg = (REAL32)(m_Ctx.m_DragStartPt.x - point.x) * amount;
				yDeg = (REAL32)(m_Ctx.m_DragStartPt.y - point.y) * amount;

				Rotate( I3_DEG2RAD(xDeg), I3_DEG2RAD(yDeg), 0 );

				m_Ctx.m_bOpenPopup = false;

				m_pCurCamera->SaveDragStartMatrix( m_Ctx );
				m_Ctx.m_DragStartPt = point;
				GetPickedPos( point, &m_Ctx.m_DragStartPos);
			}
			break;

		default:
			break;
	}
}

void i3LevelFramework::OnMouseWheel(UINT nFlags, short zDelta, POINT pt)
{
	if( m_pCurCamera )
	{
		float fZoom = 0.0f;		
		
		if( GetAsyncKeyState( VK_LSHIFT ) )
		{
			fZoom = ((0 < zDelta) ? 0.5f : -0.5f );
		}
		else
		{
			fZoom = ((0 < zDelta) ? 1.0f : -1.0f );		
			fZoom *= m_CameraZoomScale;
		}

		m_pCurCamera->CameraZoom( m_Ctx, fZoom );	
	}

	if( m_pCurCamera )
		m_pCurCamera->SaveDragStartMatrix( m_Ctx );

	if( i3Level::GetNavMesh() )
		i3Level::GetNavMesh()->UpdateCameraMove();

	ResizeAxis();

	if( m_pCtrl != nullptr)
	{
		m_pCtrl->OnMouseWheel( &m_Ctx, nFlags, zDelta, pt);
	}

	m_pViewport->Invalidate();
}

void i3LevelFramework::OnKeyDown(UINT32 nChar)
{
	switch( nChar)
	{
		case 'Z' :
			if( (GetKeyState( VK_LCONTROL) & 0x80) == 0)
			{
				Fit();
			}
			break;

		case 'T' :		SetCameraMode( CAMERA_MODE_TOP );	break;
		case 'P' :		SetCameraMode( CAMERA_MODE_FREE );	break;
		case 'U' :		SetCameraMode( CAMERA_MODE_WALK );	break;
		case 'F' :		SetCameraMode( CAMERA_MODE_FRONT );	break;

		case 'Q' :		SetLevelCtrlMode( LCM_SELECT );			break;
		case 'W' :		SetLevelCtrlMode( LCM_MOVE );			break;
		case 'E' :		SetLevelCtrlMode( LCM_ROTATE);			break;
		case 'R' :		SetLevelCtrlMode( LCM_SCALE);			break;
		case 'A' :		SetLevelCtrlMode( LCM_PATHEDIT);			break;
		//case 'S' :		SetEditMode( CTRL_MODE_RESPAWN_CREATE );	break;
		case 'B' :
			if( LEM_NORMAL == GetLevelEditMode() )
			{
				if( LCM_SCALE == GetLevelCtrlMode() )
				{
					SetLevelCtrlMode( LCM_SELECT );
				}
				else
				{
					SetLevelCtrlMode( LCM_OBJBRUSH );
				}
			}			
			break;

		case	VK_DELETE :
				{
					switch( m_LevelEditMode )
					{
					case	LEM_NORMAL:
							g_pScene->RemoveSelectedElement();
							break;

					case	LEM_NAVMESH_POINT:
							{
								if( i3Level::GetNavMesh() )
									i3Level::GetNavMesh()->RemoveSelectedVertex();
							}
							break;

					case	LEM_NAVMESH_POLYGON:
							{
								if( i3Level::GetNavMesh() )
									i3Level::GetNavMesh()->RemoveSelectedPolygon();
							}
							break;
					}
				}
				break;

		case	VK_ESCAPE :
				{
					switch( m_LevelEditMode )
					{
					case	LEM_NORMAL:
							g_pScene->UnselectAll();
							break;

					case	LEM_NAVMESH_POINT:
					case	LEM_NAVMESH_POLYGON:
							{
								if( i3Level::GetNavMesh() )
								{
									i3Level::GetNavMesh()->ClearSelectList();
									OnNavSelect();
								}
							}
							break;

					case	LEM_NAVMESH_POINT_ADD:
							SetLevelEditMode( LEM_NAVMESH_POINT );
							break;

					case	LEM_NAVMESH_POLYGON_CUT:
							SetLevelEditMode( m_prevLevelEditMode );
							break;
					}
				}			
				break;

		case VK_F6 :
			SetCollisionVisible( TRUE);
			break;

		case VK_F7 :
			SetCollisionVisible( FALSE);
			break;

		case	VK_LEFT:		m_CamMoveKey |= I3_VIEWER_KEY_LEFT;		break;
		case	VK_RIGHT:		m_CamMoveKey |= I3_VIEWER_KEY_RIGHT;	break;
		case	VK_UP:			m_CamMoveKey |= I3_VIEWER_KEY_FRONT;	break;
		case	VK_DOWN:		m_CamMoveKey |= I3_VIEWER_KEY_BACK;		break;
		case	VK_LSHIFT:		m_KeyBoardKey |= I3_VIEWER_KEY_SHIFT;	break;
	}

	if( (GetKeyState( VK_LCONTROL ) & 0x80) != 0 &&
		(GetKeyState( VK_LBUTTON ) & 0x80) == 0 )
	{
		switch( nChar)
		{
			case 'Z' :		i3LevelUndo::Undo();	break;
			case 'Y':		i3LevelUndo::Redo();	break;
		}
	}

	if( m_pCtrl != nullptr)
		m_pCtrl->OnKeyDown( &m_Ctx, nChar, 0, 0);

}

void	i3LevelFramework::MoveCamera( VEC3D Pos)
{

}

void i3LevelFramework::OnKeyUp(UINT32 nChar)
{
	switch( nChar )
	{
	case	VK_LEFT:		m_CamMoveKey &= ~I3_VIEWER_KEY_LEFT;	break;
	case	VK_RIGHT:		m_CamMoveKey &= ~I3_VIEWER_KEY_RIGHT;	break;
	case	VK_UP:			m_CamMoveKey &= ~I3_VIEWER_KEY_FRONT;	break;
	case	VK_DOWN:		m_CamMoveKey &= ~I3_VIEWER_KEY_BACK;	break;
	case	VK_LSHIFT:		m_KeyBoardKey &= ~I3_VIEWER_KEY_SHIFT;	break;
	}


	if( m_pCtrl != nullptr)
		m_pCtrl->OnKeyUp( &m_Ctx, nChar, 0, 0);

	if( (GetKeyState( VK_LCONTROL) & 0x80) != 0)
	{
		/*switch( nChar)
		{
			case 'Z' :		i3LevelUndo::Undo();	break;
			case 'Y':		i3LevelUndo::Redo();	break;
		}*/
	}
}

void i3LevelFramework::OnSaveInfo( i3::pack::Level_VIEWPORT * pInfo)
{
	pInfo->m_CtrlMode = (UINT8) _GetCtrlMode();

	if( m_pCurCamera == m_pTopCamera)
		pInfo->m_CurrentCamera = 1;
	else if( m_pCurCamera == m_pWalkCamera)
		pInfo->m_CurrentCamera = 2;
	else
		pInfo->m_CurrentCamera = 0;

	pInfo->m_bPlay = m_bPlay;
	pInfo->m_TimeScale = m_TimeScale;

	pInfo->m_AxisMask.x = i3Vector::GetX( &m_Ctx.m_AxisMask);
	pInfo->m_AxisMask.y = i3Vector::GetY( &m_Ctx.m_AxisMask);
	pInfo->m_AxisMask.z = i3Vector::GetZ( &m_Ctx.m_AxisMask);

	// Free Camera
	{
		VEC3D * pVec = m_pFreeCamera->getPos();

		pInfo->m_FreeCamPos.x = i3Vector::GetX( pVec);
		pInfo->m_FreeCamPos.y = i3Vector::GetY( pVec);
		pInfo->m_FreeCamPos.z = i3Vector::GetZ( pVec);
	}

	{
		pInfo->m_FreeCamRotate.x = 0.0f;
		pInfo->m_FreeCamRotate.y = 0.0f;
		pInfo->m_FreeCamRotate.z = 0.0f;
	}

	{
		pInfo->m_FreeCamScale.x = 1.0f;
		pInfo->m_FreeCamScale.y = 1.0f;
		pInfo->m_FreeCamScale.z = 1.0f;
	}

	// Top Camera
	{
		VEC3D * pVec = m_pTopCamera->getPos();

		pInfo->m_TopCamPos.x = i3Vector::GetX( pVec);
		pInfo->m_TopCamPos.y = i3Vector::GetY( pVec);
		pInfo->m_TopCamPos.z = i3Vector::GetZ( pVec);
	}

	{
		pInfo->m_TopCamRotate.x = 0.0f;
		pInfo->m_TopCamRotate.y = 0.0f;
		pInfo->m_TopCamRotate.z = 0.0f;
	}

	{
		pInfo->m_TopCamScale.x = 1.0f;
		pInfo->m_TopCamScale.y = 1.0f;
		pInfo->m_TopCamScale.z = 1.0f;
	}

	// Walk Camera
	{
		VEC3D * pVec = m_pWalkCamera->getPos();

		pInfo->m_WalkCamPos.x = i3Vector::GetX( pVec);
		pInfo->m_WalkCamPos.y = i3Vector::GetY( pVec);
		pInfo->m_WalkCamPos.z = i3Vector::GetZ( pVec);
	}

	{
		pInfo->m_WalkCamRotate.x = 0.0f;
		pInfo->m_WalkCamRotate.y = 0.0f;
		pInfo->m_WalkCamRotate.z = 0.0f;
	}

	{
		pInfo->m_WalkCamScale.x = 1.0f;
		pInfo->m_WalkCamScale.y = 1.0f;
		pInfo->m_WalkCamScale.z = 1.0f;
	}
}

void i3LevelFramework::OnLoadInfo( i3::pack::Level_VIEWPORT * pInfo)
{
	_SetCtrlMode( (CONTROL_MODE_DETAIL) pInfo->m_CtrlMode );

	switch( pInfo->m_CurrentCamera)
	{
		case 1 :		SetCurrentCamera( m_pTopCamera);	break;
		case 2 :		SetCurrentCamera( m_pWalkCamera);	break;
		default :		SetCurrentCamera( m_pFreeCamera);	break;
	}

	if( pInfo->m_bPlay)
		Play();
	else
		Pause();

	SetTimeScale( pInfo->m_TimeScale);

	i3Vector::Set( &m_Ctx.m_AxisMask, pInfo->m_AxisMask.x, pInfo->m_AxisMask.y, pInfo->m_AxisMask.z);

	// Free Camera
	{
		VEC3D vec;

		i3Vector::Set( &vec, pInfo->m_FreeCamPos.x, pInfo->m_FreeCamPos.y, pInfo->m_FreeCamPos.z);
		m_pFreeCamera->setPos( &vec);
	}

	// Top Camera
	{
		VEC3D vec;

		i3Vector::Set( &vec, pInfo->m_TopCamPos.x, pInfo->m_TopCamPos.y, pInfo->m_TopCamPos.z);
		m_pTopCamera->setPos( &vec);

	}

	// Walk Camera
	{
		VEC3D vec;

		i3Vector::Set( &vec, pInfo->m_WalkCamPos.x, pInfo->m_WalkCamPos.y, pInfo->m_WalkCamPos.z);
		m_pWalkCamera->setPos( &vec);

	}
}

void i3LevelFramework::SelectTarget( HWND hwndReport)
{
	i3ViewerFramework::SelectTarget( hwndReport);

	/*m_OldCtrlMode = GetEditMode();
	SetEditMode( CTRL_MODE_SELECT_TARGET);*/

	m_OldCtrlMode = _GetCtrlMode();
	_SetCtrlMode( CTRL_MODE_SELECT_TARGET );

	// 현재 선택된 Element들의 Target들을 표시한다.
	{
		i3::vector<i3LevelElement3D*> SelList;
		i3LevelElement3D * pTemp, * pTarget;
		INT32 j;

		m_pScene->GetSelectedElements( SelList);

		for( size_t i = 0; i < SelList.size(); i++)
		{
			pTemp = SelList[i];

			for( j = 0; j < pTemp->getTargetCount(); j++)
			{
				pTarget = pTemp->getTarget( j);

				pTarget->Targeted();
			}
		}
	}
}

void i3LevelFramework::UnselectAllTarget(void)
{
	i3ViewerFramework::UnselectAllTarget();
}

void i3LevelFramework::EndSelectTarget(void)
{
	//SetEditMode( m_OldCtrlMode);
	_SetCtrlMode( m_OldCtrlMode );

	
	i3LevelElement3D * pSel;
	i3::vector<i3LevelElement3D*> list;
	i3::vector<i3LevelElement3D*>	selList;

	m_pScene->GetTargetedElements( list);
	m_pScene->GetSelectedElements( selList);

	// 현재 선택된 Target들은 모두 i3LevelElement3D Class들이므로
	// 실질적인 i3GameObj를 TargetList로 구성한다.
	for(size_t j = 0; j < selList.size(); j++)
	{
		pSel = selList[j];

		pSel->removeAllTarget();

		for( size_t i = 0; i < list.size(); i++)
		{
			i3LevelElement3D * pElm = list[i];

			pElm->Untargeted();

			pSel->addTarget( pElm);
		}

		i3TDK::Update( nullptr, I3_TDK_UPDATE_EDIT, pSel);
	}

	UnselectAllTarget();
}

void i3LevelFramework::SetGroup(void)
{
	i3TDKDlgRename dlg;

	if( dlg.Execute( _T("새로운 Group에 대한 이름을 설정해 주세요."), _T("Group") ) == FALSE)
		return;

#ifdef _UNICODE
	i3::rc_string groupName;	i3::utf16_to_mb(dlg.getNameString(), groupName);
#else
	const i3::rc_string& groupName = dlg.getNameString();
#endif
	m_pScene->SetGroupToSelectedElements( groupName );
}

void i3LevelFramework::ReleaseGroup(void)
{
	m_pScene->ReleaseGroupFromSelectedElements();
}

void i3LevelFramework::ReassignGeneratedResName(void)
{
	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < m_pScene->GetElementCount(); i++)
	{
		pElm = m_pScene->GetElement( i);

		if( !i3::kind_of<i3LevelPrimitive*>(pElm))
			continue;

		i3LevelPrimitive * pPrimElm = (i3LevelPrimitive *) pElm;

		pPrimElm->AdjustResPathByName();
	}
}

bool	i3LevelFramework::SetLevelEditMode( LEVEL_EDIT_MODE _mode )
{
	if( _mode < 0 || MAX_LEVEL_EDIT_MODE <= _mode )
	{
		return false;
	}

	if( i3Level::GetNavMesh() )
		i3Level::GetNavMesh()->ClearSelectList();

	LEVEL_EDIT_MODE _prev = m_LevelEditMode;

	m_LevelEditMode		= _mode;
	m_prevLevelEditMode = _prev;

	SetLevelCtrlMode( LCM_SELECT );

	{
		if( m_pFnEditModeChanged )
			(*m_pFnEditModeChanged)( _mode );
	}

	return true;
}

bool	i3LevelFramework::SetLevelCtrlMode( LEVEL_CONTROL_MODE _mode )
{
	if( _mode < 0 || MAX_LEVEL_CONTROL_MODE <= _mode )
	{
		return false;
	}

	bool bRes = false;

	switch( m_LevelEditMode )
	{
	case	LEM_NORMAL:
			bRes = _SetCtrlMode( (CONTROL_MODE_DETAIL)( _mode +1 ) );
			break;

	case	LEM_NAVMESH_POINT:
			{
				switch( _mode )
				{
				case	LCM_SELECT:
						bRes = _SetCtrlMode( CTRL_MODE_NAVMESH_POINT_SELECT );
						break;

				case	LCM_MOVE:
						bRes = _SetCtrlMode( CTRL_MODE_NAVMESH_POINT_MOVE );
						break;
				}
			}
			break;

	case	LEM_NAVMESH_POLYGON:
			{
				switch( _mode )
				{
				case	LCM_SELECT:
						bRes = _SetCtrlMode( CTRL_MODE_NAVMESH_POLYGON_SELECT );
						break;

				case	LCM_MOVE:
						bRes = _SetCtrlMode( CTRL_MODE_NAVMESH_POLYGON_MOVE );
						break;
				}
			}
			break;

	case	LEM_NAVMESH_POINT_ADD:
			{
				if( LCM_SELECT == _mode )
					bRes = _SetCtrlMode( CTRL_MODE_NAVMESH_POINT_ADD );
			}
			break;

	case	LEM_NAVMESH_POLYGON_CUT:
			{
				if( LCM_SELECT == _mode )
					bRes = _SetCtrlMode( CTRL_MODE_NAVMESH_POLYGON_CUT );
			}
			break;
	}

	if( bRes )
		m_LevelCtrlMode = _mode;

	if( m_pViewer )
		::SetFocus( m_pViewer->GetWindowHandle() );
	return bRes;
}

bool	i3LevelFramework::_SetCtrlMode( CONTROL_MODE_DETAIL _mode )
{
	VEC3D oldAxis, oldBoundMin, oldBoundMax;

	if( _mode < 0 || CTRL_MODE_MAX <= _mode )
	{
		return false;
	}

	i3LevelViewportControl* pNewCtrl = m_pCtrlTable[ _mode ];

	if( nullptr == pNewCtrl )
	{
		return false;
	}

	if( false == pNewCtrl->OnStart( &m_Ctx) )
	{			
		return false;	// 변경하지 않는다.
	}

	if( nullptr != m_pCtrl )
		m_pCtrl->OnEnd( &m_Ctx);

	m_pCtrl		= pNewCtrl;
	m_CtrlMode	= _mode;
	
	if( nullptr != m_pCtrl->getCursor() )	
	{
		SetMouseCursor( m_pCtrl->getCursor());
	}
	else
	{
		SetMouseCursor( i3LevelViewportControl::GetDefaultCursor());
	}

	// axis
	{
		i3LevelElement3D * pObj = nullptr;

		if( (nullptr != m_pCurAxis ) )
		{
			i3Vector::Copy(&oldAxis, m_pCurAxis->getPos());
			I3ASSERT( i3Vector::isValid( &oldAxis));

			oldBoundMin = m_pCurAxis->getBoundMin();
			oldBoundMax = m_pCurAxis->getBoundMax();

			// 현재의 Target Object를 새 Axis에 넘겨 주어야 한다.
			pObj = m_pCurAxis->getTargetObject();
			I3_SAFE_ADDREF( pObj);

			m_pCurAxis->setTargetObject( nullptr);

			m_pCurAxis->Detach();
		}
		else
		{
			oldAxis = oldBoundMin = oldBoundMax = VEC3D( 0.0f, 0.0f, 0.0f);
		}

		if( nullptr != m_pCtrl )
			m_pCurAxis = m_pCtrl->getAxis();
		else
			m_pCurAxis = nullptr;

		if( nullptr == m_pCurAxis )
			m_pCurAxis = i3LevelViewportControl::GetDefaultAxis();

		m_pCurAxis->Attach( m_pAxisRoot );

		m_pCurAxis->setTargetObject( pObj);			// Target Object 이전
		m_pCurAxis->setPos(&oldAxis);
		m_pCurAxis->setBound( oldBoundMin, oldBoundMax);
		
		ResizeAxis();

		I3_SAFE_RELEASE( pObj);
	}

	OnSelect( nullptr );

	if( m_pViewport != nullptr)
		m_pViewport->Invalidate();

	return true;
}

void	i3LevelFramework::_OnUpdate_Camera( REAL32 tm )
{
	if( nullptr == m_pCurCamera )
	{
		return;
	}
	
	float _delta	= 20.0f * tm;		
	bool isMove		= false;

	VEC3D vMove;
	i3Vector::Set( &vMove, 0,0,0 );

	//if( I3_VIEWER_KEY_SHIFT & m_KeyBoardKey )	
	if( GetAsyncKeyState( VK_LSHIFT ) )
	{
		_delta *= 3.0f;
	}

	if( I3_VIEWER_KEY_FRONT & m_CamMoveKey )		
	{
		vMove.z -= _delta;
		isMove = true;
	}

	if( I3_VIEWER_KEY_BACK & m_CamMoveKey )
	{
		vMove.z += _delta;
		isMove = true;
	}

	if( I3_VIEWER_KEY_LEFT & m_CamMoveKey )
	{
		vMove.x -= _delta;
		isMove = true;
	}

	if( I3_VIEWER_KEY_RIGHT & m_CamMoveKey )
	{
		vMove.x += _delta;
		isMove = true;
	}

	if( isMove )
	{
		if( FALSE == m_pCurCamera->IsCameraFlag( I3_VIEWER_CAM_DISABLE_PIVOT_ROTATE ) )
		{
			m_pCurCamera->CameraMove( m_Ctx, vMove.x, vMove.y, vMove.z );
			m_pCurCamera->SaveDragStartMatrix( m_Ctx );

			if( i3Level::GetNavMesh() )
				i3Level::GetNavMesh()->UpdateCameraMove();
		}
	}
}

void i3LevelFramework::OnActivate( bool bActivate )
{
	i3ViewerFramework::OnActivate( bActivate );

	if( false == bActivate )
	{
		m_CamMoveKey = 0;
		m_KeyBoardKey = 0;
	}
}

void i3LevelFramework::OnKillFocus()
{
	i3ViewerFramework::OnKillFocus();

	m_CamMoveKey = 0;
	m_KeyBoardKey = 0;
}


void i3LevelFramework::setSpaceMode( I3LV_SPACE::TYPE type)
{
	m_SpaceMode = type;

	ResizeAxis();
	m_pViewport->InvalidateRect( nullptr);
}
