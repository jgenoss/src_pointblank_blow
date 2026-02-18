#include "pch.h"

#include "GameCharaDomiRaptorMercury.h"
#include "FullbodyStateDomiRaptorMercury.h"
#include "FullbodyBiStateMapFactory.h"

// 기획 문서 참조
// http://www.google.com/url?q=http://tiki.zepetto.biz/tiki-download_wiki_attachment.php?attId%3D3122&sa=D&usg=ALhdy28REQTYuwyghUiKLnWEpIAC9KwgAQ
// http://tiki.fps-pb.com/tiki-download_wiki_attachment.php?attId=3293

I3_CLASS_INSTANCE(CGameCharaDomiRaptorMercury);

void CGameCharaDomiRaptorMercury::Create(GLOBAL_CHARA_INFO* global, const CCharaInfo* db)
{
	CGameCharaFullbodyDinosaur::Create(global, db);
#if defined( I3_DEBUG)
	SetName("CHARARES_DINO_DOMI_RAPTOR_MERCURY");
#endif
}

void CGameCharaDomiRaptorMercury::ResetChara()
{
	CGameCharaFullbodyDinosaur::ResetChara();
}

void CGameCharaDomiRaptorMercury::OnBindResource()
{
	CGameCharaFullbodyDinosaur::OnBindResource();

	INT32 count = fb_fsm::domi_raptor_mercury::GetAnimationCount();
	m_smgr.ReserveMemory(count);
	for (INT32 i=0; i<count; i++)
	{
		INT32 id = m_ai->FindAIStateByName(s_fullbodyName[i]);
		if (id >= 0)
		{
			m_smgr.AddState(fb_fsm::domi_raptor_mercury::CreateAnimation(id, this));
		}
		else
		{
			I3ASSERT_0;
		}
	}

	m_smgr.SetBiStateMap(fb_fsm::BiStateMapFactory::CreateMapDomiRaptorMercury());
	m_smgr.SetName("DomiRaptorMercury");
	m_smgr.SetID(getCharaNetIndex());
	m_smgr.Transition(m_smgr.GetState(fb_fsm::domi_raptor_mercury::NONE));
}

void CGameCharaDomiRaptorMercury::OnUpdate(REAL32 tm)
{
	CGameCharaFullbodyDinosaur::OnUpdate(tm);
}
