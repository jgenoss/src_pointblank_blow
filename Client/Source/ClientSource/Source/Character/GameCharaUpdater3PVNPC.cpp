#include "pch.h"
#include "GameCharaUpdater3PVNPC.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaUpdater3PVNPC);//, CGameCharaUpdater3PV );

CGameCharaUpdater3PVNPC::CGameCharaUpdater3PVNPC()
{
}

CGameCharaUpdater3PVNPC::~CGameCharaUpdater3PVNPC()
{
}


void CGameCharaUpdater3PVNPC::ProcessRespawn( REAL32 rDeltatime)
{
	//	 리스폰 타이머 체크	
	REAL32 rRespawnElapseTime = m_pOwner->getRespawnElapseTime();

	if( rRespawnElapseTime > 0.0f)
	{	
		m_pOwner->getMaterialContext()->SetBlendEnable( true);

		rRespawnElapseTime -= rDeltatime;
		m_pOwner->setRespawnElapseTime( rRespawnElapseTime);

		//	리스폰이 된다.
		if( rRespawnElapseTime <= 0.0f)
		{
			m_pOwner->getMaterialContext()->SetBlendEnable( false);
			m_pOwner->setRespawnElapseTime( 0.f);
		}
	}
}

void CGameCharaUpdater3PVNPC::OnFirstUpdate( REAL32 rDeltatime)
{
	if( !g_pGameContext->IsP2PActivate() )	return;

	// Animation과 World-Space Transform 사이에 LOD 계산이 달라질 수 있으며,
	// 그것은 Visual 상에 여러 문제가 될 수 있기 때문에
	// 수동으로 LOD를 계산시킨다.
	//	Zoom시 FOV 비율 계산
	REAL32 fRatio = g_pCamera->GetCameraDefaultFOV() / g_pCamera->GetCameraFOV();

	//	모델링 LOD 계산
	m_pOwner->calcLOD( m_pOwner->GetPos(), i3Matrix::GetPos( g_pCamera->getCamMatrix()), fRatio);

	getBoneContext()->UpdateAnim( rDeltatime);				// Animation의 Key 값을 적용 (각 Bone의 Cache Matrix에 저장)

	getBoneContext()->TransformWorldSpace( rDeltatime);

}

void CGameCharaUpdater3PVNPC::OnUpdate( REAL32 rDeltatime)
{
	if( m_pOwner == NULL )		return;

	CGameCharaUpdater3PV::OnUpdate( rDeltatime);
	//	Add code here

	if( !g_pGameContext->IsP2PActivate() )	return;

	if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
		m_pOwner->ProcessTimer( rDeltatime);
	else
		ProcessRespawn( rDeltatime);

	getMaterialContext()->DriveLightVolume( rDeltatime);
	//	캐릭터 모델링 알파 처리
	//getMaterialContext()->ProcessMaterial( rDeltatime);

	
	REAL32 fAlpha = 1.0f;

	CGameMissionManager * pMissionMng = g_pGameContext->getMissionManager();
	I3ASSERT( pMissionMng != NULL);

	{
		//////////////////////////////////////////////////////////////////////
		//	리스폰 타임
		if( m_pOwner->getRespawnElapseTime() > 0.0f)	
		{
			//	죽은 캐릭터는 서서히 사라지는 효과
			I3ASSERT( m_pOwner->getDBInfo()->GetNPCRespawnTime() > I3_EPS );

			fAlpha = m_pOwner->getRespawnElapseTime() / m_pOwner->getDBInfo()->GetNPCRespawnTime();

			m_pOwner->Cmd_SetColor( NULL, NULL, fAlpha);
		}
		else
		{
			if( !m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) )
			{
				//////////////////////////////////////////////////////////////////////////////
				//	무적 타임
				if( m_pOwner->getInvincibleTime() > 0.0f)
				{
					//	무적시간시에는 깜빡이는 효과		      
					fAlpha = i3Math::abs( i3Math::sin( m_pOwner->getInvincibleTime() * 6.0f));

					m_pOwner->Cmd_SetColor( NULL, NULL, 1.f - fAlpha);
				}
				else
				{
					//	무적이 아닐때 원본 상태로 설정
					m_pOwner->Cmd_SetColor( NULL, NULL, 1.f);
				}
			}
		}
	}
}

void CGameCharaUpdater3PVNPC::OnProcessInvincible( REAL32 rDeltatime)
{
	//	리스폰 직후 무적시간을 계산해 준다.
	REAL32 rInvincible = m_pOwner->getInvincibleTime();

	if( rInvincible > 0.f )
	{
		rInvincible -= rDeltatime;
		m_pOwner->setInvincibleTime( rInvincible);

		getMaterialContext()->SetBlendEnable( true);
	}		
	else if( rInvincible < 0.f )
	{	
		m_pOwner->setInvincibleTime( 0.f);

		getMaterialContext()->SetBlendEnable( false);
	}
}




