#include "pch.h"

#include "GameCharaDomiRaptor.h"
#include "FullbodyStateDomiRaptor.h"
#include "FullbodyBiStateMapFactory.h"

// 기획 문서 참조
// http://tiki.fps-pb.com/tiki-download_wiki_attachment.php?attId=3293

I3_CLASS_INSTANCE(CGameCharaDomiRaptor);

void CGameCharaDomiRaptor::Create(GLOBAL_CHARA_INFO* global, const CCharaInfo* db)
{
	CGameCharaFullbodyDinosaur::Create(global, db);
#if defined( I3_DEBUG)
	SetName("CHARARES_DINO_DOMI_RAPTOR");
#endif
}

void CGameCharaDomiRaptor::ResetChara()
{
	CGameCharaFullbodyDinosaur::ResetChara();
}

void CGameCharaDomiRaptor::OnBindResource()
{
	CGameCharaFullbodyDinosaur::OnBindResource();

	INT32 count = fb_fsm::domi_raptor::GetAnimationCount();
	m_smgr.ReserveMemory(count);
	for (INT32 i=0; i<count; i++)
	{
		INT32 id = m_ai->FindAIStateByName(s_fullbodyName[i]);
		if (id >= 0)
			m_smgr.AddState(fb_fsm::domi_raptor::CreateAnimation(id, this));
		else
		{
			I3ASSERT_0;
		}
	}
	m_smgr.SetBiStateMap(fb_fsm::BiStateMapFactory::CreateMapDomiRaptor());
	m_smgr.SetName("DomiRaptor");
	m_smgr.SetID(getCharaNetIndex());
	m_smgr.Transition(m_smgr.GetState(fb_fsm::domi_raptor::NONE));
}

void CGameCharaDomiRaptor::OnUpdate(REAL32 tm)
{
	CGameCharaFullbodyDinosaur::OnUpdate(tm);
}
