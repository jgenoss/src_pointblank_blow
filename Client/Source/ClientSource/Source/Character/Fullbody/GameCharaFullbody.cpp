#include "pch.h"

#include "GameCharaFullbody.h"
#include "../GameCharaActionContext.h"

// 기획 문서 참조
// http://tiki.fps-pb.com/tiki-download_wiki_attachment.php?attId=3293

I3_CLASS_INSTANCE(CGameCharaFullbody);

const char* CGameCharaFullbody::s_fullbodyName[FULLBODY_AI_MAX] = 
{
	"A_0", "A_1", "A_2", "A_3", "A_4", "A_5", "A_6", "A_7", "A_8", "A_9",
	"B_0", "B_1", "B_2", "B_3", "B_4", "B_5", "B_6", "B_7", "B_8", "B_9",
	"C_0", "C_1", "C_2", "C_3", "C_4", "C_5", "C_6", "C_7", "C_8", "C_9",
	"D_0", "D_1", "D_2", "D_3", "D_4", "D_5", "D_6", "D_7", "D_8", "D_9",
};

CGameCharaFullbody::~CGameCharaFullbody()
{
	m_ai = nullptr;
}

void CGameCharaFullbody::Create(GLOBAL_CHARA_INFO* global, const CCharaInfo* db)
{
	CGameCharaBase::Create(global, db);
}

void CGameCharaFullbody::ResetChara()
{
}

void CGameCharaFullbody::OnBindResource()
{
	i3Chara::OnBindResource();

	m_ai = FindAI("FullBody");
}

void CGameCharaFullbody::OnUpdate(REAL32 tm)
{
	// CGameCharaBase::OnUpdate()는 풀바디에서 불필요한 컨텍스트들을 업데이트 하므로 호출하지 않는다.
	// 필요한 업데이트는 파생클래스에서 정의한다.
}

void CGameCharaFullbody::PlayFullbody(INT32 id)
{
	if (isCharaStateMask(CHARA_STATE_DEATH)) return;
	if (!m_ai) return;
	if ((id< 0) || (m_smgr.GetStateSize() <= id)) return;

	if (!m_ai->isEnable())
		m_ai->SetEnable(true);

	static_cast<fb_fsm::AnimationBase*>(m_smgr.GetState(id))->OnAnimStart();
	m_ai->setCurrentAIState(id);
}

void CGameCharaFullbody::OnFullbodyRun(INT32 id, REAL32 tm)
{
	REAL32 cur = m_ai->getLocalTime()+tm;
	REAL32 total =  m_ai->getCurrentAIState()->getActualDuration();
	if (cur > total)
	{
		static_cast<fb_fsm::AnimationBase*>(m_smgr.GetState(id))->OnAnimEnd();
	}

	static_cast<fb_fsm::AnimationBase*>(m_smgr.GetState(id))->OnAnimRun(tm);
}

const fb_fsm::StateMgr*	CGameCharaFullbody::GetFBStateMgr() const	{ return &m_smgr; }
fb_fsm::StateMgr*		CGameCharaFullbody::GetFBStateMgr()			{ return &m_smgr; }
i3AIContext* CGameCharaFullbody::GetAiContext() const { return m_ai; }
