#include "pch.h"
#include "GameDropItem.h"

#include "../../../Common/CommonSource/CommonDediCli.h"
#include "BattleSlotContext.h"

namespace DropItem_Util
{
	const REAL32 DISTANCE_CATCH_AIDROPITEM = 0.5f;
	const INT32	 TIME_DROPITEM_LIVE = (INT32)DINOKILLITEM_LIVETIME + 1;
	const INT32	 DROPEDITEM_ALPHATIME = TIME_DROPITEM_LIVE - 1;			// 드랍된 무기가 사라지기 시작하는 시간
}

INT32 CGameDropItem::s_effect_idx[DINOKILLITEM_COUNT];

I3_CLASS_INSTANCE(CGameDropItem);

void CGameDropItem::Instantiate()
{
	MATRIX mat;
	I3_ONINSTANCING_INFO info;
	info._pBase = this;
	info._pMatrixStack = g_pFramework->GetViewer()->GetSceneTracer()->GetModelViewMatrixStack();
	
	i3Matrix::SetPos( &mat, 0.f, g_pConfigEx->GetPhysix().HeightWeapon, 0.f);
	info._pMatrixStack->Push( &mat, nullptr);
	OnInstancing( &info);
	info._pMatrixStack->Pop();
}

void CGameDropItem::_SetBlendAlpha( REAL32 val)
{
	val = i3Math::Clamp( val, 0.0f, 1.0f);

	if( (m_fAlphaVal != val) && (m_pMaterialAttr != nullptr) )
	{
		i3Color::SetA( m_pMaterialAttr->GetDiffuse(), val);
		i3Color::SetA( m_pMaterialAttr->GetSpecular(), val);

		{
			if( m_pBlendEnable->Get() == false)
				m_pBlendEnable->Set( true);
		}

		m_fAlphaVal = val;
	}
}

void CGameDropItem::_InitDrop()
{
	//Init
	m_bDroped = false;
	m_bPicked = false;
	m_fFlyingTime = 0.f;
	m_fLifeTime = 0.f;
}

bool CGameDropItem::_IsValidPickedState()
{
	CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());
	return (m_bDroped) && (!m_bPicked) && (myPlayer->isCharaStateMask( CHARA_STATE_DEATH) == false);
}

void CGameDropItem::_ProcessPicked()
{
	m_bPicked = true;

	GAMEEVENT::Write_DomiDropItemCatched( BattleSlotContext::i()->getMySlotIdx(), (INT8)m_nNetCode);

#if defined( I3_DEBUG)
	i3::stack_wstring wstr;
	i3::sprintf( wstr, L" pick up drop item (code : %d) send event...", m_nNetCode);
	g_pFramework->PutSystemChatting( wstr);
#endif

}

/*virtual*/ void CGameDropItem::Create( i3GameResChara* pRes)
{
	i3Chara::Create( g_pFramework, pRes);

	Instantiate();

	//	노드 상위에 알파처리를 위한 AttrSet을 붙인다.
	{
		m_pAttrSet = i3AttrSet::new_object_ref();
		m_pAttrSet->SetPriority( 0);	

		while( getAttachNode()->GetChildCount() > 0)
		{
			i3Node * pChild = getAttachNode()->GetChild( 0);
			pChild->ChangeParent(m_pAttrSet);
		}	

		getAttachNode()->AddChild( m_pAttrSet);		

		m_pBlendEnable = i3BlendEnableAttr::new_object_ref();
		m_pBlendEnable->Set( true);
		m_pAttrSet->AddAttr( m_pBlendEnable);

		i3BlendModeAttr * pBlendMode = i3BlendModeAttr::new_object_ref();
		pBlendMode->SetSource( I3G_BLEND_SRCALPHA);
		pBlendMode->SetDestination( I3G_BLEND_INVSRCALPHA);
		m_pAttrSet->AddAttr( pBlendMode);

		m_pMaterialAttr = i3MaterialAttr::new_object_ref();
		m_pAttrSet->AddAttr( m_pMaterialAttr);

		COLORREAL col;

		i3Color::Set( &col, 1.0f, 1.0f, 1.0f, 1.0f);
		m_pMaterialAttr->SetDiffuse( &col);
		m_pMaterialAttr->SetSpecular( &col);

		m_pMaterialAttr->setValidEmissive( false);
		m_pMaterialAttr->setValidSpecular( false);
		m_pMaterialAttr->setValidShininess( false);

	}
}

/*virtual*/	void CGameDropItem::OnUpdate( REAL32 rDeltaSeconds)
{
	i3Chara::OnUpdate( rDeltaSeconds);

	m_fLifeTime += rDeltaSeconds;
	if( m_fLifeTime > DropItem_Util::TIME_DROPITEM_LIVE)
	{
		Disappear();
		return;
	}

	UpdateFly( rDeltaSeconds);
	UpdateAlpha( rDeltaSeconds);

	if( CheckForItemPickUp( rDeltaSeconds))
		_ProcessPicked();
}

void CGameDropItem::UpdateAlpha( REAL32 fDeltaSeconds)
{
	if( m_bDroped)
	{
		if( m_fLifeTime > DropItem_Util::DROPEDITEM_ALPHATIME)
		{
			REAL32 fDelta = m_fLifeTime - DropItem_Util::DROPEDITEM_ALPHATIME;

			REAL32 fRatio = fDelta / (DropItem_Util::TIME_DROPITEM_LIVE - DropItem_Util::DROPEDITEM_ALPHATIME);

			_SetBlendAlpha( 1.f - fRatio);
		}
	}
}

void CGameDropItem::UpdateFly( REAL32 fDeltaSeconds)
{
	if( m_bDroped == false)
	{
		m_fFlyingTime += fDeltaSeconds;

		VEC3D velocity;
		VEC3D gravity;
		i3Vector::Set( &gravity, 0.f, -9.8f, 0.f);

		i3Vector::Scale( &velocity, &m_vDir, m_fSpeed);
		i3Vector::Scale( &gravity, &gravity, m_fFlyingTime*1.6f);

		VEC3D delta_pos;
		i3Vector::Add( &delta_pos, &gravity, &velocity);
		i3Vector::Scale( &delta_pos, &delta_pos, fDeltaSeconds*1.6f);

		i3Vector::Add( GetPos(), GetPos(), &delta_pos);

		//I3TRACE( " (item owner) drop item net index : %d\n", getNetworkIdx());
		//I3TRACE( " (item owner) drop itme Update pos	x : %.3f, y : %.3f, z : %.3f\n", GetPos()->x, GetPos()->y, GetPos()->z);


		//회전..
		i3Matrix::PreRotateX( GetMatrix(), - I3_DEG2RAD( fDeltaSeconds * 360.f * 0.5f));
		i3Matrix::PreRotateZ( GetMatrix(), + I3_DEG2RAD( fDeltaSeconds * 360.f * 0.3f));



		//바닥 충돌..
		i3CollideeLine ColLine;
		I3_PHYSIX_HIT_RESULT* pResult;

		VEC3D vDir;
		REAL32 len;

		ColLine.SetStart( &m_vOldPos);
		i3Vector::Sub( &vDir,  GetPos(), &m_vOldPos);
		len = i3Vector::Normalize( &vDir, &vDir);

		ColLine.SetEnd( GetPos());
		ColLine.SetDir( &vDir);
		pResult = g_pFramework->raycastClosest( &ColLine, CGRP_MASKGROUP_ONLYWORLDHIT); //CGRP_MASKGROUP_ATTACK

		if( pResult == nullptr) return;

		VEC3D	vecNew;
		i3Vector::Sub( &vecNew, &pResult->m_Intersect, &m_vOldPos);
		REAL32 testlen = i3Vector::Normalize( &vecNew, &vecNew);

		if( testlen < len)	
		{
			if( pResult->m_Normal.y < 0.f) //벽이나 천장은 다시 낙하..
			{
				VEC3D vFallDir;
				i3Vector::Set( &vFallDir, 0.f, -1.f, 0.f);
				SetFalling( &vFallDir, 0.f, GetPos(), nullptr);

				g_pFramework->resetRaycastResult();
				return;
			}

			VEC3D vecRevert;
			i3Vector::Scale( &vecRevert, &vecNew, - 0.05f);

			VEC3D vGrenadeSize;
			i3Vector::Scale( &vGrenadeSize, &pResult->m_Normal, 0.05f);

			i3Vector::Add( &vecRevert, &vecRevert, &vGrenadeSize);
			i3Vector::Add( GetPos(), &pResult->m_Intersect, &vecRevert);
			i3Vector::Add( &m_vOldPos, &m_vOldPos, &vecRevert);

			m_bDroped = true;

			MATRIX matTemp;
			
			i3Matrix::SetPos( &matTemp, GetPos());					//(땅에 박히는 걸 위로 올리기 위해서)

			VEC3D vCross;
			i3Vector::Cross( &vCross, &pResult->m_Normal, &m_vUp);

			REAL32 fRadian = i3Math::acos( i3Vector::Dot( &pResult->m_Normal, &m_vUp));
			i3Matrix::PreRotateAxis( &matTemp, &vCross, - fRadian);

			i3Matrix::PreRotateZ( &matTemp, - I3_DEG2RAD( ::rand() % 360 ));
			SetMatrix( &matTemp);

			//I3TRACE( "  -- drop item(%d) is droped.....\n", m_nNetCode);
		}

		g_pFramework->resetRaycastResult();

		//old pos
		i3Vector::Copy( &m_vOldPos, GetPos());
	}
}

bool CGameDropItem::CheckForItemPickUp( REAL32 fDeltaSeconds)
{
	if( _IsValidPickedState() == false) return false;

	CGameCharaBase* myPlayer = g_pCharaManager->getCharaByNetIdx(BattleSlotContext::i()->getMySlotIdx());

	VEC3D *vPos, *vPlayerPos;
	vPos = GetPos();
	vPlayerPos = myPlayer->GetPos();

	VEC3D vdist;
	REAL32 fLength = 0.f;
	i3Vector::Sub( &vdist, vPos, vPlayerPos);
	fLength = i3Vector::Length( &vdist);

	return (fLength <= DropItem_Util::DISTANCE_CATCH_AIDROPITEM);
}

void CGameDropItem::SetFalling( VEC3D* pDir, REAL32 fSpeed, VEC3D* pStart, VEC3D* pDest)
{
	if( pDest != nullptr)
	{	//Dest가 정해져 있으면 그냥 낙하
		i3Vector::Copy( &m_vDestPos, pDest);

		i3Vector::Copy( pStart, pDest);
		addY( pStart, 1.5f);

		i3Vector::Copy( &m_vOldPos, pStart);
	}
	else
	{
		i3Vector::Copy( &m_vOldPos, pStart);
	}

	SetPos( pStart);

	i3Vector::Copy( &m_vDir, pDir);
	i3Vector::Normalize( &m_vDir, &m_vDir);
	m_fSpeed = fSpeed;

	_SetBlendAlpha( 1.f);
	AttachScene();
	_InitDrop();
}

void CGameDropItem::Disappear()
{
	DetachScene();	
	
	SetFree(true);

	//I3TRACE( "	- drop item( net index %d) DetachScene\n", m_nNetCode);
	m_nNetCode = -1;
	m_debug_time = 0.f;
}

void CGameDropItem::pick_effect( CGameCharaBase* p)
{
	I3ASSERT( p != nullptr);
	if( p)
		g_pEffectManager->AddEffect( m_effect_idx, p->GetPos(), &I3_ZAXIS, p->GetMatrix(), 180.f, p);

	//{
	//	g_pEffectManager->AddEffect( getFireEffectIndex(), GetPos(), &I3_YAXIS);
	//}
}

void CGameDropItem::effect( CGameCharaBase* p, DINOKILLITEM e)
{
	I3ASSERT( p != nullptr);
	if( p)
		g_pEffectManager->AddEffect( s_effect_idx[e], p->GetPos(), &I3_ZAXIS, p->GetMatrix(), 180.f, p);
}