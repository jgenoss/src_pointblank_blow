#include "pch.h"
#include "WeaponC4.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaActionContext.h"
#include "GameCharaBoneContext.h"
#include "./Character/GameCharaControlBase.h"
#include "BattleSlotContext.h"
#include "GameMissionManager.h"
#include "WeaponFunction.h"
#include "WeaponItemID.h"

#define C4_TIMER_MATERIAL_NAME		"C4_Lcd_Material"		//	타이머 모델링 머테리얼 이름 for Checking

#define NEXT_TIMER_STEP_TO_1			2.1f//1.2f		//	Bomb Timer Anim ElapsedTime OFF to 1 
#define NEXT_TIMER_STEP_TO_2			2.6f//1.7f		//	Bomb Timer Anim ElapsedTime 1 to 2 
#define NEXT_TIMER_STEP_TO_3			3.2f//2.3f		//	Bomb Timer Anim ElapsedTime 2 to 3 
#define NEXT_TIMER_STEP_TO_4			3.6f//2.8f		//	Bomb Timer Anim ElapsedTime 3 to 4 
#define NEXT_TIMER_STEP_TO_5			3.9f//2.8f		//	Bomb Timer Anim ElapsedTime 3 to 4 
#define NEXT_TIMER_STEP_TO_COMPLETE		4.3f//3.3f		//	Bomb Timer Anim ElapsedTime 4 to Complete 


I3_CLASS_INSTANCE( WeaponC4);//, WeaponBase);

WeaponC4::WeaponC4(void) : m_rInstallElapsedTime(0.0f)
						, m_rUninstallElpasedTime(0.0f)
						, m_rExplosionElapsedTime(0.0f)
						, m_rTracerUpdateTime(0.0f)
						, m_bDisarm(false)
						, m_nTimerStep(BOMB_TIMER_STEP_UNKNOWN)
						, m_nWarningCount(0)
						, m_bEnableActive(false)
{
	m_pVA_BombTimer = nullptr;
	m_pActiveSoundSequence = nullptr;	//순서가 바뀌면 곤란합니다.

	m_pTimerOrgPos = nullptr;
	m_pTimerOrgNormal = nullptr;
	m_pTimerOrgTex = nullptr;

	m_pTimerGeoAttr = nullptr;
	m_pEffectBombLight = nullptr;
}

WeaponC4::~WeaponC4(void)
{
	//I3_SAFE_RELEASE( m_pVA_BombTimer);
	I3_SAFE_RELEASE( m_pActiveSoundSequence);

	I3MEM_SAFE_FREE( m_pTimerOrgPos);
	I3MEM_SAFE_FREE( m_pTimerOrgNormal);
	I3MEM_SAFE_FREE( m_pTimerOrgTex);
}

void WeaponC4::_OnInitVairable( void)
{	
	WeaponBase::_OnInitVairable();

	m_rInstallElapsedTime	= 0.f;				// 설치 타임
	m_rUninstallElpasedTime	= 0.f;				// 해체 타임	
	m_rExplosionElapsedTime = 0.f;
	m_rTracerUpdateTime		= 0.f;

	m_bDisarm				= false;
	m_bEnableActive			= false;

	m_pVA_BombTimer			= nullptr;
	m_pEffectBombLight		= nullptr;
	m_nTimerStep			= BOMB_TIMER_STEP_UNKNOWN;

	i3Matrix::Identity( &m_mtxOld);

	if( m_pActiveSoundSequence != nullptr)
	{
		m_pActiveSoundSequence->RemoveFromParent();
		m_pActiveSoundSequence->OnChangeTime( 0.0f);
	}

	I3_SAFE_RELEASE( m_pActiveSoundSequence);
}

void WeaponC4::_InitCollideeGroup( void)
{
	WF::SetCollideeGroup_NoUseCollidee(this);
}

void WeaponC4::Reset()
{
	WeaponBase::Reset();		//	먼저 해야함.

	SetMatrix( &m_mtxOld);

	m_StateMaskWeapon		= WSM_LOADBULLET_DISABLE | WSM_LOADMAG_DISABLE;

	m_rInstallElapsedTime	= 0.f;	
	m_rUninstallElpasedTime	= 0.f;	
	m_rExplosionElapsedTime = 0.f;
	m_rTracerUpdateTime		= 0.f;

	m_bDisarm				= false;
	m_bEnableActive			= false;

	m_nWarningCount			= -1;

	//	Default Off Texture
	_SetBombTimerTextureCoord( BOMB_TIMER_STEP_OFF);

	if( m_pEffectBombLight != nullptr)
	{	//	점멸등 이펙트 정지
		m_pEffectBombLight->AddFlag( I3_NODEFLAG_DISABLE);
		m_pEffectBombLight = nullptr;
	}

	if( m_pActiveSoundSequence != nullptr)
	{
		m_pActiveSoundSequence->RemoveFromParent();
		m_pActiveSoundSequence->OnChangeTime( 0.0f);
	}
}

static i3TimeSequenceInfo * _MakeSoundObject( const char * pszSndName, REAL32 startTime, REAL32 duration)
{
	i3GameResSound * pRes = ( i3GameResSound*)g_pFramework->QuaryResource( pszSndName);
	I3ASSERT( pRes != nullptr);

	i3SoundPlayInfo * pInfo = pRes->getSound();
	I3ASSERT( pInfo != nullptr);

	i3SoundPlayInfo * pNewInfo = i3SoundPlayInfo::new_object_ref();
	pInfo->CopyTo( pNewInfo, I3_COPY_INSTANCE);

	if( duration > 0.0f)
	{
		pNewInfo->setLoopMode( I3SND_RT_LOOPMODE_ONESHOT);
	}
	else
	{
		pNewInfo->setLoopMode( I3SND_RT_LOOPMODE_LOOPING);
	}

	pNewInfo->set3DMode( I3SND_RT_3DMODE_WORLD_SPACE);

	pNewInfo->setInsideConeAngle( 360);
	pNewInfo->setOutsideConeAngle( 360);
	pNewInfo->setOutsideVolume( 100);

	i3SoundNode * pNewSndNode = i3SoundNode::new_object_ref();
	pNewSndNode->setSoundPlayInfo( pNewInfo);
	i3TimeSequenceInfo * pTimeSeq = i3TimeSequenceInfo::new_object_ref();
	pTimeSeq->SetStartTime( startTime);
	pTimeSeq->SetNode( pNewSndNode);

	return pTimeSeq;
}

void WeaponC4::PostCreate()
{
	INT32 i;
	
	//	최초 백업
	i3Matrix::Identity( &m_mtxOld);

	//	Find a Geometry of Bomb Timer
	{
		_InitTimerGeometry();

		if( m_pTimerGeoAttr != nullptr)
			_CreateNumberVertex();

		{
			I3ASSERT( m_pActiveSoundSequence == nullptr);

			m_pActiveSoundSequence = i3TimeSequence::new_object();

//			REAL32 rExpTime = getWeaponInfo()->m_rExplosion_Time;

			REAL32 rStartTime = 0.0f;
			REAL32 rSndDuration = 2.0f;

			for( i = 8; i > 0; --i)
			{
				m_pActiveSoundSequence->AddSequence( _MakeSoundObject( "WM_C4_Count_A.wav", rStartTime, rSndDuration));
				//I3TRACE( "Last Warning Start Time: %f, duration: %f\n", rStartTime, rSndDuration);
				rStartTime += rSndDuration;
				rSndDuration -= 0.1f;
			}

			for( i = 10; i > 0; --i)
			{
				m_pActiveSoundSequence->AddSequence( _MakeSoundObject( "WM_C4_Count_A.wav", rStartTime, rSndDuration));
//				I3TRACE( "Last Warning Start Time: %f, duration: %f\n", rStartTime, rSndDuration);
				rStartTime += rSndDuration;
				rSndDuration -= 0.04f;// 50회를 거쳤을때 0.2가 된다.
			}

			for( i = 15; i > 0; --i)
			{
				m_pActiveSoundSequence->AddSequence( _MakeSoundObject( "WM_C4_Count_A.wav", rStartTime, rSndDuration));
//				I3TRACE( "Last Warning Start Time: %f, duration: %f\n", rStartTime, rSndDuration);
				rStartTime += rSndDuration;
				rSndDuration -= 0.02f;// 50회를 거쳤을때 0.2가 된다.
			}

			for( i = 15; i > 0; --i)
			{
				m_pActiveSoundSequence->AddSequence( _MakeSoundObject( "WM_C4_Count_A.wav", rStartTime, rSndDuration));
//				I3TRACE( "Last Warning Start Time: %f, duration: %f\n", rStartTime, rSndDuration);
				rStartTime += rSndDuration;
				rSndDuration -= 0.02f;// 50회를 거쳤을때 0.2가 된다.
			}

			m_pActiveSoundSequence->AddSequence( _MakeSoundObject( "WM_C4_Count_E.wav", rStartTime, rSndDuration));
//			I3TRACE( "Last Warning Start Time: %f, duration: %f\n", rStartTime, rSndDuration);
		}
	}
}

void WeaponC4::_InitTimerGeometry(void)
{
	i3Node * pNode = i3Scene::FindNodeByName( getSceneObject(), C4_TIMER_MATERIAL_NAME);	
	if( pNode == nullptr)
		return;

	I3ASSERT( i3::same_of<i3AttrSet*>(pNode) );
	i3AttrSet * pAttrSet = (i3AttrSet *) pNode;

	m_pTimerGeoAttr = nullptr;

	for( INT32 i = 0; i < pAttrSet->GetChildCount(); i++)
	{
		i3Node * pNodeChild = pAttrSet->GetChild( i);

		if( i3::same_of<i3Geometry*>(pNodeChild))
		{
			i3Geometry * pGeo = (i3Geometry *)pNodeChild;

			m_pTimerGeoAttr = (i3GeometryAttr *) pGeo->GetGeometryAttr( 0);
			break;
		}
	}

	I3ASSERT_RETURN( m_pTimerGeoAttr != nullptr && m_pTimerGeoAttr->GetIndexArray() == nullptr);

	i3VertexArray * pArray = m_pTimerGeoAttr->GetVertexArray();

	I3MEM_SAFE_FREE( m_pTimerOrgPos);
	I3MEM_SAFE_FREE( m_pTimerOrgNormal);
	I3MEM_SAFE_FREE( m_pTimerOrgTex);

	I3ASSERT( pArray->GetCount() == 6);

	pArray->Lock();

	{
		m_pTimerOrgPos = (VEC3D *) i3MemAlloc( sizeof(VEC3D) * 6);
		m_pTimerOrgNormal = (VEC3D *) i3MemAlloc( sizeof(VEC3D) * 6);
		m_pTimerOrgTex = (VEC2D *) i3MemAlloc( sizeof(VEC2D) * 6);		

		INT32 i;

		for( i = 0; i < 6; i++)
		{
			pArray->GetPosition( i, &m_pTimerOrgPos[i]);
			pArray->GetNormal( i, &m_pTimerOrgNormal[i]);
			pArray->GetTextureCoord( 0, i, &m_pTimerOrgTex[i]);
		}
	}

	pArray->Unlock();
}

void WeaponC4::_CreateNumberVertex(void)
{
	//	Get Vertex Array
	i3VertexFormat format;

	format.SetHasPosition( true);
	format.SetHasNormal( true);
	format.SetTextureCoordSetCount( 1);
	format.SetTextureCoordCount( 2);

	m_pVA_BombTimer = i3VertexArray::new_object_ref();
	m_pVA_BombTimer->Create( &format, 6, 0);
				
	m_pTimerGeoAttr->SetVertexArray( m_pVA_BombTimer, false);

	m_pVA_BombTimer->Lock();

	for( INT32 i = 0; i < 6; i++)
	{
		m_pVA_BombTimer->SetPosition(	i, m_pTimerOrgPos + i);
		m_pVA_BombTimer->SetNormal(		i, m_pTimerOrgNormal + i);
		m_pVA_BombTimer->SetTextureCoord( 0, i, m_pTimerOrgTex + i);
	}

	m_pVA_BombTimer->Unlock();
}

void WeaponC4::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	REAL32	localTime = pCtx->getLocalTime();

	if( m_pOwner == nullptr )
		return;
	
	// 설치 시작
	if( localTime == 0.0f)
	{	
		// 연이어 발사하지 못하도록...
		setWeaponState( WEAPON::STATE_FIRE);
		if( m_pOwner->isLocal()) addWeaponStateMask( WSM_FIRE_DISABLE);
	}

	//	Check for Timer Next Step
	{
		m_pOwner->Cmd_Idle_Lower();
		BOMB_TIMER_STEP nNextStep = _GetNextTimerStepByTime( localTime);

		//	Update Timer Next Step Texture
		_SetBombTimerTextureCoord( nNextStep);
	}

	//	GameContext 폭탄 설치 시작
	{
		if( m_pOwner != nullptr && m_pOwner->isLocal() )
		{	
			//	폭탄 장착
			if( CGameMissionManager::i()->IsEnableMy() )
				CGameMissionManager::i()->SendMissionInstall( true );
			else
			{
				CGameMissionManager::i()->SendMissionInstall( false );	//	폭탄 설치 취소
				// 이동과 앉기 가능하도록 풀어준다.
				m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
				// 기본 Idle로 변경
				ReturnToReady();
				m_pOwner->Cmd_Return_Upper();
			}

			m_rInstallElapsedTime += tm;
		}

		m_nWarningCount = -1;
		m_rExplosionElapsedTime = 0.f;
	}
}

void WeaponC4::OnEndFire( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnEndFire( pCtx, tm);

	m_rInstallElapsedTime = 0.f;
}

void WeaponC4::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	m_rInstallElapsedTime = 0.f;

	if( isTakeChara())
	{
		//	Default Off Texture
		_SetBombTimerTextureCoord( BOMB_TIMER_STEP_OFF);		
	}
	else
	{	//	Just Complete Texture
		_SetBombTimerTextureCoord( BOMB_TIMER_STEP_COMPLETE);
	}
}

void WeaponC4::UpdateActive( REAL32 rDeltaSeconds)
{
	m_rExplosionElapsedTime += rDeltaSeconds;

	REAL32 rRate = m_rExplosionElapsedTime / getWeaponInfo()->GetExplosion_Time();

	switch( m_nWarningCount )
	{
	case -1 :
		GetNode()->AddChild( m_pActiveSoundSequence);
		m_nWarningCount = 0;
		break;
	case 0 :
		if( rRate > 0.35f)
			m_nWarningCount = 1;
		break;
	case 1 :
		if( rRate > 0.65f)
			m_nWarningCount = 2;
		break;
	case 2 :
		if( rRate > 0.85f)
			m_nWarningCount = 3;
		break;
	case 3 :
		if( rRate > 0.975f)
			m_nWarningCount = 4;
		break;
	}

	ISTEST_S
	{
		//	타이머가 지나면 폭발
		if( m_rExplosionElapsedTime > getWeaponInfo()->GetExplosion_Time())
		{
			GetNode()->RemoveChild( m_pActiveSoundSequence);

			Explosion();
		}	
		else
		{			
			INT32 nOldActiveTime = (INT32) m_rExplosionElapsedTime;

			INT32 nCurrentActiveTime = (INT32) m_rExplosionElapsedTime;

			if( nOldActiveTime != nCurrentActiveTime)
			{
				I3TRACE( "폭탄 작동중... (%d / %d)\n", nCurrentActiveTime, (INT32) getWeaponInfo()->GetExplosion_Time());
			}

			//	해체 중
			if( m_bDisarm)
			{
				m_rUninstallElpasedTime += rDeltaSeconds;
				
				//I3TRACE( "폭탄 해체중... (%d / %d)\n", m_rUninstallElpasedTime, (INT32) UNINSTALL_TIME);*/
			}
		}
	}
	ISTEST_E

	m_rTracerUpdateTime -= rDeltaSeconds;	
}

void WeaponC4::OnUpdate( REAL32 rDeltaSeconds)
{
	if( CGameMissionManager::i()->getMissionState() == GAME_MISSION_STATE_INSTALLED && m_bEnableActive)
	{	//	폭탄 타이머 작동	
		UpdateActive( rDeltaSeconds );
	}
	
	WeaponBase::OnUpdate( rDeltaSeconds);
}

//void WeaponC4::Fire(void)
//{
//	if( getLoadedBulletCount() <= 0)
//		return;
//
//	// 시점이 향하고 있는 목표 지점을 구한다.
//	NET_Fire( nullptr, nullptr );
//}

void WeaponC4::FireEnd(void)
{	
	WeaponBase::FireEnd();

	m_rInstallElapsedTime = 0.f;

	//	Default Off Texture
	//_SetBombTimerTextureCoord( BOMB_TIMER_STEP_OFF);
}

void WeaponC4::ActiveDisarm( bool bEnable)
{
	if( m_bEnableActive)	//	설치된 상태만 가능
	{
		if( bEnable)
		{
			m_bDisarm = true;
		}
		else
		{
			m_bDisarm = false;
			m_rUninstallElpasedTime = 0.f;
		}
	}
}

void WeaponC4::ReturnToReady( void)
{	
	WeaponBase::ReturnToReady();

	m_rInstallElapsedTime	= 0.f;	
	m_rUninstallElpasedTime	= 0.f;	
	m_rExplosionElapsedTime = 0.f;
	m_rTracerUpdateTime		= 0.f;
	m_nWarningCount = -1;

	//	GameContext 폭탄 설치 취소
	if( getOwner() != nullptr && (BattleSlotContext::i()->getMySlotIdx() == getOwner()->getCharaNetIndex()) )
	{
		if( getOwner()->getActionContext() && getOwner()->getActionContext()->isUpper( CHARA_UPPER_FIRE))
			CGameMissionManager::i()->SendMissionInstall( false);
	}

	if( m_pActiveSoundSequence != nullptr)
	{
		m_pActiveSoundSequence->RemoveFromParent();
		m_pActiveSoundSequence->OnChangeTime( 0.0f);
	}
}

void WeaponC4::Explosion()
{	
	//	폭발 이펙트
	I3ASSERT(getSceneObject());
	I3ASSERT(getSceneObject()->GetCacheMatrix());
	g_pEffectManager->AddEffect( getFireEffectIndex(), i3Matrix::GetPos(getSceneObject()->GetCacheMatrix()), &I3_YAXIS);
	g_pSndMng->StartWeapon_C4StateSnd( GTFW_C4_EXPLOSION, GetPos(), false);
	g_pSndMng->StartAnnounceMessage( GTA_BOMB_EXPLODED);

	//	점멸등 이펙트 정지
	if( m_pEffectBombLight != nullptr )
	{
		m_pEffectBombLight->AddFlag( I3_NODEFLAG_DISABLE);
		m_pEffectBombLight = nullptr;
	}

	// Check Collision
	_OnCheckCollisionByExplosion();

	m_bEnableActive = false;

	//	반드시 무기 매니저를 통해 제거해야 한다.
	g_pWeaponManager->PushDropWeaponToChara( this );

}

void WeaponC4::Uninstall( void)
{
	g_pSndMng->StartWeapon_C4StateSnd( GTFW_C4_DISARMED, GetPos(), false, false);
	g_pSndMng->StartAnnounceMessage( GTA_BOMB_REMOVED);

	//	점멸등 이펙트 정지
	if( m_pEffectBombLight != nullptr )
	{
		m_pEffectBombLight->AddFlag( I3_NODEFLAG_DISABLE);
		m_pEffectBombLight = nullptr;
	}

	m_bEnableActive = false;

	//	반드시 무기 매니저를 통해 제거해야 한다.
	g_pWeaponManager->PushDropWeaponToChara( this );

}
void WeaponC4::InstallC4ToWorld(i3Node* pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos, bool bToWorld, MATRIX* pMat)
{
	VEC3D vWeaponPos = VEC3D( pPos->x, pPos->y, pPos->z);
	
	if( getOwner() == nullptr )
		return;
	
	SetMatrix( getOwner()->GetMatrix());
		
	if( m_pWeaponInfo->GetNumber() == WEAPON::getItemIndex(WEAPON::C4))
	{
		//	오브젝트를 임의 회전. 올바른 방향을 바라 보도록. 캐릭터가 들고 있을시는 다시 복구해야 한다.
		i3Matrix::PreRotateAxisDeg( GetMatrix(), &I3_XAXIS, -135.f);
		i3Matrix::PostRotateAxisDeg( GetMatrix(), &I3_YAXIS, 180.f);
		
		i3Vector::ScaleAdd( &vWeaponPos, i3Matrix::GetUp( GetMatrix()), 0.01f);
		i3Vector::ScaleAdd( &vWeaponPos, i3Matrix::GetRight( GetMatrix()), 0.05f);	// hight 5cm
	}
	else
	{
		i3Matrix::PostRotateZDeg( GetMatrix(), 180.f);
	}

	//	폭탄 설치 점멸등 이펙트
	m_pEffectBombLight = g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_BOMB_LIGHT, pPos, &I3_YAXIS);
	// 폭탄 설치 사운드( 턱~!!)
	g_pSndMng->StartWeapon_C4StateSnd( GTFW_C4_INSTALLATION, pPos, false, false);

	//	Just set to Complete Texture
	_SetBombTimerTextureCoord( BOMB_TIMER_STEP_COMPLETE);
	I3TRACE( "--- 폭탄 설치 완료 (Pos %.4f %.4f %.4f).\n", pPos->x, pPos->y, pPos->z);
	//	C4는 월드(ParentGameNode)에 C4(GameNode)가 붙어야 합니다. (Owner 캐릭터가 사라진다 해도 OnUpdate를 호출해야 하기 때문에)
	WeaponBase::Transmission( pNewSceneNode, pNewOwner, &vWeaponPos, true);
}

void WeaponC4::InstallC4ToMoveObject(i3Transform* pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pLocalPos, VEC3D * pWorldPos, MATRIX* pMat)
{
	VEC3D vWeaponPos = VEC3D( pLocalPos->x, pLocalPos->y, pLocalPos->z);
	
	if( getOwner() == nullptr )
		return;
	
	SetMatrix( getOwner()->GetMatrix());
	
	if( m_pWeaponInfo->GetNumber() == WEAPON::getItemIndex(WEAPON::C4))
	{
		//	오브젝트를 임의 회전. 올바른 방향을 바라 보도록. 캐릭터가 들고 있을시는 다시 복구해야 한다.
		i3Matrix::PreRotateAxisDeg( GetMatrix(), &I3_XAXIS, -135.f);
		i3Matrix::PostRotateAxisDeg( GetMatrix(), &I3_YAXIS, 180.f);

		MATRIX matResult;
		i3Matrix::Mul(&matResult, GetMatrix(), pMat);
		
		i3Vector::ScaleAdd( &vWeaponPos, i3Matrix::GetUp(&matResult), 0.01f);
		i3Vector::ScaleAdd( &vWeaponPos, i3Matrix::GetRight(&matResult), 0.05f);	// hight 5cm
		
		matResult.m30 = vWeaponPos.x;
		matResult.m31 = vWeaponPos.y;
		matResult.m32 = vWeaponPos.z;

		SetMatrix(&matResult);
	}
	else
	{	
		i3Matrix::PostRotateZDeg( GetMatrix(), 180.f);
		MATRIX matResult;
		i3Matrix::Mul(&matResult, GetMatrix(), pMat);
		SetMatrix( &matResult);
	}
		
	VEC3D v3EffectPos;

	//	폭탄 설치 점멸등 이펙트
	m_pEffectBombLight = g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_BOMB_LIGHT, &v3EffectPos, &I3_YAXIS, getSceneObject()->GetCacheMatrix());
	//m_pEffectBombLight = g_pEffectManager->AddEnvironmentEffect( ENV_EFFECT_BOMB_LIGHT, pWorldPos, &I3_YAXIS);
	// 폭탄 설치 사운드( 턱~!!)
	g_pSndMng->StartWeapon_C4StateSnd( GTFW_C4_INSTALLATION, pWorldPos, false, false);

	//	Just set to Complete Texture
	_SetBombTimerTextureCoord( BOMB_TIMER_STEP_COMPLETE);
	//	C4는 월드(ParentGameNode)에 C4(GameNode)가 붙어야 합니다. (Owner 캐릭터가 사라진다 해도 OnUpdate를 호출해야 하기 때문에)
	WeaponBase::Transmission( pNewSceneNode, pNewOwner, &vWeaponPos, true);
}

//void WeaponC4::Transmission( i3Node * pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos, bool bToWorld, MATRIX* pMat)
//{	
//	VEC3D vWeaponPos = { pPos->x, pPos->y, pPos->z};
//	
//	getSceneObject()->RemoveFlag( I3_NODEFLAG_MANUAL_UPDATE, true );
//
//	if( getOwner() == nullptr )
//		return;
//
//	//	C4는 월드(ParentGameNode)에 C4(GameNode)가 붙어야 합니다. (Owner 캐릭터가 사라진다 해도 OnUpdate를 호출해야 하기 때문에)
//	WeaponBase::Transmission( pNewSceneNode, pNewOwner, &vWeaponPos, bToWorld);
//}

void WeaponC4::ReturnInstance( void)
{	
	m_bEnableActive = false;

	Reset();

	//	노드 제거 (렌더링과 게임노드에서 제거된다.)
	WeaponBase::ReturnInstance();

	m_pTimerGeoAttr = nullptr;
	m_pEffectBombLight = nullptr;

	//I3_SAFE_RELEASE( m_pVA_BombTimer);
}

void WeaponC4::_OnCheckCollisionByExplosion(void)
{
	INT32 i;
	INT32 x;
	VEC3D vLength;

	if( m_pOwner == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN,  "C4 owner is null.");
		return;
	}

	UINT32 nColGroup = CGRP_MASKGROUP_ATTACK;
	REAL32 rExplosionRange = GetExplosionRange();

	///////////////////////////////////////////////////////////////////////////
	for( x = 0; x < 2; x++)
	{
		if( x == 0)		nColGroup		= CGRP_MASKGROUP_CHARA_RED;
		else			nColGroup		= CGRP_MASKGROUP_CHARA_BLUE;
		
		for( i = 0; i < g_pCharaManager->getCharaCount(); ++i)
		{
			CGameCharaBase * pChara = g_pCharaManager->getChara( i);

			if( pChara == nullptr)		continue;
			i3Vector::Sub( &vLength, GetPos(), i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix()));
			REAL32 rLength = i3Vector::Length( &vLength);

			if( rLength < rExplosionRange )
				_OnCheckCharaCollisionByExplosion( pChara, nColGroup, rExplosionRange);
			
			// 카메라 흔들림 효과
			if( (g_pCamera != nullptr) && !pChara->is1PV() && !pChara->isCharaStateMask( CHARA_STATE_DEATH))
			{
				REAL32 MaxLength	= rExplosionRange * 2.0f;
				if( rLength < MaxLength)
				{
					REAL32 TrembAmpRate = 1.0f - rLength/MaxLength;
					g_pCamera->SetTremble( 15.0f * TrembAmpRate * TrembAmpRate, 10.0f * TrembAmpRate, 0.2f, 1.0f *	TrembAmpRate);
				}
			}
		}
	}

	// Object Collision
	_CheckExplosionObjectCollision( rExplosionRange);
}

void WeaponC4::_OnCollision(CGameCharaBase * pChara, i3CollideeLine * pLine, I3_PHYSIX_HIT_RESULT * pResult)
{	
	_OnHit( pResult, pLine, false);
}

bool WeaponC4::_SetBombTimerTextureCoord( BOMB_TIMER_STEP step)
{	//	Set Bomb Timer TextureCoord
	if( m_pVA_BombTimer == nullptr)	return false;

	I3ASSERT( step >= BOMB_TIMER_STEP_OFF && step < BOMB_TIMER_STEP_COUNT);

	if( m_nTimerStep == step)	return false;

	const INT32		nUVCount = 6;			//	Vertex Count
	//const REAL32	fPixelWidth = 128.f;
	const REAL32	fPixelHeight = 128.f;
	const REAL32	fPixelStep = 20.f;		//	한 스프라이트 간격

	m_nTimerStep = step;
	
	REAL32 fStep = (REAL32) step - 1.f;

	REAL32 v1 = ((fStep * fPixelStep) + 0.5f) / fPixelHeight;
	REAL32 v2 = ((fStep * fPixelStep) + fPixelStep + 0.5f) / fPixelHeight;

	VEC2D vTexCoord[ nUVCount] = 
	{
		VEC2D( 1.0f, v1 ),
		VEC2D( 0.0f, v1 ),
		VEC2D( 0.0f, v2 ),
		VEC2D( 0.0f, v2 ),
		VEC2D( 1.0f, v2 ),
		VEC2D( 1.0f, v1 ),
	};

	m_pVA_BombTimer->Lock();	

	//	Set a new UV
	m_pVA_BombTimer->SetTextureCoords( 0, 0, nUVCount, vTexCoord);

	m_pVA_BombTimer->Unlock();

	return true;
}

BOMB_TIMER_STEP WeaponC4::_GetNextTimerStepByTime( REAL32 fTime)
{
	BOMB_TIMER_STEP nNextStep = BOMB_TIMER_STEP_OFF;

	////-----------------	C4 Speed Kit
	//	사용자가 사용중이면 설치시간에 영향을 준다. (타이머 애니메이션만)
	REAL32 fActualSetTime = BattleSlotContext::i()->getMissionBomb_SetTime_UsingSpeedKit( m_pOwner->getCharaNetIndex());
	fActualSetTime += BattleSlotContext::i()->getMissionBomb_SetTime_UsingSkill( m_pOwner->getCharaNetIndex());

	REAL32 fTimeScale = fActualSetTime / (REAL32) WEAPON_BOMB_SETTIME;

	//	설치 완료 단계
	/*if( fTime >= NEXT_TIMER_STEP_TO_COMPLETE * fTimeScale)
	{
		nNextStep = BOMB_TIMER_STEP_COMPLETE;			 
	}*/
	//	설치 5 단계 
	if( fTime >= NEXT_TIMER_STEP_TO_5 * fTimeScale)
	{
		nNextStep = BOMB_TIMER_STEP_COMPLETE;			 
	}
	//	설치 4 단계 
	else if( fTime >= NEXT_TIMER_STEP_TO_4 * fTimeScale)
	{
		nNextStep = BOMB_TIMER_STEP_4;			 
	}
	//	설치 3 단계 
	else if( fTime >= NEXT_TIMER_STEP_TO_3 * fTimeScale)
	{
		nNextStep = BOMB_TIMER_STEP_3;			 
	}
	//	설치 2 단계 
	else if( fTime >= NEXT_TIMER_STEP_TO_2 * fTimeScale)
	{
		nNextStep = BOMB_TIMER_STEP_2;			 
	}
	//	설치 1 단계 
	else if( fTime >= NEXT_TIMER_STEP_TO_1 * fTimeScale)
	{
		nNextStep = BOMB_TIMER_STEP_1;			 
	}

	return nNextStep;
}

bool WeaponC4::OnLostDevice( bool bLostDevice)
{
	if( i3GameNode::OnLostDevice( bLostDevice) == false)
		return false;

	if( m_pTimerGeoAttr) m_pTimerGeoAttr->SetVertexArray( nullptr);

	return true;
}

bool WeaponC4::OnRevive( i3RenderContext * pCtx)
{
	if( i3GameNode::OnRevive( pCtx) == false)
		return false;

	if( m_pTimerGeoAttr)
		_CreateNumberVertex();

	return true;
}
