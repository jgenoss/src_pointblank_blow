#include "pch.h"

#include "GameCharaDomiStingMars.h"
#include "FullbodyStateDomiStingMars.h"
#include "FullbodyBiStateMapFactory.h"

// 기획 문서 참조
// http://www.google.com/url?q=http://tiki.zepetto.biz/tiki-download_wiki_attachment.php?attId%3D3122&sa=D&usg=ALhdy28REQTYuwyghUiKLnWEpIAC9KwgAQ
// http://tiki.fps-pb.com/tiki-download_wiki_attachment.php?attId=3293

I3_CLASS_INSTANCE(CGameCharaDomiStingMars);

void CGameCharaDomiStingMars::Create(GLOBAL_CHARA_INFO* global, const CCharaInfo* db)
{
	CGameCharaDomiSting::Create(global, db);
}

void CGameCharaDomiStingMars::ResetChara()
{
	CGameCharaDomiSting::ResetChara();
}

void CGameCharaDomiStingMars::OnBindResource()
{
	CGameCharaFullbodyDinosaur::OnBindResource();

	INT32 count = fb_fsm::domi_sting_mars::GetAnimationCount();
	m_smgr.ReserveMemory(count);
	for (INT32 i=0; i<count; i++)
	{
		INT32 id = m_ai->FindAIStateByName(s_fullbodyName[i]);
		if (id >= 0)
		{
			m_smgr.AddState(fb_fsm::domi_sting_mars::CreateAnimation(id, this));
		}
		else
		{
			I3ASSERT_0;
		}
	}
	m_smgr.SetBiStateMap(fb_fsm::BiStateMapFactory::CreateMapDomiStingMars());
	m_smgr.SetName("DomiStingMars");
	m_smgr.SetID(getCharaNetIndex());
	m_smgr.Transition(m_smgr.GetState(fb_fsm::domi_sting_mars::NONE));
}

void CGameCharaDomiStingMars::OnUpdate(REAL32 tm)
{
	CGameCharaDomiSting::OnUpdate(tm);
}

