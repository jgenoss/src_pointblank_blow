#include "i3FrameworkPCH.h"
#include "i3GameObj.h"
#include "i3GameRes.h"
#include "i3Game.h"

#include "i3ActionEmitEffect.h"
#include "i3ActionVisible.h"
#include "i3ActionDisable.h"
#include "i3ActionDynamicState.h"
#include "i3ActionPath.h"
#include "i3ActionPlayAnim.h"
#include "i3ActionSetWorldSection.h"

#include "i3Base/itl/bind/bind.h"
#include "i3Math/structure/aabb.h"
#include "i3Math/octree/occ_culler.h"
#include "i3Math/octree/octree.h"
#include "i3Math/octree/octree_item.h"

#include "i3Base/itl/range/algorithm/find_if.h"
#include "OccDebugInfo.h"

#pragma push_macro("min")
#undef min

#pragma push_macro("max")
#undef max

I3_CLASS_INSTANCE( i3GameObj);

i3GameObj::i3GameObj()
{
	m_pSceneObj->SetPreTraceCallback(i3::bind(&i3GameObj::OnPreTraceSceneObject, this));		//일단 Clear쪽 처리는 필요없을것 같다.
	SetNode( m_pSceneObj);

	{
		#if defined( I3_DEBUG)
			m_pFadeCtrl->SetName( "FadeCtrl");
		#endif

		// 기본 상태에서 Fade는 발생하지 않기때문에 무력화(?) 시켜둔다.
		m_pFadeCtrl->AddFlag( I3_NODEFLAG_NO_WORK);

		// 하위 Sg.에 MaterialAttr, BlendEnable 등의 Render Attribute가 있을 수 있기 때문에
		// 어떤 것보다 강력한 것으로 우선 순위를 설정해주지 않으면 효과가
		// 나타나지 않을 수 있다.
		m_pFadeCtrl->SetPriority( 0);

		m_pSceneObj->AddChild( m_pFadeCtrl);

		{
			m_pFadeMaterialAttr->SetDiffuse( 0.5f, 0.5f, 0.5f, 1.0f);
			m_pFadeMaterialAttr->SetSpecular( 0.5f, 0.5f, 0.5f, 1.0f);
			m_pFadeMaterialAttr->SetShininess( 64.0f);
			m_pFadeMaterialAttr->SetEmissive( 0.0f, 0.0f, 0.0f, 0.0f);
			m_pFadeMaterialAttr->setValidEmissive( false);
			m_pFadeMaterialAttr->setValidSpecular( false);
			m_pFadeCtrl->AddAttr( m_pFadeMaterialAttr);
		}

		{	
			m_pFadeBlendEnableAttr->Set(true);
			m_pFadeCtrl->AddAttr( m_pFadeBlendEnableAttr);
		}

		{
			m_pFadeBlendModeAttr->SetBlendOperation( I3G_BLEND_OP_ADD);
			m_pFadeBlendModeAttr->SetSource( I3G_BLEND_SRCALPHA);
			m_pFadeBlendModeAttr->SetDestination( I3G_BLEND_INVSRCALPHA);
			m_pFadeCtrl->AddAttr( m_pFadeBlendModeAttr);
		}
	}
}

void	i3GameObj::OnPreTraceSceneObject()
{
	if (!m_pRenderTransAABB)
		return;

	i3Framework* pFramework = i3Framework::getCurrentFramework();
	
	OccDebugInfo* pDebugOccInfo = pFramework->GetViewer()->getOccDebugInfo();

	if (pDebugOccInfo->IsAABBRenderEnabled() == false || 
		m_bEnableRenderAABB == false)
	{
		m_pRenderTransAABB->AddFlag(I3_NODEFLAG_DISABLE);
		return;
	}

	// m_pRenderTransAABB는 PreTrace에서 스킵되므로 여기서 처리해도 좋을듯..

	const i3::aabb* resultAABB = m_pSceneObj->GetAABB();
				
	if (resultAABB == nullptr || resultAABB->is_valid() == false)
	{
		m_pRenderTransAABB->AddFlag(I3_NODEFLAG_DISABLE);
		return;
	}
		
	m_pRenderTransAABB->RemoveFlag(I3_NODEFLAG_DISABLE);
		
	i3::vec3 vecCenter;		resultAABB->calc_center(vecCenter);
	i3::vec3 vecExtent;		resultAABB->calc_extent(vecExtent);

	MATRIX matWorldAABB;		// 이거를 그대로 쓰기가 좀 애매한 상태임...부모노드 역행렬을 곱해 로컬화시키자..
	i3Matrix::SetScale(&matWorldAABB, vecExtent.x * 2.f, vecExtent.y * 2.f, vecExtent.z * 2.f);
	i3Matrix::SetPos(&matWorldAABB, vecCenter.x, vecCenter.y, vecCenter.z);
		
	MATRIX* parentMat = m_pSceneObj->GetCacheMatrix();

	MATRIX invMatWorldParent;
	i3Matrix::Inverse(&invMatWorldParent, nullptr, parentMat);			// 부모 월드 역행렬...
	i3Matrix::Mul(&matWorldAABB, &matWorldAABB, &invMatWorldParent);
		
	m_pRenderTransAABB->SetMatrix(&matWorldAABB);				// 실제로는 부모노드 역행렬이 함께 반영됨..

	
}

namespace
{
	i3Transform*	CreateRenderableAABB(i3SceneObject* pSceneObj)
	{
		if (pSceneObj == nullptr)
			return nullptr;

		// 일단 노드 플래그(I3NODEFLAG_UPDATEAABB) 와 관계없이 생성해두도록 한다... 켜고 끌수 있는것에 대비..

		i3AttrSet* pAttrSetAABB = i3AttrSet::new_object_ref();

		pAttrSetAABB->AddFlag(I3_NODEFLAG_VOLATILE | I3_NODEFLAG_CONTROL |
			I3_NODEFLAG_TRANSPARENT | I3_NODEFLAG_DISABLE_SHADOW_CASTER | I3_NODEFLAG_DISABLE_SHADOW_RECEIVER);
		pAttrSetAABB->SetName("DEBUG_RenderableAABB");

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();
			pAttr->Set(false);
			pAttrSetAABB->AddAttr(pAttr);
		}

		{
			COLOR color;
			i3ColorAttr * pAttr = i3ColorAttr::new_object_ref();
			i3Color::Set(&color, (UINT8)64, 64, 255, 128);
			pAttr->Set(&color);
			pAttrSetAABB->AddAttr(pAttr);
		}

		{
			i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();
			pAttr->Set(false);
			pAttrSetAABB->AddAttr(pAttr);
		}

		{
			i3ZTestEnableAttr * pAttr = i3ZTestEnableAttr::new_object_ref();
			pAttr->Set(false);
			pAttrSetAABB->AddAttr(pAttr);
		}

		{
			i3ZBiasAttr * pAttr = i3ZBiasAttr::new_object_ref();
			pAttr->setBias(0.f);
			pAttrSetAABB->AddAttr(pAttr);
		}

		i3Node * pMesh = i3SceneUtil::BuildBoxMesh(1.0f, false);
		pAttrSetAABB->AddChild(pMesh);
		I3_SAFE_RELEASE(pMesh);

		i3Transform* pTrans = i3Transform::new_object_ref();
		pTrans->AddFlag(I3_NODEFLAG_VOLATILE);
	
		pTrans->AddChild(pAttrSetAABB);
		pSceneObj->AddChild(pTrans);

		return pTrans;
	}

	struct fnFindSection
	{
		INT32 _idx;
		fnFindSection(INT32 idx)
		{
			_idx = idx;
		}
		bool operator()(const ShadowSectionInfo* p) const { return p->_idx == _idx; }
	};
}

void	i3GameObj::EnableRenderAABB(bool bEnable)
{
	if (i3GameObj::IsEnabledUpdateAABB() == false)
		return;

	i3Framework* pFramework = i3Framework::getCurrentFramework();

	if (!pFramework)
		return;

	OccDebugInfo* pOccDebugInfo = pFramework->GetViewer()->getOccDebugInfo();
	if (pOccDebugInfo == nullptr)
		return;
		
	if (m_pRenderTransAABB == nullptr)
	{
		m_pRenderTransAABB = CreateRenderableAABB(m_pSceneObj);

		if (m_pRenderTransAABB == nullptr)
		{
			m_bEnableRenderAABB = bEnable;	return;
		}
	}
	else
	{
		if (m_bEnableRenderAABB == bEnable)		// 일단 생성된 이후에 같은 값이면 스킵...
			return;
	}

	m_bEnableRenderAABB = bEnable;

	if (bEnable)
	{
		m_pRenderTransAABB->RemoveFlag(I3_NODEFLAG_DISABLE);
	}
	else
	{
		m_pRenderTransAABB->AddFlag(I3_NODEFLAG_DISABLE);
	}
	
}

bool	i3GameObj::IsEnabledRenderAABB() const
{
	return m_bEnableRenderAABB;
}


i3GameObj::~i3GameObj()
{
	I3_SAFE_RELEASE( m_pSceneObj);
	DeleteAllSection();
}

void	i3GameObj::setPortalID( bool bAdd, INT32 id)
{
	if( bAdd)
	{
		//if( m_idxPortal != id)	m_cntPortal++;
		//I3TRACE("ENTER >> %d, cnt: %d\n", id, m_cntPortal);
		m_idxPortal = id;
	}
	else
	{
		//m_cntPortal--;
		if(m_cntPortal == id)
		{
			m_idxPortal = 0;
		}

		//I3TRACE("LEAVE << %d, cnt: %d\n", id, m_cntPortal);
	}
}

void	i3GameObj::SetInstanceSg( i3Node * pNode)		// Internal use only; GameRes에서 호출되는 함수.
{
	m_pFadeCtrl->AddChild( pNode);

	if( pNode->IsFlag( I3_NODEFLAG_TRANSPARENT))
	{
		m_pFadeCtrl->AddFlag( I3_NODEFLAG_TRANSPARENT);
		m_pSceneObj->AddFlag( I3_NODEFLAG_TRANSPARENT);
	}
}

void i3GameObj::SetMatrix( MATRIX * pMtx)
{
	m_pSceneObj->SetMatrix( pMtx); 

	m_pSceneObj->UpdateLastWorldTransform( pMtx);
}

void i3GameObj::SetPos( VEC3D * pVec)
{
	MATRIX * pMtx;

	pMtx = m_pSceneObj->GetMatrix();
	i3Matrix::SetPos( pMtx, pVec); 

	m_pSceneObj->UpdateLastWorldTransform( pMtx);
}

void i3GameObj::SetPos( REAL32 x, REAL32 y, REAL32 z)
{
	MATRIX * pMtx = m_pSceneObj->GetMatrix();
	i3Matrix::SetPos( pMtx, x, y, z, 1.0f);

	m_pSceneObj->UpdateLastWorldTransform( pMtx);
}

void i3GameObj::AddPos( REAL32 x, REAL32 y, REAL32 z)
{
	MATRIX * pMtx = GetMatrix();

	{
		VEC3D temp;

		i3Vector::Copy( &temp, GetPos());
		i3Vector::AddX( &temp, x);
		i3Vector::AddY( &temp, y);
		i3Vector::AddZ( &temp, z);
		i3Matrix::SetPos( pMtx, &temp);
	}
	
	m_pSceneObj->UpdateLastWorldTransform( pMtx);
}

void i3GameObj::SetInitialMatrix( MATRIX * pMtx, bool bInit)
{
	I3ASSERT( pMtx != nullptr);

	i3Matrix::Copy( &m_mtxInitial, pMtx);

	m_bHasInitialMatrix	= true;
		
	if( bInit)
	{
		SetMatrix( &m_mtxInitial);
	}
}

void i3GameObj::OnFadeStart(void)
{
	m_pFadeCtrl->RemoveFlag( I3_NODEFLAG_NO_WORK);
}

void i3GameObj::OnFading( REAL32 tm, REAL32 rate)
{
	m_pFadeMaterialAttr->SetDiffuse( 0.5f, 0.5f, 0.5f, rate);
}

void i3GameObj::OnFadeEnd(void)
{
	m_pFadeCtrl->AddFlag( I3_NODEFLAG_NO_WORK);
}

void i3GameObj::OnChangeTime( REAL32 tm)
{
	i3GameObjBase::OnChangeTime( tm);

	if((tm == 0.0f) && (m_bHasInitialMatrix	))
	{
		SetMatrix( &m_mtxInitial);

#if defined( I3_PHYSX)
		setDynamicActiveState( getInitialDynamicActiveState(), true);
		SetDynamicState( getInitialDynamicState(), true);
#endif
		m_idxPortalMask = 1;
		DeleteAllSection();
	}
}

void i3GameObj::OnBindResource(void)
{
	m_pSceneObj->BuildBoneList();
}

void i3GameObj::OnInstancing( I3_ONINSTANCING_INFO * pInfo)
{
	i3GameObjBase::OnInstancing( pInfo);
}

#if defined ( I3_PHYSX)
void i3GameObj::_UpdateDynamicState( i3Node * pNode)
{
	UINT32 addState, remState;

	addState = remState = 0;

	/*
	if( m_nDynamicState == I3GAME_DYNAMIC_STATE_DYNAMIC)
		m_nDynamicState = I3GAME_DYNAMIC_STATE_KINEMATIC;
		*/

	switch( m_nDynamicState)
	{
		case I3GAME_DYNAMIC_STATE_DYNAMIC :
			// Collision : ON
			// Response : ON
			// Gravity : ON
			remState = I3_PHYSIX_STATE_DISABLE ;
			break;

		case I3GAME_DYNAMIC_STATE_KINEMATIC :
			// Collision : ON
			// Response : OFF
			// Gravity : OFF
			addState = I3_PHYSIX_STATE_KINEMATIC | I3_PHYSIX_STATE_NO_RESPONSE | I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_LOCK_COM;
			remState = I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_NO_RAYCAST | I3_PHYSIX_STATE_FROZEN;
			break;

		case I3GAME_DYNAMIC_STATE_STATIC :
			// Collision : ON
			// Response : ON
			// Gravity : OFF
			addState = I3_PHYSIX_STATE_NO_GRAVITY | I3_PHYSIX_STATE_KINEMATIC | I3_PHYSIX_STATE_FROZEN | I3_PHYSIX_STATE_LOCK_COM;
			remState = I3_PHYSIX_STATE_NO_COLLISION | I3_PHYSIX_STATE_NO_RAYCAST | I3_PHYSIX_STATE_NO_RESPONSE;
			break;

		case I3GAME_DYNAMIC_STATE_TRIGGER:
			addState = I3_PHYSIX_STATE_DISABLE | I3_PHYSIX_STATE_TRIGGER;
			remState = 0;
			break;

		case I3GAME_DYNAMIC_STATE_NONE :
			// Collision : OFF
			// Response : OFF
			// Gravity : OFF
			addState =	I3_PHYSIX_STATE_DISABLE;
			remState =  0;
	}

	if( m_bDynamicActive == false)
	{
		addState |= I3_PHYSIX_STATE_DISABLE;
		remState = 0;
	}

	i3Scene::ModifyDynamicState( pNode, addState, remState);

	SetDynamicUpdateEnable( m_nDynamicState == I3GAME_DYNAMIC_STATE_DYNAMIC);
}

void i3GameObj::SetDynamicState( I3GAME_DYNAMIC_STATE state, bool bUpdateToNode)
{
	m_nDynamicState = state;

	if( bUpdateToNode)
		_UpdateDynamicState( GetNode());
}

void i3GameObj::setDynamicActiveState( bool bFlag, bool bUpdateToNode)
{
	m_bDynamicActive = bFlag;

	if( bUpdateToNode)
		_UpdateDynamicState( GetNode());
}

void i3GameObj::SetDynamicUpdateEnable( bool bEnable)
{
	if( m_pSceneObj != nullptr)
	{
		m_pSceneObj->SetDynamicUpdateEnable( bEnable);
	}
}

void i3GameObj::PHYSIX_SetForce( VEC3D * pLinearF, VEC3D * pAngularF )
{
	i3Scene::SetDynamicVelocity( m_pSceneObj, nullptr, pLinearF, pAngularF, pLinearF, pAngularF );
}

void i3GameObj::_SetExplosion( i3PhysixShapeSet * pShapeSet, VEC3D * pCenterPos,
							  REAL32 rExplosionRange, REAL32 rExplosionPower )
{
	REAL32	rLength, rForce;
	VEC3D	vDir;
	MATRIX	mat;
	VEC3D * pCenter = pCenterPos;

	if( pShapeSet == nullptr )
		return;

	if( pCenter == nullptr )
		pCenter	= GetPos();

	pShapeSet->getShapeGlobalPose( &mat );

	i3Vector::Sub( &vDir, i3Matrix::GetPos( &mat ), pCenter );
	rLength = i3Vector::Normalize( &vDir, &vDir );
	rForce = rExplosionRange - rLength;
	if( rForce <= 0.f )
		return;

	i3Vector::Scale( &vDir, &vDir, rForce * rExplosionPower );
	pShapeSet->setFBLinearVelocity( &vDir );
	pShapeSet->setFBAngularVelocity( &vDir );
}
void i3GameObj::PHYSIX_SetExplosionForce( VEC3D * pCenterPos, REAL32 rExplosionRange, REAL32 rExplosionPower )
{
	
	i3PhysixShapeSet * pShapeSet;
	i3Transform2 * pTrans2;

	if( m_pSceneObj->GetBody() == nullptr )
	{
		i3::vector<i3Node*>	ShapeList;
		i3Scene::FindNodeByExactType( getSceneObject(), i3Transform2::static_meta(), ShapeList, true);

		for(size_t i = 0; i < ShapeList.size(); i++ )
		{
			pTrans2 = (i3Transform2*) ShapeList[i];
			pShapeSet = pTrans2->getShapeSet();
			_SetExplosion( pShapeSet, pCenterPos, rExplosionRange, rExplosionPower );
		}
	}
	else
	{
		i3Skeleton * pSkel = m_pSceneObj->GetBody()->getFrameLOD()->getSkeleton();

		for(INT32 i = 0; i < pSkel->getBoneCount(); i++ )
		{
			pShapeSet = pSkel->getShapeSet( i );
			_SetExplosion( pShapeSet, pCenterPos, rExplosionRange, rExplosionPower );
		}
	}
}

void i3GameObj::PHYSIX_SetSleeping( bool bEnable )
{
	i3Scene::SetSleeping( GetNode(), bEnable );
	return;
}
#endif

bool i3GameObj::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if( event == I3_EVT_ACTION)
		{
			if( i3::same_of<i3ActionEmitEffect* >(pObj))
			{
				i3ActionEmitEffect * pAction = (i3ActionEmitEffect *) pObj;

				if( pAction->getEffectName() != nullptr)
				{
					if( pAction->getAttachEnable())
					{
						if( pAction->getAttachBoneMatrix() == nullptr)
						{
							MATRIX * pMtx = getSceneObject()->FindCacheMatrixByName( pAction->getAttachBoneName());

							pAction->setAttachBoneMatrix( pMtx);
						}
					}
					else
					{
						pAction->setAttachBoneMatrix( GetMatrix());
					}

					i3Game::getEffectManager()->AddEffect( pAction, GetMatrix());
				}
			}
			else if( i3::same_of<i3ActionVisible* >(pObj))
			{
				i3ActionVisible * pAction = (i3ActionVisible *) pObj;
				
				//SetVisible( pAction->getEnable());
				SetVisible( pAction->getEnable(), pAction->getAlphaTime());
			}
			else if( i3::same_of<i3ActionDisable* >(pObj))
			{
				i3ActionDisable * pAction = (i3ActionDisable *) pObj;

				SetEnable( pAction->getEnableState(), pAction->getFadeTime());
			}
			else if( i3::same_of< i3ActionDynamicState*>(pObj))
			{
				i3ActionDynamicState * pAction = (i3ActionDynamicState *) pObj;
	#if defined( I3_PHYSX)
				SetDynamicState( pAction->getState());
	#endif
			}
			else if( i3::same_of<i3ActionPath* >(pObj))
			{
				i3ActionPath * pAction = (i3ActionPath *) pObj;

				PlayAnim( pAction->getAnim(), pAction->getPlayMode());
			}
			else if( i3::same_of<i3ActionPlayAnim* >(pObj))
			{
				i3ActionPlayAnim * pAction = (i3ActionPlayAnim *) pObj;

				if( pAction->getAnim() != nullptr)
					PlayAnim( pAction->getAnim(), pAction->getPlayMode());
			}
			else if( i3::same_of<i3ActionSetWorldSection* >(pObj))
			{
				i3ActionSetWorldSection * pAction = (i3ActionSetWorldSection *) pObj;

				SetPortalMask(true, pAction->getWorldSection());
			}
			else if( i3::same_of< i3ActionSetMapSector*>(pObj))
			{
				i3ActionSetMapSector * pAction = (i3ActionSetMapSector *) pObj;

				setPortalID(true, pAction->getMapSector());
			}
		}
	}
	else
	{
		if( event == I3_EVT_ACTION)
		{
			if( i3::same_of<i3ActionSetWorldSection* >(pObj))
			{
				i3ActionSetWorldSection * pAction = (i3ActionSetWorldSection *) pObj;

				SetPortalMask(false, pAction->getWorldSection());
			}
			else if( i3::same_of<i3ActionSetMapSector* >(pObj))
			{
				i3ActionSetMapSector * pAction = (i3ActionSetMapSector *) pObj;

				setPortalID(false, pAction->getMapSector());
			}
		}
	}

	return i3GameObjBase::OnEvent( event, pObj, param, code);
}

void i3GameObj::SetPortalMask( bool bAdd, INT32 idx)
{
	i3Framework * pFramework = i3Framework::getCurrentFramework();
	if( pFramework == nullptr)
		return;

	I3ASSERT( idx >= 0);

	if (IsDropObj() == false)			//Drop된 무기가 아닐때만 SectionCount가 변동됨
	{
		ShadowSectionInfo* pSection = FindSection(idx);

		if (bAdd)
		{
			m_idxPortalMask |= ((UINT64)1) << idx;
			pSection->_Count++;
		}
		else
		{
			pSection->_Count--;
			if (pSection->_Count == 0)
				m_idxPortalMask &= ~(((UINT64)1) << idx);
		}
	}

	//I3TRACE("%s__PORTALMASK: %I64X\n", GetName(), m_idxPortalMask);
	
	OnUpdateWorldSectionMask();
}

void i3GameObj::OnUpdateWorldSectionMask()
{
	i3Framework * pFramework = i3Framework::getCurrentFramework();
	if( pFramework == nullptr)
		return;

 	UINT64 curPortalMask	= pFramework->getCurrentPortalMask();

	i3Body* pBody = NULL;

  	if (getSceneObject() && getSceneObject()->GetBody() && IsDropObj())
  		pBody = getSceneObject()->GetBody();

 	if(m_idxPortalMask == curPortalMask)
 	{
		if (pBody) pBody->RemoveFlag(I3_NODEFLAG_DISABLE_SHADOW_CASTER);
		else
		{
			GetNode()->RemoveFlag(I3_NODEFLAG_DISABLE_SHADOW_CASTER);
			GetNode()->RemoveFlag(I3_NODEFLAG_FORCE_CULL);
		}
		
 		return;
 	}

	UINT64 maskShadow	= pFramework->getSgContext()->getPortalShadowMask();
	UINT64 maskVisible	= pFramework->getSgContext()->getPortalVisibleMask();
	UINT64 mymask		= m_idxPortalMask;

	if( m_idxPortalMask > 1)
		mymask &= ~1;

	// Shadow Mask
	if (maskShadow & mymask)
	{
		if(pBody) pBody->RemoveFlag(I3_NODEFLAG_DISABLE_SHADOW_CASTER);
		else
			GetNode()->RemoveFlag(I3_NODEFLAG_DISABLE_SHADOW_CASTER);
	}
	else
	{
		if (pBody) pBody->AddFlag(I3_NODEFLAG_DISABLE_SHADOW_CASTER);
		else
			GetNode()->AddFlag(I3_NODEFLAG_DISABLE_SHADOW_CASTER);
	}

	// Visibility
	if (!pBody)
	{
		if (maskVisible & mymask)
			GetNode()->RemoveFlag(I3_NODEFLAG_FORCE_CULL);
		else
			GetNode()->AddFlag(I3_NODEFLAG_FORCE_CULL);
	}
}

void i3GameObj::SetPathAnimation( i3Animation * pAnim, I3_ANIM_PLAY_MODE mode)
{
	if( pAnim != nullptr)
	{
		getSceneObject()->SetPathAnimation( pAnim, mode);
	}
	else
	{
		getSceneObject()->SetPathAnimation( nullptr, mode);
	}
}

void i3GameObj::_ForceVisible( bool bTrue )
{
#if defined( I3_PHYSX)
	setDynamicActiveState( bTrue, true);
#endif

	i3GameObjBase::_ForceVisible( bTrue );
}

void i3GameObj::SetVisibleOnlyScene( bool bTrue)
{	//	Only apply visible scene without dynamic state - komet
	i3GameObjBase::SetVisible( bTrue );
}

void	i3GameObj::EnableUpdateAABB(bool bEnable)
{
	m_pSceneObj->EnableUpdateAABB(bEnable);

	i3Framework* pFramework = i3Framework::getCurrentFramework();
	if (pFramework)
	{
		OccDebugInfo* pInfo = pFramework->GetViewer()->getOccDebugInfo();
		if (pInfo)
		{
			i3GameObj::EnableRenderAABB(bEnable);
		}
	}
}

bool	i3GameObj::IsEnabledUpdateAABB() const
{
	return m_pSceneObj->IsEnabledUpdateAABB();
}

void	i3GameObj::EnableOccludee(bool bEnable)
{
	i3Framework* pFramework = i3Framework::getCurrentFramework();
	if (!pFramework)
		return;

	i3::occ_culler* pCuller = pFramework->getSgContext()->GetOccCuller();

	if (pCuller == nullptr)
		return;

	m_pSceneObj->EnableOccludee(pCuller, IsAttachedScene(), bEnable);
}

bool	i3GameObj::IsEnabledOccludee() const
{
	i3Framework* pFramework = i3Framework::getCurrentFramework();
	if (!pFramework) return false;
	i3::occ_culler* pCuller = pFramework->getSgContext()->GetOccCuller();
	if (!pCuller) return false;
	return m_pSceneObj->IsEnabledOccludee();
}


// 아바타의 경우 별도 구현되어 있기 때문에 해당 아바타 소스코드의 Attach,DetachScene의 경우도 별도
// 구현되어야 할 것이다..

void	i3GameObj::AttachScene(void)
{
	if (IsAttachedScene())
		return;

	i3GameObjBase::AttachScene();

	i3Framework* pFramework = i3Framework::getCurrentFramework();
	if (pFramework)
	{
		i3::occ_culler* pCuller = pFramework->getSgContext()->GetOccCuller();
		bool			isEnabledOccludee = m_pSceneObj->IsEnabledOccludee();

		if (pCuller && isEnabledOccludee)
		{
			i3::octree_item* pItem = m_pSceneObj->GetOctreeItem();
			pCuller->add_occludee_item(pItem);
		}
	}
}

void	i3GameObj::DetachScene(void)
{
	if (!IsAttachedScene())
		return;

	i3GameObjBase::DetachScene();

	i3Framework* pFramework = i3Framework::getCurrentFramework();
	if (pFramework)
	{
		i3::occ_culler* pCuller = pFramework->getSgContext()->GetOccCuller();
		bool			isEnabledOccludee = m_pSceneObj->IsEnabledOccludee();

		if (pCuller && isEnabledOccludee)
		{
			i3::octree_item* pItem = m_pSceneObj->GetOctreeItem();

			if (pItem->is_in_octree())
				pCuller->remove_occludee_item(pItem);
		}
	}
}

void i3GameObj::DeleteAllSection()
{
	for (INT32 i = 0; i < (INT32)m_vecSection.size(); i++)
	{
		I3_SAFE_DELETE(m_vecSection[i]);
	}
	m_vecSection.clear();
}

ShadowSectionInfo* i3GameObj::FindSection(INT32 idx)
{
	i3::vector<ShadowSectionInfo*>::iterator it = i3::find_if(m_vecSection.begin(), m_vecSection.end(), fnFindSection(idx));

	ShadowSectionInfo* pSection = nullptr;
	if (it != m_vecSection.end())
	{
		pSection = *it;
	}
	else
	{
		pSection = new ShadowSectionInfo;
		pSection->_idx = idx;
		pSection->_Count = 0;
		m_vecSection.push_back(pSection);
	}

	return pSection;
}

#pragma pop_macro("max")
#pragma pop_macro("min")

