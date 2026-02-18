#include "pch.h"

#include "GameCharaDomiAcidNotBomb.h"
#include "FullbodyStateDomiAcidNotBomb.h"
#include "FullbodyBiStateMapFactory.h"

// 기획 문서 참조
// http://tiki.fps-pb.com/tiki-download_wiki_attachment.php?attId=3293

I3_CLASS_INSTANCE(CGameCharaDomiAcidNotBomb);

void CGameCharaDomiAcidNotBomb::Create(GLOBAL_CHARA_INFO* global, const CCharaInfo* db)
{
	CGameCharaFullbodyDinosaur::Create(global, db);
}

void CGameCharaDomiAcidNotBomb::ResetChara()
{
	CGameCharaFullbodyDinosaur::ResetChara();
}

void CGameCharaDomiAcidNotBomb::OnBindResource()
{
	CGameCharaFullbodyDinosaur::OnBindResource();

	INT32 count = fb_fsm::domi_acid_not_bomb::GetAnimationCount();
	m_smgr.ReserveMemory(count);
	for (INT32 i=0; i<count; i++)
	{
		INT32 id = m_ai->FindAIStateByName(s_fullbodyName[i]);
		if (id >= 0)
			m_smgr.AddState(fb_fsm::domi_acid_not_bomb::CreateAnimation(id, this));
		else
		{
			I3ASSERT_0;
		}
	}
	m_smgr.SetBiStateMap(fb_fsm::BiStateMapFactory::CreateMapDomiAcidNotBomb());
	m_smgr.SetName("DomiAcidNotBomb");
	m_smgr.SetID(getCharaNetIndex());
	m_smgr.Transition(m_smgr.GetState(fb_fsm::domi_acid_not_bomb::NONE));
}

void CGameCharaDomiAcidNotBomb::OnUpdate(REAL32 tm)
{
	CGameCharaFullbodyDinosaur::OnUpdate(tm);
}
