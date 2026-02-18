#include "pch.h"

#include "MainWeapon_Knuckle.h"
#include "WeaponFunction.h"
#include "UI/UIMainFrame.h"

MainWeapon_Knuckle::MainWeapon_Knuckle(WeaponBase* p) : MainWeapon_DualKnife(p)
{
	m_bSoundstop = false;

	for(INT32 a=0; a<WEAPON_HUMAN_MAX; a++)
	{
		for(INT32 b = 0; b<WEAPON_DUAL_HANDTYPE_MAX; b++)
		{
			m_aiiIdle[a][b] = -1;
			m_aiiChange[a][b] = -1;
			m_aiiAttackA[a][b] = -1;
			m_aiiAttackB[a][b] = -1;
		}
	}

}

void	MainWeapon_Knuckle::OnCreate(bool bCreateInWeapon)
{
	MainWeapon::OnCreate( bCreateInWeapon);		// 중간 파생클래스 함수를 건너뛰고, 상단기본클래스함수를 호출한다...기존구현이 중간호출을 안한다...(이런 부분은 나중에 또 고쳐야 한다)

										
	// 오른쪽(메인) 일 경우 더미용 왼쪽을 생성해서 들고 있는다.
	if( bCreateInWeapon == false) //this->GetHandType() == WEAPON_DUAL_HANDTYPE_RIGHT)
	{
		m_pLeftHandWeapon = WeaponKnuckle::new_object();
		I3ASSERT( m_pLeftHandWeapon != nullptr);

		m_pLeftHandWeapon->PreCreateMainWeaponBeforeCreateCall();

		MainWeapon_Knuckle* mainWeapon_Left = static_cast<MainWeapon_Knuckle*>(m_pLeftHandWeapon->GetMainWeapon());

		mainWeapon_Left->SetHandType( WEAPON_DUAL_HANDTYPE_LEFT);

		m_pLeftHandWeapon->Create( m_pWeaponBase->getOwner(),	m_pWeaponBase->getWeaponInfo()->GetItemID()	, true);

		m_pLeftHandWeapon->Reset();
		m_pLeftHandWeapon->setPairWeapon( m_pWeaponBase);

		// 사운드는 하나만 있으므로 다른 사운드 나는 것을 막는다.
		m_pLeftHandWeapon->setEnablePlayFireSound( false);
	}
	
}
	
void	MainWeapon_Knuckle::OnBindResource()
{
	MainWeapon_DualKnife::OnBindResource();

	i3AIContext* pAICtx = m_pWeaponBase->GetAIContext();

	m_aiiIdle[WEAPON_MALE][WEAPON_DUAL_HANDTYPE_RIGHT] = (INT8) pAICtx->FindAIStateByName( "Idle_Male_Right");
	m_aiiIdle[WEAPON_MALE][WEAPON_DUAL_HANDTYPE_LEFT] = (INT8) pAICtx->FindAIStateByName( "Idle_Male_Left");
	m_aiiIdle[WEAPON_FEMALE][WEAPON_DUAL_HANDTYPE_RIGHT] = (INT8) pAICtx->FindAIStateByName( "Idle_Female_Right");
	m_aiiIdle[WEAPON_FEMALE][WEAPON_DUAL_HANDTYPE_LEFT] = (INT8) pAICtx->FindAIStateByName( "Idle_Female_Left");

	m_aiiChange[WEAPON_MALE][WEAPON_DUAL_HANDTYPE_RIGHT] = (INT8) pAICtx->FindAIStateByName( "Change_Male_Right");
	m_aiiChange[WEAPON_MALE][WEAPON_DUAL_HANDTYPE_LEFT] = (INT8) pAICtx->FindAIStateByName( "Change_Male_Left");
	m_aiiChange[WEAPON_FEMALE][WEAPON_DUAL_HANDTYPE_RIGHT] = (INT8) pAICtx->FindAIStateByName( "Change_Female_Right");
	m_aiiChange[WEAPON_FEMALE][WEAPON_DUAL_HANDTYPE_LEFT] = (INT8) pAICtx->FindAIStateByName( "Change_Female_Left");

	m_aiiAttackA[WEAPON_MALE][WEAPON_DUAL_HANDTYPE_RIGHT] = (INT8) pAICtx->FindAIStateByName( "AttackA_Male_Right");
	m_aiiAttackA[WEAPON_MALE][WEAPON_DUAL_HANDTYPE_LEFT] = (INT8) pAICtx->FindAIStateByName( "AttackA_Male_Left");
	m_aiiAttackA[WEAPON_FEMALE][WEAPON_DUAL_HANDTYPE_RIGHT] = (INT8) pAICtx->FindAIStateByName( "AttackA_Female_Right");
	m_aiiAttackA[WEAPON_FEMALE][WEAPON_DUAL_HANDTYPE_LEFT] = (INT8) pAICtx->FindAIStateByName( "AttackA_Female_Left");

	m_aiiAttackB[WEAPON_MALE][WEAPON_DUAL_HANDTYPE_RIGHT] = (INT8) pAICtx->FindAIStateByName( "AttackB_Male_Right");
	m_aiiAttackB[WEAPON_MALE][WEAPON_DUAL_HANDTYPE_LEFT] = (INT8) pAICtx->FindAIStateByName( "AttackB_Male_Left");
	m_aiiAttackB[WEAPON_FEMALE][WEAPON_DUAL_HANDTYPE_RIGHT] = (INT8) pAICtx->FindAIStateByName( "AttackB_Female_Right");
	m_aiiAttackB[WEAPON_FEMALE][WEAPON_DUAL_HANDTYPE_LEFT] = (INT8) pAICtx->FindAIStateByName( "AttackB_Female_Left");

	m_pWeaponBase->setAIIndex( WEAPON::AI_IDLE, m_aiiIdle[WEAPON_MALE][this->GetHandType()]);	
}

void	MainWeapon_Knuckle::OnPlayChangeAni( REAL32 rTimeScale)
{
	WEAPON_HUMAN_TYPE nHuman = GetOwnerHumanType();

	I3_BOUNDCHK( this->GetHandType(), WEAPON_DUAL_HANDTYPE_MAX);

	m_aiiDualChange[this->GetHandType()] = m_aiiChange[nHuman][this->GetHandType()];

	// 하위에서 호출해야 합니다.
	
	MainWeapon_DualKnife::OnPlayChangeAni(rTimeScale);

}

void	MainWeapon_Knuckle::NET_Fire_Knuckle( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type)
{
	WEAPON_HUMAN_TYPE nHuman = GetOwnerHumanType();

	I3_BOUNDCHK( this->GetHandType(), WEAPON_DUAL_HANDTYPE_MAX);

	m_aiiDualAttackA[this->GetHandType()] = m_aiiAttackA[nHuman][this->GetHandType()];
	m_aiiDualAttackB[this->GetHandType()] = m_aiiAttackB[nHuman][this->GetHandType()];

	MainWeapon_DualKnife::NET_Fire_DualKnife(pStartPos, pTargetPos, type);
}


void MainWeapon_Knuckle::PlayIdleAnim( void)
{
	I3_BOUNDCHK( this->GetHandType(), WEAPON_DUAL_HANDTYPE_MAX);

	WEAPON_HUMAN_TYPE nHuman = GetOwnerHumanType();

	m_pWeaponBase->setAIIndex( WEAPON::AI_IDLE, m_aiiIdle[nHuman][ this->GetHandType()]);

	WF::PlayWeaponAnim(m_pWeaponBase, m_aiiIdle[nHuman][ this->GetHandType()] );
	
 	UIMainFrame* pUIMainframe = g_pFramework->GetUIMainframe();
	if (pUIMainframe != nullptr)
	{
		m_bSoundstop = true;
	}
}

void MainWeapon_Knuckle::PlayFireAnim( void)
{
	I3_BOUNDCHK( this->GetHandType(), WEAPON_DUAL_HANDTYPE_MAX);

	WEAPON_HUMAN_TYPE nHuman = GetOwnerHumanType();

	m_pWeaponBase->setAIIndex( WEAPON::AI_FIRE, m_aiiAttackA[nHuman][ this->GetHandType() ]);

	WF::PlayWeaponAnim(m_pWeaponBase, m_aiiAttackA[nHuman][ this->GetHandType()] );
}

void MainWeapon_Knuckle::PlayUIAnim( void)
{
	WEAPON_HUMAN_TYPE nHuman = GetOwnerHumanType();

	I3_BOUNDCHK( this->GetHandType(), WEAPON_DUAL_HANDTYPE_MAX);

	WF::PlayWeaponAnim(m_pWeaponBase, m_aiiIdle[nHuman][this->GetHandType()] );
}

void MainWeapon_Knuckle::OnUpdate(REAL32 rDeltaSeconds)
{
	if (m_bSoundstop)
	{
		i3AIState * pAI = m_pWeaponBase->GetAIContext()->getCurrentAIState();
		if (pAI != nullptr)
		{
			if (pAI->getTimeSeq() != nullptr)
			{
				INT32 count = pAI->getTimeSeq()->GetSequenceCount();
				for (INT32 i = 0; i < count; i++)
				{
					i3TimeSequenceInfo * pTime = pAI->getTimeSeq()->GetSequence(i);
					if (pTime != nullptr)
						pTime->SetStyle(I3SG_TIMESEQ_STYLE_DISABLE);
				}
			}
		}

		m_bSoundstop = false;
	}
}


WEAPON_HUMAN_TYPE MainWeapon_Knuckle::GetOwnerHumanType(void)
{
	if( m_pWeaponBase->getOwner() != nullptr)
	{
		// 남 여를 구분해야 하기 때문에 ResType은 구분할 수 없다.
		if(m_pWeaponBase->getOwner()->isFemale())
			return WEAPON_FEMALE;
		else
			return WEAPON_MALE;
	}

	return WEAPON_MALE;
}
