#include "pch.h"

#include "GameCharaDomiMutantRex.h"
#include "FullbodyStateDomiMutantRex.h"
#include "FullbodyBiStateMapFactory.h"

#include "../../Mode/Domination/Domination_Defines.h"

// 기획 문서 참조
// http://tiki.fps-pb.com/tiki-download_wiki_attachment.php?attId=3293

I3_CLASS_INSTANCE(CGameCharaDomiMutantRex);

void CGameCharaDomiMutantRex::Create(GLOBAL_CHARA_INFO* global, const CCharaInfo* db)
{
	CGameCharaFullbodyDinosaur::Create(global, db);
}

void CGameCharaDomiMutantRex::ResetChara()
{
	CGameCharaFullbodyDinosaur::ResetChara();
}

void CGameCharaDomiMutantRex::OnBindResource()
{
	CGameCharaFullbodyDinosaur::OnBindResource();

	INT32 count = fb_fsm::domi_mutantrex::GetAnimationCount();
	m_smgr.ReserveMemory(count);
	for (INT32 i = 0; i < count; i++)
	{
		INT32 id = m_ai->FindAIStateByName(s_fullbodyName[i]);
		if (id >= 0)
			m_smgr.AddState(fb_fsm::domi_mutantrex::CreateAnimation(id, this));
		else
		{
			I3ASSERT_0;
		}
	}
	m_smgr.SetBiStateMap(fb_fsm::BiStateMapFactory::CreateMapDomiMutantRex());
	m_smgr.SetName("DomiMutantRex");
	m_smgr.SetID(getCharaNetIndex());
	m_smgr.Transition(m_smgr.GetState(fb_fsm::domi_mutantrex::NONE));
}

void CGameCharaDomiMutantRex::OnUpdate(REAL32 tm)
{
	CGameCharaFullbodyDinosaur::OnUpdate(tm);

	//check code
	if( !isCharaStateMask( CHARA_STATE_DEATH))
	{
		I3ASSERT( domi_state_info::i()->boss_list.size() > 0);
	}
}

