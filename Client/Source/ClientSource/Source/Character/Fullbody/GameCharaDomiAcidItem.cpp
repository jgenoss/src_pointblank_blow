#include "pch.h"

#include "GameCharaDomiAcidItem.h"
#include "FullbodyStateDomiAcidItem.h"
#include "FullbodyBiStateMapFactory.h"

#include "../../ui/hud/UIHUD_Domination_Manager.h"

// 기획 문서 참조
// http://tiki.fps-pb.com/tiki-download_wiki_attachment.php?attId=3293

I3_CLASS_INSTANCE(CGameCharaDomiAcidItem);

void CGameCharaDomiAcidItem::Create(GLOBAL_CHARA_INFO* global, const CCharaInfo* db)
{
	CGameCharaFullbodyDinosaur::Create(global, db);
#if defined( I3_DEBUG)
	SetName("CHARARES_DINO_DOMI_ACID_ITEM");
#endif
}

void CGameCharaDomiAcidItem::ResetChara()
{
	CGameCharaFullbodyDinosaur::ResetChara();

	//I3ASSERT( m_pCharaInfo != nullptr);

}

void CGameCharaDomiAcidItem::OnBindResource()
{
	CGameCharaFullbodyDinosaur::OnBindResource();

	INT32 count = fb_fsm::domi_acid_item::GetAnimationCount();
	m_smgr.ReserveMemory(count);
	for (INT32 i=0; i<count; i++)
	{
		INT32 id = m_ai->FindAIStateByName(s_fullbodyName[i]);
		if (id >= 0)
			m_smgr.AddState(fb_fsm::domi_acid_item::CreateAnimation(id, this));
		else
		{
			I3ASSERT_0;
		}
	}

	m_smgr.SetBiStateMap(fb_fsm::BiStateMapFactory::CreateMapDomiAcidItem());
	m_smgr.SetName("DomiAcidItem");
	m_smgr.SetID(getCharaNetIndex());
	m_smgr.Transition(m_smgr.GetState(fb_fsm::domi_acid_item::NONE));
}

void CGameCharaDomiAcidItem::OnUpdate(REAL32 tm)
{
	CGameCharaFullbodyDinosaur::OnUpdate(tm);
}

