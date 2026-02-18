#include "pch.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaCameraContext.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaMoveContext.h"
#include "GameCharaWeaponContext.h"
#include "GlobalFunc.h"

#include "./Character/GameCharaCameraControl1PV.h"
#include "./Character/GameCharaCameraControl3PV.h"

#include "BattleHud.h"
#include "BattleHUD_Scope.h"
#include "i3Scene/i3XrayScene.h"

I3_CLASS_INSTANCE( CGameCharaCameraContext);

CGameCharaCameraContext::~CGameCharaCameraContext()
{
	OnDestroy();
}

REAL32 CGameCharaCameraContext::getIKHeightForCam()
{
	VEC4D vIKCam;
	bool bTrue1PV = false;
	if( i3::same_of<CGameCamera_FPS*>( g_pCamera))
	{	
		CGameCharaBase* pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == m_pOwner ) bTrue1PV = true;
	}

	m_pOwner->getDBInfo()->GetCamIKHeight( &vIKCam);

	//if( is1PV())
	if( bTrue1PV )
	{
		return vIKCam.vec[0] + m_rIKHeight;
	}
	else
	{
		
		return vIKCam.vec[ m_StateForCam];
	}
}

void CGameCharaCameraContext::getVirtualCamPosition( VEC3D * pPos, MATRIX * pMat, bool bDump)
{
//	I3ASSERT( g_pCameraManager != nullptr);
	I3ASSERT( CGameCameraManager::i() != nullptr);

	REAL32	Theta;
	VEC3D	vAt, vPos;
	MATRIX	mtx;
	VEC3D	offset;

	i3Vector::Copy( &vAt, i3Matrix::GetAt( m_pOwner->GetMatrix()));

	Theta = atan2f( getX( &vAt), getZ( &vAt));
	i3Matrix::SetRotateY( &mtx, Theta + I3_DEG2RAD( 180.0f));

	m_pOwner->getDBInfo()->GetFPSCamPos( &offset);

	if( m_pOwner->isLeftHand())	offset.x = -offset.x;
	
	//Rotate
	i3Matrix::PostRotateAxis( &mtx, i3Matrix::GetRight( m_pOwner->GetMatrix()), - m_pOwner->getBoneContext()->getRho());
	i3Vector::TransformCoord( &vAt, &offset, &mtx);
	
	if( m_pOwner->is1PV())
	{	//렙터스팅
		if(m_pOwner->IsRaptorBase()) {
			if (m_pOwner->IsTank())
			{
			i3Vector::Copy( &vPos, i3Matrix::GetPos( m_pOwner->getBoneContext()->getBoneMatrix( RAPTOR_BONE_SPINE2)));

			}
			else
			i3Vector::Copy( &vPos, i3Matrix::GetPos( m_pOwner->getBoneContext()->getBoneMatrix( RAPTOR_BONE_NECK)));
		}
		else if(m_pOwner->IsRex()) {
			i3Vector::Copy( &vPos, i3Matrix::GetPos( m_pOwner->getBoneContext()->getBoneMatrix( TREX_BONE_PIVOTCAM)));
		}
		else 
		{
			i3Vector::Copy( &vPos, i3Matrix::GetPos( m_pOwner->getBoneContext()->getBoneMatrix( BONE_NECK)));
		}
	}
	else if( !m_pOwner->is1PV() && m_pOwner->isAI())
	{
		i3Vector::Copy( &vPos, m_pOwner->GetPos());
		i3Vector::SetY( &vPos, getY( m_pOwner->GetPos()) + getIKHeightForCam());
	}	
	else
	{
		if( m_pOwner->IsRex())
		{
			i3Vector::Copy( &vPos, i3Matrix::GetPos( m_pOwner->getBoneContext()->getBoneMatrix( TREX_BONE_PIVOTCAM)));
		}
#ifdef DOMI_DINO
		else if (m_pOwner->IsDomiMutantRex())
		{
			i3Vector::Copy( &vPos, i3Matrix::GetPos( m_pOwner->getBoneContext()->getBoneMatrix( DOMI_MUTANTREX_BONE_PIVOTCAM)));
		}
#endif
		else
		{
			i3Vector::Copy( &vPos, i3Matrix::GetPos( m_pOwner->getBoneContext()->getBoneMatrix( BONE_IK)));
			i3Vector::SetY( &vPos, getY( m_pOwner->GetPos()) + getIKHeightForCam());
		}
	}

	if( pPos != nullptr )
	{
		i3Vector::Add( pPos, &vAt,&vPos);

		/*GlobalFunc::PB_TRACE("pPos %f %f %f vPos %f %f %f vAt %f %f %f GetPos %f %f %f gg %f ", 
			pPos->x, pPos->y, pPos->z, vPos.x, vPos.y, vPos.z, vAt.x, vAt.y, vAt.z, m_pOwner->GetPos()->x, m_pOwner->GetPos()->y, m_pOwner->GetPos()->z, (vPos.y-m_pOwner->GetPos()->y));*/
	}

	if( pMat != nullptr )
	{
		i3Matrix::Copy( pMat, &mtx );
	}
}

void CGameCharaCameraContext::OnCreate( CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork)
{	
	m_p1PVCameraControl = CGameCharaCameraControl1PV::new_object();
	m_p1PVCameraControl->BindOwner( pOwner);
	m_p3PVCameraControl = CGameCharaCameraControl3PV::new_object();
	m_p3PVCameraControl->BindOwner( pOwner);

	CGameCharaContextBase::OnCreate( pOwner, defaultView, defaultNetwork);
}

void CGameCharaCameraContext::OnDestroy( void)
{
	CGameCharaContextBase::OnDestroy();

	m_pCurrentCameraControl = nullptr;
	I3_SAFE_RELEASE( m_p1PVCameraControl );
	I3_SAFE_RELEASE( m_p3PVCameraControl );
}

void CGameCharaCameraContext::OnReset( void)
{
	CGameCharaContextBase::OnReset();

	m_StateForCam			= CSFC_STAND;
	m_nChangeFromState		= CSFC_STAND;
	m_rChangePoseTime		= 0.f;
	m_rChangePoseAccTime	= 0.f;
	m_bChangeToStand		= false;
}

void CGameCharaCameraContext::OnChange1PV()
{
	m_pCurrentCameraControl = m_p1PVCameraControl;

	if(m_pOwner->IsDino()) {
		return;
	}

	if( g_pFramework->IsBattleStage() )
	{
		if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM ) && (getCurrentWeapon() != nullptr) )
		{
			getHUDContext()->SetScope( true, getCurrentWeapon());
		}
	}
}

void CGameCharaCameraContext::OnChange3PV()
{
	CGameCharaContextBase::OnChange3PV();

	m_pCurrentCameraControl = m_p3PVCameraControl;
}

void CGameCharaCameraContext::OnChangeLocal()
{
	CGameCharaContextBase::OnChangeLocal();
}

void CGameCharaCameraContext::OnChangeNetwork()
{
	CGameCharaContextBase::OnChangeNetwork();
}

void CGameCharaCameraContext::OnChangeNPC( void)
{
	CGameCharaContextBase::OnChangeNPC();
}

void CGameCharaCameraContext::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaContextBase::OnUpdate( rDeltatime);
	// Add code here

	if (m_pOwner->IsDino()) return;

	CHARA_STATE_FOR_CAM nNewTargetState = CSFC_UNKNOWN;

	if( getActionContext()->isLower( CHARA_LOWER_RUN) || getActionContext()->isLower( CHARA_LOWER_WALK))
	{
		if( m_pOwner->getActionContext()->isBody( CHARA_BODYLOWER_STAND))	//	서 있으면
		{
			nNewTargetState = CSFC_STAND;
		}
		else if( m_pOwner->getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))	//	앉아 있으면
		{
			nNewTargetState = CSFC_SITMOVE;
		}
	}
	else if( getActionContext()->isLower( CHARA_LOWER_JUMP))
	{
		nNewTargetState = CSFC_JUMP;
	}
	else
	{
		if( m_pOwner->getActionContext()->isBody( CHARA_BODYLOWER_STAND))	//	서 있으면
		{
			nNewTargetState = CSFC_STAND;
		}
		else if( m_pOwner->getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))	//	앉아 있으면
		{
			nNewTargetState = CSFC_SIT;
		}
	}

	if( nNewTargetState != m_StateForCam)
	{
		//자세 변경시의 소리
		if( (nNewTargetState == CSFC_STAND) && (m_StateForCam == CSFC_SIT))
		{
			INT32 nRand = i3Math::Rand()%2;

			g_pSndMng->StartCharacterActionSnd( GTBCA_ON_STANDUP, nRand, m_pOwner->GetPos(), m_pOwner->is1PV());
		}
		else if( (m_StateForCam == CSFC_STAND) && (nNewTargetState == CSFC_SIT))
		{
			INT32 nRand = i3Math::Rand()%2;

			g_pSndMng->StartCharacterActionSnd( GTBCA_ON_SITDOWN, nRand, m_pOwner->GetPos(), m_pOwner->is1PV());
		}


		bool bTrue1PV = false;
		if( i3::same_of<CGameCamera_FPS*>(g_pCamera))
		{	
			CGameCharaBase* pCharacter = g_pCamera->getCurTarget();
			if( pCharacter == m_pOwner ) bTrue1PV = true;
		}
		
		if( bTrue1PV )
		{
			if((m_StateForCam == CSFC_STAND) && (nNewTargetState != CSFC_JUMP))
			{
 				ProcessStandOn1PV( nNewTargetState, 0.1f);
			}
			else
			{
				ProcessStandOn1PV( nNewTargetState, 0.3f);// 일어날때 
			}
		}
		else
		{
			if( m_pOwner->is1PV() )
			{
				if((g_pCamera != nullptr) && (nNewTargetState != CSFC_JUMP) && m_StateForCam != CSFC_JUMP)
				{
					g_pCamera->SetInterPolate( 0.2f);
				}
			}
		}
	}
	else
	{

	}

	m_StateForCam = nNewTargetState;

	//I3ASSERT( m_StateForCam != CSFC_UNKNOWN);
}

void CGameCharaCameraContext::ProcessStandOn1PV( CHARA_STATE_FOR_CAM state, REAL32 rDeltatime)
{
	if( state == m_StateForCam)
		return;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_CHANGEPOSE))
	{// 자세 변경중이다.
		m_rChangePoseAccTime	= rDeltatime * ( (m_rChangePoseTime - m_rChangePoseAccTime) / m_rChangePoseTime);		//자세 변경의 누적 시간
	}
	else
	{
		m_rChangePoseAccTime	= 0.0f;
		m_pOwner->addCharaStateMask( CHARA_STATE_CHANGEPOSE);		//자세 변경이 시작되었음을 알리는 플래그
	}

	if( m_nChangeFromState == CSFC_JUMP || state == CSFC_JUMP)
	{
		m_rChangePoseAccTime = rDeltatime * 0.1f;
	}

	m_rChangePoseTime		= rDeltatime;								//자세 변경시 걸리는 시간
	m_nChangeFromState		= m_StateForCam;
	m_StateForCam			= state;

	//자세 변경시 시작부터 컨트롤러를 바꾼다.
	// GlobalFunc::PB_TRACE("SetPosition %f %f %f \n", m_pOwner->GetPos()->x, m_pOwner->GetPos()->y, m_pOwner->GetPos()->z);

	/*if( m_pOwner->isLocal() )
	{
		if( m_StateForCam == CSFC_SIT || m_StateForCam == CSFC_SITMOVE)
		{
			getMoveContext()->ChangeControllerHeight( CAPSULE_HALF_HEIGHT);
		}
		else
		{
			getMoveContext()->ChangeControllerHeight( CAPSULE_FULL_HEIGHT);
		}
	}*/
}

void CGameCharaCameraContext::ProcessChangePose( REAL32 rDeltatime)
{
	VEC4D vIKCam;
	m_rIKHeight = -0.15f;

	const CCharaInfo * pCharaInfo = m_pOwner->getDBInfo();

	if( pCharaInfo == nullptr )
		return;

	pCharaInfo->GetCamIKHeight( &vIKCam);

	if( m_pOwner->isCharaStateMask( CHARA_STATE_CHANGEPOSE) )
	{	// 자세 변경중이다.
		REAL32 rDelta	= vIKCam.vec[ m_nChangeFromState] - vIKCam.vec[ m_StateForCam];

		if( m_rChangePoseAccTime >= m_rChangePoseTime)
		{// 자세 변경이 완료되었다.
			m_pOwner->removeCharaStateMask( CHARA_STATE_CHANGEPOSE);
			m_rChangePoseAccTime= 0.0f;
			m_rIKHeight			+= vIKCam.vec[m_StateForCam] - vIKCam.vec[0];
			m_pOwner->getAnimContext()->setRunElapseTime(0.f);

			//I3TRACE( "ChangePoseTime : %.4f\n", m_rChangePoseAccTime );
			return ;
		}

		REAL32 rDiv				= i3Math::div( m_rChangePoseAccTime, m_rChangePoseTime);
		REAL32 rRatio			= MINMAX( 0.0f, ( rDelta > 0.0f) ? i3Math::vsin_libm( rDiv) : i3Math::vcos_libm( rDiv), 1.0f);
		
		REAL32 rHeight			= (rDelta > 0.0f) ? rDelta * ( 1.0f - rRatio) - vIKCam.vec[0] : 
									rDelta * (rRatio) - vIKCam.vec[0];

		m_rIKHeight				+= vIKCam.vec[ m_StateForCam] + rHeight;
		m_rChangePoseAccTime	+= rDeltatime;
	}
	else
	{
		m_rIKHeight				+= vIKCam.vec[ m_StateForCam] - vIKCam.vec[ 0];
	}
}

void CGameCharaCameraContext::SetZoom( void)
{
	bool bViewer = false;
	if( i3::same_of<CGameCamera_FPS*>(g_pCamera))
	{	
		CGameCharaBase* pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == m_pOwner ) bViewer = true;
	}	

	if( bViewer )
	{
		//	견착 카메라 상태
		WeaponBase * pWeapon = getCurrentWeapon();
		if( pWeapon == nullptr) return;

		// 1인칭 fov만 적용함 - 박명진 2008. 6. 24
		g_pCamera->ZoomIn( pWeapon->getWeaponInfo()->GetZoomFOV(), 0.15f);

		m_pOwner->getBoneContext()->getScopeWaveData()->m_nCurScopeLevel = 1;

		g_pSndMng->StartWeaponStateSnd( GTBWS_ON_ZOOM, 0, m_pOwner->GetPos(), m_pOwner->is1PV());

		g_pFramework->getSgContext()->GetXrayScene()->SetZoom(false);
	}
}

bool CGameCharaCameraContext::SetScopeLevel( INT32 nScopeLevel)
{
	if( nScopeLevel <= 0 )		return false;
	if(m_pOwner->IsDino())		return false;

	WeaponBase * pWeapon = getCurrentWeapon();
	if( pWeapon == nullptr )		return false;

	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr );

	bool bViewer = false;
	if(  i3::same_of<CGameCamera_FPS*>(g_pCamera))
	{	
		//	Zoom시 캐릭터 팔과 총구가 화면에 삐져나오는 것을 안보이게 하기 위해 Visible Off 시킨다.
		//	Scope 상태이면 화면 HUD만 보이도록 캐릭터와 무기 모델링은 Scene만 off, DynamicState는 On 되어야 한다.
		//
		//  NOTE
		//	Chara를 보이지 않도록 할 때, SetVisible() 및 SetEnable()을 써서는 안된다.
		//  두 함수는 Physix Actor까지 Disable 상태로 만들며, 이것은 Trigger Leave 상황을 만들게 된다.
		CGameCharaBase* pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == m_pOwner )
		{
			bViewer = true;
			m_pOwner->getSceneObject()->AddFlag( I3_NODEFLAG_INVISIBLE);
		}
		else
		{
			m_pOwner->getSceneObject()->RemoveFlag( I3_NODEFLAG_INVISIBLE);
		}
	}
	else
	{
		m_pOwner->getSceneObject()->RemoveFlag( I3_NODEFLAG_INVISIBLE);
	}

	pWeapon->setZoom( true);
	
	//	스나이퍼 카메라 상태 (스코프 단계가 가능한 상태이면)
	// 현재 스코프레벨이 넘는 경우 맥스값에 맞춰서 처리하도록 한다.( 관전모드에서 맞추기 위함)
	if( nScopeLevel > pWeaponInfo->GetMaxScopeLevel())
	{
		nScopeLevel = pWeaponInfo->GetMaxScopeLevel();
	}

	if( bViewer )
	{
		REAL32 fScopeFOV = 0.0f;
		switch( nScopeLevel)
		{
		case 1:		fScopeFOV = pWeapon->GetScopeFOV();			break;		//	1단 Scope
		case 2:		fScopeFOV = pWeapon->GetScopeFOV2();		break;		//	2단 Scope
		default:	I3PRINTLOG(I3LOG_FATAL,  "invalid scope level.");	//	설정된 이상의 Scope를 할수 없다.
		}	

		g_pCamera->ZoomIn( fScopeFOV, 0.15f);

		getHUDContext()->SetScope( true, pWeapon);

		getSoundContext()->PlayScopeSound( pWeapon);
	}

	getBoneContext()->getScopeWaveData()->m_nCurScopeLevel = nScopeLevel;

	g_pFramework->getSgContext()->GetXrayScene()->SetZoom(true);

	return true;
}

void CGameCharaCameraContext::_Idle_Camera( void)
{
	if( g_pCamera == nullptr)							return;

	bool bViewer = false;
	if( i3::same_of<CGameCamera_FPS*>(g_pCamera))
	{	
		CGameCharaBase* pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == m_pOwner ) bViewer = true;
	}	
	else if( i3::same_of<CGameCamera_Kill*>(g_pCamera))
	{
		CGameCharaBase* pCharacter = g_pCamera->getCurTarget();
		if( pCharacter == m_pOwner ) bViewer = true;
	}

	m_pOwner->removeCharaStateMask( CHARA_STATE_ZOOM );

	if( !bViewer )		return;

	if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		m_pOwner->SetEnable( true);

	//	일반 카메라 상태
	g_pCamera->SetDefaultFOV( 0.15f);			

	if(m_pOwner->IsDino() == false)
	{
		WeaponBase * pWeapon = getCurrentWeapon();

		if( pWeapon != nullptr)
		{
			pWeapon->SetEnable( true);
			pWeapon->setZoom( false);
		}

		if( m_pOwner->getBoneContext()->getScopeWaveData()->m_nCurScopeLevel > 0)
		{
			if( CHUD_Scope::instance()->IsEnabledSniperScope())
			{
				if( pWeapon != nullptr)
					getSoundContext()->PlayScopeSound( pWeapon, false);
			}
			else
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_ON_ZOOM, 2, m_pOwner->GetPos(), m_pOwner->is1PV());
			}
		}

		getHUDContext()->SetScope( false);
	}

	getBoneContext()->getScopeWaveData()->m_nCurScopeLevel = 0;
	m_pOwner->Cmd_ApplyCrosshair();
}

void CGameCharaCameraContext::_Kill_Camera( INT32 killerIdx)
{
	m_pCurrentCameraControl->OnChangeKillCamera( killerIdx);
}

void CGameCharaCameraContext::_Observer_Zoom( void)
{
	m_pCurrentCameraControl->OnSetObserveZoom();
}

void CGameCharaCameraContext::_Respawn_Camera( void)
{
	m_pCurrentCameraControl->OnProcessRespawn();
}




