#include "stdafx.h"
#include "i3LevelScene.h"
#include "i3LevelGlobalVariable.h"
#include "i3LevelViewport.h"
#include "i3LevelControl.h"
#include "i3LevelLIght.h"
#include "i3LevelElementList.h"
#include "i3LevelObject.h"
#include "../i3TDKClipboard.h"

#include "i3LevelUndoInfo.h"
#include "i3LevelUndoInfoCreate.h"
#include "i3LevelUndoInfoDelete.h"

#include "i3NavMeshDataMgr.h"


#include "i3LevelPrimitive_Box.h"
#include "i3LevelPrimitive_Sphere.h"
#include "i3LevelPrimitive_Capsule.h"
#include "i3LevelResPrim.h"


///////////////////////////////
// Element
void i3LevelScene::_AttachToScene( i3LevelRes * pRes, i3LevelElement3D * pElm)
{
	if((pRes == nullptr) || (pRes->GetElementAttachNode( this) == nullptr))
	{
		if( i3::kind_of<i3LevelControl* >(pElm))
			m_pControlRoot->AddChild( pElm->GetRoot());
		else if( i3::kind_of<i3LevelLight*>(pElm))
			m_pLightRoot->AddChild( pElm->GetRoot());
		else
		{
			if(i3::kind_of<i3LevelPrimitive* >(pElm) )
				m_pPrimitiveRoot->AddChild(pElm->GetRoot());
			else
				m_pUnitRoot->AddChild( pElm->GetRoot());
		}
	}
	else
	{
		i3Node * pAttachNode = pRes->GetElementAttachNode( this);

		pAttachNode->AddChild( pElm->GetRoot());
	}
}

INT32 i3LevelScene::AddElement( i3LevelElement3D * pElm, const char * pszPrefix, bool bCheckOnce)
{
	
	i3LevelRes * pRes;

	INT32 idx = (INT32)i3::vu::index_of(m_ElementList, pElm);
	
	if( idx == (INT32)m_ElementList.size() )
	{
		idx = -1;
		// 이름 조정
		if( pElm->hasName() == false)
		{
			char szName[256];

			GetUniqueElementName( pszPrefix, szName);

			pElm->SetName( szName);
		}

		I3_MUST_ADDREF(pElm);

		if( pElm->getLayer() == nullptr)
			pElm->setLayer( m_pCurLayer);

		idx = (INT32) m_ElementList.size();
		m_ElementList.push_back( pElm);

		pRes = pElm->getRes();

		if( pRes )
			g_pResDB->AddRes( pRes );

		_AttachToScene( pRes, pElm);

		pElm->OnAttachScene( this);

		pElm->AddState( I3_LEVEL_STATE_ATTACHED);

		i3LevelUndo::ADD_Create( pElm);

		i3Level::Update( nullptr, I3_TDK_UPDATE_ADD, pElm);
	}

	{
		VEC3D vmin, vmax;

		GetWrappingBoundBox( &vmin, &vmax);

		SetBound( &vmin, &vmax);

		i3Vector::Sub( &vmax, &vmax, &vmin);
	}

	return idx;
}

void i3LevelScene::RemoveElement( const i3::vector<i3LevelElement3D*>& List)
{
	i3LevelElement3D * pElm;

	if( List.size() > 1)	i3LevelUndo::BeginBatch();

	for( size_t i = 0; i < List.size(); i++)
	{
		pElm = List[i];

		RemoveElement( pElm);
	}

	if( List.size() > 1)	i3LevelUndo::EndBatch();
}

void i3LevelScene::RemoveElement( i3LevelElement3D * pElm)
{
	INT32 idx;

	idx = GetElementIndex( pElm);
	if( idx == -1)
		return;

	RemoveElement( idx);
}

void i3LevelScene::RemoveElement( INT32 idx)
{
	i3LevelElement3D * pElm;

	pElm = GetElement( idx);

	if( pElm->IsState( I3_LEVEL_STATE_LOCKED))
	{
		// 제거 중이기 때문에..
		return;
	}

	pElm->AddState( I3_LEVEL_STATE_LOCKED);

	if( pElm->IsStyle( I3_LEVEL_STYLE_CONTROL))
	{
		i3LevelControl * pCtrl = (i3LevelControl *) pElm;

		if( pCtrl->GetOwner() != nullptr)
		{
			i3LevelElement3D * pOwner = pCtrl->GetOwner();

			if( pOwner->OnRemoveIcon( pCtrl) == false)
			{
				pElm->RemoveState( I3_LEVEL_STATE_LOCKED);
				return;
			}
		}
	}

	i3LevelRes* pRes = pElm->getRes();

	// 해당 리소스를 사용하는 모든 경우를 확인해서 필요없다면 삭제한다.
	if( pElm->same_of( i3LevelPrimitive_Box::static_meta() ) || 		
		pElm->same_of( i3LevelPrimitive_Sphere::static_meta() ) ||
		pElm->same_of( i3LevelPrimitive_Capsule::static_meta() ) )
	{
		if( pRes )
			g_pResDB->RemoveRes( pRes );
	}

	// Layer에서 삭제
	{
		i3LevelLayer * pLayer = pElm->getLayer();

		if( pLayer != nullptr )
			pLayer->RemoveElement( pElm );
	}

	// Group에서 삭제
	{
		i3LevelGroup * pGroup = pElm->getGroup();

		if( pGroup != nullptr )
			pGroup->RemoveElement( pElm );
	}

	m_ElementList.erase( m_ElementList.begin() + idx);

	{
		pElm->GetRoot()->RemoveFromParent();
	}

	i3LevelUndo::ADD_Delete( pElm);

	pElm->RemoveState( I3_LEVEL_STATE_ATTACHED);
	pElm->OnDetachScene( this);
	i3Level::Update( nullptr, I3_TDK_UPDATE_REMOVE, pElm);

	pElm->RemoveState( I3_LEVEL_STATE_LOCKED);
	I3_MUST_RELEASE(pElm);
}

void i3LevelScene::RemoveAllElement(void)
{
	i3LevelElement3D * pElm;

	for( ; m_ElementList.size() > 0;)
	{
		pElm = m_ElementList[0];

		if( pElm->IsState( I3_LEVEL_STATE_LOCKED) == false)
		{
			pElm->AddState(I3_LEVEL_STATE_LOCKED);

			pElm->GetRoot()->RemoveFromParent();

			pElm->RemoveState( I3_LEVEL_STATE_ATTACHED);
			pElm->OnDetachScene( this);

			pElm->RemoveState(I3_LEVEL_STATE_LOCKED);
			I3_MUST_RELEASE(pElm);
		}

		m_ElementList.erase( m_ElementList.begin() );		// 이 즈음 되면 deque를 고려할수도 있을지 모른다..
	}

	m_ElementList.clear();

	i3Level::Update( nullptr, I3_TDK_UPDATE_REMOVE, nullptr, i3LevelElement3D::static_meta());
}

void i3LevelScene::RemoveSelectedElement(void)
{
	i3::vector<i3LevelElement3D*> SelList;
	i3LevelElement3D * pElm;

	GetSelectedElements( SelList);

	if( SelList.size() > 1)	i3LevelUndo::BeginBatch();

	for( size_t i = 0; i < SelList.size(); i++)
	{
		pElm = SelList[i];

		RemoveElement( pElm);
	}

	if( SelList.size() > 1)	i3LevelUndo::EndBatch();
}

INT32 i3LevelScene::FindElementByName( const char * pszName)
{
	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement( i);

		if( pElm->hasName())
		{
			if( i3::generic_is_iequal( pElm->GetName(), pszName) )
				return i;
		}
	}

	return -1;
}

void i3LevelScene::FindElementByType( i3::vector<i3LevelElement3D*>& List, i3ClassMeta * pMeta)
{
	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement( i);

		if( pElm->kind_of( pMeta))
		{
			List.push_back( pElm);
		}
	}
}

void i3LevelScene::FindElementByResource( i3::vector<i3LevelElement3D*>& List, i3LevelRes * pRes)
{
	INT32 i;
	i3LevelElement3D * pElm;

//	pList->SetOnceMode( TRUE);		// push_back_once사용
	
	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement( i);

		if( pElm->getRes() == pRes)
			i3::vu::push_back_once(List, pElm);
	}
}

void i3LevelScene::GetPersistElement( i3::vector<i3LevelElement3D*>& List)
{
	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement( i);

		if( pElm->IsStyle( I3_LEVEL_STYLE_VOLATILE) == FALSE)
			List.push_back( pElm);
	}
}

void i3LevelScene::GetUniqueElementName( const char * pszPrefix, char * pszStr)
{
	INT32 i = 0, len, idx;
	char szPrefix[ 256];
	
	// Prefix의 뒤에 번호가 붙어있다면,
	// 번호를 제외한 부분만 Prefix로 사용한다.
	{
		len = strlen( pszPrefix);
		char * pszNumeric = (char *) pszPrefix + len - 1;

		while( (*pszNumeric >= '0') && (*pszNumeric <= '9'))
			pszNumeric--;

		pszNumeric++;

		i = atoi( pszNumeric);

		// Numeric이 아닌 부분까지만 Prefix로 복사
		for( idx = 0; &(pszPrefix[idx]) < pszNumeric; idx++)
		{
			szPrefix[idx] = pszPrefix[idx];
		}

		szPrefix[idx] = 0;
	}

	do
	{
		sprintf( pszStr, "%s%d", szPrefix, i);
		i++;
	}
	while( FindElementByName( pszStr) != -1);
}

void i3LevelScene::GetUniqueGeneratedResName( const char * pszPrefix, const char * pszExt, char * pszPath, INT32 lenPath, char * pszName, INT32 lenName)
{
	INT32 i = 0, len, idx;
	char szPrefix[ 256];
	
	// Prefix의 뒤에 번호가 붙어있다면,
	// 번호를 제외한 부분만 Prefix로 사용한다.
	{
		len = strlen( pszPrefix);
		char * pszNumeric = (char *) pszPrefix + len - 1;

		while( (*pszNumeric >= '0') && (*pszNumeric <= '9'))
			pszNumeric--;

		pszNumeric++;

		i = atoi( pszNumeric);

		// Numeric이 아닌 부분까지만 Prefix로 복사
		for( idx = 0; &(pszPrefix[idx]) < pszNumeric; idx++)
		{
			szPrefix[idx] = pszPrefix[idx];
		}

		szPrefix[idx] = 0;
	}

	do
	{
		i3::snprintf( pszName, lenName, "%s%d", szPrefix, i);
		i3::snprintf( pszPath, lenPath, "Scene/%s/_Generated/%s.%s", GetName(), pszName, pszExt);
		i++;
	} while( g_pResDB->FindResByPath( pszPath) != nullptr);
}

void i3LevelScene::SelectElement( i3LevelElement3D * pElm)
{
	i3LevelGroup * pGroup = pElm->getGroup();

	if( pGroup == nullptr)
	{
		pElm->Select();
	}
	else
	{
		pGroup->Select();
	}
}

void i3LevelScene::UnselectElement( i3LevelElement3D * pElm)
{
	i3LevelGroup * pGroup = pElm->getGroup();

	if( pGroup == nullptr)
		pElm->Unselect();
	else
		pGroup->Unselect();
}

void i3LevelScene::UnselectAll(void)
{
	INT32 i;
	i3LevelElement3D * pElm;

	i3TDK::SetPendingUpdateState( true);

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement(i);

		pElm->Unselect();
	}

	i3TDK::SetPendingUpdateState( false);

	i3Level::Update( nullptr, I3_TDK_UPDATE_UNSELECT, nullptr, i3LevelElement3D::static_meta());
}

void i3LevelScene::GetSelectedElements( i3::vector<i3LevelElement3D*>& List)
{
	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement(i);

		if( pElm->IsSelected())
			List.push_back( pElm);
	}
}

void i3LevelScene::TargetElement( i3LevelElement3D * pElm)
{
	pElm->Targeted();
}

void i3LevelScene::UntargetElement( i3LevelElement3D * pElm)
{
	pElm->Untargeted();
}

void i3LevelScene::UntargetAll(void)
{
	INT32 i;
	i3LevelElement3D * pElm;

	i3TDK::SetPendingUpdateState( true);

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement(i);

		pElm->Untargeted();
	}

	i3TDK::SetPendingUpdateState( false);
}

void i3LevelScene::GetTargetedElements( i3::vector<i3LevelElement3D*>& List)
{
	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement(i);

		if( pElm->IsTargeted())
			List.push_back( pElm);
	}
}

void i3LevelScene::UnfreezeAll(void)
{
	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement(i);

		if( pElm->IsFreezed())
		{
			pElm->Unfreeze();

			i3Level::Update( nullptr, I3_TDK_UPDATE_FREEZE, pElm);
		}
	}
}

void i3LevelScene::FreezeSelection(void)
{
	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement(i);

		if( pElm->IsSelected())
		{
			pElm->Freeze();
			i3Level::Update( nullptr, I3_TDK_UPDATE_FREEZE, pElm);
		}
	}
}

void i3LevelScene::SaveMatrix(void)
{
	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement( i);

		if( pElm->IsSelected() == FALSE)
			continue;

		pElm->SaveMatrix();
	}
}

void i3LevelScene::RestoreMatrix(void)
{
	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement( i);

		if( pElm->IsSelected() == FALSE)
			continue;

		pElm->RestoreMatrix();
	}
}

bool	i3LevelScene::GetWrappingBoundBoxForSelect( VEC3D * pMin, VEC3D * pMax)
{	
	i3Vector::Set( pMin, 0.0f, 0.0f, 0.0f);
	i3Vector::Set( pMax, 0.0f, 0.0f, 0.0f);

	bool bFirst = true;	

	if( m_ElementList.empty() )
	{
		return false;
	}

	VEC3D minVec, maxVec;

	for each( i3LevelElement3D* pElm in m_ElementList )
	{
		if( FALSE == pElm->IsSelected() )
		{
			continue;
		}

		pElm->GetTransformedBoundBox( &minVec, &maxVec);

		if( bFirst)
		{
			i3Vector::Copy( pMin, &minVec);
			i3Vector::Copy( pMax, &maxVec);
			bFirst = false;
		}
		else
		{
			i3Vector::Minimize( pMin, pMin, &minVec);
			i3Vector::Maximize( pMax, pMax, &maxVec);
		}
	}

	return !bFirst;

	/*i3LevelViewport * pView;
	i3LevelFramework * pFramework;

	pView = i3LevelViewport::GetCurrentFocusViewport();
	pFramework = pView->getFramework();

	switch( pFramework->_GetCtrlMode() )
	{
	case	i3LevelFramework::CTRL_MODE_NAVMESH_POINT_SELECT:
	case	i3LevelFramework::CTRL_MODE_NAVMESH_POINT_MOVE:
			{
				i3NavMesh* pNav				= i3Level::GetNavMesh();
				i3NavMeshDataMgr* dataMgr	= pNav->GetDataMgr();

				ts::vector< int > selList;				
				pNav->GetSelectedVertexList( selList );

				if( false == selList.empty() )
				{
					VEC3D v;

					for each( int idx in selList )
					{
						i3Vector::Copy( &v, dataMgr->GetVec3D( idx ) );
						i3Vector::Add( &v, pNav->GetVertexMovePoint(), &v);

						if( bFirst )
						{
							i3Vector::Copy( pMin, &v );
							i3Vector::Copy( pMax, &v );
							bFirst = false;
						}
						else
						{
							i3Vector::Minimize( pMin, pMin, &v );
							i3Vector::Maximize( pMax, pMax, &v );
						}
					}

					return true;
				}
			}
			break;

	case	i3LevelFramework::CTRL_MODE_NAVMESH_POLYGON_SELECT:
	case	i3LevelFramework::CTRL_MODE_NAVMESH_POLYGON_MOVE:
			break;

	case	i3LevelFramework::CTRL_MODE_NAVMESH_POINT_ADD:
			break;

	default:
		{
			if( false == m_ElementList.empty() )
			{
				VEC3D minVec, maxVec;

				for each( i3LevelElement3D* pElm in m_ElementList )
				{
					if( FALSE == pElm->IsSelected() )
					{
						continue;
					}

					pElm->GetTransformedBoundBox( &minVec, &maxVec);

					if( bFirst)
					{
						i3Vector::Copy( pMin, &minVec);
						i3Vector::Copy( pMax, &maxVec);
						bFirst = false;
					}
					else
					{
						i3Vector::Minimize( pMin, pMin, &minVec);
						i3Vector::Maximize( pMax, pMax, &maxVec);
					}
				}

				return !bFirst;
			}
		}
		break;
	}

	return false;*/
}

void i3LevelScene::GetWrappingBoundBox( VEC3D * pMin, VEC3D * pMax)
{
	INT32 i;
	i3LevelElement3D * pElm;
	bool bFirst = true;
	VEC3D minVec, maxVec;

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement( i);

		pElm->GetTransformedBoundBox( &minVec, &maxVec);

		if( bFirst)
		{
			i3Vector::Copy( pMin, &minVec);
			i3Vector::Copy( pMax, &maxVec);
			bFirst = false;
		}
		else
		{
			i3Vector::Minimize( pMin, pMin, &minVec);
			i3Vector::Maximize( pMax, pMax, &maxVec);
		}
	}
}

void i3LevelScene::Moving( I3LV_SPACE::TYPE space, REAL32 x, REAL32 y, REAL32 z, bool bSnap)
{
	INT32 i;
	i3LevelElement3D * pElm;
	REAL32 snap = 0.0f;

	if( bSnap)
		snap = m_Snap_Pos;

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement( i);

		if( pElm->IsSelected() == FALSE)
			continue;

		pElm->Moving( space, x, y, z, snap);

		i3Level::Update( nullptr, I3_TDK_UPDATE_EDIT | I3_TDK_UPDATE_MOVE, pElm);
	}
}

void i3LevelScene::Moving( I3LV_SPACE::TYPE space, POINT orgPt, POINT newPt, VEC3D * pMask, bool bSnap)
{
	VEC3D minVec, maxVec, midVec;
	MATRIX viewproj;
	VEC3D vec1, vec2;
	VEC3D pos1, pos2, pos3;
	REAL32 x, y, z;
	i3LevelViewport * pView;
	i3LevelFramework * pFramework;

	pView = i3LevelViewport::GetCurrentFocusViewport();
	pFramework = pView->getFramework();

	// 선택한 Unit 전체를 포함하는 Bound Box를 계산한다.
	// 2D 상에서 이동한 거리를 3D로 환산하는 단계에서는 Z 값에 따라
	// 매우 다른 이동값이 나올 수 있기 때문에 기준 Z 값이 필요하다.
	// 따라서 전체를 포괄하는 Bound Box의 중심점을 기준으로 3D-World Space에서의
	// 이동량을 결정한다.
	GetWrappingBoundBoxForSelect( &minVec, &maxVec);
	i3Vector::Add( &midVec, &minVec, &maxVec);
	i3Vector::Scale( &midVec, &midVec, 0.5f);

	{
		REAL32 rw, rh, hw, hh;

		pFramework->GetViewProjectionMatrix( & viewproj);

		i3Matrix::TransformPoints( &pos3, &midVec, 1, &viewproj);

		i3Matrix::Inverse( &viewproj, nullptr, &viewproj);

		hw = (REAL32)(pView->GetViewWidth() >> 1);
		hh = (REAL32)(pView->GetViewHeight() >> 1);

		rw = 1.0f / hw;
		rh = -1.0f / hh;

		// Clipping Space에서의 시작점과 끝점을 설정한다.
		i3Vector::Set( &vec1, (orgPt.x - hw) * rw, (orgPt.y - hh) * rh, i3Vector::GetZ( &pos3));
		i3Vector::Set( &vec2, (newPt.x - hw) * rw, (newPt.y - hh) * rh, i3Vector::GetZ( &pos3));

		i3Matrix::TransformPoints( &pos1, &vec1, 1, &viewproj);
		i3Matrix::TransformPoints( &pos2, &vec2, 1, &viewproj);
		i3Vector::Sub( &pos1, &pos2, &pos1);

		if( pMask != nullptr)
            i3Vector::Mul( &pos1, &pos1, pMask);

		x = i3Vector::GetX( &pos1);
		y = i3Vector::GetY( &pos1);
		z = i3Vector::GetZ( &pos1);
	}

	Moving( space, x, y, z, bSnap);
}

void i3LevelScene::NavMoving( POINT orgPt, POINT newPt, VEC3D * pMask, bool bSnap)
{
	MATRIX viewproj;

	VEC3D midVec;	
	VEC3D vec1, vec2;
	VEC3D pos1, pos2, pos3;

	i3LevelViewport * pView			= i3LevelViewport::GetCurrentFocusViewport();
	i3LevelFramework * pFramework	= pView->getFramework();

	{
		switch( pFramework->GetLevelEditMode() )
		{
		case	LEM_NAVMESH_POINT:
				i3Level::GetNavMesh()->GetSelectedCenterPosition( true, midVec );
				break;

		case	LEM_NAVMESH_POLYGON:
				i3Level::GetNavMesh()->GetSelectedCenterPosition( false, midVec );
				break;

		default:
			i3Vector::Set( &midVec, 0,0,0 );
			break;
		}


		REAL32 rw, rh, hw, hh;

		pFramework->GetViewProjectionMatrix( &viewproj );

		i3Matrix::TransformPoints( &pos3, &midVec, 1, &viewproj);
		i3Matrix::Inverse( &viewproj, nullptr, &viewproj );

		hw = (REAL32)(pView->GetViewWidth() >> 1);
		hh = (REAL32)(pView->GetViewHeight() >> 1);

		rw = 1.0f / hw;
		rh = -1.0f / hh;

		// Clipping Space에서의 시작점과 끝점을 설정한다.
		i3Vector::Set( &vec1, (orgPt.x - hw) * rw, (orgPt.y - hh) * rh, i3Vector::GetZ( &pos3));
		i3Vector::Set( &vec2, (newPt.x - hw) * rw, (newPt.y - hh) * rh, i3Vector::GetZ( &pos3));

		i3Matrix::TransformPoints( &pos1, &vec1, 1, &viewproj);
		i3Matrix::TransformPoints( &pos2, &vec2, 1, &viewproj);
		i3Vector::Sub( &pos1, &pos2, &pos1);

		if( pMask != nullptr)
			i3Vector::Mul( &pos1, &pos1, pMask);

	}

	switch( pFramework->GetLevelEditMode() )
	{
	case	LEM_NAVMESH_POINT:
			i3Level::GetNavMesh()->SetVertexMovePosition( &pos1 );
			break;

	case	LEM_NAVMESH_POLYGON:
			i3Level::GetNavMesh()->SetPolygonMovePosition( &pos1 );
			break;
	}
	
	pFramework->OnNavSelect();
}

void i3LevelScene::Rotating( I3LV_SPACE::TYPE space, VEC3D * pWSStart, VEC3D * pWSEnd, VEC3D * pMask, bool bSnap)
{
	VEC3D vec1;

	{
		i3Vector::Sub( &vec1, pWSEnd, pWSStart);
		i3Vector::Mul( &vec1, &vec1, pMask);
		i3Vector::Scale( &vec1, &vec1, 3.0f);
	}

	Rotating( space, vec1.x, vec1.y, vec1.z, bSnap);
}

void i3LevelScene::Rotating( I3LV_SPACE::TYPE space, REAL32 x, REAL32 y, REAL32 z, bool bSnap)
{
	INT32 i;
	i3LevelElement3D * pElm;
	REAL32 snap = 0.0f;

	if( bSnap)
		snap = m_Snap_Rotate;

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement( i);

		if( pElm->IsSelected() == FALSE)
			continue;

		pElm->Rotating( space, x, y, z, snap);

		i3Level::Update( nullptr, I3_TDK_UPDATE_EDIT | I3_TDK_UPDATE_ROTATE, pElm);
	}
}

void i3LevelScene::Scaling( I3LV_SPACE::TYPE space, POINT orgPt, POINT newPt, VEC3D * pMask, bool bSnap)
{
	VEC3D minVec, maxVec, midVec;
	MATRIX viewproj;
	VEC3D vec1, vec2;
	VEC3D pos1, pos2, pos3;
	REAL32 x, y, z;
	i3LevelViewport * pView;
	i3LevelFramework * pFramework;

	pView = i3LevelViewport::GetCurrentFocusViewport();
	pFramework = pView->getFramework();

	// 선택한 Unit 전체를 포함하는 Bound Box를 계산한다.
	// 2D 상에서 이동한 거리를 3D로 환산하는 단계에서는 Z 값에 따라
	// 매우 다른 이동값이 나올 수 있기 때문에 기준 Z 값이 필요하다.
	// 따라서 전체를 포괄하는 Bound Box의 중심점을 기준으로 3D-World Space에서의
	// 이동량을 결정한다.
	GetWrappingBoundBoxForSelect( &minVec, &maxVec);
	i3Vector::Add( &midVec, &minVec, &maxVec);
	i3Vector::Scale( &midVec, &midVec, 0.5f);

	{
		REAL32 rw, rh, hw, hh;

		pFramework->GetViewProjectionMatrix( & viewproj);

		i3Matrix::TransformPoints( &pos3, &midVec, 1, &viewproj);

		i3Matrix::Inverse( &viewproj, nullptr, &viewproj);

		hw = (REAL32)(pView->GetViewWidth() >> 1);
		hh = (REAL32)(pView->GetViewHeight() >> 1);

		rw = 1.0f / hw;
		rh = -1.0f / hh;

		// Clipping Space에서의 시작점과 끝점을 설정한다.
		i3Vector::Set( &vec1, (orgPt.x - hw) * rw, (orgPt.y - hh) * rh, i3Vector::GetZ( &pos3));
		i3Vector::Set( &vec2, (newPt.x - hw) * rw, (newPt.y - hh) * rh, i3Vector::GetZ( &pos3));

		i3Matrix::TransformPoints( &pos1, &vec1, 1, &viewproj);
		i3Matrix::TransformPoints( &pos2, &vec2, 1, &viewproj);
		i3Vector::Sub( &pos1, &pos2, &pos1);

		if( pMask != nullptr)
            i3Vector::Mul( &pos1, &pos1, pMask);

		x = i3Vector::GetX( &pos1);
		y = i3Vector::GetY( &pos1);
		z = -i3Vector::GetZ( &pos1);
	}

	Scaling( space, x, y, z, bSnap);
}

void i3LevelScene::Scaling( I3LV_SPACE::TYPE space, REAL32 x, REAL32 y, REAL32 z, bool bSnap)
{
	INT32 i;
	i3LevelElement3D * pElm;
	REAL32 snap = 0.0f;

	if( bSnap)
		snap = m_Snap_Scale;

	I3ASSERT( i3Math::isValid( x));
	I3ASSERT( i3Math::isValid( y));
	I3ASSERT( i3Math::isValid( z));

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement( i);

		if( pElm->IsSelected() == FALSE)
			continue;

		pElm->Scaling( space, x, y, z, snap);

		i3Level::Update( nullptr, I3_TDK_UPDATE_EDIT | I3_TDK_UPDATE_SCALE, pElm, nullptr);
	}
}

static REAL32 SNAP( REAL32 val, REAL32 snap)
{
	INT32 div = (INT32)( val / snap);

	return div * snap;
}

void i3LevelScene::GripDrag( INT32 idxGrip, POINT orgPt, POINT newPt, bool bSnap)
{
	VEC3D minVec, maxVec, midVec;
	MATRIX viewproj;
	VEC3D vec1, vec2, move, scale;
	VEC3D pos1, pos2, pos3;
	i3LevelViewport * pView;
	i3LevelFramework * pFramework;
	VEC3D maskMove, maskScale;

	switch( idxGrip)
	{
		case 0 :	// X축
			maskMove = VEC3D( 1.0f, 0.0f, 0.0f);
			maskScale = VEC3D( -1.0f, 0.0f, 0.0f);
			break;		

		case 1 :	// X축
			maskMove = VEC3D( 1.0f, 0.0f, 0.0f);
			maskScale = VEC3D( 1.0f, 0.0f, 0.0f);
			break;

		case 2 :	// Y축
			maskMove = VEC3D( 0.0f, 1.0f, 0.0f);
			maskScale = VEC3D( 0.0f, -1.0f, 0.0f);
			break;

		case 3 :	// Y축
			maskMove = VEC3D( 0.0f, 1.0f, 0.0f);
			maskScale = VEC3D( 0.0f, 1.0f, 0.0f);
			break;

		case 4 :
			maskMove = VEC3D( 0.0f, 0.0f, 1.0f);
			maskScale = VEC3D( 0.0f, 0.0f, -1.0f);
			break;		// Z축

		case 5 :
			maskMove = VEC3D( 0.0f, 0.0f, 1.0f);
			maskScale = VEC3D( 0.0f, 0.0f, 1.0f);
			break;
	}

	pView = i3LevelViewport::GetCurrentFocusViewport();
	pFramework = pView->getFramework();

	// 선택한 Unit 전체를 포함하는 Bound Box를 계산한다.
	// 2D 상에서 이동한 거리를 3D로 환산하는 단계에서는 Z 값에 따라
	// 매우 다른 이동값이 나올 수 있기 때문에 기준 Z 값이 필요하다.
	// 따라서 전체를 포괄하는 Bound Box의 중심점을 기준으로 3D-World Space에서의
	// 이동량을 결정한다.
	GetWrappingBoundBoxForSelect( &minVec, &maxVec);
	i3Vector::Add( &midVec, &minVec, &maxVec);
	i3Vector::Scale( &midVec, &midVec, 0.5f);

	{
		REAL32 rw, rh, hw, hh;

		pFramework->GetViewProjectionMatrix( & viewproj);

		i3Matrix::TransformPoints( &pos3, &midVec, 1, &viewproj);

		i3Matrix::Inverse( &viewproj, nullptr, &viewproj);

		hw = (REAL32)(pView->GetViewWidth() >> 1);
		hh = (REAL32)(pView->GetViewHeight() >> 1);

		rw = 1.0f / hw;
		rh = -1.0f / hh;

		// Clipping Space에서의 시작점과 끝점을 설정한다.
		i3Vector::Set( &vec1, (orgPt.x - hw) * rw, (orgPt.y - hh) * rh, i3Vector::GetZ( &pos3));
		i3Vector::Set( &vec2, (newPt.x - hw) * rw, (newPt.y - hh) * rh, i3Vector::GetZ( &pos3));

		i3Matrix::TransformPoints( &pos1, &vec1, 1, &viewproj);
		i3Matrix::TransformPoints( &pos2, &vec2, 1, &viewproj);
		i3Vector::Sub( &pos1, &pos2, &pos1);

		pos1.x = SNAP( pos1.x, m_Snap_Scale);
		pos1.y = SNAP( pos1.y, m_Snap_Scale);
		pos1.z = SNAP( pos1.z, m_Snap_Scale);

		i3Vector::Mul( &move, &pos1, &maskMove);
		i3Vector::Mul( &scale, &pos1, &maskScale);

		i3Vector::Scale( &move, &move, 0.5f);
		i3Vector::Scale( &scale, &scale, 0.25f);
	}

	REAL32 snap = 0.0f;

	if( bSnap)
		snap = m_Snap_Scale;

	for( INT32 i = 0; i < GetElementCount(); i++)
	{
		i3LevelElement3D * pElm = GetElement( i);

		if( pElm->IsSelected() == FALSE)
			continue;

		pElm->GripDrag( idxGrip, scale, snap);

		i3Level::Update( nullptr, I3_TDK_UPDATE_EDIT | I3_TDK_UPDATE_SCALE, pElm, nullptr);
	}
}

i3LevelElement3D * i3LevelScene::GetPickedElement( POINT point, bool bSelect, SEL_MODE mode)
{
	INT32 i, hw, hh;
	i3LevelElement3D * pElm, * pPickedElm = nullptr;
	VEC3D lineWS[2], iline[2];
	MATRIX inv, view_project, tmp;
	i3LevelViewport * pView;
	REAL32 minT = 10000.0f;
	i3LevelFramework * pFramework;

	pView = i3LevelViewport::GetCurrentFocusViewport();
	pFramework = pView->getFramework();

	{
		REAL32 rw, rh;

		pFramework->GetViewProjectionMatrix( &view_project);

		// 화면 한가운데를 ( 0, 0)으로 간주한 Clipping-Space로의 좌표 변환용.
		hw = pView->GetViewWidth() >> 1;
		hh = pView->GetViewHeight() >> 1;

		rw = 1.0f / hw;
		rh = -1.0f / hh;

		// Clipping Space에서의 시작점과 끝점을 설정한다.
		i3Vector::Set( &lineWS[0], (point.x - hw) * rw, (point.y - hh) * rh, 0.0f);
		i3Vector::Set( &lineWS[1], (point.x - hw) * rw, (point.y - hh) * rh, 1.0f);
	}

	bool bFirst = true;

	for( i = 0; i < GetElementCount(); i++)
	{
		i3SceneObject * pSceneObj;

		pElm = GetElement(i);

		if( pElm->IsFreezed())
			continue;

		if( pElm->IsEnable() == FALSE)
			continue;

		pSceneObj = pElm->GetSceneObject();

		// 시작점과 끝점을 Object Local-Space로 변환하기 위한 Matrix
		i3Matrix::Mul( &tmp, pSceneObj->GetMatrix(), &view_project);
		i3Matrix::Inverse( &inv, nullptr, &tmp);

   		i3Vector::TransformCoord( &iline[0], &lineWS[0], &inv);
		i3Vector::TransformCoord( &iline[1], &lineWS[1], &inv);

		if( ContainAABBoxLine( &iline[0], &iline[1], pElm->GetBoundMin(), pElm->GetBoundMax(), 100000.0f))
		{
			REAL32 t;

			if( pElm->IsStyle( I3_LEVEL_STYLE_TOUCH_BBOXONLY) == FALSE)
			{
				t = pElm->OnTouch( &lineWS[0], &lineWS[1], &view_project, true, bSelect, mode);
			}
			else
			{
				VEC3D center, p;

				i3Vector::Add( &center, pElm->GetBoundMin(), pElm->GetBoundMax());
				i3Vector::Scale( &center, &center, 0.5f);

				t = FLT_MAX;

				bool res = i3Math::GetPerpendicularPoint( &iline[0], &iline[1], &center, &p, &t);

				if (res == FALSE && t == FLT_MAX) // 이경우에 t가 기록되지 않기 때문에 버그라서 일단 이렇게 구분..
				{
					continue;						// 아래 조건문 구현들 스킵하기 위해 컨티뉴처리..
				}

			}

			if( t > 0.0f)
			{
				if( bFirst == true)
				{
					minT = t;
					pPickedElm = pElm;
					bFirst = false;
				}
				else
				{
					if( t < minT)
					{
						minT = t;
						pPickedElm = pElm;
					}
				}
			}
		}
	}

	return pPickedElm;
}

void i3LevelScene::GetBandedElements( i3LevelViewport * pView, POINT pt1, POINT pt2, i3::vector<i3LevelElement3D*>& List, bool bSelect, SEL_MODE mode)
{
	INT32 i, hw, hh;
	i3LevelElement3D * pElm;
	VEC3D tmin, tmax;
	MATRIX view_project;
	i3::pack::RECT src, dest;
	i3LevelFramework * pFramework = pView->getFramework();

	pFramework->GetViewProjectionMatrix( &view_project);

	{
		REAL32 rw, rh;
		REAL32 x1, y1, x2, y2;

		// 화면 한가운데를 ( 0, 0)으로 간주한 Clipping-Space로의 좌표 변환용.
		hw = pView->GetViewWidth() >> 1;
		hh = pView->GetViewHeight() >> 1;

		rw = 1.0f / hw;
		rh = -1.0f / hh;

		// Clipping Space에서의 시작점과 끝점을 설정한다.
		x1	=	(pt1.x - hw) * rw;
		y1		=	(pt1.y - hh) * rh;
		x2	=	(pt2.x - hw) * rw;
		y2	=	(pt2.y - hh) * rh;

		src.left = MIN( x1, x2);
		src.top = MIN( y1, y2);
		src.right = MAX( x1, x2);
		src.bottom = MAX( y1, y2);
	}

	for( i = 0; i < GetElementCount(); i++)
	{
		bool bHit;

		pElm = GetElement(i);

		bHit = false;
		if( pElm == nullptr)
			continue;

		if( pElm->IsFreezed())
			continue;

		if( pElm->IsEnable() == FALSE)
			continue;

		// Full Transform Matrix를 구한다.
		pElm->GetTransformedBoundBox( &tmin, &tmax);

		{
			VEC3D bb[8];
			i3BoundBox bbox;
			INT32 j;

			bbox.SetMinMax( &tmin, &tmax);

			bbox.GetCornerVertices( bb);

			i3Matrix::TransformPoints( bb, bb, 8, &view_project);

			i3Vector::Copy( &tmin,  &bb[0]);
			i3Vector::Copy( &tmax,  &bb[0]);
			for( j = 1; j < 8; j++)
			{
				i3Vector::Minimize( &tmin, &tmin, &bb[j]);
				i3Vector::Maximize( &tmax, &tmax, &bb[j]);
			}
		}

		dest.left	= MIN( i3Vector::GetX( &tmin), i3Vector::GetX( &tmax));
		dest.top	= MIN( i3Vector::GetY( &tmin), i3Vector::GetY( &tmax));
		dest.right	= MAX( i3Vector::GetX( &tmin), i3Vector::GetX( &tmax));
		dest.bottom	= MAX( i3Vector::GetY( &tmin), i3Vector::GetY( &tmax));

		if( i3Math::RectInRect( &dest, &src))
			bHit = true;
		else if( i3Math::RectInRect( &src, &dest))
			bHit = true;

		if( bHit)
		{
			/*
			// OnTouch 함수가 내부에서 직선으로 Picking을 확인하는 방식인데,
			// 전달되는 선은 View 방향으로 관통하는 식의 직선이 주어져야 정상동작이 가능.
			// OnTouch의 내부에서 Triangle 단위로 Picking 검사하는 부분을 Mesh의 Local Space에서
			// 검사하도록 구현되어 있는데, 이것은 Screen-Space로 바꿀 경우, 정확한 Banding 검사는
			// 가능하지만, 상당량의 Triangle을 Transform하는데 부담이 된다고 판단하여
			// Banding 검사는 단순 Box 검사만으로 수정.
			//
			// 이진균
			//
			if( pElm->IsStyle( I3_LEVEL_STYLE_TOUCH_BBOXONLY) == FALSE)
			{
				VEC3D s, e;

				i3Vector::Set( &s, src.left, src.top, 0.0f);
				i3Vector::Set( &e, src.right, src.bottom, 0.0f);

				if( pElm->OnTouch( &s, &e, &view_project, false, bSelect, mode) > 0.0f)
				{
					pList->Add( pElm);
				}
			}
			else
			{
			*/
				List.push_back( pElm);
			//}
		}
	}
}

// CLevelEditorDoc 명령
void i3LevelScene::Pick( POINT point, PICK_TYPE pickType, SEL_MODE mode)
{
	i3LevelElement3D * pSel;

	switch( pickType)
	{
		case PICK_SELECT :
			if( mode == SEL_EXCLUSIVE)
				UnselectAll();

			pSel = GetPickedElement( point, true, mode);
			break;

		case PICK_TARGET :
			if( mode == SEL_EXCLUSIVE)
				UntargetAll();

			pSel = GetPickedElement( point, true, mode);
			break;

		default :
			pSel = GetPickedElement( point, false, mode);
			break;
	}

	if( pSel != nullptr)
	{
		switch( pickType)
		{
			case PICK_SELECT :
				switch( mode)
				{
					case SEL_ALTERNATIVE :
						if( pSel->IsSelected())
							UnselectElement( pSel);
						else
							SelectElement( pSel);
						break;

					default :
						SelectElement( pSel);
						break;
				}
				break;

			case PICK_UNSELECT :
				UnselectElement( pSel);
				break;

			case PICK_TARGET :
				switch( mode)
				{
					case SEL_ALTERNATIVE :
						if( pSel->IsTargeted())
							UntargetElement( pSel);
						else
							TargetElement( pSel);
						break;

					default :
						TargetElement( pSel);
						break;
				}
				break;

			case PICK_UNTARGET :
				UntargetElement( pSel);
				break;
		}
	}
}

void i3LevelScene::PickBand( i3LevelViewport * pView, POINT pt1, POINT pt2, PICK_TYPE pickType, SEL_MODE mode)
{
	i3::vector<i3LevelElement3D*> TempList;
	i3LevelElement3D * pElm;

	if((pickType == PICK_SELECT) || (pickType == PICK_TARGET))
		GetBandedElements( pView, pt1, pt2, TempList, true, mode);
	else
		GetBandedElements( pView, pt1, pt2, TempList, false, mode);

	if( TempList.size() > 1)
	{
		i3TDK::SetPendingUpdateState( true);
	}

	if((pickType == PICK_SELECT) && (mode == SEL_EXCLUSIVE))
		UnselectAll();

	if((pickType == PICK_TARGET) && (mode == SEL_EXCLUSIVE))
		UntargetAll();

	for(size_t i = 0; i < TempList.size(); i++)
	{
		pElm = TempList[i];

		if( pElm->IsStyle( I3_LEVEL_STYLE_UNSELECT))
			continue;

		switch( pickType)
		{
			case PICK_SELECT :
				switch( mode)
				{
					case SEL_ALTERNATIVE :
						if( pElm->IsSelected())
							UnselectElement( pElm);
						else
							SelectElement( pElm);
						break;

					case SEL_EXCLUSIVE :				
					case SEL_INCLUSIVE :
						SelectElement( pElm);
						break;
				}
				break;

			case PICK_UNSELECT :
				UnselectElement( pElm);
				break;

			case PICK_TARGET :
				switch( mode)
				{
					case SEL_ALTERNATIVE :
						if( pElm->IsTargeted())
							UntargetElement( pElm);
						else
							TargetElement( pElm);
						break;

					case SEL_EXCLUSIVE :				
					case SEL_INCLUSIVE :
						TargetElement( pElm);
						break;
				}
				break;

			case PICK_UNTARGET :
				UntargetElement( pElm);
				break;
		}
	}

	if( TempList.size() > 1)
	{
		i3TDK::SetPendingUpdateState( false);

		if( pickType == PICK_UNSELECT)
		{
			i3TDK::Update( nullptr, I3_TDK_UPDATE_UNSELECT, nullptr, i3LevelElement3D::static_meta());
		}
		else if( pickType == PICK_SELECT)
		{
			i3TDK::Update( nullptr, I3_TDK_UPDATE_SELECT, nullptr, i3LevelElement3D::static_meta());
		}
	}
}

void i3LevelScene::CopyClipboard( HWND hwnd )
{
	i3TDKClipboard clip;

	i3LevelElementList * pElmList = i3LevelElementList::new_object();
	i3::vector<i3LevelElement3D*> list;

	GetSelectedElements( list);

	for( size_t i = 0; i < list.size(); i++)
	{
		i3LevelElement * pElm = list[i];

		pElmList->add( pElm);
	}

	clip.Copy( hwnd, pElmList);

	I3_MUST_RELEASE( pElmList);
}

void i3LevelScene::PasteClipboard( HWND hwnd, CPoint * ptScreen)
{
	i3LevelElementList * pElmList;
	i3TDKClipboard clip;
	char szName[256];
	bool bMousePos = false;
	VEC3D newPos, basePos, diff;

	{
		//hitmap은 copy 사용하지 않음
		i3::vector<i3LevelElement3D*> SelList;

		GetSelectedElements(SelList);
		for (int i = 0; i < (int)SelList.size(); i++)
		{
			if (i3::kind_of<i3LevelHitPoint*>(SelList[i]))
			{
				return;
			}
		}
	}

	if( clip.IsObject( i3LevelElementList::static_meta()) == false )
		return;

	pElmList = (i3LevelElementList *) clip.Paste( hwnd);
	if( pElmList == nullptr)
		return;

	INT32 i;
	i3LevelElement3D * pElm;

	UnselectAll();

	// 배치할 위치 계산
	if( ptScreen != nullptr)
	{
		CRect rt;

		ScreenToClient( hwnd, ptScreen);

		GetClientRect( hwnd, &rt);

		if( PtInRect( &rt, *ptScreen))
		{
			i3LevelFramework * pFramework = (i3LevelFramework *) i3Framework::getCurrentFramework();

			if( pFramework != nullptr)
			{
				pFramework->GetPickedPos( *ptScreen, &newPos);
				bMousePos = true;
			}
		}
		else
		{
			i3Vector::Set( &newPos, 0.0f, 0.0f, 0.0f);
		}
	}

	for( i = 0; i < pElmList->getCount(); i++)
	{
		pElm = (i3LevelElement3D *) pElmList->get( i);

		// 이름 조정
		this->GetUniqueElementName( pElm->GetName(), szName);

		pElm->SetName( szName);

		pElm->OnAfterPaste();

		AddElement( pElm, nullptr, TRUE);

		if( bMousePos)
		{
			// 마우스로 위치가 지정되었다.
			if( i == 0)
			{
				i3Vector::Copy( &basePos, pElm->getPos());
			}

			i3Vector::Sub( &diff, pElm->getPos(), &basePos);

			VEC3D temp;

			i3Vector::Add( &temp, &newPos, &diff);

			pElm->setPos( &temp);
		}
		else
		{
			i3Vector::Copy( &newPos, pElm->getPos());			

			pElm->setPos( &newPos);
		}

		SelectElement( (i3LevelElement3D *) pElm);
	}

	I3_MUST_RELEASE(pElmList);
}


void i3LevelScene::ReplaceResOfElement( i3LevelRes * pSrc, i3LevelRes * pDest)
{
	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement( i);

		if( pElm->getRes() == pSrc)
		{
			pElm->Detach();

			pElm->BindRes( pDest);

			_AttachToScene( pDest, pElm);
		}
	}
}

void i3LevelScene::PrepareSerialize(void)
{
	INT32 i;
	i3LevelElement3D * pElm;

	for( i = 0; i < GetElementCount(); i++)
	{
		pElm = GetElement( i);

		pElm->OnPrepareSerialize();
	}
}

REAL32 i3LevelScene::getSnappedValue_Position( REAL32 val) const
{
	return SNAP( val, m_Snap_Pos);
}

REAL32 i3LevelScene::getSnappedValue_Rotation( REAL32 rad) const
{
	REAL32 deg = I3_RAD2DEG( rad);

	deg = SNAP( deg, m_Snap_Rotate);

	return I3_DEG2RAD( deg);
}

REAL32 i3LevelScene::getSnappedValue_Scaling( REAL32 val) const
{
	return SNAP( val, m_Snap_Scale);
}
