#include "pch.h"
#include "TargetInTutorial.h"
#include "StageBattle.h"
#include "TutorialMode.h"
#include "GameMode.h"

I3_CLASS_INSTANCE( CTargetInTutorial);//, CGameControledObject);


CTargetInTutorial::CTargetInTutorial()
{
	m_pAnim[TAS_IDLE] = i3Animation::LoadFromFile("Object/cgame/Tu_Target_Idle.i3a");
	m_pAnim[TAS_DOWN] = i3Animation::LoadFromFile("Object/cgame/Tu_Target_Down.i3a");
	m_pAnim[TAS_DOWN_IDLE] = i3Animation::LoadFromFile("Object/cgame/Tu_Target_Down_Idle.i3a");
	m_pAnim[TAS_RETURN] = i3Animation::LoadFromFile("Object/cgame/Tu_Target_Return.i3a");

	m_eTargetAnimState = TAS_IDLE;
	m_rStateTime = 0.0f;
}


/*virtual*/ CTargetInTutorial::~CTargetInTutorial()
{
	for(INT32 i=0; i<TAS_COUNT; i++)
	{
		I3_SAFE_RELEASE( m_pAnim[ i]);
	}
}

void CTargetInTutorial::Create( i3Framework * pFramework, i3GameResObject * pRes)
{
	i3Object::Create(pFramework, pRes);
	SetTargetState(TAS_IDLE);
}

void CTargetInTutorial::ResetObject()
{
	SetTargetState(TAS_IDLE);
	setMaxHP(1.f);
	setCurrentHP(getMaxHP());
}

void CTargetInTutorial::ResetObjectOnInit()
{
	if( g_pFramework->IsBattleStage() )
	{
		if( gmode::i()->GetTutorialMode() != nullptr)
		{
			 gmode::i()->GetTutorialMode()->AddTargetInTutorial(this);
		}
	}
}

void CTargetInTutorial::Damage( REAL32 dmg, i3GameObj * pEventSender, i3PhysixShapeSet * pHitShapeSet)
{
	i3Object::Damage( dmg, pEventSender, pHitShapeSet);

	if(getCurrentHP() <= 0.f)
	{
		if(m_eTargetAnimState == TAS_IDLE)
		{
			SetTargetState(TAS_DOWN);
		}
	}
}

void CTargetInTutorial::SetTargetState(TARGET_ANIM_STATE eState)
{
	m_rStateTime = 0.f;
	m_eTargetAnimState = eState;
	m_pSceneObj->PlayAnim( m_pAnim[ m_eTargetAnimState]);
}

void CTargetInTutorial::FirstTick()
{
	switch(m_eTargetAnimState)
	{
	case TAS_RETURN:
		{
			setCurrentHP(getMaxHP());
		}
		break;
	}
}

void CTargetInTutorial::OnUpdate( REAL32 tm)
{
	i3Object::OnUpdate(tm);

	if(m_rStateTime == 0.f)
	{
		FirstTick();
	}

	switch(m_eTargetAnimState)
	{
	case TAS_IDLE:
		{
			if(getCurrentHP() <= 0.f)
			{
				//SetTargetState(TAS_DOWN);
			}
		}
		break;
	case TAS_DOWN:
		{
			REAL32 rDuration = m_pAnim[ m_eTargetAnimState]->GetDuration();

			if( m_rStateTime + tm > rDuration )
			{
				SetTargetState(TAS_DOWN_IDLE);
			}
		}
		break;
	case TAS_DOWN_IDLE:
		{
			/*if(m_rStateTime > 5.f)
			{
				
				
			}*/
		}
		break;
	case TAS_RETURN:
		{
			REAL32 rDuration = m_pAnim[ m_eTargetAnimState]->GetDuration();

			if( m_rStateTime + tm > rDuration )
			{
				SetTargetState(TAS_IDLE);
			}
		}
		break;
	default:
		{
			I3ASSERT_0;
		}
		break;
	}

	m_rStateTime += tm;
}
