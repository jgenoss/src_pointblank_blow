#include "stdafx.h"
#include "ClothFramework.h"
#include "GlobalVariable.h"
#include "SelectSpringDialog.h"
#include "Resource.h"

I3_CLASS_INSTANCE( CClothFramework, i3Framework);

static COLOR	s_SelColor = { 255, 128, 128, 255 };
static COLOR	s_UnSelColor = { 128, 255, 128, 255 };
static COLOR	s_NormalSelColor = { 255, 0, 0, 255 };
static COLOR	s_NormalUnSelColor = { 0, 0, 255, 255 };
static COLOR	s_LockColor = { 128, 128, 128, 255 };

CClothFramework::CClothFramework(void)
{
	m_pVertexRoot = i3AttrSet::NewObject();
	m_pSpringRoot = i3AttrSet::NewObject();

	/*
	{
		i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::NewObjectRef();
		pAttr->Set( TRUE);
		m_pVertexRoot->AddAttr( pAttr);
	}
	*/

	m_pCloth = i3ClothObject::NewObject();

	g_pFramework = this;
	m_bSelectDrag = false;

	m_bFreeze = false;
	m_bSimulation = false;

	m_pTargetInfo = NULL;
	m_pTimer = NULL;

	m_SelectMask = SELMASK_PARTICLE | SELMASK_SPRING;
}

CClothFramework::~CClothFramework(void)
{
	Reset();

	I3_SAFE_RELEASE( m_pCloth);
	I3_SAFE_RELEASE( m_pVertexRoot);
	I3_SAFE_RELEASE( m_pSpringRoot);
	I3_SAFE_RELEASE( m_pColorAttr);

	I3_SAFE_RELEASE( m_pTimer );
}

void CClothFramework::Reset(void)
{
	_RemoveAllCollideeMesh();

	m_pVertexRoot->RemoveAllChild();

	m_pSpringRoot->RemoveAllChild();
}

void CClothFramework::OnCreate(void)
{
	i3ViewerFramework::OnCreate();

	m_pTimer = i3Timer::NewObject();

	Get2DRoot()->AddChild( m_pVertexRoot);
	Get3DRoot()->AddChild( m_pSpringRoot);

	{
		m_pOverrideRoot = i3OverrideAttrSet::NewObjectRef();
		/*
		i3Node * pParent;

		pParent = Get3DRoot()->GetParent();

		pParent->RemoveChild( Get3DRoot());
		pParent->InsertChild( 0, m_pOverrideRoot);
		m_pOverrideRoot->AddChild( Get3DRoot());
		*/

		Get3DRoot()->AddChild( m_pOverrideRoot);

		// Lighting Enable
		{
			m_pLightingEnableAttr = i3LightingEnableAttr::NewObjectRef();

			m_pLightingEnableAttr->Set( FALSE);

			m_pOverrideRoot->AddAttr( m_pLightingEnableAttr);
		}

		// Texture Mapping
		{
			m_pTextureEnableAttr = i3TextureEnableAttr::NewObjectRef();
			m_pTextureEnableAttr->Set( TRUE);
			m_pOverrideRoot->AddAttr( m_pTextureEnableAttr);
		}

		// Render Mode
		{
			m_pRenderModeAttr = i3RenderModeAttr::NewObjectRef();

			m_pRenderModeAttr->Set( I3G_RENDER_POLYGON);
			m_pOverrideRoot->AddAttr( m_pRenderModeAttr);
		}

		// Face-Cull Mode
		{
			m_pFaceCullModeAttr = i3FaceCullModeAttr::NewObjectRef();

			m_pFaceCullModeAttr->Set( I3G_FACE_CULL_NONE);
			m_pOverrideRoot->AddAttr( m_pFaceCullModeAttr);
		}

		// Color Attr
		{
			COLOR col;

			m_pColorAttr = i3ColorAttr::NewObject();

			i3Color::Set( &col, 48, 48, 48, 255);
			m_pColorAttr->Set( &col);
		}
		
	}
}

void CClothFramework::_RemoveAllCollideeMesh(void)
{
	INT32 i;
	COLL_MESH_INFO * pInfo;
	
	for( i = 0; i < m_CollMeshList.GetCount(); i++)
	{
		pInfo = (COLL_MESH_INFO *) m_CollMeshList.Items[i];

		I3ASSERT( pInfo != NULL);
		I3ASSERT( pInfo->m_pMesh != NULL);

		pInfo->m_pMesh->Release();
		m_pVertexRoot->RemoveChild( pInfo->m_pSprite);
		pInfo->m_pSprite->Release();
		delete pInfo->m_pVertexState;
		delete pInfo->m_ppParticle;
		delete pInfo->m_pVtxIndexTable;

		if( pInfo->m_pSpringState != NULL)
		{
			delete pInfo->m_pSpringState;
		}

		delete pInfo;
	}

	m_CollMeshList.Clear();
}

static BOOL _BuildCollMeshProc( i3Node * pNode, i3List * pList, i3MatrixStack * pStack)
{
	INT32 i;

	if( pNode->IsTypeOf( i3Geometry::GetClassMeta()))
	{
		i3Geometry * pGeo = (i3Geometry *) pNode;
		i3RenderAttr * pAttr;
		i3OptBuildCollisionMesh opt;

		for( i = 0; i < pGeo->GetGeometryAttrCount(); i++)
		{
			pAttr = (i3RenderAttr *) pGeo->GetGeometryAttr( i);

			if( pAttr->IsTypeOf( i3GeometryAttr::GetClassMeta()))
			{
				COLL_MESH_INFO * pInfo = new COLL_MESH_INFO;

				i3Matrix::Copy( &pInfo->m_Matrix, pStack->GetTop());
				pInfo->m_pGeometry = pGeo;
				pInfo->m_pGeoAttr = (i3GeometryAttr *) pAttr;

				pInfo->m_pVtxIndexTable = new INT32[ pInfo->m_pGeoAttr->GetVertexArray()->GetCount()];

				pInfo->m_pMesh = opt.ConvertTo( (i3GeometryAttr *) pAttr, pInfo->m_pVtxIndexTable);
				pInfo->m_pMesh->AddRef();

				pInfo->m_pSprite = i3Sprite2D::NewObject();
				pInfo->m_pSprite->Create( pInfo->m_pMesh->GetPositionCount(), FALSE, TRUE);

				pInfo->m_pVertexState = new UINT32[ pInfo->m_pMesh->GetPositionCount()];
				pInfo->m_pSpringState = NULL;

				pInfo->m_ppParticle = new i3PhysicsParticle *[ pInfo->m_pMesh->GetPositionCount()];
				
				// 초기화
				memset( pInfo->m_ppParticle, 0, sizeof(void *) * pInfo->m_pMesh->GetPositionCount());

				// Vertex State 초기화
				// Vertex 개수만큼 생성해야 한다.
				memset( pInfo->m_pVertexState, 0, sizeof(UINT32) * pInfo->m_pMesh->GetPositionCount());

				pList->Add( pInfo);
			}
		}
	}

	return TRUE;
}

void CClothFramework::_BuildCollisionMesh(void)
{
	INT32 i;
	i3OptBuildCollisionMesh opt;
	i3MatrixStack Stack;

	if( GetLoadedScene() == NULL)
		return;

	// 모든 GeometryAttr을 구한다.
	i3Scene::Traverse( GetLoadedScene(), (I3_SG_TRAVERSE_PROC) _BuildCollMeshProc, &m_CollMeshList, &Stack);

	// i3CollideeMesh Object를 생성한다.
	// i3CollideeMesh가 가지고 있는 Vertex, Triangle 정보를 바탕으로
	// Cloth Sim.용 데이터를 생성한다.
	// 이것은 Vertex가 중복되어 존재할 수 있다는 점, 그리고 Triangle List, Triangle Strip 등의
	// 다양한 형식의 Primitive Type 모두에 적절하게 동작하도록
	// 보장하기 위한 방법이다.
	for( i = 0; i < m_CollMeshList.GetCount(); i++)
	{
		COLL_MESH_INFO * pInfo = (COLL_MESH_INFO *) m_CollMeshList.Items[i];

		m_pVertexRoot->AddChild( pInfo->m_pSprite);
	}
}

void CClothFramework::OnChangeScene(void)
{
	i3ViewerFramework::OnChangeScene();

	Reset();
	
	_BuildCollisionMesh();
}

void CClothFramework::_UpdateVertexPos(void)
{
	INT32 i, j;
	COLL_MESH_INFO * pInfo;
	MATRIX FullMtx, ViewProjMtx, VMtx;
	i3Camera * pCam = GetCurrentCamera();
	i3CollideeMesh * pMesh;
	i3Sprite2D * pSprite;
	VEC3D pos;
	REAL32 hcx, hcy, x, y, z;

	START_PROFILE( 10);

	i3Matrix::Mul( &ViewProjMtx, pCam->GetViewMatrixAttr()->GetMatrix(), pCam->GetProjectionMatrix());

	{
		hcx = GetViewer()->GetViewWidth() * 0.5f;
		hcy = GetViewer()->GetViewHeight() * 0.5f;
	}

	for( i = 0; i < m_CollMeshList.GetCount(); i++)
	{
		pInfo = (COLL_MESH_INFO *) m_CollMeshList.Items[i];
		pMesh = pInfo->m_pMesh;
		pSprite = pInfo->m_pSprite;

		i3Matrix::Mul( &FullMtx, &pInfo->m_Matrix, &ViewProjMtx);
		i3Matrix::Mul( &VMtx, &pInfo->m_Matrix, pCam->GetViewMatrixAttr()->GetMatrix());

		// Triangle 정보를 바탕으로 Culling 처리한다.
		for( j = 0; j < (INT32) pMesh->GetTriangleCount(); j++)
		{
			I3_COLLIDEE_TRI * pTri = pMesh->GetTriangle( j);
			VEC3D norm;
			REAL32 dot;

			i3Vector::TransformNormal( &norm, pMesh->GetNormal( pTri->m_NormalIndex), &VMtx);
			//i3Vector::Normalize( &norm, &norm);

			dot = i3Vector::Dot( &norm, &I3_ZAXIS);

			if( dot > 0.0f)
			{
				pInfo->m_pVertexState[ pTri->m_Index[0]] &= ~VSTATE_CULL;
				pInfo->m_pVertexState[ pTri->m_Index[1]] &= ~VSTATE_CULL;
				pInfo->m_pVertexState[ pTri->m_Index[2]] &= ~VSTATE_CULL;
			}
			else
			{
				pInfo->m_pVertexState[ pTri->m_Index[0]] |= VSTATE_CULL;
				pInfo->m_pVertexState[ pTri->m_Index[1]] |= VSTATE_CULL;
				pInfo->m_pVertexState[ pTri->m_Index[2]] |= VSTATE_CULL;
			}
		}

		for( j = 0; j < (INT32) pMesh->GetPositionCount(); j++)
		{
			#if 1
			if( pInfo->m_pVertexState[j] & VSTATE_CULL)
			{
				pSprite->SetEnable( j, FALSE);
				continue;
			}
			#endif

			if( pInfo->m_ppParticle[j] != NULL)
				i3Vector::TransformCoord( &pos, pInfo->m_ppParticle[j]->getPos(), &FullMtx );
			else
				i3Vector::TransformCoord( &pos, pMesh->GetPosition( j), &FullMtx);

			x = (i3Vector::GetX( &pos) * hcx) + hcx;
			y = (i3Vector::GetY( &pos) * -hcy) + hcy;
			z = (i3Vector::GetZ( &pos));
			
			if( (z <= 0.0f) || (z > 1.0f))
			{
				pSprite->SetEnable( j, FALSE);
			}
			else
			{
				if( pInfo->m_ppParticle[j] != NULL)
				{
					if( pInfo->m_pVertexState[j] & VSTATE_SELECTED)
						pSprite->SetColor( j, &s_SelColor);
					else
					{
						if( pInfo->m_ppParticle[j]->getStyle() & I3_PHYSICS_PARAM_LOCKED )
						{
							// Locked된 Particle
							pSprite->SetColor( j, &s_LockColor);
						}
						else
						{
							pSprite->SetColor( j, &s_UnSelColor);
						}
					}
				}
				else
				{
					if( pInfo->m_pVertexState[j] & VSTATE_SELECTED)
						pSprite->SetColor( j, &s_NormalSelColor);
					else
						pSprite->SetColor( j, &s_NormalUnSelColor);
				}

				pSprite->SetEnable( j, TRUE);
				pSprite->SetCenter( j, x, y, z);
				pSprite->SetSize( j, 3.0f, 3.0f);
			}
		}
	}

	END_PROFILE( 10);
}

void CClothFramework::OnRender(void)
{	
	START_PROFILE( 4);
	if( m_bSimulation )
		m_pCloth->StepSimulation( m_pTimer->GetDeltaSec()  );
	END_PROFILE( 4);
	
	/*{
		INT32 i;
		MATRIX	mat, mat1, mat2;
		i3Matrix::Identity( &mat );
		i3Matrix::Identity( &mat1 );
		i3Matrix::Identity( &mat2 );
		START_PROFILE( 5);
		for( i = 0; i < 1000000; i++ )
		{
			i3Matrix::Mul( &mat, &mat1, &mat2);
		}
		END_PROFILE( 5);
	}*/
	
	
	if( m_pVertexRoot->IsFlag( I3_NODEFLAG_DISABLE) == false)
		_UpdateVertexPos();
}

void CClothFramework::_DrawDragStart( POINT pt)
{
	HDC dc;
	HWND hwnd = GetViewer()->GetWindowHandle();

	dc = ::GetDC( hwnd);

	m_DragStartPt = pt;

	m_OldDragRect.SetRect( m_DragStartPt, m_DragStartPt);

	::DrawFocusRect( dc, &m_OldDragRect);

	::ReleaseDC( hwnd, dc);
}

void CClothFramework::_DrawDrag( POINT pt)
{
	HDC dc;
	HWND hwnd = GetViewer()->GetWindowHandle();

	dc = ::GetDC( hwnd);

	::DrawFocusRect( dc, &m_OldDragRect);

	m_OldDragRect.left = MIN( m_DragStartPt.x, pt.x);
	m_OldDragRect.right = MAX( m_DragStartPt.x, pt.x);
	m_OldDragRect.top = MIN( m_DragStartPt.y, pt.y);
	m_OldDragRect.bottom = MAX( m_DragStartPt.y, pt.y);

	::DrawFocusRect( dc, &m_OldDragRect);

	::ReleaseDC( hwnd, dc);
}

void CClothFramework::_DrawDragEnd( POINT pt)
{
	HDC dc;
	HWND hwnd = GetViewer()->GetWindowHandle();

	dc = ::GetDC( hwnd);

	::DrawFocusRect( dc, &m_OldDragRect);

	::ReleaseDC( hwnd, dc);
}

void CClothFramework::_UnselectAllVertex(void)
{
	INT32 i, j;
	COLL_MESH_INFO * pInfo;
	i3CollideeMesh * pMesh;

	for( i = 0; i < m_CollMeshList.GetCount(); i++)
	{
		pInfo = (COLL_MESH_INFO *) m_CollMeshList.Items[i];

		pMesh = pInfo->m_pMesh;

		for( j = 0; j < (INT32) pMesh->GetPositionCount(); j++)
		{
			pInfo->m_pVertexState[j] &= ~VSTATE_SELECTED;
		}
	}
}

void CClothFramework::_SelectVertex( CRect * pRect, bool bSelect)
{
	INT32 i, j;
	COLL_MESH_INFO * pInfo;
	i3CollideeMesh * pMesh;
	i3Sprite2D * pSprite;
	POINT pt;

	for( i = 0; i < m_CollMeshList.GetCount(); i++)
	{
		pInfo = (COLL_MESH_INFO *) m_CollMeshList.Items[i];

		pMesh = pInfo->m_pMesh;
		pSprite = pInfo->m_pSprite;

		for( j = 0; j < (INT32) pMesh->GetPositionCount(); j++)
		{
			if( pInfo->m_pVertexState[j] & VSTATE_CULL)
				continue;

			pt.x = (INT32) i3Vector::GetX( pSprite->GetCenter( j));
			pt.y = (INT32) i3Vector::GetY( pSprite->GetCenter( j));

			if( PtInRect( pRect, pt))
			{
				if( bSelect)
					pInfo->m_pVertexState[j] |= VSTATE_SELECTED;
				else
					pInfo->m_pVertexState[j] &= ~VSTATE_SELECTED;
			}
		}
	}
}

void CClothFramework::OnLButtonDown( UINT32 nFlag, POINT point)
{
	i3ViewerFramework::OnLButtonDown( nFlag, point);

	_DrawDragStart( point);

	m_bSelectDrag = true;

	GetViewer()->SetPendingUpdateEnable( true);
}

void CClothFramework::OnLButtonUp( UINT32 nFlag, POINT point)
{
	i3ViewerFramework::OnLButtonUp( nFlag, point);

	_DrawDragEnd( point);

	m_bSelectDrag = false;

	if( !(nFlag & MK_CONTROL))
	{
		if( getSelectMask() & SELMASK_PARTICLE)
			_UnselectAllVertex();

		if( getSelectMask() & SELMASK_SPRING)
			_UnselectAllSpring();
	}

	if( GetKeyState( VK_LMENU) & 0x80)
	{
		if( getSelectMask() & SELMASK_PARTICLE)
			_SelectVertex( &m_OldDragRect, false);
		
		if( getSelectMask() & SELMASK_SPRING)
			_SelectSpring( &m_OldDragRect, false);
	}
	else
	{
		if( getSelectMask() & SELMASK_PARTICLE)
			_SelectVertex( &m_OldDragRect, true);
		
		if( getSelectMask() & SELMASK_SPRING)
			_SelectSpring( &m_OldDragRect, true);
	}

	GetViewer()->SetPendingUpdateEnable( false);
}

void CClothFramework::_SetFreezeMode( bool bFreeze)
{
	m_bFreeze = bFreeze;

	if( m_bFreeze)
		m_pOverrideRoot->AddAttr( m_pColorAttr);
	else
		m_pOverrideRoot->RemoveAttr( m_pColorAttr);

	m_pTextureEnableAttr->Set( FALSE);
	m_pLightingEnableAttr->Set( FALSE);
}

void CClothFramework::OnRButtonDown( UINT32 nFlag, POINT point)
{
	CMenu menu, * pPopup;
	POINT pt;
	UINT cmd;

	GetCursorPos( &pt);

	menu.LoadMenu( MAKEINTRESOURCE( IDR_POPUP_VERTEX));

	pPopup = menu.GetSubMenu( 0);

	{
		// Particle Visibility
		if( m_pVertexRoot->IsFlag( I3_NODEFLAG_DISABLE))
			pPopup->CheckMenuItem( ID_VERTEX_VERTEXVISIBLE, MF_BYCOMMAND | MF_UNCHECKED);
		else
			pPopup->CheckMenuItem( ID_VERTEX_VERTEXVISIBLE, MF_BYCOMMAND | MF_CHECKED);

		// Spring Visibility
		if( m_pSpringRoot->IsFlag( I3_NODEFLAG_DISABLE))
			pPopup->CheckMenuItem( ID_VERTEX_SPRINGVISIBLE, MF_BYCOMMAND | MF_UNCHECKED);
		else
			pPopup->CheckMenuItem( ID_VERTEX_SPRINGVISIBLE, MF_BYCOMMAND | MF_CHECKED);

		// Freeze
		if( m_bFreeze)
			pPopup->CheckMenuItem( ID_VERTEX_FREEZE, MF_BYCOMMAND | MF_CHECKED);
		else
			pPopup->CheckMenuItem( ID_VERTEX_FREEZE, MF_BYCOMMAND | MF_UNCHECKED);

		// Texture Enable Menu Item
		if( m_pTextureEnableAttr->Get())
			pPopup->CheckMenuItem( ID_VERTEX_TEXTUREMAPPING, MF_BYCOMMAND | MF_CHECKED);
		else
			pPopup->CheckMenuItem( ID_VERTEX_TEXTUREMAPPING, MF_BYCOMMAND | MF_UNCHECKED);

		// Lighting Enable Menu Item
		if( m_pLightingEnableAttr->Get())
			pPopup->CheckMenuItem( ID_VERTEX_LIGHTING, MF_BYCOMMAND | MF_CHECKED);
		else
			pPopup->CheckMenuItem( ID_VERTEX_LIGHTING, MF_BYCOMMAND | MF_UNCHECKED);

		// Render Mode Menu Item
		switch( m_pRenderModeAttr->Get())
		{
			case I3G_RENDER_LINE :
				pPopup->CheckMenuRadioItem( ID_RENDERMODE_POLYGON, ID_RENDERMODE_LINE, ID_RENDERMODE_LINE, MF_BYCOMMAND);
				break;

			default :
				pPopup->CheckMenuRadioItem( ID_RENDERMODE_POLYGON, ID_RENDERMODE_LINE, ID_RENDERMODE_POLYGON, MF_BYCOMMAND);
				break;
		}

		// Backface Culling Menu Item
		if( m_pFaceCullModeAttr->Get() != I3G_FACE_CULL_NONE)
			pPopup->CheckMenuItem( ID_VERTEX_BACKFACECULLING, MF_BYCOMMAND | MF_CHECKED);
		else
			pPopup->CheckMenuItem( ID_VERTEX_BACKFACECULLING, MF_BYCOMMAND | MF_UNCHECKED);
		
	}

	cmd = TrackPopupMenuEx( pPopup->m_hMenu, TPM_LEFTALIGN | TPM_HORNEGANIMATION | TPM_VERNEGANIMATION | TPM_VERTICAL | TPM_RETURNCMD,
					pt.x, pt.y, GetViewer()->GetWindowHandle(), NULL);

	switch( cmd)
	{
		case ID_VERTEX_VERTEXVISIBLE :
			if( m_pVertexRoot->IsFlag( I3_NODEFLAG_DISABLE))
				m_pVertexRoot->RemoveFlag( I3_NODEFLAG_DISABLE);
			else
				m_pVertexRoot->AddFlag( I3_NODEFLAG_DISABLE);
			break;

		case ID_VERTEX_SPRINGVISIBLE :
			if( m_pSpringRoot->IsFlag( I3_NODEFLAG_DISABLE))
				m_pSpringRoot->RemoveFlag( I3_NODEFLAG_DISABLE);
			else
				m_pSpringRoot->AddFlag( I3_NODEFLAG_DISABLE);
			break;

		case ID_VERTEX_FREEZE :				
			m_bFreeze = !m_bFreeze;
			_SetFreezeMode( m_bFreeze);
			break;

		case ID_VERTEX_TEXTUREMAPPING :		m_pTextureEnableAttr->Set( ! m_pTextureEnableAttr->Get());	break;
		case ID_VERTEX_LIGHTING :			m_pLightingEnableAttr->Set( ! m_pLightingEnableAttr->Get());	break;
		case ID_RENDERMODE_POLYGON :		m_pRenderModeAttr->Set( I3G_RENDER_POLYGON);		break;
		case ID_RENDERMODE_LINE :			m_pRenderModeAttr->Set( I3G_RENDER_LINE);			break;
		case ID_VERTEX_BACKFACECULLING :
			if( m_pFaceCullModeAttr->Get() == I3G_FACE_CULL_NONE)
				m_pFaceCullModeAttr->Set( I3G_FACE_CULL_BACK);
			else
				m_pFaceCullModeAttr->Set( I3G_FACE_CULL_NONE);
			break;

		case ID_VERTEX_CREATECLOTHPARTICLES :
			_BuildClothParticle();
			break;

		case ID_SPRING_SELECTALL :
			_SelectAllSpring();
			break;

		case ID_SPRING_UNSELECTALL :
			_UnselectAllSpring();
			break;

		case ID_SPRING_REVERSESELECTION :
			_ReverseSelectSpring();
			break;

		case ID_SPRING_SELECT :
			_SelectSpring();
			break;

		case ID_SPRING_DELETE :
			_DeleteSelectedSpring();
			break;
	}
}

void CClothFramework::OnRButtonUp( UINT32 nFlag, POINT point)
{
}

void CClothFramework::OnMouseMove( UINT32 nFlag, POINT point)
{
	if( m_bSelectDrag)
	{
		_DrawDrag( point);
	}
	else
	{
		i3ViewerFramework::OnMouseMove( nFlag, point);
	}
}

INT32 CClothFramework::_FindSelectMesh(void)
{
	INT32 i, j, count, target = -1;
	COLL_MESH_INFO * pInfo;

	count = 0;

	for( i = 0; i < m_CollMeshList.GetCount(); i++)
	{
		pInfo = (COLL_MESH_INFO *) m_CollMeshList.Items[i];

		for( j = 0; j < (INT32) pInfo->m_pMesh->GetPositionCount(); j++)
		{
			if( pInfo->m_pVertexState[j] & VSTATE_SELECTED)
			{
				count++;
				target = i;
				break;
			}
		}
	}

	if( count > 1)
	{
		::MessageBox( NULL, 
			"2개 이상의 Geometry가 동시에 선택되어 있습니다.\r\nCloth는 1개의 Geomtry에 대해서만 생성 가능합니다.", 
			"Warning", MB_OK);
	}

	return target;
}

// 지정된 Triangle과 한 변(2개의 Vertex)를 공유하는 또 다른 Triangle을 찾는다.
// 이렇게 찾아진 Triangle은 원본 Triangle과 함께 사각형을 이루는 짝이 된다.
static I3_COLLIDEE_TRI * _FindPairTriangle( i3CollideeMesh * pMesh, INT32 idx, INT32 * pShareIdx)
{
	INT32 count;
	UINT32 i, j, k;
	I3_COLLIDEE_TRI * pSrc, * pDest;

	pSrc = pMesh->GetTriangle( idx);

	for( i = 0; i < pMesh->GetTriangleCount(); i++)
	{
		if( i == (UINT32) idx)
			continue;

		pDest = pMesh->GetTriangle( i);

		count = 0;

		for( j = 0; j < 3; j++)
		{
			for( k = 0; k < 3; k++)
			{
				if( pSrc->m_Index[j] == pDest->m_Index[k])
				{
					pShareIdx[ count] = j;			// 공유되고 있는 Vertex가 어느 것인지 저장해 둔다.

					count++;

					if( count >= 2)
					{
						// 공유되고 있는 2개의 Vertex를 모두 찾았다>
						return pDest;
					}
				}
			}
		}
	}

	return NULL;
}

// Triangle의 3개 Vertex들에서 선태된 Vertex의 개수를 반환한다.
static INT32 _GetSelectedTriVertexCount( COLL_MESH_INFO * pInfo, I3_COLLIDEE_TRI * pTri)
{
	INT32 i, count = 0;

	for( i = 0; i < 3; i++)
	{
		if( pInfo->m_pVertexState[ pTri->m_Index[i]] & VSTATE_SELECTED)
			count++;
	}

	return count;
}

static void _FindTriangleByVertexIndex( i3CollideeMesh * pMesh, INT32 vidx, i3List * pList)
{
	UINT32 i, j;
	I3_COLLIDEE_TRI * pDest;

	for( i = 0; i < pMesh->GetTriangleCount(); i++)
	{
		pDest = pMesh->GetTriangle( i);

		for( j = 0; j < 3; j++)
		{
			if( pDest->m_Index[j] == vidx)
			{
				pList->Add( pDest);
				break;
			}
		}
	}
}

void _MakeVertexRefIndexTable( INT32 ptcIdx, INT32 * pIndexTable, INT32 Count, i3List * pOutList)
{
	INT32 i;

	for( i = 0; i < Count; i++)
	{
		if( pIndexTable[i] == ptcIdx)
		{
			pOutList->Add( (void *) i);
		}
	}
}

void CClothFramework::_BuildClothParticle(void)
{
	INT32 idx;
	UINT32 i, VertexCount, j, k;
	i3List List, SpringList;
	COLL_MESH_INFO * pInfo;
	i3CollideeMesh * pMesh;
	i3List IndexList;

	// Cloth Object는 1개의 Geometry Attribute에 대해서만 
	// 생성 가능하다.
	idx = _FindSelectMesh();
	if( idx == -1)
	{
		::MessageBox( NULL, "Vertex를 선택해야만 Cloth를 생성할 수 있습니다.", "Error", MB_OK);
		return;
	}

	pInfo = (COLL_MESH_INFO *) m_CollMeshList.Items[idx];

	m_pTargetInfo = pInfo;

	pMesh = pInfo->m_pMesh;
	VertexCount  = pMesh->GetPositionCount();

	SpringList.SetOnceMode( TRUE);

	// 모든 Vertex의 State에서 VSTATE_PARTICLE과 VSTATE_LOCKED를 제거한다.
	START_PROFILE( 0);
	for( i = 0; i < VertexCount; i++)
	{
		pInfo->m_pVertexState[i] &= ~(VSTATE_PARTICLE);
	}
	END_PROFILE( 0);

	List.SetOptmizeEnableWhenClear( FALSE);

	for( i = 0; i < VertexCount; i++)
	{
		// 선택되어 있지 않은 Vertex는 처리 대상에서 제외
		if( !(pInfo->m_pVertexState[i] & VSTATE_SELECTED))
			continue;

		// 후처리할 Particle 생성 표시를 해 둔다.
		pInfo->m_pVertexState[i] |= VSTATE_PARTICLE;		

		// 해당 Vertex를 사용하고 있는 모든 Triangle을 찾는다.
		List.Clear();
		START_PROFILE( 1);
		_FindTriangleByVertexIndex( pMesh, i, &List);
		END_PROFILE( 1);

		START_PROFILE( 2);
		for( j = 0; j < (UINT32) List.GetCount(); j++)
		{
			I3_COLLIDEE_TRI * pTri = (I3_COLLIDEE_TRI *) List.Items[j];

			for( k = 0; k < 3; k++)
			{
				if( pTri->m_Index[k] == i)
				{
					// 현재 처리 대상 Vertex는 처리 대상에서 제외
					continue;
				}

				// Triangle을 이루는 나머지 2개 Vertex에 대해서도 Particle 생성을 하도록 추가한다.
				// 이것은 선택되지 않은 Vertex라 하더라도 추가된다.
				// 단, 선택되지 않은 Vertex는 자동으로 Locked Particle로 간주한다.
				pInfo->m_pVertexState[ pTri->m_Index[k]] |= VSTATE_PARTICLE;

				// 시작 Vertex(i)과 끝 Vertex(pTri->m_Index[k]) 사이를 잊는 Spring을 준비한다.
				// 중복 생성되는 경우를 배제하기 위해, i3List의 Once Mode를 활용한다.
				// 
				if( i < pTri->m_Index[k])
					SpringList.Add( (void *) MAKELONG( i, pTri->m_Index[k]));
				else
					SpringList.Add( (void *) MAKELONG( pTri->m_Index[k], i));
			}
		}
		END_PROFILE( 2);
	}
	
	//////////////////////////////////////////////////////////////////
	// 실제 Cloth Particle 생성
	// 
	// 상기 처리에서 설정해 둔, VertexState를 바탕으로 i3PhysicsParticle Object를 생성한다.
	{
		memset( pInfo->m_ppParticle, 0, sizeof(void *) * pInfo->m_pMesh->GetPositionCount());	// 초기화

		START_PROFILE( 3);
		for( i = 0; i < VertexCount; i++)
		{
			if( pInfo->m_pVertexState[i] & VSTATE_PARTICLE)
			{
				i3PhysicsParticle * pObj = i3PhysicsParticle::NewObjectRef();

				// Particle이 실제 Rendering Geometry의 어느 Vertex와
				// 연관되어 있는 것인지 설정한다.
				{
					pObj->setTableStartIndex( IndexList.GetCount());

					_MakeVertexRefIndexTable( i, pInfo->m_pVtxIndexTable, 
						pInfo->m_pGeoAttr->GetVertexArray()->GetCount(), &IndexList);

					pObj->setTableEndIndex( IndexList.GetCount() - 1);
				}

				if( !(pInfo->m_pVertexState[i] & VSTATE_SELECTED))
				{
					// VSTATE_PARTICLE이 설정되어 있음에도 Select 상태가 아닌 경우에는
					// Locked Particle이라 간주한다.
					pObj->addStyle( I3_PHYSICS_PARAM_LOCKED );
				}

				// Position
				{
					pObj->setPos( pMesh->GetPosition( i));
				}
				// 기본 파티클로 세팅
				// pObj->setMass( 1.0f );
				// pObj->setRestitution( 0.1f );

				m_pCloth->AddParticle( pObj);

				// 생성된 Particle 정보를 역으로 Reference 설정해 둔다.
				// 편집 시에 관련 Vertex와 Particle의 연결 정보를 필요로 하기 때문이다.
				pInfo->m_ppParticle[ i] = pObj;	
			}
		}
		END_PROFILE( 3);
	}

	///////////////////////////////////////////////////////////////
	// Index Table의 작성
	{
		i3PHSParticleIdxTable * pIdxTable = i3PHSParticleIdxTable::NewObjectRef();

		pIdxTable->Create( IndexList.GetCount());

		for( i = 0; i < (UINT32) IndexList.GetCount(); i++)
		{
			pIdxTable->setIndex( i, (INT32) IndexList.Items[i]);
		}

		m_pCloth->SetIndexTable( pIdxTable);
	}

	///////////////////////////////////////////////////////////////
	// i3Spring Object의 생성
	{
		for( i = 0; i < (UINT32) SpringList.GetCount(); i++)
		{
			UINT32 idx0, idx1;
			i3Spring * pSpring;
			i3PhysicsParticle * pP0, * pP1;

			idx0 = LOWORD( (UINT32) SpringList.Items[i]);
			idx1 = HIWORD( (UINT32) SpringList.Items[i]);

			pP0 = pInfo->m_ppParticle[idx0];
			I3ASSERT( pP0 != NULL);

			pP1 = pInfo->m_ppParticle[idx1];
			I3ASSERT( pP1 != NULL);
			
			pSpring = i3Spring::NewObjectRef();

			pSpring->setParticles( pP0, pP1);

			m_pCloth->AddSpring( pSpring);
		}
	}

	_BuildSpringGeometry( pInfo);

	RESET_PROFILE();
	REPORT_PROFILE();
}


void CClothFramework::_BuildSpringGeometry( COLL_MESH_INFO * pInfo)
{
	INT32 i, vidx;
	i3Spring * pSpring;
	i3Geometry * pGeo;
	i3GeometryAttr * pGeoAttr;
	i3VertexFormat vf;
	i3VertexArray * pSrcVA;
	i3PHSParticleIdxTable * pIndexTable;
	COLORREAL col;

	// 기존의 Geometry들을 제거한다.
	m_pSpringRoot->RemoveAllChild();

	if( m_pCloth->getSpringCount() <= 0)
		return;

	{
		vf.SetHasPosition( true);
		vf.SetHasColor( true);

		m_pSpringVA = i3VertexArray::NewObjectRef();
		m_pSpringVA->Create( &vf, m_pCloth->getSpringCount() * 2, 0);

		pGeoAttr = i3GeometryAttr::NewObjectRef();
		pGeoAttr->SetVertexArray( m_pSpringVA, I3G_PRIM_LINELIST, m_pCloth->getSpringCount());

		pGeo = i3Geometry::NewObjectRef();
		pGeo->AppendGeometryAttr( pGeoAttr);

		m_pSpringRoot->AddChild( pGeo);
	}

	pSrcVA = pInfo->m_pGeoAttr->GetVertexArray();
	pSrcVA->Lock();

	m_pSpringVA->Lock();

	vidx = 0;
	pIndexTable = m_pCloth->getIndexTable();

	i3Color::Set( &col, &s_UnSelColor);

	if( m_pCloth->getSpringCount() > 0)
	{
		if( pInfo->m_pSpringState != NULL)
		{
			delete pInfo->m_pSpringState;
			pInfo->m_pSpringState = NULL;
		}

		pInfo->m_pSpringState = new UINT32[ m_pCloth->getSpringCount()];

		for( i = 0; i < m_pCloth->getSpringCount(); i++)
		{
			i3PhysicsParticle * pP1, * pP2;
			INT32 idx1, idx2;
			VEC3D vec;

			pSpring = m_pCloth->getSpring( i);

			pP1 = pSpring->getStartParticle();
			pP2 = pSpring->getEndParticle();

			idx1 = pIndexTable->getIndex( pP1->getTableStartIndex());		// Particle -> Render Vertex Index
			idx2 = pIndexTable->getIndex( pP2->getTableStartIndex());		// Particle -> Render Vertex Index

			pSrcVA->GetPosition( idx1, &vec);
			i3Vector::TransformCoord( &vec, &vec, &pInfo->m_Matrix);

			m_pSpringVA->SetPosition( vidx, &vec);		
			m_pSpringVA->SetColor( vidx, &col);			vidx++;

			pSrcVA->GetPosition( idx2, &vec);
			i3Vector::TransformCoord( &vec, &vec, &pInfo->m_Matrix);

			m_pSpringVA->SetPosition( vidx, &vec);		
			m_pSpringVA->SetColor( vidx, &col);			vidx++;

			pInfo->m_pSpringState[i] = 0;
		}
	}

	m_pSpringVA->Unlock();
}

i3Node * CClothFramework::OnQueryAttachNode( i3GameRes * pRes)
{
	return m_pOverrideRoot;
}

void CClothFramework::_SelectAllSpring(void)
{
	INT32 i;

	if( m_pSpringVA == NULL)
		return;

	if( m_pTargetInfo == NULL)
		return;

	m_pSpringVA->Lock();

	for( i = 0; i < m_pCloth->getSpringCount(); i++)
	{
		m_pTargetInfo->m_pSpringState[i] |= SSTATE_SELECTED;

		m_pSpringVA->SetColor( i * 2, 2, & s_SelColor);
	}

	m_pSpringVA->Unlock();

	i3TDK::Update( NULL, I3_TDK_UPDATE_SELECT, NULL, i3Spring::GetClassMeta());
}

void CClothFramework::_UnselectAllSpring(void)
{
	INT32 i;

	if( m_pSpringVA == NULL)
		return;

	if( m_pTargetInfo == NULL)
		return;

	m_pSpringVA->Lock();

	for( i = 0; i < m_pCloth->getSpringCount(); i++)
	{
		m_pTargetInfo->m_pSpringState[i] &= ~SSTATE_SELECTED;

		m_pSpringVA->SetColor( i * 2, 2, & s_UnSelColor);
	}

	m_pSpringVA->Unlock();

	i3TDK::Update( NULL, I3_TDK_UPDATE_SELECT, NULL, i3Spring::GetClassMeta());
}

void CClothFramework::_ReverseSelectSpring(void)
{
	INT32 i;

	if( m_pSpringVA == NULL)
		return;

	if( m_pTargetInfo == NULL)
		return;

	m_pSpringVA->Lock();

	for( i = 0; i < m_pCloth->getSpringCount(); i++)
	{
		if( m_pTargetInfo->m_pSpringState[i] & SSTATE_SELECTED)
		{
			m_pTargetInfo->m_pSpringState[i] &= ~SSTATE_SELECTED;
			m_pSpringVA->SetColor( i * 2, 2, & s_UnSelColor);
		}
		else
		{
			m_pTargetInfo->m_pSpringState[i] |= SSTATE_SELECTED;
			m_pSpringVA->SetColor( i * 2, 2, & s_SelColor);
		}
	}

	m_pSpringVA->Unlock();

	i3TDK::Update( NULL, I3_TDK_UPDATE_SELECT, NULL, i3Spring::GetClassMeta());
}

void CClothFramework::_SelectSpring( CRect * pRect, bool bSelect)
{
	INT32 i;
	i3PhysicsParticle * pP;
	i3Spring * pSpring;
	MATRIX mtx;
	i3Camera * pCam = GetCurrentCamera();
	REAL32 hcx, hcy;
	VEC3D pos;
	POINT pt;

	if( m_pTargetInfo == NULL)
		return;

	if( m_pSpringVA == NULL)
		return;

	//
	i3Matrix::Mul( &mtx, pCam->GetViewMatrixAttr()->GetMatrix(), pCam->GetProjectionMatrix());
	i3Matrix::Mul( &mtx, &m_pTargetInfo->m_Matrix, &mtx);

	{
		hcx = GetViewer()->GetViewWidth() * 0.5f;
		hcy = GetViewer()->GetViewHeight() * 0.5f;
	}

	m_pSpringVA->Lock();

	for( i = 0; i < m_pCloth->getSpringCount(); i++)
	{
		pSpring = m_pCloth->getSpring( i);
		
		// Particle 1
		pP = pSpring->getParticle1();
		i3Vector::TransformCoord( &pos, pP->getPos(), &mtx);

		pt.x = (INT32)((i3Vector::GetX( &pos) * hcx) + hcx);
		pt.y = (INT32)((i3Vector::GetY( &pos) * -hcy) + hcy);

		if( PtInRect( pRect, pt))
		{
			if( bSelect)
			{
				m_pTargetInfo->m_pSpringState[i] |= SSTATE_SELECTED;
				m_pSpringVA->SetColor( i * 2, 2, &s_SelColor);
			}
			else
			{
				m_pTargetInfo->m_pSpringState[i] &= ~SSTATE_SELECTED;
				m_pSpringVA->SetColor( i * 2, 2, &s_UnSelColor);
			}
		}
		else
		{
			// Particle 2
			pP = pSpring->getParticle2();
			i3Vector::TransformCoord( &pos, pP->getPos(), &mtx);

			pt.x = (INT32)((i3Vector::GetX( &pos) * hcx) + hcx);
			pt.y = (INT32)((i3Vector::GetY( &pos) * -hcy) + hcy);

			if( PtInRect( pRect, pt))
			{
				if( bSelect)
				{
					m_pTargetInfo->m_pSpringState[i] |= SSTATE_SELECTED;
					m_pSpringVA->SetColor( i * 2, 2, &s_SelColor);
				}
				else
				{
					m_pTargetInfo->m_pSpringState[i] &= ~SSTATE_SELECTED;
					m_pSpringVA->SetColor( i * 2, 2, &s_UnSelColor);
				}
			}
		}
	}

	m_pSpringVA->Unlock();

	i3TDK::Update( NULL, I3_TDK_UPDATE_SELECT, NULL, i3Spring::GetClassMeta());
}

void CClothFramework::_SelectSpring(void)
{
	INT32 i;
	i3PhysicsParticle * pP;
	i3Spring * pSpring;
	MATRIX mtx;
	i3Camera * pCam = GetCurrentCamera();
	REAL32 hcx, hcy;
	VEC3D pos;
	VEC2D pt1, pt2, * pDir;
	CSelectSpringDialog dlg;

	if( dlg.DoModal() == IDCANCEL)
		return;

	if( m_pTargetInfo == NULL)
		return;

	if( m_pSpringVA == NULL)
		return;

	pDir = dlg.getDirection();

	//
	i3Matrix::Mul( &mtx, pCam->GetViewMatrixAttr()->GetMatrix(), pCam->GetProjectionMatrix());
	i3Matrix::Mul( &mtx, &m_pTargetInfo->m_Matrix, &mtx);

	{
		hcx = GetViewer()->GetViewWidth() * 0.5f;
		hcy = GetViewer()->GetViewHeight() * 0.5f;
	}

	m_pSpringVA->Lock();

	for( i = 0; i < m_pCloth->getSpringCount(); i++)
	{
		REAL32 dot;

		pSpring = m_pCloth->getSpring( i);
		
		// Particle 1
		pP = pSpring->getParticle1();
		i3Vector::TransformCoord( &pos, pP->getPos(), &mtx);
		i3Vector::Set( &pt1, ((i3Vector::GetX( &pos) * hcx) + hcx), ((i3Vector::GetY( &pos) * -hcy) + hcy));

		// Particle 2
		pP = pSpring->getParticle2();
		i3Vector::TransformCoord( &pos, pP->getPos(), &mtx);
		i3Vector::Set( &pt2, ((i3Vector::GetX( &pos) * hcx) + hcx), ((i3Vector::GetY( &pos) * -hcy) + hcy));

		i3Vector::Sub( &pt1, &pt2, &pt1);
		i3Vector::Normalize( &pt1, &pt1);

		dot = i3Math::abs( i3Vector::Dot( &pt1, pDir));

		if( dot >= 0.95f)
		{
			m_pTargetInfo->m_pSpringState[i] |= SSTATE_SELECTED;
			m_pSpringVA->SetColor( i * 2, 2, &s_SelColor);
		}
		else
		{
			m_pTargetInfo->m_pSpringState[i] &= ~SSTATE_SELECTED;
			m_pSpringVA->SetColor( i * 2, 2, &s_UnSelColor);
		}
	}

	m_pSpringVA->Unlock();

	i3TDK::Update( NULL, I3_TDK_UPDATE_SELECT, NULL, i3Spring::GetClassMeta());
}

void CClothFramework::GetSelectedSpring( i3List * pList)
{
	INT32 i;

	if( m_pTargetInfo == NULL)
		return;

	if( m_pTargetInfo->m_pSpringState == NULL)
		return;

	for( i = 0; i < m_pCloth->getSpringCount(); i++)
	{
		if( m_pTargetInfo->m_pSpringState[i] & SSTATE_SELECTED)
			pList->Add( m_pCloth->getSpring( i));
	}
}

void CClothFramework::_DeleteSelectedSpring(void)
{
	INT32 i;
	i3Spring * pSpring;
	i3List UnSelList;

	if( m_pTargetInfo == NULL)
		return;

	if( m_pTargetInfo->m_pSpringState == NULL)
		return;

	// 선택되지 않은 Spring만을 추출한다.
	for( i = 0; i < m_pCloth->getSpringCount(); i++)
	{
		if( !(m_pTargetInfo->m_pSpringState[i] & SSTATE_SELECTED))
		{
			pSpring = m_pCloth->getSpring( i);
			UnSelList.Add( pSpring);
			pSpring->AddRef();				// 메모리 제거되지 않도록...
		}
	}

	// 모든 Spring들 및 관련 데이터를 제거한다.
	{
		m_pCloth->RemoveAllSprings();

		// Editor용 데이터는 아래의  _BuildSpringGeometry() 함수 내에서
		// 제거한다.
	}

	// 선택되지 않았던 Spring들을 다시 Cloth에 추가한다.
	for( i = 0; i < UnSelList.GetCount(); i++)
	{
		pSpring = (i3Spring *) UnSelList.Items[i];

		m_pCloth->AddSpring( pSpring);

		pSpring->Release();		// Ref.Count의 조정.
	}

	_BuildSpringGeometry( m_pTargetInfo);

	i3TDK::Update( NULL, I3_TDK_UPDATE_REMOVE, NULL, i3Spring::GetClassMeta());
}

////////////////////////////////////////////////////////////////////////////
// Particle
void CClothFramework::_SelectAllParticle(void)
{
	INT32 i;

	if( m_pSpringVA == NULL)
		return;

	if( m_pTargetInfo == NULL)
		return;

	m_pSpringVA->Lock();

	for( i = 0; i < m_pCloth->getParticleCount(); i++)
	{
		m_pTargetInfo->m_pVertexState[i] |= SSTATE_SELECTED;

		m_pSpringVA->SetColor( i * 2, 2, & s_SelColor);
	}

	m_pSpringVA->Unlock();

	i3TDK::Update( NULL, I3_TDK_UPDATE_SELECT, NULL, i3PhysicsParticle::GetClassMeta());
}

void CClothFramework::_SelectParticle( CRect * pRect, bool bSelect )
{
	INT32 i;
	i3PhysicsParticle * pParticle;
	MATRIX mtx;
	i3Camera * pCam = GetCurrentCamera();
	REAL32 hcx, hcy;
	VEC3D pos;
	POINT pt;

	if( m_pTargetInfo == NULL)
		return;

	if( m_pSpringVA == NULL)
		return;

	//
	i3Matrix::Mul( &mtx, pCam->GetViewMatrixAttr()->GetMatrix(), pCam->GetProjectionMatrix());
	i3Matrix::Mul( &mtx, &m_pTargetInfo->m_Matrix, &mtx);

	{
		hcx = GetViewer()->GetViewWidth() * 0.5f;
		hcy = GetViewer()->GetViewHeight() * 0.5f;
	}

	m_pSpringVA->Lock();

	for( i = 0; i < m_pCloth->getParticleCount(); i++)
	{
		pParticle = m_pCloth->getParticle( i);
		
		// Particle 1
		i3Vector::TransformCoord( &pos, pParticle->getPos(), &mtx);

		pt.x = (INT32)((i3Vector::GetX( &pos) * hcx) + hcx);
		pt.y = (INT32)((i3Vector::GetY( &pos) * -hcy) + hcy);

		if( PtInRect( pRect, pt))
		{
			if( bSelect)
			{
				m_pTargetInfo->m_pVertexState[i] |= SSTATE_SELECTED;
				m_pSpringVA->SetColor( i * 2, 2, &s_SelColor);
			}
			else
			{
				m_pTargetInfo->m_pVertexState[i] &= ~SSTATE_SELECTED;
				m_pSpringVA->SetColor( i * 2, 2, &s_UnSelColor);
			}
		}
	}

	m_pSpringVA->Unlock();

	i3TDK::Update( NULL, I3_TDK_UPDATE_SELECT, NULL, i3PhysicsParticle::GetClassMeta());
}
void CClothFramework::_UnselectAllParticle(void)
{
}
void CClothFramework::_ReverseSelectParticle(void)
{
}
void CClothFramework::_DeleteSelectedParticle(void)
{
}

void CClothFramework::GetSelectedParticle( i3List * pList )
{
	INT32 i;

	if( m_pTargetInfo == NULL)
		return;

	if( m_pTargetInfo->m_pVertexState == NULL)
		return;

	for( i = 0; i < m_pCloth->getParticleCount(); i++)
	{
		if( m_pTargetInfo->m_pVertexState[i] & SSTATE_SELECTED)
			pList->Add( m_pCloth->getParticle( i));
	}
}

